#!/usr/bin/env python
'''Toolbar Demo

This demonstration demonstrates the use of toolbars in GTK+'''

description = 'Toolbars'

import gtk

folder_icon = [
    "20 19 5 1",
    "  c None",
    ". c #000000",
    "X c #FFFFFF",
    "o c #FFFF00",
    "O c #7F7F00",
    "                    ",
    "                    ",
    "                    ",
    "           ...      ",
    "          .   . .   ",
    "               ..   ",
    "   ...        ...   ",
    "  .oXo.......       ",
    "  .XoXoXoXoX.       ",
    "  .oXoXoXoXo.       ",
    "  .XoXo...........  ",
    "  .oXo.OOOOOOOOO.   ",
    "  .Xo.OOOOOOOOO.    ",
    "  .o.OOOOOOOOO.     ",
    "  ..OOOOOOOOO.      ",
    "  ...........       ",
    "                    ",
    "                    ",
    "                    "
]

def set_orient_cb(button, toolbar, orient):
    toolbar.set_orientation(orient)
def set_style_cb(button, toolbar, style):
    toolbar.set_style(style)
def set_tooltips_cb(button, toolbar, enable):
    toolbar.set_tooltips(enable)

def main():
    win = gtk.Window()
    if __name__ == '__main__':
	win.connect('destroy', lambda win: gtk.main_quit())

    win.set_title("Toolbar")
    win.set_policy(gtk.FALSE, gtk.TRUE, gtk.TRUE)
    win.set_border_width(5)

    pix, mask = gtk.gdk.pixmap_colormap_create_from_xpm_d(None,
                                                          win.get_colormap(),
                                                          None, folder_icon)

    toolbar = gtk.Toolbar()
    win.add(toolbar)
    
    button = toolbar.append_item("Horizontal", "Horizontal toolbar layout",
				 None, gtk.Pixmap(pix, mask), None, None)
    button.connect("clicked", set_orient_cb, toolbar,
		   gtk.ORIENTATION_HORIZONTAL)

    button = toolbar.append_item("Vertical", "Vertical toolbar layout",
				 None, gtk.Pixmap(pix, mask), None, None)
    button.connect("clicked", set_orient_cb, toolbar,
		   gtk.ORIENTATION_VERTICAL)

    toolbar.append_space()

    button = toolbar.append_item("Icons", "Only show toolbar icons",
				 None, gtk.Pixmap(pix, mask), None, None)
    button.connect("clicked", set_style_cb, toolbar, gtk.TOOLBAR_ICONS)

    button = toolbar.append_item("Text", "Only show toolbar texts",
				 None, gtk.Pixmap(pix, mask), None, None)
    button.connect("clicked", set_style_cb, toolbar, gtk.TOOLBAR_TEXT)

    button = toolbar.append_item("Both", "Show toolbar icons and text",
				 None, gtk.Pixmap(pix, mask), None, None)
    button.connect("clicked", set_style_cb, toolbar, gtk.TOOLBAR_BOTH)

    toolbar.append_space()

    entry = gtk.Entry()
    toolbar.append_widget(entry, None, None)

    toolbar.append_space()

    button = toolbar.append_item("Enable", "Enable tooltips",
				 None, gtk.Pixmap(pix, mask), None, None)
    button.connect("clicked", set_tooltips_cb, toolbar, gtk.TRUE)

    button = toolbar.append_item("Disable", "Disable tooltips",
				 None, gtk.Pixmap(pix, mask), None, None)
    button.connect("clicked", set_tooltips_cb, toolbar, gtk.FALSE)

    win.show_all()

    if __name__ == '__main__': gtk.main()

if __name__ == '__main__':
    main()
