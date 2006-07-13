import unittest

from common import gtk, gobject

class MyContainer(gtk.Container):
    __gtype_name__ = 'MyContainer'
    def __init__(self):
        gtk.Container.__init__(self)
        self.children = []
        self.props = {}

    def do_add(self, child):
        child.set_parent(self)
        self.children.append(child)
        self.props[child] = '' # we just have one child property

    def do_remove(self, child):
        widget_was_visible = child.flags() & gtk.VISIBLE
        child.unparent()
        self.children.remove(child)
        del self.props[child]

        if widget_was_visible:
            self.queue_resize()

    def do_forall(self, internal, callback, data):
        for child in self.children:
            callback(child, data)

    def do_set_child_property(self, child, property_id, value, pspec):
        if pspec.name == 'dumb-prop':
            self.props[child] = value

    def do_get_child_property(self, child, property_id, pspec):
        if pspec.name == 'dumb-prop':
            return self.props[child]

MyContainer.install_child_property(1,
                                   ('dumb_prop',
                                    gobject.TYPE_STRING,
                                    'Dumb Prop',
                                    'Dumb Property for testing purposes',
                                    '', gobject.PARAM_READWRITE))
    
class ContainerTest(unittest.TestCase):

    def testChildProperties(self):
        obj = MyContainer()
        label = gtk.Label()
        obj.add(label)
        v = 'dumb value'
        obj.child_set_property(label, 'dumb_prop', v)
        self.assertEqual(v, obj.child_get_property(label, 'dumb_prop'))

if __name__ == '__main__':
    unittest.main()
