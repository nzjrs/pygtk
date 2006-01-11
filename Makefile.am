AUTOMAKE_OPTIONS = 1.7

if BUILD_GTK
  GTK_SUBDIR = gtk
endif

SUBDIRS = codegen

if BUILD_GOBJECT
  SUBDIRS += gobject
endif

SUBDIRS += . $(GTK_SUBDIR) docs examples tests 

PLATFORM_VERSION = 2.0

CLEANFILES = 
EXTRA_DIST = 				\
	ChangeLog.pre-2-0		\
	ChangeLog.pre-2-6		\
	config.h.win32 			\
	dsextras.py 			\
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
INCLUDES = $(PYTHON_INCLUDES) $(GLIB_CFLAGS)
if BUILD_GOBJECT
  INCLUDES += -I$(top_srcdir)/gobject
else
  INCLUDES += -I$(PYGOBJECT_INCLUDEDIR)
endif

# pkg-config files
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA =
if BUILD_GOBJECT
pkgconfig_DATA += pygobject-$(PLATFORM_VERSION).pc
endif
if BUILD_GTK
pkgconfig_DATA += pygtk-$(PLATFORM_VERSION).pc
endif

# defs files
defsdir = $(pkgdatadir)/$(PLATFORM_VERSION)/defs
defs_DATA =

# python
if BUILD_GOBJECT
pyexec_PYTHON = pygtk.py
endif
pyexec_LTLIBRARIES =
noinst_PYTHON = ltihooks.py

# pygtk scripts
pkgpythondir = $(pyexecdir)/gtk-2.0
if BUILD_GOBJECT
pkgpython_PYTHON = dsextras.py
endif

# pygtk extension modules
pkgpyexecdir = $(pyexecdir)/gtk-2.0
pkgpyexec_LTLIBRARIES =

# linker flags
common_ldflags = -module -avoid-version
if PLATFORM_WIN32
common_ldflags += -no-undefined
endif

# atk module
ATK_DEFS = atk.defs atk-types.defs
CLEANFILES += atk.c
EXTRA_DIST += $(ATK_DEFS) atk.override
atk.c: $(ATK_DEFS) atk.override
atk_la_CFLAGS = $(ATK_CFLAGS)
atk_la_LIBADD = $(ATK_LIBS)
atk_la_LDFLAGS = $(common_ldflags) -export-symbols-regex initatk
atk_la_SOURCES = atkmodule.c
nodist_atk_la_SOURCES = atk.c
if BUILD_ATK
pkgpyexec_LTLIBRARIES += atk.la
defs_DATA += $(ATK_DEFS)
endif

# pango module
PANGO_DEFS = pango.defs pango-types.defs
CLEANFILES += pango.c
EXTRA_DIST += $(PANGO_DEFS) pango.override
pango.c: $(PANGO_DEFS) pango.override
pango_la_CFLAGS = $(PANGO_CFLAGS)
pango_la_LIBADD = $(PANGO_LIBS)
pango_la_LDFLAGS = $(common_ldflags) -export-symbols-regex initpango
pango_la_SOURCES = pangomodule.c
nodist_pango_la_SOURCES = pango.c
if BUILD_PANGO
pkgpyexec_LTLIBRARIES += pango.la
defs_DATA += $(PANGO_DEFS)
endif

# pangocairo module
PANGOCAIRO_DEFS = pangocairo.defs pangocairo.override
CLEANFILES += pangocairo.c
EXTRA_DIST += $(PANGOCAIRO_DEFS) pangocairo.override
pangocairo.c: $(PANGOCAIRO_DEFS) pangocairo.override
pangocairo_la_CFLAGS = $(PANGOCAIRO_CFLAGS)
pangocairo_la_LDFLAGS = $(common_ldflags) -export-symbols-regex initpangocairo
pangocairo_la_LIBADD = $(PANGOCAIRO_LIBS)
pangocairo_la_SOURCES = pangocairomodule.c
nodist_pangocairo_la_SOURCES = pangocairo.c
if BUILD_PANGOCAIRO
pkgpyexec_LTLIBRARIES += pangocairo.la
defs_DATA += $(PANGOCAIRO_DEFS)
endif

# code generator
.defs.c:
	(cd $(srcdir)\
	 && $(PYTHON) codegen/codegen.py \
	    --register pango-types.defs \
	    --register atk-types.defs \
	    --override $*.override \
	    --prefix py$* $*.defs) > gen-$*.c \
	&& cp gen-$*.c $*.c \
	&& rm -f gen-$*.c

if BUILD_GOBJECT
# install pth file.
install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(pythondir)
	echo "gtk-$(PLATFORM_VERSION)" > $(DESTDIR)$(pyexecdir)/pygtk.pth
install-exec-local:
	$(mkinstalldirs) $(DESTDIR)$(pyexecdir)
	echo "gtk-$(PLATFORM_VERSION)" > $(DESTDIR)$(pyexecdir)/pygtk.pth
uninstall-local:
	rm -f $(DESTDIR)$(pythondir)/pygtk.pth $(DESTDIR)$(pyexecdir)/pygtk.pth
endif

snap:
	$(MAKE) dist distdir=$(PACKAGE)-SNAP-`date +"%Y%m%d"`
