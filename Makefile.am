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
defs_DATA = 				\
	$(top_srcdir)/atk.defs		\
	$(top_srcdir)/atk-types.defs 	\
	$(top_srcdir)/pango.defs	\
	$(top_srcdir)/pango-types.defs	\
	$(top_srcdir)/pangocairo.defs

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = \
	pygobject-$(PLATFORM_VERSION).pc

if BUILD_GTK
pkgconfig_DATA += pygtk-$(PLATFORM_VERSION).pc
endif

INCLUDES = 			\
	$(PYTHON_INCLUDES) 	\
	$(GLIB_CFLAGS) 		\
	$(PANGO_CFLAGS) 	\
	$(ATK_CFLAGS) 		\
	-I$(top_srcdir)/gobject

COMMONDEFS = 					\
	$(top_srcdir)/atk-types.defs 		\
	$(top_srcdir)/pango-types.defs 		\
	$(top_srcdir)/pangocairo.defs 		\
	$(top_srcdir)/gtk/gdk-types.defs 	\
	$(top_srcdir)/gtk/gtk-types.defs

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
endif
if BUILD_PANGO
pkgpyexec_LTLIBRARIES += pango.la
endif
if BUILD_PANGOCAIRO
pkgpyexec_LTLIBRARIES += pangocairo.la
endif

common_ldflags = -module -avoid-version
if PLATFORM_WIN32
common_ldflags += -no-undefined
endif

# atk module
atk_la_LDFLAGS = $(common_ldflags) -export-symbols-regex initatk
atk_la_LIBADD = $(ATK_LIBS)
atk_la_SOURCES = atkmodule.c
nodist_atk_la_SOURCES = atk.c
atk.c: $(COMMONDEFS) atk.override
CLEANFILES += atk.c
EXTRA_DIST += atk.override

# pango module
pango_la_LDFLAGS = $(common_ldflags) -export-symbols-regex initpango
pango_la_LIBADD = $(PANGO_LIBS)
pango_la_SOURCES = pangomodule.c
nodist_pango_la_SOURCES = pango.c
pango.c: $(COMMONDEFS) pango.override
CLEANFILES += pango.c
EXTRA_DIST += pango.override

# pangocairo module
pangocairo_la_LDFLAGS = $(common_ldflags) -export-symbols-regex initpangocairo
pangocairo_la_LIBADD = $(PANGOCAIRO_LIBS)
pangocairo_la_CFLAGS = $(PANGOCAIRO_CFLAGS)
pangocairo_la_SOURCES = pangocairomodule.c
nodist_pangocairo_la_SOURCES = pangocairo.c
pangocairo.c: $(COMMONDEFS) pangocairo.override
CLEANFILES += pangocairo.c
EXTRA_DIST += pangocairo.override

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

EXTRA_DIST += 				\
	ChangeLog.pre-2-0		\
	ChangeLog.pre-2-6		\
	config.h.win32 			\
	dsextras.py 			\
	makefile.msc 			\
	MANIFEST.in 			\
	MAPPING 			\
	pygtk.spec 			\
	pygtk.spec.in 			\
	pygtk-$(PLATFORM_VERSION).pc.in \
	pygtk_postinstall.py 		\
	PKG-INFO 			\
	PKG-INFO.in 			\
	setup.py 			\
	THREADS

snap:
	$(MAKE) dist distdir=$(PACKAGE)-SNAP-`date +"%Y%m%d"`
