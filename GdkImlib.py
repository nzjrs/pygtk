#
# GdkImlib: An interface to gdk_imlib to go with gtk.py.  It wraps the
#           native module _gdkimlib.  It is sugested that you import it as
#           "import GdkImlib".
#
# Author: James Henstridge <james@daa.com.au>
#
# To use GdkImlib, create an instance of the GdkImlib.Image class.  The
# constructor takes a filename as its one argument.  Then call the render
# method (optionally with width and height arguments), and finally call
# the make_pixamp method to get a GTK pixmap widget with the image loaded.
#
# For extra information, read the rest of this file, and Imlib's documentation.
#

import _gdkimlib

from gtk import GtkPixmap

#some constants ...
RT_PLAIN_PALETTE       = 0
RT_PLAIN_PALETTE_FAST  = 1
RT_DITHER_PALETTE      = 2
RT_DITHER_PALETTE_FAST = 3
RT_PLAIN_TRUECOL       = 4
RT_DITHER_TRUECOL      = 5

_gdkimlib.gdk_imlib_init()

class Image:
	def __init__(self, filename="", _obj=None):
		if _obj: self._im = _obj; return
		self._im = _gdkimlib.gdk_imlib_load_image(filename)
	def __getattr__(self, attr):
		attrs = {
			'rgb_width':  _gdkimlib.gdk_imlib_get_rgb_width,
			'rgb_height': _gdkimlib.gdk_imlib_get_rgb_height,
			'filename':   _gdkimlib.gdk_imlib_get_filename
		}
		if attrs.has_key(attr):
			return attrs[attr](self._im)
		raise AttributeError, attr
	def render(self, width=None, height=None):
		return _gdkimlib.gdk_imlib_render(self._im,
						  width or self.rgb_width,
						  height or self.rgb_height)
	def get_image_border(self):
		return _gdkimlib.gdk_imlib_get_image_border(self._im)
	def set_image_border(self, left=0, right=0, top=0, bottom=0):
		_gdkimlib.gdk_imlib_set_image_border(self._im, (left, right,
								top, bottom))
	def get_image_shape(self):
		return _gdkimlib.gdk_imlib_get_image_shape(self._im)
	def set_image_shape(self, red, green, blue):
		_gdkimlib.gdk_imlib_set_image_shape(self._im,(red,green,blue))
	def save_image_to_eim(self, file):
		_gdkimlib.gdk_imlib_save_image_to_eim(self._im, file)
	def add_image_to_eim(self, file):
		_gdkimlib.gdk_imlib_add_image_to_eim(self._im, file)
	def save_image_to_ppm(self, file):
		_gdkimlib.gdk_imlib_save_image_to_ppm(self._im, file)
	def set_image_modifier(self, gamma, brightness, contrast):
		_gdkimlib.gdk_imlib_set_image_modifier(self._im,
					       (gamma, brightness, contrast))
	def set_image_red_modifier(self, gamma, brightness, contrast):
		_gdkimlib.gdk_imlib_set_image_red_modifier(self._im,
					       (gamma, brightness, contrast))
	def set_image_blue_modifier(self, gamma, brightness, contrast):
		_gdkimlib.gdk_imlib_set_image_red_modifier(self._im,
					       (gamma, brightness, contrast))
	def set_image_green_modifier(self, gamma, brightness, contrast):
		_gdkimlib.gdk_imlib_set_image_red_modifier(self._im,
					       (gamma, brightness, contrast))
	def get_image_modifier(self):
		return _gdkimlib.gdk_imlib_get_image_modifier(self._im)
	def get_image_red_modifier(self):
		return _gdkimlib.gdk_imlib_get_image_red_modifier(self._im)
	def get_image_green_modifier(self):
		return _gdkimlib.gdk_imlib_get_image_green_modifier(self._im)
	def get_image_blue_modifier(self):
		return _gdkimlib.gdk_imlib_get_image_blue_modifier(self._im)
	def set_image_red_curve(self, curve):
		_gdkimlib.gdk_imlib_set_image_red_curve(self._im, curve)
	def set_image_green_curve(self, curve):
		_gdkimlib.gdk_imlib_set_image_green_curve(self._im, curve)
	def set_image_blue_curve(self, curve):
		_gdkimlib.gdk_imlib_set_image_blue_curve(self._im, curve)
	def get_image_red_curve(self):
		return _gdkimlib.gdk_imlib_get_image_red_curve(self._im)
	def get_image_green_curve(self):
		return _gdkimlib.gdk_imlib_get_image_green_curve(self._im)
	def get_image_blue_curve(self):
		return _gdkimlib.gdk_imlib_get_image_blue_curve(self._im)
	def apply_modifiers_to_rgb(self):
		_gdkimlib.gdk_imlib_apply_modifiers_to_rgb(self._im)
	def changed_image(self):
		_gdkimlib.gdk_imlib_changed_image(self._im)
	def flip_image_horizontal(self):
		_gdkimlib.gdk_imlib_flip_image_horizontal(self._im)
	def flip_image_vertical(self):
		_gdkimlib.gdk_imlib_flip_image_vertical(self._im)
	def rotate_image(self, d):
		_gdkimlib.gdk_imlib_rotate_image(self._im, d)
	def clone_image(self):
		return Image(_obj=_gdkimlib.gdk_imlib_clone_image(self._im))
	def clone_scaled_image(self, width, height):
		return Image(_obj=_gdkimlib.gdk_imlib_clone_scaled_image(
			self._im, width, height))
	def crop_image(self, x, y, w, h):
		_gdkimlib.gdk_imlib_crop_image(self._im, x, y, w, h)
	def save_image(self, file):
		return _gdkimlib.gdk_imlib_save_image(self._im, file)
	def crop_and_clone_image(self, x, y, w, h):
		return Image(_obj=_gdkimlib.gdk_imlib_crop_and_clone_image(
			self._im, x, y, w, h))
	def make_pixmap(self):
		return GtkPixmap(_obj=
				 _gdkimlib.gdk_imlib_make_pixmap(self._im))
	def get_pixmap(self):
		return _gdkimlib.gdk_imlib_get_pixmap(self._im)
	if hasattr(_gdkimlib, 'gdk_imlib_image_get_array'):
		def get_array(self):
			return _gdkimlib.gdk_imlib_image_get_array(self._im)

def create_image_from_data(data, alpha, width, height):
	# alpha can be None
	return Image(_obj=_gdkimlib.gdk_imlib_create_image_from_data(
		data, alpha, width, height))

def create_image_from_xpm(data):
	return Image(_obj=_gdkimlib.gdk_imlib_create_image_from_xpm(data))

if hasattr(_gdkimlib, 'gdk_imlib_create_image_from_array'):
	def create_image_from_array(data, alpha=None):
		if alpha:
			return Image(_obj=
				_gdkimlib.gdk_imlib_create_image_from_array(
					data, alpha))
		else:
			return Image(_obj=
				_gdkimlib.gdk_imlib_create_image_from_array(
					data))

def create_image_from_drawable(drawable, mask, x, y, width, height):
	return Image(_obj=_gdkimlib.gdk_imlib_create_image_from_drawable(
		drawable, mask, x, y, width, height))

def get_render_type():
	return _gdkimlib.gdk_imlib_get_render_type()
def set_render_type(rend_type):
	_gdkimlib.gdk_imlib_set_render_type(rend_type)
def load_colors(file):
	return _gdkimlib.gdk_imlib_load_colors(file)
def free_colors():
	_gdkimlib.gdk_imlib_free_colors()
def get_fallback():
	return _gdkimlib.gdk_imlib_get_fallback()
def set_fallback(fallback):
	_gdkimlib.gdk_imlib_set_fallback(fallback)
def get_sysconfig():
	return _gdkimlib.gdk_imlib_get_sysconfig()
def best_color_match(r, g, b):
	return _gdkimlib.gdk_imlib_best_color_match(r, g, b)

# these are to help with apps that use imlib a lot.  They push/pop
# imlib's visual/colormap combination ...
def push_visual():
	_gdkimlib.gdk_imlib_push_visual()
def pop_visual():
	_gdkimlib.gdk_imlib_pop_visual()

