#!/usr/bin/env python

import sys
import bdb
import repr
import string
import linecache # for linecache.getlines(filename)
from gtk import *
import GtkExtra

class GtkDb(GtkWindow, bdb.Bdb):
	def __init__(self):
		GtkWindow.__init__(self)
		bdb.Bdb.__init__(self)
		self.realize()

		self.set_title("GtkDb")
		self.connect("destroy", self.do_quit)
		self.connect("delete_event", self.do_quit)

		self.box = GtkVBox()
		self.add(self.box)
		self.box.show()

		toolbar = GtkToolbar(ORIENTATION_HORIZONTAL, TOOLBAR_BOTH)
		toolbar.set_space_size(10)
		self.box.pack_start(toolbar, expand=FALSE)
		toolbar.show()

		toolbar.append_item("next", "Next statement", None,
				    GtkPixmap(self, "next.xpm"), self.do_next)
		toolbar.append_item("step", "Step into function", None,
				    GtkPixmap(self, "step.xpm"), self.do_step)
		toolbar.append_space()
		toolbar.append_item("return",
				    "Continue execution to end of function",
				    None, GtkPixmap(self, "return.xpm"),
				    self.do_return)
		toolbar.append_space()
		toolbar.append_item("continue",
				    "Continue execution to next break point",
				    None, GtkPixmap(self, "continue.xpm"),
				    self.do_continue)
		toolbar.append_item("break",
				    "toggle break point at selected line",None,
				    GtkPixmap(self,"break.xpm"),self.do_break)
		toolbar.append_space()
		toolbar.append_item("edit",
				    "edit the value of the selected variable",
				    None, GtkPixmap(self, "edit.xpm"),
				    self.do_edit)
		toolbar.append_item("run",
			      "Execute some code in the current stack context",
			      None, GtkPixmap(self, "run.xpm"),
			      self.do_run)
		toolbar.append_space()
		toolbar.append_item("quit", "Quit the debugger", None,
				    GtkPixmap(self, "quit.xpm"), self.do_quit)

		sep = GtkHSeparator()
		self.box.pack_start(sep, expand=FALSE)
		sep.show()

		vpane = GtkVPaned()
		self.box.pack_start(vpane)
		vpane.show()

		hpane = GtkHPaned()
		vpane.add1(hpane)
		hpane.show()

		swin = GtkScrolledWindow()
		swin.set_policy(POLICY_AUTOMATIC, POLICY_AUTOMATIC)
		hpane.add1(swin)
		swin.show()

		self.stackdisp = GtkCList(1, ['Stack Frame'])
		self.stackdisp.connect("select_row", self.update_curstack)
		self.stackdisp.set_usize(280, 125)
		self.stackdisp.set_selection_mode(SELECTION_BROWSE)
		self.stackdisp.set_border_width(2)
		swin.add(self.stackdisp)
		self.stackdisp.show()

		swin = GtkScrolledWindow()
		swin.set_policy(POLICY_AUTOMATIC, POLICY_AUTOMATIC)
		hpane.add2(swin)
		swin.show()

		self.vardisp = GtkCList(3, ['local var', 'type', 'value'])
		self.vardisp.connect("select_row", self.update_selectedvar)
		self.vardisp.set_selection_mode(SELECTION_BROWSE)
		self.vardisp.set_column_width(0, 75)
		self.vardisp.set_column_width(1, 30)
		self.vardisp.set_border_width(2)
		swin.add(self.vardisp)
		self.vardisp.show()
		self.vardisp.selected = 0
		self.vardisp.varnames = []

		swin = GtkScrolledWindow()
		swin.set_policy(POLICY_AUTOMATIC, POLICY_AUTOMATIC)
		vpane.add2(swin)
		swin.show()

		self.filedisp = GtkCList(3, ['break', 'lineno', 'line'])
		self.minibreak = GtkPixmap(self, "minibreak.xpm")
		# This is giving a warning -- probably a bug in GTK
		self.filedisp.set_column_widget(0, self.minibreak)
		self.filedisp.column_titles_show()
		self.filedisp.set_column_width(0, 14)
		self.filedisp.set_column_width(1, 35)
		self.filedisp.connect("select_row", self.update_selection)
		self.filedisp.set_column_justification(1, JUSTIFY_RIGHT)
		self.filedisp.set_selection_mode(SELECTION_BROWSE)
		self.filedisp.set_border_width(2)
		self.filedisp.set_usize(600, 200)
		swin.add(self.filedisp)
		self.filedisp.show()

		separator = GtkHSeparator()
		self.box.pack_start(separator, expand=FALSE)
		separator.show()

		align = GtkAlignment(0.0, 0.5, 0.0, 0.0)
		self.box.pack_start(align, expand=FALSE)
		align.show()
		self.status = GtkLabel()
		self.status.set_padding(4, 1)
		align.add(self.status)
		self.status.show()

		self.filename = None
		self.selected = 0
		self.blockupdate = 0

	def set_status(self, str):
		self.status.set_text(str)

	def update_selection(self, clist, r, c, event):
		if self.blockupdate: return
		self.selected = r + 1
		if c == 0: # break point column
			self.do_break()

	def update_curstack(self, clist, r, c, event):
		if self.blockupdate: return
		self.curindex = r
		self.curframe = self.stack[self.curindex][0]
		self.lineno = None
		self.update_code_listing()
		self.update_var_listing()

	def update_selectedvar(self, clist, r, c, event):
		self.vardisp.selected = r
		
	def set_quit(self):
		self.hide()
		self.destroy()
		bdb.Bdb.set_quit(self)

	def reset(self):
		bdb.Bdb.reset(self)
		self.forget()
	def forget(self):
		self.lineno = None
		self.stack = []
		self.curindex = 0
		self.curframe = None
	def setup(self, f, t):
		self.forget()
		self.stack, self.curindex = self.get_stack(f, t)
		self.curframe = self.stack[self.curindex][0]

	# interaction functions -- overriden from bdb
	def user_line(self, frame):
		# called when we stop or break at this line
		self.interaction(frame, None)
	def user_return(self, frame, return_value):
		# called when a return trap is set here
		frame.f_locals['__return__'] = return_value
		if frame.f_code.co_name:
			func = frame.f_code.co_name
		else:
			func = "<lambda>"
		self.set_status(func + " rerturned " + repr.repr(return_value))
		self.interaction(frame, None)
	def user_exception(self, frame, (exc_type, exc_value, exc_traceback)):
		frame.f_locals['__exception__'] = exc_type, exc_value
		if type(exc_type) == type(''):
			exc_type_name = exc_type
		else: exc_type_name = exc_type.__name__
		self.set_status(exc_type_name + ':' + repr.repr(exc_value))
		self.interaction(frame, exc_traceback)

	def interaction(self, frame, traceback):
		self.setup(frame, traceback)
		self.update_stack_listing(self.curindex)
		mainloop()
		self.forget()

	def update_stack_listing(self, curindex):
		self.stackdisp.freeze()
		self.blockupdate = 1
		self.stackdisp.clear()
		for i in range(len(self.stack)):
			frame_lineno = self.stack[i]
			row = self.format_stack_entry(frame_lineno, "##!##")
			row = string.split(row, "##!##")[0]
			self.stackdisp.append([row])
		self.blockupdate = 0
		self.stackdisp.moveto(curindex, 0, 1.0, 0.0)
		self.stackdisp.select_row(curindex, 0)
		self.stackdisp.thaw()

	def update_var_listing(self):
		self.vardisp.freeze()
		self.vardisp.clear()
		locals = self.curframe.f_locals
		self.vardisp.varnames = locals.keys()
		self.vardisp.varnames.sort()
		for var in self.vardisp.varnames:
			row = [var, type(locals[var]).__name__, repr.repr(
				locals[var])]
			self.vardisp.append(row)
		self.vardisp.thaw()

	def update_code_listing(self):
		frame = self.curframe
		newfile = frame.f_code.co_filename
		if newfile != self.filename:
			lines = linecache.getlines(newfile)
			self.filename = newfile
			self.filedisp.freeze()
			self.blockupdate = 1
			self.filedisp.clear()
			breaks = self.get_file_breaks(newfile)
			for line in range(len(lines)):
				self.filedisp.append(['', repr.repr(line+1),
						      lines[line]])
				if line+1 in breaks:
					self.filedisp.set_pixmap(line, 0,
							 self.minibreak)
			self.blockupdate = 0
			self.filedisp.thaw()
		self.selected = frame.f_lineno
		lineno = self.selected
		self.filedisp.moveto(lineno - 1, 2)
		self.filedisp.select_row(lineno - 1, 2)

	def do_next(self, _b=None):
		self.set_next(self.curframe)
		mainquit()
	def do_step(self, _b=None):
		self.set_step()
		mainquit()
	def do_return(self, _b=None):
		self.set_return(self.curframe)
		mainquit()
	def do_continue(self, _b=None):
		self.set_continue()
		mainquit()
	def do_quit(self, _b=None, _e=None):
		self.set_quit()
		mainquit()
	def do_break(self, _b=None):
		breaks = self.get_file_breaks(self.filename)
		if self.selected in breaks:
			err = self.clear_break(self.filename, self.selected)
			if err:
				self.set_status(err)
				return
			self.filedisp.set_text(self.selected-1, 0, ' ')
		else:
			err = self.set_break(self.filename, self.selected)
			if err:
				self.set_status(err)
				return
			self.filedisp.set_pixmap(self.selected-1, 0,
						 self.minibreak)
	def do_run(self, _b=None):
		line = GtkExtra.input_box("Execute Code",
					  "Enter code to execute:")
		if line == None: return
		locals = self.curframe.f_locals
		globals = self.curframe.f_globals
		globals['__privileged__'] = 1
		try:
			code = compile(line + '\n', '<stdin>', 'single')
			exec code in globals, locals
		except:
			if type(sys.exc_type) == type(''):
				exc_type_name = sys.exc_type
			else: exc_type_name = sys.exc_type.__name__
			self.set_status('*** ' + exc_type_name + ': ' +
					str(sys.exc_value))
			return
		self.update_var_listing()

	def do_edit(self, _b=None):
		locals = self.curframe.f_locals
		varname = self.vardisp.varnames[self.vardisp.selected]
		val = repr.repr(locals[varname])
		value = GtkExtra.input_box("Edit Variable",
				"Enter new value for" +	varname + ":", val)
		if value == None: return
		globals = self.curframe.f_globals
		globals['__privileged__'] = 1
		try:
			val = eval(value, globals, locals)
			self.curframe.f_locals[varname] = val
		except:
			if type(sys.exc_type) == type(''):
				exc_type_name = sys.exc_type
			else: exc_type_name = sys.exc_type.__name__
			self.set_status('*** ' + exc_type_name + ': ' +
					str(sys.exc_value))
			return
		row = self.vardisp.selected
		self.vardisp.set_text(row, 1, type(val).__name__)
		self.vardisp.set_text(row, 2, repr.repr(val))

# this makes up the interface that is compatible with pdb.
def run(statement, globals=None, locals=None):
	win = GtkDb()
	win.show()
	win.run(statement, globals, locals)

def runeval(expression, globals=None, locals=None):
	win = GtkDb()
	win.show()
	return win.runeval(expression, globals, locals)

def runcall(*args):
	win = GtkDb()
	win.show()
	return apply(win.runcall, args)

def set_trace():
	win = GtkDb()
	win.show()
	win.set_trace()

def post_mortem(traceback):
	win = GtkDb()
	win.show()
	win.reset()
	win.interaction(None, traceback)

def pm():
	post_mortem(sys.last_traceback)

if __name__ == '__main__':
	import os
	if not sys.argv[1:]:
		print "usage: gtkdb.py scriptfile [args ...]"
		sys.exit(2)
	filename = sys.argv[1]
	del sys.argv[0] # delete gtkdb.py
	sys.path.insert(0, os.path.dirname(filename))

	run('execfile("' + filename + '")', {'__name__': '__main__'})
