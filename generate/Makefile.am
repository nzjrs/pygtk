bin_SCRIPTS = pygtk-codegen-1.2

codegendir = $(pkgdatadir)/1.2/codegen
codegen_PYTHON = generate.py scmexpr.py

defsdir = $(pkgdatadir)/1.2/defs
defs_DATA = gtk.defs gtkbase.defs gtkcontainers.defs gtkdata.defs \
 gtkedit.defs gtkenums.defs gtklists.defs gtkmenus.defs gtkmisc.defs \
 gtkranges.defs gtkdnd.defs gtkgl.defs libglade.defs

EXTRA_DIST = $(defs_DATA) pygtk-codegen-1.2.in

all: gtk.ignore gtkgl.ignore libglade.ignore

gtk.ignore: ../gtkmodule.c
	echo "# ignore list for gtkmodule" > $(srcdir)/gtk.ignore
	echo "# (automatically generated)" >> $(srcdir)/gtk.ignore
	sed -n 's/^.*_wrap_\([a-zA-Z_]*\),.*$$/\1/p' \
	  < $(srcdir)/../gtkmodule.c >> $(srcdir)/gtk.ignore

gtkgl.ignore: ../gtkglmodule.c
	echo "# ignore list for gtkglmodule" > $(srcdir)/gtkgl.ignore
	echo "# (automatically generated)" >> $(srcdir)/gtkgl.ignore
	sed -n 's/^.*_wrap_\([a-zA-Z_]*\),.*$$/\1/p' \
	  < $(srcdir)/../gtkglmodule.c >> $(srcdir)/gtkgl.ignore

libglade.ignore: ../libglademodule.c
	echo "# ignore list for libglademodule" > $(srcdir)/libglade.ignore
	echo "# (automatically generated)" >> $(srcdir)/libglade.ignore
	sed -n 's/^.*_wrap_\([a-zA-Z_]*\),.*$$/\1/p' \
	  < $(srcdir)/../libglademodule.c >> $(srcdir)/libglade.ignore

