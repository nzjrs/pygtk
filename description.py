"""This file describes the types defined in gtkmodule that don't have Python
class wrappers in gtk.py, but are still used.  This module shouldn't be
imported, as it is only meant to be documentation

In this file, when I assign '_' to a variable, it represents whatever should
be in that variable."""

raise ImportError, "I told you not to import this module"

_ = None

class GtkCTreeNode:
	"""This representsa node in a GtkCTree.  It is a cross between the"""
	"""C GtkCTreeNode and GtkCTreeRow structs."""

	"""The parent of this node (or None for no parent)"""
	parent = _
	"""This node's next sibling (or None if this is the last)"""
	sibling = _
	"""A list of this node's children"""
	children = []
	"""The level of this node"""
	level = _
	"""TRUE if this node is a leaf node"""
	is_leaf = _
	"""TRUE if this node has been expanded"""
	expanded = _

class GtkStyle:
	"""You can assign to any attribute except colormap.  Also, assigning"""
	"""to the sequence attributes is not allowed -- only assinging to"""
	"""sequence members."""

	"""COLORS"""
	black = _
	white = _
	"""These variables should be indexed with the STATE_* constants"""
	fg = (_, _, _, _, _)
	bg = (_, _, _, _, _)
	light = (_, _, _, _, _)
	dark = (_, _, _, _, _)
	mid = (_, _, _, _, _)
	text = (_, _, _, _, _)
	base = (_, _, _, _, _)

	"""The font used for text for widgets of this style"""
	font = _

	"""GC's"""
	black_gc = _
	white_gc = _
	"""These variables should be indexed with the STATE_* constants"""
	fg_gc = (_, _, _, _, _)
	bg_gc = (_, _, _, _, _)
	light_gc = (_, _, _, _, _)
	dark_gc = (_, _, _, _, _)
	mid_gc = (_, _, _, _, _)
	text_gc = (_, _, _, _, _)
	base_gc = (_, _, _, _, _)

	"""These are background pixmaps for the various states"""
	bg_pixmap = (_, _, _, _, _)

	"""The colormap for this style"""
	colormap = _

	def copy(self):
		"""take a copy of this style"""
		pass

class GtkSelectionData:
	"""This type represents the selection.  Do not keep a reference"""
	"""to it past the life of a callback"""
	"""The selection atom"""
	selection = _
	"""target atom"""
	target = _
	"""type atom"""
	type = _
	"""format type"""
	format = _
	"""The data in this selection"""
	data = "some_data"
	def set(self, type, format, data):
		"""Set the type, format and data parts of the selection data"""
		pass

class GdkAtom:
	"""This type represents a GdkAtom.  The reason for not using ints"""
	"""is that this type can automatically coerce to int, long, float"""
	"""or string, reducing the need for gdk_atom_intern/name in a lot"""
	"""of cases.  Functions that take atoms as input are really reading"""
	"""ints, and this type just gets coerced."""
	def __int__(self): pass
	def __long__(self): pass
	def __float__(self): pass
	def __oct__(self): pass
	def __hex__(self): pass
	def __coerce__(self, other):
		"""will corece self to any of int, long, float or string if"""
		"""other type matches, otherwise fail"""
		pass

class GdkColor:
	"""These are the attributes of a GdkColor:"""
	red   = _
	green = _
	blue  = _

	"""This is the pixel value for the color"""
	pixel = _

class GdkColormap:
	def __len__(self):
		"""return the size of the colormap"""
		pass
	def __getitem__(self, pos):
		"""return the GdkColor at position pos in the colormap"""
		pass
	def __getslice__(self, lo, up):
		"""return a tuple of GdkColor's according to the given slice"""
		pass
	def alloc(self, color_name):
		"""return a GdkColor matching the color name given, that has
		been allocated in the colormap"""
		pass
	def alloc(self, red, green, blue):
		"""return a GdkColor matching the 16-bit RGB values given,
		that has been allocated in the colormap"""
		pass

class GdkCursor:
	"""the type number of this cursor, or -1 for a pixmap cursor"""
	type = _
	"""the string name for this type of cursor"""
	name = _

class GdkDragContext:
	"""This event holds the context of a DND drag"""

	"""The protocol"""
	protocol = _
	"""True if we are on the source end (I think)"""
	is_source = _
	"""source and destination windows"""
	source_window = _
	dest_window = _
	"""A list of target atoms for this drag"""
	targets = [_, _, _]
	"""A combination of the GdkDragAction flags"""
	actions = _
	"""The suggested action atom"""
	suggested_action = _
	"""The action atom"""
	action = _
	"""The start time for the drag"""
	start_time = _

class GdkEvent:
	"""The type of the event -- this decides on what other attributes the
	object has"""
	type = _
	"""The GdkWindow associated with the event"""
	window = _
	send_event = _

	"""You can convert the event names to widget signal names by knocking
	off the GDK., converting to lower case, and adding '_event'."""
	if type in (GDK.NOTHING, GDK.DELETE, GDK.DESTROY, GDK.EXPOSE):
		"""No extra info for these events"""
		pass
	elif type == GDK.EXPOSE:
		"""The exposed area"""
		area = (_, _, _, _)
		count = _
	elif type == GDK.MOTION_NOTIFY:
		time = _ ; x = _ ; y = _ ; pressure = _ ; xtilt = _ ; ytilt = _
		state = _ ; is_hint = _ ; source = _ ; x_root = _ ; y_root = _
	elif type in (GDK.BUTTON_PRESS,GDK._2BUTTON_PRESS,GDK._3BUTTON_PRESS):
		time = _ ; x = _ ; y = _ ; pressure = _ ; xtilt = _ ; ytilt = _
		state = _ ; button = _ ; source = _ ; deviceid = _
		x_root = _ ; y_root = _
	elif type in (GDK.KEY_PRESS, GDK.KEY_RELEASE):
		time = _ ; keyval = _ ; string = _
	elif type in (GDK.ENTER_NOTIFY, GDK.LEAVE_NOTIFY):
		detail = _
	elif type == GDK.FOCUS_CHANGE:
		_in = _
	elif type == GDK.CONFIGURE:
		x = _ ; y = _ ; width = _ ; height = _
	elif type in (GDK.MAP, GDK.UNMAP):
		"""No extra parameters"""
		pass
	elif type == GDK.PROPERTY_NOTIFY:
		atom = _ ; time = _ ; state = _
	elif type in (GDK.SELECTION_CLEAR, GDK.SELECTION_REQUEST,
		      GDK.SELECTION_NOTIFY):
		selection = _ ; target = _ ; property = _ ; requestor = _
		time = _
	elif type in (GDK.PROXIMITY_IN, GDK.PROXIMITY_OUT):
		time = _ ; source = _ ; deviceid = _
	elif type == GDK.DRAG_BEGIN:
		protocol_version = _
	elif type == GDK.DRAG_REQUEST:
		protocol_version = _ ; sendreply = _ ; willaccept = _
		delete_data = _ ; senddata = _ ; isdrop = _
		drop_coords = (_, _) ; data_type = _ ; timestamp = _
	elif type == GDK.DROP_ENTER:
		requestor = _ ; protocol_version = _ ; sendreply = _
		extended_typelist = _
	elif type == GDK.DROP_LEAVE:
		requestor = _ ; protocol_version = _
	elif type == GDK.DROP_DATA_AVAIL:
		requestor = _ ; protocol_version = _ ; isdrop = _
		data_type = _ ; data = _ ; timestamp = _ ; coords = (_, _)
	elif type == GDK.CLIENT_EVENT:
		message_type = _ ; data_format = _ ; data = _
	elif type == GDK.VISIBILITY_NOTIFY:
		state = _
	elif type == GDK.NO_EXPOSE: pass
	elif type == GDK.OTHER_EVENT: pass
	

class GdkFont:
	""" the ascent and descent of a font"""
	ascent = _
	descent = _
	""" the font type (GDK.FONT_FONT or GDK.FONT_FONTSET) """
	type = _
	def width(self, str):
		"""return the width of the string"""
		pass
	def height(self, str):
		"""return the height of the string"""
		pass
	def measure(self, str):
		"""return the width of the string, taking into account kerning
		of the final character"""
		pass
	def extents(self, str):
		"""return (lbearing,rbearing,width,ascent,descent)"""
		pass


class GdkGC:
	"""GdkGC's are created with the GdkWindow.new_gc method.  It
	is impossible to directly create one."""

	"""These attributes match the parameters to GdkWindow.new_gc, and can
	be read and set"""
	foreground = _
	background = _
	font = _
	function = _
	fill = _
	tile = _
	stipple = _
	clip_mask = _
	ts_x_origin, ts_y_origin = (_, _)
	clip_x_origin, clip_y_origin = (_, _)
	graphics_exposures = _
	line_width = _
	line_style = _
	cap_style = _
	join_style = _
	def set_dashes(self, dash_offset, dash_list):
		"""set the dash pattern for this GC."""
		pass

class GdkWindow:
	"""All attributes are read only for GdkWindow's"""
	"""The type of window -- you shouldn't need this"""
	type = _
	"""Child windows"""
	children = [_, _, '...']
	"""A GdkColormap object"""
	colormap = _
	width = _ ; height = _
	x = _     ; y = _
	"""bit depth for this window/pixmap"""
	depth = _
	"""parent windows"""
	parent = _
	toplevel = _
	"""Pointer info"""
	pointer = (_, _)
	pointer_state = _

	"""The X window ID of the window"""
	xid = _

	def new_gc(self, foreground=None, background=None, font=None,
		   tile=None, stipple=None, clip_mask=None,
		   function=None, fill=None, subwindow_mode=None,
		   ts_x_origin=None, ts_y_origin=None,
		   clip_x_origin=None, clip_y_origin=None,
		   line_width=None, line_style=None, cap_style=None,
		   join_style=None):
		"""Creates a new GC for this
		window with the given settings.  The settings must
		be given as keyword arguments.  If a setting isn't
		given, then its default will be used"""
		pass
	def set_cursor(self, cursor):
		"""Sets the cursor for this window"""
		pass

	"""The property argument can be an atom, int or string here"""
	def property_get(self, property, type=AnyPropertyType, delete=FALSE):
		"""Gets a property on the window.  On success, returns a
		tuple like (type, format, data) where type is an atom,
		format is 8,16 or 32 and data is the data (a string for
		format=8, a tuple of ints otherwise).  On failure, None is
		returned.  The property may optionally be deleted as well."""
		pass
	def property_change(self, property, type, format, mode, data):
		"""Change the value of a property."""
		pass
	def property_delete(self, property):
		"""Delete a property if it exists."""
		pass
	
"""Aliases for GdkWindow.  GdkPixmap refers to an offscreen pixmap.
GdkDrawable refers to either a GdkWindow or a GdkPixmap"""
GdkPixmap = GdkWindow
GdkBitmap = GdkPixmap
GdkDrawable = GdkWindow
	
	
	
