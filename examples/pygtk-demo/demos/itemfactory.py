#!/usr/bin/env python
'''Item Factory

The GtkItemFactory object allows the easy creation of menus
from an array of descriptions of menu items.'''

description = 'Item Factory'

import gtk

def ifactory_cb(action, widget):
    print 'ItemFactory: activated "%s"' % (gtk.item_factory_path_from_widget(widget),)

menu_items = (
    ('/_File', None, None, 0, '<Branch>' ),
    ('/File/tearoff1', None, ifactory_cb, 0, '<Tearoff>'),
    ('/File/_New', '<control>N', ifactory_cb, 0, ''),
    ('/File/_Open', '<control>O', ifactory_cb, 0, ''),
    ('/File/_Save', '<control>S', ifactory_cb, 0, ''),
    ('/File/Save _As...', None, ifactory_cb, 0, ''),
    ('/File/sep1', None, ifactory_cb, 0, '<Separator>'),
    ('/File/_Quit', '<control>Q', ifactory_cb, 0, ''),
    
    ('/_Preferences', None, None, 0, '<Branch>'),
    ('/_Preferences/_Color', None, None, 0, '<Branch>'),
    ('/_Preferences/Color/_Red', None, ifactory_cb, 0, '<RadioItem>'),
    ('/_Preferences/Color/_Green', None, ifactory_cb, 0, '/Preferences/Color/Red'),
    ('/_Preferences/Color/_Blue', None, ifactory_cb, 0, '/Preferences/Color/Red'),
    ('/_Preferences/_Shape', None, None, 0, '<Branch>'),
    ('/_Preferences/Shape/_Square', None, ifactory_cb, 0, '<RadioItem>'),
    ('/_Preferences/Shape/_Rectangle', None, ifactory_cb, 0, '/Preferences/Shape/Square'),
    ('/_Preferences/Shape/_Oval', None, ifactory_cb, 0, '/Preferences/Shape/Rectangle'),
    
    ('/_Help', None, None, 0, '<LastBranch>'),
    ('/Help/_About', None, ifactory_cb, 0, ''),
    )

def main():
    win = gtk.Window()
    if __name__ == '__main__':
	win.connect('destroy', lambda win: gtk.main_quit())

    win.set_title('Item Factory')
    win.set_border_width(10)

    box1 = gtk.VBox()
    win.add(box1)
    win.set_title('Item Factory')
    win.set_border_width(0)

    accelgroup = gtk.AccelGroup()
    win.add_accel_group(accelgroup)
    
    item_factory = gtk.ItemFactory(gtk.MenuBar, '<main>', accelgroup)
    item_factory.create_items(menu_items)

    menubar = item_factory.get_widget('<main>')
    box1.pack_start(menubar)

    label = gtk.Label('Type\n<F10>\nto start')
    label.set_size_request(200, 200);
    box1.pack_start(label)

    separator = gtk.HSeparator()
    box1.pack_start(separator)

    box2 = gtk.VBox(gtk.FALSE, 10)
    box2.set_border_width(10)
    box1.pack_start(box2)
    
    button = gtk.Button('close')
    button.connect('clicked', lambda button, win=win: win.destroy())
    box2.pack_start(button, gtk.TRUE, gtk.TRUE)
    button.set_flags(gtk.CAN_DEFAULT)
    button.grab_default()

    win.show_all()
    if __name__ == '__main__': gtk.main()
    
if __name__ == '__main__': main()
