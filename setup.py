#!/usr/bin/env python
#
# setup.py - distutils configuration for pygtk
#
# TODO:
# pygtk.spec(.in)
# Numeric support
# win32 testing
# install *.pyc for codegen
# GtkGL
"""Python Bindings for the GTK Widget Set.

PyGTK is a set of bindings for the GTK widget set. It provides an object oriented interface that is slightly higher level than the C one. It automatically does all the type casting and reference counting that you would have to do normally with the C API. You can find out more on the official homepage, http://www.daa.com.au/~james/pygtk/"""

from commands import getoutput
import fnmatch
import os
import string
import sys

from distutils.command.build import build
from distutils.command.build_ext import build_ext
from distutils.command.install_lib import install_lib
from distutils.core import setup
from distutils.extension import Extension

from codegen.override import Overrides
from codegen.defsparser import DefsParser
from codegen.codegen import register_types, write_source, FileOutput

MAJOR_VERSION = 1
MINOR_VERSION = 99
MICRO_VERSION = 14

VERSION = "%d.%d.%d" % (MAJOR_VERSION,
                        MINOR_VERSION,
                        MICRO_VERSION)

GOBJECT_REQUIRED  = '2.0.0'
ATK_REQUIRED      = '1.0.0'
PANGO_REQUIRED    = '1.0.0'
GTK_REQUIRED      = '2.0.0'
LIBGLADE_REQUIRED = '2.0.0'

PYGTK_SUFFIX = '2.0'
PYGTK_SUFFIX_LONG = 'gtk-' + PYGTK_SUFFIX

GLOBAL_INC = ['.', 'gtk']
GLOBAL_MACROS = [('VERSION', '"%s"' % VERSION),
                 ('PYGTK_MAJOR_VERSION', MAJOR_VERSION),
                 ('PYGTK_MINOR_VERSION', MINOR_VERSION),
                 ('PYGTK_MICRO_VERSION', MICRO_VERSION)]

DEFS_DIR = 'share/pygtk/%s/defs' % PYGTK_SUFFIX
CODEGEN_DIR = 'share/pygtk/%s/codegen' % PYGTK_SUFFIX
INCLUDE_DIR = 'include/pygtk-%s' % PYGTK_SUFFIX

str_version = sys.version[:3]
version = map(int, str_version.split('.'))
if version < [2, 2]:
    raise SystemExit, \
          "Python 2.2 or higher is required, %s found" % str_version
    
class PyGtkInstallLib(install_lib):
    local_outputs = []
    local_inputs = []
    def run(self):
        install_dir = self.install_dir
        self.prefix = os.sep.join(install_dir.split(os.sep)[:-4])
        
        # Install everything in site-packages/gtk-2.0
        self.install_dir = os.path.join(self.install_dir, PYGTK_SUFFIX_LONG)
        install_lib.run(self)

        # Except these three
        self.install_codegen(install_dir)
        self.install_pc(install_dir)
        self.install_pth(install_dir)
        self.install_pygtk(install_dir)

    def install_template(self, filename, install_dir):
        """Install template filename into target directory install_dir."""
        output_file = os.path.split(filename)[-1][:-3]
        exec_prefix = os.path.join(self.prefix, 'bin')
        includedir = os.path.join(self.prefix, 'include')
        datadir = os.path.join(self.prefix, 'share')

        template = open(filename).read()
        template = template.replace('@datadir@', datadir)
        template = template.replace('@exec_prefix@', exec_prefix)
        template = template.replace('@includedir@', includedir)
        template = template.replace('@prefix@', self.prefix)
        template = template.replace('@PYTHON@', sys.executable)
        template = template.replace('@VERSION@', VERSION)

        output = os.path.join(install_dir, output_file)
        self.mkpath(install_dir)
        open(output, 'w').write(template)
        self.local_inputs.append(filename)
        self.local_outputs.append(output)
        return output
    
    def install_codegen(self, install_dir):
        codegen = os.path.join('codegen', 'pygtk-codegen-2.0.in')
        file = self.install_template(codegen,
                                     os.path.join(self.prefix, 'bin'))
        os.chmod(file, 0755)
        
    def install_pc(self, install_dir):
        install_dir = os.path.join(self.prefix, 'lib', 'pkgconfig')
        self.install_template('pygtk-2.0.pc.in', install_dir)

    def install_pth(self, install_dir):
        """Write the pygtk.pth file"""
        file = os.path.join(install_dir, 'pygtk.pth')
        open(file, 'w').write(PYGTK_SUFFIX_LONG)
        self.local_outputs.append(file)
        self.local_inputs.append('pygtk.pth')
        
    def install_pygtk(self, install_dir):
        """install pygtk.py in the right place."""
        self.copy_file('pygtk.py', install_dir)
        self.local_outputs.append(os.path.join(install_dir, 'pygtk.py'))
        self.local_inputs.append('pygtk.py')

    def get_outputs(self):
        return install_lib.get_outputs(self) + self.local_outputs

    def get_inputs(self):
        return install_lib.get_inputs(self) + self.local_inputs

class PyGtkBuild(build):
    enable_threading = 0
PyGtkBuild.user_options.append(('enable-threading', None,
                                'enable threading support'))
    
class PyGtkBuildExt(build_ext):
    def build_extension(self, ext):
        # Generate eventual templates before building
        ext.generate()
        build_ext.build_extension(self, ext)
        
class PkgConfigExtension(Extension):
    can_build_ok = None
    def __init__(self, **kwargs):
        name = kwargs['pkc_name']
        kwargs['include_dirs'] = self.get_include_dirs(name) + GLOBAL_INC
        kwargs['define_macros'] = GLOBAL_MACROS
        kwargs['libraries'] = self.get_libraries(name)
        kwargs['library_dirs'] = self.get_library_dirs(name) 
        self.pkc_name = kwargs['pkc_name']
        self.pkc_version = kwargs['pkc_version']
        del kwargs['pkc_name'], kwargs['pkc_version']
        Extension.__init__(self, **kwargs)

    def get_include_dirs(self, name):
        output = getoutput('pkg-config --cflags-only-I %s' % name)
        return output.replace('-I', '').split()

    def get_libraries(self, name):
        output = getoutput('pkg-config --libs-only-l %s' % name)
        return output.replace('-l', '').split()
    
    def get_library_dirs(self, name):
        output = getoutput('pkg-config --libs-only-L %s' % name)
        return output.replace('-L', '').split()

    def can_build(self):
        """If the pkg-config version found is good enough"""
        if self.can_build_ok != None: 
            return self.can_build_ok

        retval = os.system('pkg-config --exists %s' % self.pkc_name)
        if retval:
            print "* Could not find %s." % self.pkc_name
            self.can_build_ok = 0
            return 0

        orig_version = getoutput('pkg-config --modversion %s' % self.pkc_name)
        version = map(int, orig_version.split('.'))
        pkc_version = map(int, self.pkc_version.split('.'))
                      
        if version >= pkc_version:
            self.can_build_ok = 1
            return 1
        else:
            print "Warning: Too old version of %s" % self.pkc_name
            print "         Need %s, but %s is installed" % \
                  (self.pkc_version, orig_version)
            self.can_build_ok = 0
            return 0
        
    def generate(self):
        pass
            
class Template:
    def __init__(self, override, output, defs, prefix, register=[]):
        self.override = override
        self.defs = defs
        self.register = register
        self.output = output
        self.prefix = prefix

    def check_dates(self):
        if not os.path.exists(self.output):
            return 0

        files = self.register[:]
        files.append(self.override)
#        files.append('setup.py')
        files.append(self.defs)
        
        newest = 0
        for file in files:
            test = os.stat(file)[8]
            if test > newest:
                newest = test
                
        if newest < os.stat(self.output)[8]:
            return 1
        return 0
    
    def generate(self):
        if self.check_dates():
            return

        for item in self.register:
            dp = DefsParser(item)
            dp.startParsing()
            register_types(dp)

        dp = DefsParser(self.defs)
        dp.startParsing()
        register_types(dp)
        
        fd = open(self.output, 'w')
        write_source(dp,
                     Overrides(self.override),
                     self.prefix,
                     FileOutput(fd, self.output))
        fd.close()
        
class TemplateExtension(PkgConfigExtension):
    def __init__(self, **kwargs):
        name = kwargs['name']
        defs = kwargs['defs']
        output = defs[:-5] + '.c'
        override = kwargs['override']
        self.templates = []
        self.templates.append(Template(override, output, defs, 'py' + name,
                                       kwargs['register']))
        
        del kwargs['register'], kwargs['override'], kwargs['defs']

        if kwargs.has_key('output'):
            kwargs['name'] = kwargs['output']
            del kwargs['output']
        
        PkgConfigExtension.__init__(self, **kwargs)
        
    def generate(self):
        map(lambda x: x.generate(), self.templates)
        
def list_files(dir):
    """List all files in a dir, with filename match support:
    for example: glade/*.glade will return all files in the glade directory
    that matches *.glade. It also looks up the full path"""
    if dir.find(os.sep) != -1:
        parts = dir.split(os.sep)
        dir = string.join(parts[:-1], os.sep)
        pattern = parts[-1]
    else:
        pattern = dir
        dir = '.'

    dir = os.path.abspath(dir)
    retval = []
    for file in os.listdir(dir):
        if fnmatch.fnmatch(file, pattern):
            retval.append(os.path.join(dir, file))
    return retval

def have_pkgconfig():
    """Checks for the existence of pkg-config"""
    if os.system('pkg-config 2> /dev/null') == 256:
        return 1
    
# GObject
gobject = PkgConfigExtension(name='gobject', pkc_name='gobject-2.0',
                             pkc_version=GOBJECT_REQUIRED,
                             sources=['pygboxed.c',
                                      'pygobject.c',
                                      'pygtype.c',
                                      'gobjectmodule.c'])
# Atk
atk = TemplateExtension(name='atk', pkc_name='atk',
                        pkc_version=ATK_REQUIRED,
                        sources=['atkmodule.c', 'atk.c'],
                        register=['atk-types.defs'],
                        override='atk.override',
                        defs='atk.defs')
# Pango
pango = TemplateExtension(name='pango', pkc_name='pango',
                          pkc_version=PANGO_REQUIRED,
                          sources=['pango.c', 'pangomodule.c'],
                          register=['pango-types.defs'],
                          override='pango.override',
                          defs='pango.defs')
# Gdk (template only)
gdk_template = Template('gtk/gdk.override', 'gtk/gdk.c',
                        defs='gtk/gdk.defs', prefix='pygdk',
                        register=['atk-types.defs',
                                  'pango-types.defs',
                                  'gtk/gdk-types.defs'])
# Gtk+         
gtk = TemplateExtension(name='gtk', pkc_name='gtk+-2.0',
                        pkc_version=GTK_REQUIRED,
                        output='gtk._gtk',
                        sources=['gtk/gtkmodule.c',
                                 'gtk/gtkobject-support.c',
                                 'gtk/gtk-types.c',
                                 'gtk/pygtktreemodel.c',
                                 'gtk/pygtkcellrenderer.c',
                                 'gtk/gdk.c',
                                 'gtk/gtk.c'],
                        register=['pango-types.defs',
                                  'gtk/gdk-types.defs',
                                  'gtk/gtk-types.defs'],
                        override='gtk/gtk.override',
                        defs='gtk/gtk.defs')
gtk.templates.append(gdk_template)

# Libglade
libglade = TemplateExtension(name='libglade', pkc_name='libglade-2.0',
                             pkc_version=LIBGLADE_REQUIRED,
                             output='gtk.glade',
                             defs='gtk/libglade.defs',
                             sources=['gtk/libglademodule.c',
                                      'gtk/libglade.c'],
                             register=['gtk/gtk-types.defs',
                                       'gtk/libglade.defs'],
                             override='gtk/libglade.override')


data_files = []
ext_modules = []
py_modules = []

if not have_pkgconfig():
    print "Error, could not find pkg-config"
    raise SystemExit
    
if gobject.can_build():
    ext_modules.append(gobject)
    data_files.append((INCLUDE_DIR, ('pygobject.h',)))
    data_files.append((CODEGEN_DIR, list_files('codegen/*.py')))
else:
    print
    print 'ERROR: Nothing to do, gobject could not be found and is essential.'
    raise SystemExit
if atk.can_build():
    ext_modules.append(atk)
    data_files.append((DEFS_DIR, ('atk.defs', 'atk-types.defs')))
if pango.can_build():
    ext_modules.append(pango)
    data_files.append((DEFS_DIR, ('pango.defs', 'pango-types.defs')))
if gtk.can_build():
    ext_modules.append(gtk)
    data_files.append((INCLUDE_DIR, ('gtk/pygtk.h',)))
    data_files.append((DEFS_DIR, ('gtk/gdk.defs', 'gtk/gdk-types.defs',
                                  'gtk/gtk.defs', 'gtk/gtk-types.defs',
                                  'gtk/gtk-extrafuncs.defs')))
    py_modules += ['gtk.compat', 'gtk.keysyms']
if libglade.can_build():
    ext_modules.append(libglade)
    data_files.append((DEFS_DIR, ('gtk/libglade.defs',)))

if '--enable-threading' in sys.argv:
    try:
        import thread
    except ImportError:
        print "Warning: Could not import thread module, disabling threading"
    else:
        GLOBAL_MACROS.append(('ENABLE_PYGTK_THREADING', 1))

        name = 'gthread-2.0'
        for module in ext_modules:
            raw = getoutput('pkg-config --libs-only-l %s' % name)
            module.extra_link_args += raw.split()
            raw = getoutput('pkg-config --cflags-only-I %s' % name)
            module.extra_compile_args.append(raw)

doclines = __doc__.split("\n")

setup(name="pygtk",
      url='http://www.daa.com.au/~james/pygtk/',
      version=VERSION,
      license='LGPL',
      platforms=['yes'],
      maintainer="James Henstridge",
      maintainer_email="james@daa.com.au",
      description = doclines[0],
      long_description = "\n".join(doclines[2:]),
      py_modules=py_modules,
      ext_modules=ext_modules,
      data_files=data_files,
      cmdclass={'install_lib': PyGtkInstallLib,
                'build_ext': PyGtkBuildExt,
                'build': PyGtkBuild})
