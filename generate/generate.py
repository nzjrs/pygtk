import sys, getopt
import os
import string
import scmexpr
try:
	from cStringIO import StringIO
except ImportError:
	from StringIO import StringIO
# for fnmatchcase -- filename globbing
import fnmatch

TRUE  = 1
FALSE = 0

funcDefTmpl = '    { "%s", _wrap_%s, 1 },\n'
funcLeadTmpl = 'static PyObject *\n_wrap_%s(PyObject *self, PyObject *args)\n{\n'
enumCodeTmpl = '''    if (PyGtkEnum_get_value(%s, %s, (gint *)&%s))
        return NULL;\n'''
flagCodeTmpl = '''    if (PyGtkFlag_get_value(%s, %s, (gint *)&%s))
        return NULL;\n'''
setVarTmpl = '''  if (py_%s)
        %s = %s;\n'''
getTypeTmpl = '''static PyObject *\n_wrap_%s(PyObject *self, PyObject *args)\n{
    if (!PyArg_ParseTuple(args, ":%s"))
        return NULL;
    return PyInt_FromLong(%s());
}\n\n'''

nullokTmpl = '''    if (PyGtk_Check(py_%s))
        %s = %s(PyGtk_Get(py_%s));
    else if (py_%s != Py_None) {
        PyErr_SetString(PyExc_TypeError, "%s argument must be a %s or None");
	return NULL;
    }\n'''

nullokBoxedTmpl = '''    if (Py%s_Check(py_%s))
        %s = %s(py_%s);
    else if (py_%s != Py_None) {
        PyErr_SetString(PyExc_TypeError, "%s argument must be a %s or None");
	return NULL;
    }\n'''

convSpecialCases = {
	'GtkCList': '_GTK_CLIST',
	'GtkCTree': '_GTK_CTREE',
	'GtkCTreePos': '_GTK_CTREE_POS',
	'GtkCTreeLineStyle': '_GTK_CTREE_LINE_STYLE',
	'GtkCTreeExpanderStyle': '_GTK_CTREE_EXPANDER_STYLE',
	'GtkCTreeExpansionStyle': '_GTK_CTREE_EXPANSION_STYLE',
	'GnomeRootWin': '_GNOME_ROOTWIN',
	'GnomeAppBar': '_GNOME_APPBAR',
	'GnomeDEntryEdit': '_GNOME_DENTRY_EDIT',
}

def toUpperStr(str):
	"""Converts a typename to the equivalent upercase and underscores
	name.  This is used to form the type conversion macros and enum/flag
	name variables"""
	if convSpecialCases.has_key(str):
		return convSpecialCases[str]
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

def enumName(typename):
	"""create a GTK_TYPE_* name from the given type"""
	part = toUpperStr(typename)
	if len(part) > 4 and part[:4] == '_GTK':
		return 'GTK_TYPE' + part[4:]
	else:
		return 'GTK_TYPE' + part

def conversionMacro(typename):
	"""get the conversion macro for the given type"""
	return toUpperStr(typename)[1:]

# a dictionary of known enum types ...
# the value is a cached copy of the GTK_TYPE name
enums = {}
# and flags ...
flags = {}

# a dictionary of objects ...
# the values are the conversion macros
objects = {}

# the known boxed types
boxed = {
	'GtkAccelGroup': ('PyGtkAccelGroup_Type', 'PyGtkAccelGroup_Get',
			  'PyGtkAccelGroup_New'),
	'GtkStyle': ('PyGtkStyle_Type', 'PyGtkStyle_Get', 'PyGtkStyle_New'),
	'GdkFont': ('PyGdkFont_Type', 'PyGdkFont_Get', 'PyGdkFont_New'),
	'GdkColor': ('PyGdkColor_Type', 'PyGdkColor_Get', 'PyGdkColor_New'),
	'GdkEvent': ('PyGdkEvent_Type', 'PyGdkEvent_Get', 'PyGdkEvent_New'),
	'GdkWindow': ('PyGdkWindow_Type', 'PyGdkWindow_Get','PyGdkWindow_New'),
	'GdkPixmap': ('PyGdkWindow_Type', 'PyGdkWindow_Get','PyGdkWindow_New'),
	'GdkBitmap': ('PyGdkWindow_Type', 'PyGdkWindow_Get','PyGdkWindow_New'),
	'GdkDrawable':('PyGdkWindow_Type','PyGdkWindow_Get','PyGdkWindow_New'),
	'GdkGC': ('PyGdkGC_Type', 'PyGdkGC_Get', 'PyGdkGC_New'),
	'GdkColormap': ('PyGdkColormap_Type', 'PyGdkColormap_Get',
			'PyGdkColormap_New'),
	'GdkDragContext': ('PyGdkDragContext_Type', 'PyGdkDragContext_Get',
			   'PyGdkDragContext_New'),
	'GtkSelectionData': ('PyGtkSelectionData_Type',
			     'PyGtkSelectionData_Get',
			     'PyGtkSelectionData_New'),
	'GdkAtom': ('PyGdkAtom_Type', 'PyGdkAtom_Get', 'PyGdkAtom_New'),
	'GdkCursor': ('PyGdkCursor_Type', 'PyGdkCursor_Get',
		      'PyGdkCursor_New'),
	'GtkCTreeNode': ('PyGtkCTreeNode_Type', 'PyGtkCTreeNode_Get',
			 'PyGtkCTreeNode_New'),
}

class VarDefs:
	def __init__(self):
		self.vars = {}
	def add(self, type, name):
		if self.vars.has_key(type):
			self.vars[type] = self.vars[type] + (name,)
		else:
			self.vars[type] = (name,)
	def __str__(self):
		ret = []
		for type in self.vars.keys():
			ret.append('    ')
			ret.append(type)
			ret.append(' ')
			ret.append(string.join(self.vars[type], ', '))
			ret.append(';\n')
		if ret: ret.append('\n')
		return string.join(ret, '')

class TypesParser(scmexpr.Parser):
	"""A parser that only parses definitions -- no output"""
	def define_enum(self, name, *values):
		if not enums.has_key(name):
			enums[name] = enumName(name)
	def define_flags(self, name, *values):
		if not flags.has_key(name):
			flags[name] = enumName(name)
	def define_object(self, name, *args):
		if not objects.has_key(name):
			objects[name] = conversionMacro(name)
	def define_boxed(self, name, reffunc=None, unreffunc=None, size=None):
		if not boxed.has_key(name):
			print "Warning --", name, "not supported"
	def include(self, filename):
		if filename[0] != '/':
			# filename relative to file being parsed
			afile = os.path.join(os.path.dirname(self.filename),
					     filename)
		else:
			afile = filename
		if not os.path.exists(afile):
			# fallback on PWD relative filename
			afile = filename
		#print "including file", afile
		fp = open(afile)
		self.startParsing(scmexpr.parse(fp))

class FunctionDefsParser(TypesParser):
	def __init__(self, input, prefix='gtkmodule', typeprefix='&'):
		# typeprefix is set to & if type structs are not pointers
		TypesParser.__init__(self, input)
		self.impl = open(prefix + '_impl.c', "w")
		self.defs = open(prefix + '_defs.c', "w")
		self.tp = typeprefix

	def unknown(self, tup):
		print "Unknown function:", (tup and tup[0] or "")

	def define_object(self, name, parent=(), fields=()):
		TypesParser.define_object(self, name)
		get_type = string.lower(objects[name]) + '_get_type'
		self.defs.write(funcDefTmpl % (get_type,get_type))
		self.impl.write(getTypeTmpl % (get_type,get_type,get_type))

		if fields and fields[0] == 'fields':
			for retType, attrname in fields[1:]:
				self.get_attr_func(name, retType, attrname)

	def get_attr_func(self, name, retType, attrname):
		impl = StringIO()
		funcName = string.lower(objects[name]) + '_get_' + attrname
		impl.write(funcLeadTmpl % (funcName,))
		if retType == 'string':
			# this is needed so we can free result string
			impl.write('    gchar *ret;\n')
			impl.write('    PyObject *py_ret;\n')
		elif retType == 'static_string':
			impl.write('    gchar *ret;\n')
		elif retType in objects.keys():
			impl.write('    GtkObject *ret;\n')
		elif retType == 'GdkAtom':
			impl.write('    GdkAtom ret;\n')
		elif retType in boxed.keys():
			impl.write('    %s *ret;\n' % retType)
		impl.write('    PyObject *obj;\n\n')
		impl.write('    if (!PyArg_ParseTuple(args, "O!:')
		impl.write(funcName)
		impl.write('", ' + self.tp + 'PyGtk_Type, &obj))\n')
		impl.write('        return NULL;\n')
		funcCall = '%s(PyGtk_Get(obj))->%s' % (objects[name], attrname)
		if self.decodeRet(impl, funcCall, retType):
			return
		impl.write('}\n\n')
		# actually write the info to the output files
		self.defs.write(funcDefTmpl % (funcName, funcName))
		self.impl.write(impl.getvalue())
		
	def define_func(self, name, retType, args):
		# we write to a temp file, in case any errors occur ...
		impl = StringIO()
		impl.write(funcLeadTmpl % (name,))
		varDefs = VarDefs()
		parseStr = ''   # PyArg_ParseTuple string
		parseList = []  # args to PyArg_ParseTuple
		argList = []    # args to actual function
		extraCode = []  # any extra code (for enums, flags)
		retArgs = None
		if type(retType) == type(()):
			retArgs = retType[1:]
			retType = retType[0]
		if retType == 'string':
			# this is needed so we can free result string
			varDefs.add('gchar', '*ret')
			varDefs.add('PyObject', '*py_ret')
		elif retType == 'static_string':
			varDefs.add('gchar', '*ret')
		elif retType in objects.keys():
			varDefs.add('GtkObject', '*ret')
		elif retType == 'GdkAtom':
			varDefs.add('GdkAtom', 'ret')
		elif retType in boxed.keys():
			varDefs.add(retType, '*ret')
		for arg in args:
			argType = arg[0]
			argName = arg[1]
			# munge special names ...
			if argName in ('default', 'if', 'then', 'else',
				       'while', 'for', 'do', 'case', 'select'):
				argName = argName + '_'
			default = ''
			nullok = FALSE
			if len(arg) > 2:
				for a in arg[2:]:
					if type(a) == type(()) and a[0] == '=':
						default = ' = ' + a[1]
						if '|' not in parseStr:
							parseStr = parseStr+'|'
					elif type(a) == type(()) and \
					     a[0] == 'null-ok':
						nullok = TRUE
			if argType == 'string':
				varDefs.add('char', '*' + argName + default)
				if nullok:
					parseStr = parseStr + 'z'
				else:
					parseStr = parseStr + 's'
				parseList.append('&' + argName)
				argList.append(argName)
			elif argType in ('char', 'uchar'):
				varDefs.add('char', argName + default)
				parseStr = parseStr + 'c'
				parseList.append('&' + argName)
				argList.append(argName)
			elif argType in ('bool', 'int', 'uint', 'long',
					 'ulong', 'guint', 'GdkAtom'):
				# pretend atoms are integers for input ...
				varDefs.add('int', argName + default)
				parseStr = parseStr + 'i'
				parseList.append('&' + argName)
				argList.append(argName)
			elif argType in ('float', 'double'):
				varDefs.add('double', argName + default)
				parseStr = parseStr + 'd'
				parseList.append('&' + argName)
				argList.append(argName)
			elif argType == 'FILE':
				varDefs.add('PyObject', '*' + argName+default)
				parseStr = parseStr + 'O!'
				parseList.append('&PyFile_Type')
				parseList.append('&' + argName)
				argList.append('PyFile_AsFile(' +
					       argName + ')')
			elif argType in enums.keys():
				varDefs.add(argType, argName + default)
				if default:
					varDefs.add('PyObject', '*py_' + \
						    argName + ' = NULL')
				else:
					varDefs.add('PyObject', '*py_'+argName)
				parseStr = parseStr + 'O'
				parseList.append('&py_' + argName)
				argList.append(argName)
				extraCode.append(enumCodeTmpl %
						 (enums[argType],
						  'py_' + argName,
						  argName))
			elif argType in flags.keys():
				varDefs.add(argType, argName + default)
				if default:
					varDefs.add('PyObject', '*py_' + \
						    argName + ' = NULL')
				else:
					varDefs.add('PyObject', '*py_'+argName)
				parseStr = parseStr + 'O'
				parseList.append('&py_' + argName)
				argList.append(argName)
				extraCode.append(flagCodeTmpl %
						 (flags[argType],
						  'py_' + argName,
						  argName))
			elif argType in objects.keys():
				if nullok:
					parseStr = parseStr + 'O'
					parseList.append('&py_' + argName)
					varDefs.add('PyObject', '*py_' +
						    argName + ' = Py_None')
					varDefs.add(argType, '*' + argName +
						    ' = NULL')
					extraCode.append(nullokTmpl %
							 (argName, argName,
							  objects[argType],
							  argName, argName,
							  argName, argType))
					argList.append(argName)
				elif default:
					parseStr = parseStr + 'O!'
					parseList.append(self.tp +'PyGtk_Type')
					varDefs.add('PyObject', '*py_' +
						    argName + ' = NULL')
					varDefs.add(argType, '*' + argName +
						    default)
					parseList.append('&py_' + argName)
					extraCode.append(setVarTmpl %
							 (argName, argName,
							  objects[argType] +
							  '(PyGtk_Get(py_' +
							  argName + '))'))
					argList.append(argName)
				else:
					parseStr = parseStr + 'O!'
					parseList.append(self.tp +'PyGtk_Type')
					varDefs.add('PyObject', '*' + argName)
					parseList.append('&' + argName)
					argList.append(objects[argType] +
						       '(PyGtk_Get(' +
						       argName + '))')
			elif argType in boxed.keys():
				tp, get, new = boxed[argType]
				if nullok:
					varDefs.add('PyObject', '*py_' +
						    argName + ' = Py_None')
					varDefs.add(argType, '*' + argName +
						    ' = NULL')
					parseStr = parseStr + 'O'
					parseList.append('&py_' + argName)
					extraCode.append(nullokBoxedTmpl %
							 (argType, argName,
							  argName, get,
							  argName, argName,
							  argName, argType))
					argList.append(argName)
				else:
					varDefs.add('PyObject', '*' + argName)
					parseStr = parseStr + 'O!'
					parseList.append(self.tp + tp)
					parseList.append('&' + argName)
					argList.append(get + '('+argName+')')
			else:
				print "%s: unknown arg type '%s'" % (name,
								     argType)
				return
				
		impl.write(str(varDefs))
		impl.write('    if (!PyArg_ParseTuple(args, "')
		impl.write(parseStr)
		impl.write(':')
		impl.write(name)
		impl.write('"')
		if parseList:
			impl.write(', ')
			impl.write(string.join(parseList, ', '))
		impl.write('))\n        return NULL;\n')
		impl.write(string.join(extraCode, ''))

		funcCall = name + '(' + string.join(argList, ', ') + ')'
		if self.decodeRet(impl, funcCall, retType, retArgs):
			return
		impl.write('}\n\n')
		# actually write the info to the output files
		self.defs.write(funcDefTmpl % (name,name))
		self.impl.write(impl.getvalue())

	def decodeRet(self, impl, funcCall, retType, retArgs=None):
		nullok = FALSE
		if retArgs:
			# XXX fix me
			if retArgs[0] == 'null-ok':
				nullok = TRUE
		        else:
				print "unknown return attribute '%s'" % (retArgs,)
				return 1
		if retType == 'none':
			impl.write('    ')
			impl.write(funcCall)
			impl.write(';\n    Py_INCREF(Py_None);\n    return Py_None;\n')
		elif retType == 'static_string':
			impl.write('    ret = %s;\n' % funcCall)
			impl.write('    if (ret) {\n'
				   '        return PyString_FromString(ret);\n'
				   '    } else {\n'
				   '        Py_INCREF(Py_None);\n'
				   '        return Py_None;\n'
				   '    }\n')
		elif retType == 'string':
			impl.write('    ret = %s;\n' % funcCall)
			impl.write('    if (ret) {\n'
				   '        py_ret = PyString_FromString(ret);\n'
				   '        g_free(ret);\n'
				   '        return py_ret;\n'
				   '    } else {\n' 
				   '        Py_INCREF(Py_None);\n'
				   '        return Py_None;\n'
				   '    }\n')
		elif retType in ('char', 'uchar'):
			impl.write('    return PyString_fromStringAndSize(*(')
			impl.write(funcCall)
			impl.write('));\n')
		elif retType in ('bool','int','uint','long','ulong','guint') \
		     or retType in enums.keys() or retType in flags.keys():
			impl.write('    return PyInt_FromLong(')
			impl.write(funcCall)
			impl.write(');\n')
		elif retType in ('float','double'):
			impl.write('    return PyFloat_FromDouble(')
			impl.write(funcCall)
			impl.write(');\n')
		elif retType in boxed.keys():
			impl.write('    ret = %s;\n' % funcCall)
			impl.write('    if (ret)\n')
			impl.write('        return %s(ret);\n' % boxed[retType][2])
			impl.write('    Py_INCREF(Py_None);\n'
				   '    return Py_None;\n')
		elif retType in objects.keys():
			impl.write('    ret = (GtkObject *) %s;\n' % funcCall)
			impl.write('    if (ret)\n'
				   '        return PyGtk_New(ret);\n'
				   '    Py_INCREF(Py_None);\n'
				   '    return Py_None;\n')
		else:
			print "unknown return type '%s'" % (retType,)
			return 1
		return 0

class FilteringParser(FunctionDefsParser):
	"""A refinement of FunctionDefsParser with some common filter types
	built in"""
	def __init__(self, input, prefix='gtkmodule', typeprefix='&'):
		FunctionDefsParser.__init__(self, input, prefix, typeprefix)
		# hash lookups are pretty fast ...
		self.excludeList = {}
		self.excludeGlob = []
	def filter(self, name):
		if self.excludeList.has_key(name):
			return 0
		for glob in self.excludeGlob:
			if fnmatch.fnmatchcase(name, glob):
				return 0
		return 1
	def define_func(self, name, retType, args):
		if self.filter(name):
			FunctionDefsParser.define_func(self,name,retType,args)
		else:
			print "%s: filtered out" % (name)
	def addExcludeFile(self, fname):
		"""Adds the function names from file fname to excludeList"""
		lines = open(fname).readlines()
		# remove \n from each line ...
		lines = map(lambda x: x[:-1], lines)
		# remove comments and blank lines ...
		lines = filter(lambda x: x and x[0] != '#', lines)
		for name in lines:
			self.excludeList[name] = None
	def addExcludeGlob(self, glob):
		if glob not in self.excludeGlob:
			self.excludeGlob.append(glob)
		
if __name__ == '__main__':
	opts, args = getopt.getopt(sys.argv[1:], 'd:o:h',
				   ['defs=', 'output-prefix=', 'exclude-file=',
				    'exclude-glob', 'register=', 'help'])
	defs = None
	output_prefix = None
	exclude_files = []
	exclude_glob = []
	for opt, arg in opts:
		if opt in ('-d', '--defs'):
			defs = arg
		elif opt in ('-o', '--output-prefix'):
			output_prefix=arg
		elif opt == '--exclude-file':
			exclude_files.append(arg)
		elif opt == '--exclude-glob':
			exclude_globs.append(arg)
		elif opt == '--register':
			TypesParser(arg).startParsing()
		elif opt in (-h, '--help'):
			sys.stderr.write('usage: generate.py -d defs file -o output-prefix\n')
			sys.exit(0)
	if not defs or not output_prefix:
		sys.stderr.write('usage: generate.py -d defs file -o output-prefix\n')
		sys.exit(1)
	parser = FilteringParser(input=defs, prefix=output_prefix,
				 typeprefix='&')
	for file in exclude_files:
		parser.addExcludeFile(file)
	for glob in exclude_globs:
		parser.addExcludeGlob(glob)

	parser.startParsing()
