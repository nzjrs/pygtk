import unittest

from common import pango

class MarkupTest(unittest.TestCase):
    def testMarkupAccelMarker(self):
        self.assertRaises(TypeError, pango.parse_markup, 'test', 0)
        self.assertEqual(pango.parse_markup('test')[2], u'\x00')
        self.assertEqual(pango.parse_markup('test', u't')[2], u'e')


class TestColor(unittest.TestCase):
    def testStr(self):
        col = pango.Color('red')
        self.assertEqual(str(col), '#ffff00000000')


class TestLanguage(unittest.TestCase):
    def testStr(self):
        lang = pango.Language('sv')
        self.assertEqual(str(lang), 'sv')


class TestFontDescription(unittest.TestCase):
    def testStr(self):
        font = pango.FontDescription('monospace 10')
        self.assertEqual(str(font), 'monospace 10')

    def testHash(self):
        font = pango.FontDescription('monospace 10')
        font2 = pango.FontDescription('monospace 10')
        self.assertNotEqual(hash(font), hash(font2))
        # FIXME: How to test this properly?

    def testEquals(self):
        font = pango.FontDescription('monospace 10')
        font2 = pango.FontDescription('monospace 10')
        font3 = pango.FontDescription('monospace 12')
        self.assertEqual(font, font2)
        self.assertNotEqual(font, font3)
