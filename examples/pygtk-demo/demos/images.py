#!/usr/bin/env python
#
# Copyright (C) 2004 Joey Tsai <joeytsai@joeytsai.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the
# Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.

"""Images

GtkImage is used to display an image; the image can be in a number of formats.
Typically, you load an image into a GdkPixbuf, then display the pixbuf.

This demo code shows some of the more obscure cases, in the simple case a call
to gtk.Image's set_from_file() is all you need.
"""

import sys

import pygtk
pygtk.require("2.0")

import gobject
import gtk

description = "Images"

def error_dialog(message, parent=None):
    dialog = gtk.MessageDialog(parent,
	gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT,
	gtk.MESSAGE_WARNING, gtk.BUTTONS_OK, message)
    dialog.run()
    dialog.destroy()

def on_timeout(loader, fileobject):
    run_again = True

    try:
	bytes = fileobject.read(256)
	loader.write(bytes)
    except (IOError, gobject.GError), e:
	error_dialog(str(e))
	run_again = False

    if not bytes:
	run_again = False

    if not run_again:
	try:
	    fileobject.close()
	    loader.close()
	except gobject.GError, e:
	    # bug 136989, loader.close() will throw an exception
	    pass

    return run_again

def on_area_prepared(loader, image):
    pixbuf = loader.get_pixbuf()
    image.set_from_pixbuf(pixbuf)

def on_area_updated(loader, x, y, w, h, image):
    image.queue_draw()

def get_progressive_image(filename):
    try:
	fileobject = open(filename, "r")
    except IOError, e:
	error_dialog(str(e))
	return

    image = gtk.Image()

    loader = gtk.gdk.PixbufLoader()
    loader.connect("area_prepared", on_area_prepared, image)
    loader.connect("area_updated",  on_area_updated,  image)
    
    gobject.timeout_add(150, on_timeout, loader, fileobject)

    return image

def get_image(filename):
    try:
	anim = gtk.gdk.PixbufAnimation(filename)
    except gobject.GError, e:
	error_dialog(str(e))
	return

    image = gtk.Image()

    if anim.is_static_image():
	image.set_from_pixbuf( anim.get_static_image() )
    else:
	image.set_from_animation( anim )

    return image

def align_image(image, label_text):
    label = gtk.Label()
    label.set_markup("<u>%s</u>" % label_text)

    frame = gtk.Frame()
    frame.set_shadow_type(gtk.SHADOW_IN)

    if image:
	frame.add(image)
    else:
	frame.add(gtk.Label("(No image)"))

    align = gtk.Alignment(0.5, 0.5, 0, 0)
    align.add(frame)

    vbox = gtk.VBox(spacing=8)
    vbox.set_border_width(4)
    vbox.pack_start(label)
    vbox.pack_start(align)

    return vbox

def on_button_toggled(button, vbox):
    for widget in vbox.get_children():
	if widget != button:
	    widget.set_sensitive(not button.get_active())


def main():
    vbox = gtk.VBox()

    button = gtk.ToggleButton("_Insensitive")
    button.connect("toggled", on_button_toggled, vbox)

    i1 = get_image("gtk-logo-rgb.gif")
    i2 = get_image("floppybuddy.gif")
    i3 = get_progressive_image("alphatest.png")

    vbox.pack_start(align_image(i1, "Image loaded from a file"))
    vbox.pack_start(align_image(i2, "Animation loaded from a file"))
    vbox.pack_start(align_image(i3, "Progressive image loading"))
    vbox.pack_start(button)

    win = gtk.Window()
    win.set_title("Images")
    win.set_border_width(8)
    win.connect("destroy", gtk.main_quit)
    win.add(vbox)
    win.show_all()

    gtk.main()

    return 0

if __name__ == "__main__":
    sys.exit(main())
