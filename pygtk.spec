%define py_prefix /usr
# py_ver should only be 3 characters (1.5.1 == 1.5)
%define py_ver 1.5
%define ver 0.5.10

Summary: Python bindings for the GTK+ widget set.
Name: pygtk
Version: %ver
Release: 1
Copyright: GPL
Group: X11/Libraries
Source: ftp://ftp.daa.com.au/pub/james/python/pygtk-%{ver}.tar.gz
BuildRoot: /var/tmp/pygtk-root
Packager: James Henstridge <james@daa.com.au>
Requires: gtk+ imlib python

%description
PyGTK is an extension module for python that gives you access to the GTK+
widget set.  Just about anything you can write in C with GTK+ you can write
in python with PyGTK (within reason), but with all of python's benefits.

%prep
%setup
./configure

%build
make

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

%files
%{py_prefix}/lib/python%{py_ver}/site-packages/gtk.py*
%{py_prefix}/lib/python%{py_ver}/site-packages/Gtkinter.py*
%{py_prefix}/lib/python%{py_ver}/site-packages/GtkExtra.py*
%{py_prefix}/lib/python%{py_ver}/site-packages/GTK.py*
%{py_prefix}/lib/python%{py_ver}/site-packages/GDK.py*
%{py_prefix}/lib/python%{py_ver}/site-packages/GdkImlib.py*
%{py_prefix}/lib/python%{py_ver}/site-packages/pyglade/*.py*

%{py_prefix}/lib/python%{py_ver}/site-packages/_gtkmodule.so
%{py_prefix}/lib/python%{py_ver}/site-packages/_gdkimlibmodule.so

%doc AUTHORS NEWS README ChangeLog description.py
%doc examples

