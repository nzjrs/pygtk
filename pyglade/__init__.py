__all__ = ['build', 'style', 'xmlparse']
import build, xmlparse

def construct(filename):
	tree = xmlparse.read(filename)['gtk-interface']
	wtree = build.WidgetTree(tree)
	tree.destroy()
	return wtree

# for ni ...
try:
	__.__dict__['construct'] = construct
except NameError:
	pass
