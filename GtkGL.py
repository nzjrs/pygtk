import _gtkgl
import gtk; _gtk = gtk; del gtk

class GtkGLArea(_gtk.GtkDrawingArea):
	def __init__(self, attrList=(), share=None, _obj=None):
		if _obj: self._o = _obj; return
		if share:
			self._o = _gtkgl.gtk_gl_area_share_new(attrList,
							       share._o)
		else:
			self._o = _gtkgl.gtk_gl_area_new(attrList)
	def make_current(self):
		return _gtkgl.gtk_gl_area_make_current(self._o)
	def swap_buffers(self):
		_gtkgl.gtk_gl_area_swap_buffers(self._o)
_gtk._name2cls['GtkGLArea'] = GtkGLArea

def query():
	return _gtkgl.gdk_gl_query()
def wait_gdk():
	_gtkgl.gdk_gl_wait_gdk()
def wait_gl():
	_gtkgl.gdk_gl_wait_gl()
def use_gdk_font(font, first, count, list_base):
	_gtkgl.gdk_gl_use_gdk_font(font, first, count, list_base)

