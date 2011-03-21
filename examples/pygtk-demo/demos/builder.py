#!/usr/bin/env python
'''Builder

This is a test of the new gtk builder system.  It is a
fairly straight forward port of the example distributed with gtk.
'''

import pygtk
pygtk.require('2.0')
import gtk

class BuilderDemo(gtk.Window):
    def __init__(self, parent=None, filename="demos/demo.ui"):        
        gtk.Window.__init__(self)
        try:
            self.set_screen(parent.get_screen())
        except AttributeError:
            self.connect('destroy', lambda *w: gtk.main_quit())
        self.set_title(self.__class__.__name__)
        self.set_default_size(440, 250)
        
        # create a new empty Builer object
        builder = gtk.Builder()
        
        # load an XML definition
        builder.add_from_file(filename)
        
        # We must get the vbox and reparent it to self
        # since we don't use the main window defined in the ui file
        vbox = builder.get_object("vbox1")
        vbox.reparent(self)
        
        # autoconnect the signals from the instance self
        builder.connect_signals(self)

        self.show_all()

    def about_activate(self, action, data=None):
        about_dlg = gtk.AboutDialog()
        about_dlg.set_name("GtkBuilder demo")
        about_dlg.run()
        about_dlg.destroy()

    def quit_activate(self, action, data=None):
        gtk.main_quit()
    
    def main(self):
        gtk.main()

if __name__ == '__main__':
    build = BuilderDemo(None, "demo.ui")
    build.main()
