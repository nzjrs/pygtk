#!/usr/bin/env python
'''Button Box Test

This demo shows various button box configurations available.  It also
uses stock buttons, and use of mnemonics for navigation.'''

description = 'Button Boxes'

import gtk

def create_bbox(horizontal=gtk.TRUE, title=None, spacing=0,
		layout=gtk.BUTTONBOX_SPREAD):
    frame = gtk.GtkFrame(title)

    if horizontal:
	bbox = gtk.GtkHButtonBox()
    else:
	bbox = gtk.GtkVButtonBox()

    bbox.set_border_width(5)
    bbox.set_layout(layout)
    bbox.set_spacing(spacing)
    frame.add(bbox)

    button = gtk.GtkButton(stock='gtk-button-ok')
    bbox.add(button)

    button = gtk.GtkButton(stock='gtk-button-cancel')
    bbox.add(button)

    button = gtk.GtkButton(stock='gtk-help')
    bbox.add(button)

    return frame

def main():
    win = gtk.GtkWindow()
    if __name__ == '__main__':
	win.connect('destroy', lambda win: gtk.main_quit())

    win.set_title('Button Boxes')
    win.set_border_width(10)

    main_vbox = gtk.GtkVBox()
    win.add(main_vbox)
    
    frame_horiz = gtk.GtkFrame("Horizontal Button Boxes")
    main_vbox.pack_start(frame_horiz, padding=10)

    vbox = gtk.GtkVBox()
    vbox.set_border_width(10)
    frame_horiz.add(vbox)

    vbox.pack_start(create_bbox(gtk.TRUE, "Spread", 40, gtk.BUTTONBOX_SPREAD),
		    padding=0)
    vbox.pack_start(create_bbox(gtk.TRUE, "Edge", 40, gtk.BUTTONBOX_EDGE),
		    padding=5)
    vbox.pack_start(create_bbox(gtk.TRUE, "Start", 40, gtk.BUTTONBOX_START),
		    padding=5)
    vbox.pack_start(create_bbox(gtk.TRUE, "End", 40, gtk.BUTTONBOX_END),
		    padding=5)

    frame_vert = gtk.GtkFrame("Vertical Button Boxes")
    main_vbox.pack_start(frame_vert, padding=10)

    hbox = gtk.GtkHBox()
    hbox.set_border_width(10)
    frame_vert.add(hbox)

    hbox.pack_start(create_bbox(gtk.FALSE, "Spread", 40, gtk.BUTTONBOX_SPREAD),
		    padding=0)
    hbox.pack_start(create_bbox(gtk.FALSE, "Edge", 40, gtk.BUTTONBOX_EDGE),
		    padding=5)
    hbox.pack_start(create_bbox(gtk.FALSE, "Start", 40, gtk.BUTTONBOX_START),
		    padding=5)
    hbox.pack_start(create_bbox(gtk.FALSE, "End", 40, gtk.BUTTONBOX_END),
		    padding=5)

    win.show_all()
    if __name__ == '__main__': gtk.main()
    
if __name__ == '__main__': main()
