
INCLUDES = -I$(top_srcdir) $(PYTHON_INCLUDES) $(GTK_CFLAGS) \
	$(GDK_IMLIB_CFLAGS) $(LIBGLADE_CFLAGS) $(GTKGL_CFLAGS)

if BUILD_IMLIB
imlibmodule = imlibmodule.la
endif
if BUILD_LIBGLADE
libglademodule = libglademodule.la
endif
if BUILD_GTKGL
glmodule = glmodule.la
endif

pysitedir = $(PYTHON_SITE)
pysite_PYTHON = __init__.py GTK.py GDK.py

pyexecdir = $(PYTHON_SITE_EXEC)
pyexec_LTLIBRARIES = _gtkmodule.la $(imlibmodule) $(libglademodule) $(glmodule)

_gtkmodule_la_LDFLAGS = -module -avoid-version
_gtkmodule_la_LIBADD = $(GTK_LIBS)
_gtkmodule_la_SOURCES = \
	pygtk.h \
	pygtk-private.h \
	gtkmodule.c \
	gtkobject-support.c \
	gtk-types.c \
	gtk.c

imlibmodule_la_LDFLAGS = -module -avoid-version
imlibmodule_la_LIBADD = $(GDK_IMLIB_LIBS)
imlibmodule_la_SOURCES = \
	pygdkimlib.h \
	imlibmodule.c

libglademodule_la_LDFLAGS = -module -avoid-version
libglademodule_la_LIBADD = $(LIBGLADE_LIBS)
libglademodule_la_SOURCES = \
	libglademodule.c \
	libglade.c

glmodule_la_LDFLAGS = -module -avoid-version
glmodule_la_LIBADD = $(GTKGL_LIBS) $(GTK_LIBS)
glmodule_la_SOURCES = \
	gtkglmodule.c \
	gtkgl.c

# chmod hack here is because automake guys decided distdir should be
# read only for the next version :(
$(srcdir)/gtk.c: $(srcdir)/gtk.defs $(srcdir)/gdk.defs \
		 $(srcdir)/gtk-extrafuncs.defs $(srcdir)/gtk.override
	-chmod a+w $(srcdir)
	cd $(srcdir) \
	&& $(PYTHON) ../codegen/codegen.py -o gtk.override -p pygtk gtk.defs > gen-gtk.c \
	&& cp gen-gtk.c gtk.c \
	&& rm -f gen-gtk.c

$(srcdir)/libglade.c: $(srcdir)/libglade.defs $(srcdir)/libglade.override
	-chmod a+w $(srcdir)
	cd $(srcdir) \
	&& $(PYTHON) ../codegen/codegen.py -r gtk.defs -o libglade.override -p libglade libglade.defs > gen-libglade.c \
	&& cp gen-libglade.c libglade.c \
	&& rm -f gen-libglade.c

$(srcdir)/gtkgl.c: $(srcdir)/gtkgl.defs $(srcdir)/gtkgl.override
	-chmod a+w $(srcdir)
	cd $(srcdir) \
	&& $(PYTHON) ../codegen/codegen.py -r gtk.defs -o gtkgl.override -p pygtkgl gtkgl.defs > gen-gtkgl.c \
	&& cp gen-gtkgl.c gtkgl.c \
	&& rm -f gen-gtkgl.c


# a hack so that gtk.c is not included in the final package.
dist-hook:
	-rm -f $(distdir)/gtk.c $(distdir)/libglade.c $(distdir)/gtkgl.c

EXTRA_DIST = gtk.defs gtk-extrafuncs.defs gdk.defs gtk.override \
	libglade.defs libglade.override gtkgl.defs gtkgl.override
