#!/usr/bin/env python
# Gtk+ client-side-windows demo in python. Also demonstrates how to use
# ctypes to wrap otherwise unexposed GObject API
# John Stowers

import pygtk
pygtk.require('2.0')
import gobject
import cairo
import gtk
import gtk.gdk as gdk

#hacks for using functions not exposed in this pygtk version
import ctypes
cgdk = ctypes.CDLL("libgdk-x11-2.0.so")

DBG = True

class _PyGObject_Functions(ctypes.Structure):
    _fields_ = [
        ('register_class',
         ctypes.PYFUNCTYPE(ctypes.c_void_p, ctypes.c_char_p,
                           ctypes.c_int, ctypes.py_object,
                           ctypes.py_object)),
        ('register_wrapper',
         ctypes.PYFUNCTYPE(ctypes.c_void_p, ctypes.py_object)),
        ('register_sinkfunc',
         ctypes.PYFUNCTYPE(ctypes.py_object, ctypes.c_void_p)),
        ('lookupclass',
         ctypes.PYFUNCTYPE(ctypes.py_object, ctypes.c_int)),
        ('newgobj',
         ctypes.PYFUNCTYPE(ctypes.py_object, ctypes.c_void_p)),
        ]

class PyGObjectCPAI(object):
    def __init__(self):
        addr = ctypes.pythonapi.PyCObject_AsVoidPtr(
            ctypes.py_object(gobject._PyGObject_API))
        self._api = _PyGObject_Functions.from_address(addr)

    def pygobject_new(self, addr):
        return self._api.newgobj(addr)

cgobject = PyGObjectCPAI()

def gdk_offscreen_window_set_embedder(window, embedder):
    cgdk.gdk_offscreen_window_set_embedder(hash(window), hash(embedder))

def gdk_offscreen_window_get_pixmap(window):
    pm = cgdk.gdk_offscreen_window_get_pixmap(hash(window))
    return cgobject.pygobject_new(ctypes.c_void_p(pm))

def dbg(s):
    if DBG: print s

class GtkMirrorBin(gtk.Bin):
    __gsignals__ = {
        "damage_event"  :   "override"
    }

    def __init__(self):
        gtk.Bin.__init__(self)

        self.child = None
        self.offscreen_window = None

        self.unset_flags(gtk.NO_WINDOW)

    def _to_child(self, widget_x, widget_y):
        return widget_x, widget_y

    def _to_parent(self, offscreen_x, offscreen_y):
        return offscreen_x, offscreen_y

    def _pick_offscreen_child(self, offscreen_window, widget_x, widget_y):
        dbg("pick_child")

        if self.child and self.child.flags() & gtk.VISIBLE:
            x,y = self._to_child(widget_x, widget_y)
            ca = self.child.allocation
            if (x >= 0 and x < ca.width and y >= 0 and y < ca.height):
                return self.offscreen_window
        return None

    def _offscreen_window_to_parent(self, offscreen_window, offscreen_x, offscreen_y, parent_x, parent_y):
        dbg("to_parent")
        x,y = self._to_parent(offscreen_x, offscreen_y)

        #FIXME: parent_x and parent_y are gpointers...
        px = ctypes.c_void_p(hash(parent_x))
        px.contents = x
        py = ctypes.c_void_p(hash(parent_y))
        py.contents = y

    def _offscreen_window_from_parent(self, parent_window, parent_x, parent_y, offscreen_x, offscreen_y):
        dbg("to_child")
        x,y = self._to_child(parent_x, parent_y)

        #FIXME: offscreen_x and offscreen_y are gpointers...
        ox = ctypes.c_void_p(hash(offscreen_x))
        ox.contents = x
        oy = ctypes.c_void_p(hash(offscreen_y))
        oy.contents = y

    def do_realize(self):
        dbg("do_realize")

        self.set_flags(gtk.REALIZED)

        border_width = self.border_width

        w = self.allocation.width - 2*border_width
        h = self.allocation.height - 2*border_width

        self.window = gdk.Window(
                self.get_parent_window(),
                x=self.allocation.x + border_width,
                y=self.allocation.y + border_width,
                width=w,
                height=h,
                window_type=gdk.WINDOW_CHILD,
                event_mask=self.get_events() 
                        | gdk.EXPOSURE_MASK
                        | gdk.POINTER_MOTION_MASK
                        | gdk.BUTTON_PRESS_MASK
                        | gdk.BUTTON_RELEASE_MASK
                        | gdk.SCROLL_MASK
                        | gdk.ENTER_NOTIFY_MASK
                        | gdk.LEAVE_NOTIFY_MASK,
                visual=self.get_visual(),
                colormap=self.get_colormap(),
                wclass=gdk.INPUT_OUTPUT)

        self.window.set_user_data(self)
        self.window.connect("pick-embedded-child", self._pick_offscreen_child)

        if self.child and self.child.flags() & gtk.VISIBLE:
            w = self.child.allocation.width
            h = self.child.allocation.height

        self.offscreen_window = gdk.Window(
                self.get_root_window(),
                x=self.allocation.x + border_width,
                y=self.allocation.y + border_width,
                width=w,
                height=h,
                window_type=gdk.WINDOW_OFFSCREEN,
                event_mask=self.get_events() 
                        | gdk.EXPOSURE_MASK
                        | gdk.POINTER_MOTION_MASK
                        | gdk.BUTTON_PRESS_MASK
                        | gdk.BUTTON_RELEASE_MASK
                        | gdk.SCROLL_MASK
                        | gdk.ENTER_NOTIFY_MASK
                        | gdk.LEAVE_NOTIFY_MASK,
                visual=self.get_visual(),
                colormap=self.get_colormap(),
                wclass=gdk.INPUT_OUTPUT)
        self.offscreen_window.set_user_data(self)

        if self.child:
            self.child.set_parent_window(self.offscreen_window)

#        self.offscreen_window.set_embedder(self.window)
        gdk_offscreen_window_set_embedder(self.offscreen_window, self.window)

        self.offscreen_window.connect("to-embedder", self._offscreen_window_to_parent)
        self.offscreen_window.connect("from-embedder", self._offscreen_window_from_parent)

        self.style.attach(self.window)
        self.style.set_background(self.window, gtk.STATE_NORMAL)
        self.style.set_background(self.offscreen_window, gtk.STATE_NORMAL)

        self.offscreen_window.show()

    def do_unrealize(self):
        self.offscreen_window.set_user_data(None)
        self.offscreen_window = None

    def do_add(self, widget):
        dbg("do_add")
        if not self.child:
            widget.set_parent_window(self.offscreen_window)
            widget.set_parent(self)
            self.child = widget
        else:
            raise Exception("GtkMirrorBind cannot have more than one child")

    def do_remove(self, widget):
        dbg("do_remove")
        was_visible = widget.flags() & gtk.VISIBLE
        if self.child == widget:
            widget.unparent()
            self.child = None
            if was_visible and (self.flags() & gtk.VISIBLE):
                self.queue_resize()

    def do_forall(self, internal, callback, data):
        dbg("do_forall")
        if self.child:
            callback(self.child, data)

    def do_size_request(self, r):
        dbg("do_size_request")

        cw, ch = 0,0;
        if self.child and (self.child.flags() & gtk.VISIBLE):
            cw, ch =  self.child.size_request()

        r.width = self.border_width * 2 + cw + 10
        r.height = self.border_width * 2 + ch * 2 + 10

    def do_child_type(self):
        dbg("do_child_type")
        #FIXME: This never seems to get called...
        if self.child:
            return None
        return gtk.Widget.__gtype__

    def do_size_allocate(self, allocation):
        dbg("do_size_allocate")

        self.allocation = allocation

        border_width = self.border_width
        w = self.allocation.width - 2*border_width
        h = self.allocation.height - 2*border_width

        if self.flags() & gtk.REALIZED:
            self.window.move_resize(
                            allocation.x + border_width,
                            allocation.y + border_width,
                            w,h)

        if self.child and self.child.flags() & gtk.VISIBLE:
            cw, ch = self.child.get_child_requisition()
            ca = gtk.gdk.Rectangle(x=0,y=0,width=cw,height=ch)

            if self.flags() & gtk.REALIZED:
                self.offscreen_window.move_resize(
                            allocation.x + border_width,
                            allocation.y + border_width,
                            cw, ch)

            self.child.size_allocate(ca)

    def do_damage_event(self, eventexpose):
        dbg("do_damage")
        #invalidate the whole window
        self.window.invalidate_rect(None, False)
        return True

    def do_expose_event(self, event):
        dbg("do_expose")
        if self.flags() & gtk.VISIBLE and self.flags() & gtk.MAPPED:
            if event.window == self.window:
                #FIXME: beware pointers...
                pm = gdk_offscreen_window_get_pixmap(self.offscreen_window)
                w,h = pm.get_size()

                dbg("expose both %dx%d" % (w,h))

                cr = self.window.cairo_create()

                cr.save()

                cr.rectangle(0,0,w,h)
                cr.clip()

                #paint the offscreen child
                cr.set_source_pixmap(pm, 0, 0)
                cr.paint()

                cr.restore()

                matrix = cairo.Matrix(1.0, 0.0, 0.3, 1.0, 0.0, 0.0)
                matrix.scale(1.0, -1.0)
                matrix.translate(-10, -3 * h - 10)
                cr.transform(matrix)

                cr.rectangle(0,h,w,h)
                cr.clip()

                cr.set_source_pixmap(pm, 0, h)

                #create linear gradient
                mask = cairo.LinearGradient(0.0, h, 0.0, 2*h)
                mask.add_color_stop_rgba(0.0, 0.0, 0.0, 0.0, 0.0)
                mask.add_color_stop_rgba(0.25, 0.0, 0.0, 0.0, 0.01)
                mask.add_color_stop_rgba(0.5,  0.0, 0.0, 0.0, 0.25)
                mask.add_color_stop_rgba(0.75, 0.0, 0.0, 0.0, 0.5)
                mask.add_color_stop_rgba(1.0,  0.0, 0.0, 0.0, 1.0)
                
                #paint the reflection
                cr.mask(mask)

            elif event.window == self.offscreen_window:
                dbg("expose offscreen")

                self.style.paint_flat_box(
                                event.window,
                                gtk.STATE_NORMAL, gtk.SHADOW_NONE,
                                event.area, self, "blah",
                                0, 0, -1, -1)

                if self.child:
                    self.propagate_expose(
                                self.child,
                                event)

        return False

gobject.type_register(GtkMirrorBin)

if __name__ == "__main__":
    w = gtk.Window()
    w.connect("delete-event", gtk.main_quit)
    w.set_border_width(10)

    hb = gtk.HBox(6)
    hb.pack_start(gtk.Button(stock=gtk.STOCK_GO_BACK))
    hb.pack_start(gtk.Entry())

    bin = GtkMirrorBin()
    bin.add(hb)

    w.add(bin)
    w.show_all()
    gtk.main()

