#!/usr/bin/env python

# this is a translation of "Hello World III" from the GTK manual,
# using gtk.py

from gtk import *

def hello(*args):
    print "Hello World"
    window.destroy()

def destroy(*args):
    window.hide()
    mainquit()

window = GtkWindow(WINDOW_TOPLEVEL)
window.connect("destroy", destroy)
window.set_border_width(10)

button = GtkButton("Hello World")
button.connect("clicked", hello)
window.add(button)
button.show()

window.show()

mainloop()


