
__all__ = ['GTK', 'GDK', 'gtk']

from types import ModuleType as module

def _find_mod(modname):
    d = {}
    exec 'import %s; mod = %s' % (modname, modname) in d
    return d['mod']

class RemapModule(module):
    def __init__(self, name, modulenames):
        module.__init__(self)
        self.__name__ = name
        if isinstance(modulenames, str):
            modulenames = [modulenames]
        self.__modulenames = modulenames
        self.__modules = None
    def __getattr__(self, attr):
        if not self.__modules:
            self.__modules = map(_find_mod, self.__modulenames)
        for mod in self.__modules:
            try:
                value = getattr(mod, attr)
                setattr(self, attr, value)
                return value
            except AttributeError:
                pass
        raise AttributeError("module has no attribute '%s'" % attr)

GTK = RemapModule('GTK', 'gtk')
GDK = RemapModule('GDK', ['gtk.gdk', 'gtk.keysyms'])

class gtkModule(RemapModule):
    def __init__(self):
        RemapModule('gtk', ['gtk', 'gtk.gdk'])
        self.__name__ = 'gtk'
    def __getattr__(self, attr):
        if attr[:3] == 'Gtk':
            value = getattr(_find_mod('gtk'), attr[3:])
            setattr(self, attr, value)
            return value
        elif attr[:3] == 'Gdk':
            value = getattr(_find_mod('gtk.gdk'), attr[3:])
            setattr(self, attr, value)
            return value
        else:
            return RemapModule.__getattr__(self, attr)

gtk = gtkModule()

del RemapModule, gtkModule
