
import gtk; _gtk = gtk; del gtk
import _libglade

# enable gnome functionality if it is installed.
try:
	import _gladegnome
except:
	pass

class GladeXML(_gtk.GtkData):
	def __init__(self, filename=None, root=None, domain=None, _obj=None):
		if _obj: self._o = _obj; return
		self._o = _libglade.glade_xml_new_with_domain(filename, root,
							      domain)
	class __cnv:
		def __init__(self, func):
			self.func = func
		def __call__(self, *args):
			a = list(args)
			for i in range(len(args)):
				if type(args[i]) == _gtk._gtk.GtkObjectType:
					a[i] = _gtk._obj2inst(args[i])
				elif type(args[i]) == \
				     _gtk._gtk.GtkAccelGroupType:
					a[i] = _gtk.GtkAccelGroup(_obj=args[i])
			a = tuple(a)
			ret = apply(self.func, a)
			if hasattr(ret, '_o'): ret = ret._o
			elif hasattr(ret, '_ag'): ret = ret._ag
			elif hasattr(ret, '_im'): ret = ret._im
			return ret
	def signal_connect(self, handler_name, handler, *args):
		_libglade.glade_xml_signal_connect(self._o, handler_name,
						self.__cnv(handler).__call__,
						args)
	def signal_autoconnect(self, dict):
		hdict = {}
		for key, value in dict.items():
			if type(value) == type(()) and len(value) >= 2:
				hdict[key] = (self.__cnv(value[0]).__call__,
					      value[1:])
			else:
				hdict[key] = self.__cnv(value).__call__
		_libglade.glade_xml_signal_autoconnect(self._o, hdict)
	def get_widget(self, name):
		wid = _libglade.glade_xml_get_widget(self._o, name)
		if wid:
			return _gtk._obj2inst(wid)
		else:
			return wid
	def get_widget_by_longname(self, longname):
		wid = _libglade.glade_xml_get_widget_by_long_name(self._o,
								  longname)
		if wid:
			return _gtk._obj2inst(wid)
		else:
			return wid
_gtk._name2cls['GladeXML'] = GladeXML

def get_widget_name(widget):
	return _libglade.glade_get_widget_name(widget._o)
def get_widget_long_name(widget):
	return _libglade.glade_get_widget_long_name(widget._o)
def get_widget_tree(widget):
	xml = _libglade.glade_get_widget_tree(widget._o)
	if xml:
		return GladeXML(_obj=xml)
	return xml

