/* -*- Mode: C; c-basic-offset: 4 -*- */
/* PyGTK gdk-pixbuf module - python bindings for gdk-pixbuf
 * Copyright (C) 2001-2002 James Henstridge <james@daa.com.au>
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
#include <gdk-pixbuf/gdk-pixbuf.h>

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

#define _INSIDE_PYGDKPIXBUF_
#include "pygdkpixbuf.h"

static PyObject *
PyGdkPixbuf_New(GdkPixbuf *pixbuf)
{
    PyGdkPixbuf_Object *self;

    if (pixbuf == NULL) {
	PyErr_SetString(PyExc_RuntimeError, "couldn't create pixbuf");
	return NULL;
    }

    self = (PyGdkPixbuf_Object *)PyObject_NEW(PyGdkPixbuf_Object,
					      &PyGdkPixbuf_Type);

    if (self == NULL)
	return NULL;
    self->pixbuf = gdk_pixbuf_ref(pixbuf);
    return (PyObject *)self;
}

static void
pygdk_pixbuf_dealloc(PyGdkPixbuf_Object *self)
{
    gdk_pixbuf_unref(self->pixbuf);
    PyMem_DEL(self);
}

static int
pygdk_pixbuf_compare(PyGdkPixbuf_Object *self, PyGdkPixbuf_Object *v)
{
    if (self->pixbuf == v->pixbuf) return 0;
    if (self->pixbuf > v->pixbuf) return -1;
    return 1;
}

static long
pygdk_pixbuf_hash(PyGdkPixbuf_Object *self)
{
    return (long)self->pixbuf;
}

/* --- methods --- */

static PyObject *
pygdk_pixbuf_copy(PyGdkPixbuf_Object *self, PyObject *args)
{
    GdkPixbuf *copy;
    PyObject *ret;

    if (!PyArg_ParseTuple(args, ":GdkPixbuf.copy"))
	return NULL;

    copy = gdk_pixbuf_copy(self->pixbuf);

    if (!copy) {
	PyErr_SetString(PyExc_RuntimeError, "could not make copy");
	return NULL;
    }
    ret = PyGdkPixbuf_New(copy);
    gdk_pixbuf_unref(copy);
    return ret;
}

static PyObject *
pygdk_pixbuf_add_alpha(PyGdkPixbuf_Object *self, PyObject *args)
{
    GdkPixbuf *pixbuf;
    gboolean substitute_alpha = FALSE;
    gint r = 0, g = 0, b = 0;
    PyObject *ret;

    if (!PyArg_ParseTuple(args, "|iiii:GdkPixbuf.add_alpha",
			  &substitute_alpha, &r, &g, &b))
	return NULL;

    pixbuf = gdk_pixbuf_add_alpha(self->pixbuf, substitute_alpha, r, g, b);

    if (!pixbuf) {
	PyErr_SetString(PyExc_RuntimeError, "could not create new pixbuf");
	return NULL;
    }
    ret = PyGdkPixbuf_New(pixbuf);
    gdk_pixbuf_unref(pixbuf);
    return ret;
}

static PyObject *
pygdk_pixbuf_copy_area(PyGdkPixbuf_Object *self, PyObject *args)
{
    int src_x, src_y, width, height, dest_x = 0, dest_y = 0;
    PyGdkPixbuf_Object *dest_pixbuf;

    if (!PyArg_ParseTuple(args, "iiiiO!|ii:GdkPixbuf.copy_area",
			  &src_x, &src_y, &width, &height,
			  &PyGdkPixbuf_Type, &dest_pixbuf,
			  &dest_x, &dest_y))
	return NULL;
    gdk_pixbuf_copy_area(self->pixbuf, src_x, src_y, width, height,
			 dest_pixbuf->pixbuf, dest_x, dest_y);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
pygdk_pixbuf_render_threshold_alpha(PyGdkPixbuf_Object *self, PyObject *args)
{
    PyObject *bitmap;
    int src_x = 0, src_y = 0, dest_x = 0, dest_y = 0;
    int width, height, alpha_threshold = 127;

    width = gdk_pixbuf_get_width(self->pixbuf);
    height = gdk_pixbuf_get_height(self->pixbuf);
    if (!PyArg_ParseTuple(args, "O!|iiiiiii:GdkPixbuf.render_threshold_alpha",
			  &PyGdkWindow_Type, &bitmap, &src_x, &src_y,
			  &dest_x, &dest_y, &width, &height, &alpha_threshold))
	return NULL;
    gdk_pixbuf_render_threshold_alpha(self->pixbuf, PyGdkWindow_Get(bitmap),
				      src_x, src_y, dest_x, dest_y,
				      width, height, alpha_threshold);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
pygdk_pixbuf_render_to_drawable(PyGdkPixbuf_Object *self, PyObject *args)
{
    PyObject *drawable, *gc;
    int src_x = 0, src_y = 0, dest_x = 0, dest_y = 0, width, height;
    GdkRgbDither dither = GDK_RGB_DITHER_NORMAL;
    int x_dither = 0, y_dither = 0;

    width = gdk_pixbuf_get_width(self->pixbuf);
    height = gdk_pixbuf_get_height(self->pixbuf);
    if (!PyArg_ParseTuple(args, "O!O!|iiiiiiiii:GdkPixbuf.render_to_drawable",
			  &PyGdkWindow_Type, &drawable, &PyGdkGC_Type, &gc,
			  &src_x, &src_y, &dest_x, &dest_y, &width, &height,
			  &dither, &x_dither, &y_dither))
	return NULL;
    gdk_pixbuf_render_to_drawable(self->pixbuf, PyGdkWindow_Get(drawable),
				  PyGdkGC_Get(gc), src_x, src_y,
				  dest_x, dest_y, width, height,
				  dither, x_dither, y_dither);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
pygdk_pixbuf_render_to_drawable_alpha(PyGdkPixbuf_Object *self, PyObject *args)
{
    PyObject *drawable;
    int src_x = 0, src_y = 0, dest_x = 0, dest_y = 0, width, height;
    GdkPixbufAlphaMode alpha_mode = GDK_PIXBUF_ALPHA_BILEVEL;
    int alpha_threshold = 127;
    GdkRgbDither dither = GDK_RGB_DITHER_NORMAL;
    int x_dither = 0, y_dither = 0;

    width = gdk_pixbuf_get_width(self->pixbuf);
    height = gdk_pixbuf_get_height(self->pixbuf);
    if (!PyArg_ParseTuple(args,
			 "O!|iiiiiiiiiii:GdkPixbuf.render_to_drawable_alpha",
			  &PyGdkWindow_Type, &drawable,
			  &src_x, &src_y, &dest_x, &dest_y, &width, &height,
			  &alpha_mode, &alpha_threshold,
			  &dither, &x_dither, &y_dither))
	return NULL;
    gdk_pixbuf_render_to_drawable_alpha(self->pixbuf,
					PyGdkWindow_Get(drawable),
					src_x, src_y, dest_x, dest_y,
					width, height,
					alpha_mode, alpha_threshold,
					dither, x_dither, y_dither);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
pygdk_pixbuf_render_pixmap_and_mask(PyGdkPixbuf_Object *self, PyObject *args)
{
    GdkPixmap *pixmap;
    GdkBitmap *mask;
    int alpha_threshold = 127;
    PyObject *pypixmap, *pymask;

    if (!PyArg_ParseTuple(args, "|i:GdkPixbuf.render_pixmap_and_mask",
			  &alpha_threshold))
	return NULL;
    gdk_pixbuf_render_pixmap_and_mask(self->pixbuf, &pixmap, &mask,
				      alpha_threshold);
    if (pixmap) {
	pypixmap = PyGdkWindow_New(pixmap);
	gdk_pixmap_unref(pixmap);
    } else {
	Py_INCREF(Py_None);
	pypixmap = Py_None;
    }
    if (mask) {
	pymask = PyGdkWindow_New(mask);
	gdk_bitmap_unref(mask);
    } else {
	Py_INCREF(Py_None);
	pymask = Py_None;
    }
    return Py_BuildValue("(NN)", pypixmap, pymask);
}

static PyObject *
pygdk_pixbuf_get_from_drawable(PyGdkPixbuf_Object *self, PyObject *args)
{
    PyObject *drawable, *colormap;
    int src_x, src_y, dest_x, dest_y, width, height;

    if (!PyArg_ParseTuple(args, "O!O!iiiiii:GdkPixbuf.get_from_drawable",
			  &PyGdkWindow_Type, &drawable,
			  &PyGdkColormap_Type, &colormap,
			  &src_x, &src_y, &dest_x, &dest_y, &width, &height))
	return NULL;
    gdk_pixbuf_get_from_drawable(self->pixbuf, PyGdkWindow_Get(drawable),
				 PyGdkColormap_Get(colormap), src_x, src_y,
				 dest_x, dest_y, width, height);
    Py_INCREF(self);
    return (PyObject *)self;
}

static PyObject *
pygdk_pixbuf_scale(PyGdkPixbuf_Object *self, PyObject *args)
{
    PyGdkPixbuf_Object *dest;
    int dest_x, dest_y, dest_width, dest_height;
    int offset_x, offset_y, scale_x, scale_y;
    GdkInterpType interp_type;

    if (!PyArg_ParseTuple(args, "O!iiiiiiiii:GdkPixbuf.scale",
			  &PyGdkPixbuf_Type, &dest, &dest_x, &dest_y,
			  &dest_width, &dest_height, &offset_x, &offset_y,
			  &scale_x, &scale_y, &interp_type))
	return NULL;
    gdk_pixbuf_scale(self->pixbuf, dest->pixbuf, dest_x, dest_y,
		     dest_width, dest_height, offset_x, offset_y,
		     scale_x, scale_y, interp_type);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
pygdk_pixbuf_composite(PyGdkPixbuf_Object *self, PyObject *args)
{
    PyGdkPixbuf_Object *dest;
    int dest_x, dest_y, dest_width, dest_height;
    int offset_x, offset_y, scale_x, scale_y;
    GdkInterpType interp_type;
    int overall_alpha;

    if (!PyArg_ParseTuple(args, "O!iiiiiiiiii:GdkPixbuf.composite",
			  &PyGdkPixbuf_Type, &dest, &dest_x, &dest_y,
			  &dest_width, &dest_height, &offset_x, &offset_y,
			  &scale_x, &scale_y, &interp_type, &overall_alpha))
	return NULL;
    gdk_pixbuf_composite(self->pixbuf, dest->pixbuf, dest_x, dest_y,
			 dest_width, dest_height, offset_x, offset_y,
			 scale_x, scale_y, interp_type, overall_alpha);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
pygdk_pixbuf_composite_color(PyGdkPixbuf_Object *self, PyObject *args)
{
    PyGdkPixbuf_Object *dest;
    int dest_x, dest_y, dest_width, dest_height;
    int offset_x, offset_y, scale_x, scale_y;
    GdkInterpType interp_type;
    int overall_alpha, check_x, check_y, check_size;
    int color1, color2;

    if (!PyArg_ParseTuple(args, "O!iiiiiiiiiiiiiii:GdkPixbuf.composite_color",
			  &PyGdkPixbuf_Type, &dest, &dest_x, &dest_y,
			  &dest_width, &dest_height, &offset_x, &offset_y,
			  &scale_x, &scale_y, &interp_type, &overall_alpha,
			  &check_x, &check_y, &check_size, &color1, &color2))
	return NULL;
    gdk_pixbuf_composite_color(self->pixbuf, dest->pixbuf, dest_x, dest_y,
			       dest_width, dest_height, offset_x, offset_y,
			       scale_x, scale_y, interp_type, overall_alpha,
			       check_x, check_y, check_size, color1, color2);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
pygdk_pixbuf_scale_simple(PyGdkPixbuf_Object *self, PyObject *args)
{
    GdkPixbuf *pixbuf;
    PyObject *ret;
    int dest_width, dest_height;
    GdkInterpType interp_type;

    if (!PyArg_ParseTuple(args, "iii:GdkPixbuf.scale_simple",
			  &dest_width, &dest_height, &interp_type))
	return NULL;
    pixbuf = gdk_pixbuf_scale_simple(self->pixbuf, dest_width, dest_height,
				     interp_type);
    if (!pixbuf) {
	PyErr_SetString(PyExc_RuntimeError, "could not create new pixbuf");
	return NULL;
    }
    ret = PyGdkPixbuf_New(pixbuf);
    gdk_pixbuf_unref(pixbuf);
    return ret;
}

static PyObject *
pygdk_pixbuf_composite_color_simple(PyGdkPixbuf_Object *self, PyObject *args)
{
    GdkPixbuf *pixbuf;
    PyObject *ret;
    int dest_width, dest_height;
    GdkInterpType interp_type;
    int overall_alpha, check_size;
    int color1, color2;

    if (!PyArg_ParseTuple(args, "iiiiiii:GdkPixbuf.composite_color_simple",
			  &dest_width, &dest_height, &interp_type,
			  &overall_alpha, &check_size, &color1, &color2))
	return NULL;
    pixbuf = gdk_pixbuf_composite_color_simple(self->pixbuf, dest_width,
					       dest_height, interp_type,
					       overall_alpha, check_size,
					       color1, color2);
    if (!pixbuf) {
	PyErr_SetString(PyExc_RuntimeError, "could not create new pixbuf");
	return NULL;
    }
    ret = PyGdkPixbuf_New(pixbuf);
    gdk_pixbuf_unref(pixbuf);
    return ret;
}

static PyMethodDef pygdk_pixbuf_methods[] = {
    { "copy", (PyCFunction)pygdk_pixbuf_copy, METH_VARARGS, NULL },
    { "add_alpha", (PyCFunction)pygdk_pixbuf_add_alpha, METH_VARARGS, NULL },
    { "copy_area", (PyCFunction)pygdk_pixbuf_copy_area, METH_VARARGS, NULL },
    { "render_threshold_alpha", (PyCFunction)pygdk_pixbuf_render_threshold_alpha, METH_VARARGS, NULL },
    { "render_to_drawable", (PyCFunction)pygdk_pixbuf_render_to_drawable, METH_VARARGS, NULL }, 
    { "render_to_drawable_alpha", (PyCFunction)pygdk_pixbuf_render_to_drawable_alpha, METH_VARARGS, NULL },
    { "render_pixmap_and_mask", (PyCFunction)pygdk_pixbuf_render_pixmap_and_mask, METH_VARARGS, NULL },
    { "get_from_drawable", (PyCFunction)pygdk_pixbuf_get_from_drawable, METH_VARARGS, NULL },
    { "scale", (PyCFunction)pygdk_pixbuf_scale, METH_VARARGS, NULL },
    { "composite", (PyCFunction)pygdk_pixbuf_composite, METH_VARARGS, NULL },
    { "composite_color", (PyCFunction)pygdk_pixbuf_composite_color, METH_VARARGS, NULL },
    { "scale_simple", (PyCFunction)pygdk_pixbuf_scale_simple, METH_VARARGS, NULL },
    { "composite_color_simple", (PyCFunction)pygdk_pixbuf_composite_color_simple, METH_VARARGS, NULL },
    { NULL, 0, 0, NULL }
};

static PyObject *
pygdk_pixbuf_getattr(PyGdkPixbuf_Object *self, char *attr)
{
    if (!strcmp(attr, "__members__"))
	return Py_BuildValue("[sssssssss]", "bits_per_sample", "colorspace",
			     "has_alpha", "height", "n_channels", "pixels",
			     "pixel_array", "rowstride", "width");
    if (!strcmp(attr, "colorspace"))
	return PyInt_FromLong(gdk_pixbuf_get_colorspace(self->pixbuf));
    if (!strcmp(attr, "n_channels"))
	return PyInt_FromLong(gdk_pixbuf_get_n_channels(self->pixbuf));
    if (!strcmp(attr, "has_alpha"))
	return PyInt_FromLong(gdk_pixbuf_get_has_alpha(self->pixbuf));
    if (!strcmp(attr, "bits_per_sample"))
	return PyInt_FromLong(gdk_pixbuf_get_bits_per_sample(self->pixbuf));
    if (!strcmp(attr, "pixels"))
	return PyString_FromStringAndSize(gdk_pixbuf_get_pixels(self->pixbuf),
				gdk_pixbuf_get_height(self->pixbuf) *
				gdk_pixbuf_get_rowstride(self->pixbuf));
#ifdef HAVE_NUMPY
    if (!strcmp(attr, "pixel_array")) {
	PyArrayObject *array;
	int dims[3] = { 0, 0, 3 };

	dims[0] = gdk_pixbuf_get_width(self->pixbuf);
	dims[1] = gdk_pixbuf_get_height(self->pixbuf);
	if (gdk_pixbuf_get_has_alpha(self->pixbuf))
	    dims[2] = 4;
	array = (PyArrayObject *)PyArray_FromDimsAndData(3, dims,
				PyArray_UBYTE,
				(char *)gdk_pixbuf_get_pixels(self->pixbuf));
	if (array == NULL)
	    return NULL;

	/* set the first index stride to the rowstride of the pixbuf */
	array->strides[0] = gdk_pixbuf_get_rowstride(self->pixbuf);
	/* array holds reference to pixbuf ... */
	Py_INCREF(self);
	array->base = (PyObject *)self;
	return PyArray_Return(array);
    }
#endif
    if (!strcmp(attr, "width"))
	return PyInt_FromLong(gdk_pixbuf_get_width(self->pixbuf));
    if (!strcmp(attr, "height"))
	return PyInt_FromLong(gdk_pixbuf_get_height(self->pixbuf));
    if (!strcmp(attr, "rowstride"))
	return PyInt_FromLong(gdk_pixbuf_get_rowstride(self->pixbuf));

    return Py_FindMethod(pygdk_pixbuf_methods, (PyObject *)self, attr);
}

static PyTypeObject PyGdkPixbuf_Type = {
    PyObject_HEAD_INIT(&PyType_Type)
    0,
    "GdkPixbuf",
    sizeof(PyGdkPixbuf_Object),
    0,
    (destructor)pygdk_pixbuf_dealloc,
    (printfunc)0,
    (getattrfunc)pygdk_pixbuf_getattr,
    (setattrfunc)0,
    (cmpfunc)pygdk_pixbuf_compare,
    (reprfunc)0,
    0,
    0,
    0,
    (hashfunc)pygdk_pixbuf_hash,
    (ternaryfunc)0,
    (reprfunc)0,
    0L,0L,0L,0L,
    NULL
};

static PyObject *
PyGdkPixbufAnimation_New(GdkPixbufAnimation *animation)
{
    PyGdkPixbufAnimation_Object *self;

    if (animation == NULL) {
	PyErr_SetString(PyExc_RuntimeError, "couldn't create animation");
	return NULL;
    }

    self = (PyGdkPixbufAnimation_Object *)PyObject_NEW
	(PyGdkPixbufAnimation_Object, &PyGdkPixbufAnimation_Type);

    if (self == NULL)
	return NULL;
    self->animation = gdk_pixbuf_animation_ref(animation);
    return (PyObject *)self;
}

static void
pygdk_pixbuf_animation_dealloc(PyGdkPixbufAnimation_Object *self)
{
    gdk_pixbuf_animation_unref(self->animation);
    PyMem_DEL(self);
}

static int
pygdk_pixbuf_animation_compare(PyGdkPixbufAnimation_Object *self,
			       PyGdkPixbufAnimation_Object *v)
{
    if (self->animation == v->animation) return 0;
    if (self->animation > v->animation) return -1;
    return 1;
}

static long
pygdk_pixbuf_animation_hash(PyGdkPixbufAnimation_Object *self)
{
    return (long)self->animation;
}

static PyObject *
pygdk_pixbuf_animation_getattr(PyGdkPixbufAnimation_Object *self, char *attr)
{
    if (!strcmp(attr, "width"))
	return PyInt_FromLong(gdk_pixbuf_animation_get_width(self->animation));
    else if (!strcmp(attr, "height"))
	return PyInt_FromLong(gdk_pixbuf_animation_get_height(
							self->animation));
    else if (!strcmp(attr, "frames")) {
	GList *frames = gdk_pixbuf_animation_get_frames(self->animation);
	GList *tmp;
	PyObject *py_frames = PyList_New(0);

	for (tmp = frames; tmp != NULL; tmp = tmp->next) {
	    GdkPixbufFrame *frame = tmp->data;
	    PyObject *py_frame;

	    py_frame = Py_BuildValue("(Niiii)",
				     gdk_pixbuf_frame_get_pixbuf(frame),
				     gdk_pixbuf_frame_get_x_offset(frame),
				     gdk_pixbuf_frame_get_y_offset(frame),
				     gdk_pixbuf_frame_get_delay_time(frame),
				     gdk_pixbuf_frame_get_action(frame));
	    PyList_Append(py_frames, py_frame);
	    Py_DECREF(py_frame);
	}
	return py_frames;
    } else if (!strcmp(attr, "num_frames"))
	return PyInt_FromLong(gdk_pixbuf_animation_get_num_frames(
							self->animation));
}

static PyTypeObject PyGdkPixbufAnimation_Type = {
    PyObject_HEAD_INIT(&PyType_Type)
    0,
    "GdkPixbufAnimation",
    sizeof(PyGdkPixbufAnimation_Object),
    0,
    (destructor)pygdk_pixbuf_animation_dealloc,
    (printfunc)0,
    (getattrfunc)pygdk_pixbuf_animation_getattr,
    (setattrfunc)0,
    (cmpfunc)pygdk_pixbuf_animation_compare,
    (reprfunc)0,
    0,
    0,
    0,
    (hashfunc)pygdk_pixbuf_animation_hash,
    (ternaryfunc)0,
    (reprfunc)0,
    0L,0L,0L,0L,
    NULL
};

static PyObject *
pygdk_pixbuf_new(PyObject *self, PyObject *args)
{
    GdkColorspace colorspace;
    gboolean has_alpha;
    int bits_per_sample, width, height;
    GdkPixbuf *pixbuf;
    PyObject *ret;

    if (!PyArg_ParseTuple(args, "iiiii:GdkPixbuf.__init__", &colorspace,
			  &has_alpha, &bits_per_sample, &width, &height))
	return NULL;
    pixbuf = gdk_pixbuf_new(colorspace, has_alpha, bits_per_sample,
			    width, height);
    if (!pixbuf) {
	PyErr_SetString(PyExc_RuntimeError, "could not create new pixbuf");
	return NULL;
    }
    ret = PyGdkPixbuf_New(pixbuf);
    gdk_pixbuf_unref(pixbuf);
    return ret;
}

static PyObject *
pygdk_pixbuf_new_from_file(PyObject *self, PyObject *args)
{
    char *filename;
    GdkPixbuf *pixbuf;
    PyObject *ret;

    if (!PyArg_ParseTuple(args, "s:gdkpixbuf.new_from_file", &filename))
	return NULL;
    pixbuf = gdk_pixbuf_new_from_file(filename);
    if (!pixbuf) {
	PyErr_SetString(PyExc_RuntimeError, "could not create new pixbuf");
	return NULL;
    }
    ret = PyGdkPixbuf_New(pixbuf);
    gdk_pixbuf_unref(pixbuf);
    return ret;
}

static void
pygdk_pixbuf_destroy_notify(guchar *pixels, gpointer data)
{
    PyGtk_BlockThreads();
    Py_DECREF((PyObject *)data);
    PyGtk_UnblockThreads();
}

static PyObject *
pygdk_pixbuf_new_from_data(PyObject *self, PyObject *args)
{
    PyObject *pydata;
    guchar *data;
    gint data_len;
    GdkColorspace colorspace;
    gboolean has_alpha;
    int bits_per_sample, width, height, rowstride = -1;
    GdkPixbuf *pixbuf;
    PyObject *ret;

    if (!PyArg_ParseTuple(args, "Siiiii|i:gdkpixbuf.new_from_data",
			  &pydata, &colorspace, &has_alpha,
			  &bits_per_sample, &width, &height, &rowstride))
	return NULL;
    data = PyString_AsString(pydata);
    data_len = PyString_Size(pydata);
    if (rowstride < 0)
	rowstride = width * (has_alpha ? 4 : 3);
    if (height * rowstride > data_len) {
	PyErr_SetString(PyExc_TypeError, "data not long enough");
	return NULL;
    }
    /* make pixbuf hold a reference to the string */
    Py_INCREF(pydata);
    pixbuf = gdk_pixbuf_new_from_data(data, colorspace, has_alpha,
				      bits_per_sample, width, height,
				      rowstride,
				      pygdk_pixbuf_destroy_notify, pydata);
    if (!pixbuf) {
	PyErr_SetString(PyExc_RuntimeError, "could not create new pixbuf");
	return NULL;
    }
    ret = PyGdkPixbuf_New(pixbuf);
    gdk_pixbuf_unref(pixbuf);
    return ret;
}

#ifdef HAVE_NUMPY
static PyObject *
pygdk_pixbuf_new_from_array(PyObject *self, PyObject *args)
{
    PyArrayObject *adata;
    guchar *data;
    gboolean has_alpha;
    int width, height;
    GdkPixbuf *pixbuf;
    PyObject *ret;

    if (!PyArg_ParseTuple(args, "O!:gdkpixbuf.new_from_array",
			  &PyArray_Type, &adata))
	return NULL;
    if (!PyArray_ISCONTIGUOUS(adata) ||
	adata->descr->type_num != PyArray_UBYTE ||
	adata->nd != 3 ||
	(adata->dimensions[2] != 3 && adata->dimensions[2] != 4)) {
	PyErr_SetString(PyExc_TypeError,
	    "array must be a MxNx3 or MxNx4 contiguous unsigned byte array");
	return NULL;
    }
    data = (guchar *)adata->data;
    width = adata->dimensions[1];
    height = adata->dimensions[0];
    has_alpha = (adata->dimensions[2] == 4);

    /* store a reference to the array with the pixbuf */
    Py_INCREF(adata);
    pixbuf = gdk_pixbuf_new_from_data(data, GDK_COLORSPACE_RGB, has_alpha,
				      8, width, height, adata->strides[0],
				      pygdk_pixbuf_destroy_notify, adata);
    
    if (!pixbuf) {
	PyErr_SetString(PyExc_RuntimeError, "could not create new pixbuf");
	return NULL;
    }
    ret = PyGdkPixbuf_New(pixbuf);
    gdk_pixbuf_unref(pixbuf);
    return ret;
}
#endif

static PyObject *
pygdk_pixbuf_animation_new_from_file(PyObject *self, PyObject *args)
{
    char *filename;
    GdkPixbufAnimation *animation;
    PyObject *ret;

    if (!PyArg_ParseTuple(args, "s:gdkpixbuf.GdkPixbufAnimation", &filename))
	return NULL;
    animation = gdk_pixbuf_animation_new_from_file(filename);
    if (!animation) {
	PyErr_SetString(PyExc_RuntimeError, "could not create new animation");
	return NULL;
    }
    ret = PyGdkPixbufAnimation_New(animation);
    gdk_pixbuf_animation_unref(animation);
    return ret;
}

static PyMethodDef gdkpixbuf_functions[] = {
    { "GdkPixbuf", pygdk_pixbuf_new, METH_VARARGS, NULL },
    { "new_from_file", pygdk_pixbuf_new_from_file, METH_VARARGS, NULL },
    { "new_from_data", pygdk_pixbuf_new_from_data, METH_VARARGS, NULL },
#ifdef HAVE_NUMPY
    { "new_from_array", pygdk_pixbuf_new_from_array, METH_VARARGS, NULL },
#endif
    { "GdkPixbufAnimation", pygdk_pixbuf_animation_new_from_file, METH_VARARGS, NULL },
    { NULL, NULL }
};

static struct _PyGdkPixbuf_FunctionStruct functions = {
    &PyGdkPixbuf_Type, PyGdkPixbuf_New,
    &PyGdkPixbufAnimation_Type, PyGdkPixbufAnimation_New
};

void
initgdkpixbuf(void)
{
    PyObject *m, *d, *v;

    m = Py_InitModule("gdkpixbuf", gdkpixbuf_functions);
    d = PyModule_GetDict(m);

#ifdef HAVE_NUMPY
    import_array();
#endif

    PyDict_SetItemString(d, "GdkPixbufType", (PyObject *)&PyGdkPixbuf_Type);

    PyDict_SetItemString(d, "_PyGdkPixbuf_API",
			 v=PyCObject_FromVoidPtr(&functions, NULL));
    Py_DECREF(v);

    /* enums */
    PyDict_SetItemString(d, "COLORSPACE_RGB",
			 v=PyInt_FromLong(GDK_COLORSPACE_RGB));
    Py_DECREF(v);
    PyDict_SetItemString(d, "ALPHA_BILEVEL",
			 v=PyInt_FromLong(GDK_PIXBUF_ALPHA_BILEVEL));
    Py_DECREF(v);
    PyDict_SetItemString(d, "ALPHA_FULL",
			 v=PyInt_FromLong(GDK_PIXBUF_ALPHA_FULL));
    Py_DECREF(v);
    PyDict_SetItemString(d, "INTERP_NEAREST",
			 v=PyInt_FromLong(GDK_INTERP_NEAREST));
    Py_DECREF(v);
    PyDict_SetItemString(d, "INTERP_TILES",
			 v=PyInt_FromLong(GDK_INTERP_TILES));
    Py_DECREF(v);
    PyDict_SetItemString(d, "INTERP_BILINEAR",
			 v=PyInt_FromLong(GDK_INTERP_BILINEAR));
    Py_DECREF(v);
    PyDict_SetItemString(d, "INTERP_HYPER",
			 v=PyInt_FromLong(GDK_INTERP_HYPER));
    Py_DECREF(v);

    init_pygtk();

    if (PyErr_Occurred())
	Py_FatalError("can't initialise module gdkpixbuf");
}
