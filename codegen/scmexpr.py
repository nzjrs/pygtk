#!/usr/bin/env python
# -*- Mode: Python; py-indent-offset: 4 -*-
import string
import types
# get the fastest implementation of StringIO
try:
    from cStringIO import StringIO
except ImportError:
    from StringIO import StringIO

trans = [' '] * 256
for i in range(256):
    if chr(i) in string.letters + string.digits + '_':
	trans[i] = chr(i)
    else:
	trans[i] = '_'
trans = string.join(trans, '')

def parse(fp):
    stack = [()]
    line = fp.readline()
    while line:
	while line:
	    line = string.lstrip(line)
	    if not line:
		break
	    elif line[0] == '(':
		stack.append(())
		line = line[1:]
	    elif line[0] == ')':
		closed = stack[-1]
		del stack[-1]
		stack[-1] = stack[-1] + (closed,)
		line = line[1:]
	    elif line[0] == '"':
		pos = string.index(line[1:], '"')
		stack[-1] = stack[-1] + (eval(line[:pos+2]),)
		line = line[pos+2:]
	    elif line[0] in string.digits:
		str = ""
		while line and line[0] in "0123456789+-.":
		    str = str + line[0]
		    line = line[1:]
		stack[-1] = stack[-1] + (string.atof(str),)
	    elif line[0] == ';':
		break
            elif line[0] == "'":
		line = line[1:] # consume single quote
	    else:
		str = ""
		while line and line[0] not in "(); '\t\r\n":
		    str = str + line[0]
		    line = line[1:]
		stack[-1] = stack[-1] + (str,)
	line = fp.readline()
    if len(stack) != 1:
	raise IOError, "parentheses don't match"
    return stack[0]

class Parser:
    def __init__(self, arg):
	"""Argument is either a string, a parse tree, or file object"""
	if type(arg) == types.StringType:
	    self.filename = arg
	    self.parseTree = parse(open(arg))
	elif type(arg) == types.TupleType:
	    self.filename = '<none>'
	    self.parseTree = arg
	elif type(arg) == types.FileType:
	    self.filename = arg.name
	    self.parseTree = parse(arg)
	else:
	    raise TypeError, 'second arg must be string, tuple or file'
    def startParsing(self, arg=None):
        if type(arg) == types.StringType:
            tuples = parse(open(arg))
        elif arg:
            tuples = arg
        else:
            tuples = self.parseTree
	for tup in tuples:
	    self.handle(tup)
    def handle(self, tup):
	cmd = string.translate(tup[0], trans)
	if hasattr(self, cmd):
	    apply(getattr(self, cmd), tup[1:])
	else:
	    self.unknown(tup)
    def unknown(self, tup):
	pass

_testString = """; a scheme file
(define-func gdk_font_load    ; a comment at end of line
  GdkFont
  ((string name)))

(define-boxed GdkEvent
  gdk_event_copy
  gdk_event_free
  "sizeof(GdkEvent)")
"""

if __name__ == '__main__':
    import sys
    if sys.argv[1:]:
	fp = open(sys.argv[1])
    else:
	fp = StringIO(_testString)
    statements = parse(fp)
    for s in statements:
	print `s`
