# require automake-1.4a
AUTOMAKE_OPTIONS = 1.4a

SUBDIRS = codegen gtk

INCLUDES = $(PYTHON_INCLUDES) $(GLIB_CFLAGS)

pyexec_LTLIBRARIES = ExtensionClass.la gobjectmodule.la

ExtensionClass_la_LDFLAGS = -module -avoid-version
ExtensionClass_la_SOURCES = \
	ExtensionClass.h \
	ExtensionClass.c

gobjectmodule_la_LDFLAGS = -module -avoid-version
gobjectmodule_la_SOURCES = \
	gobjectmodule.c \
	pygobject.h
gobjectmodule_la_LIBADD = $(GLIB_LIBS)

noinst_PYTHON = ltihooks.py

EXTRA_DIST = \
	pygtk.spec \
	MAPPING \
	ExtensionClass.stx \
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
	examples/simple/hello2.py \
	examples/simple/simple2.py \
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

