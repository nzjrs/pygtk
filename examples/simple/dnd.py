#! /usr/bin/env python

from gtk import *
from GDK import *
from dndpixmap import *

trashcan_open = None
trashcan_open_mask = None
trashcan_closed = None
trashcan_closed_mask = None

have_drag = FALSE;
popped_up = FALSE
in_popup = FALSE
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
	have_drag = FALSE
	w.set(trashcan_closed, trashcan_closed_mask)
def target_drag_motion(w, context, x, y, time):
	global trashcan_open, trashcan_open_mask
	global have_drag
	if not have_drag:
		have_drag = TRUE
		w.set(trashcan_open, trashcan_open_mask)
	source_widget = w.drag_get_source_widget(context)
	print 'motion, source ',
	if source_widget:
		print source_widget.__class__.__name__
	else:
		print 'unknown'
	w.drag_status(context, context.suggested_action, time)
	return TRUE
def target_drag_drop(w, context, x, y, time):
	global trashcan_closed, trashcan_closed_mask
	global have_drag
	print 'drop'
	have_drag = FALSE
	w.set(trashcan_closed, trashcan_closed_mask)
	if context.targets:
		w.drag_get_data(context, context.targets[0], time)
		return TRUE
	return FALSE
def target_drag_data_received(w, context, x, y, data, info, time):
	if data.format == 8:
		print 'Received "%s" in trashcan' % data.data
		w.drag_finish(context, TRUE, FALSE, time)
	else:
		w.drag_finish(context, FALSE, FALSE, time)
def label_drag_data_received(w, context, x, y, data, info, time):
	if data and data.format == 8:
		print 'Received "%s" in label' % data.data
		w.drag_finish(context, TRUE, FALSE, time)
	else:
		w.drag_finish(context, FALSE, FALSE, time)
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
	popped_up = FALSE
	return FALSE
def popup_motion(w, context, x, y, time):
	global in_popup, popdown_timer
	if not in_popup:
		in_popup = TRUE
		if popdown_timer:
			print 'removed popdown'
			timeout_remove(popdown_timer)
			popdown_timer = 0
	return TRUE
def popup_leave(w, context, time):
	global in_popup, popdown_timer
	print 'popup_leave'
	if in_popup:
		in_popup = FALSE
		if not popdown_timer:
			print 'added popdown'
			popdown_timer = timeout_add(500, popdown_cb)
def popup_cb():
	global popped_up, popup_win
	global popup_timer, popdown_timer
	if not popped_up:
		if not popup_win:
			popup_win = GtkWindow(WINDOW_POPUP)
			popup_win.set_position(WIN_POS_MOUSE)
			table = GtkTable(3,3,FALSE)
			for k in range(9):
				i, j = divmod(k, 3)
				b = GtkButton("%d,%d" % (i,j))
				table.attach(b, i,i+1,j,j+1)
				b.drag_dest_set(DEST_DEFAULT_ALL, target, 
					ACTION_COPY | ACTION_MOVE)
				b.connect('drag_motion', popup_motion)
				b.connect('drag_leave', popup_leave)
			table.show_all()
			popup_win.add(table)
		popup_win.show()
		popped_up = TRUE
	popdown_timer = timeout_add(500, popdown_cb)
	print 'added popdown'
	popup_timer = 0
	return FALSE
def popsite_motion(w, context, x, y, time):
	global popup_timer
	if not popup_timer:
		popup_timer = timeout_add(500, popup_cb)
	return TRUE
def popsite_leave(w, context, time):
	global popup_timer
	if popup_timer:
		timeout_remove(popup_timer)
		popup_timer = 0
def source_drag_data_delete(w, context, data):
	print 'Delete the data!'
def create_pixmap(w, xpm):
	return create_pixmap_from_xpm_d(w, None, xpm)
def main():
	global trashcan_open, trashcan_open_mask
	global trashcan_closed, trashcan_closed_mask
	global drag_icon, drag_mask
	win = GtkWindow()
	win.connect('destroy', mainquit)
	table = GtkTable(2,2)
	win.add(table)
	drag_icon, drag_mask = create_pixmap(win, drag_icon_xpm)
	trashcan_open, trashcan_open_mask = create_pixmap(win, trashcan_open_xpm)
	trashcan_closed, trashcan_closed_mask = create_pixmap(win, trashcan_closed_xpm)
	label = GtkLabel('Drop Here!\n')
	label.drag_dest_set(DEST_DEFAULT_ALL, target[:-1], ACTION_COPY | ACTION_MOVE)
	label.connect('drag_data_received', label_drag_data_received)
	table.attach(label, 0, 1, 0, 1)

	label = GtkLabel('Popup\n')
	label.drag_dest_set(DEST_DEFAULT_ALL, target[:-1], ACTION_COPY | ACTION_MOVE)
	table.attach(label, 1, 2, 1, 2)
	label.connect('drag_motion', popsite_motion)
	label.connect('drag_leave', popsite_leave)

	pixmap = GtkPixmap(trashcan_closed, trashcan_closed_mask)
	pixmap.drag_dest_set(0, [], 0)
	table.attach(pixmap, 1, 2, 0, 1)
	pixmap.connect('drag_leave', target_drag_leave)
	pixmap.connect('drag_motion', target_drag_motion)
	pixmap.connect('drag_drop', target_drag_drop)
	pixmap.connect('drag_data_received', target_drag_data_received)

	b = GtkButton('Drag Here\n')
	b.drag_source_set(BUTTON1_MASK|BUTTON3_MASK, target, ACTION_COPY|ACTION_MOVE)
	b.drag_source_set_icon(win.get_colormap(), drag_icon, drag_mask)
	table.attach(b, 0, 1, 1, 2)
	b.connect('drag_data_get', source_drag_data_get)
	b.connect('drag_data_delete', source_drag_data_delete)
	win.show_all()
main()
mainloop()
