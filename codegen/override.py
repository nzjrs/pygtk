# -*- Mode: Python; py-indent-offset: 4 -*-

# this file contains code for loading up an override file.  The override file
# provides implementations of functions where the code generator could not
# do its job correctly.

import sys, string, fnmatch

class Overrides:
    def __init__(self, filename=None):
	self.ignores = {}
	self.glob_ignores = []
	self.overrides = {}
	self.kwargs = {}
        self.startlines = {}
        self.override_attrs = {}
        self.headers = ''
        self.init = ''
	if filename: self.__handle_file(filename)

    def __handle_file(self, filename):
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
	    self.__parse_override(buf, startline)

    def __parse_override(self, buffer, startline):
	pos = string.find(buffer, '\n')
	if pos >= 0:
	    line = buffer[:pos]
	    rest = buffer[pos+1:]
	else:
	    line = buffer ; rest = ''
	words = string.split(line)
	if words[0] == 'ignore':
	    for func in words[1:]: self.ignores[func] = 1
	    for func in string.split(rest): self.ignores[func] = 1
	elif words[0] == 'ignore-glob':
	    for func in words[1:]: self.glob_ignores.append(func)
	    for func in string.split(rest):
		self.glob_ignores.append(func)
	elif words[0] == 'override':
	    func = words[1]
	    if 'kwargs' in words[1:]:
		self.kwargs[func] = 1
	    self.overrides[func] = rest
            self.startlines[func] = startline + 1
        elif words[0] == 'override-attr':
            attr = words[1]
            self.override_attrs[attr] = rest
            self.startlines[attr] = startline + 1
        elif words[0] == 'headers':
            self.headers = self.headers + '\n' + rest
        elif words[0] == 'init':
            self.init = self.init + '\n' + rest

    def is_ignored(self, name):
	if self.ignores.has_key(name):
	    return 1
	for glob in self.glob_ignores:
	    if fnmatch.fnmatchcase(name, glob):
		return 1
	return 0
    def is_overriden(self, name):
	return self.overrides.has_key(name)
    def override(self, name):
	return self.overrides[name]
    def getstartline(self, name):
        return self.startlines[name]
    def wants_kwargs(self, name):
	return self.kwargs.has_key(name)
    def attr_is_overriden(self, attr):
        return self.override_attrs.has_key(attr)
    def attr_override(self, attr):
        return self.override_attrs[attr]
    def get_headers(self):
        return self.headers
    def get_init(self):
        return self.init
