# -*- Mode: Python; py-indent-offset: 4 -*-

# this can go when things are a little further along
try:
    import ltihooks
    del ltihooks
except ImportError:
    pass

# hack so that ltihooks is used when importing ExtensionClass ...
import ExtensionClass
del ExtensionClass

import gobject
del gobject

# load the required modules:
from GTK import *
import GDK
from _gtk import *

# old names compatibility ...
mainloop = main
def mainquit(*args):
    main_quit()
mainiteration = main_iteration

load_font = GdkFont
load_fontset = fontset_load
create_pixmap = GtkPixmap
create_pixmap_from_xpm = pixmap_create_from_xpm
create_pixmap_from_xpm_d = pixmap_create_from_xpm_d

