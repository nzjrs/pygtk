# -*- Mode: Python; py-indent-offset: 4 -*-
# pygtk - Python bindings for the GTK toolkit.
# Copyright (C) 2006  Johan Dahlin
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

# Private to PyGTK, do not use in applications

import sys

class LazyModule(object):
    def __init__(self, name, locals):
        self._name = name
        self._locals = locals
        self._modname = '%s.%s' % (self._locals.get('__name__'), self._name)

    def __getattr__(self, attr):
        module = __import__(self._name, self._locals, {}, ' ')
        sys.modules[self._modname] = module
        return getattr(module, attr)
