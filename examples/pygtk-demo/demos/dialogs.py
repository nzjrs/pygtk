#!/usr/bin/env python
"""Dialog and Message Boxes

Dialog widgets are used to pop up a transient window for user feedback."""

description = "Dialog and Message Boxes"

import gtk

window = None
entry1 = None
entry2 = None
counter = 1

def message_dialog_clicked(w):
    global window, counter
    
    dialog = gtk.MessageDialog(window,
            gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT,
            gtk.MESSAGE_INFO, gtk.BUTTONS_OK,
            "This message box has been popped up %d time%s." %
                    (counter, counter > 1 and 's' or ''))
    dialog.run()
    dialog.destroy()
    counter += 1

def interactive_dialog_clicked(w):
    global window, entry1, entry2
    
    dialog = gtk.Dialog("Interactive Dialog", window, 0,
            (gtk.STOCK_OK, gtk.RESPONSE_OK,
            "_Non-stock button", gtk.RESPONSE_CANCEL))
    
    hbox = gtk.HBox(gtk.FALSE, 8)
    hbox.set_border_width(8)
    dialog.vbox.pack_start(hbox, gtk.FALSE, gtk.FALSE, 0)
    
    stock = gtk.image_new_from_stock(
            gtk.STOCK_DIALOG_QUESTION,
            gtk.ICON_SIZE_DIALOG)
    hbox.pack_start(stock, gtk.FALSE, gtk.FALSE, 0)
    
    table = gtk.Table(2, 2)
    table.set_row_spacings(4)
    table.set_col_spacings(4)
    hbox.pack_start(table, gtk.TRUE, gtk.TRUE, 0)
    
    label = gtk.Label("Entry _1")
    label.set_use_underline(gtk.TRUE)
    table.attach_defaults(label, 0, 1, 0, 1)
    local_entry1 = gtk.Entry()
    local_entry1.set_text(entry1.get_text())
    table.attach_defaults(local_entry1, 1, 2, 0, 1)
    label.set_mnemonic_widget(local_entry1)
    
    label = gtk.Label("Entry _2")
    label.set_use_underline(gtk.TRUE)
    table.attach_defaults(label, 0, 1, 1, 2)
    local_entry2 = gtk.Entry()
    local_entry2.set_text(entry2.get_text())
    table.attach_defaults(local_entry2, 1, 2, 1, 2)
    label.set_mnemonic_widget(local_entry2)
    
    dialog.show_all()
    
    response = dialog.run()
    
    if response == gtk.RESPONSE_OK:
        entry1.set_text(local_entry1.get_text())
        entry2.set_text(local_entry2.get_text())
    
    dialog.destroy()

def main():
    global window, entry1, entry2
    
    window = gtk.Window()
    window.set_title("Dialogs")
    window.set_border_width(8)
    window.connect('destroy', lambda win: gtk.main_quit())
    
    frame = gtk.Frame("Dialogs")
    window.add(frame)
    
    vbox = gtk.VBox(gtk.FALSE, 8)
    vbox.set_border_width(8)
    frame.add(vbox)
    
    # Standard message dialog
    hbox = gtk.HBox(gtk.FALSE, 8)
    vbox.pack_start(hbox)
    button = gtk.Button("_Message Dialog")
    button.connect('clicked', message_dialog_clicked)
    hbox.pack_start(button, gtk.FALSE, gtk.FALSE, 0)
    vbox.pack_start(gtk.HSeparator(), gtk.FALSE, gtk.FALSE, 0)
    
    # Interactive dialog
    hbox = gtk.HBox(gtk.FALSE, 8)
    vbox.pack_start(hbox, gtk.FALSE, gtk.FALSE, 0)
    vbox2 = gtk.VBox()
    
    button = gtk.Button("_Interactive Dialog")
    button.connect('clicked', interactive_dialog_clicked)
    hbox.pack_start(vbox2, gtk.FALSE, gtk.FALSE, 0)
    vbox2.pack_start(button, gtk.FALSE, gtk.FALSE, 0)
    
    table = gtk.Table(2, 2)
    table.set_row_spacings(4)
    table.set_col_spacings(4)
    hbox.pack_start(table, gtk.FALSE, gtk.FALSE, 0)
    
    label = gtk.Label("Entry _1")
    label.set_use_underline(gtk.TRUE)
    table.attach_defaults(label, 0, 1, 0, 1)
    
    entry1 = gtk.Entry()
    table.attach_defaults(entry1, 1, 2, 0, 1)
    label.set_mnemonic_widget(entry1)

    label = gtk.Label("Entry _2")
    label.set_use_underline(gtk.TRUE)
    table.attach_defaults(label, 0, 1, 1, 2)
    
    entry2 = gtk.Entry()
    table.attach_defaults(entry2, 1, 2, 1, 2)
    label.set_mnemonic_widget(entry2)

    window.show_all()
    
    gtk.main()

if __name__ == '__main__':
    main()
