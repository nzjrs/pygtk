# -*- Mode: Python; py-indent-offset: 4 -*-

# this can go when things are a little further along
try:
    import ltihooks
    del ltihooks
except ImportError:
    pass

# TRUE and FALSE constants ...
FALSE = 0
TRUE  = 1

import gobject
del gobject

# load the required modules:
import keysyms
from _gtk import *
import gdk # this is created by the _gtk import

# old names compatibility ...
mainloop = main
def mainquit(*args):
    main_quit()
mainiteration = main_iteration

load_font = gdk.Font
load_fontset = gdk.fontset_load
create_pixmap = gdk.Pixmap
create_pixmap_from_xpm = gdk.pixmap_create_from_xpm
create_pixmap_from_xpm_d = gdk.pixmap_create_from_xpm_d

