#!/usr/bin/env python
"""Paned Widgets

The GtkHPaned and GtkVPaned Widgets divide their content area into two panes
with a divider in between that the user can adjust. A separate child is placed
into each pane.
There are a number of options that can be set for each pane. This test contains
both a horizontal (HPaned) and a vertical (VPaned) widget, and allows you to
adjust the options for each side of each widget."""

description = "Paned Widgets"

import gtk

def toggle_resize(w, child):
    paned = child.parent
    
    if child == paned.children()[0]:
        paned.remove(child)
        paned.pack1(child, w.get_active(), 0)
    else:
        paned.remove(child)
        paned.pack2(child, w.get_active(), 0)

def toggle_shrink(w, child):
    paned = child.parent
    
    if child == paned.children()[0]:
        paned.remove(child)
        paned.pack1(child, 0, w.get_active())
    else:
        paned.remove(child)
        paned.pack2(child, 0, w.get_active())

def create_pane_options(paned, frame_label, label1, label2):
    frame = gtk.Frame(frame_label)
    frame.set_border_width(4)
    
    table = gtk.Table(3, 2, gtk.TRUE)
    frame.add(table)
    
    label = gtk.Label(label1)
    table.attach_defaults(label, 0, 1, 0, 1)
    
    check_button = gtk.CheckButton("_Resize")
    check_button.connect('toggled', toggle_resize, paned.children()[0])
    table.attach_defaults(check_button, 0, 1, 1, 2)
    
    check_button = gtk.CheckButton("_Shrink")
    check_button.set_active(gtk.TRUE)
    check_button.connect('toggled', toggle_shrink, paned.children()[0])
    table.attach_defaults(check_button, 0, 1, 2, 3)
    
    label = gtk.Label(label2)
    table.attach_defaults(label, 1, 2, 0, 1)
    
    check_button = gtk.CheckButton("_Resize")
    check_button.set_active(gtk.TRUE)
    check_button.connect('toggled', toggle_resize, paned.children()[1])
    table.attach_defaults(check_button, 1, 2, 1, 2)
    
    check_button = gtk.CheckButton("_Shrink")
    check_button.set_active(gtk.TRUE)
    check_button.connect('toggled', toggle_shrink, paned.children()[1])
    table.attach_defaults(check_button, 1, 2, 2, 3)
    
    return frame

def main():
    window = gtk.Window()
    window.set_title("Paned Widgets")
    window.set_border_width(0)
    if __name__ == '__main__':
        window.connect('destroy', lambda win: gtk.main_quit())
    
    vbox = gtk.VBox(gtk.FALSE, 0)
    window.add(vbox)
    
    vpaned = gtk.VPaned()
    vbox.pack_start(vpaned, gtk.TRUE, gtk.TRUE)
    vpaned.set_border_width(5)
    
    hpaned = gtk.HPaned()
    vpaned.add1(hpaned)
    
    frame = gtk.Frame()
    frame.set_shadow_type(gtk.SHADOW_IN)
    frame.set_size_request(60, 60)
    hpaned.add1(frame)
    
    button = gtk.Button("_Hi there")
    frame.add(button)
    
    frame = gtk.Frame()
    frame.set_shadow_type(gtk.SHADOW_IN)
    frame.set_size_request(80, 60)
    hpaned.add2(frame)
    
    frame = gtk.Frame()
    frame.set_shadow_type(gtk.SHADOW_IN)
    frame.set_size_request(60, 80)
    vpaned.add2(frame)
    
    # Now create toggle buttons to control sizing
    
    vbox.pack_start(create_pane_options(hpaned, "Horizontal", "Left", "Right"),
            gtk.FALSE, gtk.FALSE, 0)
    
    vbox.pack_start(create_pane_options(vpaned, "Vertical", "Top", "Bottom"),
            gtk.FALSE, gtk.FALSE, 0)
    
    window.show_all()
    
    if __name__ == '__main__': gtk.main()

if __name__ == '__main__': main()
