# -*- Mode: Python; py-indent-offset: 4 -*-
import sys
import string
import traceback

_conv_special_cases = {
    'GObject': '_G_OBJECT',
    'GtkCList': '_GTK_CLIST',
    'GtkCTree': '_GTK_CTREE',
    'GtkCTreePos': '_GTK_CTREE_POS',
    'GtkCTreeLineStyle': '_GTK_CTREE_LINE_STYLE',
    'GtkCTreeExpanderStyle': '_GTK_CTREE_EXPANDER_STYLE',
    'GtkCTreeExpansionStyle': '_GTK_CTREE_EXPANSION_STYLE',
    'GnomeRootWin': '_GNOME_ROOTWIN',
    'GnomeAppBar': '_GNOME_APPBAR',
    'GnomeDEntryEdit': '_GNOME_DENTRY_EDIT',
    'PyGtkTreeModel': '_PYGTK_TREE_MODEL'
}

def _to_upper_str(str):
    """Converts a typename to the equivalent upercase and underscores
    name.  This is used to form the type conversion macros and enum/flag
    name variables"""
    if _conv_special_cases.has_key(str):
	return _conv_special_cases[str]
    ret = []
    while str:
	if len(str) > 1 and str[0] in string.uppercase and \
	   str[1] in string.lowercase:
	    ret.append("_" + str[:2])
	    str = str[2:]
	elif len(str) > 3 and \
	     str[0] in string.lowercase and \
	     str[1] in 'HV' and \
	     str[2] in string.uppercase and \
	     str[3] in string.lowercase:
	    ret.append(str[0] + "_" + str[1:3])
	    str = str[3:]
	elif len(str) > 2 and \
	     str[0] in string.lowercase and \
	     str[1] in string.uppercase and \
	     str[2] in string.uppercase:
	    ret.append(str[0] + "_" + str[1])
	    str = str[2:]
	else:
	    ret.append(str[0])
	    str = str[1:]
    return string.upper(string.join(ret, ''))

def _enum_name(typename):
    """create a GTK_TYPE_* name from the given type"""
    part = _to_upper_str(typename)
    second_underscore = string.index(part, '_', 1)
    return part[1:second_underscore] + '_TYPE' + part[second_underscore:]


class VarList:
    """Nicely format a C variable list"""
    def __init__(self):
	self.vars = {}
    def add(self, ctype, name):
	if self.vars.has_key(ctype):
	    self.vars[ctype] = self.vars[ctype] + (name,)
	else:
	    self.vars[ctype] = (name,)
    def __str__(self):
	ret = []
	for type in self.vars.keys():
	    ret.append('    ')
	    ret.append(type)
	    ret.append(' ')
	    ret.append(string.join(self.vars[type], ', '))
	    ret.append(';\n')
	if ret:
            ret.append('\n')
            return string.join(ret, '')
	return ''

class ArgType:
    def write_param(self, ptype, pname, pdflt, pnull, varlist, parselist,
		    extracode, arglist):
	"""Returns the letter code for PyArg_ParseTuple.
	Variables can be defined in varlist, and extra code can be
	appended to extracode"""
	raise RuntimeError, "this is an abstract class"
    def write_return(self, ptype, varlist):
	"""Returns a pattern containing %(func)s that will do the return"""
	raise RuntimeError, "this is an abstract class"

class NoneArg(ArgType):
    def write_return(self, ptype, varlist):
	return '    %(func)s;\n' + \
	       '    Py_INCREF(Py_None);\n' + \
	       '    return Py_None;'

class StringArg(ArgType):
    def write_param(self, ptype, pname, pdflt, pnull, varlist, parselist,
		    extracode, arglist):
	if pdflt:
            if pdflt != 'NULL': pdflt = '"' + pdflt + '"'
	    varlist.add('char', '*' + pname + ' = ' + pdflt)
	else:
	    varlist.add('char', '*' + pname)
	parselist.append('&' + pname)
	arglist.append(pname)
	if pnull:
	    return 'z'
	else:
	    return 's'
    def write_return(self, ptype, varlist):
	if ptype in ('const-gchar*', 'const-char*', 'static_string'):
	    varlist.add('const gchar', '*ret')
	    return '    ret = %(func)s;\n' + \
		   '    if (ret)\n' + \
		   '        return PyString_FromString(ret);\n' + \
		   '    Py_INCREF(Py_None);\n' + \
		   '    return Py_None;'
	else:
	    # have to free result ...
	    varlist.add('gchar', '*ret')
	    return '    ret = %(func)s;\n' + \
		   '    if (ret) {\n' + \
		   '        PyObject *py_ret = PyString_FromString(ret);\n' + \
		   '        g_free(ret);\n' + \
		   '        return py_ret;\n' + \
		   '    }\n' + \
		   '    Py_INCREF(Py_None);\n' + \
		   '    return Py_None;'

class UCharArg(ArgType):
    # allows strings with embedded NULLs.
    def write_param(self, ptype, pname, pdflt, pnull, varlist, parselist,
		    extracode, arglist):
	if pdflt:
	    varlist.add('guchar', '*' + pname + ' = "' + pdflt + '"')
	else:
	    varlist.add('guchar', '*' + pname)
        varlist.add('int', pname + '_len')
	parselist.append('&' + pname)
        parselist.append('&' + pname + '_len')
	arglist.append(pname)
	if pnull:
	    return 'z#'
	else:
	    return 's#'

class CharArg(ArgType):
    def write_param(self, ptype, pname, pdflt, pnull, varlist, parselist,
		    extracode, arglist):
	if pdflt:
	    varlist.add('char', pname + " = '" + pdflt + "'")
	else:
	    varlist.add('char', pname)
	parselist.append('&' + pname)
	arglist.append(pname)
	return 'c'
    def write_return(self, ptype, varlist):
	varlist.add('gchar', 'ret[2]')
	return '    ret[0] = %(func)s;\n' + \
	       "    ret[1] = '\0';\n" + \
	       '    return PyString_FromString(ret);'

class IntArg(ArgType):
    def write_param(self, ptype, pname, pdflt, pnull, varlist, parselist,
		    extracode, arglist):
	if pdflt:
	    varlist.add('int', pname + ' = ' + pdflt)
	else:
	    varlist.add('int', pname)
	parselist.append('&' + pname)
	arglist.append(pname)
	return 'i'
    def write_return(self, ptype, varlist):
	return '    return PyInt_FromLong(%(func)s);'

class DoubleArg(ArgType):
    def write_param(self, ptype, pname, pdflt, pnull, varlist, parselist,
		    extracode, arglist):
	if pdflt:
	    varlist.add('double', pname + ' = ' + pdflt)
	else:
	    varlist.add('double', pname)
	parselist.append('&' + pname)
	arglist.append(pname)
	return 'd'
    def write_return(self, ptype, varlist):
	return '    return PyFloat_FromDouble(%(func)s);'

class FileArg(ArgType):
    nulldflt = '    if (py_%(name)s == Py_None)\n' + \
	       '        %(pname)s = NULL;\n' + \
	       '    else if (py_%(name)s && PyFile_Check(py_%(name)s)\n' + \
	       '        %s = PyFile_AsFile(py_%(name)s);\n' + \
	       '    else if (py_%(name)s) {\n' + \
	       '        PyErr_SetString(PyExc_TypeError, "%(name)s should be a file object or None");\n' + \
	       '        return NULL;\n' + \
	       '    }\n'
    null = '    if (py_%(name)s && PyFile_Check(py_%(name)s)\n' + \
	   '        %(name)s = PyFile_AsFile(py_%(name)s);\n' + \
	   '    else if (py_%(name)s != Py_None) {\n' + \
	   '        PyErr_SetString(PyExc_TypeError, "%(name)s should be a file object or None");\n' + \
	   '        return NULL;\n' + \
	   '    }\n'
    dflt = '    if (py_%(name)s)\n' + \
	   '        %(name)s = PyFile_AsFile(py_%(name)s);\n'
    def write_param(self, ptype, pname, pdflt, pnull, varlist, parselist,
		    extracode, arglist):
	if pnull:
	    if pdflt:
		varlist.add('FILE', '*' + pname + ' = ' + pdflt)
		varlist.add('PyObject', '*py_' + pname + ' = NULL')
		parselist.append('&py_' + pname)
		extracode.append(self.nulldflt % {'name':pname})
	    else:
		varlist.add('FILE', '*' + pname + ' = NULL')
		varlist.add('PyObject', '*py_' + pname)
		parselist.append('&py_' + pname)
		extracode.append(self.null & {'name':pname})
		arglist.appned(pname)
	    return 'O'
	else:
	    if pdflt:
		varlist.add('FILE', '*' + pname + ' = ' + pdflt)
		varlist.add('PyObject', '*py_' + pname + ' = NULL')
		parselist.append('&PyFile_Type')
		parselist.append('&py_' + pname)
		extracode.append(self.dflt % {'name':pname})
		arglist.append(pname)
	    else:
		varlist.add('PyObject', '*' + pname)
		parselist.append('&PyFile_Type')
		parselist.append('&' + pname)
		arglist.append('PyFile_AsFile(' + pname + ')')
	    return 'O!'
    def write_return(self, ptype, varlist):
	varlist.add('FILE', '*ret')
	return '    ret = %(func)s;\n' + \
	       '    if (ret)\n' + \
	       '        return PyFile_FromFile(ret, "", "", fclose);\n' + \
	       '    Py_INCREF(Py_None);\n' + \
	       '    return Py_None;'

class EnumArg(ArgType):
    enum = '    if (pyg_enum_get_value(%(typecode)s, py_%(name)s, (gint *)&%(name)s))\n' + \
	   '        return NULL;\n'
    def __init__(self, enumname):
	self.enumname = enumname
	self.typecode = _enum_name(enumname)
    def write_param(self, ptype, pname, pdflt, pnull, varlist, parselist,
		    extracode, arglist):
	if pdflt:
	    varlist.add(self.enumname, pname + ' = ' + pdflt)
	else:
	    varlist.add(self.enumname, pname)
	varlist.add('PyObject', '*py_' + pname + ' = NULL')
	parselist.append('&py_' + pname)
	extracode.append(self.enum % {'typecode':self.typecode, 'name':pname})
	arglist.append(pname)
	return 'O'
    def write_return(self, ptype, varlist):
	return '    return PyInt_FromLong(%(func)s);'

class FlagsArg(ArgType):
    flag = '    if (pyg_flags_get_value(%(typecode)s, py_%(name)s, (gint *)&%(name)s))\n' + \
	   '        return NULL;\n'
    def __init__(self, flagname):
	self.flagname = flagname
	self.typecode = _enum_name(flagname)
    def write_param(self, ptype, pname, pdflt, pnull, varlist, parselist,
		    extracode, arglist):
	if pdflt:
	    varlist.add(self.flagname, pname + ' = ' + pdflt)
	else:
	    varlist.add(self.flagname, pname)
	varlist.add('PyObject', '*py_' + pname + ' = NULL')
	parselist.append('&py_' + pname)
	extracode.append(self.flag % {'typecode':self.typecode, 'name':pname})
	arglist.append(pname)
	return 'O'
    def write_return(self, ptype, varlist):
	return '    return PyInt_FromLong(%(func)s);'

class ObjectArg(ArgType):
    # should change these checks to more typesafe versions that check
    # a little further down in the class heirachy.
    nulldflt = '    if ((PyObject *)py_%(name)s == Py_None)\n' + \
	       '        %(name)s = NULL;\n' + \
	       '    else if (py_%(name)s && pygobject_check(py_%(name)s, &Py%(type)s_Type))\n' + \
	       '        %(name)s = %(cast)s(py_%(name)s->obj);\n' + \
	       '    else if (py_%(name)s) {\n' + \
	       '        PyErr_SetString(PyExc_TypeError, "%(name)s should be a %(type)s or None");\n' + \
	       '        return NULL;\n' + \
	       '    }\n'
    null = '    if (py_%(name)s && pygobject_check(py_%(name)s, &Py%(type)s_Type))\n' + \
	   '        %(name)s = %(cast)s(py_%(name)s->obj);\n' + \
	   '    else if ((PyObject *)py_%(name)s != Py_None) {\n' + \
	   '        PyErr_SetString(PyExc_TypeError, "%(name)s should be a %(type)s or None");\n' + \
	   '        return NULL;\n' + \
	   '    }\n'
    dflt = '    if (py_%(name)s && pygobject_check(py_%(name)s, &Py%(type)s_Type))\n' + \
	   '        %(name)s = %(cast)s(py_%(name)s->obj);\n' + \
	   '    else if (py_%(name)s) {\n' + \
	   '        PyErr_SetString(PyExc_TypeError, "%(name)s should be a %(type)s");\n' + \
	   '        return NULL;\n' + \
	   '    }\n'
    check = '    if (!pygobject_check(%(name)s, &Py%(type)s_Type)) {\n' + \
	    '        PyErr_SetString(PyExc_TypeError, "%(name)s should be a %(type)s");\n' + \
	    '        return NULL;\n' + \
	    '    }\n'
    def __init__(self, objname, parent):
	self.objname = objname
	self.cast = _to_upper_str(objname)[1:]
        self.parent = parent
    def write_param(self, ptype, pname, pdflt, pnull, varlist, parselist,
		    extracode, arglist):
	if pnull:
	    if pdflt:
		varlist.add(self.objname, '*' + pname + ' = ' + pdflt)
		varlist.add('PyGObject', '*py_' + pname + ' = NULL')
		parselist.append('&py_' + pname)
		extracode.append(self.nulldflt % {'name':pname,
						  'cast':self.cast,
						  'type':self.objname}) 
	    else:
		varlist.add(self.objname, '*' + pname + ' = NULL')
		varlist.add('PyGObject', '*py_' + pname)
		parselist.append('&py_' + pname)
		extracode.append(self.null % {'name':pname,
					      'cast':self.cast,
					      'type':self.objname}) 
            arglist.append(pname)
	    return 'O'
	else:
	    if pdflt:
		varlist.add(self.objname, '*' + pname + ' = ' + pdflt)
		varlist.add('PyGObject', '*py_' + pname + ' = NULL')
		parselist.append('&py_' + pname)
		extracode.append(self.dflt % {'name':pname,
					      'cast':self.cast,
					      'type':self.objname}) 
		arglist.append(pname)
	    else:
		varlist.add('PyGObject', '*' + pname)
		parselist.append('&' + pname)
		extracode.append(self.check % {'name':pname,
					       'cast':self.cast,
					       'type':self.objname}) 
		arglist.append('%s(%s->obj)' % (self.cast, pname))
	    return 'O'
    def write_return(self, ptype, varlist):
	return '    /* pygobject_new handles NULL checking */\n' + \
	       '    return pygobject_new((GObject *)%(func)s);'

class BoxedArg(ArgType):
    # haven't done support for default args.  Is it needed?
    check = '    if (pyg_boxed_check(py_%(name)s, %(typecode)s))\n' + \
           '        %(name)s = pyg_boxed_get(py_%(name)s, %(typename)s);\n' + \
	   '    else {\n' + \
	   '        PyErr_SetString(PyExc_TypeError, "%(name)s should be a %(typename)s");\n' + \
	   '        return NULL;\n' + \
	   '    }\n'
    null = '    if (pyg_boxed_check(py_%(name)s, %(typecode)s))\n' + \
           '        %(name)s = pyg_boxed_get(py_%(name)s, %(typename)s);\n' + \
	   '    else if (py_%(name)s != Py_None) {\n' + \
	   '        PyErr_SetString(PyExc_TypeError, "%(name)s should be a %(typename)s");\n' + \
	   '        return NULL;\n' + \
	   '    }\n'
    def __init__(self, ptype):
	self.typename = ptype
	self.typecode = _enum_name(ptype)
    def write_param(self, ptype, pname, pdflt, pnull, varlist, parselist,
		    extracode, arglist):
	if pnull:
            varlist.add(ptype[:-1], '*' + pname + ' = NULL')
	    varlist.add('PyObject', '*py_' + pname + ' = Py_None')
	    parselist.append('&py_' + pname)
	    extracode.append(self.null % {'name':  pname,
                                          'typename': self.typename,
                                          'typecode': self.typecode})
	    arglist.append(pname)
	    return 'O'
	else:
            varlist.add(ptype[:-1], '*' + pname + ' = NULL')
	    varlist.add('PyObject', '*py_' + pname)
	    parselist.append('&py_' + pname)
	    extracode.append(self.check % {'name':  pname,
                                           'typename': self.typename,
                                           'typecode': self.typecode})
	    arglist.append(pname)
	    return 'O'
    def write_return(self, ptype, varlist):
	return '    /* pyg_boxed_new handles NULL checking */\n' + \
	       '    return pyg_boxed_new(' + self.typecode + ', %(func)s, TRUE, TRUE);'

class CustomBoxedArg(ArgType):
    # haven't done support for default args.  Is it needed?
    null = '    if (%(check)s(py_%(name)s))\n' + \
	   '        %(name)s = %(get)s(py_%(name)s);\n' + \
	   '    else if (py_%(name)s != Py_None) {\n' + \
	   '        PyErr_SetString(PyExc_TypeError, "%(name)s should be a %(type)s");\n' + \
	   '        return NULL;\n' + \
	   '    }\n'
    def __init__(self, ptype, pytype, getter, new):
	self.pytype = pytype
	self.getter = getter
        self.checker = 'Py' + ptype + '_Check'
	self.new = new
    def write_param(self, ptype, pname, pdflt, pnull, varlist, parselist,
		    extracode, arglist):
	if pnull:
            varlist.add(ptype[:-1], '*' + pname + ' = NULL')
	    varlist.add('PyObject', '*py_' + pname + ' = Py_None')
	    parselist.append('&py_' + pname)
	    extracode.append(self.null % {'name':  pname,
                                          'get':   self.getter,
                                          'check': self.checker,
                                          'type':  ptype[:-1]})
	    arglist.append(pname)
	    return 'O'
	else:
	    varlist.add('PyObject', '*' + pname)
	    parselist.append('&' + self.pytype)
	    parselist.append('&' + pname)
	    arglist.append(self.getter + '(' + pname + ')')
	    return 'O!'
    def write_return(self, ptype, varlist):
        varlist.add(ptype[:-1], '*ret')
	return '    ret = %(func)s;\n' + \
	       '    if (ret)\n' + \
	       '        return ' + self.new + '(ret);\n' + \
	       '    Py_INCREF(Py_None);\n' + \
	       '    return Py_None;'

class AtomArg(IntArg):
    def write_return(self, ptype, varlist):
        return '    return PyGdkAtom_New(%(func)s);'

class GTypeArg(ArgType):
    gtype = '    %(name)s = pyg_type_from_object(py_%(name)s);\n' + \
            '    if (!%(name)s)\n' + \
            '        return NULL;\n'
    def write_param(self, ptype, pname, pdflt, pnull, varlist, parselist,
		    extracode, arglist):
        varlist.add(self.enumname, pname)
	varlist.add('PyObject', '*py_' + pname + ' = NULL')
	parselist.append('&py_' + pname)
	extracode.append(self.gtype % {'name': pname})
	arglist.append(pname)
	return 'O'
    def write_return(self, ptype, varlist):
	return '    return PyInt_FromLong(%(func)s);'

# simple GError handler.
# XXXX - must get codegen to handle real GError stuff
class GErrorArg(ArgType):
    def write_param(self, ptype, pname, pdflt, pnull, varlist, parselist,
		    extracode, arglist):
        arglist.append('NULL')
        return ''

class GtkTreePathArg(ArgType):
    # haven't done support for default args.  Is it needed?
    null = '    if (PyTuple_Check(py_%(name)s))\n' + \
	   '        %(name)s = pygtk_tree_path_from_pyobject(py_%(name)s);\n' + \
	   '    else if (py_%(name)s != Py_None) {\n' + \
	   '        PyErr_SetString(PyExc_TypeError, "%(name)s should be a GtkTreePath or None");\n' + \
	   '        return NULL;\n' + \
	   '    }\n'
    def __init__(self):
        pass
    def write_param(self, ptype, pname, pdflt, pnull, varlist, parselist,
		    extracode, arglist):
	if pnull:
            varlist.add('GtkTreePath', '*' + pname + ' = NULL')
	    varlist.add('PyObject', '*py_' + pname + ' = Py_None')
	    parselist.append('&py_' + pname)
	    extracode.append(self.null % {'name':  pname,
                                          'type':  ptype[:-1]})
	    arglist.append(pname)
	    return 'O'
	else:
	    varlist.add('PyObject', '*' + pname)
	    parselist.append('&PyTuple_Type')
	    parselist.append('&' + pname)
	    arglist.append('pygtk_tree_path_from_pyobject(' + pname + ')')
	    return 'O!'
    def write_return(self, ptype, varlist):
        varlist.add('GtkTreePath', '*ret')
        varlist.add('PyObject', '*py_ret')
	return '    ret = %(func)s;\n' + \
	       '    if (ret) {\n' + \
               '        py_ret = pygtk_tree_path_to_pyobject(ret);\n' + \
               '        gtk_tree_path_free(ret);\n' + \
	       '        return py_ret;\n' + \
               '    }\n' + \
	       '    Py_INCREF(Py_None);\n' + \
	       '    return Py_None;'

class ArgMatcher:
    def __init__(self):
	self.argtypes = {}

    def register(self, ptype, handler):
	self.argtypes[ptype] = handler
    def register_enum(self, ptype):
	self.register(ptype, EnumArg(ptype))
    def register_flag(self, ptype):
	self.register(ptype, FlagsArg(ptype))
    def register_object(self, ptype, parent):
        oa = ObjectArg(ptype, parent)
        self.register(ptype, oa)  # in case I forget the * in the .defs
	self.register(ptype+'*', oa)
        if ptype == 'GdkPixmap':
            # hack to handle GdkBitmap synonym.
            self.register('GdkBitmap', oa)
            self.register('GdkBitmap*', oa)
    def register_boxed(self, ptype):
	self.register(ptype+'*', BoxedArg(ptype))
    def register_custom_boxed(self, ptype, pytype, getter, new):
	self.register(ptype+'*', CustomBoxedArg(ptype, pytype, getter, new))

    def get(self, ptype):
	return self.argtypes[ptype]
    def object_is_a(self, otype, parent):
        if otype == None: return 0
        if otype == parent: return 1
        if not self.argtypes.has_key(otype): return 0
        return self.object_is_a(self.get(otype).parent, parent)

matcher = ArgMatcher()

arg = NoneArg()
matcher.register(None, arg)
matcher.register('none', arg)

arg = StringArg()
matcher.register('char*', arg)
matcher.register('gchar*', arg)
matcher.register('const-char*', arg)
matcher.register('const-gchar*', arg)
matcher.register('string', arg)
matcher.register('static_string', arg)

arg = UCharArg()
matcher.register('unsigned-char*', arg)
matcher.register('guchar*', arg)

arg = CharArg()
matcher.register('char', arg)
matcher.register('gchar', arg)
matcher.register('guchar', arg)

arg = IntArg()
matcher.register('int', arg)
matcher.register('gint', arg)
matcher.register('guint', arg)
matcher.register('short', arg)
matcher.register('gshort', arg)
matcher.register('gushort', arg)
matcher.register('long', arg)
matcher.register('glong', arg)
matcher.register('gulong', arg)
matcher.register('gboolean', arg)

matcher.register('guint8', arg)
matcher.register('gint8', arg)
matcher.register('guint16', arg)
matcher.register('gint16', arg)
matcher.register('guint32', arg)
matcher.register('gint32', arg)
matcher.register('GtkType', arg)

arg = DoubleArg()
matcher.register('double', arg)
matcher.register('gdouble', arg)
matcher.register('float', arg)
matcher.register('gfloat', arg)

arg = FileArg()
matcher.register('FILE*', arg)

# enums, flags, objects

matcher.register_custom_boxed('GdkFont', 'PyGdkFont_Type',
                              'PyGdkFont_Get', 'PyGdkFont_New')
matcher.register_custom_boxed('GdkColor', 'PyGdkColor_Type',
                              'PyGdkColor_Get', 'PyGdkColor_New')
matcher.register_custom_boxed('GdkEvent', 'PyGdkEvent_Type',
                              'PyGdkEvent_Get', 'PyGdkEvent_New')
matcher.register_custom_boxed('GtkSelectionData', 'PyGtkSelectionData_Type',
                              'PyGtkSelectionData_Get',
                              'PyGtkSelectionData_New')
matcher.register_custom_boxed('GtkCTreeNode', 'PyGtkCTreeNode_Type',
                              'PyGtkCTreeNode_Get', 'PyGtkCTreeNode_New')
matcher.register_custom_boxed('GdkDevice', 'PyGdkDevice_Type',
                              'PyGdkDevice_Get', 'PyGdkDevice_New')
matcher.register_custom_boxed('GtkTextIter', 'PyGtkTextIter_Type',
                              'PyGtkTextIter_Get', 'PyGtkTextIter_New')
matcher.register('const-GtkTextIter*', matcher.get('GtkTextIter*'))
matcher.register_custom_boxed('GtkTreeIter', 'PyGtkTreeIter_Type',
                              'PyGtkTreeIter_Get', 'PyGtkTreeIter_New')
matcher.register('const-GtkTreeIter*', matcher.get('GtkTreeIter*'))

matcher.register('GdkAtom', AtomArg())

matcher.register('GType', GTypeArg())
matcher.register('GtkType', GTypeArg())

matcher.register('GError**', GErrorArg())
matcher.register('GtkTreePath*', GtkTreePathArg())

matcher.register_object('GObject', None)

del arg
