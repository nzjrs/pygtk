#!/usr/bin/env python

import gtk
import atk

win = gtk.Window()
win.connect('destroy', lambda win: gtk.main_quit())

button = gtk.Button('Quit')
button.connect('pressed', lambda button: gtk.main_quit())
buttonAccess = button.get_accessible()
buttonAccess.set_description('Be careful, clicking this button will exit')

label = gtk.Label('This label describes a button')
labelAccess = label.get_accessible()
labelAccess.set_description('This is a useless label')

relation = atk.Relation((buttonAccess, labelAccess), atk.RELATION_LABEL_FOR)

box = gtk.HBox()
box.pack_start(label)
box.pack_start(button)

win.add(box)
win.show_all()

gtk.main()
