/* -*- Mode: C; c-basic-offset: 4 -*- */
/* PyGTK libglade module - python bindings for libglade
 * Copyright (C) 1999 James Henstridge <james@daa.com.au>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <Python.h>
#include <gtk/gtk.h>
#include <glade/glade.h>

#include "pygtk.h"

#include "libglade_impl.c"

static void connect_one(const gchar *handler_name, GtkObject *obj,
			const gchar *signal_name, const gchar *signal_data,
			GtkObject *connect_object, gboolean after,
			gpointer user_data) {
    PyObject *callback = user_data;

    if (connect_object) {
	PyObject *func, *extra, *other;

	other = PyGtk_New(connect_object);
	if (PyTuple_Check(callback)) {
	    func = PyTuple_GetItem(callback, 0);
	    extra = PyTuple_GetItem(callback, 1);
	    Py_INCREF(func);
	    Py_INCREF(extra);
	    callback = Py_BuildValue("(OOO)", func, extra, other);
	} else {
	    Py_INCREF(callback);
	    callback = Py_BuildValue("(O()O)", callback, other);
	}
    } else
	Py_INCREF(callback);

    gtk_signal_connect_full(obj, signal_name, NULL,
			    (GtkCallbackMarshal)PyGtk_CallbackMarshal,
			    callback, PyGtk_DestroyNotify, FALSE, after);
}

static void connect_many(const gchar *handler_name, GtkObject *obj,
			 const gchar *signal_name, const gchar *signal_data,
			 GtkObject *connect_object, gboolean after,
			 gpointer user_data) {
    PyObject *handler_dict = user_data;
    PyObject *callback;

    callback = PyDict_GetItemString(handler_dict, (gchar *)handler_name);
    if (!callback) {
	PyErr_Clear();
	return;
    }
    if (!PyCallable_Check(callback))
	return;

    if (connect_object) {
	PyObject *func, *extra, *other;

	other = PyGtk_New(connect_object);
	if (PyTuple_Check(callback)) {
	    func = PyTuple_GetItem(callback, 0);
	    extra = PyTuple_GetItem(callback, 1);
	    Py_INCREF(func);
	    Py_INCREF(extra);
	    callback = Py_BuildValue("(OOO)", func, extra, other);
	} else {
	    Py_INCREF(callback);
	    callback = Py_BuildValue("(O()O)", callback, other);
	}
    } else
	Py_INCREF(callback);

    gtk_signal_connect_full(obj, signal_name, NULL,
			    (GtkCallbackMarshal)PyGtk_CallbackMarshal,
			    callback, PyGtk_DestroyNotify, FALSE, after);
}

static PyObject *_wrap_glade_xml_signal_connect(PyObject *self, PyObject *args) {
    PyObject *xml, *func, *cbargs = NULL, *data;
    gchar *handler_name;

    if (!PyArg_ParseTuple(args, "O!sO|O!:glade_xml_signal_connect",
			  &PyGtk_Type, &xml, &handler_name, &func,
			  &PyTuple_Type, &cbargs))
	return NULL;
    if (!PyCallable_Check(func)) {
	PyErr_SetString(PyExc_TypeError, "third argument not callable");
	return NULL;
    }
    if (cbargs)
	Py_INCREF(cbargs);
    else
	cbargs = PyTuple_New(0);
    Py_INCREF(func);
    data = Py_BuildValue("(OO)", func, cbargs);
    glade_xml_signal_connect_full(GLADE_XML(PyGtk_Get(xml)), handler_name,
				  connect_one, data);
    Py_DECREF(data);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_glade_xml_signal_autoconnect(PyObject *self, PyObject *args) {
    PyObject *xml, *dict;

    if (!PyArg_ParseTuple(args, "O!O!:glade_xml_signal_autoconnect",
			  &PyGtk_Type, &xml, &PyDict_Type, &dict))
	return NULL;
    glade_xml_signal_autoconnect_full(GLADE_XML(PyGtk_Get(xml)),
				      connect_many, dict);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef libgladeMethods[] = {
    { "glade_xml_signal_connect", _wrap_glade_xml_signal_connect, 1 },
    { "glade_xml_signal_autoconnect", _wrap_glade_xml_signal_autoconnect, 1 },
#include "libglade_defs.c"
    { NULL, NULL, 0 }
};

void init_libglade(void) {
    PyObject *m, *d;

    m = Py_InitModule("_libglade", libgladeMethods);

    init_pygtk();

    glade_init();

    if (PyErr_Occurred())
	Py_FatalError("can't initialise module _libglade");
}

