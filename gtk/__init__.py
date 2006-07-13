# -*- Mode: Python; py-indent-offset: 4 -*-
# pygtk - Python bindings for the GTK toolkit.
# Copyright (C) 1998-2003  James Henstridge
#               2004-2006  Johan Dahlin
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

import sys

# this can go when things are a little further along
try:
    import ltihooks
    # pyflakes
    ltihooks
except ImportError:
    ltihooks = None

# For broken embedded programs which forgot to call Sys_SetArgv
if not hasattr(sys, 'argv'):
    sys.argv = []

# load the required modules:
import gobject as _gobject

ver = getattr(_gobject, 'pygobject_version', ())
if ver < (2, 11, 1):
    raise ImportError(
        "PyGTK requires PyGObject 2.11.1 or higher, but %s was found" % (ver,))

from gtk import _gtkimpl
import gdk

if ltihooks:
    ltihooks.uninstall()
    del ltihooks

from gtk._lazyutils import LazyNamespace, LazyModule
from gtk.deprecation import _Deprecated, _DeprecatedConstant

def _init():
    import sys

    try:
        sys_path = sys.path[:]

        try:
            _gtkimpl.init_check()
        except RuntimeError, e:
            print >> sys.stderr, "WARNING: %s" % e
    finally:
        # init_check calls PySys_SetArgv which calls sys.path.insert(0, ''),
        # which causes problems for pychecker, restore it if modified.
        if sys.path != sys_path:
            sys.path = sys_path

    # install the default log handlers
    _gtkimpl.add_log_handlers()

keysyms = LazyModule('keysyms', locals())

_init()

# CAPI
_PyGtk_API = _gtkimpl._PyGtk_API

gdk.INPUT_READ      = _gobject.IO_IN | _gobject.IO_HUP | _gobject.IO_ERR
gdk.INPUT_WRITE     = _gobject.IO_OUT | _gobject.IO_HUP
gdk.INPUT_EXCEPTION = _gobject.IO_PRI

# old names compatibility ...
idle_add       = _Deprecated(_gobject.idle_add, 'idle_add', 'gobject')
idle_remove    = _Deprecated(_gobject.source_remove, 'idle_remove', 'gobject')
timeout_add    = _Deprecated(_gobject.timeout_add, 'timeout_add', 'gobject')
timeout_remove = _Deprecated(_gobject.source_remove, 'timeout_remove',
                             'gobject')
input_add      = _Deprecated(_gobject.io_add_watch, 'input_add', 'gobject')
input_add_full = _Deprecated(_gobject.io_add_watch, 'input_add_full',
                             'gobject')
input_remove   = _Deprecated(_gobject.source_remove, 'input_remove', 'gobject')

mainloop                 = _Deprecated(_gtkimpl.main, 'mainloop')
mainquit                 = _Deprecated(_gtkimpl.main_quit, 'mainquit')
mainiteration            = _Deprecated(_gtkimpl.main_iteration,
                                       'mainiteration')
load_font                = _Deprecated(gdk.Font, 'load_font', 'gtk.gdk')
load_fontset             = _Deprecated(gdk.fontset_load, 'load_fontset',
                                       'gtk.gdk')
create_pixmap            = _Deprecated(gdk.Pixmap, 'create_pixmap', 'gtk.gdk')
create_pixmap_from_xpm   = _Deprecated(gdk.pixmap_create_from_xpm,
                                       'pixmap_create_from_xpm', 'gtk.gdk')
create_pixmap_from_xpm_d = _Deprecated(gdk.pixmap_create_from_xpm_d,
                                       'pixmap_create_from_xpm_d', 'gtk.gdk')

threads_init = _Deprecated(gdk.threads_init, 'threads_init', 'gtk.gdk')
threads_enter = _Deprecated(gdk.threads_enter, 'threads_enter', 'gtk.gdk')
threads_leave = _Deprecated(gdk.threads_leave, 'threads_leave', 'gtk.gdk')

TRUE = _DeprecatedConstant(True, 'gtk.TRUE', 'True')
FALSE = _DeprecatedConstant(False, 'gtk.FALSE', 'False')

# Can't figure out how to deprecate gdk.Warning
gdk.Warning = Warning

# We don't want to export this
del _Deprecated, _DeprecatedConstant, _gobject, _init, _lazyutils

# Do this as late as possible, so programs like pyflakes can check
# everything above
from gtk._gtkimpl import *

# For testing, so you can just turn of dynamicnamespace in gtk.override
if hasattr(_gtkimpl, '_get_symbol_names'):
    import gtk
    sys.modules['gtk'] = LazyNamespace(_gtkimpl, locals())
    sys.modules['gtk.glade'] = LazyModule('_glade', {})

