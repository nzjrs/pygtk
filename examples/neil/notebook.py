#!/usr/bin/env python

from gtk import *

def destroy(args):
	window.destroy()
	mainquit()


window = GtkWindow()
window.connect("destroy", destroy)
window.set_title('Notebook')
window.set_border_width(0)
window.set_usize(400, 400)

box1 = GtkVBox()
window.add(box1)
box1.show()

box2 = GtkVBox(spacing=10)
box2.set_border_width(10)
box1.pack_start(box2)
box2.show()

notebook = GtkNotebook()
notebook.set_tab_pos(POS_TOP)
box2.pack_start(notebook)
notebook.show()

names = ['Background', 'Colors', 'System', 'Setup', 'Samba']

for i in range(len(names)):
	buffer = names[i]
	frame = GtkFrame(buffer)
	frame.set_border_width(10)
	frame.set_usize(200, 300)
	frame.set_shadow_type(SHADOW_ETCHED_OUT)
	frame.show()
	
	label = GtkLabel(buffer)
	frame.add(label)
	label.show()

	label = GtkLabel(buffer)
	label.set_padding(2, 2)
	notebook.append_page(frame, label)

separator = GtkHSeparator()
box1.pack_start(separator)
separator.show()

box3 = GtkVBox(spacing=10)
box3.set_border_width(10)
box1.pack_start(box3)
box3.show()

button = GtkButton(label='close')
box3.pack_start(button)
button.connect('clicked', destroy)
button.show()

window.show()
mainloop()
