#!/usr/bin/env python
# -*- Mode: Python; py-indent-offset: 4 -*-
#
# This litte script outputs the C doc comments to an XML format.
# So far it's only used by gtkmm (The C++ bindings). Murray Cumming.

import sys, os, string, re, getopt

import docextract

if __name__ == '__main__':
    try:
        opts, args = getopt.getopt(sys.argv[1:], "d:s:o:",
                                   ["source-dir="])
    except getopt.error, e:
        sys.stderr.write('docgen.py: %s\n' % e)
	sys.stderr.write(
	    'usage: docgen.py [-s /src/dir]\n')
        sys.exit(1)
    source_dirs = []
    for opt, arg in opts:
	if opt in ('-s', '--source-dir'):
	    source_dirs.append(arg)
    if len(args) != 0:
	sys.stderr.write(
	    'usage: docgen.py  [-s /src/dir]\n')
        sys.exit(1)

    docs = docextract.extract(source_dirs);

    # print d.docs
    
    if docs:
        for name, value in docs.items():
            print "<function name=\"" + name + "\">"

            print "<description>"
            #The value is a docextract.FunctionDoc
            print value.description
            print "</description>"

             # Loop through the parameters:
            print "<parameters>"
            for name, description in value.params:
                print "<parameter name=\"" + name + "\">"
                print "<parameter_description>" + description + "</parameter_description>"
                print "</parameter>"
            
            print "</parameters>"

            # Show the return-type:
            print "<return>" + value.ret + "</return>"
            
            print "</function>\n"
    

