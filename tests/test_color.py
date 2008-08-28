# -*- Mode: Python -*-

import unittest

from common import gtk


class Tests(unittest.TestCase):

    def test_constructor(self):
        """ Test GdkColor creation """

        c = gtk.gdk.Color(1, 2, 3)
        self.assertEqual(c.red, 1)
        self.assertEqual(c.green, 2)
        self.assertEqual(c.blue, 3)

        c = gtk.gdk.Color(pixel=0xffff)
        self.assertEqual(c.pixel, 0xffff)

        c = gtk.gdk.Color(pixel=0xffffL)
        self.assertEqual(c.pixel, 0xffff)

        c = gtk.gdk.Color(pixel=0xffffffffL)
        self.assertEqual(c.pixel, 0xffffffffL)

        c = gtk.gdk.Color('red')
        self.assertEqual(c.red, 65535)
        self.assertEqual(c.green, 0)
        self.assertEqual(c.blue, 0)

        c = gtk.gdk.Color('#ff0000')
        self.assertEqual(c.red, 65535)
        self.assertEqual(c.green, 0)
        self.assertEqual(c.blue, 0)

        self.assertRaises(TypeError, lambda: gtk.gdk.Color([]))

    def test_equal(self):
        self.assertEqual(gtk.gdk.Color(0, 0, 0), gtk.gdk.Color(0, 0, 0))
        self.assertEqual(gtk.gdk.Color(100, 200, 300), gtk.gdk.Color(100, 200, 300))
        self.assertEqual(gtk.gdk.Color('#abc'), gtk.gdk.Color('#aabbcc'))
        self.assertEqual(gtk.gdk.Color('#100020003000'), gtk.gdk.Color(0x1000, 0x2000, 0x3000))

    def test_not_equal(self):
        self.assertNotEqual(gtk.gdk.Color('red'), gtk.gdk.Color('blue'))
        self.assertNotEqual(gtk.gdk.Color(1, 0, 0), gtk.gdk.Color(0, 0, 0))
        self.assertNotEqual(gtk.gdk.Color(0, 1, 0), gtk.gdk.Color(0, 0, 0))
        self.assertNotEqual(gtk.gdk.Color(0, 0, 1), gtk.gdk.Color(0, 0, 0))

    def test_non_hashable(self):
        self.assertRaises(TypeError, lambda: hash(gtk.gdk.Color()))

        def dict_key():
            {} [gtk.gdk.Color()] = 'must raise'
        self.assertRaises(TypeError, dict_key)

    def test_repr(self):
        for color in self._test_color_list():
            self.assertEqual(color, eval(repr(color)))

    def test_str(self):
        for color in self._test_color_list():
            self.assertEqual(color, gtk.gdk.Color(str(color)))

    def _test_color_list(self):
        return [gtk.gdk.Color(),
                gtk.gdk.Color(10, 20, 30),
                gtk.gdk.Color(65535, 65535, 65535),
                gtk.gdk.Color('red'),
                gtk.gdk.Color('#aaa'),
                gtk.gdk.Color('#f0a000'),
                gtk.gdk.Color('#123abcdef'),
                gtk.gdk.Color('#123412341234'),
                gtk.gdk.Color('#fedcfedcfedc')]


if __name__ == '__main__':
    unittest.main()
