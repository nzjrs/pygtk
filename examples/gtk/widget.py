import pygtk
pygtk.require('2.0')

import gobject
import pango
import gtk
from gtk import gdk

if gtk.pygtk_version < (2,3,93):
    print "PyGtk 2.3.93 or later required for this example"
    raise SystemExit

TEXT = 'A GtkWidget implemented in PyGTK'
BORDER_WIDTH = 10

class PyGtkWidget(gtk.Widget):
    __gsignals__ = { 'realize': 'override',
                     'expose-event' : 'override',
                     'size-allocate': 'override',
                     'size-request': 'override',
		     }
    def __init__(self):
        gtk.Widget.__init__(self)

        self.draw_gc = None
        self.layout = self.create_pango_layout(TEXT)
        self.layout.set_font_description(pango.FontDescription("sans serif 16"))
                                         
    def do_realize(self):
        self.set_flags(self.flags() | gtk.REALIZED)

        self.window = gdk.Window(self.get_parent_window(),
                                 width=self.allocation.width,
                                 height=self.allocation.height,
                                 window_type=gdk.WINDOW_CHILD,
                                 wclass=gdk.INPUT_OUTPUT,
                                 event_mask=self.get_events() | gdk.EXPOSURE_MASK)
        self.draw_gc = gdk.GC(self.window,
                              line_width=5,
                              line_style=gdk.SOLID,
                              join_style=gdk.JOIN_ROUND)
	self.window.set_user_data(self)
        self.style.attach(self.window)
        self.style.set_background(self.window, gtk.STATE_NORMAL)

    def do_size_request(self, requisition):
	width, height = self.layout.get_size()
	requisition.width = width // pango.SCALE + BORDER_WIDTH*4
	requisition.height = height // pango.SCALE + BORDER_WIDTH*4

    def do_size_allocate(self, allocation):
        self.allocation = allocation
        if self.flags() & gtk.REALIZED:
            self.window.move_resize(*allocation)

    def do_expose_event(self, event):
        self.chain(event)

        x, y, w, h = self.allocation
        self.window.draw_rectangle(self.draw_gc, False,
                                   x + BORDER_WIDTH, y + BORDER_WIDTH,
                                   w - BORDER_WIDTH * 2, h - BORDER_WIDTH * 2)
        fontw, fonth = self.layout.get_pixel_size()
        self.style.paint_layout(self.window, self.state, False,
                                event.area, self, "label",
                                (w - fontw) / 2, (h - fonth) / 2,
                                self.layout)
            
gobject.type_register(PyGtkWidget)

win = gtk.Window()
win.set_title(TEXT)
win.connect('delete-event', gtk.main_quit)

w = PyGtkWidget()
win.add(w)

win.show_all()

gtk.main()
