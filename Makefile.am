AUTOMAKE_OPTIONS = 1.4a

SUBDIRS = generate pyglade

INCLUDES= $(PYTHON_INCLUDES) $(GTK_CFLAGS) $(GDK_IMLIB_CFLAGS) \
	$(GTKGL_CFLAGS) $(LIBGLADE_CFLAGS)

if BUILD_IMLIB
imlib_libs = _gdkimlibmodule.la
imlib_mods = GdkImlib.py
else
imlib_libs =
imlib_mods =
endif
if BUILD_GTKGL
gtkgl_libs = _gtkglmodule.la
gtkgl_mods = gtkgl.py
else
gtkgl_libs =
gtkgl_mods =
endif
if BUILD_LIBGLADE
libglade_libs = _libglademodule.la
libglade_mods = libglade.py
else
libglade_libs =
libglade_mods =
endif

pyexecdir = $(PYTHON_SITE_EXEC)
pyexec_LTLIBRARIES = _gtkmodule.la $(imlib_libs) $(gtkgl_libs) $(libglade_libs)

_gtkmodule_la_LDFLAGS = -module -avoid-version
_gtkmodule_la_SOURCES = gtkmodule.c
_gtkmodule_la_LIBADD = $(GTK_LIBS)


_gdkimlibmodule_la_LDFLAGS = -module -avoid-version
_gdkimlibmodule_la_SOURCES = gdkimlibmodule.c
_gdkimlibmodule_la_LIBADD = $(GDK_IMLIB_LIBS)

_gtkglmodule_la_LDFLAGS = -module -avoid-version
_gtkglmodule_la_SOURCES = gtkglmodule.c
_gtkglmodule_la_LIBADD = $(GTKGL_LIBS) $(GTK_LIBS)

_libglademodule_la_LDFLAGS = -module -avoid-version
_libglademodule_la_SOURCES = libglademodule.c
_libglademodule_la_LIBADD = $(LIBGLADE_LIBS)

pkginclude_HEADERS = pygtk.h pygdkimlib.h

pysitedir = $(PYTHON_SITE)
pysite_PYTHON = gtk.py Gtkinter.py GTK.py GDK.py GtkExtra.py \
	$(imlib_mods) $(gtkgl_mods) $(libglade_mods)
noinst_PYTHON = mkgtk.py description.py

# stuff to automatically generate gtkmodule_*.c

GENERATE = generate/scmexpr.py generate/generate.py

GTKMODULE_DEFS = generate/gtk.defs \
  generate/gtkbase.defs   generate/gtkcontainers.defs \
  generate/gtkdata.defs   generate/gtkedit.defs \
  generate/gtkenums.defs  generate/gtklists.defs \
  generate/gtkmenus.defs  generate/gtkmisc.defs \
  generate/gtkranges.defs generate/gtkdnd.defs

$(srcdir)/gtkmodule_defs.c $(srcdir)/gtkmodule_impl.c $(srcdir)/gtkgl_defs.c $(srcdir)/gtkgl_impl.c $(srcdir)/libglade_defs.c $(srcdir)/libglade_impl.c: $(srcdir)/mkgtk.py $(GENERATE) $(GTKMODULE_DEFS) generate/gtkgl.defs generate/libglade.defs
	cd $(srcdir) && $(PYTHON) mkgtk.py


gtkmodule.lo: gtkmodule.c gtkmodule_defs.c gtkmodule_impl.c

gtkglmodule.lo: gtkglmodule.c gtkgl_defs.c gtkgl_impl.c

EXTRA_DIST = \
	pygtk.spec \
	MAPPING \
	examples/imlib/view.py \
	examples/ide/gtkcons.py \
	examples/ide/gtkdb.py \
	examples/ide/edit.py \
	examples/ide/browse.py \
	examples/ide/minibreak.xpm \
	examples/ide/README \
	examples/ide/gtkprof.py \
	examples/ide/pyide.py \
	examples/ide/break.xpm \
	examples/ide/return.xpm \
	examples/ide/next.xpm \
	examples/ide/continue.xpm \
	examples/ide/step.xpm \
	examples/ide/edit.xpm \
	examples/ide/run.xpm \
	examples/ide/quit.xpm \
	examples/simple/hello1.py \
	examples/simple/hello2.py \
	examples/simple/simple1.py \
	examples/simple/simple2.py \
	examples/simple/tooltip1.py \
	examples/simple/tooltip2.py \
	examples/simple/scribble.py \
	examples/simple/dnd.py \
	examples/simple/dndpixmap.py \
	examples/simple/README \
	examples/testgtk/testgtk.py \
	examples/testgtk/test.xpm \
	examples/testgtk/testgtkrc \
	examples/testgtk/README \
	examples/neil/TAppli.py \
	examples/neil/TAppli2.py \
	examples/neil/TAppli3.py \
	examples/neil/TAppli4.py \
	examples/neil/TAppli5.py \
	examples/neil/bomb.xpm \
	examples/neil/gtk-logo.xpm \
	examples/neil/question.xpm \
	examples/neil/notebook.py \
	examples/neil/README \
	examples/glade/README \
	examples/glade/test2.glade \
	examples/glade/glade.py \
	examples/glade/test.glade \
	examples/gl/README \
	examples/gl/cone.py \
	examples/gl/dots.py \
	examples/gl/gears.py


snap:
	$(MAKE) dist distdir=$(PACKAGE)-SNAP-`date +"%Y%m%d"`
