#!/usr/bin/env python
'''Drag and Drop Test

This is a test of the drag and drop capabilities of gtk.  It is a
fairly straight forward port of the example distributed with gtk.

FIXME: there are still a few things missing since I converted the
GdkDragContext wrapper to being a GObject.'''

description = 'Drag and Drop'

import gtk
from dndpixmap import *

trashcan_open = None
trashcan_open_mask = None
trashcan_closed = None
trashcan_closed_mask = None

have_drag = gtk.FALSE
popped_up = gtk.FALSE
in_popup = gtk.FALSE
popup_timer = 0
popdown_timer = 0
popup_win = None


TARGET_STRING = 0
TARGET_ROOTWIN = 1

target = [
    ('STRING', 0, TARGET_STRING),
    ('text/plain', 0, TARGET_STRING),
    ('application/x-rootwin-drop', 0, TARGET_ROOTWIN)]

def target_drag_leave(w, context, time):
    global trashcan_closed, trashcan_closed_mask
    global have_drag
    print 'leave'
    have_drag = gtk.FALSE
    w.set(trashcan_closed, trashcan_closed_mask)
def target_drag_motion(w, context, x, y, time):
    global trashcan_open, trashcan_open_mask
    global have_drag
    if not have_drag:
	have_drag = gtk.TRUE
	w.set(trashcan_open, trashcan_open_mask)
    source_widget = context.get_source_widget()
    print 'motion, source ',
    if source_widget:
	print source_widget.__class__.__name__
    else:
	print 'unknown'
    context.drag_status(context.suggested_action, time)
    return gtk.TRUE
def target_drag_drop(w, context, x, y, time):
    global trashcan_closed, trashcan_closed_mask
    global have_drag
    print 'drop'
    have_drag = gtk.FALSE
    w.set(trashcan_closed, trashcan_closed_mask)
#    if context.targets:
#	w.drag_get_data(context, context.targets[0], time)
#	return gtk.TRUE
    return gtk.FALSE
def target_drag_data_received(w, context, x, y, data, info, time):
    if data.format == 8:
	print 'Received "%s" in trashcan' % data.data
	context.finish(gtk.TRUE, gtk.FALSE, time)
    else:
	context.finish(gtk.FALSE, gtk.FALSE, time)
def label_drag_data_received(w, context, x, y, data, info, time):
    if data and data.format == 8:
	print 'Received "%s" in label' % data.data
	context.finish(gtk.TRUE, gtk.FALSE, time)
    else:
	context.finish(gtk.FALSE, gtk.FALSE, time)
def source_drag_data_get(w, context, selection_data, info, time):
    if info == TARGET_ROOTWIN:
	print 'I was dropped on the rootwin'
    else:
	selection_data.set(selection_data.target, 8, "I'm Data!")	

def popdown_cb():
    global popdown_timer, popped_up
    global popup_win
    popdown_timer = 0	
    popup_win.hide()
    popped_up = gtk.FALSE
    return gtk.FALSE
def popup_motion(w, context, x, y, time):
    global in_popup, popdown_timer
    if not in_popup:
	in_popup = gtk.TRUE
	if popdown_timer:
	    print 'removed popdown'
	    gtk.timeout_remove(popdown_timer)
	    popdown_timer = 0
    return gtk.TRUE
def popup_leave(w, context, time):
    global in_popup, popdown_timer
    print 'popup_leave'
    if in_popup:
	in_popup = gtk.FALSE
	if not popdown_timer:
	    print 'added popdown'
	    popdown_timer = gtk.timeout_add(500, popdown_cb)
def popup_cb():
    global popped_up, popup_win
    global popup_timer, popdown_timer
    if not popped_up:
	if not popup_win:
	    popup_win = gtk.Window(gtk.WINDOW_POPUP)
	    popup_win.set_position(gtk.WIN_POS_MOUSE)
	    table = gtk.Table(3, 3)
	    for k in range(9):
		i, j = divmod(k, 3)
		b = gtk.Button("%d,%d" % (i,j))
		table.attach(b, i,i+1,j,j+1)
		b.drag_dest_set(gtk.DEST_DEFAULT_ALL, target, 
				gtk.GDK.ACTION_COPY | gtk.GDK.ACTION_MOVE)
		b.connect('drag_motion', popup_motion)
		b.connect('drag_leave', popup_leave)
	    table.show_all()
	    popup_win.add(table)
	popup_win.show()
	popped_up = gtk.TRUE
    popdown_timer = gtk.timeout_add(500, popdown_cb)
    print 'added popdown'
    popup_timer = 0
    return gtk.FALSE
def popsite_motion(w, context, x, y, time):
    global popup_timer
    if not popup_timer:
	popup_timer = gtk.timeout_add(500, popup_cb)
    return gtk.TRUE
def popsite_leave(w, context, time):
    global popup_timer
    if popup_timer:
	gtk.timeout_remove(popup_timer)
	popup_timer = 0
def source_drag_data_delete(w, context, data):
    print 'Delete the data!'
def create_pixmap(widget, xpm):
    return gtk.gdk.pixmap_colormap_create_from_xpm_d(None,
                                                     widget.get_colormap(),
                                                     None, xpm)

def main():
    global trashcan_open, trashcan_open_mask
    global trashcan_closed, trashcan_closed_mask
    global drag_icon, drag_mask
    win = gtk.Window()
    if __name__ == '__main__':
	win.connect('destroy', lambda win: gtk.main_quit())
    table = gtk.Table(2,2)
    win.add(table)
    drag_icon, drag_mask = create_pixmap(win, drag_icon_xpm)
    trashcan_open, trashcan_open_mask = create_pixmap(win, trashcan_open_xpm)
    trashcan_closed, trashcan_closed_mask = create_pixmap(win, trashcan_closed_xpm)
    label = gtk.Label('Drop Here!\n')
    label.drag_dest_set(gtk.DEST_DEFAULT_ALL, target[:-1],
			gtk.GDK.ACTION_COPY | gtk.GDK.ACTION_MOVE)
    label.connect('drag_data_received', label_drag_data_received)
    table.attach(label, 0, 1, 0, 1)

    label = gtk.Label('Popup\n')
    label.drag_dest_set(gtk.DEST_DEFAULT_ALL, target[:-1],
			gtk.GDK.ACTION_COPY | gtk.GDK.ACTION_MOVE)
    table.attach(label, 1, 2, 1, 2)
    label.connect('drag_motion', popsite_motion)
    label.connect('drag_leave', popsite_leave)

    pixmap = gtk.Pixmap(trashcan_closed, trashcan_closed_mask)
    pixmap.drag_dest_set(0, [], 0)
    table.attach(pixmap, 1, 2, 0, 1)
    pixmap.connect('drag_leave', target_drag_leave)
    pixmap.connect('drag_motion', target_drag_motion)
    pixmap.connect('drag_drop', target_drag_drop)
    pixmap.connect('drag_data_received', target_drag_data_received)

    b = gtk.Button('Drag Here\n')
    b.drag_source_set(gtk.GDK.BUTTON1_MASK | gtk.GDK.BUTTON3_MASK,
		      target, gtk.GDK.ACTION_COPY | gtk.GDK.ACTION_MOVE)
    b.drag_source_set_icon(win.get_colormap(), drag_icon, drag_mask)
    table.attach(b, 0, 1, 1, 2)
    b.connect('drag_data_get', source_drag_data_get)
    b.connect('drag_data_delete', source_drag_data_delete)
    win.show_all()
    if __name__ == '__main__': gtk.main()
    
if __name__ == '__main__': main()
