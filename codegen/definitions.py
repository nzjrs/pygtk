# -*- Mode: Python; py-indent-offset: 4 -*-
import sys

class Definition:
    def __init__(self, *args):
	"""Create a new defs object of this type.  The arguments are the
	components of the definition"""
	raise RuntimeError, "this is an abstract class"
    def merge(self, old):
	"""Merge in customisations from older version of definition"""
	raise RuntimeError, "this is an abstract class"
    def write_defs(self, fp=sys.stdout):
	"""write out this definition in defs file format"""
	raise RuntimeError, "this is an abstract class"

class ObjectDef(Definition):
    def __init__(self, name, *args):
	self.name = name
	self.module = None
	self.c_name = None
	self.parent = (None, None)
	self.fields = []
	for arg in args:
	    if type(arg) != type(()) or len(arg) < 2:
		continue
	    if arg[0] == 'in-module':
		self.module = arg[1]
	    elif arg[0] == 'parent':
		if len(arg) > 2:
		    self.parent = (arg[1], arg[2][0])
		else:
		    self.parent = (arg[1], None)
	    elif arg[0] == 'c-name':
		self.c_name = arg[1]
	    elif arg[0] == 'field':
		for parg in arg[1:]:
		    if parg[0] == 'type-and-name':
			self.fields.append((parg[1], parg[2]))
    def merge(self, old):
	# currently the .h parser doesn't try to work out what fields of
	# an object structure should be public, so we just copy the list
	# from the old version ...
	self.fields = old.fields
    def write_defs(self, fp=sys.stdout):
	fp.write('(object ' + self.name + '\n')
	if self.module:
	    fp.write('  (in-module ' + self.module + ')\n')
	if self.parent != (None, None):	
	    fp.write('  (parent ' + self.parent[0])
	    if self.parent[1]:
		fp.write(' (' + self.parent[1] + ')')
	    fp.write(')\n')
	if self.c_name:
	    fp.write('  (c-name ' + self.c_name + ')\n')
	for (ftype, fname) in self.fields:
	    fp.write('  (field (type-and-name ' + ftype + ' ' + fname + '))\n')
	fp.write(')\n\n')

class EnumDef(Definition):
    def __init__(self, name, *args):
	self.deftype = 'enum'
	self.name = name
	self.in_module = None
	self.c_name = None
	self.values = []
	for arg in args:
	    if type(arg) != type(()) or len(arg) < 2:
		continue
	    if arg[0] == 'in-module':
		self.in_module = arg[1]
	    elif arg[0] == 'c-name':
		self.c_name = arg[1]
	    elif arg[0] == 'value':
		vname = None
		vcname = None
		for parg in arg[1:]:
		    if parg[0] == 'name':
			vname = parg[1]
		    elif parg[0] == 'c-name':
			vcname = parg[1]
		self.values.append((vname, vcname))
    def merge(self, old):
	pass
    def write_defs(self, fp=sys.stdout):
	fp.write('(' + self.deftype + ' ' + self.name + '\n')
	if self.in_module:
	    fp.write('  (in-module ' + self.in_module + ')\n')
	fp.write('  (c-name ' + self.c_name + ')\n')
	for name, val in self.values:
	    fp.write('  (value (name ' + name + ') (c-name ' + val + '))\n')
	fp.write(')\n\n')

class FlagsDef(EnumDef):
    def __init__(self, *args):
	apply(EnumDef.__init__, (self,) + args)
	self.deftype = 'flags'

class MethodDef(Definition):
    def __init__(self, name, *args):
	self.name = name
	self.ret = None
	self.c_name = None
	self.of_object = (None, None)
	self.params = [] # of form (type, name, default, nullok)
        self.varargs = 0
	for arg in args:
	    if type(arg) != type(()) or len(arg) < 2:
		continue
	    if arg[0] == 'of-object':
		if len(arg) > 2:
		    self.of_object = (arg[1], arg[2][0])
		else:
		    self.of_object = (arg[1], None)
	    elif arg[0] == 'c-name':
		self.c_name = arg[1]
	    elif arg[0] == 'return-type':
		self.ret = arg[1]
	    elif arg[0] == 'parameter':
		ptype = None
		pname = None
		pdflt = None
		pnull = 0
		for parg in arg[1:]:
		    if parg[0] == 'type-and-name':
			ptype = parg[1]
			pname = parg[2]
		    elif parg[0] == 'default':
			pdflt = parg[1]
		    elif parg[0] == 'null-ok':
			pnull = 1
		self.params.append((ptype, pname, pdflt, pnull))
            elif arg[0] == 'varargs':
                self.varargs = arg[1] == 't'
    def merge(self, old):
	# here we merge extra parameter flags accross to the new object.
	for i in range(len(self.params)):
	    ptype, pname, pdflt, pnull = self.params[i]
	    for p2 in old.params:
		if p2[1] == pname:
		    self.params[i] = (ptype, pname, p2[2], p2[3])
		    break
    def write_defs(self, fp=sys.stdout):
	fp.write('(method ' + self.name + '\n')
	if self.of_object != (None, None):
	    fp.write('  (of-object ' + self.of_object[0])
	    if self.of_object[1]:
		fp.write(' (' + self.of_object[1] + ')')
	    fp.write(')\n')
	if self.c_name:
	    fp.write('  (c-name ' + self.c_name + ')\n')
	if self.ret:
	    fp.write('  (return-type ' + self.ret + ')\n')
	for ptype, pname, pdflt, pnull in self.params:
	    fp.write('  (parameter (type-and-name ' + ptype + ' ' + pname +')')
	    if pdflt: fp.write(' (default "' + pdflt + '")')
	    if pnull: fp.write(' (null-ok)')
	    fp.write(')\n')
	fp.write(')\n\n')

class FunctionDef(Definition):
    def __init__(self, name, *args):
	self.name = name
	self.in_module = None
	self.is_constructor_of = None
	self.ret = None
	self.c_name = None
	self.params = [] # of form (type, name, default, nullok)
        self.varargs = 0
	for arg in args:
	    if type(arg) != type(()) or len(arg) < 2:
		continue
	    if arg[0] == 'in-module':
		self.in_module = arg[1]
	    elif arg[0] == 'is-constructor-of':
		self.is_constructor_of = arg[1]
	    elif arg[0] == 'c-name':
		self.c_name = arg[1]
	    elif arg[0] == 'return-type':
		self.ret = arg[1]
	    elif arg[0] == 'parameter':
		ptype = None
		pname = None
		pdflt = None
		pnull = 0
		for parg in arg[1:]:
		    if parg[0] == 'type-and-name':
			ptype = parg[1]
			pname = parg[2]
		    elif parg[0] == 'default':
			pdflt = parg[1]
		    elif parg[0] == 'null-ok':
			pnull = 1
		self.params.append((ptype, pname, pdflt, pnull)) 
            elif arg[0] == 'varargs':
                self.varargs = arg[1] == 't'
    _method_write_defs = MethodDef.__dict__['write_defs']
    def merge(self, old):
	# here we merge extra parameter flags accross to the new object.
	for i in range(len(self.params)):
	    ptype, pname, pdflt, pnull = self.params[i]
	    for p2 in old.params:
		if p2[1] == pname:
		    self.params[i] = (ptype, pname, p2[2], p2[3])
		    break
	if not self.is_constructor_of:
	    self.is_constructor_of = old.is_constructor_of
	if isinstance(old, MethodDef):
	    self.name = old.name
	    # transmogrify from function into method ...
	    self.write_defs = self._method_write_defs
	    self.of_object = old.of_object
	    del self.params[0]
    def write_defs(self, fp=sys.stdout):
	fp.write('(function ' + self.name + '\n')
	if self.in_module:
	    fp.write('  (in-module (' + self.in_module + '))\n')
	if self.is_constructor_of:
	    fp.write('  (is-constructor-of ' + self.is_constructor_of + ')\n')
	if self.c_name:
	    fp.write('  (c-name ' + self.c_name + ')\n')
	if self.ret:
	    fp.write('  (return-type ' + self.ret + ')\n')
	for ptype, pname, pdflt, pnull in self.params:
	    fp.write('  (parameter (type-and-name ' + ptype + ' ' + pname +')')
	    if pdflt: fp.write(' (default "' + pdflt + '")')
	    if pnull: fp.write(' (null-ok)')
	    fp.write(')\n')
	fp.write(')\n\n')

