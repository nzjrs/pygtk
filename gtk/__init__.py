# -*- Mode: Python; py-indent-offset: 4 -*-
# pygtk - Python bindings for the GTK toolkit.
# Copyright (C) 1998-2003  James Henstridge
#
#   gtk/__init__.py: initialisation file for gtk package.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#                                                                             
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#                                                                             
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
# USA

# this can go when things are a little further along
try:
    import ltihooks
    del ltihooks
except ImportError:
    pass

# TRUE and FALSE constants ...
import __builtin__
if not hasattr(__builtin__, 'True'):
    __builtin__.True = (1 == 1)
    __builtin__.False = (1 != 1)
del __builtin__

FALSE = False
TRUE  = True
import gobject as _gobject

# load the required modules:
import keysyms
from _gtk import *
import gdk # this is created by the _gtk import

threads_init = gdk.threads_init
threads_enter = gdk.threads_enter
threads_leave = gdk.threads_leave

idle_add       = _gobject.idle_add
idle_remove    = _gobject.source_remove
timeout_add    = _gobject.timeout_add
timeout_remove = _gobject.source_remove
input_add      = _gobject.io_add_watch
input_add_full = _gobject.io_add_watch
input_remove   = _gobject.source_remove

gdk.INPUT_READ      = _gobject.IO_IN | _gobject.IO_HUP | _gobject.IO_ERR
gdk.INPUT_WRITE     = _gobject.IO_OUT | _gobject.IO_HUP
gdk.INPUT_EXCEPTION = _gobject.IO_PRI

# _gobject deprecation
from types import ModuleType as _module
class _GObjectWrapper(_module):
    from warnings import warn
    warn = staticmethod(warn)
    _gobject = _gobject
    def __getattr__(self, attr):
        self.warn('gtk._gobject is deprecated, use gobject directly instead')
        return getattr(self._gobject, attr)
_gobject = _GObjectWrapper('gtk._gobject')
del _GObjectWrapper, _module

# other deprecated symbols
class _Deprecated:
    from warnings import warn
    warn = staticmethod(warn)
    def __init__(self, func, oldname, module=''):
        self.func = func
        self.oldname = oldname
        self.name = func.__name__
        if module:
            self.module = 'gtk.' + module
        else:
            self.module = 'gtk'
            
    def __repr__(self):
        return '<deprecated function %s>' % (self.oldname)
    
    def __call__(self, *args, **kwargs):
        message = 'gtk.%s is deprecated, use %s.%s instead' % (self.oldname,
                                                               self.module,
                                                               self.name)
        # DeprecationWarning is imported from _gtk, so it's not the same
        # as the one found in exceptions.
        self.warn(message, DeprecationWarning)
        return self.func(*args, **kwargs)


# old names compatibility ...
mainloop = _Deprecated(main, 'mainloop')
mainquit = _Deprecated(main_quit, 'mainquit')
mainiteration = _Deprecated(main_iteration, 'mainiteration')
load_font = _Deprecated(gdk.Font, 'load_font', 'gdk')
load_fontset = _Deprecated(gdk.fontset_load, 'load_fontset', 'gdk')
create_pixmap = _Deprecated(gdk.Pixmap, 'create_pixmap', 'gdk')
create_pixmap_from_xpm = _Deprecated(gdk.pixmap_create_from_xpm,
                                     'pixmap_create_from_xpm', 'gdk')
create_pixmap_from_xpm_d = _Deprecated(gdk.pixmap_create_from_xpm_d,
                                       'pixmap_create_from_xpm_d', 'gdk')

del _Deprecated
