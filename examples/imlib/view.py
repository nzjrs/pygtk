#!/usr/bin/env python

from gtk import *
import GdkImlib

def close(win, _event=None):
	win.hide()
	win.destroy()

def resize(win, event):
	im = win.get_data('user_data')
	# note that render must be called once before each call to make_pixmap
	im.render(event.width, event.height)
	pix = win.children()[0]
	win.remove(pix)
	pix = im.make_pixmap()
	pix.show()
	win.add(pix)

def open_img(_b):
        file = fs.get_filename()
	try:
		im = GdkImlib.Image(file)
	except RuntimeError: return
	win = GtkWindow()
	win.connect('destroy', close)
	win.connect('delete_event', close)
	win.connect('configure_event', resize)
	win.set_title(file)
	win.set_data('user_data', im)
	im.render()
	pix = im.make_pixmap()
	pix.show()
	win.add(pix)
	win.show()

fs = GtkFileSelection()
fs.set_title('Image Viewer')
fs.connect('destroy', mainquit)
fs.connect('delete_event', mainquit)

label = fs.ok_button.children()[0]
label.set_text('View')
fs.ok_button.connect('clicked', open_img)

label = fs.cancel_button.children()[0]
label.set_text('Quit')
fs.cancel_button.connect('clicked', mainquit)

fs.show()

mainloop()
