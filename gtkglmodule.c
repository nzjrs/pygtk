/* -*- Mode: C; c-basic-offset: 4 -*- */
/* pygtk - python bindings for GTK+
 * Copyright (C) 1998-2002 James Henstridge <james@daa.com.au>
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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <Python.h>
#include <gtkgl/gtkglarea.h>

#include "pygtk.h"

static PyObject *_wrap_gdk_gl_wait_gdk(PyObject *self, PyObject *args) {
    if (!PyArg_ParseTuple(args, ":gdk_gl_wait_gdk"))
        return NULL;
    PyGtk_UnblockThreads();
    gdk_gl_wait_gdk();
    PyGtk_BlockThreads();
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_gl_wait_gl(PyObject *self, PyObject *args) {
    if (!PyArg_ParseTuple(args, ":gdk_gl_wait_gl"))
        return NULL;
    PyGtk_UnblockThreads();
    gdk_gl_wait_gl();
    PyGtk_BlockThreads();
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_gl_area_new(PyObject *self, PyObject *args) {
    GtkWidget *widget;
    gint *attrList;
    PyObject *seq;
    gint i, len;

    if (!PyArg_ParseTuple(args, "O:gtk_gl_area_new", &seq))
	return NULL;
    if (!PySequence_Check(seq)) {
	PyErr_SetString(PyExc_TypeError, "first argument must be a sequence");
	return NULL;
    }
    len = PySequence_Length(seq);
    attrList = g_new(int, len+1);
    for (i = 0; i < len; i++) {
	PyObject *item = PySequence_GetItem(seq, i);
        Py_DECREF(item);
	if (!PyInt_Check(item)) {
	    g_free(attrList);
	    PyErr_SetString(PyExc_TypeError, "sequence member must be an int");
	    return NULL;
	}
	attrList[i] = PyInt_AsLong(item);
    }
    attrList[len] = GDK_GL_NONE; /* sentinel */
    widget = gtk_gl_area_new(attrList);
    g_free(attrList);
    return PyGtk_New((GtkObject *)widget);
}

static PyObject *_wrap_gtk_gl_area_share_new(PyObject *self, PyObject *args) {
    GtkWidget *widget;
    gint *attrList;
    PyObject *seq, *share;
    gint i, len;

    if (!PyArg_ParseTuple(args, "OO!:gtk_gl_area_share_new", &seq,
			  &PyGtk_Type, &share))
	return NULL;
    if (!PySequence_Check(seq)) {
	PyErr_SetString(PyExc_TypeError, "first argument must be a sequence");
	return NULL;
    }
    len = PySequence_Length(seq);
    attrList = g_new(int, len+1);
    for (i = 0; i < len; i++) {
	PyObject *item = PySequence_GetItem(seq, i);
        Py_DECREF(item);
	if (!PyInt_Check(item)) {
	    g_free(attrList);
	    PyErr_SetString(PyExc_TypeError, "sequence member must be an int");
	    return NULL;
	}
	attrList[i] = PyInt_AsLong(item);
    }
    attrList[len] = GDK_GL_NONE; /* sentinel */
    widget = gtk_gl_area_share_new(attrList, GTK_GL_AREA(PyGtk_Get(share)));
    g_free(attrList);
    return PyGtk_New((GtkObject *)widget);
}

#include "gtkgl_impl.c"

static PyMethodDef gtkglMethods[] = {
    { "gdk_gl_wait_gdk", _wrap_gdk_gl_wait_gdk, 1 },
    { "gdk_gl_wait_gl", _wrap_gdk_gl_wait_gl, 1 },
    { "gtk_gl_area_new", _wrap_gtk_gl_area_new, 1 },
    { "gtk_gl_area_share_new", _wrap_gtk_gl_area_share_new, 1 },
#include "gtkgl_defs.c"
    { NULL, NULL, 0 }
};

void init_gtkgl(void) {
    PyObject *m, *d;

    m = Py_InitModule("_gtkgl", gtkglMethods);

    init_pygtk();

    if (PyErr_Occurred())
	Py_FatalError("can't initialise module _gtkgl");
}

