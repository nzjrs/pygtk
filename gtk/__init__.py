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

from types import ModuleType as _module
from warnings import warn as _warn

# this can go when things are a little further along
try:
    import ltihooks, sys
    sys.path.insert(1, 'gobject')
    del ltihooks, sys
except ImportError:
    pass

import gobject as _gobject

# load the required modules:
import keysyms
from _gtk import *
import gdk # this is created by the _gtk import

threads_init = gdk.threads_init
threads_enter = gdk.threads_enter
threads_leave = gdk.threads_leave

gdk.INPUT_READ      = _gobject.IO_IN | _gobject.IO_HUP | _gobject.IO_ERR
gdk.INPUT_WRITE     = _gobject.IO_OUT | _gobject.IO_HUP
gdk.INPUT_EXCEPTION = _gobject.IO_PRI

# other deprecated symbols
class _Deprecated:
    def __init__(self, func, oldname, module=''):
        self.func = func
        self.oldname = oldname
        self.name = func.__name__
        if module:
            self.module = module
        else:
            self.module = 'gtk'
            
    def __repr__(self):
        return '<deprecated function %s>' % (self.oldname)
    
    def __call__(self, *args, **kwargs):
        oldname = 'gtk.' + self.oldname
	newname = self.module + '.' + self.name
        message = '%s is deprecated, use %s instead' % (oldname, newname)
        # DeprecationWarning is imported from _gtk, so it's not the same
        # as the one found in exceptions.
        _warn(message, DeprecationWarning, 2)
        try:
	    return self.func(*args, **kwargs)
	except TypeError, e:
	    msg = str(e).replace(self.name, self.oldname)
	    raise TypeError(msg)

class _DeprecatedConstant:
    def __init__(self, value, name, suggestion):
        self._v = value
        self._name = name
        self._suggestion = suggestion
        
    def _deprecated(self, value):
        message = '%s is deprecated, use %s instead' % (self._name,
                                                        self._suggestion)
        _warn(message, DeprecationWarning, 3)
        return value
    
    __nonzero__ = lambda self: self._deprecated(self._v == True)
    __int__     = lambda self: self._deprecated(int(self._v))
    __str__     = lambda self: self._deprecated(str(self._v))
    __repr__    = lambda self: self._deprecated(repr(self._v))
    __cmp__     = lambda self, other: self._deprecated(cmp(self._v, other))
        
# _gobject deprecation
class _GObjectWrapper(_module):
    _gobject = _gobject
    def __getattr__(self, attr):
        _warn('gtk._gobject is deprecated, use gobject directly instead',
	      DeprecationWarning, 2)
        return getattr(self._gobject, attr)
    
# old names compatibility ...
idle_add       = _Deprecated(_gobject.idle_add, 'idle_add', 'gobject')
idle_remove    = _Deprecated(_gobject.source_remove, 'idle_remove', 'gobject')
timeout_add    = _Deprecated(_gobject.timeout_add, 'timeout_add', 'gobject')
timeout_remove = _Deprecated(_gobject.source_remove, 'timeout_remove', 'gobject')
input_add      = _Deprecated(_gobject.io_add_watch, 'input_add', 'gobject')
input_add_full = _Deprecated(_gobject.io_add_watch, 'input_add_full', 'gobject')
input_remove   = _Deprecated(_gobject.source_remove, 'input_remove', 'gobject')

mainloop                 = _Deprecated(main, 'mainloop')
mainquit                 = _Deprecated(main_quit, 'mainquit')
mainiteration            = _Deprecated(main_iteration, 'mainiteration')
load_font                = _Deprecated(gdk.Font, 'load_font', 'gtk.gdk')
load_fontset             = _Deprecated(gdk.fontset_load, 'load_fontset', 'gtk.gdk')
create_pixmap            = _Deprecated(gdk.Pixmap, 'create_pixmap', 'gtk.gdk')
create_pixmap_from_xpm   = _Deprecated(gdk.pixmap_create_from_xpm,
                                       'pixmap_create_from_xpm', 'gtk.gdk')
create_pixmap_from_xpm_d = _Deprecated(gdk.pixmap_create_from_xpm_d,
                                       'pixmap_create_from_xpm_d', 'gtk.gdk')

TRUE = _DeprecatedConstant(True, 'gtk.TRUE', 'True')
FALSE = _DeprecatedConstant(False, 'gtk.FALSE', 'False')
	    
_gobject = _GObjectWrapper('gtk._gobject')

del _Deprecated, _DeprecatedConstant, _GObjectWrapper, _module, 
