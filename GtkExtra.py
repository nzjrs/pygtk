#
# This file contains some convenience routines for gtk.py.
# It includes a python implementation of the GtkMenuFactory, and
# the functions message_box, file_open_box and file_save_box create
# and manage the respective dialogs.
#

from gtk import *
import string

MENU_FACTORY_MENU_BAR    = 0
MENU_FACTORY_MENU        = 1
MENU_FACTORY_OPTION_MENU = 2

# type is MENU_FACTORY_{MENU,MENU_BAR,OPTION_MENU}
class MenuFactory:
	def __init__(self, type=MENU_FACTORY_MENU_BAR):
		self.accelerator = GtkAccelGroup()
		if type == MENU_FACTORY_MENU_BAR:
			self.__w = GtkMenuBar()
			self.__ret = self.__w
		elif type == MENU_FACTORY_MENU:
			self.__w = GtkMenu()
			self.__w.set_accel_group(self.accelerator)
			self.__ret = self.__w
		elif type == MENU_FACTORY_OPTION_MENU:
			self.__w = GtkMenu()
			self.__w.set_accel_group(self.accelerator)
			self.__ret = GtkOptionMenu()
			self.__ret.set_menu(self.__w)
		self.__menus = {}
		self.__items = {}
	def __getattr__(self, key):
		return getattr(self.__ret, key)
	def add_entries(self, entries):
		for entry in entries:
			apply(self.create, tuple(entry))
	def create(self, path, accelerator=None, callback=None, *args):
		last_slash = string.rfind(path, '/')
		if last_slash < 0:
			parentmenu = self.__w
		else:
			parentmenu = self.get_menu(path[:last_slash])
		label = path[last_slash+1:]
		if label == '<separator>':
			item = GtkMenuItem()
		elif label[:7] == '<check>':
			item = GtkCheckMenuItem(label[7:])
		else:
			item = GtkMenuItem(label)
		if label != '<nothing>':
			item.show()
		if accelerator:
			key, mods = self.parse_accelerator(accelerator)
			item.add_accelerator("activate", self.accelerator,
					     key, mods, 'visible')
		if callback:
			apply(item.connect, ("activate", callback) + args)
		# right justify the help menu automatically
		if string.lower(label) == 'help' and parentmenu == self.__w:
			item.right_justify()
		parentmenu.append(item)
		self.__items[path] = item
		return item
	def get_menu(self, path):
		if path == '':
			return self.__w
		if self.__menus.has_key(path):
			return self.__menus[path]
		wid = self.create(path)
		menu = GtkMenu()
		menu.set_accel_group(self.accelerator)
		wid.set_submenu(menu)
		self.__menus[path] = menu
		return menu
	def parse_accelerator(self, accelerator):
		key = 0
		mods = 0
		done = FALSE
		while not done:
			if accelerator[:7] == '<shift>':
				mods = mods | GDK.SHIFT_MASK
				accelerator = accelerator[7:]
			elif accelerator[:5] == '<alt>':
				mods = mods | GDK.MOD1_MASK
				accelerator = accelerator[5:]
			elif accelerator[:6] == '<meta>':
				mods = mods | GDK.MOD1_MASK
				accelerator = accelerator[6:]
			elif accelerator[:9] == '<control>':
				mods = mods | GDK.CONTROL_MASK
				accelerator = accelerator[9:]
			else:
				done = TRUE
				key = ord(accelerator[0])
		return key, mods
	def remove_entry(self, path):
		if path not in self.__items.keys():
			return
		item = self.__items[path]
		item.destroy()
		length = len(path)
		# clean up internal hashes
		for i in self.__items.keys():
			if i[:length] == path:
				del self.__items[i]
		for i in self.__menus.keys():
			if i[:length] == path:
				del self.__menus[i]
	def remove_entries(self, paths):
		for path in paths:
			self.remove_entry(path)
	def find(self, path):
		return self.__items[path]

class _MessageBox(GtkDialog):
	def __init__(self, message="", buttons=(), pixmap=None, modal=TRUE):
		GtkDialog.__init__(self)
		self.connect("destroy", self.quit)
		self.connect("delete_event", self.quit)
		if modal:
			grab_add(self)
		hbox = GtkHBox(spacing=5)
		hbox.set_border_width(5)
		self.vbox.pack_start(hbox)
		hbox.show()
		if pixmap:
			self.realize()
			pixmap = GtkPixmap(self, pixmap)
			hbox.pack_start(pixmap, expand=FALSE)
			pixmap.show()
		label = GtkLabel(message)
		hbox.pack_start(label)
		label.show()

		for text in buttons:
			b = GtkButton(text)
			b.set_flags(CAN_DEFAULT)
			b.set_data("user_data", text)
			b.connect("clicked", self.click)
			self.action_area.pack_start(b)
			b.show()
		self.ret = None
	def quit(self, *args):
		self.hide()
		self.destroy()
		mainquit()
	def click(self, button):
		self.ret = button.get_data("user_data")
		self.quit()

# create a message box, and return which button was pressed		
def message_box(title="Message Box", message="", buttons=(), pixmap=None,
		modal=TRUE):
	win = _MessageBox(message, buttons, pixmap=pixmap, modal=modal)
	win.set_title(title)
	win.show()
	mainloop()
	return win.ret

class _EntryDialog(GtkDialog):
	def __init__(self, message="", modal=TRUE):
		GtkDialog.__init__(self)
		self.connect("destroy", self.quit)
		self.connect("delete_event", self.quit)
		if modal:
			grab_add(self)
		box = GtkVBox(spacing=10)
		box.set_border_width(10)
		self.vbox.pack_start(box)
		box.show()
		if message:
			label = GtkLabel(message)
			box.pack_start(label)
			label.show()
		self.entry = GtkEntry()
		box.pack_start(self.entry)
		self.entry.show()
		self.entry.grab_focus()

		button = GtkButton("OK")
		button.connect("clicked", self.click)
		button.set_flags(CAN_DEFAULT)
		self.action_area.pack_start(button)
		button.show()
		button.grab_default()
		button = GtkButton("Cancel")
		button.connect("clicked", self.quit)
		button.set_flags(CAN_DEFAULT)
		self.action_area.pack_start(button)
		button.show()

		self.ret = None
	def quit(self, w=None, event=None):
		self.hide()
		self.destroy()
		mainquit()
	def click(self, button):
		self.ret = self.entry.get_text()
		self.quit()

def input_box(title="Input Box", message="", modal=TRUE):
	win = _EntryDialog(message, modal=modal)
	win.set_title(title)
	win.show()
	mainloop()
	return win.ret

# File selection helper functions
class _FileSelection(GtkFileSelection):
	def __init__(self, modal=TRUE):
		GtkFileSelection.__init__(self)
		self.connect("destroy", self.quit)
		self.connect("delete_event", self.quit)
		if modal:
			grab_add(self)
		self.cancel_button.connect('clicked', self.quit)
		self.ok_button.connect('clicked', self.ok_cb)
		self.ret = None
	def quit(self, *args):
		self.hide()
		self.destroy()
		mainquit()
	def ok_cb(self, b):
		self.ret = self.get_filename()
		self.quit()

def file_sel_box(title="Browse", modal=FALSE):
	win = _FileSelection(modal=modal)
	win.set_title(title)
	win.show()
	mainloop()
	return win.ret

def file_open_box(modal=TRUE):
	return file_sel_box("Open", modal=modal)
def file_save_box(modal=TRUE):
	return file_sel_box("Save As", modal=modal)

def debug_main_quit():
	"""Create a window with a button to call mainquit"""
        win = GtkWindow()
	win.set_title("Quit")
	win.set_usize(125, -1)
	b = GtkButton("Main Quit")
	b.connect("clicked", mainquit)
	win.add(b)
	b.show()
	win.show()
