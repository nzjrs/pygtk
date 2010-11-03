
"""pygtk is now installed on your machine.

Local configuration files were successfully updated."""

import os, os.path, re, sys
import distutils.sysconfig
import distutils.file_util
import distutils.errors

pkgconfig_file = os.path.normpath(
    os.path.join(sys.prefix,
                 'lib/pkgconfig/pygtk-2.0.pc'))

prefix_pattern=re.compile("^prefix=.*")
exec_pattern=re.compile("^exec\s.*")
codegendir_pattern=re.compile("^codegendir=.*")
version_pattern=re.compile("Version: ([0-9]+\.[0-9]+\.[0-9]+)")

def replace_prefix(s):
    if prefix_pattern.match(s):
        s='prefix='+sys.prefix.replace("\\","/")+'\n'
    if exec_pattern.match(s):
        s=('exec '+sys.prefix+'\\python.exe '+
           '$codegendir/codegen.py \"$@\"\n').replace("\\","/")
    if codegendir_pattern.match(s):
        s=('codegendir='
           +distutils.sysconfig.get_python_lib().replace("\\","/")+
           '/gtk-2.0/codegen' + '\n')
    return s


if len(sys.argv) == 2:
    if sys.argv[1] == "-install":
        # fixup the pkgconfig file
        lines=open(pkgconfig_file).readlines()
        open(pkgconfig_file, 'w').writelines(map(replace_prefix,lines))
        print __doc__

