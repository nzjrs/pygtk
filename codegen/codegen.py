#!/usr/bin/env python
# -*- Mode: Python; py-indent-offset: 4 -*-
import sys, os, string
import getopt
import defsparser, argtypes, override
import traceback, keyword, exceptions

def exc_info():
    #traceback.print_exc()
    etype, value, tb = sys.exc_info()
    ret = ""
    try:
        sval = str(value)
        if etype == exceptions.KeyError:
            ret = "No ArgType for %s" % (sval,)
        else:
            ret = sval
    finally:
        del etype, value, tb
    return ret

class FileOutput:
    '''Simple wrapper for file object, that makes writing #line
    statements easier.''' # "
    def __init__(self, fp, filename=None):
        self.fp = fp
        self.lineno = 1
        if filename:
            self.filename = filename
        else:
            self.filename = self.fp.name
    # handle writing to the file, and keep track of the line number ...
    def write(self, str):
        self.fp.write(str)
        self.lineno = self.lineno + string.count(str, '\n')
    def writelines(self, sequence):
        for line in sequence:
            self.write(line)
    def close(self):
        self.fp.close()
    def flush(self):
        self.fp.flush()

    def setline(self, linenum, filename):
        '''writes out a #line statement, for use by the C
        preprocessor.''' # "
        self.write('#line %d "%s"\n' % (linenum, filename))
    def resetline(self):
        '''resets line numbering to the original file'''
        self.setline(self.lineno + 1, self.filename)

# list of slots that we will override
slots_list = ['tp_getattr', 'tp_setattr', 'tp_compare', 'tp_repr',
              'tp_as_number', 'tp_as_sequence', 'tp_as_mapping', 'tp_hash',
              'tp_call', 'tp_str', 'tp_richcompare', 'tp_iter',
              'tp_iternext', 'tp_descr_get', 'tp_descr_set']

# have to do return type processing
functmpl = ('static PyObject *\n'
            '_wrap_%(cname)s(PyObject *self%(extraparams)s)\n'
            '{\n'
            '%(varlist)s'
            '%(parseargs)s'
            '%(codebefore)s'
            '    %(funccall)s\n'
            '%(codeafter)s\n'
            '}\n\n')

funccalltmpl = '%(cname)s(%(arglist)s);'

methtmpl = ('static PyObject *\n'
            '_wrap_%(cname)s(PyGObject *self%(extraparams)s)\n'
            '{\n'
            '%(varlist)s'
            '%(parseargs)s'
            '%(codebefore)s'
            '    %(funccall)s\n'
            '%(codeafter)s\n'
            '}\n\n')

methcalltmpl = '%(cname)s(%(cast)s(self->obj)%(arglist)s);'

consttmpl = ('static int\n'
             '_wrap_%(cname)s(PyGObject *self, PyObject *args, PyObject *kwargs)\n'
             '{\n'
             '%(varlist)s'
             '    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "%(typecodes)s:%(class)s.__init__"%(parselist)s))\n'
             '        return -1;\n'
             '%(codebefore)s'
             '    self->obj = (GObject *)%(cname)s(%(arglist)s);\n'
             '%(codeafter)s\n'
             '    if (!self->obj) {\n'
             '        PyErr_SetString(PyExc_RuntimeError, "could not create %(class)s object");\n'
             '        return -1;\n'
             '    }\n'
             '%(gtkobjectsink)s'
             '    pygobject_register_wrapper((PyObject *)self);\n'
             '    return 0;\n'
             '}\n\n')

methdeftmpl = '    { "%(name)s", (PyCFunction)%(cname)s, %(flags)s },\n'

deprecatedtmpl = ('    if (PyErr_Warn(PyExc_DeprecationWarning, "%s") < 0)\n'
                  '        return NULL;\n')

gettertmpl = ('static PyObject *\n'
              '%(funcname)s(PyGObject *self, void *closure)\n'
              '{\n'
              '%(varlist)s'
              '    ret = %(field)s;\n'
              '%(codeafter)s\n'
              '}\n\n')

noconstructor = ('static int\n'
                 'pygobject_no_constructor(PyObject *self, PyObject *args, PyObject *kwargs)\n'
                 '{\n'
                 '    gchar buf[512];\n'
                 '\n'
                 '    g_snprintf(buf, sizeof(buf), "%s is an abstract widget", self->ob_type->tp_name);\n'
                 '    PyErr_SetString(PyExc_NotImplementedError, buf);\n'
                 '    return -1;\n'
                 '}\n\n')

typetmpl = """PyTypeObject Py%(class)s_Type = {
    PyObject_HEAD_INIT(NULL)
    0,					/* ob_size */
    "%(classname)s",			/* tp_name */
    sizeof(PyGObject),			/* tp_basicsize */
    0,					/* tp_itemsize */
    /* methods */
    (destructor)0,			/* tp_dealloc */
    (printfunc)0,			/* tp_print */
    (getattrfunc)%(tp_getattr)s,	/* tp_getattr */
    (setattrfunc)%(tp_setattr)s,	/* tp_setattr */
    (cmpfunc)%(tp_compare)s,		/* tp_compare */
    (reprfunc)%(tp_repr)s,		/* tp_repr */
    %(tp_as_number)s,			/* tp_as_number */
    %(tp_as_sequence)s,			/* tp_as_sequence */
    %(tp_as_mapping)s,			/* tp_as_mapping */
    (hashfunc)%(tp_hash)s,		/* tp_hash */
    (ternaryfunc)%(tp_call)s,		/* tp_call */
    (reprfunc)%(tp_str)s,		/* tp_str */
    (getattrofunc)0,			/* tp_getattro */
    (setattrofunc)0,			/* tp_setattro */
    0,					/* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    NULL, 				/* Documentation string */
    (traverseproc)0,			/* tp_traverse */
    (inquiry)0,				/* tp_clear */
    (richcmpfunc)%(tp_richcompare)s,	/* tp_richcompare */
    offsetof(PyGObject, weakreflist),	/* tp_weaklistoffset */
    (getiterfunc)%(tp_iter)s,		/* tp_iter */
    (iternextfunc)%(tp_iternext)s,	/* tp_iternext */
    %(methods)s,			/* tp_methods */
    0,					/* tp_members */
    %(getsets)s,		       	/* tp_getset */
    NULL,				/* tp_base */
    NULL,				/* tp_dict */
    (descrgetfunc)%(tp_descr_get)s,	/* tp_descr_get */
    (descrsetfunc)%(tp_descr_set)s,	/* tp_descr_set */
    offsetof(PyGObject, inst_dict),	/* tp_dictoffset */
    (initproc)%(initfunc)s,		/* tp_init */
};\n\n"""

interfacetypetmpl = """PyTypeObject Py%(class)s_Type = {
    PyObject_HEAD_INIT(NULL)
    0,					/* ob_size */
    "%(classname)s",			/* tp_name */
    sizeof(PyObject),			/* tp_basicsize */
    0,					/* tp_itemsize */
    /* methods */
    (destructor)0,			/* tp_dealloc */
    (printfunc)0,			/* tp_print */
    (getattrfunc)%(tp_getattr)s,	/* tp_getattr */
    (setattrfunc)%(tp_setattr)s,	/* tp_setattr */
    (cmpfunc)%(tp_compare)s,		/* tp_compare */
    (reprfunc)%(tp_repr)s,		/* tp_repr */
    %(tp_as_number)s,			/* tp_as_number */
    %(tp_as_sequence)s,			/* tp_as_sequence */
    %(tp_as_mapping)s,			/* tp_as_mapping */
    (hashfunc)%(tp_hash)s,		/* tp_hash */
    (ternaryfunc)%(tp_call)s,		/* tp_call */
    (reprfunc)%(tp_str)s,		/* tp_str */
    (getattrofunc)0,			/* tp_getattro */
    (setattrofunc)0,			/* tp_setattro */
    0,					/* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    NULL, 				/* Documentation string */
    (traverseproc)0,			/* tp_traverse */
    (inquiry)0,				/* tp_clear */
    (richcmpfunc)%(tp_richcompare)s,	/* tp_richcompare */
    0,					/* tp_weaklistoffset */
    (getiterfunc)%(tp_iter)s,		/* tp_iter */
    (iternextfunc)%(tp_iternext)s,	/* tp_iternext */
    %(methods)s,			/* tp_methods */
    0,					/* tp_members */
    0,					/* tp_getset */
    NULL,				/* tp_base */
    NULL,				/* tp_dict */
    (descrgetfunc)%(tp_descr_get)s,	/* tp_descr_get */
    (descrsetfunc)%(tp_descr_set)s,	/* tp_descr_set */
    0,					/* tp_dictoffset */
    (initproc)0,			/* tp_init */
};\n\n"""

boxedmethtmpl = ('static PyObject *\n'
                 '_wrap_%(cname)s(PyObject *self%(extraparams)s)\n'
                 '{\n'
                 '%(varlist)s'
                 '%(parseargs)s'
                 '%(codebefore)s'
                 '    %(funccall)s\n'
                 '%(codeafter)s\n'
                 '}\n\n')

boxedmethcalltmpl = '%(cname)s(pyg_boxed_get(self, %(typename)s)%(arglist)s);'

boxedconsttmpl = ('static int\n'
                  '_wrap_%(cname)s(PyGBoxed *self, PyObject *args, PyObject *kwargs)\n'
                  '{\n'
                  '%(varlist)s'
                  '    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "%(typecodes)s:%(typename)s.__init__"%(parselist)s))\n'
                  '        return -1;\n'
                  '%(codebefore)s'
                  '    self->gtype = %(typecode)s;\n'
                  '    self->free_on_dealloc = FALSE;\n'
                  '    self->boxed = %(cname)s(%(arglist)s);\n'
                  '%(codeafter)s\n'
                  '    if (!self->boxed) {\n'
                  '        PyErr_SetString(PyExc_RuntimeError, "could not create %(typename)s object");\n'
                  '        return -1;\n'
                  '    }\n'
                  '    self->free_on_dealloc = TRUE;\n'
                  '    return 0;\n'
                  '}\n\n')
                  
boxedgettertmpl = ('static PyObject *\n'
                   '%(funcname)s(PyObject *self, void *closure)\n'
                   '{\n'
                   '%(varlist)s'
                   '    ret = %(field)s;\n'
                   '%(codeafter)s\n'
                   '}\n\n')
                    

boxedtmpl = """PyTypeObject Py%(typename)s_Type = {
    PyObject_HEAD_INIT(NULL)
    0,					/* ob_size */
    "%(classname)s",			/* tp_name */
    sizeof(PyGBoxed),			/* tp_basicsize */
    0,					/* tp_itemsize */
    /* methods */
    (destructor)0,			/* tp_dealloc */
    (printfunc)0,			/* tp_print */
    (getattrfunc)%(tp_getattr)s,	/* tp_getattr */
    (setattrfunc)%(tp_setattr)s,	/* tp_setattr */
    (cmpfunc)%(tp_compare)s,		/* tp_compare */
    (reprfunc)%(tp_repr)s,		/* tp_repr */
    %(tp_as_number)s,			/* tp_as_number */
    %(tp_as_sequence)s,			/* tp_as_sequence */
    %(tp_as_mapping)s,			/* tp_as_mapping */
    (hashfunc)%(tp_hash)s,		/* tp_hash */
    (ternaryfunc)%(tp_call)s,		/* tp_call */
    (reprfunc)%(tp_str)s,		/* tp_str */
    (getattrofunc)0,			/* tp_getattro */
    (setattrofunc)0,			/* tp_setattro */
    0,					/* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    NULL,				/* Documentation string */
    (traverseproc)0,			/* tp_traverse */
    (inquiry)0,				/* tp_clear */
    (richcmpfunc)%(tp_richcompare)s,	/* tp_richcompare */
    0,					/* tp_weaklistoffset */
    (getiterfunc)%(tp_iter)s,		/* tp_iter */
    (iternextfunc)%(tp_iternext)s,	/* tp_iternext */
    %(methods)s,			/* tp_methods */
    0,					/* tp_members */
    %(getsets)s,			/* tp_getset */
    NULL,				/* tp_base */
    NULL,				/* tp_dict */
    (descrgetfunc)%(tp_descr_get)s,	/* tp_descr_get */
    (descrsetfunc)%(tp_descr_set)s,	/* tp_descr_set */
    0,					/* tp_dictoffset */
    (initproc)%(initfunc)s,		/* tp_init */
};\n\n"""

pointermethcalltmpl = '%(cname)s(pyg_pointer_get(self, %(typename)s)%(arglist)s);'

pointerconsttmpl = ('static int\n'
                    '_wrap_%(cname)s(PyGPointer *self, PyObject *args, PyObject *kwargs)\n'
                    '{\n'
                    '%(varlist)s'
                    '    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "%(typecodes)s:%(typename)s.__init__"%(parselist)s))\n'
                    '        return -1;\n'
                    '%(codebefore)s'
                    '    self->gtype = %(typecode)s;\n'
                    '    self->pointer = %(cname)s(%(arglist)s);\n'
                    '%(codeafter)s\n'
                    '    if (!self->pointer) {\n'
                    '        PyErr_SetString(PyExc_RuntimeError, "could not create %(typename)s object");\n'
                    '        return -1;\n'
                    '    }\n'
                    '    return 0;\n'
                    '}\n\n')

pointertmpl = """PyTypeObject Py%(typename)s_Type = {
    PyObject_HEAD_INIT(NULL)
    0,					/* ob_size */
    "%(classname)s",			/* tp_name */
    sizeof(PyGPointer),			/* tp_basicsize */
    0,					/* tp_itemsize */
    /* methods */
    (destructor)0,			/* tp_dealloc */
    (printfunc)0,			/* tp_print */
    (getattrfunc)%(tp_getattr)s,	/* tp_getattr */
    (setattrfunc)%(tp_setattr)s,	/* tp_setattr */
    (cmpfunc)%(tp_compare)s,		/* tp_compare */
    (reprfunc)%(tp_repr)s,		/* tp_repr */
    %(tp_as_number)s,			/* tp_as_number */
    %(tp_as_sequence)s,			/* tp_as_sequence */
    %(tp_as_mapping)s,			/* tp_as_mapping */
    (hashfunc)%(tp_hash)s,		/* tp_hash */
    (ternaryfunc)%(tp_call)s,		/* tp_call */
    (reprfunc)%(tp_str)s,		/* tp_str */
    (getattrofunc)0,			/* tp_getattro */
    (setattrofunc)0,			/* tp_setattro */
    0,					/* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    NULL,				/* Documentation string */
    (traverseproc)0,			/* tp_traverse */
    (inquiry)0,				/* tp_clear */
    (richcmpfunc)%(tp_richcompare)s,	/* tp_richcompare */
    0,					/* tp_weaklistoffset */
    (getiterfunc)%(tp_iter)s,		/* tp_iter */
    (iternextfunc)%(tp_iternext)s,	/* tp_iternext */
    %(methods)s,			/* tp_methods */
    0,					/* tp_members */
    %(getsets)s,			/* tp_getset */
    NULL,				/* tp_base */
    NULL,				/* tp_dict */
    (descrgetfunc)%(tp_descr_get)s,	/* tp_descr_get */
    (descrsetfunc)%(tp_descr_set)s,	/* tp_descr_set */
    0,					/* tp_dictoffset */
    (initproc)%(initfunc)s,		/* tp_init */
};\n\n"""


def fixname(name):
    if keyword.iskeyword(name):
	return name + '_'
    return name

def write_function(funcobj, fp=sys.stdout):
    info = argtypes.WrapperInfo()

# have to do return type processing
    if funcobj.varargs:
        raise ValueError, "varargs functions not supported"
    
    dict = {
	'cname':   funcobj.c_name,
    }

    for ptype, pname, pdflt, pnull in funcobj.params:
	if pdflt and '|' not in info.parsestr:
            info.add_parselist('|', [], [])
	handler = argtypes.matcher.get(ptype)
        handler.write_param(ptype, pname, pdflt, pnull, info)

    funccall = funccalltmpl % { 'cname': funcobj.c_name,
                                'arglist': info.get_arglist() }
    if funcobj.ret not in ('none', None):
        funccall = 'ret = ' + funccall

    handler = argtypes.matcher.get(funcobj.ret)
    handler.write_return(funcobj.ret, info)

    deprecated = ""
    if funcobj.deprecated != None:
        deprecated = deprecatedtmpl % funcobj.deprecated
    
    dict['codebefore'] = deprecated + info.get_codebefore()
    dict['funccall']   = funccall
    dict['codeafter']  = info.get_codeafter()

    if info.parsestr:
        parsetmpl = ('    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "%(typecodes)s:%(name)s"%(parselist)s))\n'
                     '        return NULL;\n')
        parsedict = {'name':      fixname(funcobj.name),
                     'typecodes': info.parsestr,
                     'parselist': info.get_parselist()}

        dict['parseargs'] = parsetmpl % parsedict
        dict['varlist'] = info.get_kwlist() + info.get_varlist()
        dict['extraparams'] = ', PyObject *args, PyObject *kwargs'
        flags = 'METH_VARARGS|METH_KEYWORDS'
    else:
        dict['parseargs'] = ''
        dict['varlist'] = info.get_varlist()
        dict['extraparams'] = ''
        flags = 'METH_NOARGS'

    fp.write(functmpl % dict)

    return flags

def write_method(objname, castmacro, methobj, fp=sys.stdout):
    info = argtypes.WrapperInfo()
    info.arglist.append('')

    if methobj.varargs:
        raise ValueError, "varargs methods not supported"
    
    dict = {
	'cname':   methobj.c_name,
    }
    
    for ptype, pname, pdflt, pnull in methobj.params:
	if pdflt and '|' not in info.parsestr:
            info.add_parselist('|', [], [])
	handler = argtypes.matcher.get(ptype)
        handler.write_param(ptype, pname, pdflt, pnull, info)

    funccall = methcalltmpl % { 'cname':   methobj.c_name,
                                'cast':    castmacro,
                                'arglist': info.get_arglist() }
    if methobj.ret not in ('none', None):
        funccall = 'ret = ' + funccall

    handler = argtypes.matcher.get(methobj.ret)
    handler.write_return(methobj.ret, info)

    deprecated = ""
    if methobj.deprecated != None:
        deprecated = deprecatedtmpl % methobj.deprecated

    dict['codebefore'] = deprecated + info.get_codebefore()
    dict['funccall']   = funccall
    dict['codeafter']  = info.get_codeafter()

    if info.parsestr:
        parsetmpl = ('    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "%(typecodes)s:%(class)s.%(name)s"%(parselist)s))\n'
                     '        return NULL;\n')
        parsedict = {'class':     objname,
                     'name':      fixname(methobj.name),
                     'typecodes': info.parsestr,
                     'parselist': info.get_parselist()}

        dict['parseargs'] = parsetmpl % parsedict
        dict['varlist'] = info.get_kwlist() + info.get_varlist()
        dict['extraparams'] = ', PyObject *args, PyObject *kwargs'
        flags = 'METH_VARARGS|METH_KEYWORDS'
    else:
        dict['parseargs'] = ''
        dict['varlist']   = info.get_varlist()
        dict['extraparams'] = ''
        flags = 'METH_NOARGS'

    fp.write(methtmpl % dict)

    return flags

def write_constructor(objname, castmacro, funcobj, fp=sys.stdout):
    info = argtypes.WrapperInfo()

    dict = {
	'name':    funcobj.name,
	'cname':   funcobj.c_name,
	'class':   objname,
	'cast':    castmacro,
	'gtkobjectsink': ''
    }

    if argtypes.matcher.object_is_a(objname, 'GtkObject'):
        dict['gtkobjectsink'] = \
                       ('    gtk_object_ref(GTK_OBJECT(self->obj));\n'
			'    gtk_object_sink(GTK_OBJECT(self->obj));\n')

    for ptype, pname, pdflt, pnull in funcobj.params:
	if pdflt and '|' not in info.parsestr:
            info.add_parselist('|', [], [])
	handler = argtypes.matcher.get(ptype)
        handler.write_param(ptype, pname, pdflt, pnull, info)
        
    deprecated = ""
    if funcobj.deprecated != None:
        deprecated = deprecatedtmpl % funcobj.deprecated

    dict['varlist']    = info.get_kwlist() + info.get_varlist()
    dict['typecodes']  = info.parsestr
    dict['parselist']  = info.get_parselist()
    dict['codebefore'] = string.replace(deprecated + info.get_codebefore(),
                                        'return NULL;', 'return -1;')
    dict['codeafter']  = string.replace(info.get_codeafter(),
                                       'return NULL;', 'return -1;')
    dict['arglist']    = info.get_arglist()

    fp.write(consttmpl % dict)

def write_getsets(parser, objobj, castmacro, overrides, fp=sys.stdout):
    getsets_name = string.lower(castmacro) + '_getsets'
    getterprefix = '_wrap_' + string.lower(castmacro) + '__get_'
    setterprefix = '_wrap_' + string.lower(castmacro) + '__set_'

    # no overrides for the whole function.  If no fields, don't write a func
    if not objobj.fields:
        return '0'
    getsets = []
    for ftype, fname in objobj.fields:
        gettername = '0'
        settername = '0'
        attrname = objobj.c_name + '.' + fname
        if overrides.attr_is_overriden(attrname):
            lineno, filename = overrides.getstartline(attrname)
            code = overrides.attr_override(attrname)
            fp.setline(lineno, filename)
            fp.write(code)
            fp.resetline()
            if string.find(code, getterprefix + fname) >= 0:
                gettername = getterprefix + fname
            if string.find(code, setterprefix + fname) >= 0:
                settername = setterprefix + fname
        if gettername == '0':
            try:
                funcname = getterprefix + fname
                info = argtypes.WrapperInfo()
                handler = argtypes.matcher.get(ftype)
                handler.write_return(ftype, info)
                fp.write(gettertmpl %
                         { 'funcname': funcname,
                           'varlist': info.varlist,
                           'field': castmacro+'(self->obj)->' + fname,
                           'codeafter': info.get_codeafter() })
                gettername = funcname
            except:
                sys.stderr.write("Could not write getter for %s.%s: %s\n"
                                 % (objobj.c_name, fname, exc_info()))
        if gettername != '0' or settername != '0':
            getsets.append('    { "%s", (getter)%s, (setter)%s },\n' %
                           (fixname(fname), gettername, settername))

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
                lineno, filename = overrides.getstartline(constructor.c_name)
                fp.setline(lineno, filename)
                fp.write(overrides.override(constructor.c_name))
                fp.resetline()
                fp.write('\n\n')
            else:
                write_constructor(objobj.c_name, castmacro, constructor, fp)
            initfunc = '_wrap_' + constructor.c_name
	except:
	    sys.stderr.write('Could not write constructor for %s: %s\n' 
			     % (objobj.c_name, exc_info()))
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
                lineno, filename = overrides.getstartline(meth.c_name)
                fp.setline(lineno, filename)
                fp.write(overrides.override(meth.c_name))
                fp.resetline()
                fp.write('\n\n')
                if overrides.wants_kwargs(meth.c_name):
                    methtype = methtype + '|METH_KEYWORDS'
                elif overrides.wants_noargs(meth.c_name):
                    methtype = 'METH_NOARGS'
            else:
                methtype = write_method(objobj.c_name, castmacro, meth, fp)
	    methods.append(methdeftmpl % { 'name':  fixname(meth.name),
					   'cname': '_wrap_' + meth.c_name,
					   'flags': methtype})
	except:
	    sys.stderr.write('Could not write method %s.%s: %s\n'
                             % (objobj.c_name, meth.name, exc_info()))

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
    if overrides.modulename:
        dict['classname'] = '%s.%s' % (overrides.modulename, objobj.name)
    else:
        dict['classname'] = objobj.name
    dict['getsets'] = write_getsets(parser, objobj, castmacro, overrides, fp)
    dict['methods'] = '_Py' + dict['class'] + '_methods'

    # handle slots ...
    for slot in slots_list:
        slotname = '%s.%s' % (objobj.c_name, slot)
        slotfunc = '_wrap_%s_%s' % (string.lower(castmacro), slot)
        if slot[:6] == 'tp_as_':
            slotfunc = '&' + slotfunc
        if overrides.slot_is_overriden(slotname):
            lineno, filename = overrides.getstartline(slotname)
            fp.setline(lineno, filename)
            fp.write(overrides.slot_override(slotname))
            fp.resetline()
            fp.write('\n\n')
            dict[slot] = slotfunc
        else:
            dict[slot] = '0'
    
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
                lineno, filename = overrides.getstartline(meth.c_name)
                fp.setline(lineno, filename)
                fp.write(overrides.override(meth.c_name))
                fp.resetline()
                fp.write('\n\n')
                if overrides.wants_kwargs(meth.c_name):
                    methtype = methtype + '|METH_KEYWORDS'
                elif overrides.wants_noargs(meth.c_name):
                    methtype = 'METH_NOARGS'
            else:
                methtype = write_method(interface.c_name, castmacro, meth, fp)
	    methods.append(methdeftmpl % { 'name':  fixname(meth.name),
					   'cname': '_wrap_' + meth.c_name,
					   'flags': methtype})
	except:
	    sys.stderr.write('Could not write method %s.%s: %s\n'
                             % (interface.c_name, meth.name, exc_info()))

    # write the PyMethodDef structure
    methods.append('    { NULL, NULL, 0 }\n')
    fp.write('static PyMethodDef _Py' + interface.c_name + '_methods[] = {\n')
    fp.write(string.join(methods, ''))
    fp.write('};\n\n')

    # write the type template
    dict = { 'class': interface.c_name }
    if overrides.modulename:
        dict['classname'] = '%s.%s' % (overrides.modulename, interface.name)
    else:
        dict['classname'] = inerface.name
    dict['methods'] = '_Py' + dict['class'] + '_methods'

    # handle slots ...
    for slot in slots_list:
        slotname = '%s.%s' % (interface.c_name, slot)
        slotfunc = '_wrap_%s_%s' % (string.lower(castmacro), slot)
        if slot[:6] == 'tp_as_':
            slotfunc = '&' + slotfunc
        if overrides.slot_is_overriden(slotname):
            lineno, filename = overrides.getstartline(slotname)
            fp.setline(lineno, filename)
            fp.write(overrides.slot_override(slotname))
            fp.resetline()
            fp.write('\n\n')
            dict[slot] = slotfunc
        else:
            dict[slot] = '0'

    fp.write(interfacetypetmpl % dict)

## boxed types ...
def write_boxed_method(objname, methobj, fp=sys.stdout):
    info = argtypes.WrapperInfo()
    info.arglist.append('')

    if methobj.varargs:
        raise ValueError, "varargs methods not supported"
    
    dict = {
	'cname':    methobj.c_name,
    }
    
    for ptype, pname, pdflt, pnull in methobj.params:
	if pdflt and '|' not in info.parsestr:
            info.add_parselist('|', [], [])
	handler = argtypes.matcher.get(ptype)
        handler.write_param(ptype, pname, pdflt, pnull, info)

    funccall = boxedmethcalltmpl % { 'cname':    methobj.c_name,
                                     'typename': objname,
                                     'arglist':  info.get_arglist() }
    if methobj.ret not in ('none', None):
        funccall = 'ret = ' + funccall

    handler = argtypes.matcher.get(methobj.ret)
    handler.write_return(methobj.ret, info)

    dict['codebefore'] = info.get_codebefore()
    dict['funccall']   = funccall
    dict['codeafter']  = info.get_codeafter()
    
    if info.parsestr:
        parsetmpl = ('    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "%(typecodes)s:%(typename)s.%(name)s"%(parselist)s))\n'
                     '        return NULL;\n')
        parsedict = {'typecodes': info.parsestr,
                     'typename':  objname,
                     'name':      fixname(methobj.name),
                     'parselist': info.get_parselist()}

        dict['parseargs'] = parsetmpl % parsedict
        dict['varlist'] = info.get_kwlist() + info.get_varlist()
        dict['extraparams'] = ', PyObject *args, PyObject *kwargs'
        flags = 'METH_VARARGS|METH_KEYWORDS'
    else:
        dict['parseargs'] = ''
        dict['varlist']    = info.get_varlist()
        dict['extraparams'] = ''
        flags = 'METH_NOARGS'

    fp.write(boxedmethtmpl % dict)

    return flags

def write_boxed_constructor(objname, typecode, funcobj, fp=sys.stdout):
    info = argtypes.WrapperInfo()

    dict = {
	'name':    funcobj.name,
	'cname':   funcobj.c_name,
	'typename': objname,
        'typecode': typecode,
    }

    for ptype, pname, pdflt, pnull in funcobj.params:
	if pdflt and '|' not in info.parsestr:
	    info.add_parselist('|', [], [])
	handler = argtypes.matcher.get(ptype)
        spec = handler.write_param(ptype, pname, pdflt, pnull, info)

    dict['varlist']    = info.get_kwlist() + info.get_varlist()
    dict['typecodes']  = info.parsestr
    dict['parselist']  = info.get_parselist()
    dict['codebefore'] = string.replace(info.get_codebefore(),
                                       'return NULL;', 'return -1;')
    dict['codeafter']  = string.replace(info.get_codeafter(),
                                       'return NULL;', 'return -1;')
    dict['arglist']    = info.get_arglist()
    fp.write(boxedconsttmpl % dict)

def write_boxed_getsets(parser, boxedobj, overrides, fp=sys.stdout):
    typecode = boxedobj.typecode
    uline = string.replace(typecode, '_TYPE_', '_', 1)
    getsets_name = string.lower(uline) + '_getsets'
    getterprefix = '_wrap_' + string.lower(uline) + '__get_'
    setterprefix = '_wrap_' + string.lower(uline) + '__set_'

    if not boxedobj.fields:
        return '0'

    getsets = []
    for ftype, fname in boxedobj.fields:
        attrname = boxedobj.c_name + '.' + fname
        gettername = '0'
        settername = '0'
        if overrides.attr_is_overriden(attrname):
            lineno, filename = overrides.getstartline(attrname)
            code = overrides.attr_override(attrname)
            fp.setline(lineno, filename)
            fp.write(code)
            fp.resetline()
            if string.find(code, getterprefix + fname) >= 0:
                gettername = getterprefix + fname
            if string.find(code, setterprefix + fname) >= 0:
                settername = setterprefix + fname
        if gettername == '0':
            try:
                funcname = getterprefix + fname
                info =  argtypes.WrapperInfo()
                handler = argtypes.matcher.get(ftype)
                handler.write_return(ftype, info)
                field = 'pyg_boxed_get(self, ' + boxedobj.c_name + ')->' + fname
                fp.write(boxedgettertmpl %
                         { 'funcname': funcname,
                           'varlist': info.varlist,
                           'field': field,
                           'codeafter': info.get_codeafter() })
                gettername = funcname
            except:
                sys.stderr.write("Could not write getter for %s.%s: %s\n"
                                 % (boxedobj.c_name, fname, exc_info()))
        if gettername != '0' or settername != '0':
            getsets.append('    { "%s", (getter)%s, (setter)%s },\n' %
                           (fixname(fname), gettername, settername))

    if not getsets:
        return '0'
    fp.write('static PyGetSetDef %s[] = {\n' % getsets_name)
    for getset in getsets:
        fp.write(getset)
    fp.write('    { NULL, (getter)0, (setter)0 },\n')
    fp.write('};\n\n')

    return getsets_name

def write_boxed(parser, boxedobj, overrides, fp=sys.stdout):
    fp.write('\n/* ----------- ' + boxedobj.c_name + ' ----------- */\n\n')
    constructor = parser.find_constructor(boxedobj, overrides)
    methods = []
    initfunc = '0'
    if constructor:
	try:
            if overrides.is_overriden(constructor.c_name):
                lineno, filename = overrides.getstartline(constructor.c_name)
                fp.setline(lineno, filename)
                fp.write(overrides.override(constructor.c_name))
                fp.resetline()
                fp.write('\n\n')
            else:
                write_boxed_constructor(boxedobj.c_name, boxedobj.typecode,
                                        constructor, fp)
            initfunc = '_wrap_' + constructor.c_name
	except:
	    sys.stderr.write('Could not write constructor for %s: %s\n'
                             % (boxedobj.c_name, exc_info()))
    for meth in parser.find_methods(boxedobj):
        if overrides.is_ignored(meth.c_name):
            continue
	try:
            methtype = 'METH_VARARGS'
            if overrides.is_overriden(meth.c_name):
                lineno, filename = overrides.getstartline(meth.c_name)
                fp.setline(lineno, filename)
                fp.write(overrides.override(meth.c_name))
                fp.resetline()
                fp.write('\n\n')
                if overrides.wants_kwargs(meth.c_name):
                    methtype = methtype + '|METH_KEYWORDS'
                elif overrides.wants_noargs(meth.c_name):
                    methtype = 'METH_NOARGS'
            else:
                methtype = write_boxed_method(boxedobj.c_name, meth, fp)
	    methods.append(methdeftmpl % { 'name':  fixname(meth.name),
					   'cname': '_wrap_' + meth.c_name,
					   'flags': methtype})
	except:
	    sys.stderr.write('Could not write method %s.%s: %s\n'
                             % (boxedobj.c_name, meth.name, exc_info()))

    # write the PyMethodDef structure
    methods.append('    { NULL, NULL, 0 }\n')
    fp.write('static PyMethodDef _Py' + boxedobj.c_name + '_methods[] = {\n')
    fp.write(string.join(methods, ''))
    fp.write('};\n\n')

    # write the type template
    dict = { 'typename': boxedobj.c_name, 'initfunc': initfunc }

    if overrides.modulename:
        dict['classname'] = '%s.%s' % (overrides.modulename, boxedobj.name)
    else:
        dict['classname'] = boxedobj.name

    dict['getsets'] = write_boxed_getsets(parser, boxedobj, overrides, fp)
    dict['methods'] = '_Py' + dict['typename'] + '_methods'

    # handle slots ...
    for slot in slots_list:
        slotname = '%s.%s' % (boxedobj.c_name, slot)
        slotfunc = '_wrap_%s_%s' % \
                   (string.lower(string.replace(boxedobj.typecode,
                                                '_TYPE_', '_', 1)), slot)
        if slot[:6] == 'tp_as_':
            slotfunc = '&' + slotfunc
        if overrides.slot_is_overriden(slotname):
            lineno, filename = overrides.getstartline(slotname)
            fp.setline(lineno, filename)
            fp.write(overrides.slot_override(slotname))
            fp.resetline()
            fp.write('\n\n')
            dict[slot] = slotfunc
        else:
            dict[slot] = '0'

    fp.write(boxedtmpl % dict)

## pointer types ...
def write_pointer_method(objname, methobj, fp=sys.stdout):
    info = argtypes.WrapperInfo()
    info.arglist.append('')

    if methobj.varargs:
        raise ValueError, "varargs methods not supported"
    
    dict = {
	'cname':    methobj.c_name,
    }
    
    for ptype, pname, pdflt, pnull in methobj.params:
	if pdflt and '|' not in info.parsestr:
            info.add_parselist('|', [], [])
	handler = argtypes.matcher.get(ptype)
        handler.write_param(ptype, pname, pdflt, pnull, info)

    funccall = pointermethcalltmpl % { 'cname':    methobj.c_name,
                                       'typename': objname,
                                       'arglist':  info.get_arglist() }
    if methobj.ret not in ('none', None):
        funccall = 'ret = ' + funccall

    handler = argtypes.matcher.get(methobj.ret)
    handler.write_return(methobj.ret, info)

    dict['codebefore'] = info.get_codebefore()
    dict['funccall']   = funccall
    dict['codeafter']  = info.get_codeafter()
    
    if info.parsestr:
        parsetmpl = ('    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "%(typecodes)s:%(typename)s.%(name)s"%(parselist)s))\n'
                     '        return NULL;\n')
        parsedict = {'typecodes': info.parsestr,
                     'typename':  objname,
                     'name':      fixname(methobj.name),
                     'parselist': info.get_parselist()}

        dict['parseargs'] = parsetmpl % parsedict
        dict['varlist'] = info.get_kwlist() + info.get_varlist()
        dict['extraparams'] = ', PyObject *args, PyObject *kwargs'
        flags = 'METH_VARARGS|METH_KEYWORDS'
    else:
        dict['parseargs'] = ''
        dict['varlist']    = info.get_varlist()
        dict['extraparams'] = ''
        flags = 'METH_NOARGS'

    fp.write(boxedmethtmpl % dict)

    return flags

def write_pointer_constructor(objname, typecode, funcobj, fp=sys.stdout):
    info = argtypes.WrapperInfo()

    dict = {
	'name':    funcobj.name,
	'cname':   funcobj.c_name,
	'typename': objname,
        'typecode': typecode,
    }

    for ptype, pname, pdflt, pnull in funcobj.params:
	if pdflt and '|' not in info.parsestr:
	    info.add_parselist('|', [], [])
	handler = argtypes.matcher.get(ptype)
        spec = handler.write_param(ptype, pname, pdflt, pnull, info)

    dict['varlist']    = info.get_kwlist() + info.get_varlist()
    dict['typecodes']  = info.parsestr
    dict['parselist']  = info.get_parselist()
    dict['codebefore'] = string.replace(info.get_codebefore(),
                                       'return NULL;', 'return -1;')
    dict['codeafter']  = string.replace(info.get_codeafter(),
                                       'return NULL;', 'return -1;')
    dict['arglist']    = info.get_arglist()
    fp.write(pointerconsttmpl % dict)

def write_pointer_getsets(parser, pointerobj, overrides, fp=sys.stdout):
    typecode = pointerobj.typecode
    uline = string.replace(typecode, '_TYPE_', '_', 1)
    getsets_name = string.lower(uline) + '_getsets'
    getterprefix = '_wrap_' + string.lower(uline) + '__get_'
    setterprefix = '_wrap_' + string.lower(uline) + '__set_'

    if not pointerobj.fields:
        return '0'

    getsets = []
    for ftype, fname in pointerobj.fields:
        attrname = pointerobj.c_name + '.' + fname
        gettername = '0'
        settername = '0'
        if overrides.attr_is_overriden(attrname):
            lineno, filename = overrides.getstartline(attrname)
            code = overrides.attr_override(attrname)
            fp.setline(lineno, filename)
            fp.write(code)
            fp.resetline()
            if string.find(code, getterprefix + fname) >= 0:
                gettername = getterprefix + fname
            if string.find(code, setterprefix + fname) >= 0:
                settername = setterprefix + fname
        if gettername == '0':
            try:
                funcname = getterprefix + fname
                info =  argtypes.WrapperInfo()
                handler = argtypes.matcher.get(ftype)
                handler.write_return(ftype, info)
                field = 'pyg_pointer_get(self, ' + pointerobj.c_name + ')->' + fname
                fp.write(boxedgettertmpl %
                         { 'funcname': funcname,
                           'varlist': info.varlist,
                           'field': field,
                           'codeafter': info.get_codeafter() })
                gettername = funcname
            except:
                sys.stderr.write("Could not write check for %s.%s: %s\n"
                                 % (pointerobj.c_name, fname, exc_info()))
        if gettername != '0' or settername != '0':
            getsets.append('    { "%s", (getter)%s, (setter)%s },\n' %
                           (fixname(fname), gettername, settername))

    if not getsets:
        return '0'
    fp.write('static PyGetSetDef %s[] = {\n' % getsets_name)
    for getset in getsets:
        fp.write(getset)
    fp.write('    { NULL, (getter)0, (setter)0 },\n')
    fp.write('};\n\n')

    return getsets_name

def write_pointer(parser, pointerobj, overrides, fp=sys.stdout):
    fp.write('\n/* ----------- ' + pointerobj.c_name + ' ----------- */\n\n')
    constructor = parser.find_constructor(pointerobj, overrides)
    methods = []
    initfunc = '0'
    if constructor:
	try:
            if overrides.is_overriden(constructor.c_name):
                lineno, filename = overrides.getstartline(constructor.c_name)
                fp.setline(lineno, filename)
                fp.write(overrides.override(constructor.c_name))
                fp.resetline()
                fp.write('\n\n')
            else:
                write_pointer_constructor(pointerobj.c_name, pointerobj.typecode,
                                          constructor, fp)
            initfunc = '_wrap_' + constructor.c_name
	except:
	    sys.stderr.write('Could not write constructor for %s: %s\n'
                             % (pointerobj.c_name, exc_info()))
    for meth in parser.find_methods(pointerobj):
        if overrides.is_ignored(meth.c_name):
            continue
	try:
            methtype = 'METH_VARARGS'
            if overrides.is_overriden(meth.c_name):
                lineno, filename = overrides.getstartline(meth.c_name)
                fp.setline(lineno, filename)
                fp.write(overrides.override(meth.c_name))
                fp.resetline()
                fp.write('\n\n')
                if overrides.wants_kwargs(meth.c_name):
                    methtype = methtype + '|METH_KEYWORDS'
                elif overrides.wants_noargs(meth.c_name):
                    methtype = 'METH_NOARGS'
            else:
                methtype = write_pointer_method(pointerobj.c_name, meth, fp)
	    methods.append(methdeftmpl % { 'name':  fixname(meth.name),
					   'cname': '_wrap_' + meth.c_name,
					   'flags': methtype})
	except:
	    sys.stderr.write('Could not write method %s.%s: %s\n'
                             % (pointerobj.c_name, meth.name, exc_info()))

    # write the PyMethodDef structure
    methods.append('    { NULL, NULL, 0 }\n')
    fp.write('static PyMethodDef _Py' + pointerobj.c_name + '_methods[] = {\n')
    fp.write(string.join(methods, ''))
    fp.write('};\n\n')

    # write the type template
    dict = { 'typename': pointerobj.c_name, 'initfunc': initfunc }

    if overrides.modulename:
        dict['classname'] = '%s.%s' % (overrides.modulename, pointerobj.name)
    else:
        dict['classname'] = pointerobj.name

    dict['getsets'] = write_pointer_getsets(parser, pointerobj, overrides, fp)
    dict['methods'] = '_Py' + dict['typename'] + '_methods'

    # handle slots ...
    for slot in slots_list:
        slotname = '%s.%s' % (pointerobj.c_name, slot)
        slotfunc = '_wrap_%s_%s' % \
                   (string.lower(string.replace(pointerobj.typecode,
                                                '_TYPE_', '_', 1)), slot)
        if slot[:6] == 'tp_as_':
            slotfunc = '&' + slotfunc
        if overrides.slot_is_overriden(slotname):
            lineno, filename = overrides.getstartline(slotname)
            fp.setline(lineno, filename)
            fp.write(overrides.slot_override(slotname))
            fp.resetline()
            fp.write('\n\n')
            dict[slot] = slotfunc
        else:
            dict[slot] = '0'

    fp.write(pointertmpl % dict)


def write_functions(parser, overrides, prefix, fp=sys.stdout):
    fp.write('\n/* ----------- functions ----------- */\n\n')
    functions = []
    for func in parser.find_functions():
        if overrides.is_ignored(func.c_name):
            continue
	try:
            methtype = 'METH_VARARGS'
            if overrides.is_overriden(func.c_name):
                lineno, filename = overrides.getstartline(func.c_name)
                fp.setline(lineno, filename)
                fp.write(overrides.override(func.c_name))
                fp.resetline()
                fp.write('\n\n')
                if overrides.wants_kwargs(func.c_name):
                    methtype = methtype + '|METH_KEYWORDS'
                elif overrides.wants_noargs(func.c_name):
                    methtype = 'METH_NOARGS'
            else:
                methtype = write_function(func, fp)
	    functions.append(methdeftmpl % { 'name':  func.name,
                                             'cname': '_wrap_' + func.c_name,
                                             'flags': methtype})
	except:
	    sys.stderr.write('Could not write function %s: %s\n'
                             %(func.name, exc_info()))
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
        if enum.typecode is None:
            for nick, value in enum.values:
                fp.write('    PyModule_AddIntConstant(module, pyg_constant_strip_prefix("%s", strip_prefix), %s);\n'
                         % (value, value))
        else:
            if enum.deftype == 'enum':
                fp.write('    pyg_enum_add_constants(module, %s, strip_prefix);\n'
                         % (enum.typecode,))
            else:
                fp.write('    pyg_flags_add_constants(module, %s, strip_prefix);\n'
                         % (enum.typecode,))
    fp.write('}\n\n')

def write_source(parser, overrides, prefix, fp=FileOutput(sys.stdout)):
    fp.write('/* -*- Mode: C; c-basic-offset: 4 -*- */\n\n')
    fp.write('#include <Python.h>\n\n\n')
    fp.write(overrides.get_headers())
    fp.resetline()
    fp.write('\n\n')
    fp.write('/* ---------- types from other modules ---------- */\n')
    for module, pyname, cname in overrides.get_imports():
        fp.write('static PyTypeObject *_%s;\n' % cname)
        fp.write('#define %s (*_%s)\n' % (cname, cname))
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
    for pointer in parser.pointers:
        write_pointer(parser, pointer, overrides, fp)
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
    imports = overrides.get_imports()[:]
    if imports:
        bymod = {}
        for module, pyname, cname in imports:
            bymod.setdefault(module, []).append((pyname, cname))
        fp.write('    PyObject *module;\n\n')
        for module in bymod:
            fp.write('    if ((module = PyImport_ImportModule("%s")) != NULL) {\n' % module)
            fp.write('        PyObject *moddict = PyModule_GetDict(module);\n\n')
            for pyname, cname in bymod[module]:
                fp.write('        _%s = (PyTypeObject *)PyDict_GetItemString(moddict, "%s");\n' % (cname, pyname))
            fp.write('    } else {\n')
            fp.write('        Py_FatalError("could not import %s");\n' %module)
            fp.write('        return;\n')
            fp.write('    }\n')
        fp.write('\n')
    fp.write(overrides.get_init() + '\n')
    fp.resetline()

    for boxed in parser.boxes:
        fp.write('    pyg_register_boxed(d, "' + boxed.name +
                 '", ' + boxed.typecode + ', &Py' + boxed.c_name + '_Type);\n')
    for pointer in parser.pointers:
        fp.write('    pyg_register_pointer(d, "' + pointer.name +
                 '", ' + pointer.typecode + ', &Py' + pointer.c_name + '_Type);\n')
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
    for pointer in parser.pointers:
        argtypes.matcher.register_pointer(pointer.c_name, pointer.typecode)
    for obj in parser.objects:
        argtypes.matcher.register_object(obj.c_name, obj.parent, obj.typecode)
    for obj in parser.interfaces:
        argtypes.matcher.register_object(obj.c_name, None, obj.typecode)
    for enum in parser.enums:
	if enum.deftype == 'flags':
	    argtypes.matcher.register_flag(enum.c_name, enum.typecode)
	else:
	    argtypes.matcher.register_enum(enum.c_name, enum.typecode)

def main():
    o = override.Overrides()
    prefix = 'pygtk'
    outfilename = None
    opts, args = getopt.getopt(sys.argv[1:], "o:p:r:t:",
                        ["override=", "prefix=", "register=", "outfilename=",
                         "load-types="])
    for opt, arg in opts:
        if opt in ('-o', '--override'):
            o = override.Overrides(arg)
        elif opt in ('-p', '--prefix'):
            prefix = arg
        elif opt in ('-r', '--register'):
            p = defsparser.DefsParser(arg)
            p.startParsing()
            register_types(p)
            del p
        elif opt == '--outfilename':
            outfilename = arg
        elif opt in ('-t', '--load-types'):
            globals = {}
            execfile(arg, globals)
    if len(args) < 1:
        sys.stderr.write(
            'usage: codegen.py [-o overridesfile] [-p prefix] defsfile\n')
        sys.exit(1)
    p = defsparser.DefsParser(args[0])
    if not outfilename:
        outfilename = os.path.splitext(args[0])[0] + '.c'
    p.startParsing()
    register_types(p)
    write_source(p, o, prefix, FileOutput(sys.stdout, outfilename))

if __name__ == '__main__':
    main()
