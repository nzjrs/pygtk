# -*- Mode: Python; py-indent-offset: 4 -*-
import os, sys
import scmexpr
from definitions import *

class IncludeParser(scmexpr.Parser):
    """A simple parser that follows include statements automatically"""
    def include(self, filename):
	if filename[0] != '/':
	    afile = os.path.join(os.path.dirname(self.filename),filename)
	else:
	    afile = filename
	if not os.path.exists(afile):
	    afile = filename
	fp = open(afile)
	self.startParsing(scmexpr.parse(fp))

class DefsParser(IncludeParser):
    def __init__(self, arg):
	IncludeParser.__init__(self, arg)
	self.objects = []
        self.interfaces = []
	self.enums = []      # enums and flags
        self.boxes = []      # boxed types
	self.functions = []  # functions and methods
	self.c_name = {}     # hash of c names of functions
	self.methods = {}    # hash of methods of particular objects

    def define_object(self, *args):
	odef = apply(ObjectDef, args)
	self.objects.append(odef)
	self.c_name[odef.c_name] = odef
    def define_interface(self, *args):
        idef = apply(InterfaceDef, args)
        self.interfaces.append(idef)
        self.c_name[idef.c_name] = idef
    def define_enum(self, *args):
        edef = apply(EnumDef, args)
        self.enums.append(edef)
        self.c_name[edef.c_name] = edef
    def define_flags(self, *args):
        fdef = apply(FlagsDef, args)
        self.enums.append(fdef)
        self.c_name[fdef.c_name] = fdef
    def define_boxed(self, *args):
        bdef = apply(BoxedDef, args)
        self.boxes.append(bdef)
        self.c_name[bdef.c_name] = bdef
    def define_function(self, *args):
	fdef = apply(FunctionDef, args)
	self.functions.append(fdef)
	self.c_name[fdef.c_name] = fdef
    def define_method(self, *args):
	mdef = apply(MethodDef, args)
	self.functions.append(mdef)
	self.c_name[mdef.c_name] = mdef
    
    def merge(self, old):
	for obj in self.objects:
	    if old.c_name.has_key(obj.c_name):
		obj.merge(old.c_name[obj.c_name])
	for f in self.functions:
	    if old.c_name.has_key(f.c_name):
		f.merge(old.c_name[f.c_name])

    def write_defs(self, fp=sys.stdout):
	for obj in self.objects:
	    obj.write_defs(fp)
	for enum in self.enums:
	    enum.write_defs(fp)
        for boxed in self.boxes:
            boxed.write_defs(fp)
	for func in self.functions:
	    func.write_defs(fp)

    def find_object(self, c_name):
        for obj in self.objects:
            if obj.c_name == c_name:
                return obj
        else:
            raise ValueError, 'object not found'

    def find_constructor(self, obj, overrides):
        for func in self.functions:
            if isinstance(func, FunctionDef) and \
               func.is_constructor_of == obj.c_name and \
               not overrides.is_ignored(func.c_name):
                return func

    def find_methods(self, obj):
        objname = obj.c_name
        return filter(lambda func, on=objname: isinstance(func, MethodDef) and
                      func.of_object == on, self.functions)

    def find_functions(self):
        return filter(lambda func: isinstance(func, FunctionDef) and
                      not func.is_constructor_of, self.functions)
