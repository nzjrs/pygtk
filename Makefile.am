SUBDIRS = generate pyglade

INCLUDES= $(PYTHON_INCLUDES) $(PYTHON_CFLAGS) $(GTK_CFLAGS) $(GDK_IMLIB_CFLAGS)

## BUILD_IMLIB == # if imlib wasn't found
pyexec_PROGRAMS = _gtkmodule$(SO) @BUILD_IMLIB@ _gdkimlibmodule$(SO)
EXTRA_PROGRAMS =

_gtkmodule__SO__SOURCES = gtkmodule.c
_gtkmodule__SO__LDADD = $(GTK_LIBS)
_gtkmodule__SO__LINK = $(PYTHON_LINK)


_gdkimlibmodule__SO__SOURCES = gdkimlibmodule.c
_gdkimlibmodule__SO__LDADD = $(GDK_IMLIB_LIBS)
_gdkimlibmodule__SO__LINK = $(PYTHON_LINK)

python_PYTHON = gtk.py Gtkinter.py GTK.py GDK.py GtkExtra.py GdkImlib.py
noinst_PYTHON = mkgtk.py description.py

# stuff to automatically generate gtkmodule_*.c

GENERATE = generate/scmexpr.py generate/generate.py
GTKMODULE_DEFS = generate/gtk.defs generate/gtkbase.defs \
 generate/gtkcontainers.defs generate/gtkdata.defs generate/gtkedit.defs \
 generate/gtkenums.defs generate/gtklists.defs generate/gtkmenus.defs \
 generate/gtkmisc.defs generate/gtkranges.defs generate/gtkdnd.defs

gtkmodule_defs.c gtkmodule_impl.c: mkgtk.py $(GENERATE) $(GTKMODULE_DEFS)
	cd $(srcdir) && $(PYTHON) mkgtk.py

gtkmodule.o: gtkmodule_defs.c gtkmodule_impl.c

EXTRA_DIST = \
	pygtk.spec \
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
	examples/glade/test.glade
