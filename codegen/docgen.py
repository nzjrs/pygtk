#!/usr/bin/env python
# -*- Mode: Python; py-indent-offset: 4 -*-
import sys, string, getopt
import defsparser
import override
import docextract

class DocWriter:
    def __init__(self, defs_file, overrides_file, source_dirs):
	# parse the defs file
	self.parser = defsparser.DefsParser(defs_file)
	self.parser.startParsing()

        # the overrides file
        if overrides_file:
            self.overrides = override.Overrides(open(overrides_file))
        else:
            self.overrides = override.Overrides(None)

	# extract documentation from C source code
	self.docs = docextract.extract(source_dirs)

    def output_docs(self, output_prefix):
	obj_defs = self.parser.objects[:]
	obj_defs.sort(lambda a, b: cmp(a.c_name, b.c_name))
	for obj_def in obj_defs:
	    fp = open(self.create_filename(obj_def, output_prefix), 'w')
	    self.output_object_docs(obj_def, fp)
	    fp.close()
    def output_object_docs(self, obj_def, fp=sys.stdout):
	self.write_object_header(obj_def.c_name, fp)

	# construct the inheritence heirachy ...
	ancestry = [ (obj_def.c_name, obj_def.implements) ]
        try:
            parent = obj_def.parent
            while parent != (None, None):
                if parent[1] + parent[0] == 'GObject':
                    ancestry.append(('GObject', []))
                    parent = (None, None)
                else:
                    parent_def = self.parser.find_object(parent[1] + parent[0])
                    ancestry.append((parent_def.c_name, parent_def.implements))
                    parent = parent_def.parent
        except ValueError:
            pass
        ancestry.reverse()
        self.write_heirachy(obj_def.c_name, ancestry, fp)

        constructor = self.parser.find_constructor(obj_def, self.overrides)
        if constructor:
            self.write_heading('Constructor', fp)
            self.write_constructor(constructor,
                                   self.docs.get(constructor.c_name, None),
                                   fp)
            self.close_section(fp)
            
        self.write_heading('Methods', fp)
        methods = self.parser.find_methods(obj_def)
        for method in methods:
            if self.overrides.is_ignored(method.c_name):
                continue
            self.write_method(method, self.docs.get(method.c_name, None), fp)
        self.close_section(fp)

    # override the following to create a more complex output format
    def create_filename(self, obj_def, output_prefix):
	'''Create output filename for this particular object'''
	return output_prefix + '-' + string.lower(obj_def.c_name) + '.txt'

    # these need to handle default args ...
    def create_constructor_prototype(self, func_def):
        return func_def.is_constructor_of + '(' + \
               string.join(map(lambda x: x[1], func_def.params), ', ') + \
               ')'
    def create_function_prototype(self, func_def):
        return func_def.name + '(' + \
               string.join(map(lambda x: x[1], func_def.params), ', ') + \
               ')'
    def create_method_prototype(self, meth_def):
        return meth_def.of_object[1] + meth_def.of_object[0] + '.' + \
               meth_def.name + '(' + \
               string.join(map(lambda x: x[1], meth_def.params), ', ') + \
               ')'

    def write_object_header(self, obj_name, fp):
	fp.write('Class %s\n' % obj_name)
	fp.write('======%s\n\n' % ('=' * len(obj_name)))
    def write_heading(self, text, fp):
        fp.write('\n' + text + '\n' + ('-' * len(text)) + '\n')
    def close_section(self, fp):
        pass
    def write_heirachy(self, obj_name, ancestry, fp):
        indent = ''
        for name, interfaces in ancestry:
            fp.write(indent + '+-- ' + name)
            if interfaces:
                fp.write(' (implements ')
                fp.write(string.join(interfaces, ', '))
                fp.write(')\n')
            else:
                fp.write('\n')
            indent = indent + '  '
        fp.write('\n')
    def write_constructor(self, func_def, func_doc, fp):
        prototype = self.create_constructor_prototype(func_def)
        fp.write(prototype + '\n\n')
        for type, name, dflt, null in func_def.params:
            if func_doc:
                descr = func_doc.get_param_description(name)
            else:
                descr = ''
            fp.write('  ' + name + ': ' + descr + '\n')
        if func_doc and func_doc.ret:
            fp.write('  Returns: ' + func_doc.ret + '\n\n')
        if func_doc and func_doc.description:
            fp.write(func_doc.description)
        fp.write('\n\n\n')
    def write_method(self, meth_def, func_doc, fp):
        prototype = self.create_method_prototype(meth_def)
        fp.write(prototype + '\n\n')
        for type, name, dflt, null in meth_def.params:
            if func_doc:
                descr = func_doc.get_param_description(name)
            else:
                descr = ''
            fp.write('  ' + name + ': ' + descr + '\n')
        if func_doc and func_doc.ret:
            fp.write('  Returns: ' + func_doc.ret + '\n\n')
        if func_doc and func_doc.description:
            fp.write(func_doc.description)
        fp.write('\n\n')

if __name__ == '__main__':
    opts, args = getopt.getopt(sys.argv[1:], "d:s:o:",
			       ["defs-file=", "override=", "source-dir=",
                                "output-prefix="])
    defs_file = None
    overrides_file = None
    source_dirs = []
    output_prefix = 'docs'
    for opt, arg in opts:
	if opt in ('-d', '--defs-file'):
	    defs_file = arg
        if opt in ('--override',):
            overrides_file = arg
	elif opt in ('-s', '--source-dir'):
	    source_dirs.append(arg)
	elif opt in ('-o', '--output-prefix'):
	    output_prefix = arg
    if len(args) != 0 or not defs_file:
	sys.stderr.write(
	    'usage: docgen.py -d file.defs [-s /src/dir] [-o output-prefix]')

    d = DocWriter(defs_file, overrides_file, source_dirs)
    d.output_docs(output_prefix)
