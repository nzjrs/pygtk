#!/usr/bin/env python

# this is a translation of "Hello World III" from the GTK manual,
# using gtkmodule

from _gtk import *
from GTK import *

def hello(*args):
    print "Hello World"
    gtk_widget_destroy(window)

def destroy(*args):
    gtk_widget_hide(window)
    gtk_main_quit()

gtk_init()

window = gtk_window_new(WINDOW_TOPLEVEL)
gtk_signal_connect(window, "destroy", destroy)
gtk_container_set_border_width(window, 10)

button = gtk_button_new_with_label("Hello World")
gtk_signal_connect(button, "clicked", hello)
gtk_container_add(window, button)
gtk_widget_show(button)

gtk_widget_show(window)

gtk_main()


