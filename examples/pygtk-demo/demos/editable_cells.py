"""Tree View/Editable Cells

This demo demonstrates the use of editable cells in a GtkTreeView. If
you're new to the GtkTreeView widgets and associates, look into
the GtkListStore example first.

"""

description = 'Editable cells'

import gobject
import gtk
from gtk import TRUE, FALSE

COLUMN_NUMBER = 0
COLUMN_PRODUCT = 1
COLUMN_EDITABLE = 2

ARTICLES = [
    [3, "bottles of coke", TRUE],
    [5, "packages of noodles", TRUE],
    [2, "packages of chocolate chip cookies", TRUE],
    [1, "can vanilla ice cream", TRUE],
    [6, "eggs", TRUE]
]

def create_model():
    # create model
    model = gtk.ListStore(gobject.TYPE_INT,
                          gobject.TYPE_STRING,
                          gobject.TYPE_BOOLEAN)

    # insert articles
    for article in ARTICLES:
        iter = model.append()
        model.set(iter,
                  COLUMN_NUMBER, article[0],
                  COLUMN_PRODUCT, article[1],
                  COLUMN_EDITABLE, article[2])
    return model

def add_item(button, model):
    item = [0, "Description here", TRUE]

    ARTICLES.append(item)

    iter = model.append()
    model.set(iter,
              COLUMN_NUMBER, item[0],
              COLUMN_PRODUCT, item[1],
              COLUMN_EDITABLE, item[2])
    
def remove_item(button, treeview):
    selection = treeview.get_selection()
    selected = selection.get_selected()
    if selected:
        model, iter = selected
        i = model.get_path(iter)[0]
        model.remove(iter)
        ARTICLES.pop(i)

def cell_edited (cell, row, new_text, model):
    article = ARTICLES[int(row)]
    iter = model.get_iter_from_string(row)
    
    column = cell.get_data("column")
    if column == COLUMN_NUMBER:
        article[column] = int(new_text)
        model.set(iter, column, article[column])
    elif column == COLUMN_PRODUCT:
        article[column] = new_text
        model.set(iter, column, article[column]) 

def add_columns(treeview):
    model = treeview.get_model()

    # number column
    renderer = gtk.CellRendererText()
    renderer.connect('edited', cell_edited, model)
    renderer.set_data('column', COLUMN_NUMBER)
    treeview.insert_column_with_attributes(-1, 'Number', renderer,
                                           text=COLUMN_NUMBER,
                                           editable=COLUMN_EDITABLE)
    # product column
    renderer = gtk.CellRendererText()
    renderer.connect('edited', cell_edited, model)
    renderer.set_data('column', COLUMN_PRODUCT)

    treeview.insert_column_with_attributes(-1, 'Number', renderer,
                                           text=COLUMN_PRODUCT,
                                           editable=COLUMN_EDITABLE)
    
def main():
    # create window, etc
    window = gtk.Window()
    window.set_title('Shopping list')
    window.set_border_width(5)
    window.connect('destroy', lambda win: gtk.mainquit())

    vbox = gtk.VBox()
    window.add(vbox)

    label = gtk.Label("Shopping list (you can edit the cells!)")
    vbox.pack_start(label, FALSE, FALSE)

    sw = gtk.ScrolledWindow()
    sw.set_shadow_type(gtk.SHADOW_ETCHED_IN)
    sw.set_policy(gtk.POLICY_AUTOMATIC,
                  gtk.POLICY_AUTOMATIC)
    vbox.pack_start(sw)

    # create model
    model = create_model()

    # create tree view
    treeview = gtk.TreeView(model)
    treeview.set_rules_hint(TRUE)
    treeview.set_headers_visible(TRUE)
    selection = treeview.get_selection()
    selection.set_mode(gtk.SELECTION_SINGLE)

    add_columns(treeview)

    sw.add(treeview)

    # some buttons
    hbox = gtk.HBox()
    vbox.pack_start(hbox, FALSE, FALSE)

    button = gtk.Button('Add item')
    button.connect('clicked', add_item, model)
    hbox.pack_start(button)
    
    button = gtk.Button('Remove item')
    button.connect('clicked', remove_item, treeview)
    hbox.pack_start(button)

    window.set_default_size(330, 220)
    window.show_all()

    gtk.main()
    
if __name__=="__main__":
    main()
