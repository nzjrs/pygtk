
INCLUDES = -I$(top_srcdir) $(PYTHON_INCLUDES) $(GTK_CFLAGS) $(LIBGLADE_CFLAGS) -Wall

pygtkdir = $(pythondir)/gtk
pygtk_PYTHON = __init__.py keysyms.py

if BUILD_LIBGLADE
glademodule_la = glademodule.la
endif

pygtkexecdir = $(pyexecdir)/gtk
pygtkexec_LTLIBRARIES = _gtkmodule.la $(glademodule_la)

_gtkmodule_la_LDFLAGS = -module -avoid-version -export-symbols-regex init_gtk
_gtkmodule_la_LIBADD = $(GTK_LIBS)
_gtkmodule_la_SOURCES = \
  pygtk.h \
  pygtk-private.h \
  gtkmodule.c \
  gtkobject-support.c \
  gtk-types.c \
  pygtktreemodel.h \
  pygtktreemodel.c \
  gtk.c \
  gdk.c

glademodule_la_LDFLAGS = -module -avoid-version \
  -export-symbols-regex initlibglade
glademodule_la_LIBADD = $(LIBGLADE_LIBS)
glademodule_la_SOURCES = \
  libglademodule.c \
  libglade.c

pkginclude_HEADERS = pygtk.h
pkgincludedir = $(includedir)/pygtk-2.0/pygtk

# chmod hack here is because automake guys decided distdir should be
# read only for the next version :(
$(srcdir)/gtk.c: $(srcdir)/gtk.defs $(srcdir)/gdk.defs \
		 $(srcdir)/gtk-extrafuncs.defs $(srcdir)/gtk-types.defs \
		 $(srcdir)/gtk.override
	-chmod a+w $(srcdir)
	cd $(srcdir) \
	&& $(PYTHON) ../codegen/codegen.py \
	    --register ../pango-types.defs \
	    --register gdk-types.defs \
	    --override gtk.override \
	    --prefix pygtk gtk.defs > gen-gtk.c \
	&& cp gen-gtk.c gtk.c \
	&& rm -f gen-gtk.c

$(srcdir)/gdk.c: $(srcdir)/gdk.defs $(srcdir)/gdk-types.defs \
		 $(srcdir)/gdk.override
	-chmod a+w $(srcdir)
	cd $(srcdir) \
	&& $(PYTHON) ../codegen/codegen.py \
	    --register ../pango-types.defs \
	    --register gtk-types.defs \
	    --override gdk.override \
	    --prefix pygdk gdk.defs > gen-gdk.c \
	&& cp gen-gdk.c gdk.c \
	&& rm -f gen-gdk.c

$(srcdir)/libglade.c: $(srcdir)/libglade.defs $(srcdir)/libglade.override
	-chmod a+w $(srcdir)
	cd $(srcdir) \
	&& $(PYTHON) ../codegen/codegen.py \
	    --register ../pango-types.defs \
	    --register gdk-types.defs \
	    --register gtk-types.defs \
	    --override libglade.override \
	    --prefix pylibglade libglade.defs > gen-libglade.c \
	&& cp gen-libglade.c libglade.c \
	&& rm -f gen-libglade.c

# a hack so that gtk.c is not included in the final package.
dist-hook:
	-rm -f $(distdir)/gtk.c $(distdir)/libglade.c

defsdir = $(pkgdatadir)/2.0/defs
defs_DATA = gtk.defs gtk-types.defs gtk-extrafuncs.defs \
  gdk.defs gdk-types.defs libglade.defs

EXTRA_DIST = gtk.defs gtk-types.defs gtk-extrafuncs.defs gtk.override \
  gdk.defs gdk-types.defs gdk.override libglade.defs libglade.override
