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
pygtkexec_LTLIBRARIES = _gtkmodule.la
_gtkmodule_la_CFLAGS = $(GTK_CFLAGS)
_gtkmodule_la_LDFLAGS = $(common_ldflags) -export-symbols-regex init_gtk
_gtkmodule_la_LIBADD = $(GTK_LIBS)
_gtkmodule_la_SOURCES = \
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
# prevent the .defs.c rule from trying to generate gtk-types.c because
# there is gtk-types.defs.  This is basically a hack and I really wish I
# could use pattern rules.  Perhaps we should rename gtk-types.c.
gtk-types.c:
	:
gdk.c: $(srcdir)/gdk.defs $(COMMONDEFS) $(srcdir)/gdk.override
gtk.c: $(srcdir)/gtk.defs $(COMMONDEFS) $(srcdir)/gtk.override $(srcdir)/gtk-extrafuncs.defs
nodist__gtkmodule_la_SOURCES = gtk.c gdk.c
CLEANFILES += gtk.c gdk.c

defs_DATA += 			\
	gtk.defs 		\
	gtk-types.defs 		\
	gtk-extrafuncs.defs	\
	gdk.defs 		\
	gdk-types.defs

EXTRA_DIST += 			\
	gdk.override		\
	gtk.override 		\
	gtkclist.override	\
	gtkwidget.override	\
	gtkcontainer.override	\
	gtkstyle.override	\
	gtkwindow.override	\
	gtkctree.override	\
	gtktextview.override	\
	gtktreeview.override

# libglade module
if BUILD_LIBGLADE
glademodule_la = glademodule.la
endif
pygtkexec_LTLIBRARIES += $(glademodule_la)
glademodule_la_CFLAGS = $(LIBGLADE_CFLAGS)
glademodule_la_LDFLAGS = $(common_ldflags) -export-symbols-regex initglade
glademodule_la_LIBADD = $(LIBGLADE_LIBS)
glademodule_la_SOURCES = libglademodule.c
libglade.c: $(COMMONDEFS) libglade.override
nodist_glademodule_la_SOURCES = libglade.c
CLEANFILES += libglade.c
defs_DATA += libglade.defs
EXTRA_DIST += libglade.override

# gtkgl module
if BUILD_GTKGL
glmodule_la = glmodule.la
endif
pygtkexec_LTLIBRARIES += $(glmodule_la)
glmodule_la_CFLAGS = $(GTKGL_CFLAGS)
glmodule_la_LDFLAGS = $(common_ldflags) -export-symbols-regex initgl
glmodule_la_LIBADD = $(GTKGL_LIBS)
glmodule_la_SOURCES = gtkglmodule.c
libgl.c: $(COMMONDEFS) gtkgl.override
nodist_glmodule_la_SOURCES = gtkgl.c
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
