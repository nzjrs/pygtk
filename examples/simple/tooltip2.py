#!/usr/bin/env python

# translation of "Hello World III" from GTK manual, using new() function
# from gtk.py.  Also implements a tooltip for the button.

from gtk import *

def hello(*args):
	print "Hello World"
	window.destroy()

def destroy(*args):
	window.hide()
	mainquit()

tt = GtkTooltips()
tt.set_delay(500)

window = new(GtkWindow, type=WINDOW_TOPLEVEL, title="Hello World",
	     allow_grow=FALSE, allow_shrink=FALSE, border_width=10)
window.connect("destroy", destroy)

button = new(GtkButton, label="Hello World", parent=window, visible=TRUE)
button.connect("clicked", hello)

tt.set_tip(button, 'Prints "Hello World"', '')
tt.enable()

window.show()
mainloop()


