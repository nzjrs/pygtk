#!/usr/bin/env python
"""Color Selector

GtkColorSelection lets the user choose a color. GtkColorSelectionDialog is a
prebuilt dialog containing a GtkColorSelection."""

description = "Color Selector"

import gtk

window = None
color = None
da = None

def change_color_cb(w):
    global window, color, da
    
    dialog = gtk.ColorSelectionDialog("Changing color")
    dialog.set_transient_for(window)
    colorsel = dialog.colorsel
    
    colorsel.set_previous_color(color)
    colorsel.set_current_color(color)
    colorsel.set_has_palette(gtk.TRUE)
    
    response = dialog.run()
    
    if response == gtk.RESPONSE_OK:
        color = colorsel.get_current_color()
        da.modify_bg(gtk.STATE_NORMAL, color)
    
    dialog.destroy()

def main():
    global window, color, da
    
    color = gtk.gdk.color_parse("blue")
    
    window = gtk.Window()
    window.set_title("Color selection")
    window.set_border_width(8)
    if __name__ == '__main__':
        window.connect('destroy', lambda win: gtk.main_quit())
    
    vbox = gtk.VBox()
    vbox.set_border_width(8)
    window.add(vbox)
    
    # Create the color swatch area
    
    frame = gtk.Frame()
    frame.set_shadow_type(gtk.SHADOW_IN)
    vbox.pack_start(frame, gtk.TRUE, gtk.TRUE, 8)
    
    da = gtk.DrawingArea()
    da.set_size_request(200, 200)
    da.modify_bg(gtk.STATE_NORMAL, color)
    frame.add(da)
    
    alignment = gtk.Alignment(1.0, 0.5, 0.0, 0.0)
    
    button = gtk.Button("_Change the above color")
    alignment.add(button)
    
    vbox.pack_start(alignment, gtk.TRUE, gtk.TRUE)
    
    button.connect('clicked', change_color_cb)
    button.set_flags(gtk.CAN_DEFAULT)
    button.grab_default()
    
    window.show_all()
    
    if __name__ == '__main__': gtk.main()

if __name__ == '__main__': main()
