# you may want to remove some of the sub packages depending on what you
# have installed on your system when building this package.

Summary: Python bindings for the GTK+ widget set.
Name: pygtk
Version: 0.6.11
Release: 1
Copyright: GPL
Group: Development/Languages
Source: ftp://ftp.gtk.org/pub/gtk/python/pygtk-%{version}.tar.gz
BuildRoot: /var/tmp/pygtk-root
Packager: James Henstridge <james@daa.com.au>
Requires: gtk+ >= 1.2.10
Requires: imlib
Requires: python >= 1.5.2

%description
PyGTK is an extension module for python that gives you access to the GTK+
widget set.  Just about anything you can write in C with GTK+ you can write
in python with PyGTK (within reason), but with all the benefits of python.

%package glarea
Summary: A wrapper for the GtkGLArea widget for use with PyGTK
Group: Development/Languages
Requires: pygtk = %{version}

%description glarea
This module contains a wrapper for the GtkGLArea widget, which allows you
to display OpenGL output inside your pygtk program.  It needs a set of
Python OpenGL bindings such as PyOpenGL to actually do any OpenGL rendering.

%package libglade
Summary: A wrapper for the libglade library for use with PyGTK
Group: Development/Languages
Requires: pygtk = %{version}

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
./configure --prefix=%{_prefix}

%build
make

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

%files
%defattr(644, root, root, 755)
%{_prefix}/lib/python?.?/site-packages/pygtk.pth
%{_prefix}/lib/python?.?/site-packages/pygtk.py*
%dir %{_prefix}/lib/python?.?/site-packages/gtk-1.2
%{_prefix}/lib/python?.?/site-packages/gtk-1.2/gtk.py*
%{_prefix}/lib/python?.?/site-packages/gtk-1.2/GtkExtra.py*
%{_prefix}/lib/python?.?/site-packages/gtk-1.2/GTK.py*
%{_prefix}/lib/python?.?/site-packages/gtk-1.2/GDK.py*
%{_prefix}/lib/python?.?/site-packages/gtk-1.2/GdkImlib.py*
%dir %{_prefix}/lib/python?.?/site-packages/gtk-1.2/pyglade
%{_prefix}/lib/python?.?/site-packages/gtk-1.2/pyglade/*.py*

%{_prefix}/lib/python?.?/site-packages/gtk-1.2/_gtkmodule.so
%{_prefix}/lib/python?.?/site-packages/gtk-1.2/_gdkimlibmodule.so
%{_prefix}/lib/python?.?/site-packages/gtk-1.2/gdkpixbufmodule.so

%{_prefix}/include/pygtk

%doc AUTHORS NEWS README MAPPING ChangeLog description.py
%doc examples

%files glarea
%defattr(644, root, root, 755)
%{_prefix}/lib/python?.?/site-packages/gtk-1.2/gtkgl.py*
%{_prefix}/lib/python?.?/site-packages/gtk-1.2/_gtkglmodule.so

%files libglade
%defattr(644, root, root, 755)
%{_prefix}/lib/python?.?/site-packages/gtk-1.2/libglade.py*
%{_prefix}/lib/python?.?/site-packages/gtk-1.2/_libglademodule.so

%files devel
%defattr(644, root, root, 755)
%{_prefix}/bin/pygtk-codegen-1.2
%dir %{_prefix}/include/pygtk
%{_prefix}/include/pygtk/*.h
%dir %{_prefix}/share/pygtk
%dir %{_prefix}/share/pygtk/1.2
%dir %{_prefix}/share/pygtk/1.2/codegen
%dir %{_prefix}/share/pygtk/1.2/defs
%{_prefix}/share/pygtk/1.2/codegen/*
%{_prefix}/share/pygtk/1.2/defs/gtk.defs
%{_prefix}/share/pygtk/1.2/defs/gtkbase.defs
%{_prefix}/share/pygtk/1.2/defs/gtkcontainers.defs
%{_prefix}/share/pygtk/1.2/defs/gtkdata.defs
%{_prefix}/share/pygtk/1.2/defs/gtkdnd.defs
%{_prefix}/share/pygtk/1.2/defs/gtkedit.defs
%{_prefix}/share/pygtk/1.2/defs/gtkenums.defs
%{_prefix}/share/pygtk/1.2/defs/gtkgl.defs
%{_prefix}/share/pygtk/1.2/defs/gtklists.defs
%{_prefix}/share/pygtk/1.2/defs/gtkmenus.defs
%{_prefix}/share/pygtk/1.2/defs/gtkmisc.defs
%{_prefix}/share/pygtk/1.2/defs/gtkranges.defs
%{_prefix}/share/pygtk/1.2/defs/libglade.defs

