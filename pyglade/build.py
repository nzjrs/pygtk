"""This module is responsible for creating widgets from a
pyglade.xmhtml.TagTree.  As opposed to the C code generation approach,
this will build the widget tree at runtime.  There is nothing wrong with
the code generation approach, but this one is what I have decided to do.
I may write a code generator at some later point (the tag tree and style
generators are sufficiently general).
"""

import sys
import string
from gtk import *
# the style sheet generator
import style

error = "pyglade.build.error"

# This is a dictionary of widget creation routines.
# It should have string keys representing widget classes, and a tuple as
# the value.  The first function is for creation of this widget type.
# It should take one argument (the TagTree node), and returns a widget
# with those attributes.  Note that it doesn't have to set the widget
# names or connect signal handlers (in fact it shouldn't do this).
#
# The second function is for adding a child to this container widget.  If
# this widget class is not a container, the second item in the tuple should
# be None.  If it is a container, the function should take as arguments the
# container, the child, and finally the 'child' TagTree node corresponding to
# the child widget (or None if there is no child TagTree node).

_widgets = {}

class WidgetTree:
	def __init__(self, tree):
		if tree.has_key('gtk-interface'): tree = tree['gtk-interface']
		if tree.tag != 'gtk-interface':
			raise error, "first argument not the base of tag tree"
		self.__signals = {}  # signals
		self.__widgets = {}  # widgets by name
		self.__paths = {}    # widgets by path
		self.tooltips = None

		# parse rc strings
		if tree.has_key('style'):
			rc_parse_string(style.as_string(tree))

		# create widgets
		if tree.has_key('widget'):
			children = tree.widget
			if type(children) != type(()): children = (children,)
			for child in children:
				self.__new_widget(child)
		if self.tooltips:
			self.tooltips.enable();

	def get_widget(self, name):
		if string.find(name, '.') == -1:  # no dot
			return self.__widgets[name]
		else:
			return self.__paths[name]

	def connect(self, signame, handler, *data):
		self.__signals[signame] = (self.__signals[signame][0], data,
					   handler)
	def disconnect(self, signame):
		self.__signals[signame] = (self.__signals[signame][0], (),None)

	def __signal_handler(self, wid, *args):
		signame = args[-1]
		args = args[:-1]
		objname, data, handler = self.__signals[signame]
		if not handler: return
		obj = self.__widgets[objname]
		return apply(handler, (obj,) + args + data)
		
	def __new_widget(self, node, parent=''):
		path = parent + '.' + node.name
		wclass = node['class']
		if wclass == 'Placeholder':
			sys.stderr.write(
				"warning: placeholders still in description\n")
			return
		create, add = _widgets[wclass]
		widget = create(node)
		if node.has_key('name'): widget.set_name(node.name)
		self.__widgets[node.name] = widget
		self.__paths[path[:-1]] = widget
		self.__set_common_params(widget, node)

		if node.has_key('signal'):
			self.__setup_sighandlers(widget, node)
		if node.has_key('widget'):
			if not add:
				raise error, "don't know how to add " + \
				      "widgets to this container"
			children = node.widget
			if type(children) != type(()): children = (children,)
			for child in children:
				self.__new_widget(child, parent=path)
				cwidget = self.get_widget(child.name)
				add(widget, cwidget, child)

				# these can only be set after packing
				has_default = child.get_bool('has_default',
							     FALSE)
				if has_default: cwidget.grab_default()
				has_focus = child.get_bool('has_focus', FALSE)
				if has_focus: cwidget.grab_focus()

		if node.get_bool('visible', TRUE):
			widget.show()

	def __set_common_params(self, widget, node):
		width = node.get_int('width', -1)
		height = node.get_int('height', -1)
		if width != -1 or height != -1:
			widget.set_usize(width, height)
		sensitive = node.get_bool('sensitive', TRUE)
		if not sensitive:
			widget.set_sensitive(FALSE)
		tooltip = node.get('tooltip', None)
		if tooltip:
			if not self.tooltips: self.tooltips = GtkTooltips()
			self.tooltips.set_tip(widget, tooltip, '')
		can_default = node.get_bool('can_default', FALSE)
		if can_default: widget.set_flags(CAN_DEFAULT)
		can_focus = node.get_bool('can_focus', FALSE)
		if can_focus: widget.set_flags(CAN_FOCUS)
		events = node.get_int('events', 0)
		if events: widget.set_events(events)
		extension = node.get('extension_events', None)
		if extension: widget.set_extension_events(extension)
		border_width = node.get_int('border_width', 0)
		if border_width: widget.set_border_width(border_width)

	def __setup_sighandlers(self, widget, node):
		signals = node.signal
		if type(signals) != type(()): signals = (signals,)
		for sig in signals:
			signame = sig.name
			self.__signals[sig.handler] = (
					sig.get('object', node.name), (), None)
			if sig.get_bool('after', FALSE):
				widget.connect_after(signame,
						     self.__signal_handler,
						     sig.handler)
			else:
				widget.connect(signame, self.__signal_handler,
					       sig.handler)


# These functions handle adding elements to the various container types
def container_add(cont, child, info):
	cont.add(child)
def box_add(box, child, info):
	info = info.child
	expand = info.get_bool('expand', TRUE)
	fill = info.get_bool('fill', TRUE)
	padding = info.get_int('padding', 0)
	if info.get('pack', 'GTK_PACK_START') == 'GTK_PACK_START':
		box.pack_start(child, expand=expand, fill=fill,padding=padding)
	else:
		box.pack_end(child, expand=expand, fill=fill,padding=padding)
def table_add(table, child, info):
	info = info.child
	la = info.get_int('left_attach', 0)
	ra = info.get_int('right_attach', 1)
	ta = info.get_int('top_attach', 0)
	ba = info.get_int('bottom_attach', 1)
	xpad = info.get_int('xpad', 0)
	ypad = info.get_int('ypad', 0)
	xoptions=0
	yoptions=0
	if info.get_bool('xexpand', TRUE):  xoptions = xoptions | EXPAND
	if info.get_bool('xshrink', FALSE): xoptions = xoptions | SHRINK
	if info.get_bool('xfill', TRUE):    xoptions = xoptions | FILL
	if info.get_bool('yexpand', TRUE):  yoptions = yoptions | EXPAND
	if info.get_bool('yshrink', FALSE): yoptions = yoptions | SHRINK
	if info.get_bool('yfill', TRUE):    yoptions = yoptions | FILL
	table.attach(child, la,ra, ta,ba, xoptions=xoptions, yoptions=yoptions,
		     xpadding=xpad, ypadding=ypad)
def fixed_add(fix, child, info):
	x = info.get_int('x', 0)
	y = info.get_int('y', 0)
	fix.put(child, x, y)
def clist_add(clist, child, info):
	col = info.parent.get('col_no', 0)
	info.parent['col_no'] = col + 1
	clist.set_column_widget(col, child)
def paned_add(paned, child, info):
	pane = info.parent.get('pane2', FALSE)
	info.parent['pane2'] = TRUE
	if not pane:
		paned.add1(child)
	else:
		paned.add2(child)
def notebook_add(book, child, info):
	pages = info.parent.pages
	if not info.has_key('child_name') or info.child_name != 'Notebook:tab':
		pages.append(child)
	else:
		# child is a label
		book.append_page(pages[0], child)
		del pages[0]
def dialog_add(dlg, child, info):
	# the widgets are already added
	pass

def menuitem_add(mi, menu, info):
	mi.set_submenu(menu)
def menushell_add(menu, mi, info):
	menu.append(mi)

def misc_set(misc, info):
	xalign = info.get_float('xalign', 0.5)
	yalign = info.get_float('yalign', 0.5)
	misc.set_alignment(xalign, yalign)
	xpad = info.get_int('xpad', 0)
	ypad = info.get_int('ypad', 0)
	misc.set_padding(xpad, ypad)


def label_new(node):
	str = node.get('label', '')
	label = GtkLabel(str)
	misc_set(label, node)
	just = node.get('justify', JUSTIFY_CENTER)
	label.set_justify(just)
	return label
def entry_new(node):
	ent = GtkEntry(maxlen=node.get_int('text_max_length', -1))
	if not node.get_bool('editable', TRUE):
		ent.set_editable(FALSE)
	if not node.get_bool('text_visible', TRUE):
		ent.set_visibility(FALSE)
	text = node.get('text', '')
	if text: ent.set_text(text)
	return ent
def text_new(node):
	text = GtkText()
	text.set_editable(node.get_bool('editable', FALSE))
	t = node.get('text', '')
	if t:
		text.insert_text(t)
	return text
def button_new(node):
	label = node.get('label', '')
	return GtkButton(label)
def togglebutton_new(node):
	label = node.get('label', '')
	tog = GtkToggleButton(label)
	if node.get_bool('active', FALSE):
		tog.set_state(TRUE)
	return tog
def checkbutton_new(node):
	label = node.get('label', '')
	cb = GtkCheckButton(label)
	if node.get_bool('active', FALSE):
		cb.set_state(TRUE)
	if not node.get_bool('draw_indicator', TRUE):
		cb.set_mode(FALSE)
	return cb
def radiobutton_new(node):
	label = node.get('label', '')
	# do something about radio button groups ...
	rb = GtkRadioButton(label)
	if node.get_bool('active', FALSE):
		rb.set_state(TRUE)
	if not node.get_bool('draw_indicator', TRUE):
		rb.set_mode(FALSE)
	return rb
def optionmenu_new(node):
	omenu = GtkOptionMenu()
	menu = GtkMenu()
	for item in string.split(node.get('items', ''), '\n'):
		mi = GtkMenuItem(item)
		menu.append(mi)
		mi.show()
	omenu.set_menu(menu)
	return omenu
def combo_new(node):
	combo = GtkCombo()
	if node.get_bool('case_sensitive', FALSE):
		combo.set_case_sensitive(TRUE)
	if not node.get_bool('use_arrows', TRUE):
		combo.set_use_arrows(FALSE)
	if node.get_bool('use_arrows_always', FALSE):
		combo.set_use_arrows_always(TRUE)
	return combo
def list_new(node):
	list = GtkList()
	mode = node.get('selection_mode', SELECTION_SINGLE)
	list.set_selection_mode(mode)
	return list
def clist_new(node):
	numcols = node.get_int('columns', 1)
	clist = GtkCList(numcols)
	widths = node.get('column_widths', None)
	if widths:
		widths = map(string.atoi, string.split(widths, ','))
		for i in range(numcols):
			clist.set_column_width(i, widths[i])
	if node.get_bool('show_titles', TRUE):
		clist.column_titles_show()
	mode = node.get('selection_mode', SELECTION_SINGLE)
	clist.set_selection_mode(mode)
	#shadow = node.get('shadow_type', SHADOW_IN)
	#clist.set_border(shadow)
	#hpol = node.get('hscrollbar_policy', POLICY_ALWAYS)
	#vpol = node.get('vscrollbar_policy', POLICY_ALWAYS)
	#clist.set_policy(hpol, vpol)
	return clist
def tree_new(node):
	tree = GtkTree()
	mode = node.get('selection_mode', SELECTION_SINGLE)
	tree.set_selection_mode(mode)
	mode = node.get('view_mode', TREE_VIEW_LINE)
	tree.set_view_mode(mode)
	if not node.get_bool('view_line', TRUE):
		tree.set_view_lines(FALSE)
	return tree
def spinbutton_new(node):
	climb_rate = node.get_int('climb_rate', 1)
	digits = node.get_int('digits', 0)
	hvalue = node.get_float('hvalue', 1)
	hlower = node.get_float('hlower', 0)
	hupper = node.get_float('hupper', 100)
	hstep  = node.get_float('hstep', 1)
	hpage  = node.get_float('hpage', 10)
	hpage_size = node.get_float('hpage_size', 10)
	adj = GtkAdjustment(hvalue, hlower, hupper, hstep, hpage, hpage_size)
	spin = GtkSpinButton(adj=adj, climb_rate=climb_rate, digits = digits)
	spin.set_numeric(node.get_bool('numeric', FALSE))
	pol = node.get('update_policy', UPDATE_IF_VALID)
	spin.set_update_policy(pol)
	spin.set_snap_to_ticks(node.get_bool('snap', FALSE))
	spin.set_wrap(node.get_bool('wrap', FALSE))
	return spin
def hscale_new(node):
	hvalue = node.get_float('hvalue', 1)
	hlower = node.get_float('hlower', 0)
	hupper = node.get_float('hupper', 100)
	hstep  = node.get_float('hstep', 1)
	hpage  = node.get_float('hpage', 10)
	hpage_size = node.get_float('hpage_size', 10)
	adj = GtkAdjustment(hvalue, hlower, hupper, hstep, hpage, hpage_size)
	scale = GtkHScale(adj)
	scale.set_draw_value(node.get_bool('draw_value', TRUE))
	scale.set_value_pos(node.get('value_pos', POS_TOP))
	scale.set_digits(node.get_int('digits', 1))
	scale.set_update_policy(node.get('policy', UPDATE_CONTINUOUS))
	return scale
def vscale_new(node):
	hvalue = node.get_float('hvalue', 1)
	hlower = node.get_float('hlower', 0)
	hupper = node.get_float('hupper', 100)
	hstep  = node.get_float('hstep', 1)
	hpage  = node.get_float('hpage', 10)
	hpage_size = node.get_float('hpage_size', 10)
	adj = GtkAdjustment(hvalue, hlower, hupper, hstep, hpage, hpage_size)
	scale = GtkVScale(adj)
	scale.set_draw_value(node.get_bool('draw_value', TRUE))
	scale.set_value_pos(node.get('value_pos', POS_TOP))
	scale.set_digits(node.get_int('digits', 1))
	scale.set_update_policy(node.get('policy', UPDATE_CONTINUOUS))
	return scale
def hruler_new(node):
	widget = GtkHRuler()
	widget.set_metric(node.get('metric', PIXELS))
	lower = node.get_int('lower', 0)
	upper = node.get_int('upper', 10)
	pos = node.get_int('position', 0)
	max = node.get_int('max_size', 10)
	widget.set_range(lower, upper, pos, max)
	return widget
def vruler_new(node):
	widget = GtkVRuler()
	widget.set_metric(node.get('metric', PIXELS))
	lower = node.get_int('lower', 0)
	upper = node.get_int('upper', 10)
	pos = node.get_int('position', 0)
	max = node.get_int('max_size', 10)
	widget.set_range(lower, upper, pos, max)
	return widget
def hscrollbar_new(node):
	hvalue = node.get_float('hvalue', 1)
	hlower = node.get_float('hlower', 0)
	hupper = node.get_float('hupper', 100)
	hstep  = node.get_float('hstep', 1)
	hpage  = node.get_float('hpage', 10)
	hpage_size = node.get_float('hpage_size', 10)
	adj = GtkAdjustment(hvalue, hlower, hupper, hstep, hpage, hpage_size)
	scroll = GtkHScrollbar(adj)
	scroll.set_update_policy(node.get('policy', UPDATE_CONTINUOUS))
	return scroll
def vscrollbar_new(node):
	hvalue = node.get_float('hvalue', 1)
	hlower = node.get_float('hlower', 0)
	hupper = node.get_float('hupper', 100)
	hstep  = node.get_float('hstep', 1)
	hpage  = node.get_float('hpage', 10)
	hpage_size = node.get_float('hpage_size', 10)
	adj = GtkAdjustment(hvalue, hlower, hupper, hstep, hpage, hpage_size)
	scroll = GtkVScrollbar(adj)
	scroll.set_update_policy(node.get('policy', UPDATE_CONTINUOUS))
	return scroll
def statusbar_new(node):
	return GtkStatusbar()
def toolbar_new(node):
	orient = node.get('orientation', ORIENTATION_HORIZONTAL)
	style = node.get('type', TOOLBAR_ICONS)
	tool = GtkToolbar(orient, style)
	tool.set_space_size(node.get_int('space_size', 5))
	tool.set_tooltips(node.get_bool('tooltips', TRUE))
	return tool
def progressbar_new(node):
	return GtkProgressBar()
def arrow_new(node):
	dir = node.get('arrow_type', ARROW_RIGHT)
	shadow = node.get('shadow_type', SHADOW_OUT)
	arr = GtkArrow(dir, shadow)
	misc_set(arr, node)
	return arr
# image not supported
def pixmap_new(node):
	# the first parameter needs to be the toplevel widget
	pix,bit = create_pixmap_from_xpm(None, None, node.get('filename', ''))
	pixmap = GtkPixmap(pix, bit)
	misc_set(pixmap)
	return pixmap
def drawingarea_new(node):
	return GtkDrawingArea()
def hseparator_new(node):
	return GtkHSeparator()
def vseparator_new(node):
	return GtkVSeparator()

def menubar_new(node):
	return GtkMenuBar()
def menu_new(node):
	return GtkMenu()
def menuitem_new(node):
	if node.has_key('label'):
		ret = GtkMenuItem(node.label)
	else:
		ret = GtkMenuItem()
	if node.get_bool('right_justify', FALSE):
		ret.right_justify()
	return ret
def checkmenuitem_new(node):
	ret = GtkCheckMenuItem(node.label)
	if node.get_bool('right_justify', FALSE):
		ret.right_justify()
	if node.get_bool('active', FALSE):
		ret.set_state(TRUE)
	if node.get_bool('always_show_toggle', FALSE):
		ret.set_show_toggle(TRUE)
	return ret
def radiomenuitem_new(node):
	ret = GtkRadioMenuItem(node.label)
	if node.get_bool('right_justify', FALSE):
		ret.right_justify()
	if node.get_bool('active', FALSE):
		ret.set_state(TRUE)
	if node.get_bool('always_show_toggle', FALSE):
		ret.set_show_toggle(TRUE)
	return ret

def hbox_new(node):
	if node.has_key('child_name') and node.child_name[:7] == 'Dialog:':
		return node.__wid
	homogeneous = node.get_bool('homogeneous', FALSE)
	spacing = node.get_int('spacing', 0)
	return GtkHBox(homogeneous=homogeneous, spacing=spacing)
def vbox_new(node):
	if node.has_key('child_name') and node.child_name[:7] == 'Dialog:':
		return node.__wid
	homogeneous = node.get_bool('homogeneous', FALSE)
	spacing = node.get_int('spacing', 0)
	return GtkVBox(homogeneous=homogeneous, spacing=spacing)
def table_new(node):
	rows = node.get_int('rows', 1)
	cols = node.get_int('columns', 1)
	homog = node.get_bool('homogeneous', FALSE)
	table = GtkTable(rows, cols, homog)
	table.set_row_spacings(node.get_int('row_spacing', 0))
	table.set_col_spacings(node.get_int('col_spacing', 0))
	return table
def fixed_new(node):
	return GtkFixed()
def hbuttonbox_new(node):
	bbox = GtkHButtonBox()
	layout = node.get('layout_style', BUTTONBOX_DEFAULT_STYLE)
	bbox.set_layout(layout)
	spacing = node.get_int('child_min_width', None)
	if spacing: bbox.set_spacing(spacing)
	width, height = bbox.get_child_size_default()
	width = node.get_int('child_min_width', width)
	height = node.get_int('child_min_height', height)
	bbox.set_child_size(width, height)
	ipx, ipy = bbox.get_child_ipadding_default()
	ipx = node.get_int('child_ipad_x', ipx)
	ipy = node.get_int('child_ipad_y', ipy)
	bbox.set_child_ipadding(ipx, ipy)
	return bbox
def vbuttonbox_new(node):
	bbox = GtkVButtonBox()
	layout = node.get('layout_style', BUTTONBOX_DEFAULT_STYLE)
	bbox.set_layout(layout)
	spacing = node.get_int('child_min_width', None)
	if spacing: bbox.set_spacing(spacing)
	width, height = bbox.get_child_size_default()
	width = node.get_int('child_min_width', width)
	height = node.get_int('child_min_height', height)
	bbox.set_child_size(width, height)
	ipx, ipy = bbox.get_child_ipadding_default()
	ipx = node.get_int('child_ipad_x', ipx)
	ipy = node.get_int('child_ipad_y', ipy)
	bbox.set_child_ipadding(ipx, ipy)
	return bbox
def frame_new(node):
	label = node.get('label', '')
	frame = GtkFrame(label)
	xalign = node.get_int('label_xalign', 0)
	frame.set_label_align(xalign, 0.5)
	shadow = node.get('shadow_type', SHADOW_ETCHED_IN)
	frame.set_shadow_type(shadow)
	return frame
def aspectframe_new(node):
	xalign = node.get_int('xalign', 0)
	yalign = node.get_int('yalign', 0)
	ratio = node.get_float('ratio', 1)
	obey = node.get_bool('obey_child', TRUE)
	frame = GtkAspectFrame(xalign, yalign, ratio, obey_child)
	label = node.get('label', '')
	if label: frame.set_label(label)
	xalign = node.get_int('label_xalign', 0)
	frame.set_label_align(xalign, 0.5)
	shadow = node.get('shadow_type', SHADOW_ETCHED_IN)
	frame.set_shadow_type(shadow)
	return frame
def hpaned_new(node):
	paned = GtkHPaned()
	handle_size = node.get_int('handle_size', 0)
	if handle_size: paned.set_handle_size(handle_size)
	gutter_size = node.get_int('gutter_size', 0)
	if gutter_size: paned.set_gutter_size(gutter_size)
	return paned
def vpaned_new(node):
	paned = GtkVPaned()
	handle_size = node.get_int('handle_size', 0)
	if handle_size: paned.set_handle_size(handle_size)
	gutter_size = node.get_int('gutter_size', 0)
	if gutter_size: paned.set_gutter_size(gutter_size)
	return paned
def handlebox_new(node):
	return GtkHandleBox()
def notebook_new(node):
	book = GtkNotebook()
	book.set_show_tabs(node.get_bool('show_tabs', TRUE))
	book.set_show_border(node.get_bool('show_border', TRUE))
	book.set_tab_pos(node.get('tab_pos', POS_TOP))
	book.set_scrollable(node.get_bool('scrollable', FALSE))
	book.set_tab_border(node.get_int('tab_border', 3))
	book.popup_enable(node.get_bool('popup_enable', FALSE))
	node['pages'] = []
	return book
def alignment_new(node):
	xalign = node.get_float('xalign', 0.5)
	yalign = node.get_float('yalign', 0.5)
	xscale = node.get_float('xscale', 0)
	yscale = node.get_float('yscale', 0)
	return GtkAlignment(xalign, yalign, xscale, yscale)
def eventbox_new(node):
	return GtkEventBox()
def scrolledwindow_new(node):
	win = GtkScrolledWindow()
	hpol = node.get('hscrollbar_policy', POLICY_ALWAYS)
	vpol = node.get('vscrollbar_policy', POLICY_ALWAYS)
	win.set_policy(hpol, vpol)
	# do something about update policies here ...
	return win
def viewport_new(node):
	port = GtkViewport()
	shadow = node.get('shadow_type', SHADOW_IN)
	port.set_shadow_type(shadow)
	return port

def curve_new(node):
	curve = GtkCurve()
	curve.set_curve_type(node.get('curve_type', CURVE_TYPE_SPLINE))
	minx = node.get_float('min_x', 0)
	maxx = node.get_float('max_x', 1)
	miny = node.get_float('min_y', 0)
	maxy = node.get_float('max_y', 1)
	curve.set_range(minx, maxx, miny, maxy)
	return curve
def gammacurve_new(node):
	gamma = GtkGammaCurve()
	# do something about the curve parameters ...
	return gamma
def colorselection_new(node):
	cs = GtkColorSelection()
	cs.set_update_policy(node.get('policy', UPDATE_CONTINUOUS))
	return cs
def preview_new(node):
	type = node.get('type', PREVIEW_COLOR)
	prev = GtkPreview(type)
	prev.set_expand(node.get_bool('expand', TRUE))
	return prev

def window_new(node):
	wintype = node.get('type', 'toplevel')
	widget = GtkWindow(wintype)
	widget.set_title(node.get('title', node.name))
	x = node.get_int('x', -1)
	y = node.get_int('y', -1)
	if x != -1 or y != -1:
		widget.set_uposition(x, y)
	pos = node.get('position', None)
	if pos: widget.set_position(pos)
	ashrink = node.get_bool('allow_shrink', TRUE)
	agrow = node.get_bool('allow_grow', TRUE)
	autoshrink = node.get_bool('auto_shrink', FALSE)
	if not ashrink or not agrow or autoshrink:
		widget.set_policy(ashrink, agrow, autoshrink)
	return widget
def dialog_new(node):
	widget = GtkDialog()
	widget.set_title(node.get('title', node.name))
	x = node.get_int('x', -1)
	y = node.get_int('y', -1)
	if x != -1 or y != -1:
		widget.set_uposition(x, y)
	pos = node.get('position', None)
	if pos: widget.set_position(pos)
	ashrink = node.get_bool('allow_shrink', TRUE)
	agrow = node.get_bool('allow_grow', TRUE)
	autoshrink = node.get_bool('auto_shrink', FALSE)
	if not ashrink or not agrow or autoshrink:
		widget.set_policy(ashrink, agrow, autoshrink)
	# do some weird stuff because the vbox/action area is already created
	node.widget['__wid'] = widget.vbox
	children = node.widget.widget
	if type(children) != type(()): children = (children,)
	for i in range(len(children)):
		if children[i].has_key('child_name') and \
		   children[i].child_name == 'Dialog:action_area':
			node['widget'] = (node.widget, children[i])
			children[i].parent = node
			children[i]['__wid'] = widget.action_area
			node.widget[0]['widget'] = children[0:i]+children[i+1:]
			break
	return widget
def colorselectiondialog_new(node):
	return GtkColorSelectionDialog()


_widgets = {
	# widgets ...
	'GtkLabel':        (label_new,        None),
	'GtkEntry':        (entry_new,        None),
	'GtkText':         (text_new,         None),
	'GtkButton':       (button_new,       None),
	'GtkToggleButton': (togglebutton_new, None),
	'GtkCheckButton':  (checkbutton_new,  None),
	'GtkRadioButton':  (radiobutton_new,  None),
	'GtkOptionMenu':   (optionmenu_new,   None),
	'GtkCombo':        (combo_new,        None),
	'GtkList':         (list_new,         None),
	'GtkCList':        (clist_new,        clist_add),
	'GtkTree':         (tree_new,         None),
	'GtkSpinButton':   (spinbutton_new,   None),
	'GtkHScale':       (hscale_new,       None),
	'GtkVScale':       (vscale_new,       None),
	'GtkHRuler':       (hruler_new,       None),
	'GtkVRuler':       (vruler_new,       None),
	'GtkHScrollbar':   (hscrollbar_new,   None),
	'GtkVScrollbar':   (vscrollbar_new,   None),
	'GtkStatusbar':    (statusbar_new,    None),
	'GtkToolbar':      (toolbar_new,      None),
	'GtkProgressBar':  (progressbar_new,  None),
	'GtkArrow':        (arrow_new,        None),
	'GtkPixmap':       (pixmap_new,       None),
	'GtkDrawingArea':  (drawingarea_new,  None),
	'GtkHSeparator':   (hseparator_new,   None),
	'GtkVSeparator':   (vseparator_new,   None),

	# Menu stuff ...
	'GtkMenuBar':      (menubar_new,      menushell_add),
	'GtkMenu':         (menu_new,         menushell_add),
	'GtkMenuItem':     (menuitem_new,     menuitem_add),
	'GtkCheckMenuItem':(checkmenuitem_new,menuitem_add),
	'GtkRadioMenuItem':(radiomenuitem_new,menuitem_add),

	# Containers ...
	'GtkHBox':         (hbox_new,         box_add),
	'GtkVBox':         (vbox_new,         box_add),
	'GtkTable':        (table_new,        table_add),
	'GtkFixed':        (fixed_new,        fixed_add),
	'GtkHButtonBox':   (hbuttonbox_new,   container_add),
	'GtkVButtonBox':   (vbuttonbox_new,   container_add),
	'GtkFrame':        (frame_new,        container_add),
	'GtkAspectFrame':  (aspectframe_new,  container_add),
	'GtkHPaned':       (hpaned_new,       paned_add),
	'GtkVPaned':       (vpaned_new,       paned_add),
	'GtkHandleBox':    (handlebox_new,    container_add),
	'GtkNotebook':     (notebook_new,     notebook_add),
	'GtkAlignment':    (alignment_new,    container_add),
	'GtkEventBox':     (eventbox_new,     container_add),
	'GtkScrolledWindow': (scrolledwindow_new, container_add),
	'GtkViewport':     (viewport_new,     container_add),

	# Miscellaneous ...
	'GtkCurve':        (curve_new,        None),
	'GtkGammaCurve':   (gammacurve_new,   None),
	'GtkColorSelection': (colorselection_new, None),
	'GtkPreview':      (preview_new,      None),

	# Windows ...
	'GtkWindow':       (window_new,       container_add),
	'GtkDialog':       (dialog_new,       dialog_add),
	'GtkColorSelectionDialog': (colorselectiondialog_new, None),
}



