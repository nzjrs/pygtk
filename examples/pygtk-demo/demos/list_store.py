#!/usr/bin/env python2.2
'''Tree View/List Store

The GtkListStore is used to store data in list form, to be used 
later on by a GtkTreeView to display it. This demo builds a
simple GtkListStore and displays it. See the Stock Browser
demo for a more advanced example.''' # "

description = 'List Store'

import gobject
import gtk

COLUMN_FIXED       = 0
COLUMN_NUMBER      = 1
COLUMN_SEVERITY    = 2
COLUMN_DESCRIPTION = 3

data = \
[[gtk.FALSE, 60482, 'Normal',      'scrollable notebooks and hidden tabs'],
 [gtk.FALSE, 60620, 'Critical',    'gdk_window_clear_area (gdkwindow-win32.c) is not thread-safe'],
 [gtk.FALSE, 50214, 'Major',       'Xft support does not clean up correctly'],
 [gtk.TRUE,  52877, 'Major',       'GtkFileSelection needs a refresh method. '],
 [gtk.FALSE, 56070, 'Normal',      "Can't click button after setting in sensitive"],
 [gtk.TRUE,  56355, 'Normal',      'GtkLabel - Not all changes propagate correctly'],
 [gtk.FALSE, 50055, 'Normal',      'Rework width/height computations for TreeView'],
 [gtk.FALSE, 58278, 'Normal',      "gtk_dialog_set_response_sensitive () doesn't work"],
 [gtk.FALSE, 55767, 'Normal',      'Getters for all setters'],
 [gtk.FALSE, 56925, 'Normal',      'Gtkcalender size'],
 [gtk.FALSE, 56221, 'Normal',      'Selectable label needs right-click copy menu'],
 [gtk.TRUE,  50939, 'Normal',      'Add shift clicking to GtkTextView'],
 [gtk.FALSE, 6112,  'Enhancement', 'netscape-like collapsable toolbars'],
 [gtk.FALSE, 1,     'Normal',      'First bug :=)']]

def create_model ():
    store = gtk.ListStore (gobject.TYPE_BOOLEAN,
			   gobject.TYPE_UINT,
			   gobject.TYPE_STRING,
			   gobject.TYPE_STRING)
    for item in data:
	iter = store.append ()
	store.set (iter, COLUMN_FIXED, item[0],
                   COLUMN_NUMBER, item[1],
                   COLUMN_SEVERITY, item[2],
                   COLUMN_DESCRIPTION, item[3])
    return store

def fixed_toggled (cell, path, model):
    # get toggled iter
    iter = model.get_iter (int (path))
    fixed = model.get_value (iter, COLUMN_FIXED)
    
    # do something with the value
    fixed = not fixed
    
    # set new value
    model.set (iter, COLUMN_FIXED, fixed)
	
def add_columns (treeview):
    model = treeview.get_model ()
    
    # column for fixed toggles
    renderer = gtk.CellRendererToggle ()
    renderer.connect ('toggled', fixed_toggled, model)
    
    column = gtk.TreeViewColumn ('Fixed?', renderer, active=COLUMN_FIXED)
    column.set_clickable (gtk.TRUE)
    
    # set this column to a fixed sizing (of 50 pixels)
    column.set_sizing (gtk.TREE_VIEW_COLUMN_FIXED)
    column.set_fixed_width (50)
    column.set_clickable (gtk.TRUE)
    
    treeview.append_column (column)

    # column for bug numbers
    renderer = gtk.CellRendererText ()
    column = gtk.TreeViewColumn ('Bug number', renderer, text=COLUMN_NUMBER)
    treeview.append_column (column)
    
    # columns for severities
    renderer = gtk.CellRendererText ()
    column = gtk.TreeViewColumn ('Severity', renderer, text=COLUMN_SEVERITY)
    treeview.append_column (column)
    
    # column for description
    renderer = gtk.CellRendererText ()
    column = gtk.TreeViewColumn ('Description', renderer, text=COLUMN_DESCRIPTION)
    treeview.append_column (column)
    
def main ():
    win = gtk.Window ()
    if __name__ == '__main__':
	win.connect ('destroy', lambda win: gtk.main_quit ())
	
    win.set_title ('GtkListStore demo')
    win.set_border_width (8)
    
    vbox = gtk.VBox (gtk.FALSE, 8)
    win.add (vbox)
    
    label = gtk.Label ('This is the bug list (note: not based on real data, it would be nice to have a nice ODBC interface to bugzilla or so, though).')
    vbox.pack_start (label, gtk.FALSE, gtk.FALSE)
    
    sw = gtk.ScrolledWindow ()
    sw.set_shadow_type (gtk.SHADOW_ETCHED_IN)
    sw.set_policy (gtk.POLICY_NEVER,
                   gtk.POLICY_AUTOMATIC)
    vbox.pack_start (sw)
    
    model = create_model ()
    
    treeview = gtk.TreeView (model)
    treeview.set_rules_hint (gtk.TRUE)
    treeview.set_search_column (COLUMN_DESCRIPTION)
    
    sw.add (treeview)
    
    add_columns (treeview)
    
    win.set_default_size (280, 250)
    
    win.show_all ()
    if __name__ == '__main__': gtk.main ()
                                     
if __name__ == '__main__':
    main ()
	    
