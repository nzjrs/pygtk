AUTOMAKE_OPTIONS = 1.7

PLATFORM_VERSION = 2.0

CLEANFILES =
EXTRA_DIST = 
INCLUDES = \
	$(PYTHON_INCLUDES) \
	$(PYGOBJECT_CFLAGS) \
	-I$(srcdir)/gtk

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
pygtk_PYTHON = 			\
	__init__.py 		\
	compat.py		\
	deprecation.py		\
	keysyms.py

# gtk headers
pkgincludedir = $(includedir)/pygtk-$(PLATFORM_VERSION)/pygtk
pkginclude_HEADERS = pygtk.h

# linker flags
common_ldflags = -module -avoid-version
if PLATFORM_WIN32
common_ldflags += -no-undefined
endif

# gtk module
GTK_DEFS = gtk-base.defs
GDK_DEFS = gdk-base.defs
GTK_TYPES_DEFS = gtk-base-types.defs
GDK_TYPES_DEFS = gdk-base-types.defs

if HAVE_GTK_2_10
GDK_DEFS += gdk-2.10.defs 
GTK_DEFS += gtk-2.10.defs 
GTK_TYPES_DEFS += gtk-2.10-types.defs
endif

CREATEDEFS = $(PYTHON) $(top_srcdir)/codegen/createdefs.py

GDK_OVERRIDES = \
	gdk.override 		\
	gdkcairo.override	\
	gdkcolor.override	\
	gdkdraw.override	\
	gdkevent.override	\
	gdkgc.override		\
	gdkpixbuf.override	\
	gdkrectangle.override	\
	gdkwindow.override

GTK_OVERRIDES = 		\
	gtk.override 		\
	gtkclist.override 	\
	gtkcontainer.override 	\
	gtkctree.override 	\
	gtkiconview.override 	\
	gtkstyle.override 	\
	gtktextview.override 	\
	gtktoolbar.override 	\
	gtktreeview.override 	\
	gtkwidget.override 	\
	gtkwindow.override

CLEANFILES += 			\
	gtk.c 			\
	gdk.c			\
	gtk.defs		\
	gtk-types.defs		\
	gdk.defs		\
	gdk-types.defs

EXTRA_DIST += 			\
	gtk-extrafuncs.defs	\
	$(GDK_DEFS) 		\
	$(GDK_TYPES_DEFS) 	\
	$(GDK_OVERRIDES) 	\
	$(GTK_DEFS) 		\
	$(GTK_TYPES_DEFS)	\
	$(GTK_OVERRIDES)


gdk.defs: $(GDK_DEFS)
	$(CREATEDEFS) $@ $(GDK_DEFS)

gtk.defs: $(GTK_DEFS)
	$(CREATEDEFS) $@ $(GTK_DEFS)

gdk-types.defs: $(GDK_TYPES_DEFS)
	$(CREATEDEFS) $@ $(GDK_TYPES_DEFS)

gtk-types.defs: $(GTK_TYPES_DEFS)
	$(CREATEDEFS) $@ $(GTK_TYPES_DEFS)

gdk.c: gdk-types.defs gdk.defs $(GDK_OVERRIDES)
gtk.c: gdk-types.defs gtk-types.defs gtk.defs gdk-types.defs $(GTK_OVERRIDES)
_gtk_la_CFLAGS = $(PYCAIRO_CFLAGS) $(GTK_CFLAGS)
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
defs_DATA += $(GDK_DEFS) $(GDK_TYPES_DEFS) $(GTK_DEFS) $(GTK_TYPES_DEFS) \
	gdk.defs gtk.defs gdk-types.defs gtk-types.defs
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

# gtkunixprint module
GTKUNIXPRINT_DEFS = gtkunixprint.defs
CLEANFILES += gtkunixprint.c
EXTRA_DIST += $(GTKUNIXPRINT_DEFS) gtkunixprint.override
gtkunixprint.c: $(GTKUNIXPRINT_DEFS) gtkunixprint.override
gtkunixprint_la_CFLAGS = $(PYCAIRO_CFLAGS) $(GTKUNIXPRINT_CFLAGS)
gtkunixprint_la_LIBADD = $(GTKUNIXPRINT_LIBS)
gtkunixprint_la_LDFLAGS = $(common_ldflags) -export-symbols-regex initgtkunixprint
gtkunixprint_la_SOURCES = gtkunixprintmodule.c
nodist_gtkunixprint_la_SOURCES = gtkunixprint.c
if BUILD_GTKUNIXPRINT
pkgpyexec_LTLIBRARIES = gtkunixprint.la
defs_DATA += $(GTKUNIXPRINT_DEFS)
endif

if PLATFORM_WIN32
_gtk_la_CFLAGS  += -DPLATFORM_WIN32
endif


.defs.c: 
	($(PYTHON) $(top_srcdir)/codegen/codegen.py \
	   $(PYGTK_CODEGEN_DEFINES) \
	    -I $(srcdir) \
	    --register $(top_srcdir)/pango-types.defs \
	    --register $(top_srcdir)/atk-types.defs \
	    --register gdk-types.defs \
	    --register gtk-types.defs \
	    --override $(srcdir)/$*.override \
	    --prefix py$* $<) > gen-$*.c \
	&& cp gen-$*.c $*.c \
	&& rm -f gen-$*.c

# prevent the .defs.c rule from trying to generate gtk-types.c because
# there is gtk-types.defs.  This is basically a hack and I really wish I
# could use pattern rules.  Perhaps we should rename gtk-types.c.
gtk-types.c:
	@:

