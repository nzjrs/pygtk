import ltihooks
import gobject
print gobject
import pango
import gtk
from gtk import gdk

TEXT = 'A GtkWidget implemented in PyGTK'

class Widget(gtk.Widget):
    __gsignals__ = { 'realize': 'override',
                     'expose-event' : 'override',
                     'size-allocate': 'override'}   
    def __init__(self):
        self.__gobject_init__()

        self.draw_gc = None
        self.layout = self.create_pango_layout(TEXT)
        self.layout.set_font_description(pango.FontDescription("sans serif 16"))
                                         
    def do_realize(self):
        self.set_flags(self.flags() | gtk.REALIZED)

        self.window = gdk.Window(self.get_parent_window(),
                                 width=self.allocation.width,
                                 height=self.allocation.height,
                                 window_type=gtk.gdk.WINDOW_CHILD,
                                 wclass=gtk.gdk.INPUT_OUTPUT,
                                 event_mask=self.get_events() | gdk.EXPOSURE_MASK)
        self.draw_gc = gdk.GC(self.window,
                              line_width=5,
                              line_style=gdk.SOLID,
                              join_style=gdk.JOIN_ROUND)
	self.window.set_user_data(self)
        self.style.attach(self.window)
        self.style.set_background(self.window, gtk.STATE_NORMAL)
        
        self.send_configure()

    def do_size_allocate(self, allocation):
        self.allocation = allocation

        if self.flags() & gtk.REALIZED:
            self.window.move_resize(*allocation)
            self.send_configure()

    def do_expose_event(self, event):
        self.chain(event)

        x, y, w, h = self.allocation
        width = 10
        self.window.draw_rectangle(self.draw_gc, False,
                                   x + width, y + width,
                                   w - width * 2, h - width * 2)
        fontw, fonth = self.layout.get_pixel_size()
        self.style.paint_layout(self.window, self.state, False,
                                event.area, self, "label",
                                (w - fontw) / 2, (h - fonth) / 2,
                                self.layout)
            
    def send_configure(self):
        allocation = self.allocation
        
        event = gdk.Event(gdk.CONFIGURE)
        event.send_event = True
        event.window = self.window
        event.x = allocation.x
        event.y = allocation.y
        event.width = allocation.width
        event.height = allocation.height
        self.event(event)

gobject.type_register(Widget)

win = gtk.Window()
win.set_title(TEXT)
win.connect('delete-event', gtk.main_quit)
w = Widget()
w.set_size_request(400, 200)
win.add(w)

win.show_all()

gtk.main()
