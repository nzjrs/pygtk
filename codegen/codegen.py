#!/usr/bin/env python
# -*- Mode: Python; py-indent-offset: 4 -*-
import sys, string
import getopt
import parser, argtypes, override

import traceback

# have to do return type processing
functmpl = 'static PyObject *\n' + \
	   '_wrap_%(cname)s(PyObject *self, PyObject *args, PyObject *kwargs)\n' + \
	   '{\n' + \
	   '%(varlist)s' + \
	   '    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "%(typecodes)s:%(name)s"%(parselist)s))\n' + \
	   '        return NULL;\n' + \
	   '%(extracode)s\n' + \
	   '%(handleret)s\n' + \
	   '}\n\n'
funccalltmpl = '%(cname)s(%(arglist)s)'

methtmpl = 'static PyObject *\n' + \
	   '_wrap_%(cname)s(PyGtk_Object *self, PyObject *args, PyObject *kwargs)\n' + \
	   '{\n' + \
	   '%(varlist)s' + \
	   '    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "%(typecodes)s:%(class)s.%(name)s"%(parselist)s))\n' + \
	   '        return NULL;\n' + \
	   '%(extracode)s\n' + \
	   '%(handleret)s\n' + \
	   '}\n\n'
methcalltmpl = '%(cname)s(%(cast)s(self->obj)%(arglist)s)'

consttmpl = 'static PyObject *\n' + \
	    '_wrap_%(cname)s(PyGtk_Object *self, PyObject *args, PyObject *kwargs)\n' + \
	    '{\n' + \
	    '%(varlist)s' + \
	    '    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "%(typecodes)s:%(class)s.__init__"%(parselist)s))\n' + \
	    '        return NULL;\n' + \
	    '%(extracode)s\n' + \
	    '    self->obj = (GtkObject *)%(cname)s(%(arglist)s);\n' + \
	    '    if (!self->obj) {\n' + \
	    '        PyErr_SetString(PyExc_RuntimeError, "could not create %(class)s object");\n' + \
	    '        return NULL;\n' + \
	    '    }\n' + \
	    '    pygtk_register_wrapper((PyObject *)self);\n' + \
	    '    Py_INCREF(Py_None);\n' + \
	    '    return Py_None;\n' + \
	    '}\n\n'

methdeftmpl = '    { "%(name)s", (PyCFunction)%(cname)s, %(flags)s },\n'

gettypetmpl = 'static PyObject *\n' + \
              '_wrap_%(uclass)s_get_type(PyObject *self, PyObject *args)\n' + \
              '{\n' + \
              '    if (!PyArg_ParseTuple(args, ":%(class)s.get_type"))\n' + \
              '        return NULL;\n' + \
              '    return PyInt_FromLong(%(uclass)s_get_type());\n' + \
              '}\n\n'
getattrtmpl = 'static PyObject *\n' + \
              '%(getattr)s(PyGtk_Object *self, char *attr)\n' + \
              '{\n' + \
              '%(attrchecks)s' + \
              '    return pygtk_getattr(self, attr);\n' + \
              '}\n\n'
attrchecktmpl = '    if (!strcmp(attr, "%(attr)s")) {\n' + \
                '%(varlist)s' + \
                '%(code)s\n' + \
                '    }\n'

typetmpl = 'PyExtensionClass Py%(class)s_Type = {\n' + \
	   '    PyObject_HEAD_INIT(NULL)\n' + \
	   '    0,				/* ob_size */\n' + \
	   '    "%(class)s",			/* tp_name */\n' + \
	   '    sizeof(PyGtk_Object),		/* tp_basicsize */\n' + \
	   '    0,				/* tp_itemsize */\n' + \
	   '    /* methods */\n' + \
	   '    (destructor)0,			/* tp_dealloc */\n' + \
	   '    (printfunc)0,			/* tp_print */\n' + \
	   '    (getattrfunc)%(getattr)s,	/* tp_getattr */\n' + \
	   '    (setattrfunc)0,			/* tp_setattr */\n' + \
	   '    (cmpfunc)0,			/* tp_compare */\n' + \
	   '    (reprfunc)0,			/* tp_repr */\n' + \
	   '    0,				/* tp_as_number */\n' + \
	   '    0,				/* tp_as_sequence */\n' + \
	   '    0,				/* tp_as_mapping */\n' + \
	   '    (hashfunc)0,			/* tp_hash */\n' + \
           '    (ternaryfunc)0,			/* tp_call */\n' + \
           '    (reprfunc)0,			/* tp_str */\n' + \
	   '    (getattrofunc)0,		/* tp_getattro */\n' + \
	   '    (setattrofunc)0,		/* tp_setattro */\n' + \
	   '    /* Space for future expansion */\n' + \
	   '    0L, 0L,\n' + \
	   '    NULL, /* Documentation string */\n' + \
	   '    %(methods)s,\n' + \
           '    EXTENSIONCLASS_INSTDICT_FLAG,\n' + \
	   '};\n\n'

def write_function(funcobj, fp=sys.stdout):
    varlist = argtypes.VarList()
    parsestr = ''
    parselist = ['']
    extracode = []
    arglist = []

    if funcobj.varargs:
        raise ValueError, "varargs functions not supported"
    
    dict = {
	'name':    funcobj.name,
	'cname':   funcobj.c_name,
	'varlist': varlist,
    }

    # create the keyword argument name list ...
    kwlist = string.join(map(lambda x: '"'+x[1]+'"', funcobj.params) +
                         ['NULL'], ', ')
    varlist.add('static char', '*kwlist[] = { ' + kwlist + ' }')
    parselist.append('kwlist')

    for ptype, pname, pdflt, pnull in funcobj.params:
	if pdflt and '|' not in parsestr:
	    parsestr = parsestr + '|'
	handler = argtypes.matcher.get(ptype)
	parsestr = parsestr + handler.write_param(ptype, pname, pdflt, pnull,
						  varlist, parselist,
						  extracode, arglist)
    dict['typecodes'] = parsestr
    dict['parselist'] = string.join(parselist, ', ')
    dict['extracode'] = string.join(extracode, '')
    dict['arglist']   = string.join(arglist, ', ')

    call = funccalltmpl % dict
    handler = argtypes.matcher.get(funcobj.ret)
    dict['handleret'] = handler.write_return(funcobj.ret, varlist) % {'func': call}

    fp.write(functmpl % dict)

def write_method(objname, methobj, fp=sys.stdout):
    varlist = argtypes.VarList()
    parsestr = ''
    parselist = ['']
    extracode = []
    arglist = ['']

    if methobj.varargs:
        raise ValueError, "varargs methods not supported"
    
    dict = {
	'name':    methobj.name,
	'cname':   methobj.c_name,
	'varlist': varlist,
	'class':   objname,
	'cast':    argtypes._to_upper_str(objname)[1:]
    }

    # create the keyword argument name list ...
    kwlist = string.join(map(lambda x: '"'+x[1]+'"', methobj.params) +
                         ['NULL'], ', ')
    varlist.add('static char', '*kwlist[] = { ' + kwlist + ' }')
    parselist.append('kwlist')

    # handle return type ...
    for ptype, pname, pdflt, pnull in methobj.params:
	if pdflt and '|' not in parsestr:
	    parsestr = parsestr + '|'
	handler = argtypes.matcher.get(ptype)
	parsestr = parsestr + handler.write_param(ptype, pname, pdflt, pnull,
						  varlist, parselist,
						  extracode, arglist)
    dict['typecodes'] = parsestr
    dict['parselist'] = string.join(parselist, ', ')
    dict['extracode'] = string.join(extracode, '')
    dict['arglist']   = string.join(arglist, ', ')

    call = methcalltmpl % dict
    handler = argtypes.matcher.get(methobj.ret)
    dict['handleret'] = handler.write_return(methobj.ret, varlist) % {'func': call}

    fp.write(methtmpl % dict)

def write_constructor(objname, funcobj, fp=sys.stdout):
    varlist = argtypes.VarList()
    parsestr = ''
    parselist = ['']
    extracode = []
    arglist = []

    dict = {
	'name':    funcobj.name,
	'cname':   funcobj.c_name,
	'varlist': varlist,
	'class':   objname,
	'cast':    argtypes._to_upper_str(objname)[1:]
    }

    # create the keyword argument name list ...
    kwlist = string.join(map(lambda x: '"'+x[1]+'"', funcobj.params) +
                         ['NULL'], ', ')
    varlist.add('static char', '*kwlist[] = { ' + kwlist + ' }')
    parselist.append('kwlist')

    # handle return type ...
    for ptype, pname, pdflt, pnull in funcobj.params:
	if pdflt and '|' not in parsestr:
	    parsestr = parsestr + '|'
	handler = argtypes.matcher.get(ptype)
	parsestr = parsestr + handler.write_param(ptype, pname, pdflt, pnull,
						  varlist, parselist,
						  extracode, arglist)
    dict['typecodes'] = parsestr
    dict['parselist'] = string.join(parselist, ', ')
    dict['extracode'] = string.join(extracode, '')
    dict['arglist']   = string.join(arglist, ', ')
    fp.write(consttmpl % dict)

def write_getattr(parser, objobj, fp=sys.stdout):
    uline = argtypes._to_upper_str(objobj.c_name)[1:]
    attrchecks = ''
    for ftype, fname in objobj.fields:
        try:
            varlist = argtypes.VarList()
            handler = argtypes.matcher.get(ftype)
            code = handler.write_return(ftype, varlist) % \
                   {'func': uline + '(self->obj)->' + fname}
            if code:
                # indent code ...
                code = '    ' + string.replace(code, '\n', '\n    ')
            attrchecks = attrchecks + attrchecktmpl % { 'attr': fname,
                                                        'varlist': varlist,
                                                        'code': code }
        except:
            sys.stderr.write("couldn't write check for " + objobj.c_name +
                             '.' + fname + '\n')
	    #traceback.print_exc()
    funcname = '_wrap_' + string.lower(uline) + '_getattr'
    fp.write(getattrtmpl % {'getattr':    funcname,
                            'attrchecks': attrchecks })
    return funcname

def write_class(parser, objobj, overrides, fp=sys.stdout):
    fp.write('\n/* ----------- ' + objobj.c_name + ' ----------- */\n\n')
    constructor = parser.find_constructor(objobj, overrides)
    methods = []
    if constructor:
	try:
            methtype = 'METH_VARARGS'
            if overrides.is_overriden(constructor.c_name):
                fp.write(overrides.override(constructor.c_name))
                fp.write('\n\n')
                if overrides.wants_kwargs(constructor.c_name):
                    methtype = methtype + '|METH_KEYWORDS'
            else:
                write_constructor(objobj.c_name, constructor, fp)
                methtype = methtype + '|METH_KEYWORDS'
	    methods.append(methdeftmpl %
			   { 'name':  '__init__',
			     'cname': '_wrap_' + constructor.c_name,
			     'flags': methtype})
	except:
	    sys.stderr.write('Could not write constructor for ' +
			     objobj.c_name + '\n')
	    #traceback.print_exc()
            methods.append(methdeftmpl %
                           { 'name':  '__init__',
                             'cname': 'pygtk_no_constructor',
                             'flags': 'METH_VARARGS'})
    else:
        methods.append(methdeftmpl %
                       { 'name':  '__init__',
                         'cname': 'pygtk_no_constructor',
                         'flags': 'METH_VARARGS'})
    # do the get_type routine as class method ...
    uclass = string.lower(argtypes._to_upper_str(objobj.c_name)[1:])
    fp.write(gettypetmpl % { 'class':  objobj.c_name,
                             'uclass': uclass })
    methods.append(methdeftmpl %
                   { 'name':  'get_type',
                     'cname': '_wrap_' + uclass + '_get_type',
                     'flags': 'METH_VARARGS|METH_CLASS_METHOD'})
    for meth in parser.find_methods(objobj):
        if overrides.is_ignored(meth.c_name):
            continue
	try:
            methtype = 'METH_VARARGS'
            if overrides.is_overriden(meth.c_name):
                fp.write(overrides.override(meth.c_name))
                fp.write('\n\n')
                if overrides.wants_kwargs(meth.c_name):
                    methtype = methtype + '|METH_KEYWORDS'
            else:
                write_method(objobj.c_name, meth, fp)
                methtype = methtype + '|METH_KEYWORDS'
	    methods.append(methdeftmpl % { 'name':  meth.name,
					   'cname': '_wrap_' + meth.c_name,
					   'flags': methtype})
	except:
	    sys.stderr.write('Could not write method ' + objobj.c_name +
			     '.' + meth.name + '\n')
            #traceback.print_exc()

    # write the PyMethodDef structure
    methods.append('    { NULL, NULL, 0 }\n')
    fp.write('static PyMethodDef _Py' + objobj.c_name + '_methods[] = {\n')
    fp.write(string.join(methods, ''))
    fp.write('};\n\n')

    # write the type template
    dict = { 'class': objobj.c_name }
    if objobj.fields:
        dict['getattr'] = write_getattr(parser, objobj, fp)
    else:
        dict['getattr'] = '0'
    if objobj.c_name == 'GtkObject':
        dict['methods'] = '{ _PyGtkObject_methods, &base_object_method_chain }'
    else:
        dict['methods'] = 'METHOD_CHAIN(_Py' + dict['class'] + '_methods)'
    fp.write(typetmpl % dict)

def write_functions(parser, overrides, prefix, fp=sys.stdout):
    fp.write('\n/* ----------- functions ----------- */\n\n')
    functions = []
    for func in parser.find_functions():
        if overrides.is_ignored(func.c_name):
            continue
	try:
            methtype = 'METH_VARARGS'
            if overrides.is_overriden(func.c_name):
                fp.write(overrides.override(func.c_name))
                fp.write('\n\n')
                if overrides.wants_kwargs(func.c_name):
                    methtype = methtype + '|METH_KEYWORDS'
            else:
                write_function(func, fp)
                methtype = methtype + '|METH_KEYWORDS'
	    functions.append(methdeftmpl % { 'name':  func.name,
                                             'cname': '_wrap_' + func.c_name,
                                             'flags': methtype})
	except:
	    sys.stderr.write('Could not write function ' + func.name + '\n')
	    #traceback.print_exc()
    # write the PyMethodDef structure
    functions.append('    { NULL, NULL, 0 }\n')
    fp.write('PyMethodDef ' + prefix + '_functions[] = {\n')
    fp.write(string.join(functions, ''))
    fp.write('};\n\n')


def write_source(parser, overrides, prefix, fp=sys.stdout):
    fp.write('/* -*- Mode: C; c-basic-offset: 4 -*- */\n\n')
    fp.write('#include <Python.h>\n#include <ExtensionClass.h>\n\n')
    fp.write(overrides.get_headers())
    fp.write('\n\n')
    fp.write('/* ---------- forward type declarations ---------- */\n')
    for obj in parser.objects:
        fp.write('PyExtensionClass Py' + obj.c_name + '_Type;\n')
    fp.write('\n')
    for obj in parser.objects:
        write_class(parser, obj, overrides, fp)
        fp.write('\n')

    write_functions(parser, overrides, prefix, fp)

    fp.write('/* intialise stuff extension classes */\n')
    fp.write('void\n' + prefix + '_register_classes(PyObject *d)\n{\n')
    fp.write('    ExtensionClassImported;\n')
    fp.write(overrides.get_init() + '\n')
    for obj in parser.objects:
        if obj.parent != (None, None):
            fp.write('    pygtk_register_class(d, "' + obj.c_name + '", &Py' +
                     obj.c_name + '_Type, &Py' + obj.parent[1] +
                     obj.parent[0] + '_Type);\n')
        else:
            fp.write('    pygtk_register_class(d, "' + obj.c_name +
                     '", &Py' + obj.c_name + '_Type, NULL);\n')
    fp.write('}\n')

def register_types(parser):
    for obj in parser.objects:
	argtypes.matcher.register_object(obj.c_name)
    for enum in parser.enums:
	if enum.deftype == 'flags':
	    argtypes.matcher.register_flag(enum.c_name)
	else:
	    argtypes.matcher.register_enum(enum.c_name)

if __name__ == '__main__':
    o = override.Overrides(None)
    prefix = 'pygtk'
    opts, args = getopt.getopt(sys.argv[1:], "o:p:r:",
                               ["override=", "prefix=", "register="])
    for opt, arg in opts:
        if opt in ('-o', '--override'):
            o = override.Overrides(open(arg))
        elif opt in ('-p', '--prefix'):
            prefix = arg
        elif opt in ('-r', '--register'):
            p = parser.DefsParser(arg)
            p.startParsing()
            register_types(p)
            del p
    if len(args) < 1:
        sys.stderr.write(
            'usage: codegen.py [-o overridesfile] [-p prefix] defsfile\n')
        sys.exit(1)
    p = parser.DefsParser(args[0])
    p.startParsing()
    register_types(p)
    write_source(p, o, prefix)
