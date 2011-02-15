#!/usr/bin/env python
"""Spinner

GtkSpinner allows to show that background activity is on-going.
"""
# pygtk version: John Stowers <john.stowers@gmail.com>

import pygtk
pygtk.require('2.0')
import gtk

class SpinnerDemo(gtk.Window):
    def __init__(self, parent=None):
        gtk.Window.__init__(self)
        try:
            self.set_screen(parent.get_screen())
        except AttributeError:
            self.connect('destroy', lambda *w: gtk.main_quit())
        self.set_title(self.__class__.__name__)
        self.set_border_width(5)

        vb = gtk.VBox(spacing=5)
        self.add(vb)

        #sensitive
        hb = gtk.HBox(spacing=5)
        self.ss = gtk.Spinner()
        hb.add(self.ss)
        hb.add(gtk.Entry())
        vb.add(hb)

        #unsensitive
        hb = gtk.HBox(spacing=5)
        self.us = gtk.Spinner()
        hb.add(self.us)
        hb.add(gtk.Entry())
        hb.set_sensitive(False)
        vb.add(hb)

        b = gtk.Button(stock=gtk.STOCK_MEDIA_PLAY)
        b.connect("clicked", self._on_play_clicked)
        vb.add(b)

        b = gtk.Button(stock=gtk.STOCK_MEDIA_STOP)
        b.connect("clicked", self._on_stop_clicked)
        vb.add(b)

        self._on_play_clicked(None)

        self.show_all()

    def _on_play_clicked(self, b):
        self.ss.start()
        self.us.start()

    def _on_stop_clicked(self, b):
        self.ss.stop()
        self.us.stop()

def main():
    SpinnerDemo()
    gtk.main()

if __name__ == '__main__':
    main()
