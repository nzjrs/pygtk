#!/usr/bin/env python
#
# Tests of python-gtk.
#

import sys
from gtk import *
import GtkExtra

class Application:		
	def __init__(self, argv):
		
		# hardcoded font - maybe not on every server ?
		self.medium_fonts= [
			"lucidasanstypewriter-10",
			"lucidasanstypewriter-12",
			"lucidasanstypewriter-14",
			"lucidasanstypewriter-18",
			"lucidasanstypewriter-24"
			]
		self.bold_fonts= [
			"lucidasanstypewriter-bold-10",
			"lucidasanstypewriter-bold-12",
			"lucidasanstypewriter-bold-14",
			"lucidasanstypewriter-bold-18",
			"lucidasanstypewriter-bold-24"
			]
		self.font_type=0	
		self.font_index=0
		# dictionary of loaded fonts 
		self.fonts={}
								
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

	def init_text(self):		
		self.w_table=GtkTable(2,2,0)
		self.w_table.show()

		self.w_text=GtkText()
		self.w_text.set_usize(350,250)
		## Note: editable text is disable in gtk+-971109
		self.w_text.set_editable(0)
		self.w_table.attach(self.w_text,0,1,0,1,xpadding=1,ypadding=1)		
		self.w_text.show()

		self.w_text.connect("key_press_event", self.process_key)
		self.w_text.set_events(GDK.KEY_PRESS_MASK)

		self.w_hscrollbar=GtkHScrollbar(self.w_text.get_hadjustment())
		self.w_table.attach(self.w_hscrollbar,0,1,1,2,
				    yoptions=FILL)
		self.w_hscrollbar.show()
		
		self.w_vscrollbar=GtkVScrollbar(self.w_text.get_vadjustment())
		self.w_table.attach(self.w_vscrollbar,1,2,0,1,
				    xoptions=FILL)
		self.w_vscrollbar.show()
				
		self.w_vbox.pack_start(self.w_table)
					
	def process_file(self, action, widget):
		if action == 0: print "File:<unknown>"
		elif action == 1:
			print "File:New"
			self.w_text.freeze()
			self.w_text.set_point(0)
			self.w_text.insert_defaults("*new file*")
			self.w_text.thaw()
			#self.w_text.queueDraw()
		elif action == 2:
			print "File:Open"
			print GtkExtra.file_open_box(modal=FALSE), "chosen"
		elif action == 3:
			print "File:Save"
			GtkExtra.message_box("Test Application",
					     "Not Implemented",
					     ("OK",), pixmap='bomb.xpm')
		elif action == 4:
			print "File:Save As"
			print GtkExtra.message_box(modal=FALSE), "chosen"
		elif action == 5:
			print "File:Close"
			GtkExtra.message_box("Test Application",
					     "Not Implemented",
					     ("OK",), pixmap='bomb.xpm')
		elif action == 6:
			print "File:Exit"
			mainquit()

	def process_edit(self, action, widget):
		if action == 0: print "Edit:<unknown>"
		elif action == 1: print "Edit:Cut"
		elif action == 2: print "Edit:Copy"
		elif action == 3: print "Edit:Paste"

	def process_help(self, action, widget):
		GtkExtra.message_box("Test Application",
		"""Test Application for Python-Gtk.
		This is free software.
		This application tests key press events and text display.
		Any entered key (printable character) is displayed on
		screen. <F1> toggle medium and bold font. <F2> permits
		to change font size.
		""", ("OK",), pixmap='question.xpm')

	def process_key(self, win, event):
		if(event.keyval>=32 and event.keyval<=256):
			if(self.font_type==0):
				font=self.medium_fonts[self.font_index]
				if(not self.fonts.has_key(font)):
					self.fonts[font] = load_font(font)
				f=self.fonts[font]
			else:
				font=self.bold_fonts[self.font_index]
				if(not self.fonts.has_key(font)):
					self.fonts[font] = load_font(font)
				f=self.fonts[font]
			self.w_text.freeze()
			style = self.w_text.get_style()
			self.w_text.insert(f, style.fg[STATE_NORMAL],
					   style.bg[STATE_NORMAL],
					   chr(event.keyval))
			self.w_text.thaw()
		else:	
			# hardcoded key interpretation... XXX need some python keysymdef ?
			if(event.keyval==GDK.F1):
				# this is F1
				self.font_type = (self.font_type+1)%2
			elif(event.keyval==GDK.F2):	
				# this is F2
				self.font_index=(self.font_index+1)%len(self.medium_fonts)
		
if(__name__=="__main__"):
	use_defaults=0
	for arg in sys.argv:
		if(arg=="-d"):
			import pdb
			pdb.set_trace()

		if(arg=="-n"):
			use_defaults=0
			
	if(use_defaults==1):
		gtk.rc_parse("defaults.rc")
	
	app=Application(sys.argv)
	app.mainloop()
	
