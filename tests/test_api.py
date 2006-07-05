import unittest

from common import gtk

class APITest(unittest.TestCase):
    def testKeysyms(self):
        self.failUnless(hasattr(gtk.keysyms, 'Escape'))
        self.assertEqual(gtk.keysyms.Escape, 0xFF1B)
