import sys

srcdir = '.'
if len(sys.argv) > 1:
    [srcdir] = sys.argv[1:]

# add the generate directory to beginning of path ...
sys.path.insert(0, srcdir + '/generate')

import generate

p = generate.FilteringParser(input=srcdir + '/generate/gtk.defs',
			     prefix='gtkmodule',
			     typeprefix='&')
p.addExcludeFile('generate/gtk.ignore')
p.addExcludeGlob('*_interp')
p.addExcludeGlob('*_visual')
p.startParsing()

p = generate.FilteringParser(input=srcdir + '/generate/gtkgl.defs',
			     prefix='gtkgl',
			     typeprefix='&')
p.addExcludeFile('generate/gtkgl.ignore')
p.startParsing()

p = generate.FilteringParser(input=srcdir + '/generate/libglade.defs',
			     prefix='libglade',
			     typeprefix='&')
p.addExcludeFile('generate/libglade.ignore')
p.startParsing()
