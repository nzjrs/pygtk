# require automake-1.5
AUTOMAKE_OPTIONS = 1.5

SUBDIRS = codegen gtk

INCLUDES = $(PYTHON_INCLUDES) $(GLIB_CFLAGS) $(PANGO_CFLAGS)

pyexec_LTLIBRARIES = gobjectmodule.la pangomodule.la

gobjectmodule_la_LDFLAGS = -module -avoid-version \
 -export-symbols-regex initgobject
gobjectmodule_la_SOURCES = \
	gobjectmodule.c \
	pygobject.h
gobjectmodule_la_LIBADD = $(GLIB_LIBS)

pangomodule_la_LDFLAGS = -module -avoid-version \
 -export-symbols-regex initpango
pangomodule_la_SOURCES = \
  pangomodule.c \
  pango.c
pangomodule_la_LIBADD = $(PANGO_LIBS)

$(srcdir)/pango.c: $(srcdir)/pango.defs $(srcdir)/pango.override
	-chmod a+w $(srcdir)
	cd $(srcdir) \
	&& $(PYTHON) codegen/codegen.py \
	    --override pango.override \
	    --prefix pypango pango.defs > gen-pango.c \
	&& cp gen-pango.c pango.c \
	&& rm -f gen-pango.c

pkginclude_HEADERS = pygobject.h

noinst_PYTHON = ltihooks.py

dist-hook:
	-rm -f $(distdir)/pango.c

defsdir = $(pkgdatadir)/2.0/defs
defs_DATA = pango.defs

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = pygtk-2.0.pc

EXTRA_DIST = \
  pygtk.spec \
  pygtk-2.0.pc.in \
  MAPPING \
  pango.defs \
  pango.override \
  examples/gobject/signal.py \
  examples/pygtk-demo/pygtk-demo.py \
  examples/pygtk-demo/demos/__init__.py \
  examples/pygtk-demo/demos/buttonbox.py \
  examples/pygtk-demo/demos/dnd.py \
  examples/pygtk-demo/demos/dndpixmap.py \
  examples/pygtk-demo/demos/menu.py \
  examples/pygtk-demo/demos/toolbar.py \
  examples/pygtk-demo/demos/treemodel.py \
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
  examples/pygtk-demo/pygtk-demo.py \
  examples/pygtk-demo/demos/__init__.py \
  examples/pygtk-demo/demos/dnd.py \
  examples/pygtk-demo/demos/dndpixmap.py \
  examples/pygtk-demo/demos/menu.py \
  examples/pygtk-demo/demos/treemodel.py \
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

