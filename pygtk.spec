%define py_prefix /usr
# py_ver should only be 3 characters (1.5.1 == 1.5)
%define py_ver 1.5
%define ver 0.6.8

# you may want to remove some of the sub packages depending on what you
# have installed on your system when building this package.

Summary: Python bindings for the GTK+ widget set.
Name: pygtk
Version: %ver
Release: 1
Copyright: GPL
Group: Development/Languages
Source: ftp://ftp.gtk.org/pub/gtk/python/pygtk-%{ver}.tar.gz
BuildRoot: /var/tmp/pygtk-root
Packager: James Henstridge <james@daa.com.au>
Requires: gtk+ >= 1.2.6
Requires: imlib
Requires: python >= 1.5.2

%description
PyGTK is an extension module for python that gives you access to the GTK+
widget set.  Just about anything you can write in C with GTK+ you can write
in python with PyGTK (within reason), but with all the benefits of python.

%package glarea
Summary: A wrapper for the GtkGLArea widget for use with PyGTK
Group: Development/Languages
Requires: pygtk = %{ver}

%description glarea
This module contains a wrapper for the GtkGLArea widget, which allows you
to display OpenGL output inside your pygtk program.  It needs a set of
Python OpenGL bindings such as PyOpenGL to actually do any OpenGL rendering.

%package libglade
Summary: A wrapper for the libglade library for use with PyGTK
Group: Development/Languages
Requires: pygtk = %{ver}

%description libglade
This module contains a wrapper for the libglade library.  Libglade is a
library similar to the pyglade module, except that it is written in C (so
is faster) and is more complete.

%package devel
Summary: files needed to build wrappers for GTK+ addon libraries
Group: Development/Languages
Requires: pygtk = %{ver}

%description devel
This package contains files required to build wrappers for GTK+ addon
libraries so that they interoperate with pygtk.

%prep
%setup
./configure --prefix=%{py_prefix}

%build
make

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

%files
%{py_prefix}/lib/python%{py_ver}/site-packages/gtk.py*
%{py_prefix}/lib/python%{py_ver}/site-packages/GtkExtra.py*
%{py_prefix}/lib/python%{py_ver}/site-packages/GTK.py*
%{py_prefix}/lib/python%{py_ver}/site-packages/GDK.py*
%{py_prefix}/lib/python%{py_ver}/site-packages/GdkImlib.py*
%{py_prefix}/lib/python%{py_ver}/site-packages/pyglade/*.py*

%{py_prefix}/lib/python%{py_ver}/site-packages/_gtkmodule.so
%{py_prefix}/lib/python%{py_ver}/site-packages/_gdkimlibmodule.so
%{py_prefix}/lib/python%{py_ver}/site-packages/gdkpixbufmodule.so

%{py_prefix}/include/pygtk

%doc AUTHORS NEWS README MAPPING ChangeLog description.py
%doc examples

%files glarea
%{py_prefix}/lib/python%{py_ver}/site-packages/gtkgl.py*
%{py_prefix}/lib/python%{py_ver}/site-packages/_gtkglmodule.so

%files libglade
%{py_prefix}/lib/python%{py_ver}/site-packages/libglade.py*
%{py_prefix}/lib/python%{py_ver}/site-packages/_libglademodule.so

%files devel
%{py_prefix}/bin/pygtk-codegen-1.2
%dir %{py_prefix}/include/pygtk
%{py_prefix}/include/pygtk/*.h
%dir %{py_prefix}/share/pygtk
%dir %{py_prefix}/share/pygtk/1.2
%dir %{py_prefix}/share/pygtk/1.2/codegen
%dir %{py_prefix}/share/pygtk/1.2/defs
%{py_prefix}/share/pygtk/1.2/codegen/*
%{py_prefix}/share/pygtk/1.2/defs/gtk.defs
%{py_prefix}/share/pygtk/1.2/defs/gtkbase.defs
%{py_prefix}/share/pygtk/1.2/defs/gtkcontainers.defs
%{py_prefix}/share/pygtk/1.2/defs/gtkdata.defs
%{py_prefix}/share/pygtk/1.2/defs/gtkdnd.defs
%{py_prefix}/share/pygtk/1.2/defs/gtkedit.defs
%{py_prefix}/share/pygtk/1.2/defs/gtkenums.defs
%{py_prefix}/share/pygtk/1.2/defs/gtkgl.defs
%{py_prefix}/share/pygtk/1.2/defs/gtklists.defs
%{py_prefix}/share/pygtk/1.2/defs/gtkmenus.defs
%{py_prefix}/share/pygtk/1.2/defs/gtkmisc.defs
%{py_prefix}/share/pygtk/1.2/defs/gtkranges.defs
%{py_prefix}/share/pygtk/1.2/defs/libglade.defs

