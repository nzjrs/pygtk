#!/usr/bin/env python

import string
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
    source_buffer.insert(iter, open(mod.__file__, 'r').read())

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
    scrolled_window = gtk.GtkScrolledWindow()
    scrolled_window.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
    scrolled_window.set_shadow_type(gtk.SHADOW_IN)

    text_view = gtk.GtkTextView()
    scrolled_window.add(text_view)

    buffer = gtk.GtkTextBuffer(None)
    text_view.set_buffer(buffer)
    text_view.set_editable(gtk.FALSE)
    text_view.set_cursor_visible(gtk.FALSE)

    text_view.set_wrap_mode(not is_source)

    return scrolled_window, buffer

def create_list():
    global model

    model = gtk.GtkListStore('gstring', 'gstring')
    tree_view = gtk.GtkTreeView(model)
    selection = tree_view.get_selection()
    selection.set_type('single')
    tree_view.set_usize(200, -1)

    for title, module in demos.demos:
	iter = model.append()
	model.set_cell(iter, TITLE_COLUMN, title)
	model.set_cell(iter, MODULE_COLUMN, module)
    cell = gtk.GtkCellRendererText()
    # this column 
    column = gtk.GtkTreeViewColumn("Widget", cell, text=TITLE_COLUMN)
    tree_view.append_column(column)

    selection.connect('selection_changed', selection_cb)
    return tree_view

def main():
    global info_buffer, source_buffer

    window = gtk.GtkWindow()
    window.connect('destroy', lambda win: gtk.main_quit())

    hbox = gtk.GtkHBox(gtk.FALSE, 3)
    window.add(hbox)

    list = create_list()
    hbox.pack_start(list, expand=gtk.FALSE)
    
    vbox = gtk.GtkVBox()
    hbox.pack_start(vbox, expand=gtk.TRUE)

    notebook = gtk.GtkNotebook()
    vbox.pack_start(notebook, expand=gtk.TRUE)
    scrolled_window, info_buffer = create_text(gtk.FALSE)
    notebook.append_page(scrolled_window, gtk.GtkLabel('Info'))
    scrolled_window, source_buffer = create_text(gtk.TRUE)
    notebook.append_page(scrolled_window, gtk.GtkLabel('Source'))

    tag = info_buffer.create_tag('title')
    #tag.set_param('font', 'Sans 18')
    tag = source_buffer.create_tag('source')
    #tag.set_param('font', 'Courier 10')
    #tag.set_param('pixels_above_lines', 0)
    #tag.set_param('pixels_below_lines', 0)

    button = gtk.GtkButton('Launch')
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
