#!/usr/bin/env python

import pygtk
pygtk.require('2.0')
import gtk

def destroy(args):
    window.destroy()
    gtk.main_quit()

window = gtk.Window()
window.connect("destroy", destroy)
window.set_title('Notebook')
window.set_border_width(0)
window.set_size_request(400, 400)

box1 = gtk.VBox()
window.add(box1)
box1.show()

box2 = gtk.VBox(spacing=10)
box2.set_border_width(10)
box1.pack_start(box2)
box2.show()

notebook = gtk.Notebook()
notebook.set_tab_pos(gtk.POS_TOP)
box2.pack_start(notebook)
notebook.show()

names = ['Background', 'Colors', 'System', 'Setup', 'Samba']

for i in range(len(names)):
    buffer = names[i]
    frame = gtk.Frame(buffer)
    frame.set_border_width(10)
    frame.set_size_request(200, 300)
    frame.set_shadow_type(gtk.SHADOW_ETCHED_OUT)
    frame.show()

    label = gtk.Label(buffer)
    frame.add(label)
    label.show()

    label = gtk.Label(buffer)
    label.set_padding(2, 2)
    notebook.append_page(frame, label)

separator = gtk.HSeparator()
box1.pack_start(separator)
separator.show()

box3 = gtk.VBox(spacing=10)
box3.set_border_width(10)
box1.pack_start(box3)
box3.show()

button = gtk.Button(stock=gtk.STOCK_CLOSE)
box3.pack_start(button)
button.connect('clicked', destroy)
button.show()

window.show()
gtk.main()
