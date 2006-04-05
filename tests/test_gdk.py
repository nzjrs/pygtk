import unittest
import gc

from common import gtk

class CallOnDel:
    def __init__(self, callback):
        self.callback = callback
    def __del__(self):
        self.callback()

class GdkTest(unittest.TestCase):
    def testBitmapCreateFromData(self):
        gtk.gdk.bitmap_create_from_data(None, '\x00', 1, 1)

    def testPixmapCreateFromData(self):
        black = gtk.gdk.color_parse('black')
        gtk.gdk.pixmap_create_from_data(None, '\x00', 1, 1, 1,
                                        black, black)
    def testWindow(self):
        common = {'finalized': False}
        def on_finalize():
            common['finalized'] = True
        w = gtk.gdk.Window(None, 200, 200, gtk.gdk.WINDOW_TEMP, 0, 0)
        w.set_data('foo', CallOnDel(on_finalize))
        w.destroy()
        while gtk.events_pending():
            gtk.main_iteration(block=False)
        del w

        # Note that this depends on the mainloop processing an X event so
        # if might fail if the timing is off
        while gc.collect():
            pass
        assert common['finalized']
