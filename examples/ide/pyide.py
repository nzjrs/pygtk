#!/usr/bin/env python

from gtk import *
import GtkExtra
import gtkcons, gtkdb, gtkprof, edit
import os, sys, string

# select a good VT emulator
for vt in 'Eterm', 'nxterm', 'xterm-color', 'xterm', 'rxvt':
	for dirname in string.split(os.environ['PATH'], os.pathsep):
		fullname = os.path.join(dirname, vt)
		if os.path.exists(fullname):
			VT_CMD = fullname + ' -geometry 80x6 -e '
			break
	else:
		continue
	break
else:
	VT_CMD=''  # this is not ideal

class Application(GtkWindow):
	def __init__(self):
		GtkWindow.__init__(self, WINDOW_TOPLEVEL)
		self.connect("destroy", self.quit)
		self.connect("delete_event", self.quit)
		self.set_title("Python")
		self.set_usize(475, 325)
		self.main_box = GtkVBox()
		self.add(self.main_box)
		self.main_box.show()
		hdlbox = GtkHandleBox()
		self.main_box.pack_start(hdlbox, expand=FALSE)
		hdlbox.show()
		mf = GtkExtra.MenuFactory()
		mf.add_entries([
			('File/New', '<control>N', self.file_new),
			('File/Open...', '<control>O', self.file_open),
			('File/<separator>', None, None),
			('File/Exit', '<control>Q', self.file_exit),
			('Edit/Copy', '<control>C', self.edit_copy),
			('Edit/Paste', '<control>V', self.edit_paste),
			('Edit/Clear', None, self.edit_clear),
			('Python/Reload Module...', None, self.python_reload),
			('Python/<separator>', None, None),
			('Python/Run...', None, self.python_run),
			('Python/Debug...', None, self.python_debug),
			('Python/Profile...', None, self.python_prof),
			('Help/About...', None, self.help_about),
		])
		self.add_accel_group(mf.accelerator)
		hdlbox.add(mf)
		mf.show()
		self.mf = mf
		self.interp = gtkcons.Console(
				namespace={'__builtins__': __builtins__,
					   '__name__': '__main__',
					   '__doc__': None}, quit_cb=self.quit)
		self.main_box.pack_start(self.interp)
		self.interp.show()
		self.interp.init()
		self.editwins = []
	def quit(self, *args):
		for win in self.editwins:
			if win.chk_save(): return
			win.hide()
			win.destroy()
		mainquit()

	def reload_file(self, fname):
		if not os.path.isfile(fname):
			GtkExtra.message_box("File Not Found", fname +
					     " was not found.", ("OK",))
		dir = os.path.dirname(fname)
		base = os.path.basename(fname)
		if dir not in sys.path: sys.path.insert(0, dir)
		if   string.lower(base[-3:]) == '.py':  base = base[:-3]
		elif string.lower(base[-4:]) == '.pyc': base = base[:-4]
		if not sys.modules.has_key(base):
			self.interp.run('import ' + base)
		else:
			self.interp.run('import ' + base)
			self.interp.run('reload(' + base + ')')

	# execute a python script normally or with the debugger or profiler
	def run_script(self, fname):
		if not fname or os.path.exists(fname):
			GtkExtra.message_box("Run","Invalid filename",("OK",))
			return
		args = GtkExtra.input_box("Arguments",
				 "Enter any command line arguments")
		if args == None: return
		os.system(VT_CMD+'python "'+fname+'" ' + args + ' &')
	def debug_script(self, fname):
		if not fname or os.path.exists(fname):
			GtkExtra.message_box("Debug", "Invalid filename",
					     ("OK",))
			return
		args = GtkExtra.input_box("Arguments",
				 "Enter any command line arguments")
		if args == None: return
		os.system(VT_CMD+'python '+gtkdb.__file__+' "'+fname+'" ' +
			  args + ' &')
	def profile_script(self, fname):
		if not fname or not os.ppath.exists(fname):
			GtkExtra.message_box("Profile", "Invalid filename",
					     ("OK",))
			return
		args = GtkExtra.input_box("Arguments",
				 "Enter any command line arguments")
		if args == None: return
		os.system(VT_CMD+'python '+gtkprof.__file__+' "'+fname+'" ' +
			  args + ' &')

	def add_py_menu(self, ew):
		def run(b=None, ew=ew, app=self): app.run_script(ew.fname)
		def dbg(b=None, ew=ew, app=self): app.debug_script(ew.fname)
		def prf(b=None, ew=ew, app=self): app.profile_script(ew.fname)
		py_menu = GtkExtra.MenuFactory(MENU_FACTORY_MENU)
		py_menu.add_entries([
			('Run...',     None, run),
			('Debug...',   None, dbg),
			('Profile...', None, prf)
		])
		mi = GtkMenuItem('Python')
		ew.mf.insert(mi, 2)
		mi.show()
		mi.set_submenu(py_menu)
		ew.mf.hide()
		ew.mf.show()
		
	def file_new(self, mi=None):
		ew = edit.EditWindow(quit_cb=self.rem_editwin)
		self.editwins.append(ew)
		self.add_py_menu(ew)
		ew.show()
		ew.set_usize(0,0)
	def file_open(self, mi=None):
		fname = GtkExtra.file_open_box(modal=FALSE)
		if fname:
			ew = edit.EditWindow(quit_cb=self.rem_editwin)
			ew.load_file(fname)
			self.editwins.append(ew)
			self.add_py_menu(ew)
			ew.show()
			ew.set_usize(0,0)
	def rem_editwin(self, win=None, event=None):
		for i in range(len(self.editwins)):
			if self.editwins[i] == win:
				del self.editwins[i]
				break
	def file_exit(self, mi=None):
		self.quit()
	def edit_copy(self, mi=None):
		self.interp.text.copy_clipboard(0)
	def edit_paste(self, mi=None):
		self.interp.line.paste_clipboard(0)
	def edit_clear(self, mi=None):
		self.interp.line.delete_selection()
	def python_reload(self, mi=None):
		print "python_reload"
	def python_run(self, mi=None):
		fname = GtkExtra.file_sel_box("Run")
		if fname:
			self.run_script(fname)
	def python_debug(self, mi=None):
		fname = GtkExtra.file_sel_box("Debug")
		if fname:
			self.debug_script(fname)
	def python_prof(self, mi=None):
		fname = GtkExtra.file_sel_box("Profile")
		if fname:
			self.profile_script(fname)
	def help_about(self, mi=None):
		print "help_about"

if __name__ == '__main__':
	app = Application()
	app.show()
	app.set_usize(0,0)
	mainloop()
