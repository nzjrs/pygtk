#!/usr/bin/env python

#this is a simple translation of the scribble example that comes with GTK+

import sys
from gtk import *

pixmap = None

def configure_event(widget, event):
	global pixmap
	win = widget.get_window()
	pixmap = create_pixmap(win, win.width, win.height, -1)
	draw_rectangle(pixmap, widget.get_style().white_gc, TRUE,
		       0, 0, win.width, win.height)
	return TRUE

def expose_event(widget, event):
	area = event.area
	gc = widget.get_style().fg_gc[STATE_NORMAL]
	widget.draw_pixmap(gc, pixmap, area[0], area[1], area[0], area[1],
			   area[2], area[3])
	return FALSE

def draw_brush(widget, x, y):
	rect = (x-5, y-5, 10, 10)
	draw_rectangle(pixmap, widget.get_style().black_gc, TRUE,
		       x-5, y-5, 10, 10)
	widget.queue_draw()

def button_press_event(widget, event):
	if event.button == 1 and pixmap != None:
		draw_brush(widget, event.x, event.y)
	return TRUE

def motion_notify_event(widget, event):
	if event.is_hint:
		x, y = event.window.pointer
		state = event.window.pointer_state
	else:
		x = event.x; y = event.y
		state = event.state
	if state & GDK.BUTTON1_MASK and pixmap != None:
		draw_brush(widget, x, y)
	return TRUE

def main():
	win = GtkWindow()
	win.set_name("Test Input")
	win.connect("destroy", mainquit)
	win.set_border_width(5)

	vbox = GtkVBox(spacing=3)
	win.add(vbox)
	vbox.show()

	drawing_area = GtkDrawingArea()
	drawing_area.size(200, 200)
	vbox.pack_start(drawing_area)
	drawing_area.show()

	drawing_area.connect("expose_event", expose_event)
	drawing_area.connect("configure_event", configure_event)
	drawing_area.connect("motion_notify_event", motion_notify_event)
	drawing_area.connect("button_press_event", button_press_event)
	drawing_area.set_events(GDK.EXPOSURE_MASK |
				GDK.LEAVE_NOTIFY_MASK |
				GDK.BUTTON_PRESS_MASK |
				GDK.POINTER_MOTION_MASK |
				GDK.POINTER_MOTION_HINT_MASK)

	button = GtkButton("Quit")
	vbox.pack_start(button, expand=FALSE, fill=FALSE)
	button.connect("clicked", win.destroy)
	button.show()
	win.show()
	mainloop()

if __name__ == '__main__':
	main()
	
