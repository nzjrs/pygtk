AUTOMAKE_OPTIONS = 1.7

if BUILD_GTK
  GTK_SUBDIR = gtk
endif

SUBDIRS = codegen . $(GTK_SUBDIR) examples tests docs

PLATFORM_VERSION = 2.0

CLEANFILES = 
EXTRA_DIST = 				\
	ChangeLog.pre-2-0		\
	ChangeLog.pre-2-6		\
	config.h.win32 			\
	MANIFEST.in 			\
	MAPPING 			\
	pygtk.spec 			\
	pygtk.spec.in 			\
	pygtk-$(PLATFORM_VERSION).pc.in \
	pygtk_postinstall.py 		\
	PKG-INFO 			\
	PKG-INFO.in 			\
	setup.py 			\
	THREADS				\
	m4/jhflags.m4			\
	m4/python.m4

INCLUDES = 				\
	$(PYTHON_INCLUDES) 		\
	$(PYGOBJECT_CFLAGS)		\
	$(GLIB_CFLAGS)			\
	-I$(srcdir)/gtk

# pkg-config files
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA =
pkgconfig_DATA += pygtk-$(PLATFORM_VERSION).pc

# defs files
defsdir = $(pkgdatadir)/$(PLATFORM_VERSION)/defs
defs_DATA =

# python
pyexec_LTLIBRARIES =
noinst_PYTHON = ltihooks.py

# pygtk scripts
pkgpythondir = $(pyexecdir)/gtk-2.0

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
ATK_OVERRIDES = atk.override atkrectangle.override
CLEANFILES += atk.c
EXTRA_DIST += $(ATK_DEFS) $(ATK_OVERRIDES)
atk.c: $(ATK_DEFS) $(ATK_OVERRIDES)
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
pangocairo_la_CFLAGS = $(PYCAIRO_CFLAGS) $(PANGOCAIRO_CFLAGS)
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
	    --py_ssize_t-clean \
	    --register pango-types.defs \
	    --register atk-types.defs \
	    --register gtk/gtk-base-types.defs \
	    --override $*.override \
	    --prefix py$(*F) $*.defs) > $(*D)/gen-$(*F).c \
	&& cp $(*D)/gen-$(*F).c $(*D)/$(*F).c \
	&& rm -f $(*D)/gen-$(*F).c

snap:
	$(MAKE) dist distdir=$(PACKAGE)-SNAP-`date +"%Y%m%d"`

release-tag:
	@ROOT=`svn info --xml|egrep ^"<url>"|cut -d\> -f2|cut -d\< -f1`; \
	SVNVERSION=`echo $(VERSION)|sed s/\\\./_/g`;\
	  echo "* Tagging $(VERSION)"; \
          svn cp -m "Tag $(VERSION)" \
              $$ROOT \
              svn+ssh://svn.gnome.org/svn/pygtk/tags/PYGTK_$$SVNVERSION

doc-dist:
	mkdir -p pygtk/icons pygtk/cursors
	cp -r docs/html/*.html pygtk
	cp -r docs/html/*.sgml pygtk
	cp -r docs/html/*.devhelp pygtk
	cp -r $(PYGOBJECT_PYGDOCS)/style.css pygtk
	cp -r docs/icons/* pygtk/icons
	cp -r docs/cursors/* pygtk/cursors
	tar cfz $(PACKAGE)-docs.tar.gz pygtk
	rm -fr pygtk
