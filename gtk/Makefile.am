pygtkdir = $(pythondir)/gtk
pygtk_PYTHON = __init__.py keysyms.py

pygtkexecdir = $(pyexecdir)/gtk

defsdir = $(pkgdatadir)/2.0/defs
defs_DATA =
EXTRA_DIST = $(defs_DATA)
CLEANFILES =

pkgincludedir = $(includedir)/pygtk-2.0/pygtk
pkginclude_HEADERS = pygtk.h

INCLUDES = -I$(top_srcdir) $(PYTHON_INCLUDES) -Wall
COMMONDEFS = $(srcdir)/gtk-types.defs $(srcdir)/gdk-types.defs $(top_srcdir)/atk-types.defs \
	     $(top_srcdir)/pango-types.defs

# gtk module
pygtkexec_LTLIBRARIES = _gtkmodule.la
_gtkmodule_la_CFLAGS = $(GTK_CFLAGS)
_gtkmodule_la_LDFLAGS = -module -avoid-version -export-symbols-regex init_gtk
_gtkmodule_la_LIBADD = $(GTK_LIBS)
_gtkmodule_la_SOURCES = pygtk.h \
			pygtk-private.h \
			gtkmodule.c \
			gtkobject-support.c \
			gtk-types.c \
			pygtktreemodel.h \
			pygtktreemodel.c
# prevent the .defs.c rule from trying to generate gtk-types.c because
# there is gtk-types.defs.  This is basically a hack and I really wish I
# could use pattern rules.  Perhaps we should rename gtk-types.c.
gtk-types.c:
	:
gdk.c: $(srcdir)/gdk.defs $(COMMONDEFS) $(srcdir)/gdk.override
gtk.c: $(srcdir)/gtk.defs $(COMMONDEFS) $(srcdir)/gtk.override $(srcdir)/gtk-extrafuncs.defs
nodist__gtkmodule_la_SOURCES = gtk.c gdk.c
CLEANFILES += gtk.c gdk.c
defs_DATA += gtk.defs gtk-types.defs gtk-extrafuncs.defs gdk.defs \
	     gdk-types.defs
EXTRA_DIST += gtk.override gdk.override

# libglade module
if BUILD_LIBGLADE
glademodule_la = glademodule.la
endif
pygtkexec_LTLIBRARIES += $(glademodule_la)
glademodule_la_CFLAGS = $(LIBGLADE_CFLAGS)
glademodule_la_LDFLAGS = -module -avoid-version \
			 -export-symbols-regex initlibglade
glademodule_la_LIBADD = $(LIBGLADE_LIBS)
glademodule_la_SOURCES = libglademodule.c
libglade.c: $(COMMONDEFS) libglade.override
nodist_glademodule_la_SOURCES = libglade.c
CLEANFILES += libglade.c
defs_DATA += libglade.defs
EXTRA_DIST += libglade.override

.defs.c:
	(cd $(srcdir) \
	 && $(PYTHON) ../codegen/codegen.py \
	    --register ../pango-types.defs \
	    --register ../atk-types.defs \
	    --register gdk-types.defs \
	    --override $*.override \
	    --prefix py$* $*.defs) > gen-$*.c \
	&& cp gen-$*.c $*.c \
	&& rm -f gen-$*.c

