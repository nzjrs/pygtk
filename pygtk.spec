# you may want to remove some of the sub packages depending on what you
# have installed on your system when building this package.

Summary: Python bindings for the GTK+ widget set.
Name: pygtk
Version: 1.99
Release: 1
Copyright: LGPL
Group: Development/Languages
Source: ftp://ftp.gtk.org/pub/gtk/python/pygtk-%{version}.tar.gz
BuildRoot: /var/tmp/pygtk-root
Packager: James Henstridge <james@daa.com.au>
Requires: gtk+ >= 1.3.7
Requires: imlib
Requires: python >= 2.0

%description
PyGTK is an extension module for python that gives you access to the GTK+
widget set.  Just about anything you can write in C with GTK+ you can write
in python with PyGTK (within reason), but with all of python's benefits.

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

%prep
%setup
./configure --prefix=%{_prefix}

%build
make

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

%files
%dir %{_prefix}/lib/python?.?/site-packages/gtk
%{_prefix}/lib/python?.?/site-packages/gtk/__init__.py*
%{_prefix}/lib/python?.?/site-packages/gtk/GDK.py*
%{_prefix}/lib/python?.?/site-packages/gtk/GTK.py*

%{_prefix}/lib/python?.?/site-packages/gobjectmodule.so
%{_prefix}/lib/python?.?/site-packages/pangomodule.so
%{_prefix}/lib/python?.?/site-packages/gtk/_gtkmodule.so

%doc AUTHORS NEWS README MAPPING ChangeLog description.py
%doc examples

%files libglade
%{_prefix}/lib/python?.?/site-packages/gtk/libglademodule.so

