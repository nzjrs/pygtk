import unittest

from common import pango

class MarkupTest(unittest.TestCase):
    def testMarkupAccelMarker(self):
        self.assertRaises(TypeError, pango.parse_markup, 'test', 0)
        self.assertEqual(pango.parse_markup('test')[2], u'\x00')
        self.assertEqual(pango.parse_markup('test', u't')[2], u'e')
