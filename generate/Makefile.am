noinst_PYTHON = generate.py scmexpr.py

EXTRA_DIST = gtk.defs gtkbase.defs gtkcontainers.defs gtkdata.defs \
 gtkedit.defs gtkenums.defs gtklists.defs gtkmenus.defs gtkmisc.defs \
 gtkranges.defs gtkdnd.defs gtk.ignore

all: gtk.ignore

gtk.ignore: ../gtkmodule.c
	echo "# ignore list for gtkmodule" > gtk.ignore
	echo "# (automatically generated)" >> gtk.ignore
	sed -n 's/^.*_wrap_\([a-zA-Z_]*\),.*$$/\1/p' \
	  < ../gtkmodule.c >> gtk.ignore