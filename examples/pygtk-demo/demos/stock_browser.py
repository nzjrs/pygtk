"""Stock Item and Icon Browser

This source code for this demo doesn't demonstrate anything
particularly useful in applications. The purpose of the "demo" is
 just to provide a handy place to browse the available stock icons
 and stock items."""

description = "Stock Browser"

import gobject
import gtk

window = None

class StockItemInfo(gobject.GObject):
    id = ''
    item = None
    small_icon = None
    macro = ''
    accel_str = ''
gobject.type_register(StockItemInfo)

class StockItemDisplay:
    pass

def id_to_macro(str):
    if str.startswith('gtk-'):
        str = str.replace('gtk-', 'gtk-stock-')
    str = str.upper()
    return str.replace('-', '_')
    
def create_model():
    store = gtk.ListStore(StockItemInfo)

    ids = gtk.stock_list_ids()
    ids.sort()

    for id in ids:
        info = StockItemInfo()
        info.id = id
        item = gtk.stock_lookup(info.id)
        if item:
            info.item = item
        else:
            info.item = [None, None, 0, 0, None]
        
        icon_set = gtk.icon_factory_lookup_default(info.id)
        if icon_set:
            sizes = icon_set.get_sizes()
            if gtk.ICON_SIZE_MENU in sizes:
                size = gtk.ICON_SIZE_MENU
            else:
                size = sizes[0]

            info.small_icon = window.render_icon(info.id,
                                                 size)
            if size != gtk.ICON_SIZE_MENU:
                 #w, h = gtk.icon_size_lookup(gtk.ICON_SIZE_MENU)
                 raise NotImplementedError

        if info.item[2]:
            info.accel_str = gtk.accelerator_name(info.item[3],
                                                  info.item[2])
            
        info.macro = id_to_macro(info.id)

        iter = store.append()
        store.set(iter, 0, info)
        
    return store

def get_largest_size(id):
    set = gtk.icon_factory_lookup_default(id)
    best_size = gtk.ICON_SIZE_INVALID
    best_pixels = 0
    
    for size in set.get_sizes():
        width, height = gtk.icon_size_lookup(size)
        if width * height > best_pixels:
            best_size = size
            best_pixels = width * height

    return best_size

def selection_changed(selection):
    treeview = selection.get_tree_view()
    display = treeview.get_data('stock-display')
    value = selection.get_selected()
    if value:
        model, iter = value
        info = model.get_value(iter, 0)
        
        if info.small_icon and info.item[1]:
            display.type_label.set_text('Icon and Item')
        elif info.small_icon:
            display.type_label.set_text('Icon only')
        elif info.item[1]:
            display.type_label.set_text('Item only')
        else:
            display.type_label.set_text('???????')

        display.macro_label.set_text(info.macro)
        display.id_label.set_text(info.id)

        if info.item[1]:
            display.label_accel_label.set_text_with_mnemonic('%s %s' % \
                                                             (info.item[1],
                                                              info.accel_str))
        else:
            display.label_accel_label.set_text('')

        if info.small_icon:
            display.icon_image.set_from_stock(info.id, get_largest_size(info.id))
        else:
            display.icon_image.set_from_pixbuf(None)
    else:
        display.type_label.set_text('No selected item')
        display.macro_label.set_text('')
        display.id_label.set_text('')
        display.label_accel_label.set_text('')
        display.icon_image.set_from_pixbuf(None)
            
def macro_set_func_text(tree, cell, model, iter):
    info = model.get_value(iter, 0)
    cell.set_property('text', info.macro)

def macro_set_func_pixbuf(tree, cell, model, iter):
    info = model.get_value(iter, 0)
    cell.set_property('pixbuf', info.small_icon)
    
def id_set_func(tree, cell, model, iter):
    info = model.get_value(iter, 0)
    cell.set_property('text', info.id)

def accel_set_func(tree, cell, model, iter):
    info = model.get_value(iter, 0)
    cell.set_property('text', info.accel_str)

def label_set_func(tree, cell, model, iter):
    info = model.get_value(iter, 0)
    cell.set_property('text', info.item[1])
    
def main():
    global window
    
    window = gtk.Window(gtk.WINDOW_TOPLEVEL)
    window.set_title('Stock Icons and Items')
    window.set_default_size(-1, 500)

    window.connect('destroy', lambda *x: gtk.mainquit())
    window.set_border_width(8)

    hbox = gtk.HBox(False, 8)
    window.add(hbox)

    sw = gtk.ScrolledWindow()
    sw.set_policy(gtk.POLICY_NEVER, gtk.POLICY_AUTOMATIC)
    hbox.pack_start(sw)

    model = create_model()

    treeview = gtk.TreeView(model)
    sw.add(treeview)

    column = gtk.TreeViewColumn()
    column.set_title('Macro')

    cell_renderer = gtk.CellRendererPixbuf()
    column.pack_start(cell_renderer, False)
    column.set_cell_data_func(cell_renderer, macro_set_func_pixbuf)
    cell_renderer = gtk.CellRendererText()
    column.pack_start(cell_renderer, True)
    column.set_cell_data_func(cell_renderer, macro_set_func_text)
    treeview.append_column(column)

    cell_renderer = gtk.CellRendererText()
    treeview.insert_column_with_data_func(-1, "Label", cell_renderer,
                                          label_set_func)
    treeview.insert_column_with_data_func(-1, "Accel", cell_renderer,
                                          accel_set_func)
    treeview.insert_column_with_data_func(-1, "ID",  cell_renderer,
                                          id_set_func)
    
    align = gtk.Alignment(0.5, 0.0, 0.0, 0.0)
    hbox.pack_start(align)

    frame = gtk.Frame("Selected Item")
    align.add(frame)

    vbox = gtk.VBox(False, 8)
    vbox.set_border_width(4)
    frame.add(vbox)

    
    display = StockItemDisplay()
    treeview.set_data('stock-display', display)
    display.type_label = gtk.Label()
    display.macro_label = gtk.Label()
    display.id_label = gtk.Label()
    display.label_accel_label = gtk.Label()
    display.icon_image = gtk.Image()
    
    vbox.pack_start(display.type_label)
    vbox.pack_start(display.icon_image)
    vbox.pack_start(display.label_accel_label)
    vbox.pack_start(display.macro_label)
    vbox.pack_start(display.id_label)

    selection = treeview.get_selection()
    selection.set_mode(gtk.SELECTION_SINGLE)
    selection.connect('changed', selection_changed)

    window.show_all()

if __name__ == '__main__':
    main()
    gtk.main()
    
