#!/usr/bin/env python
# -*- Mode: Python; py-indent-offset: 4 -*-
import sys, os, string, re, getopt
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
        files = []
	for obj_def in obj_defs:
            filename = self.create_filename(obj_def.c_name, output_prefix)
	    fp = open(filename, 'w')
	    self.output_object_docs(obj_def, fp)
	    fp.close()
            files.append((os.path.basename(filename), obj_def))
        if files:
            filename = self.create_filename('docs', output_prefix)
	    fp = open(filename, 'w')
            self.output_toc(files, fp)
            fp.close()
            
    def output_object_docs(self, obj_def, fp=sys.stdout):
	self.write_class_header(obj_def.c_name, fp)

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
        self.write_heading('Ancestry', fp)
        self.write_heirachy(obj_def.c_name, ancestry, fp)
        self.close_section(fp)

        constructor = self.parser.find_constructor(obj_def, self.overrides)
        if constructor:
            self.write_heading('Constructor', fp)
            self.write_constructor(constructor,
                                   self.docs.get(constructor.c_name, None),
                                   fp)
            self.close_section(fp)

        methods = self.parser.find_methods(obj_def)
        methods = filter(lambda meth, self=self:
                         not self.overrides.is_ignored(meth.c_name), methods)
        if methods:
            self.write_heading('Methods', fp)
            for method in methods:
                self.write_method(method, self.docs.get(method.c_name, None), fp)
            self.close_section(fp)

        self.write_class_footer(obj_def.c_name, fp)

    def output_toc(self, files, fp=sys.stdout):
        fp.write('TOC\n\n')
        for filename, obj_def in files:
            fp.write(obj_def.c_name + ' - ' + filename + '\n')

    # override the following to create a more complex output format
    def create_filename(self, obj_name, output_prefix):
	'''Create output filename for this particular object'''
	return output_prefix + '-' + string.lower(obj_name) + '.txt'

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

    def write_class_header(self, obj_name, fp):
	fp.write('Class %s\n' % obj_name)
	fp.write('======%s\n\n' % ('=' * len(obj_name)))
    def write_class_footer(self, obj_name, fp):
        pass
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
                descr = 'a ' + type
            fp.write('  ' + name + ': ' + descr + '\n')
        if func_def.ret and func_def.ret != 'none':
            if func_doc and func_doc.ret:
                descr = func_doc.ret
            else:
                descr = 'a ' + func_def.ret
            fp.write('  Returns: ' + descr + '\n')
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
                descr = 'a ' + type
            fp.write('  ' + name + ': ' + descr + '\n')
        if meth_def.ret and meth_def.ret != 'none':
            if func_doc and func_doc.ret:
                descr = func_doc.ret
            else:
                descr = 'a ' + meth_def.ret
            fp.write('  Returns: ' + descr + '\n')
        if func_doc and func_doc.description:
            fp.write('\n')
            fp.write(func_doc.description)
        fp.write('\n\n')

class DocbookDocWriter(DocWriter):
    def create_filename(self, obj_name, output_prefix):
	'''Create output filename for this particular object'''
	return output_prefix + '-' + string.lower(obj_name) + '.sgml'

    # make string -> reference translation func
    __transtable = [ '-' ] * 256
    for letter in 'abcdefghijklmnopqrstuvwxyz':
        __transtable[ord(letter)] = letter
        __transtable[ord(string.upper(letter))] = letter
    __transtable = string.join(__transtable, '')

    def make_class_ref(self, obj_name):
        return 'class-' + string.translate(obj_name, self.__transtable)
    def make_method_ref(self, meth_def):
        return 'method-' + string.translate(meth_def.of_object[1] +
                                            meth_def.of_object[0],
                                            self.__transtable) + \
            '--' + string.translate(meth_def.name, self.__transtable)

    __function_pat = re.compile(r'(\w+)\s*\(\)')
    def __format_function(self, match):
        info = self.parser.c_name.get(match.group(1), None)
        if info:
            if info.__class__ == defsparser.FunctionDef:
                if info.is_constructor_of is not None:
                    # should have a link here
                    return '<function>' + info.is_constructor_of + \
                           '()</function>'
                else:
                    return '<function>' + info.name + '()</function>'
            if info.__class__ == defsparser.MethodDef:
                return '<link linkend="' + self.make_method_ref(info) + \
                       '"><function>' + info.of_object[1] + \
                       info.of_object[0] + '.' + info.name + \
                       '()</function></link>'
        # fall through through
        return '<function>' + match.group(1) + '()</function>'
    __parameter_pat = re.compile(r'\@(\w+)')
    def __format_param(self, match):
        return '<parameter>' + match.group(1) + '</parameter>'
    __constant_pat = re.compile(r'\%(-?\w+)')
    def __format_const(self, match):
        return '<literal>' + match.group(1) + '</literal>'
    __symbol_pat = re.compile(r'#([\w-]+)')
    def __format_symbol(self, match):
        info = self.parser.c_name.get(match.group(1), None)
        if info:
            if info.__class__ == defsparser.FunctionDef:
                if info.is_constructor_of is not None:
                    # should have a link here
                    return '<function>' + info.is_constructor_of + \
                           '</function>'
                else:
                    return '<function>' + info.name + '</function>'
            if info.__class__ == defsparser.MethodDef:
                return '<link linkend="' + self.make_method_ref(info) + \
                       '"><function>' + info.of_object[1] + \
                       info.of_object[0] + '.' + info.name + \
                       '</function></link>'
            if info.__class__ == defsparser.ObjectDef:
                return '<link linkend="' + self.make_class_ref(info.c_name) + \
                       '"><type>' + info.c_name + '</type></link>'
        # fall through through
        return '<literal>' + match.group(1) + '</literal>'
    
    def reformat_text(self, text):
        # replace special strings ...
        text = self.__function_pat.sub(self.__format_function, text)
        text = self.__parameter_pat.sub(self.__format_param, text)
        text = self.__constant_pat.sub(self.__format_const, text)
        text = self.__symbol_pat.sub(self.__format_symbol, text)
        
        lines = string.split(string.strip(text), '\n')
        for index in range(len(lines)):
            if string.strip(lines[index]) == '':
                lines[index] = '</para>\n<para>'
                continue
        lines.insert(0, '<para>')
        lines.append('</para>')
        return string.join(lines, '\n')

    # these need to handle default args ...
    def create_constructor_prototype(self, func_def):
        sgml = [ '  <funcsynopsis>\n    <funcprototype>\n']
        sgml.append('      <funcdef><function>')
        sgml.append(func_def.is_constructor_of)
        sgml.append('</function></funcdef>\n')
        for type, name, dflt, null in func_def.params:
            sgml.append('      <paramdef><parameter>')
            sgml.append(name)
            sgml.append('</parameter>')
            if dflt:
                sgml.append('=')
                sgml.append(dflt)
            sgml.append('</paramdef>\n')
        if not func_def.params:
            sgml.append('      <paramdef></paramdef>')
        sgml.append('    </funcprototype>\n  </funcsynopsis>\n')
        return string.join(sgml, '')
    def create_function_prototype(self, func_def):
        sgml = [ '  <funcsynopsis>\n    <funcprototype>\n']
        sgml.append('      <funcdef><function>')
        sgml.append(func_def.name)
        sgml.append('</function></funcdef>\n')
        for type, name, dflt, null in func_def.params:
            sgml.append('      <paramdef><parameter>')
            sgml.append(name)
            sgml.append('</parameter>')
            if dflt:
                sgml.append('=')
                sgml.append(dflt)
            sgml.append('</paramdef>\n')
        if not func_def.params:
            sgml.append('      <paramdef></paramdef>')
        sgml.append('    </funcprototype>\n  </funcsynopsis>\n')
        return string.join(sgml, '')
    def create_method_prototype(self, meth_def):
        sgml = [ '  <funcsynopsis>\n    <funcprototype>\n']
        sgml.append('      <funcdef><replaceable>instance</replaceable>.<function>')
        sgml.append(meth_def.name)
        sgml.append('</function></funcdef>\n')
        for type, name, dflt, null in meth_def.params:
            sgml.append('      <paramdef><parameter>')
            sgml.append(name)
            sgml.append('</parameter>')
            if dflt:
                sgml.append('=')
                sgml.append(dflt)
            sgml.append('</paramdef>\n')
        if not meth_def.params:
            sgml.append('      <paramdef></paramdef>')
        sgml.append('    </funcprototype>\n  </funcsynopsis>\n')
        return string.join(sgml, '')
    
    def write_class_header(self, obj_name, fp):
        fp.write('<sect1 id="' + self.make_class_ref(obj_name) + '">\n')
        fp.write('  <title>Class ' + obj_name + '</title>\n\n')
    def write_class_footer(self, obj_name, fp):
        fp.write('</sect1>\n')
    def write_heading(self, text, fp):
        fp.write('  <sect2>\n')
        fp.write('    <title>' + text + '</title>\n\n')
    def close_section(self, fp):
        fp.write('  </sect2>\n')

    def write_heirachy(self, obj_name, ancestry, fp):
        fp.write('<synopsis>')
        indent = ''
        for name, interfaces in ancestry:
            fp.write(indent + '+-- <link linkend="' +
                     self.make_class_ref(name) + '">'+ name + '</link>')
            if interfaces:
                fp.write(' (implements ')
                fp.write(string.join(interfaces, ', '))
                fp.write(')\n')
            else:
                fp.write('\n')
            indent = indent + '  '
        fp.write('</synopsis>\n\n')

    def write_constructor(self, func_def, func_doc, fp):
        prototype = self.create_constructor_prototype(func_def)
        fp.write(prototype + '\n')
        varlist_started = 0
        for type, name, dflt, null in func_def.params:
            if not varlist_started:
                fp.write('  <variablelist>\n')
                varlist_started = 1
            if func_doc:
                descr = string.strip(func_doc.get_param_description(name))
            else:
                descr = 'a ' + type
            fp.write('    <varlistentry>\n')
            fp.write('      <term><parameter>'+ name +'</parameter></term>\n')
            fp.write('      <listitem>' + self.reformat_text(descr) +
                     '</listitem>\n')
            fp.write('    </varlistentry\n')
        if func_def.ret and func_def.ret != 'none':
            if not varlist_started:
                fp.write('  <variablelist>\n')
                varlist_started = 1
            if func_doc and func_doc.ret:
                descr = string.strip(func_doc.ret)
            else:
                descr = 'a ' + func_def.ret
            fp.write('    <varlistentry>\n')
            fp.write('      <term>Returns:</term>\n')
            fp.write('      <listitem>' + self.reformat_text(descr) +
                     '</listitem>\n')
            fp.write('    </varlistentry\n')
        if varlist_started:
            fp.write('  </variablelist>\n')
        if func_doc and func_doc.description:
            fp.write(self.reformat_text(func_doc.description))
        fp.write('\n\n\n')

    def write_method(self, meth_def, func_doc, fp):
        fp.write('  <sect3 id="' + self.make_method_ref(meth_def) + '">\n')
        fp.write('    <title>' + meth_def.of_object[1] +
                 meth_def.of_object[0] + '.' + meth_def.name + '</title>\n\n')
        prototype = self.create_method_prototype(meth_def)
        fp.write(prototype + '\n')
        varlist_started = 0
        for type, name, dflt, null in meth_def.params:
            if not varlist_started:
                fp.write('  <variablelist>\n')
                varlist_started = 1
            if func_doc:
                descr = string.strip(func_doc.get_param_description(name))
            else:
                descr = 'a ' + type
            fp.write('    <varlistentry>\n')
            fp.write('      <term><parameter>'+ name +'</parameter></term>\n')
            fp.write('      <listitem>' + self.reformat_text(descr) +
                     '</listitem>\n')
            fp.write('    </varlistentry\n')
        if meth_def.ret and meth_def.ret != 'none': 
            if not varlist_started:
                fp.write('  <variablelist>\n')
                varlist_started = 1
            if func_doc and func_doc.ret:
                descr = string.strip(func_doc.ret)
            else:
                descr = 'a ' + meth_def.ret
            fp.write('    <varlistentry>\n')
            fp.write('      <term>Returns:</term>\n')
            fp.write('      <listitem>' + self.reformat_text(descr) +
                     '</listitem>\n')
            fp.write('    </varlistentry\n')
        if varlist_started:
            fp.write('  </variablelist>\n')
        if func_doc and func_doc.description:
            fp.write(self.reformat_text(func_doc.description))
        fp.write('  </sect3>\n\n\n')

    def output_toc(self, files, fp=sys.stdout):
        fp.write('<!doctype article PUBLIC "-//OASIS//DTD DocBook V3.1//EN" [\n')
        for filename, obj_def in files:
            fp.write('  <!entity ' + string.translate(obj_def.c_name,
                                                      self.__transtable) +
                     ' SYSTEM "' + filename + '" >\n')
        fp.write(']>\n\n')

        fp.write('<article id="index">\n\n')
        fp.write('  <artheader>\n')
        fp.write('    <title>PyGTK Docs</title>\n')
        fp.write('    <authorgroup>\n')
        fp.write('      <author>\n')
        fp.write('        <firstname>James</firstname>\n')
        fp.write('        <surname>Henstridge</surname>\n')
        fp.write('      </author>\n')
        fp.write('    </authorgroup>\n')
        fp.write('  </artheader>\n\n')

        fp.write('  <sect1>\n')
        fp.write('    <title>Class Heirachy</title>\n')
        fp.write('    <para>Not done yet</para>\n')
        fp.write('  </sect1>\n')

        for filename, obj_def in files:
            fp.write('&' +string.translate(obj_def.c_name, self.__transtable)+
                     ';\n')

        fp.write('</article>\n')

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

    d = DocbookDocWriter(defs_file, overrides_file, source_dirs)
    d.output_docs(output_prefix)
