bin_SCRIPTS = pygtk-codegen-1.2

codegendir = $(pkgdatadir)/1.2/codegen
codegen_PYTHON = generate.py scmexpr.py

defsdir = $(pkgdatadir)/1.2/defs
defs_DATA = gtk.defs gtkbase.defs gtkcontainers.defs gtkdata.defs \
 gtkedit.defs gtkenums.defs gtklists.defs gtkmenus.defs gtkmisc.defs \
 gtkranges.defs gtkdnd.defs gtkgl.defs libglade.defs

EXTRA_DIST = $(defs_DATA) pygtk-codegen-1.2.in

IGNORE_FILES = gtk.ignore gtkgl.ignore libglade.ignore

all: $(IGNORE_FILES)

distclean-local:
	rm -f $(IGNORE_FILES)

gtk.ignore: $(srcdir)/../gtkmodule.c
	echo "# ignore list for gtkmodule" > gtk.ignore
	echo "# (automatically generated)" >> gtk.ignore
	sed -n 's/^.*_wrap_\([a-zA-Z_]*\),.*$$/\1/p' \
	  < $(srcdir)/../gtkmodule.c >> gtk.ignore

gtkgl.ignore: $(srcdir)/../gtkglmodule.c
	echo "# ignore list for gtkglmodule" > gtkgl.ignore
	echo "# (automatically generated)" >> gtkgl.ignore
	sed -n 's/^.*_wrap_\([a-zA-Z_]*\),.*$$/\1/p' \
	  < $(srcdir)/../gtkglmodule.c >> gtkgl.ignore

libglade.ignore: $(srcdir)/../libglademodule.c
	echo "# ignore list for libglademodule" > libglade.ignore
	echo "# (automatically generated)" >> libglade.ignore
	sed -n 's/^.*_wrap_\([a-zA-Z_]*\),.*$$/\1/p' \
	  < $(srcdir)/../libglademodule.c >> libglade.ignore
