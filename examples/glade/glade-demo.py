#!/usr/bin/env python
import sys
import gtk
from gtk import glade

if len(sys.argv) > 1:
    fname = sys.argv[1]
else:
    fname = 'test.glade'

# create widget tree ...
xml = glade.XML(fname)

def gtk_main_quit(*args):
    gtk.main_quit()

xml.signal_autoconnect(locals())

gtk.main()
