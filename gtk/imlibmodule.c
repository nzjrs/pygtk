/* -*- Mode: C; c-basic-offset: 4 -*- */
/* PyGTK imlib module - python bindings for gdk_imlib
 * Copyright (C) 1998-2000 James Henstridge <james@daa.com.au>
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
#  elif defined(HAVE_EXTESNSIONS_ARRAYOBJECT_H)
#    include <Extensions/arrayobject.h>
#  elif defined(HAVE_NUMERIC_ARRAYOBJECT_H)
#    include <Numeric/arrayobject.h>
#  else
#    error "arrayobject.h not found, but HAVE_NUMPY defined"
#  endif
#endif

#include "pygtk.h"
/* we don't want the ExtensionClass version of this function here */
#undef Py_FindMethod

#define _INSIDE_PYGDKIMLIB_
#include "pygdkimlib.h"

static PyObject *
PyGdkImlibImage_New(GdkImlibImage *obj)
{
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
pygdk_imlib_image_dealloc(PyGdkImlibImage_Object *self)
{
    gdk_imlib_destroy_image(self->obj); 
    PyMem_DEL(self);
}

static int
pygdk_imlib_image_compare(PyGdkImlibImage_Object *self,
			PyGdkImlibImage_Object *v)
{
    if (self->obj == v->obj) return 0;
    if (self->obj > v->obj) return -1;
    return 1;
}

static long
pygdk_imlib_image_hash(PyGdkImlibImage_Object *self)
{
    return (long)self->obj;
}

/* ------------ GdkImlibImage methods ------------------ */

static PyObject *
_wrap_gdk_imlib_render(PyGdkImlibImage_Object *self, PyObject *args)
{
    int width = -1;
    int height = -1;

    if(!PyArg_ParseTuple(args,"|ii:GdkImlibImage.render", &width, &height)) 
        return NULL;
    if (width < 0) width = self->obj->rgb_width;
    if (height < 0) height = self->obj->rgb_height;
    return PyInt_FromLong(gdk_imlib_render(self->obj, width, height));
}

static PyObject *
_wrap_gdk_imlib_get_pixmap(PyGdkImlibImage_Object *self, PyObject *args)
{
    PyObject *ret;
    GdkPixmap *pix;
    GdkBitmap *mask;

    if (!PyArg_ParseTuple(args, ":GdkImlibImage.get_pixmap"))
        return NULL;
    pix  = gdk_imlib_move_image(self->obj);
    mask = gdk_imlib_move_mask(self->obj);
    ret = PyTuple_New(2);
    if (pix)
	PyTuple_SetItem(ret, 0, PyGdkWindow_New(pix));
    else {
	Py_INCREF(Py_None);
	PyTuple_SetItem(ret, 0, Py_None);
    }
    if (mask)
        PyTuple_SetItem(ret, 1, PyGdkWindow_New(mask));
    else {
        Py_INCREF(Py_None);
        PyTuple_SetItem(ret, 1, Py_None);
    }
    if (pix)
	gdk_imlib_free_pixmap(pix);
    return ret;
}

static PyObject *
_wrap_gdk_imlib_get_image_border(PyGdkImlibImage_Object *self, PyObject *args)
{
    GdkImlibBorder border;

    if(!PyArg_ParseTuple(args,":GdkImlibImage.get_image_border"))
        return NULL;
    gdk_imlib_get_image_border(self->obj, &border);
    return Py_BuildValue("(iiii)", border.left, border.right,
			 border.top, border.bottom);
}

static PyObject *
_wrap_gdk_imlib_set_image_border(PyGdkImlibImage_Object *self, PyObject *args)
{
    GdkImlibBorder border;

    if(!PyArg_ParseTuple(args, "iiii:GdkImlibImage.set_image_border",
			 &(border.left), &(border.right),
			 &(border.top), &(border.bottom)))
        return NULL;
    gdk_imlib_set_image_border(self->obj, &border);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gdk_imlib_get_image_shape(PyGdkImlibImage_Object *self, PyObject *args)
{
    GdkImlibColor color;

    if(!PyArg_ParseTuple(args,":GdkImlibImage.get_image_shape"))
        return NULL;
    gdk_imlib_get_image_shape(self->obj, &color);
    return Py_BuildValue("(iii)", color.r, color.g, color.b);
}

static PyObject *
_wrap_gdk_imlib_set_image_shape(PyGdkImlibImage_Object *self, PyObject *args)
{
    GdkImlibColor color;

    if(!PyArg_ParseTuple(args,"iii:GdkImlibImage.set_image_shape",
			 &(color.r), &(color.g), &(color.b)))
        return NULL;
    gdk_imlib_set_image_shape(self->obj, &color);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gdk_imlib_save_image_to_eim(PyGdkImlibImage_Object *self, PyObject *args)
{
    char *file;

    if(!PyArg_ParseTuple(args,"s:GdkImlibImage.save_image_to_eim", &file)) 
        return NULL;
    return PyInt_FromLong(gdk_imlib_save_image_to_eim(self->obj, file));
}

static PyObject *
_wrap_gdk_imlib_add_image_to_eim(PyGdkImlibImage_Object *self, PyObject *args)
{
    char *file;

    if(!PyArg_ParseTuple(args,"s:GdkImlibImage.add_image_to_eim", &file))
        return NULL;
    return PyInt_FromLong(gdk_imlib_add_image_to_eim(self->obj, file));
}

static PyObject *
_wrap_gdk_imlib_save_image_to_ppm(PyGdkImlibImage_Object *self, PyObject *args)
{
    char *file;

    if(!PyArg_ParseTuple(args,"s:GdkImlibImage.save_image_to_ppm", &file)) 
        return NULL;
    return PyInt_FromLong(gdk_imlib_save_image_to_ppm(self->obj, file));
}

static PyObject *
_wrap_gdk_imlib_set_image_modifier(PyGdkImlibImage_Object *self, PyObject*args)
{
    GdkImlibColorModifier mods;

    if(!PyArg_ParseTuple(args,"iii:GdkImlibImage.set_image_modifier",
			 &(mods.gamma), &(mods.brightness), &(mods.contrast)))
        return NULL;
    gdk_imlib_set_image_modifier(self->obj, &mods);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gdk_imlib_set_image_red_modifier(PyGdkImlibImage_Object *self,
				       PyObject *args)
{
    GdkImlibColorModifier mods;

    if(!PyArg_ParseTuple(args,"iii:GdkImlibImage.set_image_red_modifier",
			 &(mods.gamma), &(mods.brightness), &(mods.contrast)))
        return NULL;
    gdk_imlib_set_image_red_modifier(self->obj, &mods);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gdk_imlib_set_image_green_modifier(PyGdkImlibImage_Object *self,
					 PyObject *args)
{
    GdkImlibColorModifier mods;

    if(!PyArg_ParseTuple(args,"iii:GdkImlibImage.set_image_green_modifier",
			 &(mods.gamma), &(mods.brightness), &(mods.contrast)))
        return NULL;
    gdk_imlib_set_image_green_modifier(self->obj, &mods);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gdk_imlib_set_image_blue_modifier(PyGdkImlibImage_Object *self,
					PyObject *args)
{
    GdkImlibColorModifier mods;

    if(!PyArg_ParseTuple(args,"iii:GdkImlibImage.set_image_blue_modifier",
			 &(mods.gamma), &(mods.brightness), &(mods.contrast)))
        return NULL;
    gdk_imlib_set_image_blue_modifier(self->obj, &mods);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gdk_imlib_get_image_modifier(PyGdkImlibImage_Object *self, PyObject*args)
{
    GdkImlibColorModifier mods;

    if(!PyArg_ParseTuple(args,":GdkImlibImage.get_image_modifier"))
        return NULL;
    gdk_imlib_get_image_modifier(self->obj, &mods);
    return Py_BuildValue("(iii)", mods.gamma, mods.brightness, mods.contrast);
}

static PyObject *
_wrap_gdk_imlib_get_image_red_modifier(PyGdkImlibImage_Object *self,
				       PyObject *args)
{
    GdkImlibColorModifier mods;

    if(!PyArg_ParseTuple(args,":GdkImlibImage.get_image_red_modifier"))
        return NULL;
    gdk_imlib_get_image_red_modifier(self->obj, &mods);
    return Py_BuildValue("(iii)", mods.gamma, mods.brightness, mods.contrast);
}

static PyObject *
_wrap_gdk_imlib_get_image_green_modifier(PyGdkImlibImage_Object *self,
					 PyObject *args)
{
    GdkImlibColorModifier mods;

    if(!PyArg_ParseTuple(args,":GdkImlibImage.get_image_green_modifier"))
        return NULL;
    gdk_imlib_get_image_green_modifier(self->obj, &mods);
    return Py_BuildValue("(iii)", mods.gamma, mods.brightness, mods.contrast);
}

static PyObject *
_wrap_gdk_imlib_get_image_blue_modifier(PyGdkImlibImage_Object *self,
					PyObject *args)
{
    GdkImlibColorModifier mods;

    if(!PyArg_ParseTuple(args,":GdkImlibImage.get_image_blue_modifier"))
        return NULL;
    gdk_imlib_get_image_blue_modifier(self->obj, &mods);
    return Py_BuildValue("(iii)", mods.gamma, mods.brightness, mods.contrast);
}

static PyObject *
_wrap_gdk_imlib_set_image_red_curve(PyGdkImlibImage_Object *self,
				    PyObject *args)
{
    PyObject *list, *item;
    unsigned char mod[256];
    int i;

    if(!PyArg_ParseTuple(args,"O:GdkImlibImage.set_image_red_curve", &list)) 
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
    gdk_imlib_set_image_red_curve(self->obj, mod);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gdk_imlib_set_image_green_curve(PyGdkImlibImage_Object *self,
				      PyObject *args)
{
    PyObject *list, *item;
    unsigned char mod[256];
    int i;

    if(!PyArg_ParseTuple(args,"O:GdkImlibImage.set_image_green_curve", &list)) 
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
    gdk_imlib_set_image_green_curve(self->obj, mod);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gdk_imlib_set_image_blue_curve(PyGdkImlibImage_Object *self,
				     PyObject *args)
{
    PyObject *list, *item;
    unsigned char mod[256];
    int i;

    if(!PyArg_ParseTuple(args,"O:GdkImlibImage.set_image_blue_curve", &list)) 
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
    gdk_imlib_set_image_blue_curve(self->obj, mod);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gdk_imlib_get_image_red_curve(PyGdkImlibImage_Object *self,
				    PyObject *args)
{
    PyObject *ret;
    unsigned char mod[256];
    int i;

    if(!PyArg_ParseTuple(args,":GdkImlibImage.get_image_red_curve"))
        return NULL;
    gdk_imlib_get_image_red_curve(self->obj, mod);
    ret = PyList_New(256);
    for (i = 0; i < 256; i++)
        PyList_SetItem(ret, i, PyInt_FromLong(mod[i]));
    return ret;
}

static PyObject *
_wrap_gdk_imlib_get_image_green_curve(PyGdkImlibImage_Object *self,
				      PyObject *args)
{
    PyObject *ret;
    unsigned char mod[256];
    int i;

    if(!PyArg_ParseTuple(args,":GdkImlibImage.get_image_green_curve"))
        return NULL;
    gdk_imlib_get_image_green_curve(self->obj, mod);
    ret = PyList_New(256);
    for (i = 0; i < 256; i++)
        PyList_SetItem(ret, i, PyInt_FromLong(mod[i]));
    return ret;
}

static PyObject *
_wrap_gdk_imlib_get_image_blue_curve(PyGdkImlibImage_Object *self,
				     PyObject *args)
{
    PyObject *ret;
    unsigned char mod[256];
    int i;

    if(!PyArg_ParseTuple(args,":GdkImlibImage.get_image_blue_curve"))
        return NULL;
    gdk_imlib_get_image_blue_curve(self->obj, mod);
    ret = PyList_New(256);
    for (i = 0; i < 256; i++)
        PyList_SetItem(ret, i, PyInt_FromLong(mod[i]));
    return ret;
}

static PyObject *
_wrap_gdk_imlib_apply_modifiers_to_rgb(PyGdkImlibImage_Object *self,
				       PyObject *args)
{
    if(!PyArg_ParseTuple(args,":GdkImlibImage.apply_modifiers_to_rgb"))
        return NULL;
    gdk_imlib_apply_modifiers_to_rgb(self->obj);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gdk_imlib_changed_image(PyGdkImlibImage_Object *self, PyObject *args)
{
    if(!PyArg_ParseTuple(args,":GdkImlibImage.changed_image"))
        return NULL;
    gdk_imlib_changed_image(self->obj);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gdk_imlib_flip_image_horizontal(PyGdkImlibImage_Object *self,
				      PyObject *args)
{
    if(!PyArg_ParseTuple(args,":GdkImlibImage.flip_image_horizontal"))
        return NULL;
    gdk_imlib_flip_image_horizontal(self->obj);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gdk_imlib_flip_image_vertical(PyGdkImlibImage_Object *self,
				    PyObject *args)
{
    if(!PyArg_ParseTuple(args,":GdkImlibImage.flip_image_vertical"))
        return NULL;
    gdk_imlib_flip_image_vertical(self->obj);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gdk_imlib_rotate_image(PyGdkImlibImage_Object *self, PyObject *args)
{
    int degrees;

    if(!PyArg_ParseTuple(args,"i:GdkImlibImage.rotate_image", &degrees))
        return NULL;
    gdk_imlib_rotate_image(self->obj, degrees);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gdk_imlib_clone_image(PyGdkImlibImage_Object *self, PyObject *args)
{
    if(!PyArg_ParseTuple(args,":GdkImlibImage.clone_image"))
        return NULL;
    return PyGdkImlibImage_New(gdk_imlib_clone_image(self->obj));
}

static PyObject *
_wrap_gdk_imlib_clone_scaled_image(PyGdkImlibImage_Object *self,
				   PyObject *args)
{
    int width, height;

    if(!PyArg_ParseTuple(args,"ii:GdkImlibImage.clone_scaled_image",
			 &width, &height)) 
        return NULL;
    return PyGdkImlibImage_New(gdk_imlib_clone_scaled_image(self->obj,
							    width, height));
}

static PyObject *
_wrap_gdk_imlib_crop_image(PyGdkImlibImage_Object *self, PyObject *args)
{
    int x, y, w, h;

    if(!PyArg_ParseTuple(args,"iiii:GdkImlibImage.crop_image", &x, &y, &w, &h))
        return NULL;
    gdk_imlib_crop_image(self->obj, x, y, w, h);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gdk_imlib_save_image(PyGdkImlibImage_Object *self, PyObject *args)
{
    char *file;

    if(!PyArg_ParseTuple(args,"s:GdkImlibImage.save_image", &file))
        return NULL;
    return PyInt_FromLong(gdk_imlib_save_image(self->obj, file, NULL));
}

static PyObject *
_wrap_gdk_imlib_crop_and_clone_image(PyGdkImlibImage_Object *self,
				     PyObject *args)
{
    int x, y, w, h;

    if (!PyArg_ParseTuple(args, "iiii:GdkImlibImage.crop_and_clone_image",
			  &x, &y, &w, &h))
        return NULL;
    return PyGdkImlibImage_New(gdk_imlib_crop_and_clone_image(self->obj,
							      x, y, w, h));
}

#ifdef HAVE_NUMPY
static PyObject *
_wrap_gdk_imlib_image_get_array(PyGdkImlibImage_Object *self, PyObject *args)
{
    GdkImlibImage *obj;
    PyArrayObject *ap_data;
    int dims[3] = {0, 0, 3};

    if (!PyArg_ParseTuple(args, ":GdkImlibImage.get_array"))
	return NULL;
    dims[0] = (int)self->obj->rgb_width;
    dims[1] = (int)self->obj->rgb_height;

    ap_data = (PyArrayObject *)PyArray_FromDimsAndData(3, dims, PyArray_UBYTE,
						(char *)self->obj->rgb_data);
    if (ap_data == NULL)
	return NULL;
    /* this makes sure the GdkImlibImage is not freed while someone has a
     * reference to the new array. */
    Py_INCREF(self);
    ap_data->base = (PyObject *)self;

    return PyArray_Return(ap_data);
}
#endif

static PyMethodDef pygdk_imlib_image_methods[] = {
    { "crop_and_clone_image", (PyCFunction)_wrap_gdk_imlib_crop_and_clone_image, 1 },
    { "save_image", (PyCFunction)_wrap_gdk_imlib_save_image, 1 },
    { "crop_image", (PyCFunction)_wrap_gdk_imlib_crop_image, 1 },
    { "clone_scaled_image", (PyCFunction)_wrap_gdk_imlib_clone_scaled_image, 1 },
    { "clone_image", (PyCFunction)_wrap_gdk_imlib_clone_image, 1 },
    { "rotate_image", (PyCFunction)_wrap_gdk_imlib_rotate_image, 1 },
    { "flip_image_vertical", (PyCFunction)_wrap_gdk_imlib_flip_image_vertical, 1 },
    { "flip_image_horizontal", (PyCFunction)_wrap_gdk_imlib_flip_image_horizontal, 1 },
    { "changed_image", (PyCFunction)_wrap_gdk_imlib_changed_image, 1 },
    { "apply_modifiers_to_rgb", (PyCFunction)_wrap_gdk_imlib_apply_modifiers_to_rgb, 1 },
    { "get_image_blue_curve", (PyCFunction)_wrap_gdk_imlib_get_image_blue_curve, 1 },
    { "get_image_green_curve", (PyCFunction)_wrap_gdk_imlib_get_image_green_curve, 1 },
    { "get_image_red_curve", (PyCFunction)_wrap_gdk_imlib_get_image_red_curve, 1 },
    { "set_image_blue_curve", (PyCFunction)_wrap_gdk_imlib_set_image_blue_curve, 1 },
    { "set_image_green_curve", (PyCFunction)_wrap_gdk_imlib_set_image_green_curve, 1 },
    { "set_image_red_curve", (PyCFunction)_wrap_gdk_imlib_set_image_red_curve, 1 },
    { "get_image_blue_modifier", (PyCFunction)_wrap_gdk_imlib_get_image_blue_modifier, 1 },
    { "get_image_green_modifier", (PyCFunction)_wrap_gdk_imlib_get_image_green_modifier, 1},
    { "get_image_red_modifier", (PyCFunction)_wrap_gdk_imlib_get_image_red_modifier, 1 },
    { "get_image_modifier", (PyCFunction)_wrap_gdk_imlib_get_image_modifier, 1 },
    { "set_image_blue_modifier", (PyCFunction)_wrap_gdk_imlib_set_image_blue_modifier, 1 },
    { "set_image_green_modifier", (PyCFunction)_wrap_gdk_imlib_set_image_green_modifier, 1},
    { "set_image_red_modifier", (PyCFunction)_wrap_gdk_imlib_set_image_red_modifier, 1 },
    { "set_image_modifier", (PyCFunction)_wrap_gdk_imlib_set_image_modifier, 1 },
    { "save_image_to_ppm", (PyCFunction)_wrap_gdk_imlib_save_image_to_ppm, 1 },
    { "add_image_to_eim", (PyCFunction)_wrap_gdk_imlib_add_image_to_eim, 1 },
    { "save_image_to_eim", (PyCFunction)_wrap_gdk_imlib_save_image_to_eim, 1 },
    { "set_image_shape", (PyCFunction)_wrap_gdk_imlib_set_image_shape, 1 },
    { "get_image_shape", (PyCFunction)_wrap_gdk_imlib_get_image_shape, 1 },
    { "set_image_border", (PyCFunction)_wrap_gdk_imlib_set_image_border, 1 },
    { "get_image_border", (PyCFunction)_wrap_gdk_imlib_get_image_border, 1 },
    { "get_pixmap", (PyCFunction)_wrap_gdk_imlib_get_pixmap, 1 },
    { "render", (PyCFunction)_wrap_gdk_imlib_render, 1 },
#ifdef HAVE_NUMPY
    { "get_array", (PyCFunction)_wrap_gdk_imlib_image_get_array, 1 },
#endif
    
    { NULL, NULL, 0 }
};

/* ------------- end of GdkImlibImage methods ------------ */

static PyObject *
pygdk_imlib_image_getattr(PyGdkImlibImage_Object *self, char *attr)
{
    if (!strcmp(attr, "__members__"))
	return Py_BuildValue("[sss]", "filename", "rgb_height", "rgb_width");
    else if (!strcmp(attr, "rgb_width"))
	return PyInt_FromLong(self->obj->rgb_width);
    else if (!strcmp(attr, "rgb_height"))
	return PyInt_FromLong(self->obj->rgb_height);
    else if (!strcmp(attr, "filename"))
	return PyString_FromString(self->obj->filename);

    return Py_FindMethod(pygdk_imlib_image_methods, (PyObject *)self, attr);
}


static PyTypeObject PyGdkImlibImage_Type = {
    PyObject_HEAD_INIT(&PyType_Type)
    0,
    "GdkImlibImage",
    sizeof(PyGdkImlibImage_Object),
    0,
    (destructor)pygdk_imlib_image_dealloc,
    (printfunc)0,
    (getattrfunc)pygdk_imlib_image_getattr,
    (setattrfunc)0,
    (cmpfunc)pygdk_imlib_image_compare,
    (reprfunc)0,
    0,
    0,
    0,
    (hashfunc)pygdk_imlib_image_hash,
    (ternaryfunc)0,
    (reprfunc)0,
    0L,0L,0L,0L,
    NULL
};


static PyObject *
_wrap_gdk_imlib_get_render_type(PyObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args,":get_render_type")) 
        return NULL;
    return PyInt_FromLong(gdk_imlib_get_render_type());
}

static PyObject *
_wrap_gdk_imlib_set_render_type(PyObject *self, PyObject *args)
{
    int rend_type;

    if (!PyArg_ParseTuple(args,"i:set_render_type", &rend_type)) 
        return NULL;
    gdk_imlib_set_render_type(rend_type);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gdk_imlib_load_colors(PyObject *self, PyObject *args)
{
    char *file;

    if (!PyArg_ParseTuple(args,"s:load_colors",&file)) 
        return NULL;
    return PyInt_FromLong(gdk_imlib_load_colors(file));
}

static PyObject *
_wrap_gdk_imlib_load_image(PyObject *self, PyObject *args)
{
    char *file;

    if(!PyArg_ParseTuple(args,"s:GdkImlibImage",&file)) 
        return NULL;
    return PyGdkImlibImage_New(gdk_imlib_load_image(file));
}

static PyObject *
_wrap_gdk_imlib_best_color_match(PyObject *self, PyObject *args)
{
    int r, g, b;
    if (!PyArg_ParseTuple(args, "iii:best_color_match", &r, &g, &b))
	return NULL;
    gdk_imlib_best_color_match(&r, &g, &b);
    return Py_BuildValue("(iii)", r, g, b);
}

static PyObject *
_wrap_gdk_imlib_free_colors(PyObject *self, PyObject *args)
{
    if(!PyArg_ParseTuple(args,":free_colors")) 
        return NULL;
    gdk_imlib_free_colors();
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gdk_imlib_create_image_from_data(PyObject *self, PyObject *args)
{
    unsigned char *data, *alpha;
    int w, h, ndata, nalpha;

    if (!PyArg_ParseTuple(args, "s#z#ii:create_image_from_data",
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

static PyObject *
_wrap_gdk_imlib_create_image_from_xpm(PyObject *self, PyObject *args)
{
    PyObject *lines;
    int len, i;
    char **data;

    if (!PyArg_ParseTuple(args, "O!:create_image_from_xpm",
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

static PyObject *
_wrap_gdk_imlib_get_fallback(PyObject *self, PyObject *args)
{
    if(!PyArg_ParseTuple(args,":get_fallback")) 
        return NULL;
    return PyInt_FromLong(gdk_imlib_get_fallback());
}

static PyObject *
_wrap_gdk_imlib_set_fallback(PyObject *self, PyObject *args)
{
    int fallback;

    if(!PyArg_ParseTuple(args,"i:set_fallback",&fallback)) 
        return NULL;
    gdk_imlib_set_fallback(fallback);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gdk_imlib_get_sysconfig(PyObject *self, PyObject *args)
{
    if(!PyArg_ParseTuple(args,":get_sysconfig")) 
        return NULL;
    return PyString_FromString(gdk_imlib_get_sysconfig());
}

static PyObject *
_wrap_gdk_imlib_create_image_from_drawable(PyObject *self, PyObject *args)
{
    PyObject *win, *py_mask;
    int x, y, w, h;
    GdkBitmap *mask = NULL;

    if (!PyArg_ParseTuple(args, "O!Oiiii:create_image_from_drawable",
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

static PyObject *
_wrap_gdk_imlib_push_visual(PyObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":push_visual"))
        return NULL;
    gtk_widget_push_visual(gdk_imlib_get_visual());
    gtk_widget_push_colormap(gdk_imlib_get_colormap());
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gdk_imlib_pop_visual(PyObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":pop_visual"))
        return NULL;
    gtk_widget_pop_visual();
    gtk_widget_pop_colormap();
    Py_INCREF(Py_None);
    return Py_None;
}

#ifdef HAVE_NUMPY
static PyObject *
_wrap_gdk_imlib_create_image_from_array(PyObject *self, PyObject *args)
{
    PyArrayObject *ap_data, *ap_alpha=NULL;
    unsigned char *data, *alpha = NULL;
    int w, h;
    if (!PyArg_ParseTuple(args, "O!|O!:create_image_from_array",
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

    if (ap_data != NULL && (!PyArray_ISCONTIGUOUS(ap_alpha) ||
			    ap_alpha->descr->type_num != PyArray_UBYTE ||
			    ap_alpha->nd != 2 ||
			    ap_alpha->dimensions[0] != h ||
			    ap_alpha->dimensions[1] != w)) {
	PyErr_SetString(PyExc_TypeError,
		"alpha channel array must be 2D contiguous byte array");
	return NULL;
    }
    if (ap_alpha != NULL)
	alpha = (unsigned char *)ap_alpha->data;
    return PyGdkImlibImage_New(gdk_imlib_create_image_from_data(
						data, alpha, w, h));
}
#endif

static PyMethodDef _gdkimlibMethods[] = {
    { "push_visual", _wrap_gdk_imlib_push_visual, 1 },
    { "pop_visual", _wrap_gdk_imlib_pop_visual, 1 },
    { "create_image_from_drawable", _wrap_gdk_imlib_create_image_from_drawable, 1 },
    { "get_sysconfig", _wrap_gdk_imlib_get_sysconfig, 1 },
    { "set_fallback", _wrap_gdk_imlib_set_fallback, 1 },
    { "get_fallback", _wrap_gdk_imlib_get_fallback, 1 },
    { "create_image_from_data", _wrap_gdk_imlib_create_image_from_data, 1 },
    { "create_image_from_xpm", _wrap_gdk_imlib_create_image_from_xpm, 1 },
    { "free_colors", _wrap_gdk_imlib_free_colors, 1 },
    { "best_color_match", _wrap_gdk_imlib_best_color_match, 1 },
    { "GdkImlibImage", _wrap_gdk_imlib_load_image, 1 },
    { "load_colors", _wrap_gdk_imlib_load_colors, 1 },
    { "set_render_type", _wrap_gdk_imlib_set_render_type, 1 },
    { "get_render_type", _wrap_gdk_imlib_get_render_type, 1 },
#ifdef HAVE_NUMPY
    { "create_image_from_array", _wrap_gdk_imlib_create_image_from_data, 1 },
#endif
    { NULL, NULL }
};

static struct _PyGdkImlib_FunctionStruct functions = {
    &PyGdkImlibImage_Type, PyGdkImlibImage_New
};

void initimlib() {
    PyObject *m, *d, *o;

    m = Py_InitModule("imlib", _gdkimlibMethods);
    d = PyModule_GetDict(m);

#ifdef HAVE_NUMPY
    import_array();
#endif
    
    PyDict_SetItemString(d, "GdkImlibImageType",
			 (PyObject *)&PyGdkImlibImage_Type);

    PyDict_SetItemString(d, "_PyGdkI_API",
			 PyCObject_FromVoidPtr(&functions, NULL));

#define add_const(cnst) PyDict_SetItemString(d, #cnst,o=PyInt_FromLong(cnst)); Py_DECREF(o)

    add_const(RT_PLAIN_PALETTE);
    add_const(RT_PLAIN_PALETTE_FAST);
    add_const(RT_DITHER_PALETTE);
    add_const(RT_DITHER_PALETTE_FAST);
    add_const(RT_PLAIN_TRUECOL);
    add_const(RT_DITHER_TRUECOL);

    /* get references to the pygtk types and functions */
    init_pygtk();

    gdk_imlib_init();

    if (PyErr_Occurred())
         Py_FatalError("can't initialise module gtk.imlib");
}
