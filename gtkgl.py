import _gtkgl
import gtk; _gtk = gtk; del gtk

# the constants ...
NONE             = 0
USE_GL           = 1
BUFFER_SIZE      = 2
LEVEL            = 3
RGBA             = 4
DOUBLEBUFFER     = 5
STEREO           = 6
AUX_BUFFERS      = 7
RED_SIZE         = 8
GREEN_SIZE       = 9
BLUE_SIZE        = 10
ALPHA_SIZE       = 11
DEPTH_SIZE       = 12
STENCIL_SIZE     = 13
ACCUM_RED_SIZE   = 14
ACCUM_GREEN_SIZE = 15
ACCUM_BLUE_SIZE  = 16
ACCUM_ALPHA_SIZE = 17

X_VISUAL_TYPE_EXT           = 0x22
TRANSPARENT_TYPE_EXT        = 0x23
TRANSPARENT_INDEX_VALUE_EXT = 0x24
TRANSPARENT_RED_VALUE_EXT   = 0x25
TRANSPARENT_GREEN_VALUE_EXT = 0x26
TRANSPARENT_BLUE_VALUE_EXT  = 0x27
TRANSPARENT_ALPHA_VALUE_EXT = 0x28

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

