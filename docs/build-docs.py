import sys, os

sys.path.insert(0, '../codegen')

if not os.path.exists('xml'):
    os.mkdir('xml')

import docgen

dg = docgen.DocbookDocWriter(use_xml=1)

# extract documentation from glib
sys.stderr.write('extracting documentation from C source ...\n')
dg.add_sourcedirs([
    '../../../glib',
    '../../../pango',
    '../../../atk',
    '../../../gtk+',
    '../../../libglade',
])

sys.stderr.write('extracting documentation from tmpl files ...\n')
dg.add_tmpldirs([
    '../../../glib/docs/reference/glib/tmpl',
    '../../../glib/docs/reference/gobject/tmpl',
    '../../../pango/docs/tmpl',
    '../../../atk/docs/tmpl',
    '../../../gtk+/docs/reference/gdk-pixbuf/tmpl',
    '../../../gtk+/docs/reference/gdk/tmpl',
    '../../../gtk+/docs/reference/gtk/tmpl',
    '../../../libglade/doc/tmpl',
])
    

# gojbect docs ...
sys.stderr.write('loading gobject defs ...')
sys.stderr.flush()
dg.add_docs('gobject.defs', None, 'gobject')
sys.stderr.write('done.\n')

# pango docs ...
sys.stderr.write('loading pango defs ...')
sys.stderr.flush()
dg.add_docs('../pango.defs', '../pango.override', 'pango')
sys.stderr.write('done.\n')

# atk docs ...
sys.stderr.write('loading atk defs ...')
sys.stderr.flush()
dg.add_docs('../atk.defs', '../atk.override', 'atk')
sys.stderr.write('done.\n')

# gdk docs ...
sys.stderr.write('loading gdk defs ...')
sys.stderr.flush()
dg.add_docs('../gtk/gdk.defs', '../gtk/gdk.override', 'gtk.gdk')
sys.stderr.write('done.\n')

# gtk docs ...
sys.stderr.write('loading gtk defs ...')
sys.stderr.flush()
dg.add_docs('../gtk/gtk.defs', '../gtk/gtk.override', 'gtk')
sys.stderr.write('done.\n')

# libglade docs
sys.stderr.write('loading libglade defs ...')
sys.stderr.flush()
dg.add_docs('../gtk/libglade.defs', '../gtk/libglade.override', 'gtk.glade')
sys.stderr.write('done.\n')

# write docs into the docs/ subdir with pygtk prefix
sys.stderr.write('writing xml ...')
sys.stderr.flush()
dg.output_docs('xml/pygtk')
sys.stderr.write('done.\n')
