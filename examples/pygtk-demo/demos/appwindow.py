#!/usr/bin/env python
"""Application main window

Demonstrates a typical application window, with menubar, toolbar, statusbar."""

description = 'Application main window'

import gtk

def menuitem_cb(window, action, widget):
    dialog = gtk.MessageDialog(window, gtk.DIALOG_DESTROY_WITH_PARENT,
                               gtk.MESSAGE_INFO, gtk.BUTTONS_CLOSE,
                               'You selected or toggled the menu item: '
                               '"%s"' % (gtk.item_factory_path_from_widget(widget),))
    dialog.connect('response', lambda dialog, response: dialog.destroy())
    dialog.show()

menu_items = (
    ('/_File',            None,         None,        0, '<Branch>' ),
    ('/File/tearoff1',    None,         menuitem_cb, 0, '<Tearoff>'),
    ('/File/_New',        '<control>N', menuitem_cb, 0, '<StockItem>', gtk.STOCK_NEW),
    ('/File/_Open',       '<control>O', menuitem_cb, 0, '<StockItem>', gtk.STOCK_OPEN),
    ('/File/_Save',       '<control>S', menuitem_cb, 0, '<StockItem>', gtk.STOCK_SAVE),
    ('/File/Save _As...', None,         menuitem_cb, 0, '<StockItem>', gtk.STOCK_SAVE),
    ('/File/sep1',        None,         menuitem_cb, 0, '<Separator>'),
    ('/File/_Quit',       '<control>Q', menuitem_cb, 0, '<StockItem>', gtk.STOCK_QUIT),
    
    ('/_Preferences',                  None, None,        0, '<Branch>'),
    ('/_Preferences/_Color',           None, None,        0, '<Branch>'),
    ('/_Preferences/Color/_Red',       None, menuitem_cb, 0, '<RadioItem>'),
    ('/_Preferences/Color/_Green',     None, menuitem_cb, 0, '/Preferences/Color/Red'),
    ('/_Preferences/Color/_Blue',      None, menuitem_cb, 0, '/Preferences/Color/Red'),
    ('/_Preferences/_Shape',           None, None,        0, '<Branch>'),
    ('/_Preferences/Shape/_Square',    None, menuitem_cb, 0, '<RadioItem>'),
    ('/_Preferences/Shape/_Rectangle', None, menuitem_cb, 0, '/Preferences/Shape/Square'),
    ('/_Preferences/Shape/_Oval',      None, menuitem_cb, 0, '/Preferences/Shape/Rectangle'),

    # If you wanted this to be right justified you would use
    # "<LastBranch>", not "<Branch>".  Right justified help menu items
    # are generally considered a bad idea now days.

    ('/_Help',       None, None, 0, '<Branch>'),
    ('/Help/_About', None, menuitem_cb, 0, ''),
    )

def toolbar_cb(button, window):
    dialog = gtk.MessageDialog(window, gtk.DIALOG_DESTROY_WITH_PARENT,
                               gtk.MESSAGE_INFO, gtk.BUTTONS_CLOSE,
                               'You selected a toolbar button')
    dialog.connect('response', lambda dialog, response: dialog.destroy())
    dialog.show()

def register_stock_icons ():
    # TODO: Fix this
    
    items = [('demo-gtk-logo', '_GTK!', 0, 0, '')]
    
    # Register our stock items
    gtk.stock_add (items)
    
    # Add our custom icon factory to the list of defaults
    factory = gtk.IconFactory ()
    factory.add_default ()
    
    pixbuf = gtk.gdk.pixbuf_new_from_file ('gtk-logo-rgb.gif')
    
    # Register icon to accompany stock item
    if pixbuf:
	icon_set = gtk.IconSet (pixbuf)
	factory.add ('demo-gtk-logo', icon_set)
	icon_set.unref()
    else:
	print 'failed to load GTK logo for toolbar'
	
def update_statusbar(buffer, statusbar):
    # clear any previous message, underflow is allowed 
    statusbar.pop(0)
    count = buffer.get_char_count()
    iter = buffer.get_iter_at_mark(buffer.get_insert())
    row = iter.get_line()
    col = iter.get_line_offset()
    statusbar.push(0, 'Cursor at row %d column %d - %d chars in document' %
                   (row, col, count))

mark_set_callback = (lambda buffer, new_location, mark, statusbar:
                     update_statusbar(buffer, statusbar))
    

def main():
    #register_stock_icons ()
    
    # Create the toplevel window
    window = gtk.Window()
    window.set_title('Application Window')
    window.connect('destroy', lambda win: gtk.main_quit())

    table = gtk.Table(1, 4, gtk.FALSE)
    window.add(table)

    # Create the menubar

    accel_group = gtk.AccelGroup()
    window.add_accel_group(accel_group)
    
    item_factory = gtk.ItemFactory(gtk.MenuBar, '<main>', accel_group)
    
    # create menu items

    item_factory.create_items(menu_items, window)
    
    table.attach(item_factory.get_widget('<main>'),
                 # X direction           Y direction
                 0, 1,                      0, 1,
                 gtk.EXPAND | gtk.FILL,     0,
                 0,                         0)

    # Create the toolbar

    toolbar = gtk.Toolbar()
    toolbar.insert_stock(gtk.STOCK_OPEN,
                         "This is a demo button with an 'open' icon",
                         None,
                         toolbar_cb,
                         window,
                         -1)
    toolbar.insert_stock(gtk.STOCK_CLOSE,
                         "This is a demo button with an 'close' icon",
                         None,
                         toolbar_cb,
                         window,
                         -1)
    
    toolbar.append_space()

    toolbar.insert_stock('demo-gtk-logo',
                         "This is a demo button with a 'gtk' icon",
                         None,
                         toolbar_cb,
                         window,
                         -1)

    table.attach(toolbar,
                 # X direction           Y direction
                 0, 1,                   1, 2,
                 gtk.EXPAND | gtk.FILL,  0,
                 0,                      0)

    # Create document

    sw = gtk.ScrolledWindow()
    sw.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
    sw.set_shadow_type(gtk.SHADOW_IN)
    table.attach(sw,
                 # X direction           Y direction
                 0, 1,                   2, 3,
                 gtk.EXPAND | gtk.FILL,  gtk.EXPAND | gtk.FILL,
                 0,                      0)

    window.set_default_size(200, 200)
    
    contents = gtk.TextView()
    sw.add(contents)

    # Create statusbar 

    statusbar = gtk.Statusbar();
    table.attach(statusbar,
                 # X direction           Y direction
                 0, 1,                   3, 4,
                 gtk.EXPAND | gtk.FILL,  0,
                 0,                      0)

    buffer = contents.get_buffer()
    buffer.connect('changed', update_statusbar, statusbar)
    buffer.connect('mark_set', mark_set_callback, statusbar)
    update_statusbar(buffer, statusbar)
   
    window.show_all()

    gtk.main()

if __name__ == '__main__':
    main()
