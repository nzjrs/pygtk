#!/usr/bin/env python

# This is a sample implementation of an editor.

from gtk import *
import GtkExtra
import os

BLOCK_SIZE = 2048

class EditWindow(GtkWindow):
	def __init__(self, quit_cb=None):
		GtkWindow.__init__(self, WINDOW_TOPLEVEL)
		self.set_usize(470, 300)
		self.connect("delete_event", self.file_exit)

		self.quit_cb = quit_cb

		self.table = GtkTable(2,3)
		self.add(self.table)
		self.table.show()
		hdlbox = GtkHandleBox()
		self.table.attach(hdlbox, 0,2, 0,1, xoptions=FILL,
				  yoptions=FILL)
		hdlbox.show()
		self.menubar = self.create_menu()
		hdlbox.add(self.menubar)
		self.menubar.show()

		self.text = GtkText()
		self.text.connect("changed", self.set_dirty)
		self.text.set_editable(TRUE)
		self.table.attach(self.text, 0,1, 1,2)
		self.text.show()

		vadj = self.text.get_vadjustment()
		self.vscroll = GtkVScrollbar(vadj)
		self.table.attach(self.vscroll, 1,2, 1,2, xoptions=FILL)
		self.vscroll.show()
		vadj.connect('changed', self.chk_scroll)

		self.text.realize()
		self.dirty = 0
		self.file_new()
		self.text.grab_focus()

	def set_dirty(self, text=None):
		self.dirty = 1
	def chk_scroll(self, adj):
		if adj.upper - adj.lower <= adj.page_size:
			self.vscroll.hide()
		else:
			self.vscroll.show()

	def load_file(self, fname):
		try:
			fd = open(fname)
			self.text.freeze()
			self.text.delete_text(0, self.text.get_length())
			buf = fd.read(BLOCK_SIZE)
			while buf != '':
				self.text.insert_defaults(buf)
				buf = fd.read(BLOCK_SIZE)
			self.text.thaw()
			self.text.get_vadjustment().set_value(0)
			self.text.queue_draw()
			self.set_title(os.path.basename(fname))
			self.fname = fname
			self.dirty = 0
			self.new = 0
		except:
			GtkExtra.message_box('Edit', "Can't open " + fname,
					     ("OK",))

	def create_menu(self):
		mf = GtkExtra.MenuFactory()

		mf.add_entries([
			('File/New',        '<control>N', self.file_new),
			('File/Open...',    '<control>O', self.file_open),
			('File/Save',       '<control>S', self.file_save),
			('File/Save As...', None,         self.file_saveas),
			('File/<separator>',None,         None),
			('File/Exit',       '<control>Q', self.file_exit),
			('Edit/Cut',        '<control>X', self.edit_cut),
			('Edit/Copy',       '<control>C', self.edit_copy),
			('Edit/Paste',      '<control>V', self.edit_paste),
			('Edit/Clear',      None,         self.edit_clear),
			('Edit/<separator>',None,         None),
			('Edit/Find...',    None,         self.edit_find),
			('Edit/Find Next',  None,         self.edit_find_next),
			('Help/About...',   None,         self.help_about)
		])
		# activate key bindings ...
		self.add_accel_group(mf.accelerator)
		self.mf = mf
		return mf

	def chk_save(self):
		if self.dirty:
			ret = GtkExtra.message_box("Unsaved File",
						   (self.fname or "Untitled")+
						   " has not been saved\n" +
						   "Do you want to save it?",
						   ("Yes", "No", "Cancel"))
			if ret == None or ret == "Cancel": return 1
			if ret == "Yes": self.file_save()
		return 0

	def file_new(self, mi=None):
		if self.chk_save(): return
		self.text.freeze()
		self.text.delete_text(0, self.text.get_length())
		self.text.thaw()
		self.dirty = 0
		self.fname = None
		self.set_title("Untitled")
		self.new = 1
	def file_open(self, mi=None):
		if self.chk_save(): return
		fname = GtkExtra.file_open_box(modal=FALSE)
		if not fname: return
		self.load_file(fname)
	def file_save(self, mi=None):
		if self.new:
			self.file_saveas()
			return
		try:
			pos = 0
			length = self.text.get_length()
			fd = open(self.fname, "w")
			while pos < length:
				buf = self.text.get_chars(pos,
						  min(pos+BLOCK_SIZE, length))
				if buf != None: fd.write(buf)
				pos = pos + BLOCK_SIZE
			self.dirty = 0
		except:
			GtkExtra.message_box("Save", "Error saving file " +
					     self.fname, ("OK",))
		
	def file_saveas(self, mi=None):
		fname = GtkExtra.file_save_box(modal=FALSE)
		if not fname: return
		self.fname = fname
		self.set_title(os.path.basename(fname))
		self.new = 0
		self.file_save()
	def file_exit(self, mi=None, event=None):
		if self.chk_save(): return
		self.hide()
		self.destroy()
		if self.quit_cb: self.quit_cb(self)
	def edit_cut(self, mi):
		self.text.cut_clipboard(0)
	def edit_copy(self, mi):
		self.text.copy_clipboard(0)
	def edit_paste(self, mi):
		self.text.paste_clipboard(0)
	def edit_clear(self, mi):
		self.text.delete_selection()
	# I'll implement these later
	def edit_find(self, mi): pass
	def edit_find_next(self, mi): pass
	def help_about(self, mi):
		GtkExtra.message_box("Edit Window", "Copyright (C) 1998  " +
				     "James Henstridge\n" +
				     "This program is covered by the GPL>=2",
				     ("OK",))

def edit(fname, mainwin=FALSE):
	if mainwin: quit_cb = mainquit
	else:       quit_cb = None
	w = EditWindow(quit_cb=quit_cb)
	w.load_file(fname)
	w.show()
	w.set_usize(0,0)
	if mainwin: mainloop()
if __name__ == '__main__':
	import sys
	edit(sys.argv[-1], mainwin=TRUE)
