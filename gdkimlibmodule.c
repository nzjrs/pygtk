/* -*- Mode: C; c-basic-offset: 4 -*- */
/* PyGTK imlib module - python bindings for gdk_imlib
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
#include <gtk/gtk.h>
#include <gdk_imlib.h>

#ifdef HAVE_NUMPY
#  if defined(HAVE_ARRAYOBJECT_H)
#    include <arrayobject.h>
#  elif defined(HAVE_EXTENSIONS_ARRAYOBJECT_H)
#    include <Extensions/arrayobject.h>
#  elif defined(HAVE_NUMERIC_ARRAYOBJECT_H)
#    include <Numeric/arrayobject.h>
#  else
#    error "arrayobject.h not found, but HAVE_NUMPY defined"
#  endif
#endif

#include "pygtk.h"

#define _INSIDE_PYGDKIMLIB_
#include "pygdkimlib.h"

static PyObject *
PyGdkImlibImage_New(GdkImlibImage *obj) {
    PyGdkImlibImage_Object *self;

    if (obj == NULL) {
	PyErr_SetString(PyExc_IOError, "couldn't load image");
	return NULL;
    }

    self = (PyGdkImlibImage_Object *)PyObject_NEW(PyGdkImlibImage_Object,
						  &PyGdkImlibImage_Type);
    if (self == NULL)
	return NULL;
    self->obj = obj;
    return (PyObject *)self;
}

static void
PyGdkImlibImage_Dealloc(PyGdkImlibImage_Object *self) {
    gdk_imlib_destroy_image(self->obj); 
    PyMem_DEL(self);
}

static int
PyGdkImlibImage_Compare(PyGdkImlibImage_Object *self,
			PyGdkImlibImage_Object *v) {
    if (self->obj == v->obj) return 0;
    if (self->obj > v->obj) return -1;
    return 1;
}

static PyTypeObject PyGdkImlibImage_Type = {
    PyObject_HEAD_INIT(&PyType_Type)
    0,
    "GdkImlibImage",
    sizeof(PyGdkImlibImage_Object),
    0,
    (destructor)PyGdkImlibImage_Dealloc,
    (printfunc)0,
    (getattrfunc)0,
    (setattrfunc)0,
    (cmpfunc)PyGdkImlibImage_Compare,
    (reprfunc)0,
    0,
    0,
    0,
    (hashfunc)0,
    (ternaryfunc)0,
    (reprfunc)0,
    0L,0L,0L,0L,
    NULL
};


static PyObject *_wrap_gdk_imlib_init(PyObject *self, PyObject *args) {
    static int called = 0;

    if (called) {
        Py_INCREF(Py_None);
	return Py_None;
    }
    called = 1;
    if (!PyArg_ParseTuple(args, ":gdk_imlib_init"))
      return NULL;

    /* now actually initialise imlib */
    gdk_imlib_init();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_imlib_get_render_type(PyObject *self, PyObject *args) {
    if (!PyArg_ParseTuple(args,":gdk_imlib_get_render_type")) 
        return NULL;
    return PyInt_FromLong(gdk_imlib_get_render_type());
}

static PyObject *_wrap_gdk_imlib_set_render_type(PyObject *self, PyObject *args) {
    int rend_type;

    if (!PyArg_ParseTuple(args,"i:gdk_imlib_set_render_type", &rend_type)) 
        return NULL;
    gdk_imlib_set_render_type(rend_type);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_imlib_load_colors(PyObject *self, PyObject *args) {
    char *file;

    if (!PyArg_ParseTuple(args,"s:gdk_imlib_load_colors",&file)) 
        return NULL;
    return PyInt_FromLong(gdk_imlib_load_colors(file));
}

static PyObject *_wrap_gdk_imlib_load_image(PyObject *self, PyObject *args) {
    char *file;

    if(!PyArg_ParseTuple(args,"s:gdk_imlib_load_image",&file)) 
        return NULL;
    return PyGdkImlibImage_New(gdk_imlib_load_image(file));
}

static PyObject *_wrap_gdk_imlib_best_color_match(PyObject *self, PyObject *args) {
    int r, g, b;
    if (!PyArg_ParseTuple(args, "iii:gdk_imlib_best_color_match", &r, &g, &b))
	return NULL;
    gdk_imlib_best_color_match(&r, &g, &b);
    return Py_BuildValue("(iii)", r, g, b);
}

static PyObject *_wrap_gdk_imlib_render(PyObject *self, PyObject *args) {
    int width;
    int height;
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!ii:gdk_imlib_render", &PyGdkImlibImage_Type,
			 &image, &width, &height)) 
        return NULL;
     return PyInt_FromLong(gdk_imlib_render(PyGdkImlibImage_Get(image),
					    width, height));
}

static PyObject *_wrap_gdk_imlib_get_pixmap(PyObject *self, PyObject *args) {
    PyObject *image, *ret;
    GdkPixmap *pix;
    GdkBitmap *mask;

    if (!PyArg_ParseTuple(args, "O!:gdk_imlib_get_pixmap",
			  &PyGdkImlibImage_Type, &image))
        return NULL;
    pix  = gdk_imlib_move_image(PyGdkImlibImage_Get(image));
    mask = gdk_imlib_move_mask(PyGdkImlibImage_Get(image));
    ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyGdkWindow_New(pix));
    if (mask)
        PyTuple_SetItem(ret, 1, PyGdkWindow_New(mask));
    else {
        Py_INCREF(Py_None);
        PyTuple_SetItem(ret, 1, Py_None);
    }
    gdk_imlib_free_pixmap(pix);
    return ret;
}

static PyObject *_wrap_gdk_imlib_make_pixmap(PyObject *self, PyObject *args) {
    GtkWidget *w;
    GdkPixmap *p;
    GdkBitmap *b;
    PyObject *image;

    if (!PyArg_ParseTuple(args, "O!:gdk_imlib_make_pixmap",
			  &PyGdkImlibImage_Type, &image))
        return NULL;
    p = gdk_imlib_move_image(PyGdkImlibImage_Get(image));
    b = gdk_imlib_move_mask(PyGdkImlibImage_Get(image));

    gtk_widget_push_visual(gdk_imlib_get_visual());
    gtk_widget_push_colormap(gdk_imlib_get_colormap());
    w = gtk_pixmap_new(p, b);
    gtk_widget_pop_visual();
    gtk_widget_pop_colormap();

    gdk_imlib_free_pixmap(p);
    return PyGtk_New((GtkObject *)w);
}

static PyObject *_wrap_gdk_imlib_free_colors(PyObject *self, PyObject *args) {
    if(!PyArg_ParseTuple(args,":gdk_imlib_free_colors")) 
        return NULL;
    gdk_imlib_free_colors();
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_imlib_get_image_border(PyObject *self, PyObject *args) {
    GdkImlibBorder border;
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!:gdk_imlib_get_image_border",
			 &PyGdkImlibImage_Type, &image)) 
        return NULL;
    gdk_imlib_get_image_border(PyGdkImlibImage_Get(image), &border);
    return Py_BuildValue("(iiii)", border.left, border.right,
			 border.top, border.bottom);
}

static PyObject *_wrap_gdk_imlib_set_image_border(PyObject *self, PyObject *args) {
    GdkImlibBorder border;
    PyObject *image;

    if(!PyArg_ParseTuple(args, "O!(iiii):gdk_imlib_set_image_border",
			 &PyGdkImlibImage_Type, &image,
			 &(border.left), &(border.right),
			 &(border.top), &(border.bottom)))
        return NULL;
    gdk_imlib_set_image_border(PyGdkImlibImage_Get(image), &border);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_imlib_get_image_shape(PyObject *self, PyObject *args) {
    GdkImlibColor color;
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!:gdk_imlib_get_image_shape",
			 &PyGdkImlibImage_Type, &image)) 
        return NULL;
    gdk_imlib_get_image_shape(PyGdkImlibImage_Get(image), &color);
    return Py_BuildValue("(iii)", color.r, color.g, color.b);
}

static PyObject *_wrap_gdk_imlib_set_image_shape(PyObject *self, PyObject *args) {
    GdkImlibColor color;
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!(iii):gdk_imlib_set_image_shape",
			 &PyGdkImlibImage_Type, &image,
			 &(color.r), &(color.g), &(color.b)))
        return NULL;
    gdk_imlib_set_image_shape(PyGdkImlibImage_Get(image), &color);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_imlib_save_image_to_eim(PyObject *self, PyObject *args) {
    char *file;
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!s:gdk_imlib_save_image_to_eim",
			 &PyGdkImlibImage_Type, &image, &file)) 
        return NULL;
    return PyInt_FromLong(gdk_imlib_save_image_to_eim(
				PyGdkImlibImage_Get(image), file));
}

static PyObject *_wrap_gdk_imlib_add_image_to_eim(PyObject *self, PyObject *args) {
    char *file;
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!s:gdk_imlib_add_image_to_eim",
			 &PyGdkImlibImage_Type, &image, &file)) 
        return NULL;
    return PyInt_FromLong(gdk_imlib_add_image_to_eim(
				PyGdkImlibImage_Get(image), file));
}

static PyObject *_wrap_gdk_imlib_save_image_to_ppm(PyObject *self, PyObject *args) {
    char *file;
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!s:gdk_imlib_save_image_to_ppm",
			 &PyGdkImlibImage_Type, &image, &file)) 
        return NULL;
    return PyInt_FromLong(gdk_imlib_save_image_to_ppm(
				PyGdkImlibImage_Get(image), file));
}

static PyObject *_wrap_gdk_imlib_set_image_modifier(PyObject *self, PyObject *args) {
    GdkImlibColorModifier mods;
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!(iii):gdk_imlib_set_image_modifier",
			 &PyGdkImlibImage_Type, &image,
			 &(mods.gamma), &(mods.brightness), &(mods.contrast)))
        return NULL;
    gdk_imlib_set_image_modifier(PyGdkImlibImage_Get(image), &mods);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_imlib_set_image_red_modifier(PyObject *self, PyObject *args) {
    GdkImlibColorModifier mods;
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!(iii):gdk_imlib_set_image_red_modifier",
			 &PyGdkImlibImage_Type, &image,
			 &(mods.gamma), &(mods.brightness), &(mods.contrast)))
        return NULL;
    gdk_imlib_set_image_red_modifier(PyGdkImlibImage_Get(image), &mods);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_imlib_set_image_green_modifier(PyObject *self, PyObject *args) {
    GdkImlibColorModifier mods;
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!(iii):gdk_imlib_set_image_green_modifier",
			 &PyGdkImlibImage_Type, &image,
			 &(mods.gamma), &(mods.brightness), &(mods.contrast)))
        return NULL;
    gdk_imlib_set_image_green_modifier(PyGdkImlibImage_Get(image), &mods);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_imlib_set_image_blue_modifier(PyObject *self, PyObject *args) {
    GdkImlibColorModifier mods;
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!(iii):gdk_imlib_set_image_blue_modifier",
			 &PyGdkImlibImage_Type, &image,
			 &(mods.gamma), &(mods.brightness), &(mods.contrast)))
        return NULL;
    gdk_imlib_set_image_blue_modifier(PyGdkImlibImage_Get(image), &mods);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_imlib_get_image_modifier(PyObject *self, PyObject *args) {
    GdkImlibColorModifier mods;
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!:gdk_imlib_get_image_modifier",
			 &PyGdkImlibImage_Type, &image)) 
        return NULL;
    gdk_imlib_get_image_modifier(PyGdkImlibImage_Get(image), &mods);
    return Py_BuildValue("(iii)", mods.gamma, mods.brightness, mods.contrast);
}

static PyObject *_wrap_gdk_imlib_get_image_red_modifier(PyObject *self, PyObject *args) {
    GdkImlibColorModifier mods;
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!:gdk_imlib_get_image_red_modifier",
			 &PyGdkImlibImage_Type, &image)) 
        return NULL;
    gdk_imlib_get_image_red_modifier(PyGdkImlibImage_Get(image), &mods);
    return Py_BuildValue("(iii)", mods.gamma, mods.brightness, mods.contrast);
}

static PyObject *_wrap_gdk_imlib_get_image_green_modifier(PyObject *self, PyObject *args) {
    GdkImlibColorModifier mods;
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!:gdk_imlib_get_image_green_modifier",
			 &PyGdkImlibImage_Type, &image)) 
        return NULL;
    gdk_imlib_get_image_green_modifier(PyGdkImlibImage_Get(image), &mods);
    return Py_BuildValue("(iii)", mods.gamma, mods.brightness, mods.contrast);
}

static PyObject *_wrap_gdk_imlib_get_image_blue_modifier(PyObject *self, PyObject *args) {
    GdkImlibColorModifier mods;
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!:gdk_imlib_get_image_blue_modifier",
			 &PyGdkImlibImage_Type, &image)) 
        return NULL;
    gdk_imlib_get_image_blue_modifier(PyGdkImlibImage_Get(image), &mods);
    return Py_BuildValue("(iii)", mods.gamma, mods.brightness, mods.contrast);
}

static PyObject *_wrap_gdk_imlib_set_image_red_curve(PyObject *self, PyObject *args) {
    PyObject *image, *list, *item;
    unsigned char mod[256];
    int i;

    if(!PyArg_ParseTuple(args,"O!O:gdk_imlib_set_image_red_curve",
			 &PyGdkImlibImage_Type, &image, &list)) 
        return NULL;
    if (!PySequence_Check(list) || PySequence_Length(list) < 256) {
        PyErr_SetString(PyExc_TypeError, "second arg must be sequence");
	return NULL;
    }
    for (i = 0; i < 256; i++) {
        item = PySequence_GetItem(list, i);
        Py_DECREF(item);
        if (!PyInt_Check(item)) {
	    PyErr_SetString(PyExc_TypeError,"item of sequence not an integer");
	    return NULL;
        }
        mod[i] = PyInt_AsLong(item);
    }
    gdk_imlib_set_image_red_curve(PyGdkImlibImage_Get(image), mod);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_imlib_set_image_green_curve(PyObject *self, PyObject *args) {
    PyObject *image, *list, *item;
    unsigned char mod[256];
    int i;

    if(!PyArg_ParseTuple(args,"O!O:gdk_imlib_set_image_green_curve",
			 &PyGdkImlibImage_Type, &image, &list)) 
        return NULL;
    if (!PySequence_Check(list) || PySequence_Length(list) < 256) {
        PyErr_SetString(PyExc_TypeError, "second arg must be sequence");
	return NULL;
    }
    for (i = 0; i < 256; i++) {
        item = PySequence_GetItem(list, i);
        Py_DECREF(item);
        if (!PyInt_Check(item)) {
	    PyErr_SetString(PyExc_TypeError,"item of sequence not an integer");
	    return NULL;
        }
        mod[i] = PyInt_AsLong(item);
    }
    gdk_imlib_set_image_green_curve(PyGdkImlibImage_Get(image), mod);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_imlib_set_image_blue_curve(PyObject *self, PyObject *args) {
    PyObject *image, *list, *item;
    unsigned char mod[256];
    int i;

    if(!PyArg_ParseTuple(args,"O!O:gdk_imlib_set_image_blue_curve",
			 &PyGdkImlibImage_Type, &image, &list)) 
        return NULL;
    if (!PySequence_Check(list) || PySequence_Length(list) < 256) {
        PyErr_SetString(PyExc_TypeError, "second arg must be sequence");
	return NULL;
    }
    for (i = 0; i < 256; i++) {
        item = PySequence_GetItem(list, i);
        Py_DECREF(item);
        if (!PyInt_Check(item)) {
	    PyErr_SetString(PyExc_TypeError,"item of sequence not an integer");
	    return NULL;
        }
        mod[i] = PyInt_AsLong(item);
    }
    gdk_imlib_set_image_blue_curve(PyGdkImlibImage_Get(image), mod);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_imlib_get_image_red_curve(PyObject *self, PyObject *args) {
    PyObject *image, *ret;
    unsigned char mod[256];
    int i;

    if(!PyArg_ParseTuple(args,"O!:gdk_imlib_get_image_red_curve",
			 &PyGdkImlibImage_Type, &image)) 
        return NULL;
    gdk_imlib_get_image_red_curve(PyGdkImlibImage_Get(image), mod);
    ret = PyList_New(256);
    for (i = 0; i < 256; i++)
        PyList_SetItem(ret, i, PyInt_FromLong(mod[i]));
    return ret;
}

static PyObject *_wrap_gdk_imlib_get_image_green_curve(PyObject *self, PyObject *args) {
    PyObject *image, *ret;
    unsigned char mod[256];
    int i;

    if(!PyArg_ParseTuple(args,"O!:gdk_imlib_get_image_green_curve",
			 &PyGdkImlibImage_Type, &image)) 
        return NULL;
    gdk_imlib_get_image_green_curve(PyGdkImlibImage_Get(image), mod);
    ret = PyList_New(256);
    for (i = 0; i < 256; i++)
        PyList_SetItem(ret, i, PyInt_FromLong(mod[i]));
    return ret;
}

static PyObject *_wrap_gdk_imlib_get_image_blue_curve(PyObject *self, PyObject *args) {
    PyObject *image, *ret;
    unsigned char mod[256];
    int i;

    if(!PyArg_ParseTuple(args,"O!:gdk_imlib_get_image_blue_curve",
			 &PyGdkImlibImage_Type, &image)) 
        return NULL;
    gdk_imlib_get_image_blue_curve(PyGdkImlibImage_Get(image), mod);
    ret = PyList_New(256);
    for (i = 0; i < 256; i++)
        PyList_SetItem(ret, i, PyInt_FromLong(mod[i]));
    return ret;
}

static PyObject *_wrap_gdk_imlib_apply_modifiers_to_rgb(PyObject *self, PyObject *args) {
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!:gdk_imlib_apply_modifiers_to_rgb",
			 &PyGdkImlibImage_Type, &image)) 
        return NULL;
    gdk_imlib_apply_modifiers_to_rgb(PyGdkImlibImage_Get(image));
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_imlib_changed_image(PyObject *self, PyObject *args) {
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!:gdk_imlib_changed_image",
			 &PyGdkImlibImage_Type, &image)) 
        return NULL;
    gdk_imlib_changed_image(PyGdkImlibImage_Get(image));
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_imlib_flip_image_horizontal(PyObject *self, PyObject *args) {
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!:gdk_imlib_flip_image_horizontal",
			 &PyGdkImlibImage_Type, &image)) 
        return NULL;
    gdk_imlib_flip_image_horizontal(PyGdkImlibImage_Get(image));
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_imlib_flip_image_vertical(PyObject *self, PyObject *args) {
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!:gdk_imlib_flip_image_vertical",
			 &PyGdkImlibImage_Type, &image)) 
        return NULL;
    gdk_imlib_flip_image_vertical(PyGdkImlibImage_Get(image));
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_imlib_rotate_image(PyObject *self, PyObject *args) {
    PyObject *image;
    int degrees;

    if(!PyArg_ParseTuple(args,"O!i:gdk_imlib_rotate_image",
			 &PyGdkImlibImage_Type, &image, &degrees))
        return NULL;
    gdk_imlib_rotate_image(PyGdkImlibImage_Get(image), degrees);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_imlib_create_image_from_data(PyObject *self, PyObject *args) {
    unsigned char *data, *alpha;
    int w, h, ndata, nalpha;

    if (!PyArg_ParseTuple(args, "s#z#ii:gdk_imlib_create_image_from_data",
			  &data, &ndata, &alpha, &nalpha, &w, &h))
        return NULL;

    if (data && ndata < w * h * 3) {
	PyErr_SetString(PyExc_TypeError,"data argument is too short for size");
	return NULL;
    }
    if (alpha && nalpha < w * h) {
	PyErr_SetString(PyExc_TypeError,"alpha channel is too short for size");
	return NULL;
    }
    return PyGdkImlibImage_New(gdk_imlib_create_image_from_data(
					data, alpha, w, h));
}

static PyObject *_wrap_gdk_imlib_create_image_from_xpm(PyObject *self, PyObject *args) {
    PyObject *lines;
    int len, i;
    char **data;

    if (!PyArg_ParseTuple(args, "O!:gdk_imlib_create_image_from_data",
			  &PyList_Type, &lines))
	return NULL;

    len = PyList_Size(lines);
    data = g_new(char *, len);
    for (i = 0; i < len; i++) {
	PyObject *item = PyList_GetItem(lines, i);
	if (!PyString_Check(item)) {
	    PyErr_SetString(PyExc_TypeError, "list items must be strings");
	    g_free(data);
	    return NULL;
	}
	data[i] = PyString_AsString(item);
    }
    return PyGdkImlibImage_New(gdk_imlib_create_image_from_xpm_data(data));
}

static PyObject *_wrap_gdk_imlib_clone_image(PyObject *self, PyObject *args) {
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!:gdk_imlib_clone_image",
			 &PyGdkImlibImage_Type, &image)) 
        return NULL;
    return PyGdkImlibImage_New(gdk_imlib_clone_image(
				PyGdkImlibImage_Get(image)));
}

static PyObject *_wrap_gdk_imlib_clone_scaled_image(PyObject *self, PyObject *args) {
    PyObject *image;
    int width, height;

    if(!PyArg_ParseTuple(args,"O!ii:gdk_imlib_clone_scaled_image",
			 &PyGdkImlibImage_Type, &image, &width, &height)) 
        return NULL;
    return PyGdkImlibImage_New(gdk_imlib_clone_scaled_image(
				PyGdkImlibImage_Get(image), width, height));
}

static PyObject *_wrap_gdk_imlib_get_fallback(PyObject *self, PyObject *args) {
    if(!PyArg_ParseTuple(args,":gdk_imlib_get_fallback")) 
        return NULL;
    return PyInt_FromLong(gdk_imlib_get_fallback());
}

static PyObject *_wrap_gdk_imlib_set_fallback(PyObject *self, PyObject *args) {
    int fallback;

    if(!PyArg_ParseTuple(args,"i:gdk_imlib_set_fallback",&fallback)) 
        return NULL;
    gdk_imlib_set_fallback(fallback);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_imlib_get_sysconfig(PyObject *self, PyObject *args) {
    if(!PyArg_ParseTuple(args,":gdk_imlib_get_sysconfig")) 
        return NULL;
    return PyString_FromString(gdk_imlib_get_sysconfig());
}

static PyObject *_wrap_gdk_imlib_crop_image(PyObject *self, PyObject *args) {
    PyObject *image;
    int x, y, w, h;

    if(!PyArg_ParseTuple(args,"O!iiii:gdk_imlib_crop_image",
			 &PyGdkImlibImage_Type, &image, &x, &y, &w, &h))
        return NULL;
    gdk_imlib_crop_image(PyGdkImlibImage_Get(image), x, y, w, h);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_imlib_save_image(PyObject *self, PyObject *args) {
    PyObject *image;
    char *file;

    if(!PyArg_ParseTuple(args,"O!s:gdk_imlib_save_image",
			 &PyGdkImlibImage_Type, &image, &file))
        return NULL;
    return PyInt_FromLong(gdk_imlib_save_image(
				PyGdkImlibImage_Get(image), file, NULL));
}

static PyObject *_wrap_gdk_imlib_crop_and_clone_image(PyObject *self, PyObject *args) {
    PyObject *image;
    int x, y, w, h;

    if (!PyArg_ParseTuple(args, "O!iiii:gdk_imlib_crop_and_clone_image",
			  &PyGdkImlibImage_Type, &image, &x, &y, &w, &h))
        return NULL;
    return PyGdkImlibImage_New(gdk_imlib_crop_and_clone_image(
			PyGdkImlibImage_Get(image), x, y, w, h));
}

static PyObject *_wrap_gdk_imlib_create_image_from_drawable(PyObject *self, PyObject *args) {
    PyObject *win, *py_mask;
    int x, y, w, h;
    GdkBitmap *mask = NULL;

    if (!PyArg_ParseTuple(args, "O!Oiiii:gdk_imlib_create_image_from_drawable",
			  &PyGdkWindow_Type, &win, &py_mask, &x, &y, &w, &h))
        return NULL;
    if (py_mask != Py_None && !PyGdkWindow_Check(py_mask)) {
        PyErr_SetString(PyExc_TypeError,
			"second argument must be a GdkBitmap or None");
	return NULL;
    }
    if (py_mask != Py_None) mask = PyGdkWindow_Get(py_mask);
    return PyGdkImlibImage_New(gdk_imlib_create_image_from_drawable(
				PyGdkWindow_Get(win), mask, x, y, w, h));
}

static PyObject *_wrap_gdk_imlib_get_rgb_width(PyObject *self, PyObject *args) {
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!:gdk_imlib_get_rgb_width",
			 &PyGdkImlibImage_Type, &image)) 
        return NULL;
    return PyInt_FromLong(PyGdkImlibImage_Get(image)->rgb_width);
}

static PyObject *_wrap_gdk_imlib_get_rgb_height(PyObject *self, PyObject *args) {
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!:gdk_imlib_get_rgb_height",
			 &PyGdkImlibImage_Type, &image)) 
        return NULL;
    return PyInt_FromLong(PyGdkImlibImage_Get(image)->rgb_height);
}

static PyObject *_wrap_gdk_imlib_get_filename(PyObject *self, PyObject *args) {
    PyObject *image;

    if(!PyArg_ParseTuple(args,"O!:gdk_imlib_get_filename",
			 &PyGdkImlibImage_Type, &image)) 
        return NULL;
    return PyString_FromString(PyGdkImlibImage_Get(image)->filename);
}

static PyObject *_wrap_gdk_imlib_push_visual(PyObject *self, PyObject *args) {
    if (!PyArg_ParseTuple(args, ":gdk_imlib_push_visual"))
        return NULL;
    gtk_widget_push_visual(gdk_imlib_get_visual());
    gtk_widget_push_colormap(gdk_imlib_get_colormap());
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_imlib_pop_visual(PyObject *self, PyObject *args) {
    if (!PyArg_ParseTuple(args, ":gdk_imlib_pop_visual"))
        return NULL;
    gtk_widget_pop_visual();
    gtk_widget_pop_colormap();
    Py_INCREF(Py_None);
    return Py_None;
}

#ifdef HAVE_NUMPY
static PyObject *_wrap_gdk_imlib_create_image_from_array(PyObject *self, PyObject *args) {
    PyArrayObject *ap_data, *ap_alpha=NULL;
    unsigned char *data, *alpha = NULL;
    int w, h;
    if (!PyArg_ParseTuple(args, "O!|O!:gdk_imlib_create_image_from_array",
			  &PyArray_Type, &ap_data, &PyArray_Type, &ap_alpha))
	return NULL;
    if (!PyArray_ISCONTIGUOUS(ap_data) ||
	ap_data->descr->type_num != PyArray_UBYTE ||
	ap_data->nd != 3 ||
	ap_data->dimensions[2] != 3) {
	PyErr_SetString(PyExc_TypeError,
		"image array must be MxNx3 contiguous unsigned byte array");
	return NULL;
    }
    data = (unsigned char *)ap_data->data;
    h = ap_data->dimensions[0];
    w = ap_data->dimensions[1];

    if (ap_alpha != NULL) {
	if (!PyArray_ISCONTIGUOUS(ap_alpha) ||
	    ap_alpha->descr->type_num != PyArray_UBYTE ||
	    ap_alpha->nd != 2 ||
	    ap_alpha->dimensions[0] != h ||
	    ap_alpha->dimensions[1] != w) {
	    PyErr_SetString(PyExc_TypeError,
			    "alpha channel array must be 2D contiguous byte array");
	    return NULL;
	}

	alpha = (unsigned char *)ap_alpha->data;
    }
    return PyGdkImlibImage_New(gdk_imlib_create_image_from_data(
						data, alpha, w, h));
}

static PyObject *_wrap_gdk_imlib_image_get_array(PyObject *self, PyObject *args) {
    PyObject *image;
    GdkImlibImage *obj;
    PyArrayObject *ap_data;
    int dims[3] = {0, 0, 3};

    if (!PyArg_ParseTuple(args, "O!:gdk_imlib_image_get_array",
			  &PyGdkImlibImage_Type, &image))
	return NULL;
    obj = PyGdkImlibImage_Get(image);
    dims[0] = (int)obj->rgb_width;
    dims[1] = (int)obj->rgb_height;

    ap_data = (PyArrayObject *)PyArray_FromDimsAndData(3, dims, PyArray_UBYTE,
						       (char *)obj->rgb_data);
    if (ap_data == NULL)
	return NULL;
    /* this makes sure the GdkImlibImage is not freed while someone has a
     * reference to the new array. */
    Py_INCREF(image);
    ap_data->base = image;

    return PyArray_Return(ap_data);
}
#endif

static PyMethodDef _gdkimlibMethods[] = {
    { "gdk_imlib_push_visual", _wrap_gdk_imlib_push_visual, 1 },
    { "gdk_imlib_pop_visual", _wrap_gdk_imlib_pop_visual, 1 },
    { "gdk_imlib_get_filename", _wrap_gdk_imlib_get_filename, 1 },
    { "gdk_imlib_get_rgb_height", _wrap_gdk_imlib_get_rgb_height, 1 },
    { "gdk_imlib_get_rgb_width", _wrap_gdk_imlib_get_rgb_width, 1 },
    { "gdk_imlib_create_image_from_drawable", _wrap_gdk_imlib_create_image_from_drawable, 1 },
    { "gdk_imlib_crop_and_clone_image", _wrap_gdk_imlib_crop_and_clone_image, 1 },
    { "gdk_imlib_save_image", _wrap_gdk_imlib_save_image, 1 },
    { "gdk_imlib_crop_image", _wrap_gdk_imlib_crop_image, 1 },
    { "gdk_imlib_get_sysconfig", _wrap_gdk_imlib_get_sysconfig, 1 },
    { "gdk_imlib_set_fallback", _wrap_gdk_imlib_set_fallback, 1 },
    { "gdk_imlib_get_fallback", _wrap_gdk_imlib_get_fallback, 1 },
    { "gdk_imlib_clone_scaled_image", _wrap_gdk_imlib_clone_scaled_image, 1 },
    { "gdk_imlib_clone_image", _wrap_gdk_imlib_clone_image, 1 },
    { "gdk_imlib_create_image_from_data", _wrap_gdk_imlib_create_image_from_data, 1 },
    { "gdk_imlib_create_image_from_xpm", _wrap_gdk_imlib_create_image_from_xpm, 1 },
    { "gdk_imlib_rotate_image", _wrap_gdk_imlib_rotate_image, 1 },
    { "gdk_imlib_flip_image_vertical", _wrap_gdk_imlib_flip_image_vertical, 1 },
    { "gdk_imlib_flip_image_horizontal", _wrap_gdk_imlib_flip_image_horizontal, 1 },
    { "gdk_imlib_changed_image", _wrap_gdk_imlib_changed_image, 1 },
    { "gdk_imlib_apply_modifiers_to_rgb", _wrap_gdk_imlib_apply_modifiers_to_rgb, 1 },
    { "gdk_imlib_get_image_blue_curve", _wrap_gdk_imlib_get_image_blue_curve, 1 },
    { "gdk_imlib_get_image_green_curve", _wrap_gdk_imlib_get_image_green_curve, 1 },
    { "gdk_imlib_get_image_red_curve", _wrap_gdk_imlib_get_image_red_curve, 1 },
    { "gdk_imlib_set_image_blue_curve", _wrap_gdk_imlib_set_image_blue_curve, 1 },
    { "gdk_imlib_set_image_green_curve", _wrap_gdk_imlib_set_image_green_curve, 1 },
    { "gdk_imlib_set_image_red_curve", _wrap_gdk_imlib_set_image_red_curve, 1 },
    { "gdk_imlib_get_image_blue_modifier", _wrap_gdk_imlib_get_image_blue_modifier, 1 },
    { "gdk_imlib_get_image_green_modifier", _wrap_gdk_imlib_get_image_green_modifier, 1 },
    { "gdk_imlib_get_image_red_modifier", _wrap_gdk_imlib_get_image_red_modifier, 1 },
    { "gdk_imlib_get_image_modifier", _wrap_gdk_imlib_get_image_modifier, 1 },
    { "gdk_imlib_set_image_blue_modifier", _wrap_gdk_imlib_set_image_blue_modifier, 1 },
    { "gdk_imlib_set_image_green_modifier", _wrap_gdk_imlib_set_image_green_modifier, 1 },
    { "gdk_imlib_set_image_red_modifier", _wrap_gdk_imlib_set_image_red_modifier, 1 },
    { "gdk_imlib_set_image_modifier", _wrap_gdk_imlib_set_image_modifier, 1 },
    { "gdk_imlib_save_image_to_ppm", _wrap_gdk_imlib_save_image_to_ppm, 1 },
    { "gdk_imlib_add_image_to_eim", _wrap_gdk_imlib_add_image_to_eim, 1 },
    { "gdk_imlib_save_image_to_eim", _wrap_gdk_imlib_save_image_to_eim, 1 },
    { "gdk_imlib_set_image_shape", _wrap_gdk_imlib_set_image_shape, 1 },
    { "gdk_imlib_get_image_shape", _wrap_gdk_imlib_get_image_shape, 1 },
    { "gdk_imlib_set_image_border", _wrap_gdk_imlib_set_image_border, 1 },
    { "gdk_imlib_get_image_border", _wrap_gdk_imlib_get_image_border, 1 },
    { "gdk_imlib_free_colors", _wrap_gdk_imlib_free_colors, 1 },
    { "gdk_imlib_make_pixmap", _wrap_gdk_imlib_make_pixmap, 1 },
    { "gdk_imlib_get_pixmap", _wrap_gdk_imlib_get_pixmap, 1 },
    { "gdk_imlib_render", _wrap_gdk_imlib_render, 1 },
    { "gdk_imlib_best_color_match", _wrap_gdk_imlib_best_color_match, 1 },
    { "gdk_imlib_load_image", _wrap_gdk_imlib_load_image, 1 },
    { "gdk_imlib_load_colors", _wrap_gdk_imlib_load_colors, 1 },
    { "gdk_imlib_set_render_type", _wrap_gdk_imlib_set_render_type, 1 },
    { "gdk_imlib_get_render_type", _wrap_gdk_imlib_get_render_type, 1 },
    { "gdk_imlib_init", _wrap_gdk_imlib_init, 1 },
#ifdef HAVE_NUMPY
    { "gdk_imlib_create_image_from_array", _wrap_gdk_imlib_create_image_from_array, 1 },
    { "gdk_imlib_image_get_array", _wrap_gdk_imlib_image_get_array, 1 },
#endif
    { NULL, NULL }
};

static struct _PyGdkImlib_FunctionStruct functions = {
    &PyGdkImlibImage_Type, PyGdkImlibImage_New
};

void init_gdkimlib() {
    PyObject *m, *d, *private;

    m = Py_InitModule("_gdkimlib", _gdkimlibMethods);
    d = PyModule_GetDict(m);

#ifdef HAVE_NUMPY
    import_array();
#endif
    
    PyDict_SetItemString(d, "GdkImlibImageType",
			 (PyObject *)&PyGdkImlibImage_Type);

     PyDict_SetItemString(d, "_PyGdkI_API",
			  PyCObject_FromVoidPtr(&functions, NULL));

    private = PyDict_New();
    PyDict_SetItemString(d, "_private", private); Py_DECREF(private);

    PyDict_SetItemString(private, "PyGdkImlibImage_New",
			 d=PyCObject_FromVoidPtr(PyGdkImlibImage_New, NULL));
    Py_DECREF(d);

    /* get references to the pygtk types and functions */
    init_pygtk();

    if (PyErr_Occurred())
         Py_FatalError("can't initialise module _gdkimlib");
}
