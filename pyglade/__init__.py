__all__ = ['build', 'style', 'xmlparse']
import build, xmlparse

def construct(filename):
	tree = xmlparse.read(filename)['gtk-interface']
	wtree = build.WidgetTree(tree)
	tree.destroy()
	return wtree

import sys
sys.stderr.write(
	'*** You should really think about switching over to libglade\n')
del sys
