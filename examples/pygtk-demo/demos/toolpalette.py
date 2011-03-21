#!/usr/bin/env python
"""Tool Palette

A tool palette widget shows groups of toolbar items as a grid of icons or a list of names.
"""
# pygtk version: John Stowers <john.stowers@gmail.com>

import pygtk
pygtk.require('2.0')
import gtk

class ToolPaletteDemo(gtk.Window):
    def __init__(self, parent=None):
        gtk.Window.__init__(self)
        try:
            self.set_screen(parent.get_screen())
        except AttributeError:
            self.connect('destroy', lambda *w: gtk.main_quit())
        self.set_title(self.__class__.__name__)
        self.set_border_width(8)
        self.set_default_size(400,400)

        vb = gtk.VBox(spacing=6)

        m = gtk.ListStore(str, object)
        m.append( ("Vertical", gtk.ORIENTATION_VERTICAL) )
        m.append( ("Horizontal", gtk.ORIENTATION_HORIZONTAL) )
        cb = gtk.ComboBox(m)
        cell = gtk.CellRendererText()
        cb.pack_start(cell)
        cb.add_attribute(cell, 'text',0)
        cb.set_active(0)
        cb.connect("changed", self._orientation_changed)
        vb.pack_start(cb, False, False)

        m = gtk.ListStore(str, object)
        m.append( ("Icons", gtk.TOOLBAR_ICONS) )
        m.append( ("Text", gtk.TOOLBAR_TEXT) )
        m.append( ("Both", gtk.TOOLBAR_BOTH) )
        m.append( ("Both: Horizontal", gtk.TOOLBAR_BOTH_HORIZ) )
        cb = gtk.ComboBox(m)
        cell = gtk.CellRendererText()
        cb.pack_start(cell)
        cb.add_attribute(cell, 'text',0)
        cb.set_active(0)
        cb.connect("changed", self._style_changed)
        vb.pack_start(cb, False, False)

        self.palette = gtk.ToolPalette()
        self._load_stock_items()

        self.sw = gtk.ScrolledWindow()
        self.sw.set_policy (gtk.POLICY_NEVER, gtk.POLICY_AUTOMATIC)
        self.sw.add(self.palette)
        vb.pack_start(self.sw, True, True)

        self.add(vb)
        self.show_all()

    def _orientation_changed(self, cb):
        i = cb.get_active_iter()
        if i:
            val = cb.get_model().get_value(i, 1)
            if val == gtk.ORIENTATION_HORIZONTAL:
                self.sw.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_NEVER)
            else:
                self.sw.set_policy(gtk.POLICY_NEVER, gtk.POLICY_AUTOMATIC)
            self.palette.set_orientation(val)

    def _style_changed(self, cb):
        i = cb.get_active_iter()
        if i:
            self.palette.set_style(cb.get_model().get_value(i, 1))

    def _load_stock_items(self):
        groups = {
            "a":gtk.ToolItemGroup("Stock Icons (A-F)"),
            "g":gtk.ToolItemGroup("Stock Icons (G-N)"),
            "o":gtk.ToolItemGroup("Stock Icons (O-R)"),
            "s":gtk.ToolItemGroup("Stock Icons (S-Z)")
        }
        group = groups["a"]
        for g in groups.values():
            self.palette.add(g)

        items = gtk.stock_list_ids()
        items.sort()
        for i in items:
            #stock id names are in form gtk-foo, sort into one of the groups above
            try:
                group = groups[i[4]]
            except KeyError:
                pass

            b = gtk.ToolButton(i)
            b.set_tooltip_text(i)
            b.set_is_important(True)
            info = gtk.stock_lookup(i)
            if info:
                b.set_label(info[1].replace("_",""))
            else:
                b.set_label(i)

            group.insert(b,-1)
            
            

def main():
    ToolPaletteDemo()
    gtk.main()

if __name__ == '__main__':
    main()
