import unittest

from common import gtk

class TreeViewTest(unittest.TestCase):
    # Check for #350252
    def _test_default_attributes(self):
        model = gtk.ListStore(str)
        treeview = gtk.TreeView(model)
        treeview.set_cursor(1)

if __name__ == '__main__':
    unittest.main()
