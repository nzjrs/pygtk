#!/usr/bin/env python
import sys
if sys.version[:3] == '1.4':
	import ni
from gtk import *
import pyglade

if len(sys.argv) > 1:
	fname = sys.argv[1]
else:
	fname = 'test.glade'

# create widget tree ...
wtree = pyglade.construct(fname)

# if there is anything that needs to be added to the UI, we can access all
# the widgets like this:
try:
	win = wtree.get_widget('window1')
except: pass

# we can connect to any of the signals named in the template:
try:
	wtree.connect('close_window', mainquit)
except: pass

mainloop()
