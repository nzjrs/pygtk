pkgpythondir = $(pythondir)/gtk-2.0
pkgpyexecdir = $(pyexecdir)/gtk-2.0

pygtkdir = $(pkgpythondir)/gtk
pygtk_PYTHON = __init__.py keysyms.py compat.py $(top_srcdir)/dsextras.py

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

if PLATFORM_WIN32
common_ldflags = -module -avoid-version -no-undefined
pygtk_win32_sources = gtk-fake-win32.c
else
common_ldflags = -module -avoid-version
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
  pygtkcellrenderer.c \
  $(pygtk_win32_sources)
nodist__gtk_la_SOURCES = gtk.c gdk.c

# prevent the .defs.c rule from trying to generate gtk-types.c because
# there is gtk-types.defs.  This is basically a hack and I really wish I
# could use pattern rules.  Perhaps we should rename gtk-types.c.
gtk-types.c:
	@:

gtk_overrides = \
  gdk.override \
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

EXTRA_DIST += $(gtk_overrides)

# libglade module
if BUILD_LIBGLADE
glade_la = glade.la
endif
pygtkexec_LTLIBRARIES += $(glade_la)
glade_la_CFLAGS = $(LIBGLADE_CFLAGS)
glade_la_LDFLAGS = $(common_ldflags) -export-symbols-regex initglade
glade_la_LIBADD = $(LIBGLADE_LIBS)
glade_la_SOURCES = libglademodule.c libglade.c
nodist_glade_la_SOURCES = libglade.c
libglade.c: $(COMMONDEFS) libglade.override
CLEANFILES += libglade.c
defs_DATA += libglade.defs
EXTRA_DIST += libglade.override

# gtkgl module
if BUILD_GTKGL
gl_la = gl.la
endif
pygtkexec_LTLIBRARIES += $(gl_la)
gl_la_CFLAGS = $(GTKGL_CFLAGS)
gl_la_LDFLAGS = $(common_ldflags) -export-symbols-regex initgl
gl_la_LIBADD = $(GTKGL_LIBS)
gl_la_SOURCES = gtkglmodule.c
nodist_gl_la_SOURCES = gtkgl.c
libgl.c: $(COMMONDEFS) gtkgl.override
CLEANFILES += gtkgl.c
defs_DATA += gtkgl.defs
EXTRA_DIST += gtkgl.override

.defs.c:
	(cd $(srcdir) \
	 && $(PYTHON) ../codegen/codegen.py \
	    --register ../pango-types.defs \
	    --register ../atk-types.defs \
	    --register gdk-types.defs \
	    --register gtk-types.defs \
	    --override $*.override \
	    --prefix py$* $*.defs) > gen-$*.c \
	&& cp gen-$*.c $*.c \
	&& rm -f gen-$*.c
