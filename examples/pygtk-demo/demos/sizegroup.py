#!/usr/bin/env python
"""Size Groups

GtkSizeGroup provides a mechanism for grouping a number of widgets together so
they all request the same amount of space. This is typically useful when you
want a column of widgets to have the same size, but you can't use a GtkTable
widget.

Note that size groups only affect the amount of space requested, not the size
that the widgets finally receive. If you want the widgets in a GtkSizeGroup to
actually be the same size, you need to pack them in such a way that they get
the size they request and not more. For example, if you are packing your
widgets into a table, you would not include the GTK_FILL flag."""

description = "Size Groups"

import gtk

def create_option_menu(options):
    menu = gtk.Menu()
    for str in options:
        menu_item = gtk.MenuItem(str)
        menu_item.show()
        gtk.MenuShell.append(menu, menu_item)
    
    option_menu = gtk.OptionMenu()
    option_menu.set_menu(menu)
    
    return option_menu

def add_row(table, row, size_group, label_text, options):
    label = gtk.Label(label_text)
    label.set_use_underline(gtk.TRUE)
    label.set_alignment(0, 1)
    table.attach(label, 0, 1, row, row + 1, gtk.EXPAND + gtk.FILL, 0, 0, 0)
    
    option_menu = create_option_menu(options)
    label.set_mnemonic_widget(option_menu)
    size_group.add_widget(option_menu)
    table.attach(option_menu, 1, 2, row, row + 1, 0, 0, 0, 0)

def toggle_grouping(check_button, size_group):
    # gtk.SIZE_GROUP_NONE is not generally useful, but is useful
    # here to show the effect of gtk.SIZE_GROUP_HORIZONTAL by
    # contrast.
    if check_button.get_active():
        size_group.set_mode(gtk.SIZE_GROUP_HORIZONTAL)
    else:
        size_group.set_mode(gtk.SIZE_GROUP_NONE)

def main():
    color_options = ["Red", "Green", "Blue"]
    dash_options = ["Solid", "Dashed", "Dotted"]
    end_options = ["Square", "Round", "Arrow"]
    
    window = gtk.Dialog("GtkSizeGroups", None, 0,
            (gtk.STOCK_CLOSE, gtk.RESPONSE_NONE))
    window.set_resizable(gtk.FALSE)
    window.connect('response', lambda w, d: window.destroy())
    window.connect('destroy', lambda win: gtk.main_quit())
    
    vbox = gtk.VBox(gtk.FALSE, 5)
    window.vbox.pack_start(vbox, gtk.TRUE, gtk.TRUE, 0)
    vbox.set_border_width(5)
    
    size_group = gtk.SizeGroup(gtk.SIZE_GROUP_HORIZONTAL)
    
    # Create one frame holding color options
    
    frame = gtk.Frame("Color options")
    vbox.pack_start(frame, gtk.TRUE, gtk.TRUE, 0)
    
    table = gtk.Table(2, 2, gtk.FALSE)
    table.set_border_width(5)
    table.set_row_spacings(5)
    table.set_col_spacings(10)
    frame.add(table)
    
    add_row(table, 0, size_group, "_Foreground", color_options)
    add_row(table, 1, size_group, "_Background", color_options)
    
    # And another frame holding line style options
    
    frame = gtk.Frame("Line options")
    vbox.pack_start(frame, gtk.FALSE, gtk.FALSE, 0)
    
    table = gtk.Table(2, 2, gtk.FALSE)
    table.set_border_width(5)
    table.set_row_spacings(5)
    table.set_col_spacings(10)
    frame.add(table)
    
    add_row(table, 0, size_group, "_Dashing", dash_options)
    add_row(table, 1, size_group, "_Line ends", end_options)
    
    # And a check button to turn grouping on and off
    
    check_button = gtk.CheckButton("_Enable grouping")
    vbox.pack_start(check_button, gtk.FALSE, gtk.FALSE, 0)
    check_button.set_active(gtk.TRUE)
    check_button.connect('toggled', toggle_grouping, size_group)
    
    window.show_all()
    
    gtk.main()

if __name__ == '__main__':
    main()
