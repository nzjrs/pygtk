# require automake-1.7
AUTOMAKE_OPTIONS = 1.7

SUBDIRS = codegen gobject gtk docs examples tests 

CLEANFILES =
EXTRA_DIST = $(defs_DATA)
pyexec_LTLIBRARIES =

defsdir = $(pkgdatadir)/2.0/defs
defs_DATA =

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = pygtk-2.0.pc

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


pkgpython_PYTHON = dsextras.py

pkgpyexec_LTLIBRARIES = atk.la pango.la 

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
  ChangeLog.pre-2-0

snap:
	$(MAKE) dist distdir=$(PACKAGE)-SNAP-`date +"%Y%m%d"`
