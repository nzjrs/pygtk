#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
	gtk.EntryCompletion Test
	(c) Fernando San Martín Woerner 2004
	snmartin@galilea.cl
	
	A simple test for gtk.EntryCompletion widget
"""
description = 'Entry Completion'


import pygtk; pygtk.require("2.0")
import gobject
import gtk

def on_match_selected(completion, model, iter):
	month = model.get_value(iter,0)
	print "You have selected " + month

def main():
	# The window...
	window = gtk.Window()
	window.set_title("EntryCompletion Test")
	window.connect('delete-event', gtk.main_quit)
	
	# The entry to fill with autocompletion
	entry = gtk.Entry()
	
	# The EntryCompletion Widget
	compl = gtk.EntryCompletion()
	
	# Connected to on_match_selected
	compl.connect("match-selected", on_match_selected)
	
	# The model with autocompletion
	model = gtk.ListStore(str)
	
	# A model and a text column
	compl.set_model(model)
	compl.set_text_column(0)
	
	# Adding rows
	model.append(["JANUARY"])
	model.append(["FEBRUARY"])
	model.append(["MARCH"])
	model.append(["APRIL"])
	model.append(["MAY"])
	model.append(["JUN"])
	model.append(["JULY"])
	model.append(["AUGUST"])
	model.append(["SEPTEMBER"])
	model.append(["OCTOBER"])
	model.append(["NOVEMBER"])
	model.append(["DECEMBER"])
	
	# Setting the widgets together 
	entry.set_completion(compl)
	
	vbox = gtk.VBox()
	window.add(vbox)
	vbox.pack_start(gtk.Label("Type a month:"))
	vbox.pack_start(entry)
	window.show_all()
	# Running
	gtk.main()

    
if __name__ == '__main__':
	main()
