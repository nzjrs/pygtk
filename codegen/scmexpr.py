#!/usr/bin/env python
# -*- Mode: Python; py-indent-offset: 4 -*-
from __future__ import generators

import string
import types
from cStringIO import StringIO

trans = [' '] * 256
for i in range(256):
    if chr(i) in string.letters + string.digits + '_':
        trans[i] = chr(i)
    else:
        trans[i] = '_'
trans = string.join(trans, '')

def parse(fp):
    whitespace = ' \t\n\r\x0b\x0c'
    nonsymbol = whitespace + '();\'"'
    stack = []
    for line in fp:
        pos = 0
        while pos < len(line):
            if line[pos] in whitespace: # ignore whitespace
                pass
            elif line[pos] == ';': # comment
                break
            elif line[pos:pos+2] == "'(":
                pos += 1
                stack.append(())
            elif line[pos] == '(':
                stack.append(())
            elif line[pos] == ')':
                assert len(stack) != 0, 'parentheses do not match'
                closed = stack[-1]
                del stack[-1]
                if stack:
                    stack[-1] += (closed,)
                else:
                    yield closed
            elif line[pos] == '"': # quoted string
                endpos = pos + 1
                chars = []
                while endpos < len(line):
                    if endpos+1 < len(line) and line[endpos] == '\\':
                        endpos += 1
                        if line[endpos] == 'n':
                            chars.append('\n')
                        elif line[endpos] == 'r':
                            chars.append('\r')
                        elif line[endpos] == 't':
                            chars.append('\t')
                        else:
                            chars.append('\\')
                            chars.append(line[endpos])
                    elif line[endpos] == '"':
                        break
                    else:
                        chars.append(line[endpos])
                    endpos += 1
                pos = endpos
                stack[-1] += (''.join(chars),)
            else: # symbol/number
                endpos = pos
                while endpos < len(line) and line[endpos] not in nonsymbol:
                    endpos += 1
                symbol = line[pos:endpos]
                pos = max(pos, endpos-1)
                try: symbol = int(symbol)
                except ValueError:
                    try: symbol = float(symbol)
                    except ValueError: pass
                stack[-1] += (symbol,)
            pos += 1
    if len(stack) != 0:
        raise IOError, "parentheses don't match"


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
            statements = parse(open(arg, 'r'))
        elif arg:
            statements = arg
        else:
            statements = self.parseTree
        for statement in statements:
            self.handle(statement)
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
