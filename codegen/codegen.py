#!/usr/bin/env python
# -*- Mode: Python; py-indent-offset: 4 -*-
import sys, string
import getopt
import defsparser, argtypes, override

import traceback, keyword

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
	   '_wrap_%(cname)s(PyGObject *self, PyObject *args, PyObject *kwargs)\n' + \
	   '{\n' + \
	   '%(varlist)s' + \
	   '    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "%(typecodes)s:%(class)s.%(name)s"%(parselist)s))\n' + \
	   '        return NULL;\n' + \
	   '%(extracode)s\n' + \
	   '%(handleret)s\n' + \
	   '}\n\n'
methcalltmpl = '%(cname)s(%(cast)s(self->obj)%(arglist)s)'

consttmpl = 'static PyObject *\n' + \
	    '_wrap_%(cname)s(PyGObject *self, PyObject *args, PyObject *kwargs)\n' + \
	    '{\n' + \
	    '%(varlist)s' + \
	    '    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "%(typecodes)s:%(class)s.__init__"%(parselist)s))\n' + \
	    '        return NULL;\n' + \
	    '%(extracode)s\n' + \
	    '    self->obj = (GObject *)%(cname)s(%(arglist)s);\n' + \
	    '    if (!self->obj) {\n' + \
	    '        PyErr_SetString(PyExc_RuntimeError, "could not create %(class)s object");\n' + \
	    '        return NULL;\n' + \
	    '    }\n' + \
            '%(gtkobjectsink)s' + \
	    '    pygobject_register_wrapper((PyObject *)self);\n' + \
	    '    Py_INCREF(Py_None);\n' + \
	    '    return Py_None;\n' + \
	    '}\n\n'

methdeftmpl = '    { "%(name)s", (PyCFunction)%(cname)s, %(flags)s },\n'

getattrtmpl = 'static PyObject *\n' + \
              '%(getattr)s(PyGObject *self, char *attr)\n' + \
              '{\n' + \
              '%(attrchecks)s' + \
              '    PyErr_SetString(PyExc_AttributeError, attr);\n' + \
              '    return NULL;\n' + \
              '}\n\n'
attrchecktmpl = '    if (!strcmp(attr, "%(attr)s")) {\n' + \
                '%(varlist)s' + \
                '%(code)s\n' + \
                '    }\n'

noconstructor = 'static PyObject *\n' + \
                'pygobject_no_constructor(PyObject *self, PyObject *args)\n' +\
                '{\n' + \
                '    gchar buf[512];\n' + \
                '\n' + \
                '    g_snprintf(buf, sizeof(buf), "%s is an abstract widget", self->ob_type->tp_name);\n' + \
                '    PyErr_SetString(PyExc_NotImplementedError, buf);\n' + \
                '    return NULL;\n' + \
                '}\n\n'

typetmpl = 'PyExtensionClass Py%(class)s_Type = {\n' + \
	   '    PyObject_HEAD_INIT(NULL)\n' + \
	   '    0,				/* ob_size */\n' + \
	   '    "%(classname)s",		/* tp_name */\n' + \
	   '    sizeof(PyGObject),		/* tp_basicsize */\n' + \
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

interfacetypetmpl = 'PyExtensionClass Py%(class)s_Type = {\n' + \
	   '    PyObject_HEAD_INIT(NULL)\n' + \
	   '    0,				/* ob_size */\n' + \
	   '    "%(classname)s",		/* tp_name */\n' + \
	   '    sizeof(PyPureMixinObject),	/* tp_basicsize */\n' + \
	   '    0,				/* tp_itemsize */\n' + \
	   '    /* methods */\n' + \
	   '    (destructor)0,			/* tp_dealloc */\n' + \
	   '    (printfunc)0,			/* tp_print */\n' + \
	   '    (getattrfunc)0,			/* tp_getattr */\n' + \
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
           '    0,\n' + \
	   '};\n\n'

boxedmethtmpl = 'static PyObject *\n' + \
                '_wrap_%(cname)s(PyObject *self, PyObject *args, PyObject *kwargs)\n' + \
                '{\n' + \
                '%(varlist)s' + \
                '    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "%(typecodes)s:%(typename)s.%(name)s"%(parselist)s))\n' + \
                '        return NULL;\n' + \
                '%(extracode)s\n' + \
                '%(handleret)s\n' + \
                '}\n\n'
boxedmethcalltmpl = '%(cname)s(pyg_boxed_get(self, %(typename)s)%(arglist)s)'

boxedconsttmpl = 'static PyObject *\n' + \
                 '_wrap_%(cname)s(PyGBoxed *self, PyObject *args, PyObject *kwargs)\n' + \
                 '{\n' + \
                 '%(varlist)s' + \
                 '    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "%(typecodes)s:%(typename)s.__init__"%(parselist)s))\n' + \
                 '        return NULL;\n' + \
                 '%(extracode)s\n' + \
                 '    self->gtype = %(typecode)s;\n' + \
                 '    self->free_on_dealloc = FALSE;\n' + \
                 '    self->boxed = %(cname)s(%(arglist)s);\n' + \
                 '    if (!self->boxed) {\n' + \
                 '        PyErr_SetString(PyExc_RuntimeError, "could not create %(typename)s object");\n' + \
                 '        return NULL;\n' + \
                 '    }\n' + \
                 '    self->free_on_dealloc = TRUE;\n' + \
                 '    Py_INCREF(Py_None);\n' + \
                 '    return Py_None;\n' + \
                 '}\n\n'

boxedgetattrtmpl = 'static PyObject *\n' + \
                   '%(getattr)s(PyObject *self, char *attr)\n' + \
                   '{\n' + \
                   '%(attrchecks)s' + \
                   '    PyErr_SetString(PyExc_AttributeError, attr);\n' + \
                   '    return NULL;\n' + \
                   '}\n\n'

boxedtmpl = 'PyExtensionClass Py%(typename)s_Type = {\n' + \
            '    PyObject_HEAD_INIT(NULL)\n' + \
            '    0,				/* ob_size */\n' + \
            '    "%(typename)s",			/* tp_name */\n' + \
            '    sizeof(PyGBoxed),		/* tp_basicsize */\n' + \
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
            '    0,\n' + \
            '};\n\n'

def fixname(name):
    if keyword.iskeyword(name):
	return name + '_'
    return name

def write_function(funcobj, fp=sys.stdout):
    varlist = argtypes.VarList()
    parsestr = ''
    parselist = ['']
    extracode = []
    arglist = []

    if funcobj.varargs:
        raise ValueError, "varargs functions not supported"
    
    dict = {
	'name':    fixname(funcobj.name),
	'cname':   funcobj.c_name,
	'varlist': varlist,
    }

    # create the keyword argument name list ...
    kwlist = string.join(map(lambda x: '"'+fixname(x[1])+'"', funcobj.params) +
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

def write_method(objname, castmacro, methobj, fp=sys.stdout):
    varlist = argtypes.VarList()
    parsestr = ''
    parselist = ['']
    extracode = []
    arglist = ['']

    if methobj.varargs:
        raise ValueError, "varargs methods not supported"
    
    dict = {
	'name':    fixname(methobj.name),
	'cname':   methobj.c_name,
	'varlist': varlist,
	'class':   objname,
	'cast':    castmacro
    }

    # create the keyword argument name list ...
    kwlist = string.join(map(lambda x: '"'+fixname(x[1])+'"', methobj.params) +
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

def write_constructor(objname, castmacro, funcobj, fp=sys.stdout):
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
	'cast':    castmacro,
	'gtkobjectsink': ''
    }

    if argtypes.matcher.object_is_a(objname, 'GtkObject'):
        dict['gtkobjectsink'] = \
			'    gtk_object_ref(GTK_OBJECT(self->obj));\n' + \
			'    gtk_object_sink(GTK_OBJECT(self->obj));\n'
    # create the keyword argument name list ...
    kwlist = string.join(map(lambda x: '"'+fixname(x[1])+'"', funcobj.params) +
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

def write_getattr(parser, objobj, castmacro, overrides, fp=sys.stdout):
    funcname = '_wrap_' + string.lower(castmacro) + '_getattr'

    if overrides.is_overriden(funcname[6:]):
        fp.write(overrides.override(funcname[6:]))
        fp.write('\n\n')
        return funcname
    
    # no overrides for the whole function.  If no fields, don't write a func
    if not objobj.fields:
        return '0'
    attrchecks = ''
    for ftype, fname in objobj.fields:
        attrname = objobj.c_name + '.' + fname
        if overrides.attr_is_overriden(attrname):
            code = overrides.attr_override(attrname)
            code = '        ' + string.replace(code, '\n', '\n        ')
            attrchecks = attrchecks + attrchecktmpl % { 'attr': fixname(fname),
                                                        'varlist': '',
                                                        'code': code }
            continue
        try:
            varlist = argtypes.VarList()
            handler = argtypes.matcher.get(ftype)
            code = handler.write_return(ftype, varlist) % \
                   {'func': castmacro + '(self->obj)->' + fname}
            if code:
                # indent code ...
                code = '    ' + string.replace(code, '\n', '\n    ')
            attrchecks = attrchecks + attrchecktmpl % { 'attr': fixname(fname),
                                                        'varlist': varlist,
                                                        'code': code }
        except:
            sys.stderr.write("couldn't write check for " + objobj.c_name +
                             '.' + fname + '\n')
	    #traceback.print_exc()
    fp.write(getattrtmpl % {'getattr':    funcname,
                            'attrchecks': attrchecks })
    return funcname

def write_class(parser, objobj, overrides, fp=sys.stdout):
    fp.write('\n/* ----------- ' + objobj.c_name + ' ----------- */\n\n')
    constructor = parser.find_constructor(objobj, overrides)
    methods = []
    castmacro = string.replace(objobj.typecode, '_TYPE_', '_', 1)
    if constructor:
	try:
            methtype = 'METH_VARARGS'
            if overrides.is_overriden(constructor.c_name):
                fp.write(overrides.override(constructor.c_name))
                fp.write('\n\n')
                if overrides.wants_kwargs(constructor.c_name):
                    methtype = methtype + '|METH_KEYWORDS'
            else:
                write_constructor(objobj.c_name, castmacro, constructor, fp)
                methtype = methtype + '|METH_KEYWORDS'
	    methods.append(methdeftmpl %
			   { 'name':  '__init__',
			     'cname': '_wrap_' + constructor.c_name,
			     'flags': methtype})
	except:
	    sys.stderr.write('Could not write constructor for ' +
			     objobj.c_name + '\n')
	    #traceback.print_exc()
            # this is a hack ...
            if not hasattr(overrides, 'no_constructor_written'):
                fp.write(noconstructor)
                overrides.no_constructor_written = 1
            methods.append(methdeftmpl %
                           { 'name':  '__init__',
                             'cname': 'pygobject_no_constructor',
                             'flags': 'METH_VARARGS'})
    else:
        # this is a hack ...
        if not hasattr(overrides, 'no_constructor_written'):
            fp.write(noconstructor)
            overrides.no_constructor_written = 1
        methods.append(methdeftmpl %
                       { 'name':  '__init__',
                         'cname': 'pygobject_no_constructor',
                         'flags': 'METH_VARARGS'})
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
                write_method(objobj.c_name, castmacro, meth, fp)
                methtype = methtype + '|METH_KEYWORDS'
	    methods.append(methdeftmpl % { 'name':  fixname(meth.name),
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
    dict = { 'class': objobj.c_name, 'classname': objobj.name }
    dict['getattr'] = write_getattr(parser, objobj, castmacro, overrides, fp)
    dict['methods'] = 'METHOD_CHAIN(_Py' + dict['class'] + '_methods)'
    fp.write(typetmpl % dict)

def write_interface(parser, interface, overrides, fp=sys.stdout):
    fp.write('\n/* ----------- ' + interface.c_name + ' ----------- */\n\n')
    methods = []
    castmacro = string.replace(interface.typecode, '_TYPE_', '_', 1)
    for meth in parser.find_methods(interface):
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
                write_method(interface.c_name, castmacro, meth, fp)
                methtype = methtype + '|METH_KEYWORDS'
	    methods.append(methdeftmpl % { 'name':  fixname(meth.name),
					   'cname': '_wrap_' + meth.c_name,
					   'flags': methtype})
	except:
	    sys.stderr.write('Could not write method ' + interface.c_name +
			     '.' + meth.name + '\n')
            #traceback.print_exc()

    # write the PyMethodDef structure
    methods.append('    { NULL, NULL, 0 }\n')
    fp.write('static PyMethodDef _Py' + interface.c_name + '_methods[] = {\n')
    fp.write(string.join(methods, ''))
    fp.write('};\n\n')

    # write the type template
    dict = { 'class': interface.c_name, 'classname': interface.name,
             'getattr': '0' }
    dict['methods'] = 'METHOD_CHAIN(_Py' + dict['class'] + '_methods)'
    fp.write(interfacetypetmpl % dict)

## boxed types ...
def write_boxed_method(objname, methobj, fp=sys.stdout):
    varlist = argtypes.VarList()
    parsestr = ''
    parselist = ['']
    extracode = []
    arglist = ['']

    if methobj.varargs:
        raise ValueError, "varargs methods not supported"
    
    dict = {
	'name':     fixname(methobj.name),
	'cname':    methobj.c_name,
	'varlist':  varlist,
	'typename': objname,
    }

    # create the keyword argument name list ...
    kwlist = string.join(map(lambda x: '"'+fixname(x[1])+'"', methobj.params) +
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

    call = boxedmethcalltmpl % dict
    handler = argtypes.matcher.get(methobj.ret)
    dict['handleret'] = handler.write_return(methobj.ret, varlist) % {'func': call}

    fp.write(boxedmethtmpl % dict)

def write_boxed_constructor(objname, typecode, funcobj, fp=sys.stdout):
    varlist = argtypes.VarList()
    parsestr = ''
    parselist = ['']
    extracode = []
    arglist = []

    dict = {
	'name':    funcobj.name,
	'cname':   funcobj.c_name,
	'varlist': varlist,
	'typename': objname,
        'typecode': typecode,
    }

    # create the keyword argument name list ...
    kwlist = string.join(map(lambda x: '"'+fixname(x[1])+'"', funcobj.params) +
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
    fp.write(boxedconsttmpl % dict)

def write_boxed_getattr(parser, boxedobj, overrides, fp=sys.stdout):
    typecode = boxedobj.typecode
    uline = string.replace(typecode, '_TYPE_', '_', 1)
    funcname = '_wrap_' + string.lower(uline) + '_getattr'

    if overrides.is_overriden(funcname[6:]):
        fp.write(overrides.override(funcname[6:]))
        fp.write('\n\n')
        return funcname

    # no overrides for the whole function.  If no fields, don't write a func
    if not boxedobj.fields:
        return '0'
    attrchecks = ''
    for ftype, fname in boxedobj.fields:
        attrname = boxedobj.c_name + '.' + fname
        if overrides.attr_is_overriden(attrname):
            code = overrides.attr_override(attrname)
            code = '        ' + string.replace(code, '\n', '\n        ')
            attrchecks = attrchecks + attrchecktmpl % { 'attr': fixname(fname),
                                                        'varlist': '',
                                                        'code': code }
            continue
        try:
            varlist = argtypes.VarList()
            handler = argtypes.matcher.get(ftype)
            code = handler.write_return(ftype, varlist) % \
                   {'func': 'pyg_boxed_get(self, ' + boxedobj.c_name + ')->' + fname}
            if code:
                # indent code ...
                code = '    ' + string.replace(code, '\n', '\n    ')
            attrchecks = attrchecks + attrchecktmpl % { 'attr': fixname(fname),
                                                        'varlist': varlist,
                                                        'code': code }
        except:
            sys.stderr.write("couldn't write check for " + boxedobj.c_name +
                             '.' + fname + '\n')
	    #traceback.print_exc()
    funcname = '_wrap_' + string.lower(uline) + '_getattr'
    fp.write(boxedgetattrtmpl % {'getattr':    funcname,
                                 'attrchecks': attrchecks })
    return funcname

def write_boxed(parser, boxedobj, overrides, fp=sys.stdout):
    fp.write('\n/* ----------- ' + boxedobj.c_name + ' ----------- */\n\n')
    constructor = parser.find_constructor(boxedobj, overrides)
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
                write_boxed_constructor(boxedobj.c_name, boxedobj.typecode,
                                        constructor, fp)
                methtype = methtype + '|METH_KEYWORDS'
	    methods.append(methdeftmpl %
			   { 'name':  '__init__',
			     'cname': '_wrap_' + constructor.c_name,
			     'flags': methtype})
	except:
	    sys.stderr.write('Could not write constructor for ' +
			     boxedobj.c_name + '\n')
	    #traceback.print_exc()
    for meth in parser.find_methods(boxedobj):
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
                write_boxed_method(boxedobj.c_name, meth, fp)
                methtype = methtype + '|METH_KEYWORDS'
	    methods.append(methdeftmpl % { 'name':  fixname(meth.name),
					   'cname': '_wrap_' + meth.c_name,
					   'flags': methtype})
	except:
	    sys.stderr.write('Could not write method ' + boxedobj.c_name +
			     '.' + meth.name + '\n')
            #traceback.print_exc()

    # write the PyMethodDef structure
    methods.append('    { NULL, NULL, 0 }\n')
    fp.write('static PyMethodDef _Py' + boxedobj.c_name + '_methods[] = {\n')
    fp.write(string.join(methods, ''))
    fp.write('};\n\n')

    # write the type template
    dict = { 'typename': boxedobj.c_name }
    dict['getattr'] = write_boxed_getattr(parser, boxedobj, overrides, fp)
    dict['methods'] = 'METHOD_CHAIN(_Py' + dict['typename'] + '_methods)'
    fp.write(boxedtmpl % dict)


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
    for obj in parser.boxes:
        fp.write('PyExtensionClass Py' + obj.c_name + '_Type;\n')
    for obj in parser.objects:
        fp.write('PyExtensionClass Py' + obj.c_name + '_Type;\n')
    for interface in parser.interfaces:
        fp.write('PyExtensionClass Py' + interface.c_name + '_Type;\n')
    fp.write('\n')
    for boxed in parser.boxes:
        write_boxed(parser, boxed, overrides, fp)
        fp.write('\n')
    for obj in parser.objects:
        write_class(parser, obj, overrides, fp)
        fp.write('\n')
    for interface in parser.interfaces:
        write_interface(parser, interface, overrides, fp)
        fp.write('\n')

    write_functions(parser, overrides, prefix, fp)

    fp.write('/* intialise stuff extension classes */\n')
    fp.write('void\n' + prefix + '_register_classes(PyObject *d)\n{\n')
    fp.write('    ExtensionClassImported;\n')
    fp.write(overrides.get_init() + '\n')

    for boxed in parser.boxes:
        fp.write('    pyg_register_boxed(d, "' + boxed.name +
                 '", ' + boxed.typecode + ', &Py' + boxed.c_name + '_Type);\n')
    for interface in parser.interfaces:
        uclass = string.lower(string.replace(interface.typecode, '_TYPE_', '_', 1))
        fp.write('    pyg_register_interface(d, "' + interface.name +
                 '", '+ uclass + '_get_type, &Py' + interface.c_name +
                 '_Type);\n')
    objects = parser.objects[:]
    pos = 0
    while pos < len(objects):
        parent = objects[pos].parent
        for i in range(pos+1, len(objects)):
            if objects[i].c_name == parent:
                objects.insert(i+1, objects[pos])
                del objects[pos]
                break
        else:
            pos = pos + 1
    for obj in objects:
        bases = []
        if obj.parent != None:
            bases.append(obj.parent)
        bases = bases + obj.implements
        uclass = string.lower(string.replace(obj.typecode, '_TYPE_', '_', 1))
        if bases:
            fp.write('    pygobject_register_class(d, "' + obj.c_name +
                     '", ' + uclass + '_get_type, &Py' + obj.c_name +
                     '_Type, Py_BuildValue("(' + 'O' * len(bases) + ')", ' +
                     string.join(map(lambda s: '&Py'+s+'_Type', bases), ', ') +
                     '));\n')
        else:
            fp.write('    pygobject_register_class(d, "' + obj.c_name +
                     '", ' + uclass + '_get_type, &Py' + obj.c_name +
                     '_Type, NULL);\n')
    fp.write('}\n')

def register_types(parser):
    for boxed in parser.boxes:
        argtypes.matcher.register_boxed(boxed.c_name, boxed.typecode)
    for obj in parser.objects:
        argtypes.matcher.register_object(obj.c_name, obj.parent, obj.typecode)
    for obj in parser.interfaces:
        argtypes.matcher.register_object(obj.c_name, None, obj.typecode)
    for enum in parser.enums:
	if enum.deftype == 'flags':
	    argtypes.matcher.register_flag(enum.c_name, enum.typecode)
	else:
	    argtypes.matcher.register_enum(enum.c_name, enum.typecode)

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
            p = defsparser.DefsParser(arg)
            p.startParsing()
            register_types(p)
            del p
    if len(args) < 1:
        sys.stderr.write(
            'usage: codegen.py [-o overridesfile] [-p prefix] defsfile\n')
        sys.exit(1)
    p = defsparser.DefsParser(args[0])
    p.startParsing()
    register_types(p)
    write_source(p, o, prefix)
