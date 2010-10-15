#!/usr/bin/env python
# example toolbar.py by Usmar A Padow

import pygtk
pygtk.require('2.0')
import gtk

class ToolbarExample:

    def __init__(self):
        dialog = gtk.Dialog()
        dialog.connect("delete-event", gtk.main_quit)
        dialog.set_size_request(450, -1)
        dialog.set_resizable(True)

        # to make it nice we'll put the toolbar into the handle box, 
        # so that it can be detached from the main window
        handlebox = gtk.HandleBox()
        dialog.vbox.pack_start(handlebox, False, False, 5)

        # toolbar will be horizontal, with both icons and text, and
        # with 5pxl spaces between items and finally, 
        # we'll also put it into our handlebox
        toolbar = gtk.Toolbar()
        toolbar.set_orientation(gtk.ORIENTATION_HORIZONTAL)
        toolbar.set_style(gtk.TOOLBAR_BOTH)
        toolbar.set_border_width(5)
        handlebox.add(toolbar)

        self.buttons=[]
        for a in range(0,5):
            if a == 0:
                #first button with none group
                widget = None
            else:
                #additional buttons with belong to the group of the first button
                widget = but
            but = gtk.RadioToolButton(group=widget, stock_id=gtk.STOCK_APPLY)
            but.set_label("button %d" % a)
            but.connect("clicked", self.radio_event, a)

            toolbar.insert(but,a)
            self.buttons.append(but)

        dialog.show_all()

    def radio_event(self, widget, btn_num):
        if widget.get_active():
            print "button %d toggled on" % btn_num


if __name__ == "__main__":
    ToolbarExample()
    gtk.main()

