#!/usr/bin/env python
#
# Tests of python-Gtk
#

import sys
from gtk import *
import GtkExtra

class Application:		
	def __init__(self, argv):
		self.w_window=GtkWindow(title="Test Application")
		self.w_window.set_border_width(10)
		self.w_vbox=GtkVBox()

		self.init_menu()
		self.init_canvas()
		
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
		help_cb = self.process_help
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
			('/_Edit/_Paste',   '<control>V', edit_cb, 3, ''),
			('/_Help',          None,         None, 0, '<LastBranch>'),
			('/_Help/_About',   '<control>A', help_cb, 1, '')
		])
		self.w_menubar = itemf.get_widget('<main>')
		self.w_vbox.pack_start(self.w_menubar, expand=FALSE)
		self.w_menubar.show()

	def init_canvas(self):		
		self.w_canvas=GtkDrawingArea()
		self.w_canvas.size(350,250)
		self.w_canvas.show()
		self.w_vbox.pack_start(self.w_canvas)
				
		self.w_canvas.connect("button_press_event", self.process_button)
		self.w_canvas.connect("motion_notify_event", self.process_motion)
		self.w_canvas.connect("key_press_event", self.process_key)
		self.w_canvas.set_events(GDK.BUTTON_PRESS_MASK |
					 GDK.POINTER_MOTION_MASK |
					 GDK.POINTER_MOTION_HINT_MASK |
					 GDK.KEY_PRESS_MASK)
					
	def process_file(self, action, widget):
		if action == 0: print "File:<unknwon>"
		elif action == 1:
			print "File:New"
		elif action == 2:
			print "File:Open"
			print GtkExtra.file_open_box(modal=FALSE), "chosen"
		elif action == 3:
			print "FileSave"
			dlg=GtkExtra.message_box("Test Application",
						 "Not implemented",
						 ("OK",), pixmap='bomb.xpm')
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

	def process_help(self, action, widget):
		dlg=GtkExtra.message_box("Test Application",
					 "Test Application for Python-Gtk.\n" +
					 "This is free software.\n" +
					 "This application tests mouse and " +
					 "key press events.", ("OK",),
					 pixmap='question.xpm')
		
	def process_button(self, win, event):
		print event, "b =", event.button, "x =", event.x, "y =", event.y
		
	def process_motion(self, win, event):
		print event, "is_hint =", event.is_hint, "x =", event.x, "y =", event.y
		
	def process_key(self, win, event):
		print event, "keyval =", event.keyval, "state =", event.state
		
			
if(__name__=="__main__"):
	use_defaults=1
	for arg in sys.argv:
		if(arg=="-d"):
			import pdb
			pdb.set_trace()

		if(arg=="-n"):
			use_defaults=0
			
#	if(use_defaults==1):
#		rc_parse("defaults.rc")
	
	app=Application(sys.argv)
	app.mainloop()
	
