#!/usr/bin/env python

# translation of "Hello World III" from gtk manual, using the new() function
# from gtk.py

from gtk import *

def hello(*args):
	print "Hello World"
	window.destroy()

def destroy(*args):
	window.hide()
	mainquit()

window = new(GtkWindow, type=WINDOW_TOPLEVEL, title='Hello World',
	     allow_grow=FALSE, allow_shrink=FALSE, border_width=10)
window.connect("destroy", destroy)

print ""
button = new(GtkButton, label="Hello World", parent=window, visible=TRUE)
button.connect("clicked", hello)

window.show_all()
mainloop()

