noinst_PYTHON = generate.py scmexpr.py

EXTRA_DIST = gtk.defs gtkbase.defs gtkcontainers.defs gtkdata.defs \
 gtkedit.defs gtkenums.defs gtklists.defs gtkmenus.defs gtkmisc.defs \
 gtkranges.defs gtkdnd.defs gtkgl.defs

all: gtk.ignore gtkgl.ignore

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
