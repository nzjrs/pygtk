pkgpythondir = $(pythondir)/gtk-2.0
pkgpyexecdir = $(pyexecdir)/gtk-2.0

pygtkdir = $(pkgpythondir)/gtk
pygtk_PYTHON = __init__.py keysyms.py compat.py

pygtkexecdir = $(pkgpyexecdir)/gtk

defsdir = $(pkgdatadir)/2.0/defs
defs_DATA =
EXTRA_DIST = $(defs_DATA)
CLEANFILES =

EXTRA_DIST += makefile.msc gtk-fake-win32.c

pkgincludedir = $(includedir)/pygtk-2.0/pygtk
pkginclude_HEADERS = pygtk.h

INCLUDES = -I$(top_srcdir) $(PYTHON_INCLUDES)
COMMONDEFS = $(srcdir)/gtk-types.defs $(srcdir)/gdk-types.defs \
  $(top_srcdir)/atk-types.defs $(top_srcdir)/pango-types.defs

common_ldflags = -module -avoid-version
if PLATFORM_WIN32
common_ldflags += -no-undefined
endif

# gtk module
pygtkexec_LTLIBRARIES = _gtk.la
_gtk_la_CFLAGS = $(GTK_CFLAGS)
_gtk_la_LDFLAGS = $(common_ldflags) -export-symbols-regex init_gtk
_gtk_la_LIBADD = $(GTK_LIBS)
_gtk_la_SOURCES = \
  pygtk.h \
  pygtk-private.h \
  gtkmodule.c \
  gtkobject-support.c \
  gtk-types.c \
  pygtktreemodel.h \
  pygtktreemodel.c \
  pygtkcellrenderer.h \
  pygtkcellrenderer.c
nodist__gtk_la_SOURCES = gtk.c gdk.c

if PLATFORM_WIN32
_gtk_la_SOURCES += gtk-fake-win32.c
_gtk_la_CFLAGS  += -DPLATFORM_WIN32
endif

# prevent the .defs.c rule from trying to generate gtk-types.c because
# there is gtk-types.defs.  This is basically a hack and I really wish I
# could use pattern rules.  Perhaps we should rename gtk-types.c.
gtk-types.c:
	@:

gtk_overrides = \
  gtk.override \
  gtkclist.override \
  gtkwidget.override \
  gtkcontainer.override \
  gtkstyle.override \
  gtkwindow.override \
  gtkctree.override \
  gtktextview.override \
  gtktreeview.override

gdk.c: $(srcdir)/gdk.defs $(COMMONDEFS) $(srcdir)/gdk.override
gtk.c: $(srcdir)/gtk.defs $(COMMONDEFS) $(gtk_overrides) $(srcdir)/gtk-extrafuncs.defs
CLEANFILES += gtk.c gdk.c

defs_DATA += 			\
	gtk.defs 		\
	gtk-types.defs 		\
	gtk-extrafuncs.defs	\
	gdk.defs 		\
	gdk-types.defs

EXTRA_DIST += gdk.override $(gtk_overrides)

# libglade module
if BUILD_LIBGLADE
pygtkexec_LTLIBRARIES += glade.la
endif
glade_la_CFLAGS = $(LIBGLADE_CFLAGS)
glade_la_LDFLAGS = $(common_ldflags) -export-symbols-regex initglade
glade_la_LIBADD = $(LIBGLADE_LIBS)
glade_la_SOURCES = libglademodule.c
nodist_glade_la_SOURCES = libglade.c
libglade.c: $(COMMONDEFS) libglade.override
CLEANFILES += libglade.c
defs_DATA += libglade.defs
EXTRA_DIST += libglade.override

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
