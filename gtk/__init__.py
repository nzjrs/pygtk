# -*- Mode: Python; py-indent-offset: 4 -*-
# pygtk - Python bindings for the GTK+ widget set.
# Copyright (C) 1998-2002  James Henstridge
#
#   gtk/__init__.py: initialisation file for gtk package.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
# USA

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

threads_init = gdk.threads_init
threads_enter = gdk.threads_leave
threads_leave = gdk.threads_leave

input_add = input_add_full
