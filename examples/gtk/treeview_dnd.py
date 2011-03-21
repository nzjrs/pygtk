#!/usr/bin/env python


# This file is based on http://code.google.com/p/quodlibet/source/browse/junk/dndtest.py


import pygtk
pygtk.require('2.0')
import gobject
import gtk


class TestTreeView(gtk.TreeView):
    __gtype_name__ = 'TestTreeView'

    def __init__(self, name):
        gtk.TreeView.__init__(self)

        model = gtk.ListStore(str)
        for i in range(10): model.append(row=['Item%d' % i])
        self.set_model(model)

        crt = gtk.CellRendererText()
        col = gtk.TreeViewColumn(name, crt, text=0)
        self.append_column(col)

        self.get_selection().set_mode(gtk.SELECTION_MULTIPLE)
        targets = [('text/uri-list', 0, 1)]
        self.drag_source_set(gtk.gdk.BUTTON1_MASK|gtk.gdk.CONTROL_MASK,
                             targets,
                             gtk.gdk.ACTION_COPY|gtk.gdk.ACTION_MOVE)
        self.drag_dest_set(gtk.DEST_DEFAULT_ALL,
                           targets,
                           gtk.gdk.ACTION_COPY|gtk.gdk.ACTION_MOVE)
        self.connect('drag-begin', self._drag_begin)
        self.connect('drag-data-delete', self._drag_data_delete)
        self.connect('drag-motion', self._drag_motion)
        self.connect('drag-data-get', self._drag_data_get)
        self.connect('drag-data-received', self._drag_data_received)

    def _drag_begin(self, view, ctx):
        return True

    def _drag_data_delete(self, view, ctx):
        if ctx.is_source and ctx.action == gtk.gdk.ACTION_MOVE:
            # For some reason this gets called twice.
            map(self.get_model().remove, self.__iters)
            self.__iters = []

    def _drag_motion(self, view, ctx, x, y, time):
        try:
            self.set_drag_dest_row(*self.get_dest_row_at_pos(x, y))
        except TypeError:
            self.set_drag_dest_row(len(self.get_model()) - 1, gtk.TREE_VIEW_DROP_AFTER)

        # You can also pass None as the first parameter to set_drag_dest_row
        #try:
        #    print 'Testing set_drag_dest_row(None, pos)'
        #    (path, pos) = self.get_dest_row_at_pos(x, y)
        #    self.set_drag_dest_row(None, pos)
        #except TypeError:
        #    self.set_drag_dest_row(len(self.get_model()) - 1, gtk.TREE_VIEW_DROP_AFTER)

        if ctx.get_source_widget() == self:
            kind = gtk.gdk.ACTION_MOVE
        else:
            kind = gtk.gdk.ACTION_COPY

        ctx.drag_status(kind, time)
        return True

    def _drag_data_get(self, view, ctx, sel, tid, etime):
        model, paths = self.get_selection().get_selected_rows()
        uris = ['file:///' + model[path][0] for path in paths]

        if tid == 1:
            if ctx.action == gtk.gdk.ACTION_MOVE:
                self.__iters = map(model.get_iter, paths)
            else:
                self.__iters = []

            sel.set('text/uri-list', 8, '\x00'.join(uris))
        else:
            sel.set_uris(uris)

            return True

    def _drag_data_received(self, view, ctx, x, y, sel, info, etime):
        model = view.get_model()

        if info == 1:
            files = sel.data.split('\x00')
        elif info == 2:
            files = sel.get_uris()

        try:
            path, position = self.get_dest_row_at_pos(x, y)
        except TypeError:
            path, position = len(model) - 1, gtk.TREE_VIEW_DROP_AFTER

        iter = model.get_iter(path)
        file = files.pop(0)

        if position in (gtk.TREE_VIEW_DROP_BEFORE, gtk.TREE_VIEW_DROP_INTO_OR_BEFORE):
            iter = model.insert_before(iter, [file[8:]])
        else:
            iter = model.insert_after(iter, [file[8:]])

        for file in files:
            iter = model.insert_after(iter, [file[8:]])

        ctx.finish(True, True, etime)
        return True


if __name__ == '__main__':
    w = gtk.Window()
    w.connect('delete-event', gtk.main_quit)
    w.add(gtk.HBox(spacing=3))
    w.child.pack_start(TestTreeView('Test 1'))
    w.child.pack_start(TestTreeView('Test 2'))
    w.show_all()
    gtk.main()
