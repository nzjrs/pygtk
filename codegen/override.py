# -*- Mode: Python; py-indent-offset: 4 -*-

# this file contains code for loading up an override file.  The override file
# provides implementations of functions where the code generator could not
# do its job correctly.

import sys, string, fnmatch

class Overrides:
    def __init__(self, fp=sys.stdin):
	self.ignores = {}
	self.glob_ignores = []
	self.overrides = {}
	self.kwargs = {}
        self.headers = ''
        self.init = ''
	if fp == None: return
	# read all the components of the file ...
	bufs = map(string.strip, string.split(fp.read(), '%%'))
	if bufs == ['']: return
	for buf in bufs:
	    self.__parse_override(buf)
    def __parse_override(self, buffer):
	pos = string.find(buffer, '\n')
	if pos:
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
    def wants_kwargs(self, name):
	return self.kwargs.has_key(name)
    def get_headers(self):
        return self.headers
    def get_init(self):
        return self.init
