# require automake-1.7
AUTOMAKE_OPTIONS = 1.7

if BUILD_GTK
  GTK_SUBDIR = gtk
endif

SUBDIRS = codegen gobject . $(GTK_SUBDIR) docs examples tests 

PLATFORM_VERSION = 2.0

CLEANFILES =
EXTRA_DIST = $(defs_DATA)
pyexec_LTLIBRARIES =

defsdir = $(pkgdatadir)/$(PLATFORM_VERSION)/defs
defs_DATA =

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = \
	pygobject-$(PLATFORM_VERSION).pc
	
if BUILD_GTK
pkgconfig_DATA += pygtk-$(PLATFORM_VERSION).pc
endif

INCLUDES = \
	$(PYTHON_INCLUDES) \
	$(GLIB_CFLAGS) \
	$(PANGO_CFLAGS) \
	$(ATK_CFLAGS) \
	-I$(top_srcdir)/gobject

COMMONDEFS = \
	atk-types.defs \
	pango-types.defs \
	gtk/gdk-types.defs \
	gtk/gtk-types.defs

pkgpythondir = $(pyexecdir)/gtk-2.0
pkgpyexecdir = $(pyexecdir)/gtk-2.0
pkgpyexec_LTLIBRARIES =

# this file is common to all pygtk versions.
pyexec_PYTHON = pygtk.py

# install pth file.
install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(pythondir)
	echo "gtk-$(PLATFORM_VERSION)" > $(DESTDIR)$(pyexecdir)/pygtk.pth
install-exec-local:
	$(mkinstalldirs) $(DESTDIR)$(pyexecdir)
	echo "gtk-$(PLATFORM_VERSION)" > $(DESTDIR)$(pyexecdir)/pygtk.pth
uninstall-local:
	rm -f $(DESTDIR)$(pythondir)/pygtk.pth $(DESTDIR)$(pyexecdir)/pygtk.pth


pkgpython_PYTHON = dsextras.py

if BUILD_ATK
pkgpyexec_LTLIBRARIES += atk.la
defs_DATA += atk.defs atk-types.defs
endif

if BUILD_PANGO
pkgpyexec_LTLIBRARIES += pango.la
defs_DATA += pango.defs pango-types.defs
endif

common_ldflags = -module -avoid-version
if PLATFORM_WIN32
common_ldflags += -no-undefined
endif

# pango module
pango_la_LDFLAGS = $(common_ldflags) -export-symbols-regex initpango
pango_la_LIBADD = $(PANGO_LIBS)
pango_la_SOURCES = pangomodule.c
nodist_pango_la_SOURCES = pango.c
pango.c: $(COMMONDEFS) pango.override
CLEANFILES += pango.c
EXTRA_DIST += pango.override

# atk module
atk_la_LDFLAGS = $(common_ldflags) -export-symbols-regex initatk
atk_la_LIBADD = $(ATK_LIBS)
atk_la_SOURCES = atkmodule.c
nodist_atk_la_SOURCES = atk.c
atk.c: $(COMMONDEFS) atk.override
CLEANFILES += atk.c
EXTRA_DIST += atk.override

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
  pygtk-$(PLATFORM_VERSION).pc.in \
  MAPPING \
  THREADS \
  config.h.win32 \
  makefile.msc \
  setup.py \
  dsextras.py \
  pygtk_postinstall.py \
  MANIFEST.in \
  PKG-INFO \
  PKG-INFO.in \
  ChangeLog.pre-2-0

snap:
	$(MAKE) dist distdir=$(PACKAGE)-SNAP-`date +"%Y%m%d"`
