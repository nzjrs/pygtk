#!/usr/bin/env python
import string

import pygtk
pygtk.require('2.0')

import gobject
import gtk
from gtk import TRUE, FALSE

import demos

TITLE_COLUMN = 0
MODULE_COLUMN = 1

class PyGtkDemo(gtk.Window):
    info_buffer = None
    source_buffer = None
    def __init__(self):
        gtk.Window.__init__(self)
        self.connect('destroy', lambda w: gtk.main_quit())
        self.set_default_size(600, 400)

        hbox = gtk.HBox(FALSE, 3)
        self.add(hbox)

        treeview = self.create_treeview()
        hbox.pack_start(treeview, expand=FALSE)

        self.notebook = gtk.Notebook()
        hbox.pack_start(self.notebook, expand=TRUE)
        
        scrolled_window, self.info_buffer = self.create_text(FALSE)
        self._new_notebook_page(scrolled_window, '_Info')
        tag = self.info_buffer.create_tag('title')
        tag.set_property('font', 'Sans 18')
        
        scrolled_window, self.source_buffer = self.create_text(TRUE)
        self._new_notebook_page(scrolled_window, '_Source')
        tag = self.source_buffer.create_tag('source')
        tag.set_property('font', 'Courier 10')
        tag.set_property('pixels_above_lines', 0)
        tag.set_property('pixels_below_lines', 0)

    def _new_notebook_page(self, widget, label):
        l = gtk.Label('')
        l.set_text_with_mnemonic(label)
        self.notebook.append_page(widget, l)

    def create_treeview(self):
        model = gtk.ListStore(gobject.TYPE_STRING,
                              gobject.TYPE_STRING)
        
        treeview = gtk.TreeView(model)
        treeview.connect('row-activated', self.row_activated_cb)
        selection = treeview.get_selection()
        selection.set_mode('single')
        treeview.set_size_request(200, -1)
        
        for title, module in demos.demos:
            iter = model.append()
            model.set_value(iter, TITLE_COLUMN, title)
            model.set_value(iter, MODULE_COLUMN, module)

        cell = gtk.CellRendererText()
        column = gtk.TreeViewColumn("Widget", cell, text=TITLE_COLUMN)
        treeview.append_column(column)

        selection.connect('changed', self.selection_changed_cb)
        self.model = model
        return treeview
    
    def create_text(self, is_source=FALSE):
        scrolled_window = gtk.ScrolledWindow()
        scrolled_window.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        scrolled_window.set_shadow_type(gtk.SHADOW_IN)

        text_view = gtk.TextView()
        scrolled_window.add(text_view)

        buffer = gtk.TextBuffer(None)
        text_view.set_buffer(buffer)
        text_view.set_editable(FALSE)
        text_view.set_cursor_visible(FALSE)

        text_view.set_wrap_mode(not is_source)

        return scrolled_window, buffer

    def row_activated_cb(self, treeview, row, column):
        iter = self.model.get_iter(row)
        module_name = self.model.get_value(iter, MODULE_COLUMN)
        module = getattr(demos, module_name)
        module.main()
        
    def selection_changed_cb(self, selection):
        selection = selection.get_selected()
        if not selection:
            return
        
        model, iter = selection

        name = model.get_value(iter, MODULE_COLUMN)
        self.load_module(name)

    def read_module(self, module):
        filename = module.__file__
        if filename[-4:] == '.pyc':
            filename = filename[:-1]
        fd = open(filename)
        return fd.read()
    
    def insert_documentation(self, module):
        buffer = self.info_buffer
        iter = buffer.get_iter_at_offset(0)

        lines = string.split(module.__doc__ or '', '\n')
        buffer.insert(iter, lines[0])
        start = buffer.get_iter_at_offset(0)
        buffer.apply_tag_by_name('title', start, iter)
        buffer.insert(iter, '\n')
        for line in lines[1:]:
            buffer.insert(iter, line)
            buffer.insert(iter, '\n')

    def clear_buffers(self):
        start, end = self.info_buffer.get_bounds()
        self.info_buffer.delete(start, end)

        start, end = self.source_buffer.get_bounds()
        self.source_buffer.delete(start, end)

    def insert_source(self, data):
        source_buffer = self.source_buffer
        iter = source_buffer.get_iter_at_offset(0)
        source_buffer.insert(iter, data)
        
        start = source_buffer.get_iter_at_offset(0)
        source_buffer.apply_tag_by_name('source', start, iter)
        
    def load_module(self, name):
        self.clear_buffers()
        
        module = getattr(demos, name)
        if module.__doc__:
            self.insert_documentation(module)
            
        source = self.read_module(module)
        self.insert_source(source)
            
d = PyGtkDemo()
d.show_all()

gtk.main()

#if __name__ == '__main__':
#    main()
