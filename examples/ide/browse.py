#!/usr/bin/env python

# This is an example of using dynamic trees (trees where nodes are only
# calculated as they are needed) with pygtk.  This particular example
# allows for the browsing of the variables in a module, allowing the
# descent into classes and modules and other objects with a __dict__
# attribute.

# If this file is run straight, it will let you browse the gtk module.

from gtk import *

class BrowseTreeItem(GtkTreeItem):
	def __init__(self, name, dict=None, disp=None):
		GtkTreeItem.__init__(self, name)
		self.name = name
		self.dict = dict
		self.disp = disp
		self.exp_id = self.connect("expand", self.sig_expand)
	def init_subtree(self):
		if self.dict:
			self.subtree = GtkTree()
			self.subtree.set_selection_mode(SELECTION_BROWSE)
			self.subtree.connect("select_child", self.subsel_child)
			self.set_subtree(self.subtree)
			self.subtree.show()
	def subsel_child(self, _t, _c):
		if self.disp:
			key = _c.children()[0].get()
			if key == '__builtins__':  # builtins includes itself
				value = key
			else:
				value = `self.dict[key]`
			self.disp.set_text(value)
	def sig_select(self):
		if self.disp:
			self.disp.set_text(value)
	def sig_expand(self, _t):
		keys = self.dict.keys()
		keys.sort()
		for key in keys:
			dict = None
			try:
				dict = vars(self.dict[key])
			except TypeError:
				pass
			item = BrowseTreeItem(key, dict, self.disp)
			self.subtree.append(item)
			item.init_subtree()
			item.show()
		self.disconnect(self.exp_id)

class BrowseVariables(GtkVBox):
	def __init__(self, name, dict):
		GtkVBox.__init__(self)
		self.set_spacing(2)

		self.sw = GtkScrolledWindow()
		self.sw.set_usize(300, 200)
		self.sw.set_policy(POLICY_AUTOMATIC, POLICY_AUTOMATIC)
		self.pack_start(self.sw)
		self.sw.show()

		self.disp = GtkEntry()
		self.disp.set_editable(FALSE)
		self.pack_start(self.disp, expand=FALSE)
		self.disp.show()

		self.root_tree = GtkTree()
		self.sw.add_with_viewport(self.root_tree)
		self.root_tree.show()

		self.browse = BrowseTreeItem(name, dict, self.disp)
		self.root_tree.append(self.browse)
		self.browse.init_subtree()
		self.browse.show()

class BrowseWindow(GtkWindow):
	def __init__(self, name, dict):
		GtkWindow.__init__(self)
		self.set_title("Browse Window")

		box = GtkVBox()
		self.add(box)
		box.show()

		browse = BrowseVariables(name, dict)
		browse.set_border_width(10)
		box.pack_start(browse)
		browse.show()
		
		separator = GtkHSeparator()
		box.pack_start(separator, expand=FALSE)
		separator.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box.pack_start(box2, expand=FALSE)
		box2.show()

		button = GtkButton("Close")
		box2.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()
		self.close_button = button

if __name__ == '__main__':
	import gtk
	win = BrowseWindow('gtk', vars(gtk))
	win.set_title("Browse gtk.py")
	win.connect("destroy", mainquit)
	win.connect("delete_event", mainquit)
	win.close_button.connect("clicked", mainquit)
	win.show()

	mainloop()
