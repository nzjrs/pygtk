AUTOMAKE_OPTIONS = 1.6

SUBDIRS = generate pyglade

INCLUDES= $(PYTHON_INCLUDES) $(GTK_CFLAGS) $(GDK_IMLIB_CFLAGS) \
  $(GTKGL_CFLAGS) $(LIBGLADE_CFLAGS) $(GDK_PIXBUF_CFLAGS)

# this file is common to all pygtk versions.
python_PYTHON = pygtk.py

# install pth file.
install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(pythondir)
	echo "gtk-1.2" > $(DESTDIR)$(pythondir)/pygtk.pth
install-exec-local:
	$(mkinstalldirs) $(DESTDIR)$(pyexecdir)
	echo "gtk-1.2" > $(DESTDIR)$(pyexecdir)/pygtk.pth
uninstall-local:
	rm -f $(DESTDIR)$(pythondir)/pygtk.pth $(DESTDIR)$(pyexecdir)/pygtk.pth

pkgpythondir = $(pythondir)/gtk-1.2
pkgpyexecdir = $(pyexecdir)/gtk-1.2

noinst_PYTHON = mkgtk.py description.py

pkgpyexec_LTLIBRARIES = _gtkmodule.la $(gdkimlibmodule) $(gtkglmodule) \
  $(libglademodule) $(gdkpixbufmodule)
pkgpython_PYTHON = gtk.py GTK.py GDK.py GtkExtra.py $(gdkimlibpy) \
  $(gtkglpy) $(libgladepy)

if BUILD_IMLIB
gdkimlibmodule = _gdkimlibmodule.la
gdkimlibpy = GdkImlib.py
endif
if BUILD_GTKGL
gtkglmodule = _gtkglmodule.la
gtkglpy = gtkgl.py
endif
if BUILD_LIBGLADE
libglademodule = _libglademodule.la
libgladepy = libglade.py
endif
if BUILD_PIXBUF
gdkpixbufmodule = gdkpixbufmodule.la
endif

_gtkmodule_la_LDFLAGS = -module -avoid-version -export-symbols-regex init_gtk
_gtkmodule_la_SOURCES = gtkmodule.c
_gtkmodule_la_LIBADD = $(GTK_LIBS)


_gdkimlibmodule_la_LDFLAGS = -module -avoid-version -export-symbols-regex init_gdkimlib
_gdkimlibmodule_la_SOURCES = gdkimlibmodule.c
_gdkimlibmodule_la_LIBADD = $(GDK_IMLIB_LIBS)

_gtkglmodule_la_LDFLAGS = -module -avoid-version -export-symbols-regex init_gtkgl
_gtkglmodule_la_SOURCES = gtkglmodule.c
_gtkglmodule_la_LIBADD = $(GTKGL_LIBS) $(GTK_LIBS)

_libglademodule_la_LDFLAGS = -module -avoid-version -export-symbols-regex init_libglade
_libglademodule_la_SOURCES = libglademodule.c
_libglademodule_la_LIBADD = $(LIBGLADE_LIBS)

gdkpixbufmodule_la_LDFLAGS = -module -avoid-version -export-symbols-regex initgdkpixbuf
gdkpixbufmodule_la_SOURCES = gdkpixbufmodule.c
gdkpixbufmodule_la_LIBADD = $(GDK_PIXBUF_LIBS)

pkginclude_HEADERS = pygtk.h pygdkimlib.h pygdkpixbuf.h

# stuff to automatically generate gtkmodule_*.c

GENERATE = $(srcdir)/generate/scmexpr.py $(srcdir)/generate/generate.py

GTKMODULE_DEFS = $(srcdir)/generate/gtk.defs \
  $(srcdir)/generate/gtkbase.defs   $(srcdir)/generate/gtkcontainers.defs \
  $(srcdir)/generate/gtkdata.defs   $(srcdir)/generate/gtkedit.defs \
  $(srcdir)/generate/gtkenums.defs  $(srcdir)/generate/gtklists.defs \
  $(srcdir)/generate/gtkmenus.defs  $(srcdir)/generate/gtkmisc.defs \
  $(srcdir)/generate/gtkranges.defs $(srcdir)/generate/gtkdnd.defs

GEN_FILES = \
  gtkmodule_defs.c gtkmodule_impl.c gtkgl_defs.c gtkgl_impl.c \
  libglade_defs.c libglade_impl.c

$(GEN_FILES): $(srcdir)/mkgtk.py $(GENERATE) $(GTKMODULE_DEFS) $(srcdir)/generate/gtkgl.defs $(srcdir)/generate/libglade.defs
	$(PYTHON) $(srcdir)/mkgtk.py $(srcdir)

distclean-local:
	rm -f $(GEN_FILES)

gtkmodule.lo: gtkmodule.c gtkmodule_defs.c gtkmodule_impl.c

gtkglmodule.lo: gtkglmodule.c gtkgl_defs.c gtkgl_impl.c

EXTRA_DIST = \
  pygtk.spec \
  MAPPING \
  mkgtk.py \
  description.py \
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
