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

consttmpl = 'static int\n' + \
	    '_wrap_%(cname)s(PyGObject *self, PyObject *args, PyObject *kwargs)\n' + \
	    '{\n' + \
	    '%(varlist)s' + \
	    '    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "%(typecodes)s:%(class)s.__init__"%(parselist)s))\n' + \
	    '        return -1;\n' + \
	    '%(extracode)s\n' + \
	    '    self->obj = (GObject *)%(cname)s(%(arglist)s);\n' + \
	    '    if (!self->obj) {\n' + \
	    '        PyErr_SetString(PyExc_RuntimeError, "could not create %(class)s object");\n' + \
	    '        return -1;\n' + \
	    '    }\n' + \
            '%(gtkobjectsink)s' + \
	    '    pygobject_register_wrapper((PyObject *)self);\n' + \
            '    return 0;\n' + \
	    '}\n\n'

methdeftmpl = '    { "%(name)s", (PyCFunction)%(cname)s, %(flags)s },\n'

gettertmpl = 'static PyObject *\n' + \
             '%(funcname)s(PyGObject *self, void *closure)\n' + \
             '{\n' + \
             '%(varlist)s' + \
             '%(code)s\n' + \
             '}\n\n'

noconstructor = 'static int\n' + \
                'pygobject_no_constructor(PyObject *self, PyObject *args, PyObject *kwargs)\n' +\
                '{\n' + \
                '    gchar buf[512];\n' + \
                '\n' + \
                '    g_snprintf(buf, sizeof(buf), "%s is an abstract widget", self->ob_type->tp_name);\n' + \
                '    PyErr_SetString(PyExc_NotImplementedError, buf);\n' + \
                '    return -1;\n' + \
                '}\n\n'

typetmpl = 'PyTypeObject Py%(class)s_Type = {\n' + \
	   '    PyObject_HEAD_INIT(NULL)\n' + \
	   '    0,				/* ob_size */\n' + \
	   '    "%(classname)s",		/* tp_name */\n' + \
	   '    sizeof(PyGObject),		/* tp_basicsize */\n' + \
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
           '    0,				/* tp_as_buffer */\n' + \
           '    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */\n' + \
	   '    NULL, /* Documentation string */\n' + \
           '    (traverseproc)0,		/* tp_traverse */\n' + \
           '    (inquiry)0,			/* tp_clear */\n' + \
           '    (richcmpfunc)0,			/* tp_richcompare */\n' + \
           '    offsetof(PyGObject, weakreflist), /* tp_weaklistoffset */\n' +\
           '    (getiterfunc)0,			/* tp_iter */\n' + \
           '    (iternextfunc)0,		/* tp_iternext */\n' + \
	   '    %(methods)s,			/* tp_methods */\n' + \
           '    0,				/* tp_members */\n' + \
           '    %(getsets)s,			/* tp_getset */\n' + \
           '    NULL,				/* tp_base */\n' + \
           '    NULL,				/* tp_dict */\n' + \
           '    (descrgetfunc)0,		/* tp_descr_get */\n' + \
           '    (descrsetfunc)0,		/* tp_descr_set */\n' + \
           '    offsetof(PyGObject, inst_dict),	/* tp_dictoffset */\n' + \
           '    (initproc)%(initfunc)s,		/* tp_init */\n' + \
	   '};\n\n'

interfacetypetmpl = 'PyTypeObject Py%(class)s_Type = {\n' + \
	   '    PyObject_HEAD_INIT(NULL)\n' + \
	   '    0,				/* ob_size */\n' + \
	   '    "%(classname)s",		/* tp_name */\n' + \
	   '    sizeof(PyObject),		/* tp_basicsize */\n' + \
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
           '    0,				/* tp_as_buffer */\n' + \
           '    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */\n' + \
	   '    NULL, /* Documentation string */\n' + \
           '    (traverseproc)0,		/* tp_traverse */\n' + \
           '    (inquiry)0,			/* tp_clear */\n' + \
           '    (richcmpfunc)0,			/* tp_richcompare */\n' + \
           '    0,				/* tp_weaklistoffset */\n' +\
           '    (getiterfunc)0,			/* tp_iter */\n' + \
           '    (iternextfunc)0,		/* tp_iternext */\n' + \
	   '    %(methods)s,			/* tp_methods */\n' + \
           '    0,				/* tp_members */\n' + \
           '    0,				/* tp_getset */\n' + \
           '    NULL,				/* tp_base */\n' + \
           '    NULL,				/* tp_dict */\n' + \
           '    (descrgetfunc)0,		/* tp_descr_get */\n' + \
           '    (descrsetfunc)0,		/* tp_descr_set */\n' + \
           '    0,				/* tp_dictoffset */\n' + \
           '    (initproc)0,			/* tp_init */\n' + \
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

boxedconsttmpl = 'static int\n' + \
                 '_wrap_%(cname)s(PyGBoxed *self, PyObject *args, PyObject *kwargs)\n' + \
                 '{\n' + \
                 '%(varlist)s' + \
                 '    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "%(typecodes)s:%(typename)s.__init__"%(parselist)s))\n' + \
                 '        return -1;\n' + \
                 '%(extracode)s\n' + \
                 '    self->gtype = %(typecode)s;\n' + \
                 '    self->free_on_dealloc = FALSE;\n' + \
                 '    self->boxed = %(cname)s(%(arglist)s);\n' + \
                 '    if (!self->boxed) {\n' + \
                 '        PyErr_SetString(PyExc_RuntimeError, "could not create %(typename)s object");\n' + \
                 '        return -1;\n' + \
                 '    }\n' + \
                 '    self->free_on_dealloc = TRUE;\n' + \
                 '    return 0;\n' + \
                 '}\n\n'

boxedgetattrtmpl = 'static PyObject *\n' + \
                   '%(getattr)s(PyObject *self, char *attr)\n' + \
                   '{\n' + \
                   '%(attrchecks)s' + \
                   '    PyErr_SetString(PyExc_AttributeError, attr);\n' + \
                   '    return NULL;\n' + \
                   '}\n\n'
attrchecktmpl = '    if (!strcmp(attr, "%(attr)s")) {\n' + \
                '%(varlist)s' + \
                '%(code)s\n' + \
                '    }\n'

boxedtmpl = 'PyTypeObject Py%(typename)s_Type = {\n' + \
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
            '    0,				/* tp_as_buffer */\n' + \
            '    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */\n' +\
            '    NULL, /* Documentation string */\n' + \
            '    (traverseproc)0,		/* tp_traverse */\n' + \
            '    (inquiry)0,			/* tp_clear */\n' + \
            '    (richcmpfunc)0,		/* tp_richcompare */\n' + \
            '    0,				/* tp_weaklistoffset */\n' +\
            '    (getiterfunc)0,			/* tp_iter */\n' + \
            '    (iternextfunc)0,		/* tp_iternext */\n' + \
            '    %(methods)s,			/* tp_methods */\n' + \
            '    0,				/* tp_members */\n' + \
            '    0,				/* tp_getset */\n' + \
            '    NULL,				/* tp_base */\n' + \
            '    NULL,				/* tp_dict */\n' + \
            '    (descrgetfunc)0,		/* tp_descr_get */\n' + \
            '    (descrsetfunc)0,		/* tp_descr_set */\n' + \
            '    0,				/* tp_dictoffset */\n' + \
            '    (initproc)%(initfunc)s,	/* tp_init */\n' + \
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
    dict['extracode'] = string.replace(string.join(extracode, ''),
                                       'return NULL;', 'return -1;')
    dict['arglist']   = string.join(arglist, ', ')
    fp.write(consttmpl % dict)

def write_getsets(parser, objobj, castmacro, overrides, fp=sys.stdout):
    getsets_name = string.lower(castmacro) + '_getsets'
    funcprefix = '_wrap_' + string.lower(castmacro) + '__get_'

    # no overrides for the whole function.  If no fields, don't write a func
    if not objobj.fields:
        return '0'
    getsets = []
    for ftype, fname in objobj.fields:
        funcname = funcprefix + fname
        attrname = objobj.c_name + '.' + fname
        if overrides.attr_is_overriden(attrname):
            code = overrides.attr_override(attrname)
            fp.write(code)
            getsets.append('    { "%s", (getter)%s, (setter)0 },\n' %
                           (fixname(fname), funcname))
            continue
        try:
            varlist = argtypes.VarList()
            handler = argtypes.matcher.get(ftype)
            code = handler.write_return(ftype, varlist) % \
                   {'func': castmacro + '(self->obj)->' + fname}
            fp.write(gettertmpl % { 'funcname': funcname,
                                    'attr': fname,
                                    'varlist': varlist,
                                    'code': code })
            getsets.append('    { "%s", (getter)%s, (setter)0 },\n' %
                           (fixname(fname), funcname))
        except:
            sys.stderr.write("couldn't write check for " + objobj.c_name +
                             '.' + fname + '\n')
	    #traceback.print_exc()
    if not getsets:
        return '0'
    fp.write('static PyGetSetDef %s[] = {\n' % getsets_name)
    for getset in getsets:
        fp.write(getset)
    fp.write('    { NULL, (getter)0, (setter)0 },\n')
    fp.write('};\n\n')
    
    return getsets_name

def write_class(parser, objobj, overrides, fp=sys.stdout):
    fp.write('\n/* ----------- ' + objobj.c_name + ' ----------- */\n\n')
    constructor = parser.find_constructor(objobj, overrides)
    methods = []
    castmacro = string.replace(objobj.typecode, '_TYPE_', '_', 1)
    initfunc = '0'
    if constructor:
	try:
            if overrides.is_overriden(constructor.c_name):
                fp.write(overrides.override(constructor.c_name))
                fp.write('\n\n')
            else:
                write_constructor(objobj.c_name, castmacro, constructor, fp)
            initfunc = '_wrap_' + constructor.c_name
	except:
	    sys.stderr.write('Could not write constructor for ' +
			     objobj.c_name + '\n')
	    #traceback.print_exc()
            # this is a hack ...
            if not hasattr(overrides, 'no_constructor_written'):
                fp.write(noconstructor)
                overrides.no_constructor_written = 1
            initfunc = 'pygobject_no_constructor'
    else:
        # this is a hack ...
        if not hasattr(overrides, 'no_constructor_written'):
            fp.write(noconstructor)
            overrides.no_constructor_written = 1
        initfunc = 'pygobject_no_constructor'
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
    dict = {
        'class': objobj.c_name,
        'classname': objobj.name,
        'initfunc': initfunc
    }
    dict['getsets'] = write_getsets(parser, objobj, castmacro, overrides, fp)
    dict['methods'] = '_Py' + dict['class'] + '_methods'
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
    dict['methods'] = '_Py' + dict['class'] + '_methods'
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
    dict['extracode'] = string.replace(string.join(extracode, ''),
                                       'return NULL;', 'return -1;')
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
    initfunc = '0'
    if constructor:
	try:
            if overrides.is_overriden(constructor.c_name):
                fp.write(overrides.override(constructor.c_name))
                fp.write('\n\n')
            else:
                write_boxed_constructor(boxedobj.c_name, boxedobj.typecode,
                                        constructor, fp)
            initfunc = '_wrap_' + constructor.c_name
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
    dict = { 'typename': boxedobj.c_name, 'initfunc': initfunc }
    dict['getattr'] = write_boxed_getattr(parser, boxedobj, overrides, fp)
    dict['methods'] = '_Py' + dict['typename'] + '_methods'
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

def write_enums(parser, prefix, fp=sys.stdout):
    if not parser.enums:
        return
    fp.write('\n/* ----------- enums and flags ----------- */\n\n')
    fp.write('void\n' + prefix + '_add_constants(PyObject *module, const gchar *strip_prefix)\n{\n')
    for enum in parser.enums:
        if enum.deftype == 'enum':
            fp.write('    pyg_enum_add_constants(module, %s, strip_prefix);\n'
                     % (enum.typecode,))
        else:
            fp.write('    pyg_flags_add_constants(module, %s, strip_prefix);\n'
                     % (enum.typecode,))
    fp.write('}\n\n')

def write_source(parser, overrides, prefix, fp=sys.stdout):
    fp.write('/* -*- Mode: C; c-basic-offset: 4 -*- */\n\n')
    fp.write('#include <Python.h>\n\n\n')
    fp.write(overrides.get_headers())
    fp.write('\n\n')
    fp.write('/* ---------- forward type declarations ---------- */\n')
    for obj in parser.boxes:
        fp.write('PyTypeObject Py' + obj.c_name + '_Type;\n')
    for obj in parser.objects:
        fp.write('PyTypeObject Py' + obj.c_name + '_Type;\n')
    for interface in parser.interfaces:
        fp.write('PyTypeObject Py' + interface.c_name + '_Type;\n')
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

    write_enums(parser, prefix, fp)

    fp.write('/* intialise stuff extension classes */\n')
    fp.write('void\n' + prefix + '_register_classes(PyObject *d)\n{\n')
    fp.write(overrides.get_init() + '\n')

    for boxed in parser.boxes:
        fp.write('    pyg_register_boxed(d, "' + boxed.name +
                 '", ' + boxed.typecode + ', &Py' + boxed.c_name + '_Type);\n')
    for interface in parser.interfaces:
        fp.write('    pyg_register_interface(d, "' + interface.name +
                 '", '+ interface.typecode + ', &Py' + interface.c_name +
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
        if bases:
            fp.write('    pygobject_register_class(d, "' + obj.c_name +
                     '", ' + obj.typecode + ', &Py' + obj.c_name +
                     '_Type, Py_BuildValue("(' + 'O' * len(bases) + ')", ' +
                     string.join(map(lambda s: '&Py'+s+'_Type', bases), ', ') +
                     '));\n')
        else:
            fp.write('    pygobject_register_class(d, "' + obj.c_name +
                     '", ' + obj.typecode + ', &Py' + obj.c_name +
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
