import sys
# add the generate directory to beginning of path ...
sys.path.insert(0, './generate')

import generate

p = generate.FilteringParser(input='generate/gtk.defs',
			     prefix='gtkmodule',
			     typeprefix='&')
p.addExcludeFile('generate/gtk.ignore')
p.addExcludeGlob('*_interp')
p.addExcludeGlob('*_visual')
p.startParsing()

