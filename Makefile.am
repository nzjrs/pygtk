# require automake-1.7
AUTOMAKE_OPTIONS = 1.7

SUBDIRS = codegen gtk docs

CLEANFILES =
EXTRA_DIST = $(defs_DATA)
pyexec_LTLIBRARIES =

defsdir = $(pkgdatadir)/2.0/defs
defs_DATA =

pkgincludedir = $(includedir)/pygtk-2.0
pkginclude_HEADERS = pygobject.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = pygtk-2.0.pc

INCLUDES = $(PYTHON_INCLUDES) $(GLIB_CFLAGS) $(PANGO_CFLAGS) $(ATK_CFLAGS)
COMMONDEFS = pango-types.defs atk-types.defs gtk/gdk-types.defs \
	     gtk/gtk-types.defs

pkgpythondir = $(pythondir)/gtk-2.0
pkgpyexecdir = $(pyexecdir)/gtk-2.0

# this file is common to all pygtk versions.
python_PYTHON = pygtk.py

# install pth file.
install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(pythondir)
	echo "gtk-2.0" > $(DESTDIR)$(pythondir)/pygtk.pth
install-exec-local:
	$(mkinstalldirs) $(DESTDIR)$(pyexecdir)
	echo "gtk-2.0" > $(DESTDIR)$(pyexecdir)/pygtk.pth
uninstall-local:
	rm -f $(DESTDIR)$(pythondir)/pygtk.pth $(DESTDIR)$(pyexecdir)/pygtk.pth

pkgpyexec_LTLIBRARIES = \
  gobject.la \
  pango.la \
  atk.la

common_ldflags = -module -avoid-version
if PLATFORM_WIN32
common_ldflags += -no-undefined
endif

# gobject module
gobject_la_LDFLAGS = $(common_ldflags) -export-symbols-regex initgobject
gobject_la_LIBADD = $(GLIB_LIBS)
gobject_la_SOURCES = \
  pygobject.h pygobject-private.h \
  gobjectmodule.c pygtype.c pygobject.c pygboxed.c

# pango module
pango_la_LDFLAGS = $(common_ldflags) -export-symbols-regex initpango
pango_la_LIBADD = $(PANGO_LIBS)
pango_la_SOURCES = pangomodule.c
nodist_pango_la_SOURCES = pango.c
pango.c: $(COMMONDEFS) pango.override
CLEANFILES += pango.c
EXTRA_DIST += pango.override
defs_DATA += pango.defs pango-types.defs

# atk module
atk_la_LDFLAGS = $(common_ldflags) -export-symbols-regex initatk
atk_la_LIBADD = $(ATK_LIBS)
atk_la_SOURCES = atkmodule.c
nodist_atk_la_SOURCES = atk.c
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
  config.h.win32 \
  makefile.msc \
  setup.py \
  dsextras.py \
  MANIFEST.in \
  PKG-INFO \
  PKG-INFO.in \
  ChangeLog.pre-2-0 \
  examples/atk/atk-demo.py \
  examples/gobject/signal.py \
  examples/gobject/properties.py \
  examples/pango/utf8-demo.py \
  examples/pygtk-demo/pygtk-demo.py \
  examples/pygtk-demo/demos/__init__.py \
  examples/pygtk-demo/demos/appwindow.py \
  examples/pygtk-demo/demos/buttonbox.py \
  examples/pygtk-demo/demos/colorsel.py \
  examples/pygtk-demo/demos/dialogs.py \
  examples/pygtk-demo/demos/dndpixmap.py \
  examples/pygtk-demo/demos/dnd.py \
  examples/pygtk-demo/demos/editable_cells.py \
  examples/pygtk-demo/demos/itemfactory.py \
  examples/pygtk-demo/demos/list_store.py \
  examples/pygtk-demo/demos/menu.py \
  examples/pygtk-demo/demos/panes.py \
  examples/pygtk-demo/demos/sizegroup.py \
  examples/pygtk-demo/demos/stock_browser.py \
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
  examples/simple/hello.py \
  examples/simple/simple.py \
  examples/simple/tooltip.py \
  examples/simple/scribble.py \
  examples/simple/dnd.py \
  examples/simple/dndpixmap.py \
  examples/simple/README \
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
  examples/glade/glade-demo.py \
  examples/glade/test.glade \
  examples/gl/README \
  examples/gl/cone.py \
  examples/gl/dots.py \
  examples/gl/gears.py

snap:
	$(MAKE) dist distdir=$(PACKAGE)-SNAP-`date +"%Y%m%d"`
