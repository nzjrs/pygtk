#!/usr/bin/env python

import profile, pstats, fpformat

from gtk import *

class PStatWindow(GtkWindow):
	def __init__(self, stats):
		GtkWindow.__init__(self)
		self.connect("destroy", self.quit)
		self.connect("delete_event", self.quit)
		self.set_title("Profile Statistics")

		self.stats = stats

		box1 = GtkVBox()
		self.add(box1)
		box1.show()

		text = `stats.total_calls` + " function calls "
		if stats.total_calls != stats.prim_calls:
			text = text + "( " + `stats.prim_calls` + \
			       " primitive calls) "
		text = text + "in " + fpformat.fix(stats.total_tt, 3) + \
		       " CPU seconds"
		label = GtkLabel(text)
		label.set_padding(2, 2)
		box1.pack_start(label, expand=FALSE)
		label.show()

		swin.set_policy(POLICY_AUTOMATIC, POLICY_AUTOMATIC)
		box1.pack_start(swin)
		swin.show()
		
		titles = ['ncalls', 'tottime', 'percall', 'cumtime',
			  'percall', 'filename:lineno(function)']
		swin = GtkScrolledWindow()
		clist = GtkCList(len(titles), titles)
		clist.set_column_width(0, 40)
		clist.set_column_width(1, 50)
		clist.set_column_width(2, 50)
		clist.set_column_width(3, 50)
		clist.set_column_width(4, 50)
		clist.set_usize(500, 200)
		self.clist = clist
		clist.set_border_width(10)
		swin.add(clist)
		clist.show()

		for i in range(5):
			clist.set_column_justification(i, JUSTIFY_RIGHT)

		self.insert_stats()

		separator = GtkHSeparator()
		box1.pack_start(separator, expand=FALSE)
		separator.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2, expand=FALSE)
		box2.show()

		button = GtkButton("close")
		button.connect("clicked", self.quit)
		self.close_button = button
		box2.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()

	def quit(self, *args):
		self.hide()
		self.destroy()
		mainquit()

	def get_stats_list(self):
		if self.stats.fcn_list:
			return self.stats.fcn_list[:]
		else:
			return sekf.stats.stats.keys()

	def insert_stats(self):
		list = self.get_stats_list()
		if list:
			row = [None] * 6
			self.clist.clear()
			self.clist.freeze()
			for func in list:
				cc,nc,tt,ct,callers = self.stats.stats[func]
				row[0] = `nc`
				if nc != cc:
					row[0] = row[0] + '/' + `cc`
				row[1] = fpformat.fix(tt, 3)
				if nc == 0:
					row[2] = ''
				else:
					row[2] = fpformat.fix(tt/nc, 3)
				row[3] = fpformat.fix(ct, 3)
				if cc == 0:
					row[4] = ''
				else:
					row[4] = fpformat.fix(ct/cc, 3)
				file,line,name = func
				row[5] = file + ":" + `line` + "(" + name + \
					 ")"
				self.clist.append(row)
			self.clist.thaw()
				

def run(cmd):
        prof = profile.Profile()
	try:
		stats = pstats.Stats(prof.run(cmd))
	except SystemExit:
		pass
	stats.strip_dirs().sort_stats("time", "module", "name")
	win = PStatWindow(stats)
	win.show()
	mainloop()

def run_file(file):
	return run('execfile("' + file + '")')


if __name__ == '__main__':
	import sys, os
	if not sys.argv[1:]:
		print "usage: gtkprof.py scriptfile [args ...]"
		sys.exit(2)
	filename = sys.argv[1]
	del sys.argv[0]
	sys.path.insert(0, os.path.dirname(filename))

	run_file(filename)


