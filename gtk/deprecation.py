# -*- Mode: Python; py-indent-offset: 4 -*-
# pygtk - Python bindings for the GTK toolkit.
# Copyright (C) 2004-2006  Johan Dahlin
#
#   gtk/deprecation.py: deprecation helpers for gtk
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

import os
import sys
import warnings

from gtk._gtkimpl import DeprecationWarning

def _is_pydoc():
    if sys.argv:
        name = os.path.basename(sys.argv[0])
        if 'pydoc' in name:
            return True

    return False

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
        if not _is_pydoc():
            oldname = 'gtk.' + self.oldname
            newname = self.module + '.' + self.name
            message = '%s is deprecated, use %s instead' % (oldname, newname)
            # DeprecationWarning is imported from _gtk, so it's not the same
            # as the one found in exceptions.
            warnings.warn(message, DeprecationWarning, 2)
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
        if not _is_pydoc():
            message = '%s is deprecated, use %s instead' % (self._name,
                                                            self._suggestion)
            warnings.warn(message, DeprecationWarning, 3)
        return value

    __nonzero__ = lambda self: self._deprecated(self._v == True)
    __int__     = lambda self: self._deprecated(int(self._v))
    __str__     = lambda self: self._deprecated(str(self._v))
    __repr__    = lambda self: self._deprecated(repr(self._v))
    __cmp__     = lambda self, other: self._deprecated(cmp(self._v, other))
