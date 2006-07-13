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
from types import ModuleType

class LazyModule(object):
    def __init__(self, name, locals):
        self._name = name
        self._locals = locals
        self._modname = '%s.%s' % (self._locals.get('__name__'), self._name)

    def __getattr__(self, attr):
        module = __import__(self._name, self._locals, {}, ' ')
        sys.modules[self._modname] = module
        return getattr(module, attr)

_marker = object()

class LazyNamespace(ModuleType):
    def __init__(self, module, locals):
        self._module = module
        self.__dict__.update(locals)
        symbols = {}
        def __getattribute__(_, name, d=self.__dict__, module=module):
            v = d.get(name, _marker)
            if v is not _marker:
                return v

            if name in symbols:
                return module._get_symbol(d, name)
            elif name == 'glade':
                m = __import__('_glade', {}, {}, ' ')
                d['glade'] = m
                return m
            elif name == '_gtk':
                m = __import__('gtk._gtk', {}, {}, ' ')
                d['_gtk'] = m
                return m
            if name == '__dict__':
                return d
            elif name == '__bases__':
                return (ModuleType,)

            raise AttributeError(name)

        LazyNamespace.__getattribute__ = __getattribute__

        for symbol in module._get_symbol_names():
            symbols[symbol] = None
        self._symbols = symbols

        ModuleType.__init__(self, self.__name__)

    def ___getattr__(self, name):
        if name in self._symbols:
            self._module._get_symbol(self.__dict__, name)
        elif name == 'glade':
            module = __import__('_glade', {}, {}, ' ')
            self.__dict__['glade'] = module
            return module
        elif name == '_gtk':
            module = __import__('gtk._gtk', {}, {}, ' ')
            self.__dict__['_gtk'] = module
            return module
        return self.__dict__[name]
