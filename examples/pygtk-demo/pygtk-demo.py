#!/usr/bin/env python

import string
import gobject
import gtk

import demos

TITLE_COLUMN = 0
MODULE_COLUMN = 1

# globals
model = None
info_buffer = None
source_buffer = None
current_module = None

def load_module(module):
    global current_module

    current_module = module

    mod = getattr(demos, current_module)

    start, end = info_buffer.get_bounds()
    info_buffer.delete(start, end)

    if mod.__doc__:
	iter = info_buffer.get_iter_at_offset(0)

	lines = string.split(mod.__doc__ or '', '\n')
	info_buffer.insert(iter, lines[0])
	start = info_buffer.get_iter_at_offset(0)
	info_buffer.apply_tag_by_name('title', start, iter)
	info_buffer.insert(iter, '\n')
	for line in lines[1:]:
	    info_buffer.insert(iter, line)
	    info_buffer.insert(iter, '\n')

    start, end = source_buffer.get_bounds()
    source_buffer.delete(start, end)

    iter = source_buffer.get_iter_at_offset(0)
    filename = mod.__file__
    if filename[-4:] == '.pyc': filename = filename[:-1]
    source_buffer.insert(iter, open(filename, 'r').read())
    source_buffer.apply_tag_by_name('source',
                                    source_buffer.get_iter_at_offset(0),
                                    iter)

def selection_cb(selection):
    sel = selection.get_selected()
    if sel:
	model, iter = sel
	module = model.get_value(iter, MODULE_COLUMN)

	load_module(module)

def launch_cb(button):
    if current_module:
	mod = getattr(demos, current_module)
	mod.main()

def create_text(is_source=gtk.FALSE):
    scrolled_window = gtk.ScrolledWindow()
    scrolled_window.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
    scrolled_window.set_shadow_type(gtk.SHADOW_IN)

    text_view = gtk.TextView()
    scrolled_window.add(text_view)

    buffer = gtk.TextBuffer(None)
    text_view.set_buffer(buffer)
    text_view.set_editable(gtk.FALSE)
    text_view.set_cursor_visible(gtk.FALSE)

    text_view.set_wrap_mode(not is_source)

    return scrolled_window, buffer

def create_list():
    global model

    model = gtk.ListStore(gobject.TYPE_STRING, gobject.TYPE_STRING)
    tree_view = gtk.TreeView(model)
    selection = tree_view.get_selection()
    selection.set_mode('single')
    tree_view.set_usize(200, -1)

    for title, module in demos.demos:
	iter = model.append()
	model.set_value(iter, TITLE_COLUMN, title)
	model.set_value(iter, MODULE_COLUMN, module)
    cell = gtk.CellRendererText()
    # this column 
    column = gtk.TreeViewColumn("Widget", cell, text=TITLE_COLUMN)
    tree_view.append_column(column)

    selection.connect('changed', selection_cb)
    return tree_view

def main():
    global info_buffer, source_buffer

    window = gtk.Window()
    window.connect('destroy', lambda win: gtk.main_quit())

    hbox = gtk.HBox(gtk.FALSE, 3)
    window.add(hbox)

    list = create_list()
    hbox.pack_start(list, expand=gtk.FALSE)
    
    vbox = gtk.VBox()
    hbox.pack_start(vbox, expand=gtk.TRUE)

    notebook = gtk.Notebook()
    vbox.pack_start(notebook, expand=gtk.TRUE)
    scrolled_window, info_buffer = create_text(gtk.FALSE)
    l = gtk.Label('') ; l.set_text_with_mnemonic('_Info')
    notebook.append_page(scrolled_window, l)
    scrolled_window, source_buffer = create_text(gtk.TRUE)
    l = gtk.Label('') ; l.set_text_with_mnemonic('_Source')
    notebook.append_page(scrolled_window, l)

    tag = info_buffer.create_tag('title')
    tag.set_property('font', 'Sans 18')
    tag = source_buffer.create_tag('source')
    tag.set_property('font', 'Courier 10')
    tag.set_property('pixels_above_lines', 0)
    tag.set_property('pixels_below_lines', 0)

    button = gtk.Button('_Launch')
    button.set_border_width(5)
    button.set_flags(gtk.CAN_DEFAULT)
    vbox.pack_start(button, expand=gtk.FALSE)
    button.grab_default()
    button.connect('clicked', launch_cb)

    window.set_default_size(600, 400)
    window.show_all()

    load_module(demos.demos[0][1])

    gtk.main()

if __name__ == '__main__': main()
