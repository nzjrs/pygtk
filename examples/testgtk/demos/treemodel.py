#!/usr/bin/env python
'''Tree Model Test

This test is designed to demonstrate creating a new type of tree model
in python for use with the new tree widget in gtk 2.0.'''
description = 'Tree Model'

import gtk

# to create a new GtkTreeModel from python, you must derive from
# PyGtkTreeModel.
class MyTreeModel(gtk.PyGtkTreeModel):
    '''This class represents the model of a tree.  The iterators used
    to represent positions are converted to python objects when passed
    to the on_* methods.  This means you can use any python object to
    represent a node in the tree.  The None object represents a NULL
    iterator.

    In this tree, we use simple tuples to represent nodes, which also
    happen to be the tree paths for those nodes.  This model is a tree
    of depth 3 with 5 nodes at each level of the tree.  The values in
    the tree are just the string representations of the nodes.'''

    TREE_DEPTH = 4
    TREE_SIBLINGS = 5
    def __init__(self):
	'''constructor for the model.  Make sure you call
	PyGtkTreeModel.__init__'''
	gtk.PyGtkTreeModel.__init__(self)

    # the implementations for GtkTreeModel methods are prefixed with on_
    def on_get_flags(self):
	'''returns the GtkTreeModelFlags for this particular type of model'''
	return 0
    def on_get_n_columns(self):
	'''returns the number of columns in the model'''
	return 1
    def on_get_column_type(self, index):
	'''returns the type of a column in the model'''
	STRING_TYPE = 14 # XXXX have to fix this up at some point
	return STRING_TYPE
    def on_get_path(self, node):
	'''returns the tree path (a tuple of indices at the various
	levels) for a particular node.'''
	return node
    def on_get_value(self, node, column):
	'''returns the value stored in a particular column for the node'''
	assert column == 0
	return `node`
    def on_iter_next(self, node):
	'''returns the next node at this level of the tree'''
	if node[-1] == self.TREE_SIBLINGS - 1: # last node at level
	    return None
	return node[:-1] + (node[-1]+1,)
    def on_iter_children(self, node):
	'''returns the first child of this node'''
	if node == None: # top of tree
	    return (0,)
	if len(node) >= self.TREE_DEPTH: # no more levels
	    return None
	return node + (0,)
    def on_iter_has_child(self, node):
	'''returns true if this node has children'''
	return len(node) < self.TREE_DEPTH
    def on_iter_n_children(self, node):
	'''returns the number of children of this node'''
	if len(node) < self.TREE_DEPTH:
	    return self.TREE_SIBLINGS
	else:
	    return 0
    def on_iter_nth_child(self, node, n):
	'''returns the nth child of this node'''
        if node == None:
            return (n,)
	if len(node) < self.TREE_DEPTH and n < self.TREE_SIBLINGS:
	    return node + (n,)
	else:
	    return None
    def on_iter_parent(self, node):
	'''returns the parent of this node'''
	if len(node) == 0:
	    return None
	else:
	    return node[:-1]

def main():
    window = gtk.GtkWindow()
    if __name__ == '__main__':
        window.connect('destroy', lambda win: gtk.main_quit())
    window.set_title('Menus')

    model = MyTreeModel()
    tree_view = gtk.GtkTreeView(model)
    cell = gtk.GtkCellRendererText()
    # the text in the column comes from column 0
    column = gtk.GtkTreeViewColumn("tuples", cell, text=0)
    tree_view.append_column(column)

    window.add(tree_view)
    window.show_all()

    if __name__ == '__main__': gtk.main()

if __name__ == '__main__': main()

