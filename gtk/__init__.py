# -*- Mode: Python; py-indent-offset: 4 -*-

# this can go when things are a little further along
import ltihooks
del ltihooks

# hack so that ltihooks is used when importing ExtensionClass ...
import ExtensionClass
del ExtensionClass

# load the required modules:
from GTK import *
import GDK
from _gtk import *

# old names compatibility ...
mainloop = main
def mainquit(*args):
    main_quit()
mainiteration = main_iteration

load_font = font_load
load_fontset = fontset_load
create_pixmap = pixmap_new
create_pixmap_from_xpm = pixmap_create_from_xpm
create_pixmap_from_xpm_d = pixmap_create_from_xpm_d

# for importing ...
__all__ = ['_gtk', 'GTK', 'GDK', 'imlib', 'libglade', 'gl' ]

