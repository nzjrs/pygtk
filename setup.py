#!/usr/bin/env python
#
# setup.py - distutils configuration for pygtk
#
# TODO:
# pygtk.spec(.in)
# install *.pyc for codegen
#
"""Python Bindings for the GTK Widget Set.

PyGTK is a set of bindings for the GTK widget set. It provides an object 
oriented interface that is slightly higher level than the C one. It 
automatically does all the type casting and reference counting that you 
would have to do normally with the C API. You can find out more on the 
official homepage, http://www.daa.com.au/~james/pygtk/"""

from distutils.command.build import build
from distutils.core import setup
import glob
import os
import sys

from dsextras import get_m4_define, getoutput, have_pkgconfig, list_files, \
     GLOBAL_INC, GLOBAL_MACROS, InstallLib, InstallData, BuildExt, \
     PkgConfigExtension, Template, TemplateExtension

if sys.platform != "win32":
    raise SystemExit, "Building PyGTK through distutils is not supported on Linux, please use configure to build PyGTK."

MAJOR_VERSION = int(get_m4_define('pygtk_major_version'))
MINOR_VERSION = int(get_m4_define('pygtk_minor_version'))
MICRO_VERSION = int(get_m4_define('pygtk_micro_version'))

VERSION = "%d.%d.%d" % (MAJOR_VERSION,
                        MINOR_VERSION,
                        MICRO_VERSION)

GOBJECT_REQUIRED  = get_m4_define('glib_required_version')
ATK_REQUIRED      = get_m4_define('atk_required_version')
PANGO_REQUIRED    = get_m4_define('pango_required_version')
GTK_REQUIRED      = get_m4_define('gtk_required_version')
LIBGLADE_REQUIRED = get_m4_define('libglade_required_version')

PYGTK_SUFFIX = '2.0'
PYGTK_SUFFIX_LONG = 'gtk-' + PYGTK_SUFFIX

GLOBAL_INC += ['.', 'gtk', 'gobject']
GLOBAL_MACROS += [('PYGTK_MAJOR_VERSION', MAJOR_VERSION),
                  ('PYGTK_MINOR_VERSION', MINOR_VERSION),
                  ('PYGTK_MICRO_VERSION', MICRO_VERSION)]

if sys.platform == 'win32':
    GLOBAL_MACROS.append(('VERSION', '\\\"%s\\\"' % VERSION))
    GLOBAL_MACROS.append(('PLATFORM_WIN32',1))
    GLOBAL_MACROS.append(('HAVE_BIND_TEXTDOMAIN_CODESET',1))
else:
    GLOBAL_MACROS.append(('VERSION', '"%s"' % VERSION))

DEFS_DIR    = os.path.join('share', 'pygtk', PYGTK_SUFFIX, 'defs')
CODEGEN_DIR = os.path.join('share', 'pygtk', PYGTK_SUFFIX, 'codegen')
INCLUDE_DIR = os.path.join('include', 'pygtk-%s' % PYGTK_SUFFIX)

str_version = sys.version[:3]
version = map(int, str_version.split('.'))
if version < [2, 3]:
    raise SystemExit, \
          "Python 2.3 or higher is required, %s found" % str_version

class PyGtkInstallLib(InstallLib):
    def run(self):

        # Install pygtk.pth, pygtk.py[c] and templates
        self.install_pth()
        self.install_pygtk()

        # Modify the base installation dir
        install_dir = os.path.join(self.install_dir, PYGTK_SUFFIX_LONG)
        self.set_install_dir(install_dir)

        InstallLib.run(self)

    def install_pth(self):
        """Write the pygtk.pth file"""
        file = os.path.join(self.install_dir, 'pygtk.pth')
        self.mkpath(self.install_dir)
        open(file, 'w').write(PYGTK_SUFFIX_LONG)
        self.local_outputs.append(file)
        self.local_inputs.append('pygtk.pth')

    def install_pygtk(self):
        """install pygtk.py in the right place."""
        self.copy_file('pygtk.py', self.install_dir)
        pygtk = os.path.join(self.install_dir, 'pygtk.py')
        self.byte_compile([pygtk])
        self.local_outputs.append(pygtk)
        self.local_inputs.append('pygtk.py')

class PyGtkInstallData(InstallData):
    def run(self):
        self.add_template_option('VERSION', VERSION)
        self.prepare()

        # Install templates
        self.install_templates()

        InstallData.run(self)

    def install_templates(self):
        file = self.install_template('codegen/pygtk-codegen-2.0.in',
                                     os.path.join(self.install_dir, 'bin'))
        os.chmod(file, 0755)
        self.install_template('pygtk-2.0.pc.in',
                              os.path.join(self.install_dir,
                                           'lib','pkgconfig'))
        self.install_template('pygobject-2.0.pc.in',
                              os.path.join(self.install_dir,
                                           'lib', 'pkgconfig'))

class PyGtkBuild(build):
    enable_threading = 1
PyGtkBuild.user_options.append(('enable-threading', None,
                                'enable threading support'))

# GObject
gobject = PkgConfigExtension(name='gobject', pkc_name='gobject-2.0',
                             pkc_version=GOBJECT_REQUIRED,
                             sources=['gobject/gobjectmodule.c',
                                      'gobject/pygboxed.c',
                                      'gobject/pygenum.c',
                                      'gobject/pygflags.c',
                                      'gobject/pygobject.c',
                                      'gobject/pygmaincontext.c',
                                      'gobject/pygmainloop.c',
                                      'gobject/pygparamspec.c',
                                      'gobject/pygpointer.c',
                                      'gobject/pygtype.c',
                                      ])

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
if sys.platform == 'win32':
    gtk.sources.append('gtk/gtk-fake-win32.c')

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
py_modules.append('dsextras')

if not have_pkgconfig():
    print "Error, could not find pkg-config"
    raise SystemExit

if gobject.can_build():
    ext_modules.append(gobject)
    data_files.append((INCLUDE_DIR, ('gobject/pygobject.h',)))
    data_files.append((CODEGEN_DIR, list_files(os.path.join('codegen', '*.py'))))
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
    if '--disable-numeric' in sys.argv:
        sys.argv.remove('--disable-numeric')
    else:
        try:
            import Numeric
            GLOBAL_MACROS.append(('HAVE_NUMPY', 1))
        except ImportError:
            print ('* Numeric module could not be found, '
                   'will build without Numeric support.')
    ext_modules.append(gtk)
    data_files.append((os.path.join(INCLUDE_DIR, 'pygtk'), ('gtk/pygtk.h',)))
    data_files.append((DEFS_DIR, ('gtk/gdk.defs', 'gtk/gdk-types.defs',
                                  'gtk/gtk.defs', 'gtk/gtk-types.defs',
                                  'gtk/gtk-extrafuncs.defs')))
    py_modules += ['gtk.compat', 'gtk.keysyms']
if libglade.can_build():
    ext_modules.append(libglade)
    data_files.append((DEFS_DIR, ('gtk/libglade.defs',)))

if '--enable-threading' in sys.argv:
    sys.argv.remove('--enable-threading')
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

options = {"bdist_wininst": {"install_script": "pygtk_postinstall.py"}}

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
      scripts = ["pygtk_postinstall.py"],
      options=options,
      cmdclass={'install_lib': PyGtkInstallLib,
                'install_data': PyGtkInstallData,
                'build_ext': BuildExt,
                'build': PyGtkBuild})
