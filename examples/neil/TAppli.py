#!/usr/bin/env python
#
# Test the GtkMenuBar, GtkMenu and GtkMenuItem, GtkList and
# GtkListItem, GtkFileSelection objects
#

import sys
import time
from gtk import *

class Application:		
	def __init__(self, argv):
		self.w_window=GtkWindow()
		self.w_window.set_title("Test application")
		self.w_window.set_border_width(10)
		self.w_vbox=GtkVBox()
		
		self.init_menu()
		self.init_list()
		self.init_button()
		
		self.w_window.add(self.w_vbox)
		self.w_vbox.show()
		self.w_window.show()		
		
		self.idlecount=0
		idle_add(self.idle)

	def mainloop(self):
		mainloop()

	def quit(self, mi):
		mainquit()
				
	def doit(self, button):
		z=[]
		for x in range(10,20):
			item=GtkListItem(label="line %d" % x)
			item.show()
			z.append(item)
		self.w_listbox.append_items(z)
	

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

	def init_list(self):		
		c=GtkScrolledWindow()	
		c.set_usize(250,200)
		self.w_listbox=GtkList()
		self.w_listbox.set_selection_mode(SELECTION_MULTIPLE)
		c.add_with_viewport(self.w_listbox)
		self.w_vbox.pack_start(c)
		
		for x in range(0,10):
			item=GtkListItem(label="line %d" % x)
			item.show()
			self.w_listbox.add(item)
		self.w_listbox.show()
		c.show()
		
	def init_button(self):
		t = GtkTable(rows=1, cols=2, homogeneous=TRUE)
		b1 = GtkButton('Do it!')
		b1.connect('clicked', self.doit)
		b2 = GtkButton('Quit')
		b2.connect('clicked', self.quit)
		t.attach(b1, 0, 1, 0, 1, yoptions=0, xpadding=2, ypadding=2)
		t.attach(b2, 1, 2, 0, 1, yoptions=0, xpadding=2, ypadding=2)
		
		self.w_vbox.pack_end(t, expand=FALSE)
				
		b1.show()
		b2.show()
		t.show()

	def process_file(self, action, widget):
		if action == 0: print "unknown"
		elif action == 1: print "File:New"
		elif action == 2: print "File:Open"
		elif action == 3: print "File:Save"
		elif action == 4: print "File:Save As"
		elif action == 5: print "File:Close"
		elif action == 6:
			print "File:Exit"
			mainquit()
	def process_edit(self, action, widget):
		if action == 0: print "Edit:<unknown>"
		elif action == 1: print "Edit:Cut"
		elif action == 2: print "Edit:Copy"
		elif action == 3: print "Edit:Paste"
	
	def idle(self):
		self.idlecount = self.idlecount + 1
		if(self.idlecount % 1000 == 0):
			print "Idle:", self.idlecount
			# if measuring time
			##self.quit()
		return TRUE

			
if(__name__=="__main__"):
	use_defaults=1
	for arg in sys.argv:
		if(arg=="-d"):
			import pdb
			pdb.set_trace()

		if(arg=="-n"):
			use_defaults=0
			
	start_time = time.time()
	
#	if(use_defaults==1):
#		gtk.rc_parse("defaults.rc")
	
	app=Application(sys.argv)
	app.mainloop()
	the_time = time.time() - start_time
	print "Application ran %.2f s." % the_time
	
