#!/usr/bin/env python
#
# Test of gtk.Text widget.
#

import sys
import time
from gtk import *
import GtkExtra
				
class Application:		
	def __init__(self, argv):
		self.w_window=GtkWindow()
		self.w_window.set_title("Test application")
		self.w_window.set_border_width(10)
		self.w_vbox=GtkVBox()

		self.init_menu()
		self.init_text()
		
		self.w_window.add(self.w_vbox)
		self.w_vbox.show()
		self.w_window.show()		
		
	def mainloop(self):
		mainloop()

	def init_menu(self):
		ag = GtkAccelGroup()
		itemf = GtkItemFactory(GtkMenuBar, "<main>", ag)
		self.w_window.add_accel_group(ag)
		file_cb = self.process_file
		edit_cb = self.process_edit
		itemf.create_items([
			('/_File',          None,         None, 0, '<Branch>'),
			('/_File/_New',     '<control>N', file_cb, 1, ''),
			('/_File/_Open',    '<control>O', file_cb, 2, ''),
			('/_File/_Save',    '<control>S', file_cb, 3, ''),
			('/_File/Save _As', None,         file_cb, 4, ''),
			('/_File/_Close',   None,         file_cb, 5, ''),
			('/_File/sep1',     None,   file_cb, 0, '<Separator>'),
			('/_File/E_xit',    '<alt>F4',    file_cb, 6, ''),
			('/_Edit',          None,         None, 0, '<Branch>'),
			('/_Edit/C_ut',     '<control>X', edit_cb, 1, ''),
			('/_Edit/_Copy',    '<control>C', edit_cb, 2, ''),
			('/_Edit/_Paste',   '<control>V', edit_cb, 3, '')
		])
		self.w_menubar = itemf.get_widget('<main>')
		self.w_vbox.pack_start(self.w_menubar, expand=FALSE)
		self.w_menubar.show()

	def init_text(self):		
		self.w_table=GtkTable(2,2,0)
		self.w_table.show()
		
		self.w_text=GtkText()
		self.w_text.set_usize(350,250)
		## Note: editable text is disable in gtk+-971109
		self.w_text.set_editable(0)
		self.w_table.attach(self.w_text,0,1,0,1,xpadding=1,ypadding=1)
		self.w_text.show()

		self.w_hscrollbar=GtkHScrollbar(self.w_text.get_hadjustment())
		self.w_table.attach(self.w_hscrollbar,0,1,1,2,yoptions=FILL)
		self.w_hscrollbar.show()
		
		self.w_vscrollbar=GtkVScrollbar(self.w_text.get_vadjustment())
		self.w_table.attach(self.w_vscrollbar,1,2,0,1,xoptions=FILL)
		self.w_vscrollbar.show()
				
		self.w_vbox.pack_start(self.w_table)

	def process_file(self, action, widget):
		if action == 0: print "File:<unknown>"
		elif action == 1:
			print "File:New"
			self.w_text.freeze()
			self.w_text.set_point(0)
			self.w_text.forward_delete(self.w_text.get_length())
			self.w_text.insert_defaults("Hello")
			self.w_text.thaw()
			#self.w_text.queueDraw()
		elif action == 2:
			print "File:Open"
			fname = GtkExtra.file_open_box(modal=FALSE)
			if fname:
				try:
					f=open(fname, "r")
				except IOError:
					return
				self.w_text.freeze()
				while TRUE:
					line = f.readline()
					if line == "":
						break
					self.w_text.insert_defaults(line)
				self.w_text.thaw()
		elif action == 3:
			print "File:Save"
		elif action == 4:
			print "File:Save As"
			print GtkExtra.file_save_box(modal=FALSE), "chosen"
		elif action == 5:
			print "File:Close"
		elif action == 6:
			print "File:Exit"
			mainquit()
	def process_edit(self, action, widget):
		if action == 0: print "Edit:<unknown>"
		elif action == 1: print "Edit:Cut"
		elif action == 2: print "Edit:Copy"
		elif action == 3: print "Edit:Paste"
			
if __name__ == "__main__":
	use_defaults=0
	for arg in sys.argv:
		if arg == "-d":
			import pdb
			pdb.set_trace()

		if arg == "-n":
			use_defaults=0
			
	if use_defaults == 1:
		gtk.rc_parse("defaults.rc")
	
	app=Application(sys.argv)
	app.mainloop()
	
