AUTOMAKE_OPTIONS = 1.7

PLATFORM_VERSION = 2.0

CLEANFILES =
EXTRA_DIST = 
INCLUDES = $(PYTHON_INCLUDES) -I$(top_srcdir)/gobject 

# defs files
defsdir = $(pkgdatadir)/$(PLATFORM_VERSION)/defs
defs_DATA =

# python scripts & extension modules
pkgpythondir = $(pyexecdir)/gtk-2.0
pkgpyexecdir = $(pyexecdir)/gtk-2.0

# gtk python extension modules
pygtkexecdir = $(pkgpyexecdir)/gtk
pygtkexec_LTLIBRARIES =

# gtk python scripts
pygtkdir = $(pkgpythondir)/gtk
pygtk_PYTHON = __init__.py keysyms.py compat.py

# gtk headers
pkgincludedir = $(includedir)/pygtk-$(PLATFORM_VERSION)/pygtk
pkginclude_HEADERS = pygtk.h

# linker flags
common_ldflags = -module -avoid-version
if PLATFORM_WIN32
common_ldflags += -no-undefined
endif

# gtk module
GDK_DEFS = gdk.defs gdk-types.defs
GTK_DEFS = gtk.defs gtk-types.defs gtk-extrafuncs.defs
GDK_OVERRIDES = gdk.override gdkcairo.override
GTK_OVERRIDES = 		\
	gtk.override 		\
	gtkclist.override 	\
	gtkcontainer.override 	\
	gtkctree.override 	\
	gtkiconview.override 	\
	gtkstyle.override 	\
	gtktextview.override 	\
	gtktreeview.override 	\
	gtkwidget.override 	\
	gtkwindow.override
CLEANFILES += gtk.c gdk.c
EXTRA_DIST += $(GDK_DEFS) $(GDK_OVERRIDES) $(GTK_DEFS) $(GTK_OVERRIDES)
gdk.c: $(GDK_DEFS) $(GDK_OVERRIDES)
gtk.c: $(GTK_DEFS) $(GTK_OVERRIDES)
_gtk_la_CFLAGS = $(GTK_CFLAGS)
_gtk_la_LDFLAGS = $(common_ldflags) -export-symbols-regex init_gtk
_gtk_la_LIBADD = $(GTK_LIBS)
_gtk_la_SOURCES = 		\
	gtk-types.c 		\
	gtkmodule.c 		\
	gtkobject-support.c 	\
	pygtk.h 		\
	pygtk-private.h 	\
	pygtkcellrenderer.h 	\
	pygtkcellrenderer.c	\
	pygtktreemodel.h 	\
	pygtktreemodel.c
nodist__gtk_la_SOURCES = gtk.c gdk.c
if BUILD_GTK
pygtkexec_LTLIBRARIES += _gtk.la
defs_DATA += $(GTK_DEFS) $(GDK_DEFS)
endif
if PLATFORM_WIN32
_gtk_la_CFLAGS  += -DPLATFORM_WIN32
endif

# libglade module
LIBGLADE_DEFS = libglade.defs
CLEANFILES += libglade.c
EXTRA_DIST += $(LIBGLADE_DEFS) libglade.override
libglade.c: $(LIBGLADE_DEFS) libglade.override
glade_la_CFLAGS = $(LIBGLADE_CFLAGS)
glade_la_LIBADD = $(LIBGLADE_LIBS)
glade_la_LDFLAGS = $(common_ldflags) -export-symbols-regex initglade
glade_la_SOURCES = libglademodule.c
nodist_glade_la_SOURCES = libglade.c
if BUILD_LIBGLADE
pygtkexec_LTLIBRARIES += glade.la
defs_DATA += $(LIBGLADE_DEFS)
endif

.defs.c:
	(cd $(srcdir) \
	 && $(PYTHON) ../codegen/codegen.py \
	   $(PYGTK_CODEGEN_DEFINES) \
	    --register ../pango-types.defs \
	    --register ../atk-types.defs \
	    --register gdk-types.defs \
	    --register gtk-types.defs \
	    --override $*.override \
	    --prefix py$* $*.defs) > gen-$*.c \
	&& cp gen-$*.c $*.c \
	&& rm -f gen-$*.c

# prevent the .defs.c rule from trying to generate gtk-types.c because
# there is gtk-types.defs.  This is basically a hack and I really wish I
# could use pattern rules.  Perhaps we should rename gtk-types.c.
gtk-types.c:
	@:

