#!/usr/bin/env python
import sys
import gtk
import libglade

if len(sys.argv) > 1:
	fname = sys.argv[1]
else:
	fname = 'test.glade'

# create widget tree ...
xml = libglade.GladeXML(fname)

# if there is anything that needs to be added to the UI, we can access all
# the widgets like this:
win = xml.get_widget('window1')

# we can connect to any of the signals named in the template:
xml.signal_connect('close_window', gtk.mainquit)

gtk.mainloop()
