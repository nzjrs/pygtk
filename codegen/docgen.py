import sys, string, getopt
import parser
import docextract

class DocWriter:
    def __init__(self, defs_file, source_dirs):
	# parse the defs file
	self.parser = parser.DefsParser(defs_file)
	self.parser.startParsing()

	# extract documentation from C source code
	self.docs = docextract.extract(source_dirs)

    def output_docs(self, output_prefix):
	obj_defs = self.parser.objects[:]
	obj_defs.sort(lambda a, b: cmp(a.c_name, b.c_name))
	for obj_def in obj_defs:
	    fp = open(self.create_filename(obj_def, output_prefix), 'w')
	    self.output_object_docs(obj_def, fp)
	    fp.close()
    def output_object_docs(self, obj_def, fp=sys.stdout):
	self.write_object_header(obj_def.c_name, fp)

	# construct the inheritence heirachy ...
	ancestry = [ (obj_def.c_name, obj_def.implements) ]
        try:
            parent = obj_def.parent
            while parent != (None, None):
                if parent[1] + parent[0] == 'GObject':
                    ancestry.append(('GObject', []))
                    parent = (None, None)
                else:
                    parent_def = self.parser.find_object(parent[1] + parent[0])
                    ancestry.append((parent_def.c_name, parent_def.implements))
                    parent = parent_def.parent
        except ValueError:
            pass
        self.write_heirachy(obj_def.c_name, ancestry, fp)

    # override the following to create a more complex output format
    def create_filename(self, obj_def, output_prefix):
	'''Create output filename for this particular object'''
	return output_prefix + '-' + string.lower(obj_def.c_name) + '.txt'

    def write_object_header(self, obj_name, fp):
	fp.write('Class %s\n' % obj_name)
	fp.write('======%s\n\n' % ('=' * len(obj_name)))
    def write_heirachy(self, obj_name, ancestry, fp):
        indent = ''
        for name, interfaces in ancestry:
            fp.write(indent + '+-- ' + name)
            if interfaces:
                fp.write(' (implements ')
                fp.write(string.join(interfaces, ', '))
                fp.write(')\n')
            else:
                fp.write('\n')
            indent = indent + '  '
        fp.write('\n')

if __name__ == '__main__':
    opts, args = getopt.getopt(sys.argv[1:], "d:s:o:",
			       ["defs-file=", "source-dir=", "output-prefix="])
    defs_file = None
    source_dirs = []
    output_prefix = 'docs'
    for opt, arg in opts:
	if opt in ('-d', '--defs-file'):
	    defs_file = arg
	elif opt in ('-s', '--source-dir'):
	    source_dirs.append(arg)
	elif opt in ('-o', '--output-prefix'):
	    output_prefix = arg
    if len(args) != 0 or not defs_file:
	sys.stderr.write(
	    'usage: docgen.py -d file.defs [-s /src/dir] [-o output-prefix]')

    d = DocWriter(defs_file, source_dirs)
    d.output_docs(output_prefix)
