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
	_gtk.py			\
	_lazyutils.py 		\
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
gdk.defs: Makefile

gtk.defs: $(GTK_DEFS)
	$(CREATEDEFS) $@ $(GTK_DEFS)
gtk.defs: Makefile

gdk-types.defs: $(GDK_TYPES_DEFS)
	$(CREATEDEFS) $@ $(GDK_TYPES_DEFS)
gdk-types.defs: Makefile

gtk-types.defs: $(GTK_TYPES_DEFS)
	$(CREATEDEFS) $@ $(GTK_TYPES_DEFS)
gtk-types.defs: Makefile

gdk.c: gdk-types.defs gdk.defs $(GDK_OVERRIDES)
gtk.c: gdk-types.defs gtk-types.defs gtk.defs gdk-types.defs $(GTK_OVERRIDES)
_gtkimpl_la_CFLAGS = $(PYCAIRO_CFLAGS) $(GTK_CFLAGS)
_gtkimpl_la_LDFLAGS = $(common_ldflags) -export-symbols-regex init_gtkimpl
_gtkimpl_la_LIBADD = $(GTK_LIBS)
_gtkimpl_la_SOURCES = 		\
	gtk-types.c 		\
	gtkmodule.c 		\
	gtkobject-support.c 	\
	pygtk.h 		\
	pygtk-private.h 	\
	pygtkcellrenderer.h 	\
	pygtkcellrenderer.c	\
	pygtktreemodel.h 	\
	pygtktreemodel.c
nodist__gtkimpl_la_SOURCES = gtk.c gdk.c
if BUILD_GTK
pygtkexec_LTLIBRARIES += _gtkimpl.la
defs_DATA += $(GDK_DEFS) $(GDK_TYPES_DEFS) $(GTK_DEFS) $(GTK_TYPES_DEFS) \
	gdk.defs gtk.defs gdk-types.defs gtk-types.defs
endif
if PLATFORM_WIN32
_gtkimpl_la_CFLAGS  += -DPLATFORM_WIN32
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


install-exec-hook: # Remove old deprecated modules
	-$(LIBTOOL) --mode=uninstall rm -f $(pygtkdir)/_gtk.la
