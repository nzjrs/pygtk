SUBDIRS = generate pyglade

INCLUDES= $(PYTHON_INCLUDES) $(PYTHON_CFLAGS) $(GTK_CFLAGS) \
	$(GDK_IMLIB_CFLAGS) $(GTKGL_CFLAGS) $(LIBGLADE_CFLAGS)

if BUILD_IMLIB
imlib_libs = _gdkimlibmodule$(SO)
imlib_mods = GdkImlib.py
else
imlib_libs =
imlib_mods =
endif
if BUILD_GTKGL
gtkgl_libs = _gtkglmodule$(SO)
gtkgl_mods = gtkgl.py
else
gtkgl_libs =
gtkgl_mods =
endif
if BUILD_LIBGLADE
libglade_libs = _libglademodule$(SO)
libglade_mods = libglade.py
else
libglade_libs =
libglade_mods =
endif

pyexec_PROGRAMS = _gtkmodule$(SO) $(imlib_libs) $(gtkgl_libs) $(libglade_libs)
EXTRA_PROGRAMS =

_gtkmodule__SO__SOURCES = gtkmodule.c
_gtkmodule__SO__LDADD = $(GTK_LIBS)
_gtkmodule__SO__LINK = $(PYTHON_LINK)


_gdkimlibmodule__SO__SOURCES = gdkimlibmodule.c
_gdkimlibmodule__SO__LDADD = $(GDK_IMLIB_LIBS)
_gdkimlibmodule__SO__LINK = $(PYTHON_LINK)

_gtkglmodule__SO__SOURCES = gtkglmodule.c
_gtkglmodule__SO__LDADD = $(GTKGL_LIBS) $(GTK_LIBS)
_gtkglmodule__SO__LINK = $(PYTHON_LINK)

_libglademodule__SO__SOURCES = libglademodule.c
_libglademodule__SO__LDADD = $(LIBGLADE_LIBS)
_libglademodule__SO__LINK = $(PYTHON_LINK)

pkginclude_HEADERS = pygtk.h pygdkimlib.h

python_PYTHON = gtk.py Gtkinter.py GTK.py GDK.py GtkExtra.py \
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


gtkmodule.o: gtkmodule.c gtkmodule_defs.c gtkmodule_impl.c

gtkglmodule.o: gtkglmodule.c gtkgl_defs.c gtkgl_impl.c

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
