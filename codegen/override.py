# -*- Mode: Python; py-indent-offset: 4 -*-

# this file contains code for loading up an override file.  The override file
# provides implementations of functions where the code generator could not
# do its job correctly.

import sys, string, fnmatch
import re

import_pat = re.compile(r'\s*import\s+(\S+)\.([^\s.]+)\s+as\s+(\S+)')

class Overrides:
    def __init__(self, filename=None):
        self.modulename = None
	self.ignores = {}
	self.glob_ignores = []
	self.overrides = {}
        self.overridden = {}
	self.kwargs = {}
        self.noargs = {}
        self.startlines = {}
        self.override_attrs = {}
        self.override_slots = {}
        self.headers = ''
        self.init = ''
        self.imports = []
	if filename: self.handle_file(filename)

    def handle_file(self, filename):
        fp = open(filename, 'r')
	# read all the components of the file ...
        bufs = []
        startline = 1
        lines = []
        line = fp.readline()
        linenum = 1
        while line:
            if line == '%%\n' or line == '%%':
                if lines:
                    bufs.append((string.join(lines, ''), startline))
                startline = linenum + 1
                lines = []
            else:
                lines.append(line)
            line = fp.readline()
            linenum = linenum + 1
        if lines:
            bufs.append((string.join(lines, ''), startline))
	if not bufs: return

	for buf, startline in bufs:
	    self.__parse_override(buf, startline, filename)

    def __parse_override(self, buffer, startline, filename):
	pos = string.find(buffer, '\n')
	if pos >= 0:
	    line = buffer[:pos]
	    rest = buffer[pos+1:]
	else:
	    line = buffer ; rest = ''
	words = string.split(line)
	if words[0] == 'ignore' or words[0] == 'ignore-' + sys.platform :
	    for func in words[1:]: self.ignores[func] = 1
	    for func in string.split(rest): self.ignores[func] = 1
	elif words[0] == 'ignore-glob' or words[0] == 'ignore-glob-' + sys.platform :
	    for func in words[1:]: self.glob_ignores.append(func)
	    for func in string.split(rest):
		self.glob_ignores.append(func)
	elif words[0] == 'override':
	    func = words[1]
	    if 'kwargs' in words[1:]:
		self.kwargs[func] = 1
            elif 'noargs' in words[1:]:
		self.noargs[func] = 1
	    self.overrides[func] = rest
            self.startlines[func] = (startline + 1, filename)
        elif words[0] == 'override-attr':
            attr = words[1]
            self.override_attrs[attr] = rest
            self.startlines[attr] = (startline + 1, filename)
        elif words[0] == 'override-slot':
            slot = words[1]
            self.override_slots[slot] = rest
            self.startlines[slot] = (startline + 1, filename)
        elif words[0] == 'headers':
            self.headers = '%s\n#line %d "%s"\n%s' % \
                           (self.headers, startline + 1, filename, rest)
        elif words[0] == 'init':
            self.init = '%s\n#line %d "%s"\n%s' % \
                        (self.init, startline + 1, filename, rest)
        elif words[0] == 'modulename':
            self.modulename = words[1]
        elif words[0] == 'import':
            for line in string.split(buffer, '\n'):
                match = import_pat.match(line)
                if match:
                    self.imports.append(match.groups())

    def is_ignored(self, name):
	if self.ignores.has_key(name):
	    return 1
	for glob in self.glob_ignores:
	    if fnmatch.fnmatchcase(name, glob):
		return 1
	return 0
    def is_overriden(self, name):
	return self.overrides.has_key(name)
    def is_already_included(self, name):
        return self.overridden.has_key(name)
    def override(self, name):
        self.overridden[name] = 1
        return self.overrides[name]
    def getstartline(self, name):
        return self.startlines[name]
    def wants_kwargs(self, name):
	return self.kwargs.has_key(name)
    def wants_noargs(self, name):
	return self.noargs.has_key(name)
    def attr_is_overriden(self, attr):
        return self.override_attrs.has_key(attr)
    def attr_override(self, attr):
        return self.override_attrs[attr]
    def slot_is_overriden(self, slot):
        return self.override_slots.has_key(slot)
    def slot_override(self, slot):
        return self.override_slots[slot]
    def get_headers(self):
        return self.headers
    def get_init(self):
        return self.init
    def get_imports(self):
        return self.imports
