# require automake-1.5
AUTOMAKE_OPTIONS = 1.5

SUBDIRS = codegen gtk

CLEANFILES =
EXTRA_DIST =
pyexec_LTLIBRARIES =

defsdir = $(pkgdatadir)/2.0/defs
defs_DATA =

pkgincludedir = $(includedir)/pygtk-2.0
pkginclude_HEADERS = pygobject.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = pygtk-2.0.pc

INCLUDES = $(PYTHON_INCLUDES)
COMMONDEFS = pango-types.defs atk-types.defs gtk/gdk-types.defs \
	     gtk/gtk-types.defs

# gobject module
pyexec_LTLIBRARIES += gobjectmodule.la
gobjectmodule_la_LDFLAGS = -module -avoid-version \
 -export-symbols-regex initgobject
gobjectmodule_la_CFLAGS = $(GLIB_CFLAGS)
gobjectmodule_la_SOURCES = gobjectmodule.c pygobject.h
gobjectmodule_la_LIBADD = $(GLIB_LIBS)

# pango module
pyexec_LTLIBRARIES += pangomodule.la
pangomodule_la_LDFLAGS = -module -avoid-version -export-symbols-regex initpango
pangomodule_la_CFLAGS = $(PANGO_CFLAGS)
pangomodule_la_SOURCES = pangomodule.c
nodist_pangomodule_la_SOURCES = pango.c
pangomodule_la_LIBADD = $(PANGO_LIBS)
pango.c: $(COMMONDEFS) pango.override
CLEANFILES += pango.c
EXTRA_DIST += pango.override
defs_DATA += pango.defs pango-types.defs

# atk module
pyexec_LTLIBRARIES += atkmodule.la
atkmodule_la_LDFLAGS = -module -avoid-version -export-symbols-regex initatk
atkmodule_la_CFLAGS = $(ATK_CFLAGS)
atkmodule_la_SOURCES = atkmodule.c
nodist_atkmodule_la_SOURCES = atk.c
atkmodule_la_LIBADD = $(ATK_LIBS)
atk.c: $(COMMONDEFS) atk.override
CLEANFILES += atk.c
EXTRA_DIST += atk.override
defs_DATA += atk.defs atk-types.defs

.defs.c:
	(cd $(srcdir)\
	 && $(PYTHON) codegen/codegen.py \
	    --register pango-types.defs \
	    --register atk-types.defs \
	    --register gtk/gdk-types.defs \
	    --register gtk/gtk-types.defs \
	    --override $*.override \
	    --prefix py$* $*.defs) > gen-$*.c \
	&& cp gen-$*.c $*.c \
	&& rm -f gen-$*.c

noinst_PYTHON = ltihooks.py

EXTRA_DIST += \
  pygtk.spec \
  pygtk.spec.in \
  pygtk-2.0.pc.in \
  MAPPING \
  THREADS \
  build-docs.py \
  examples/atk/atk-demo.py \
  examples/gobject/signal.py \
  examples/gobject/properties.py \
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
