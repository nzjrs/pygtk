"""This file creates a GTK rc file to work with a widget tree created from
the XML output of GLADE.

It outputs both the style definitions, and links to the names of the widgets
that use it.  The style namesare of the form "PyGLADE_<name>_style".  Then
there is a number of "widget <path> style PyGLADE_<name>_style" lines that
bind the styles to particular widgets.
"""
import string

# Get the fastest implementation of StringIO ...
try:
	from cStringIO import StringIO
except ImportError:
	from StringIO import StringIO

error = 'pyglade.style.error'

def convert_colour(str):
	r, g, b = map(string.atoi, string.split(str, ','))
	return "{ %.3f, %.3f, %.3f }" % (r/255.0, g/255.0, b/255.0)

def output_widget_names(fp, widget, parent):
	parent = parent + '.' + widget.name
	if widget.has_key('style_name'):
		fp.write('widget "%s" style "PyGLADE_%s_style"\n' %
			 (parent[1:], widget.style_name))
	if not widget.has_key('widget'):
		return
	children = widget.widget
	if type(children) != type(()): children = (children,)
	for child in children:
		output_widget_names(fp, child, parent)

def write_stream(tree, fp):
	if tree.has_key('gtk-interface'):
		# let the user off if they just used the top of the parse tree
		# instead of the GTK-Interface node
		tree = tree['gtk-interface']
	if tree.tag != 'gtk-interface':
		raise error, "Not at the top node of the GLADE output"

	styles = tree.style
	if type(styles) != type(()): styles = (styles,)
	for style in styles:
		name = "PyGLADE_" + style.style_name + "_style"
		fp.write('style "%s"\n{\n' % (name,))
		if style.has_key('style_font'):
			fp.write('  font = "%s"\n' % (style.style_font,))
		for state in ('NORMAL', 'ACTIVE', 'PRELIGHT',
			      'SELECTED', 'INSENSITIVE'):
			for colour in 'fg', 'bg', 'text', 'base':
				key = colour + ':' + string.lower(state)
				if style.has_key(key):
					cstr = convert_colour(style[key])
					fp.write("  %s[%s] = %s\n" % (
						colour, string.upper(state),
						cstr))
			key = "bg_pixmap:" + state
			if style.has_key(key):
				fp.write("  bg_pixmap[%s] = \"%s\"\n" % (
					state, style[key]))
		fp.write("}\n\n")
	if not tree.has_key('widget'):
		return
	children = tree.widget
	if type(children) != type(()): children = (children,)
	for child in children:
		output_widget_names(fp, child, '')

def write(tree, fname):
	write_stream(tree, open(fname, "w"))

def as_string(tree):
	fp = StringIO()
	write_stream(tree, fp)
	return fp.getvalue()
