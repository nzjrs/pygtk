#!/usr/bin/env python

from gtk import *
import GtkExtra

wins = {}

def delete_event(win, event=None):
	win.hide()
	# don't destroy window -- just leave it hidden
	return TRUE

def create_statusbar_test(_button):
	if not wins.has_key('statusbar_test'):
		win = GtkDialog()
		wins["statusbar_test"] = win
		win.connect("delete_event", delete_event)
		win.set_title("Status Bar Test")

		vbox = GtkVBox()
		win.action_area.pack_start( vbox )
		hbox = GtkHBox()
		vbox.add( hbox )
		statusbar = win.statusbar = GtkStatusbar()
		vbox.add( statusbar )
		statusbar.show()
		
		c = win.context_id = statusbar.get_context_id("test")
		statusbar.push( c, "First message")
		def pop_statusbar_test(_b, sbar = statusbar, context = c):
			sbar.pop( context )
		def push_statusbar_test(_b, sbar = statusbar, context = c):
			import time
			sbar.push( context, "Message "+str(time.time() ))

		button = GtkButton("Pop")
		button.connect("clicked", pop_statusbar_test)
		hbox.add(button)
		button.show()

		button = GtkButton("Push")
		button.connect("clicked", push_statusbar_test)
		hbox.add(button)
		button.show()

		button = GtkButton("close")
		button.connect("clicked", win.hide)
		hbox.add(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()

		hbox.show()
		vbox.show()
	wins["statusbar_test"].show()

		
def create_buttons(_button):
	if not wins.has_key("buttons"):
		win = GtkWindow()
		wins["buttons"] = win
		win.connect("delete_event", delete_event)
		win.set_title("buttons")
		box1 = GtkVBox()
		win.add(box1)
		box1.show()
		table = GtkTable(3, 3, FALSE)
		table.set_row_spacings(5)
		table.set_col_spacings(5)
		table.set_border_width(10)
		box1.pack_start(table)
		table.show()

		def toggle_show(b):
			if b.flags(VISIBLE):
				b.hide()
			else:
				b.show()
		button = map(lambda i: GtkButton("button"+str(i)), range(9))
		for i in range(9):
			button[i].connect("clicked",
					  lambda _b, _b2=button[(i+1)%9],
					  t=toggle_show: t(_b2))
			y,x = divmod(i, 3)
			table.attach(button[i], x,x+1, y,y+1)
			button[i].show()

		separator = GtkHSeparator()
		box1.pack_start(separator, expand=FALSE)
		separator.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2, expand=FALSE)
		box2.show()

		button = GtkButton("close")
		button.connect("clicked", win.hide)
		box2.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()
	wins["buttons"].show()
				   
def create_toggle_buttons(_button):
	if not wins.has_key("toggle_buttons"):
		win = GtkWindow()
		wins["toggle_buttons"] = win
		win.connect("delete_event", delete_event)
		win.set_title("toggle butttons")
		win.set_border_width(0)
		box1 = GtkVBox()
		win.add(box1)
		box1.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2)
		box2.show()
		for i in range(1, 4):
			button = GtkToggleButton("button" + str(i))
			box2.pack_start(button)
			button.show()
		separator = GtkHSeparator()
		box1.pack_start(separator, expand=FALSE)
		separator.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2, expand=FALSE)
		box2.show()

		button = GtkButton("close")
		button.connect("clicked", win.hide)
		box2.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()
	wins["toggle_buttons"].show()

def create_check_buttons(_button):
	if not wins.has_key("check_buttons"):
		win = GtkWindow()
		wins["check_buttons"] = win
		win.connect("delete_event", delete_event)
		win.set_title("check buttons")
		box1 = GtkVBox()
		win.add(box1)
		box1.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2)
		box2.show()

		for i in range(1,4):
			button = GtkCheckButton("button" + str(i))
			box2.pack_start(button)
			button.show()
		separator = GtkHSeparator()
		box1.pack_start(separator, expand=FALSE)
		separator.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2, expand=FALSE)
		box2.show()

		button = GtkButton("close")
		button.connect("clicked", win.hide)
		box2.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()
	wins["check_buttons"].show()

def create_radio_buttons(_button):
	if not wins.has_key("radio_buttons"):
		win = GtkWindow()
		wins["radio_buttons"] = win
		win.connect("delete_event", delete_event)
		win.set_title("radio buttons")
		box1 = GtkVBox()
		win.add(box1)
		box1.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2)
		box2.show()

                button1 = GtkRadioButton(None, "button1")
		box2.pack_start(button1)
		button1.show()
		for i in range(2,4):
			button = GtkRadioButton(button1, "button" + str(i))
			box2.pack_start(button)
			button.show()
		separator = GtkHSeparator()
		box1.pack_start(separator, expand=FALSE)
		separator.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2, expand=FALSE)
		box2.show()

		button = GtkButton("close")
		button.connect("clicked", win.hide)
		box2.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()
	wins["radio_buttons"].show()

def create_button_box(_button):
	if not wins.has_key("button_box"):
		win = GtkDialog()
		win.set_title("button box")
		wins["button_box"] = win
		win.connect("delete_event", delete_event)

		box1 = GtkHButtonBox()
		box1.set_usize(550, -1)
		box1.set_spacing(25)
		box1.set_layout(BUTTONBOX_EDGE)
		box1.set_border_width(10)
		win.vbox.pack_start(box1)
		box1.show()

		def bbox_start(_b, bbox=box1):
			bbox.set_layout(BUTTONBOX_START)
			bbox.queue_resize()
		def bbox_edge(_b, bbox=box1):
			bbox.set_layout(BUTTONBOX_EDGE)
			bbox.queue_resize()
		def bbox_spread(_b, bbox=box1):
			bbox.set_layout(BUTTONBOX_SPREAD)
			bbox.queue_resize()
		def bbox_end(_b, bbox=box1):
			bbox.set_layout(BUTTONBOX_END)
			bbox.queue_resize()

		button = GtkButton("start")
		button.connect("clicked", bbox_start)
		box1.add(button)
		button.show()
		button = GtkButton("edge")
		button.connect("clicked", bbox_edge)
		box1.add(button)
		button.show()
		button = GtkButton("spread")
		button.connect("clicked", bbox_spread)
		box1.add(button)
		button.show()
		button = GtkButton("end")
		button.connect("clicked", bbox_end)
		box1.add(button)
		button.show()

		button = GtkButton("close")
		button.connect("clicked", win.hide)
		win.action_area.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()
	wins["button_box"].show()

def make_toolbar(win):
		toolbar = GtkToolbar(ORIENTATION_HORIZONTAL, TOOLBAR_BOTH)
		bg = win.get_style().bg[STATE_NORMAL]

		pix, mask = create_pixmap_from_xpm(win, None, "test.xpm")

		def set_toolbar_horizontal(_b, toolbar=toolbar):
			toolbar.set_orientation(ORIENTATION_HORIZONTAL)
		def set_toolbar_vertical(_b, toolbar=toolbar):
			toolbar.set_orientation(ORIENTATION_VERTICAL)
		def set_toolbar_icons(_b, toolbar=toolbar):
			toolbar.set_style(TOOLBAR_ICONS)
		def set_toolbar_text(_b, toolbar=toolbar):
			toolbar.set_style(TOOLBAR_TEXT)
		def set_toolbar_both(_b, toolbar=toolbar):
			toolbar.set_style(TOOLBAR_BOTH)
		def set_toolbar_small_space(_b, toolbar=toolbar):
			toolbar.set_space_size(5)
		def set_toolbar_big_space(_b, toolbar=toolbar):
			toolbar.set_space_size(10)
		def set_toolbar_enable(_b, toolbar=toolbar):
			toolbar.set_tooltips(TRUE)
		def set_toolbar_disable(_b, toolbar=toolbar):
			toolbar.set_tooltips(FALSE)
			print "Hello"

		toolbar.append_item("Horizontal", "Horizontal toolbar layout",
				    "Horizontal toolbar layout",
				    GtkPixmap(pix, mask),
				    set_toolbar_horizontal)
		toolbar.append_item("Vertical", "Vertical toolbar layout",
				    "Vertical toolbar layout",
				    GtkPixmap(pix, mask),
				    set_toolbar_vertical)
		toolbar.append_space()
		toolbar.append_item("Icons", "Only show toolbar icons",
				    "Only Icons",
				    GtkPixmap(pix, mask),
				    set_toolbar_icons)
		toolbar.append_item("Text", "Only show toolbar text",
				    "Only Text",
				    GtkPixmap(pix, mask),
				    set_toolbar_text)
		toolbar.append_item("Both", "Show toolbar icons and text",
				    "Both Text and Icons",
				    GtkPixmap(pix, mask),
				    set_toolbar_both)
		toolbar.append_space()

		entry = GtkEntry()
		entry.show()
		toolbar.append_widget(entry, "", "")

		toolbar.append_space()
		toolbar.append_item("Small", "Use small spaces",
				    "Small spaces",
				    GtkPixmap(pix, mask),
				    set_toolbar_small_space)
		toolbar.append_item("Big", "Use big spaces",
				    "Big spaces",
				    GtkPixmap(pix, mask),
				    set_toolbar_big_space)
		toolbar.append_space()
		toolbar.append_item("Enable", "Enable tooltips",
				    "Enable tooltips",
				    GtkPixmap(pix, mask),
				    set_toolbar_enable)
		toolbar.append_item("Disable", "Disable tooltips",
				    "Disable tooltips",
				    GtkPixmap(pix, mask),
				    set_toolbar_disable)
		return toolbar

def create_toolbar(_button):
	if not wins.has_key("toolbar"):
		win = GtkWindow()
		wins["toolbar"] = win
		win.set_title("Toolbar test")
		win.set_policy(FALSE, TRUE, TRUE)
		win.connect("delete_event", delete_event)
		toolbar = make_toolbar(win)
		win.add(toolbar)
		toolbar.show()
	wins["toolbar"].show()

def create_handle_box(_button):
	if not wins.has_key("handle_box"):
		win = GtkWindow()
		wins["handle_box"] = win
		win.set_title("Handle Box Test")
		win.set_policy(FALSE, FALSE, FALSE)
		win.connect("delete_event", delete_event)
		win.set_border_width(20)

		hbox = GtkHandleBox()
		win.add(hbox)
		hbox.show()

		toolbar = make_toolbar(win)
		hbox.add(toolbar)
		toolbar.show()
	wins["handle_box"].show()

def create_reparent(_button):
	if not wins.has_key("reparent"):
		win = GtkWindow()
		wins["reparent"] = win
		win.connect("delete_event", delete_event)
		win.set_title("buttons")

		box1 = GtkVBox()
		win.add(box1)
		box1.show()

		box2 = GtkHBox(spacing=5)
		box2.set_border_width(10)
		box1.pack_start(box2)
		box2.show()

		label = GtkLabel("Hello World")
		frame = GtkFrame("Frame 1")
		box2.pack_start(frame)
		frame.show()
		box3 = GtkVBox(spacing=5)
		box3.set_border_width(5)
		frame.add(box3)
		box3.show()
		def reparent_label(button, new_parent=box3, label=label):
			label.reparent(new_parent)
		button = GtkButton("switch")
		button.connect("clicked", reparent_label)
		box3.pack_start(button, expand=FALSE)
		button.show()

		box3.pack_start(label, expand=FALSE)
		label.show()

		frame = GtkFrame("Frame 2")
		box2.pack_start(frame)
		frame.show()
		box3 = GtkVBox(spacing=5)
		box3.set_border_width(5)
		frame.add(box3)
		box3.show()
		def reparent_label(button, new_parent=box3, label=label):
			label.reparent(new_parent)
		button = GtkButton("switch")
		button.connect("clicked", reparent_label)
		box3.pack_start(button, expand=FALSE)
		button.show()

		separator = GtkHSeparator()
		box1.pack_start(separator, expand=FALSE)
		separator.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2, expand=FALSE)
		box2.show()

		button = GtkButton("close")
		button.connect("clicked", win.hide)
		box2.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()
	wins["reparent"].show()

def create_pixmap(_button):
	if not wins.has_key("pixmap"):
		win = GtkWindow()
		wins["pixmap"] = win
		win.connect("delete_event", delete_event)
		win.set_title("pixmap")

		box1 = GtkVBox()
		win.add(box1)
		box1.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2)
		box2.show()

		button = GtkButton()
		box2.pack_start(button, expand=FALSE, fill=FALSE)
		button.show()

		pix, msk = create_pixmap_from_xpm(win, None, "test.xpm")
		pixmap = GtkPixmap(pix, msk)
		label = GtkLabel("Pixmap\ntest")
		box3 = GtkHBox()
		box3.set_border_width(2)
		box3.add(pixmap)
		box3.add(label)
		button.add(box3)
		pixmap.show()
		label.show()
		box3.show()

		separator = GtkHSeparator()
		box1.pack_start(separator, expand=FALSE)
		separator.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2, expand=FALSE)
		box2.show()
		button = GtkButton("close")
		button.connect("clicked", win.hide)
		box2.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()
	wins["pixmap"].show()

def create_tooltips(_button):
	if not wins.has_key("tooltips"):
		win = GtkWindow()
		wins["tooltips"] = win
		win.connect("delete_event", delete_event)
		win.set_title("tooltips")

		tooltips = GtkTooltips()

		box1 = GtkVBox()
		win.add(box1)
		box1.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2)
		box2.show()

		button = GtkToggleButton("button1")
		box2.pack_start(button)
		button.show()
		tooltips.set_tip(button, "This is button 1", "button 1")

		button = GtkToggleButton("button2")
		box2.pack_start(button)
		button.show()
		tooltips.set_tip(button, "This is button 2", "button 2")

		button = GtkToggleButton("button3")
		box2.pack_start(button)
		button.show()
		tooltips.set_tip(button, "This is button 3.  This is also a really long tooltip which probably won't fit on a single line and will therefore need to be wrapped.  Hopefully the wrapping will work correctly.", "long")

		separator = GtkHSeparator()
		box1.pack_start(separator, expand=FALSE)
		separator.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2, expand=FALSE)
		box2.show()

		button = GtkButton("close")
		button.connect("clicked", win.hide)
		box2.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()

		tooltips.set_tip(button, "Push this button to close window",
				 "push")
		tooltips.enable()
	wins["tooltips"].show()

def create_menu(depth):
	if depth < 1: return None
	menu = GtkMenu()
	group = None
	for i in range(5):
		menuitem = GtkRadioMenuItem(group,
					    "item %2d - %d" % (depth, i+1))
		group = menuitem
		menu.append(menuitem)
		menuitem.show()
		if depth > 1:
		        submenu = create_menu(depth - 1)
			menuitem.set_submenu(submenu)
	return menu
def create_menus(_button):
	if not wins.has_key("menus"):
		win = GtkWindow()
		wins["menus"] = win
		win.connect("delete_event", delete_event)
		win.set_title("menus")

		box1 = GtkVBox()
		win.add(box1)
		box1.show()

		menubar = GtkMenuBar()
		box1.pack_start(menubar, expand=FALSE)
		menubar.show()

		menuitem = GtkMenuItem("test\nline2")
		menuitem.set_submenu(create_menu(2))
		menubar.append(menuitem)
		menuitem.show()

		menuitem = GtkMenuItem("foo")
		menuitem.set_submenu(create_menu(2))
		menubar.append(menuitem)
		menuitem.show()

		menuitem = GtkMenuItem("bar")
		menuitem.set_submenu(create_menu(2))
		menuitem.right_justify()
		menubar.append(menuitem)
		menuitem.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2)
		box2.show()

		optionmenu = GtkOptionMenu()
		optionmenu.set_menu(create_menu(1))
		box2.pack_start(optionmenu)
		optionmenu.show()

		separator = GtkHSeparator()
		box1.pack_start(separator, expand=FALSE)
		separator.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2, expand=FALSE)
		box2.show()

		button = GtkButton("close")
		button.connect("clicked", win.hide)
		box2.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()
	wins["menus"].show()

def create_scrolled_windows(_button):
	if not wins.has_key("scrolled_windows"):
		win = GtkDialog()
		wins["scrolled_windows"] = win
		win.connect("delete_event", delete_event)
		win.set_title("dialog")

		scrolled_window = GtkScrolledWindow()
		scrolled_window.set_border_width(10)
		scrolled_window.set_policy(POLICY_AUTOMATIC, POLICY_AUTOMATIC)
		win.vbox.pack_start(scrolled_window)
		scrolled_window.show()

		table = GtkTable(20, 20)
		table.set_row_spacings(10)
		table.set_col_spacings(10)
		scrolled_window.add_with_viewport(table)
		table.show()

		for i in range(20):
			for j in range(20):
				button = GtkToggleButton("button (%d,%d)" %
							 (i, j))
				table.attach(button, i,i+1, j,j+1)
				button.show()
		button = GtkButton("close")
		button.connect("clicked", win.hide)
		win.action_area.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()
	wins["scrolled_windows"].show()

def create_entry(_button):
	if not wins.has_key("entry"):
		win = GtkWindow()
		wins["entry"] = win
		win.connect("delete_event", delete_event)
		win.set_title("entry")

		box1 = GtkVBox()
		win.add(box1)
		box1.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2)
		box2.show()

		entry = GtkEntry()
		entry.set_text("Hello World")
		box2.pack_start(entry)
		entry.show()

		separator = GtkHSeparator()
		box1.pack_start(separator, expand=FALSE)
		separator.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2, expand=FALSE)
		box2.show()

		button = GtkButton("close")
		button.connect("clicked", win.hide)
		box2.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()
	wins["entry"].show()

def create_list(_button):
	if not wins.has_key("list"):
		win = GtkWindow()
		wins["list"] = win
		win.connect("delete_event", delete_event)
		win.set_title("list")

		box1 = GtkVBox()
		win.add(box1)
		box1.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2)
		box2.show()

		scrolled_win = GtkScrolledWindow()
		scrolled_win.set_policy(POLICY_AUTOMATIC, POLICY_AUTOMATIC)
		box2.pack_start(scrolled_win)
		scrolled_win.show()

		list = GtkList()
		list.set_selection_mode(SELECTION_MULTIPLE)
		list.set_selection_mode(SELECTION_BROWSE)
		scrolled_win.add_with_viewport(list)
		list.show()

		list_items = ("hello", "world", "blah", "foo", "bar", "argh")
		for i in list_items:
			list_item = GtkListItem(i)
			list.add(list_item)
			list_item.show()

		def list_add(_button, list=list, num=[1]):
			list_item = GtkListItem("added item "+str(num[0]))
			num[0] = num[0] + 1
			list_item.show()
			list.add(list_item)

		def list_remove(_button, list=list):
			selection = list.get_selection()
			list.remove_items(selection)
			for i in selection: i.destroy()

		box3 = GtkHBox(spacing=10)
		box2.pack_start(box3, expand=FALSE)
		box3.show()

		button = GtkButton("add")
		button.connect("clicked", list_add)
		box3.pack_start(button)
		button.show()

		button = GtkButton("remove")
		button.connect("clicked", list_remove)
		box3.pack_start(button)
		button.show()

		separator = GtkHSeparator()
		box1.pack_start(separator, expand=FALSE)
		separator.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2, expand=FALSE)
		box2.show()

		button = GtkButton("close")
		button.connect("clicked", win.hide)
		box2.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()
	wins["list"].show()

def create_clist(_button):
	if not wins.has_key("clist"):
		win = GtkWindow()
		wins["clist"] = win
		win.connect("delete_event", delete_event)
		win.set_title("clist")

		box1 = GtkVBox()
		win.add(box1)
		box1.show()

		box2 = GtkHBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2, expand=FALSE, fill=FALSE)
		box2.show()

		clist = GtkCList(7, map(lambda i: "Title "+str(i), range(7)))
		clist.set_column_width(0, 100)
		for i in range(1, 7):
			clist.set_column_width(i, 80)

		text = map(lambda i: "Column "+str(i), range(7))
		text[1] = "Right"
		text[2] = "Center"

		def add1000_clist(_button, clist=clist, t=text):
			clist.freeze()
			for i in range(1000):
				t[0] = "Row "+str(i)
				clist.append(t)
			clist.thaw()
		def add10000_clist(_button, clist=clist, t=text):
			clist.freeze()
			for i in range(10000):
				t[0] = "Row "+str(i)
				clist.append(t)
			clist.thaw()
		def clear_clist(_button, clist=clist):
			clist.clear()
		s = [0]
		def remove_row_clist(_button, clist=clist, selected=s):
			clist.remove(selected[0])
		def select_clist(_clist, r, c, event, selected=s):
			selected[0] = r

		button = GtkButton("Add 1,000 Rows")
		box2.pack_start(button)
		button.connect("clicked", add1000_clist)
		button.show()

		button = GtkButton("Add 10,000 Rows")
		box2.pack_start(button)
		button.connect("clicked", add10000_clist)
		button.show()

		button = GtkButton("Clear List")
		box2.pack_start(button)
		button.connect("clicked", clear_clist)
		button.show()

		button = GtkButton("Remove Row")
		box2.pack_start(button)
		button.connect("clicked", remove_row_clist)
		button.show()

		box2 = GtkHBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2, expand=FALSE, fill=FALSE)
		box2.show()

		button = GtkButton("Show Title Buttons")
		box2.pack_start(button)
		button.connect("clicked", clist.column_titles_show)
		button.show()

		button = GtkButton("Hide Title Buttons")
		box2.pack_start(button)
		button.connect("clicked", clist.column_titles_hide)
		button.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2)
		box2.show()

		clist.set_row_height(20)
		clist.connect("select_row", select_clist)
		clist.set_column_width(0, 100)

		for i in range(1, 7):
			clist.set_column_width(0, 80)
		clist.set_selection_mode(SELECTION_BROWSE)
		clist.set_column_justification(1, JUSTIFY_RIGHT)
		clist.set_column_justification(2, JUSTIFY_CENTER)

		for i in range(100):
			text[0] = "Row "+str(i)
			clist.append(text)
		clist.set_border_width(5)

		swin = GtkScrolledWindow()
		swin.set_policy(POLICY_AUTOMATIC, POLICY_AUTOMATIC)
		box2.pack_start(swin)
		swin.show()

		swin.add(clist)
		clist.show()

		separator = GtkHSeparator()
		box1.pack_start(separator, expand=FALSE)
		separator.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2, expand=FALSE)
		box2.show()

		button = GtkButton("close")
		button.connect("clicked", win.hide)
		box2.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()
	wins["clist"].show()

def create_color_selection(_button):
	if not wins.has_key("color_selection"):
		win = GtkColorSelectionDialog("color selection dialog")
		wins["color_selection"] = win
		win.colorsel.set_opacity(TRUE)
		win.colorsel.set_update_policy(UPDATE_CONTINUOUS)
		win.set_position(WIN_POS_MOUSE)
		win.connect("delete_event", delete_event)

		win.cancel_button.connect("clicked", win.hide)
		win.ok_button.connect("clicked", win.hide)
	wins["color_selection"].show()

def create_file_selection(_button):
	if not wins.has_key("file_selection"):
		win = GtkFileSelection("file selection dialog")
		wins["file_selection"] = win
		win.connect("delete_event", delete_event)
		def file_selection_ok(_button, fs=win):
			print fs.get_filename()
		win.ok_button.connect("clicked", file_selection_ok)
		win.cancel_button.connect("clicked", win.hide)
	wins["file_selection"].show()

def create_dialog(_button):
	if not wins.has_key("dialog"):
		win = GtkDialog()
		wins["dialog"] = win
		win.connect("delete_event", delete_event)
		win.set_title("dialog")

		button = GtkButton("OK")
		button.connect("clicked", win.hide)
		win.action_area.pack_start(button)
		button.show()

		def label_toggle(_button, label=[None], box=win.vbox):
			l = label[0]
			if not l:
				l = GtkLabel("Dialog Test")
				l.set_padding(10, 10)
				box.pack_start(l)
				l.show()
				label[0] = l
			else:
				l.destroy()
				label[0] = None

		button = GtkButton("Toggle")
		button.connect("clicked", label_toggle)
		win.action_area.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()
	wins["dialog"].show()

def create_range_controls(_button):
	if not wins.has_key("range_controls"):
		win = GtkWindow()
		wins["range_controls"] = win
		win.connect("delete_event", delete_event)
		win.set_title("range_controls")

		box1 = GtkVBox()
		win.add(box1)
		box1.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2)
		box2.show()

		adjustment = GtkAdjustment(0, 0, 101, 0.1, 1, 1)

		scale = GtkHScale(adjustment)
		scale.set_usize(150, 30)
		scale.set_update_policy(UPDATE_DELAYED)
		scale.set_digits(1)
		scale.set_draw_value(TRUE)
		box2.pack_start(scale)
		scale.show()

		scrollbar = GtkHScrollbar(adjustment)
		scrollbar.set_update_policy(UPDATE_DELAYED)
		box2.pack_start(scrollbar)
		scrollbar.show()

		separator = GtkHSeparator()
		box1.pack_start(separator, expand=FALSE)
		separator.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2, expand=FALSE)
		box2.show()

		button = GtkButton("close")
		button.connect("clicked", win.hide)
		box2.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()
	wins["range_controls"].show()

def create_rulers(_button):
	if not wins.has_key("rulers"):
		win = GtkWindow()
		wins["rulers"] = win
		win.connect("delete_event", delete_event)
		win.set_title("rulers")
		win.set_usize(300, 300)
		# these should be defined elsewhere
		GDK_POINTER_MOTION_MASK      = 1 << 2
		GDK_POINTER_MOTION_HINT_MASK = 1 << 3
		win.set_events(GDK_POINTER_MOTION_MASK |
			       GDK_POINTER_MOTION_HINT_MASK)

		table = GtkTable(2, 3)
		win.add(table)
		table.show()

		ruler = GtkHRuler()
		ruler.set_range(5, 15, 0, 20)
		ruler.set_metric(PIXELS)
		def motion_notify(obj, event, ruler=ruler):
			return ruler.emit("motion_notify_event", event)
		win.connect("motion_notify_event", motion_notify)
		table.attach(ruler, 1,2, 0,1, yoptions=FILL)
		ruler.show()

		ruler = GtkVRuler()
		ruler.set_range(5, 15, 0, 20)
		ruler.set_metric(PIXELS)
		def motion_notify(obj, event, ruler=ruler):
			return ruler.emit("motion_notify_event", event)
		win.connect("motion_notify_event", motion_notify)
		table.attach(ruler, 0,1, 1,2, xoptions=FILL)
		ruler.show()

		label = GtkLabel("The rulers now work!\n" +
				 "They use the new interface to\n" +
				 "gtk_signal_emit.")
		table.attach(label, 1,2, 1,2)
		label.show()

		# XXX Finish this
	wins["rulers"].show()

def create_text(_button):
	if not wins.has_key("text"):
		win = GtkWindow()
		wins["text"] = win
		win.connect("delete_event", delete_event)
		win.set_title("text")

		box1 = GtkVBox()
		win.add(box1)
		box1.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2)
		box2.show()

		table = GtkTable(2, 2)
		table.set_row_spacing(0, 2)
		table.set_col_spacing(0, 2)
		box2.pack_start(table)
		table.show()

		text = GtkText()
		text.set_editable(FALSE)
		table.attach(text, 0,1, 0,1)
		text.show()

		hscrollbar = GtkHScrollbar(text.get_hadjustment())
		table.attach(hscrollbar, 0,1, 1,2, yoptions=FILL)
		hscrollbar.show()

		vscrollbar = GtkVScrollbar(text.get_vadjustment())
		table.attach(vscrollbar, 1,2, 0,1, xoptions=FILL)
		vscrollbar.show()

		text.freeze()
#		style = text.get_style()
#		font = style.font ; fg = style.white; bg = style.bg[
	        text.insert_defaults("some text\n")
		text.insert_defaults("more text\n")
		text.insert_defaults("\n")
		text.insert_defaults("(a blank line)\n")
		text.insert_defaults("\n")
		text.insert_defaults("beaverloop\n")
		text.insert_defaults("\n")
		text.insert_defaults("Bad Acid ... Jazz!\n")
		text.insert_defaults("\n")
		text.insert_defaults("They all call me mellow yellow,\n")
		text.insert_defaults("Thats 'cause I'm a mellow fellow,\n")
		text.insert_defaults("You walk by and I'll say hello,\n")
		text.thaw()

		separator = GtkHSeparator()
		box1.pack_start(separator, expand=FALSE)
		separator.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2, expand=FALSE)
		box2.show()

		button = GtkButton("close")
		button.connect("clicked", win.hide)
		box2.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()
	wins["text"].show()

def create_notebook(_button):
	if not wins.has_key("notebook"):
		win = GtkWindow()
		wins["notebook"] = win
		win.connect("delete_event", delete_event)
		win.set_title("notebook")

		box1 = GtkVBox()
		win.add(box1)
		box1.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2)
		box2.show()

		notebook = GtkNotebook()
		notebook.set_tab_pos(POS_TOP)
		box2.pack_start(notebook)
		notebook.show()

		for i in range(5):
			frame = GtkFrame("Page " + str(i))
			frame.set_border_width(10)
			frame.set_usize(200, 150)
			frame.show()
			label = GtkLabel("Page " + str(i))
			frame.add(label)
			label.show()
			label = GtkLabel("Tab " + str(i))
			notebook.append_page(frame, label)
		separator = GtkHSeparator()
		box1.pack_start(separator, expand=FALSE)
		separator.show()

		box2 = GtkHBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2, expand=FALSE)
		box2.show()

		button = GtkButton("close")
		button.connect("clicked", win.hide)
		box2.pack_start(button)
		button.show()

		button = GtkButton("next")
		button.connect("clicked", notebook.next_page)
		box2.pack_start(button)
		button.show()

		button = GtkButton("prev")
		button.connect("clicked", notebook.prev_page)
		box2.pack_start(button)
		button.show()

		def rotate_notebook(_b, note=notebook, pos=[POS_TOP]):
			pos[0] = (pos[0] + 1) % 4
			note.set_tab_pos(pos[0])

		button = GtkButton("rotate")
		button.connect("clicked", rotate_notebook)
		box2.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()
	wins["notebook"].show()


def create_panes(_button):
	if not wins.has_key("panes"):
		win = GtkWindow()
		wins["panes"] = win
		win.connect("delete_event", delete_event)
		win.set_title("Panes")

		vpaned = GtkVPaned()
		win.add(vpaned)
		vpaned.set_border_width(5)
		vpaned.show()

		hpaned = GtkHPaned()
		vpaned.add1(hpaned)

		frame = GtkFrame()
		frame.set_shadow_type(SHADOW_IN)
		frame.set_usize(60, 60)
		hpaned.add1(frame)
		frame.show()

		frame = GtkFrame()
		frame.set_shadow_type(SHADOW_IN)
		frame.set_usize(80, 60)
		hpaned.add2(frame)
		frame.show()

		hpaned.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		vpaned.add2(box2)
		box2.show()

		button = GtkButton("close")
		button.connect("clicked", win.hide)
		box2.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()
	wins["panes"].show()

def create_dnd(_button):
	if not wins.has_key("dnd"):
		targets = [
			('text/plain', 0, -1)
		]
		
		# note that these two functions don't use any global variables
		# to communicate.  In fact, try openning two copies of
		# testgtk.py (or the C version) and drag between them.
		def dnd_drag_data_get(w, context, selection_data, info, time):
			dnd_string = "Bill Gates demands royalties for\n" + \
				     "your use of his innovation."
			selection_data.set(selection_data.target, 8,dnd_string)
		def dnd_drag_data_received(w, context, x, y, data, info, time):
			if data and data.format == 8:
				msg = "Drop data of type %s was:\n\n%s" % \
				      (data.target, data.data)
				GtkExtra.message_box("Drop", msg,
					("Continue with life in\n" +
					"spite of this oppression",))
		win = GtkWindow(WINDOW_TOPLEVEL)
		wins["dnd"] = win
		win.connect("delete_event", delete_event)
		win.set_title("Drag -N- Drop")

		box1 = GtkVBox()
		win.add(box1)
		box1.show()

		box2 = GtkHBox(spacing=5)
		box2.set_border_width(10)
		box1.pack_start(box2)
		box2.show()

		frame = GtkFrame("Drag")
		box2.pack_start(frame)
		frame.show()

		box3 = GtkVBox(spacing=5)
		box3.set_border_width(5)
		frame.add(box3)
		box3.show()

		button = GtkButton("Drag me!")
		box3.pack_start(button)
		button.show()
		button.connect('drag_data_get', dnd_drag_data_get)
		button.drag_source_set(GDK.BUTTON1_MASK|GDK.BUTTON3_MASK,
				       targets, GDK.ACTION_COPY)

		frame = GtkFrame("Drop")
		box2.pack_start(frame)
		frame.show()

		box3 = GtkVBox(spacing=5)
		box3.set_border_width(5)
		frame.add(box3)
		box3.show()

		button = GtkButton("To")
		box3.pack_start(button)
		button.show()
		button.realize()
		button.connect('drag_data_received', dnd_drag_data_received)
		button.drag_dest_set(DEST_DEFAULT_ALL, targets,GDK.ACTION_COPY)

		separator = GtkHSeparator()
		box1.pack_start(separator, expand=FALSE)
		separator.show()

		box2 = GtkVBox(spacing=10)
		box2.set_border_width(10)
		box1.pack_start(box2, expand=FALSE)
		box2.show()

		button = GtkButton("close")
		button.connect("clicked", win.hide)
		box2.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()
	wins['dnd'].show()

def create_progress_bar(_button):
	if not wins.has_key("progress_bar"):
		win = GtkDialog()
		wins["progress_bar"] = win
		win.set_title("dialog")

		vbox = GtkVBox(spacing=5)
		vbox.set_border_width(10)
		win.vbox.pack_start(vbox)
		vbox.show()

		label = GtkLabel("progress...")
		label.set_alignment(0, 0.5)
		vbox.pack_start(label, expand=FALSE)
		label.show()

		pbar = GtkProgressBar()
		pbar.set_usize(200, 20)
		vbox.pack_start(pbar)
		pbar.show()

		def timeout(pbar=pbar, pcnt=[0]):
			pcnt[0] = (pcnt[0] + 0.02) % 1.0
			pbar.update(pcnt[0])
			return TRUE

		win.func = [timeout, 0]

		def killit(arg1=None, arg2=None, tid=win.func, win=win):
			timeout_remove(tid[1])
			win.hide()
			return TRUE
		win.connect("destroy", killit)
		win.connect("delete_event", killit)

		button = GtkButton("close")
		button.connect("clicked", killit)
		win.action_area.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()
	win = wins["progress_bar"]
	win.show()
	win.func[1] = timeout_add(100, win.func[0])

def create_gamma_curve(_button):
	if not wins.has_key("gamma_curve"):
		win = GtkWindow()
		wins["gamma_curve"] = win
		win.connect("delete_event", delete_event)
		win.set_title("test")
		win.set_usize(200, 150)

		curve = GtkGammaCurve()
		win.add(curve)
		curve.show()
	wins["gamma_curve"].show()

def create_timeout_test(_button):
	if not wins.has_key("timeout_test"):
		win = GtkDialog()
		wins["timeout_test"] = win
		win.connect("delete_event", delete_event)
		win.set_title("Timeout Test")

		label = GtkLabel("count: 0")
		label.set_padding(10, 10)
		win.vbox.pack_start(label)
		label.show()

		func = [None, 0]
		def timeout_test(label=label, count=[0]):
			count[0] = count[0] + 1
			label.set_text("count: " + str(count[0]))
			return TRUE

		func = [timeout_test, 0]
		def start_timeout_test(_button, func=func):
			if not func[1]:
				func[1] = timeout_add(100, func[0])
		def stop_timeout_test(_button, func=func):
			if func[1]:
				timeout_remove(func[1])
				func[1] = 0

		button = GtkButton("close")
		button.connect("clicked", win.hide)
		win.action_area.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()

		button = GtkButton("start")
		button.connect("clicked", start_timeout_test)
		win.action_area.pack_start(button)
		button.show()

		button = GtkButton("stop")
		button.connect("clicked", stop_timeout_test)
		win.action_area.pack_start(button)
		button.show()
	wins["timeout_test"].show()

def create_idle_test(_button):
	if not wins.has_key("idle_test"):
		win = GtkDialog()
		wins["idle_test"] = win
		win.connect("delete_event", delete_event)
		win.set_title("Idle Test")

		label = GtkLabel("count: 0")
		label.set_padding(10, 10)
		win.vbox.pack_start(label)
		label.show()

		def idle_test(label=label, count=[0]):
			count[0] = count[0] + 1
			label.set_text("count: " + str(count[0]))
			return TRUE

		win.func = [idle_test, 0]
		def start_idle_test(_button, func=win.func):
			if not func[1]:
				func[1] = idle_add(func[0])
		def stop_idle_test(_button, func=win.func):
			if func[1]:
				idle_remove(func[1])
				func[1] = 0

		button = GtkButton("close")
		button.connect("clicked", win.hide)
		win.action_area.pack_start(button)
		button.set_flags(CAN_DEFAULT)
		button.grab_default()
		button.show()

		button = GtkButton("start")
		button.connect("clicked", start_idle_test)
		win.action_area.pack_start(button)
		button.show()

		button = GtkButton("stop")
		button.connect("clicked", stop_idle_test)
		win.action_area.pack_start(button)
		button.show()
	wins["idle_test"].show()

def create_cursor_test(_button):
	if not wins.has_key("cursor_test"):
		def expose_event(darea, event):
			drawable = darea.get_window()
			white_gc = darea.get_style().white_gc
			grey_gc = darea.get_style().bg_gc[STATE_NORMAL]
			black_gc = darea.get_style().black_gc
			max_width = drawable.width
			max_height = drawable.height
			draw_rectangle(drawable, white_gc, TRUE, 0, 0,
				       max_width, max_height / 2)
			draw_rectangle(drawable, black_gc, TRUE, 0,
				       max_height/2, max_width, max_height / 2)
			draw_rectangle(drawable, grey_gc, TRUE,
				       max_width / 3, max_height / 3,
				       max_width / 3, max_height / 3)

		def button_press(widget, event, spinner):
			if event.type == GDK.BUTTON_PRESS:
				if event.button == 1:
					spinner.spin(SPIN_STEP_FORWARD, 0)
				elif event.button == 3:
					spinner.spin(SPIN_STEP_BACKWARD, 0)
		def set_cursor(spinner, widget, cur_name):
			c = spinner.get_value_as_int()
			c = c & 0xfe
			cursor = cursor_new(c)
			widget.get_window().set_cursor(cursor)
			cur_name.set_text(cursor.name)
		
		win = GtkWindow()
		wins["cursor_test"] = win
		win.connect("delete_event", delete_event)
		win.set_title("Cursor Test")

		main_vbox = GtkVBox(FALSE, 5)
		main_vbox.set_border_width(0)
		win.add(main_vbox)
		main_vbox.show()

		vbox = GtkVBox(FALSE, 5)
		vbox.set_border_width(10)
		main_vbox.pack_start(vbox)
		vbox.show()

		hbox = GtkHBox(FALSE, 5)
		vbox.pack_start(hbox, expand=FALSE)
		hbox.show()

		label = GtkLabel('Cursor value: ')
		label.set_alignment(0, 0.5)
		hbox.pack_start(label, expand=FALSE)
		label.show()

		spinner = GtkSpinButton(GtkAdjustment(0,0,152,2,10,0), 0, 0)
		hbox.pack_start(spinner)
		spinner.show()

		frame = GtkFrame("Cursor Area")
		frame.set_border_width(10)
		frame.set_label_align(0.5, 0)
		vbox.pack_start(frame)
		frame.show()

		darea = GtkDrawingArea()
		darea.set_usize(80, 80)
		frame.add(darea)
		darea.show()

		cur_name = GtkLabel("")
		vbox.pack_start(cur_name, expand=FALSE)
		cur_name.show()
		
		darea.connect("expose_event", expose_event)
		darea.add_events(GDK.EXPOSURE_MASK | GDK.BUTTON_PRESS_MASK)
		darea.connect("button_press_event", button_press, spinner)
		spinner.connect("changed", set_cursor, darea, cur_name)

		hsep = GtkHSeparator()
		main_vbox.pack_start(hsep, expand=FALSE)
		hsep.show()

		hbox = GtkHBox(FALSE, 5)
		hbox.set_border_width(10)
		main_vbox.pack_start(hbox, expand=FALSE)
		hbox.show()

		button = GtkButton("Close")
		button.connect("clicked", win.hide)
		hbox.pack_start(button)
		button.show()
	wins["cursor_test"].show()

def do_exit(button):
	mainquit()

def create_main_window():
	buttons = {
		"buttons": create_buttons,
		"toggle buttons": create_toggle_buttons,
		"check buttons": create_check_buttons,
		"radio buttons": create_radio_buttons,
		"button box": create_button_box,
		"toolbar": create_toolbar,
		"handle box": create_handle_box,
		"reparent": create_reparent,
		"pixmap": create_pixmap,
		"tooltips": create_tooltips,
		"menus": create_menus,
		"scrolled windows": create_scrolled_windows,
		"drawing areas": None,
		"entry": create_entry,
		"list": create_list,
		"clist": create_clist,
		"color selection": create_color_selection,
		"file selection": create_file_selection,
		"dialog": create_dialog,
		"miscellaneous": None,
		"range controls": create_range_controls,
		"rulers": create_rulers,
		"text": create_text,
		"notebook": create_notebook,
		"panes": create_panes,
		"shapes": None, #create_shapes,
		"dnd": create_dnd,
		"progress bar": create_progress_bar,
		"preview color": None, #create_color_preview,
		"preview gray": None, #create_gray_preview,
		"gamma curve": create_gamma_curve,
		"test scrolling": None, #creaate_scroll_text,
		"test selection": None, #create_selection_test,
		"test timeout": create_timeout_test,
		"test idle": create_idle_test,
		"test": None, #create_test
		"status bar": create_statusbar_test,
		"cursor": create_cursor_test,
	}
	win = GtkWindow()
	win.set_name("main window")
	win.set_usize(200, 400)
	win.set_uposition(20, 20)
	win.connect("destroy", mainquit)
	win.connect("delete_event", mainquit)
	win.set_title("gtktest")
	box1 = GtkVBox(FALSE, 0)
	win.add(box1)
	box1.show()
	scrolled_window = GtkScrolledWindow()
	scrolled_window.set_border_width(10)
	scrolled_window.set_policy(POLICY_AUTOMATIC, POLICY_AUTOMATIC)
	box1.pack_start(scrolled_window)
	scrolled_window.show()
	box2 = GtkVBox()
	box2.set_border_width(10)
	scrolled_window.add_with_viewport(box2)
	box2.show()
	k = buttons.keys()
	k.sort()
	for i in k:
		button = GtkButton(i)
		if buttons[i]:
			button.connect("clicked", buttons[i])
		else:
			button.set_sensitive(FALSE)
		box2.pack_start(button)
		button.show()
	separator = GtkHSeparator()
	box1.pack_start(separator, expand=FALSE)
	separator.show()
	box2 = GtkVBox(spacing=10)
	box2.set_border_width(10)
	box1.pack_start(box2, expand=FALSE)
	box2.show()
	button = GtkButton("close")
	button.connect("clicked", do_exit)
	button.set_flags(CAN_DEFAULT)
	box2.pack_start(button)
	button.grab_default()
	button.show()
	win.show()

def main():
        rc_parse("testgtkrc")
	create_main_window()
	mainloop()

if __name__ == '__main__': main()
