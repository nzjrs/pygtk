/* -*- Mode: C; c-basic-offset: 2 -*- */
/* PyGTK - python bindings for GTK+
 * Copyright (C) 1997-2002 James Henstridge <james@daa.com.au>
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
#include <sysmodule.h>
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

#include <gtk/gtk.h>
#include <locale.h>

#define WITH_XSTUFF
#ifdef WITH_XSTUFF
#include <gdk/gdkx.h>
#endif

#define _INSIDE_PYGTK_
#include "pygtk.h"

/* The threading hacks are based on ones supplied by Duncan Grisby
 * of AT&T Labs Cambridge.  Since then they have been modified a bit. */

/* The threading code has been enhanced to be a little better with multiple
 * threads accessing GTK+.  Here are some notes on the changes by
 * Paul Fisher:
 *
 * If threading is enabled, we create a recursive version of Python's
 * global interpreter mutex using TSD.  This scheme makes it possible,
 * although rather hackish, for any thread to make a call into PyGTK,
 * as long as the GDK lock is held (that is, Python code is wrapped
 * around a threads_{enter,leave} pair).
 *
 * A viable alternative would be to wrap each and every GTK call, at
 * the Python/C level, with Py_{BEGIN,END}_ALLOW_THREADS.  However,
 * given the nature of Python threading, this option is not
 * particularly appealing.
 */


#ifdef ENABLE_PYGTK_THREADING
static GStaticPrivate pythreadstate_key = G_STATIC_PRIVATE_INIT;
static GStaticPrivate counter_key = G_STATIC_PRIVATE_INIT;
static gboolean disable_threads = FALSE;

/* The global Python lock will be grabbed by Python when entering a
 * Python/C function; thus, the initial lock count will always be one.
 */
#  define INITIAL_LOCK_COUNT 1
#  define PyGTK_BLOCK_THREADS                                                \
   {                                                                         \
     if (!disable_threads) {                                                 \
         gint counter = GPOINTER_TO_INT(g_static_private_get(&counter_key)); \
         if (counter == -INITIAL_LOCK_COUNT) {                               \
           PyThreadState *_save;                                             \
           _save = g_static_private_get(&pythreadstate_key);                 \
           Py_BLOCK_THREADS;                                                 \
         }                                                                   \
         counter++;                                                          \
         g_static_private_set(&counter_key, GINT_TO_POINTER(counter), NULL); \
      }                                                                      \
   }

#  define PyGTK_UNBLOCK_THREADS                                              \
   {                                                                         \
      if (!disable_threads) {                                                \
         gint counter = GPOINTER_TO_INT(g_static_private_get(&counter_key)); \
         counter--;                                                          \
         if (counter == -INITIAL_LOCK_COUNT) {                               \
           PyThreadState *_save;                                             \
           Py_UNBLOCK_THREADS;                                               \
           g_static_private_set(&pythreadstate_key, _save, NULL);            \
         }                                                                   \
         g_static_private_set(&counter_key, GINT_TO_POINTER(counter), NULL); \
      }                                                                      \
   }


#else /* !ENABLE_PYGTK_THREADING */
#  define PyGTK_BLOCK_THREADS
#  define PyGTK_UNBLOCK_THREADS
#endif

static void PyGtk_BlockThreads(void) { PyGTK_BLOCK_THREADS }
static void PyGtk_UnblockThreads(void) { PyGTK_UNBLOCK_THREADS }

static PyObject *
PyGtk_New(GtkObject *go) {
  PyGtk_Object *self;

  self = (PyGtk_Object *)PyObject_NEW(PyGtk_Object, &PyGtk_Type);
  if (self == NULL)
    return NULL;
  self->obj = go;
  gtk_object_ref(self->obj);
  /*  gtk_object_sink(self->obj); */
  return (PyObject *)self;
}

static void 
PyGtk_dealloc(PyGtk_Object *self) {
  gtk_object_unref(self->obj);
  PyMem_DEL(self);
}

static int
PyGtk_compare(PyGtk_Object *self, PyGtk_Object *v) {
  if (self->obj == v->obj) return 0;
  if (self->obj > v->obj) return -1;
  return 1;
}

static long
PyGtk_hash(PyGtk_Object *self) {
  return (long)self->obj;
}

static PyObject *
PyGtk_repr(PyGtk_Object *self) {
  char buf[100];

  sprintf(buf, "<GtkObject of type %s at %lx>",
    gtk_type_name(PyGtk_Get(self)->klass->type), (long)PyGtk_Get(self));
  return PyString_FromString(buf);
}

static char PyGtk_Type__doc__[] = "This is the type of GTK Objects";

static PyTypeObject PyGtk_Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,                              /*ob_size*/
  "GtkObject",                    /*tp_name*/
  sizeof(PyGtk_Object),           /*tp_basicsize*/
  0,                              /*tp_itemsize*/
  (destructor)PyGtk_dealloc,      /*tp_dealloc*/
  (printfunc)0,                   /*tp_print*/
  (getattrfunc)0,                 /*tp_getattr*/
  (setattrfunc)0,                 /*tp_setattr*/
  (cmpfunc)PyGtk_compare,         /*tp_compare*/
  (reprfunc)PyGtk_repr,           /*tp_repr*/
  0,                              /*tp_as_number*/
  0,                              /*tp_as_sequence*/
  0,                              /*tp_as_mapping*/
  (hashfunc)PyGtk_hash,           /*tp_hash*/
  (ternaryfunc)0,                 /*tp_call*/
  (reprfunc)0,                    /*tp_str*/
  0L,0L,0L,0L,
  PyGtk_Type__doc__
};

/* return 1 on failure */
gint PyGtkEnum_get_value(GtkType enum_type, PyObject *obj, int *val) {
  if (PyInt_Check(obj)) {
    *val = PyInt_AsLong(obj);
    return 0;
  } else if (PyString_Check(obj)) {
    GtkEnumValue *info = gtk_type_enum_find_value(enum_type,
						  PyString_AsString(obj));
    if (!info) {
      PyErr_SetString(PyExc_TypeError, "couldn't translate string");
      return 1;
    }
    *val = info->value;
    return 0;
  }
  PyErr_SetString(PyExc_TypeError, "enum values must be integers or strings");
  return 1;
}

gint PyGtkFlag_get_value(GtkType flag_type, PyObject *obj, int *val) {
  if (PyInt_Check(obj)) {
    *val = PyInt_AsLong(obj);
    return 0;
  } else if (PyString_Check(obj)) {
    GtkFlagValue *info = gtk_type_flags_find_value(flag_type,
						  PyString_AsString(obj));
    if (!info) {
      PyErr_SetString(PyExc_TypeError, "couldn't translate string");
      return 1;
    }
    *val = info->value;
    return 0;
  } else if (PyTuple_Check(obj)) {
    int i, len;
    PyObject *item;
    len = PyTuple_Size(obj);
    *val = 0;
    for (i = 0; i < len; i++) {
      item = PyTuple_GetItem(obj, i);
      if (PyInt_Check(item))
	*val |= PyInt_AsLong(item);
      else if (PyString_Check(item)) {
	GtkFlagValue *info = gtk_type_flags_find_value(flag_type,
						      PyString_AsString(item));
	if (!info) {
	  PyErr_SetString(PyExc_TypeError, "couldn't translate string");
	  return 1;
	}
	*val |= info->value;
      } else {
	PyErr_SetString(PyExc_TypeError,
			"tuple components must be ints or strings");
	return 1;
      }
    }
    return 0;
  }
  PyErr_SetString(PyExc_TypeError,
		  "flag values must be ints, strings or tuples");
  return 1;
}

static PyObject *
PyGtkAccelGroup_New(GtkAccelGroup *obj) {
  PyGtkAccelGroup_Object  *self;

  self = (PyGtkAccelGroup_Object *)PyObject_NEW(PyGtkAccelGroup_Object ,
              &PyGtkAccelGroup_Type);
  if (self == NULL)
    return NULL;
  self->obj = obj;
  gtk_accel_group_ref(self->obj);
  return (PyObject *)self;
}

static PyObject *
PyGtkStyle_New(GtkStyle *obj) {
  PyGtkStyle_Object *self;

  if (!obj) {
    Py_INCREF(Py_None);
    return Py_None;
  }    
  
  self = (PyGtkStyle_Object *)PyObject_NEW(PyGtkStyle_Object,
                                           &PyGtkStyle_Type);
  if (self == NULL)
    return NULL;

  self->obj = obj;
  gtk_style_ref(self->obj);
  return (PyObject *)self;
}

static PyObject *
PyGdkFont_New(GdkFont *obj) {
  PyGdkFont_Object *self;

  self = (PyGdkFont_Object *)PyObject_NEW(PyGdkFont_Object, &PyGdkFont_Type);
  if (self == NULL)
    return NULL;
  self->obj = obj;
  gdk_font_ref(self->obj);
  return (PyObject *)self;
}

static PyObject *
PyGdkColor_New(GdkColor *obj) {
  PyGdkColor_Object *self;

  self = (PyGdkColor_Object *)PyObject_NEW(PyGdkColor_Object,&PyGdkColor_Type);
  if (self == NULL)
    return NULL;
  self->obj.pixel = obj->pixel;
  self->obj.red   = obj->red;
  self->obj.green = obj->green;
  self->obj.blue  = obj->blue;
  return (PyObject *)self;
}

static PyObject *
PyGdkWindow_New(GdkWindow *win) {
  PyGdkWindow_Object *self;

  self = (PyGdkWindow_Object *)PyObject_NEW(PyGdkWindow_Object,
					    &PyGdkWindow_Type);
  if (self == NULL)
    return NULL;
  self->obj = win;
  gdk_window_ref(self->obj);
  return (PyObject *)self;
}

static PyObject *
PyGdkGC_New(GdkGC *gc) {
  PyGdkGC_Object *self;

  self = (PyGdkGC_Object *)PyObject_NEW(PyGdkGC_Object, &PyGdkGC_Type);
  if (self == NULL)
    return NULL;
  self->obj = gc;
  gdk_gc_ref(self->obj);
  return (PyObject *)self;
}

static PyObject *
PyGdkColormap_New(GdkColormap *cmap) {
  PyGdkColormap_Object *self;

  self = (PyGdkColormap_Object *)PyObject_NEW(PyGdkColormap_Object,
					      &PyGdkColormap_Type);
  if (self == NULL)
    return NULL;
  self->obj = cmap;
  gdk_colormap_ref(self->obj);
  return (PyObject *)self;
}

static PyObject *
PyGdkDragContext_New(GdkDragContext *ctx) {
  PyGdkDragContext_Object *self;

  self = (PyGdkDragContext_Object *)PyObject_NEW(PyGdkDragContext_Object,
						 &PyGdkDragContext_Type);
  if (self == NULL)
    return NULL;
  self->obj = ctx;
  gdk_drag_context_ref(self->obj);
  return (PyObject *)self;
}

static PyObject *
PyGtkSelectionData_New(GtkSelectionData *data) {
  PyGtkSelectionData_Object *self;

  self = (PyGtkSelectionData_Object *)PyObject_NEW(PyGtkSelectionData_Object,
						   &PyGtkSelectionData_Type);
  if (self == NULL)
    return NULL;
  self->obj = data;
  return (PyObject *)self;
}

static PyObject *
PyGdkAtom_New(GdkAtom atom) {
  PyGdkAtom_Object *self;

  self = (PyGdkAtom_Object *)PyObject_NEW(PyGdkAtom_Object, &PyGdkAtom_Type);
  if (self == NULL)
    return NULL;
  self->atom = atom;
  self->name = NULL;
  return (PyObject *)self;
}

static PyObject *
PyGdkCursor_New(GdkCursor *obj) {
  PyGdkCursor_Object *self;

  self = (PyGdkCursor_Object *)PyObject_NEW(PyGdkCursor_Object,
					    &PyGdkCursor_Type);
  if (self == NULL)
    return NULL;
  self->obj = obj;
  return (PyObject *)self;
}

static PyObject *
PyGtkCTreeNode_New(GtkCTreeNode *node) {
  PyGtkCTreeNode_Object *self;

  self = (PyGtkCTreeNode_Object *)PyObject_NEW(PyGtkCTreeNode_Object,
					       &PyGtkCTreeNode_Type);
  if (self == NULL)
    return NULL;
  self->node = node;
  return (PyObject *)self;
}

static void
PyGtkAccelGroup_Dealloc(PyGtkAccelGroup_Object *self) {
  gtk_accel_group_unref(self->obj); 
  PyMem_DEL(self);
}

static int
PyGtkAccelGroup_Compare(PyGtkAccelGroup_Object *self,
                            PyGtkAccelGroup_Object *v) {
  if (self->obj == v->obj) return 0;
  if (self->obj > v->obj) return -1;
  return 1;
}

static long PyGtkAccelGroup_Hash(PyGtkAccelGroup_Object *self) {
  return (long)self->obj;
}

static PyTypeObject PyGtkAccelGroup_Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,
  "GtkAccelGroup",
  sizeof(PyGtkAccelGroup_Object),
  0,
  (destructor)PyGtkAccelGroup_Dealloc,
  (printfunc)0,
  (getattrfunc)0,
  (setattrfunc)0,
  (cmpfunc)PyGtkAccelGroup_Compare,
  (reprfunc)0,
  0,
  0,
  0,
  (hashfunc)PyGtkAccelGroup_Hash,
  (ternaryfunc)0,
  (reprfunc)0,
  0L,0L,0L,0L,
  NULL
};

typedef struct {
  PyObject_HEAD
  GtkStyle *style; /* parent style */
  enum {STYLE_COLOUR_ARRAY, STYLE_GC_ARRAY, STYLE_PIXMAP_ARRAY} type;
  gpointer array;
} PyGtkStyleHelper_Object;
staticforward PyTypeObject PyGtkStyleHelper_Type;

static PyObject *PyGtkStyleHelper_New(GtkStyle *style, int type,
				      gpointer array) {
  PyGtkStyleHelper_Object *self;

  self = (PyGtkStyleHelper_Object *)PyObject_NEW(PyGtkStyleHelper_Object,
						 &PyGtkStyleHelper_Type);
  if (self == NULL)
    return NULL;
  self->style = gtk_style_ref(style);
  self->type = type;
  self->array = array;
  return (PyObject *)self;
}

static void PyGtkStyleHelper_Dealloc(PyGtkStyleHelper_Object *self) {
  gtk_style_unref(self->style);
  PyMem_DEL(self);
}

static int PyGtkStyleHelper_Length(PyGtkStyleHelper_Object *self) {
  return 5;
}
static PyObject *PyGtkStyleHelper_GetItem(PyGtkStyleHelper_Object *self,
					  int pos) {
  if (pos < 0) pos += 5;
  if (pos < 0 || pos >= 5) {
    PyErr_SetString(PyExc_IndexError, "index out of range");
    return NULL;
  }
  switch (self->type) {
  case STYLE_COLOUR_ARRAY:
    {
      GdkColor *array = (GdkColor *)self->array;
      return PyGdkColor_New(&array[pos]);
    }
  case STYLE_GC_ARRAY:
    {
      GdkGC **array = (GdkGC **)self->array;
      if (array[pos])
	return PyGdkGC_New(array[pos]);
      Py_INCREF(Py_None);
      return Py_None;
    }
  case STYLE_PIXMAP_ARRAY:
    {
      GdkWindow **array = (GdkWindow **)self->array;
      if (array[pos])
	return PyGdkWindow_New(array[pos]);
      Py_INCREF(Py_None);
      return Py_None;
    }
  }
  g_assert_not_reached();
  return NULL;
}
static int PyGtkStyleHelper_SetItem(PyGtkStyleHelper_Object *self, int pos,
				    PyObject *value) {
  if (pos < 0) pos += 5;
  if (pos < 0 || pos >= 5) {
    PyErr_SetString(PyExc_IndexError, "index out of range");
    return -1;
  }
  switch (self->type) {
  case STYLE_COLOUR_ARRAY:
    {
      GdkColor *array = (GdkColor *)self->array;

      if (!PyGdkColor_Check(value)) {
	PyErr_SetString(PyExc_TypeError, "can only assign a GdkColor");
	return -1;
      }
      array[pos] = *PyGdkColor_Get(value);
      return 0;
    }
  case STYLE_GC_ARRAY:
    {
      GdkGC **array = (GdkGC **)self->array;

      if (!PyGdkGC_Check(value)) {
	PyErr_SetString(PyExc_TypeError, "can only assign a GdkGC");
	return -1;
      }
      if (array[pos]) gdk_gc_unref(array[pos]);
      array[pos] = gdk_gc_ref(PyGdkGC_Get(value));
      return 0;
    }
  case STYLE_PIXMAP_ARRAY:
    {
      GdkWindow **array = (GdkWindow **)self->array;

      if (!PyGdkWindow_Check(value) && value != Py_None) {
	PyErr_SetString(PyExc_TypeError,"can only assign a GdkPixmap or None");
	return -1;
      }
      if (array[pos]) gdk_pixmap_unref(array[pos]);
      if (value != Py_None)
	array[pos] = gdk_pixmap_ref(PyGdkWindow_Get(value));
      else
	array[pos] = NULL;
      return 0;
    }
  }
  g_assert_not_reached();
  return -1;
}
static PySequenceMethods PyGtkStyleHelper_SeqMethods = {
  (inquiry)PyGtkStyleHelper_Length,
  (binaryfunc)0,
  (intargfunc)0,
  (intargfunc)PyGtkStyleHelper_GetItem,
  (intintargfunc)0,
  (intobjargproc)PyGtkStyleHelper_SetItem,
  (intintobjargproc)0
};
static PyTypeObject PyGtkStyleHelper_Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,
  "GtkStyleHelper",
  sizeof(PyGtkStyleHelper_Object),
  0,
  (destructor)PyGtkStyleHelper_Dealloc,
  (printfunc)0,
  (getattrfunc)0,
  (setattrfunc)0,
  (cmpfunc)0,
  (reprfunc)0,
  0,
  &PyGtkStyleHelper_SeqMethods,
  0,
  (hashfunc)0,
  (ternaryfunc)0,
  (reprfunc)0,
  0L,0L,0L,0L,
  NULL
};


static void
PyGtkStyle_Dealloc(PyGtkStyle_Object *self) {
  gtk_style_unref(self->obj);
  PyMem_DEL(self);
}

static int
PyGtkStyle_Compare(PyGtkStyle_Object *self, PyGtkStyle_Object *v) {
  if (self->obj == v->obj) return 0;
  if (self->obj > v->obj) return -1;
  return 1;
}

static long PyGtkStyle_Hash(PyGtkStyle_Object *self) {
  return (long)self->obj;
}

static PyObject *PyGtkStyle_copy(PyGtkStyle_Object *self, PyObject *args) {
  GtkStyle *style;
  PyObject *ret;
  if (!PyArg_ParseTuple(args, ":GtkStyle.copy"))
    return NULL;
  /* this is so that new style has ref count one and doesn't get destroyed */
  style = gtk_style_copy(self->obj);
  ret = PyGtkStyle_New(style);
  gtk_style_unref(style);
  return ret;
}

static PyMethodDef PyGtkStyle_methods[] = {
  {"copy", (PyCFunction)PyGtkStyle_copy, METH_VARARGS, NULL},
  {NULL, 0, 0, NULL}
};

static PyObject *PyGtkStyle_GetAttr(PyGtkStyle_Object *self, char *attr) {
  GtkStyle *style = self->obj;

  if (!strcmp(attr, "__members__"))
    return Py_BuildValue("[sssssssssssssssssssss]", "base", "base_gc", "bg",
			 "bg_gc", "bg_pixmap", "black", "black_gc", "colormap",
			 "dark", "dark_gc", "fg", "fg_gc", "font", "light",
			 "light_gc", "mid", "mid_gc", "text", "text_gc",
			 "white", "white_gc");
  if (!strcmp(attr, "fg"))
    return PyGtkStyleHelper_New(style, STYLE_COLOUR_ARRAY, style->fg);
  if (!strcmp(attr, "bg"))
    return PyGtkStyleHelper_New(style, STYLE_COLOUR_ARRAY, style->bg);
  if (!strcmp(attr, "light"))
    return PyGtkStyleHelper_New(style, STYLE_COLOUR_ARRAY, style->light);
  if (!strcmp(attr, "dark"))
    return PyGtkStyleHelper_New(style, STYLE_COLOUR_ARRAY, style->dark);
  if (!strcmp(attr, "mid"))
    return PyGtkStyleHelper_New(style, STYLE_COLOUR_ARRAY, style->mid);
  if (!strcmp(attr, "text"))
    return PyGtkStyleHelper_New(style, STYLE_COLOUR_ARRAY, style->text);
  if (!strcmp(attr, "base"))
    return PyGtkStyleHelper_New(style, STYLE_COLOUR_ARRAY, style->base);
  if (!strcmp(attr, "black"))
    return PyGdkColor_New(&style->black);
  if (!strcmp(attr, "white"))
    return PyGdkColor_New(&style->white);
  if (!strcmp(attr, "font"))
    return PyGdkFont_New(style->font);
  if (!strcmp(attr, "fg_gc"))
    return PyGtkStyleHelper_New(style, STYLE_GC_ARRAY, style->fg_gc);
  if (!strcmp(attr, "bg_gc"))
    return PyGtkStyleHelper_New(style, STYLE_GC_ARRAY, style->bg_gc);
  if (!strcmp(attr, "light_gc"))
    return PyGtkStyleHelper_New(style, STYLE_GC_ARRAY, style->light_gc);
  if (!strcmp(attr, "dark_gc"))
    return PyGtkStyleHelper_New(style, STYLE_GC_ARRAY, style->dark_gc);
  if (!strcmp(attr, "mid_gc"))
    return PyGtkStyleHelper_New(style, STYLE_GC_ARRAY, style->mid_gc);
  if (!strcmp(attr, "text_gc"))
    return PyGtkStyleHelper_New(style, STYLE_GC_ARRAY, style->text_gc);
  if (!strcmp(attr, "base_gc"))
    return PyGtkStyleHelper_New(style, STYLE_GC_ARRAY, style->base_gc);
  if (!strcmp(attr, "black_gc")) {
      if (style->black_gc)
	  return PyGdkGC_New(style->black_gc);
      Py_INCREF(Py_None);
      return Py_None;
  }
  if (!strcmp(attr, "white_gc")) {
      if (style->white_gc)
	  return PyGdkGC_New(style->white_gc);
      Py_INCREF(Py_None);
      return Py_None;
  }
  if (!strcmp(attr, "bg_pixmap"))
    return PyGtkStyleHelper_New(style, STYLE_PIXMAP_ARRAY, style->bg_pixmap);
  if (!strcmp(attr, "colormap")) {
    if (style->colormap)
      return PyGdkColormap_New(style->colormap);
    Py_INCREF(Py_None);
    return Py_None;
  }

  return Py_FindMethod(PyGtkStyle_methods, (PyObject *)self, attr);
}

static int PyGtkStyle_SetAttr(PyGtkStyle_Object *self, char *key,
			      PyObject *value) {
  GtkStyle *style = self->obj;
  if (!strcmp(key, "font")) {
    if (PyGdkFont_Check(value)) {
      if (style->font)
	gdk_font_unref(style->font);
      style->font = gdk_font_ref(PyGdkFont_Get(value));
    } else {
      PyErr_SetString(PyExc_TypeError, "font attribute must be type GdkFont");
      return -1;
    }
  } else if (!strcmp(key, "black")) {
    if (PyGdkColor_Check(value))
      style->black = *PyGdkColor_Get(value);
    else {
      PyErr_SetString(PyExc_TypeError, "black attribute must be a GdkColor");
      return -1;
    }
  } else if (!strcmp(key, "white")) {
    if (PyGdkColor_Check(value))
      style->white = *PyGdkColor_Get(value);
    else {
      PyErr_SetString(PyExc_TypeError, "white attribute must be a GdkColor");
      return -1;
    }
  } else if (!strcmp(key, "black_gc")) {
    if (PyGdkGC_Check(value)) {
      if (style->black_gc)
	gdk_gc_unref(style->black_gc);
      style->black_gc = gdk_gc_ref(PyGdkGC_Get(value));
    } else {
      PyErr_SetString(PyExc_TypeError,"black_gc attribute must be a GdkColor");
      return -1;
    }
  } else if (!strcmp(key, "white_gc")) {
    if (PyGdkGC_Check(value)) {
      if (style->white_gc)
	gdk_gc_unref(style->white_gc);
      style->white_gc = gdk_gc_ref(PyGdkGC_Get(value));
    } else {
      PyErr_SetString(PyExc_TypeError,"white_gc attribute must be a GdkColor");
      return -1;
    }
  } else {
    PyErr_SetString(PyExc_AttributeError, key);
    return -1;
  }
  return 0;
}

static PyTypeObject PyGtkStyle_Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,
  "GtkStyle",
  sizeof(PyGtkStyle_Object),
  0,
  (destructor)PyGtkStyle_Dealloc,
  (printfunc)0,
  (getattrfunc)PyGtkStyle_GetAttr,
  (setattrfunc)PyGtkStyle_SetAttr,
  (cmpfunc)PyGtkStyle_Compare,
  (reprfunc)0,
  0,
  0,
  0,
  (hashfunc)PyGtkStyle_Hash,
  (ternaryfunc)0,
  (reprfunc)0,
  0L,0L,0L,0L,
  NULL
};

static void
PyGdkFont_Dealloc(PyGdkFont_Object *self) {
  gdk_font_unref(self->obj);
  PyMem_DEL(self);
}

static PyObject *
PyGdkFont_Width(PyGdkFont_Object *self, PyObject *args) {
  char *text;
  int length;

  if (!PyArg_ParseTuple(args, "s#:GdkFont.width", &text, &length))
    return NULL;
  return PyInt_FromLong(gdk_text_width(self->obj, text, length));
}

static PyObject *
PyGdkFont_Measure(PyGdkFont_Object *self, PyObject *args) {
  char *text;
  int length;

  if (!PyArg_ParseTuple(args, "s#:GdkFont.measure", &text, &length))
    return NULL;
  return PyInt_FromLong(gdk_text_measure(self->obj, text, length));
}

static PyObject *
PyGdkFont_Height(PyGdkFont_Object *self, PyObject *args) {
  char *text;
  int length;

  if (!PyArg_ParseTuple(args, "s#:GdkFont.height", &text, &length))
    return NULL;
  return PyInt_FromLong(gdk_text_height(self->obj, text, length));
}

static PyObject *
PyGdkFont_Extents(PyGdkFont_Object *self, PyObject *args) {
  char *text;
  int length, lbearing, rbearing, width, ascent, descent;
  if (!PyArg_ParseTuple(args, "s#:GdkFont.extents", &text, &length))
    return NULL;
  gdk_text_extents(self->obj, text, length, &lbearing, &rbearing,
		   &width, &ascent, &descent);
  return Py_BuildValue("(iiiii)", lbearing, rbearing, width, ascent, descent);
}

static PyMethodDef PyGdkFont_methods[] = {
  {"width",   (PyCFunction)PyGdkFont_Width,   METH_VARARGS, NULL},
  {"measure", (PyCFunction)PyGdkFont_Measure, METH_VARARGS, NULL},
  {"height",  (PyCFunction)PyGdkFont_Height,  METH_VARARGS, NULL},
  {"extents", (PyCFunction)PyGdkFont_Extents, METH_VARARGS, NULL},
  {NULL, 0, 0, NULL}
};

static PyObject *
PyGdkFont_GetAttr(PyGdkFont_Object *self, char *key) {
  if (!strcmp(key, "__members__"))
    return Py_BuildValue("[sss]", "ascent", "descent", "type");
  if (!strcmp(key, "ascent"))
    return PyInt_FromLong(self->obj->ascent);
  if (!strcmp(key, "descent"))
    return PyInt_FromLong(self->obj->descent);
  if (!strcmp(key, "type"))
    return PyInt_FromLong(self->obj->type);
  return Py_FindMethod(PyGdkFont_methods, (PyObject *)self, key);
}

static int
PyGdkFont_Compare(PyGdkFont_Object *self, PyGdkFont_Object *v) {
  if (self->obj == v->obj) return 0;
  if (self->obj > v->obj) return -1;
  return 1;
}

static long PyGdkFont_Hash(PyGdkFont_Object *self) {
  return (long)self->obj;
}

static PyTypeObject PyGdkFont_Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,
  "GdkFont",
  sizeof(PyGdkFont_Object),
  0,
  (destructor)PyGdkFont_Dealloc,
  (printfunc)0,
  (getattrfunc)PyGdkFont_GetAttr,
  (setattrfunc)0,
  (cmpfunc)PyGdkFont_Compare,
  (reprfunc)0,
  0,
  0,
  0,
  (hashfunc)PyGdkFont_Hash,
  (ternaryfunc)0,
  (reprfunc)0,
  0L,0L,0L,0L,
  NULL
};

static void
PyGdkColor_Dealloc(PyGdkColor_Object *self) {
  PyMem_DEL(self);
}

static PyObject *
PyGdkColor_GetAttr(PyGdkColor_Object *self, char *key) {
  if (!strcmp(key, "red"))
    return PyInt_FromLong(self->obj.red);
  if (!strcmp(key, "green"))
    return PyInt_FromLong(self->obj.green);
  if (!strcmp(key, "blue"))
    return PyInt_FromLong(self->obj.blue);
  if (!strcmp(key, "pixel"))
    return PyInt_FromLong(self->obj.pixel);
  PyErr_SetString(PyExc_AttributeError, key);
  return NULL;
}

static PyObject *
PyGdkColor_SetAttr(PyGdkColor_Object *self, char *key, PyObject * val) {
  if (!strcmp(key, "red"))
    self->obj.red = PyInt_AsLong(val);
  if (!strcmp(key, "green"))
    self->obj.green = PyInt_AsLong(val);
  if (!strcmp(key, "blue"))
    self->obj.blue = PyInt_AsLong(val);
  if (!strcmp(key, "pixel"))
    self->obj.pixel = PyInt_AsLong(val);
  PyErr_SetString(PyExc_AttributeError, key);
  return NULL;
}

static int
PyGdkColor_Compare(PyGdkColor_Object *self, PyGdkColor_Object *v) {
  if (self->obj.pixel == v->obj.pixel) return 0;
  if (self->obj.pixel > v->obj.pixel) return -1;
  return 1;
}

static long PyGdkColor_Hash(PyGdkColor_Object *self) {
  return (long)self->obj.pixel;
}

static PyObject *
PyGdkColor_Repr(PyGdkColor_Object *self) {
  char buf[80];

  g_snprintf(buf, 79, "<GdkColor (%hu, %hu, %hu)>", self->obj.red,
	     self->obj.green, self->obj.blue);
  return PyString_FromString(buf);
}

static PyTypeObject PyGdkColor_Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,
  "GdkColor",
  sizeof(PyGdkColor_Object),
  0,
  (destructor)PyGdkColor_Dealloc,
  (printfunc)0,
  (getattrfunc)PyGdkColor_GetAttr,
  (setattrfunc)PyGdkColor_SetAttr,
  (cmpfunc)PyGdkColor_Compare,
  (reprfunc)PyGdkColor_Repr,
  0,
  0,
  0,
  (hashfunc)PyGdkColor_Hash,
  (ternaryfunc)0,
  (reprfunc)0,
  0L,0L,0L,0L,
  NULL
};

static PyObject *
PyGdkEvent_New(GdkEvent *obj) {
  PyGdkEvent_Object *self;
  PyObject *v;

  if (obj == NULL) {
    Py_INCREF(Py_None);
    return Py_None;
  }
  if ((self = (PyGdkEvent_Object *)
       PyObject_NEW(PyGdkEvent_Object,&PyGdkEvent_Type)) == NULL)
    return NULL;
  self->obj = obj;
  if ((self->attrs = PyDict_New()) == NULL)
    return NULL;
  if ((v = PyInt_FromLong(obj->type)) == NULL)
    return NULL;
  PyDict_SetItemString(self->attrs, "type", v);
  Py_DECREF(v);
  if (obj->any.window) {
    if ((v = PyGdkWindow_New(obj->any.window)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "window", v);
    Py_DECREF(v);
  } else
    PyDict_SetItemString(self->attrs, "window", Py_None);
  if ((v = PyInt_FromLong(obj->any.send_event)) == NULL)
    return NULL;
  PyDict_SetItemString(self->attrs, "send_event", v);
  Py_DECREF(v);
  /* XXX Does anyone need the window attribute?? */
  switch(obj->type) {
  case GDK_NOTHING: break;
  case GDK_DELETE: break;
  case GDK_DESTROY: break;
  case GDK_EXPOSE:            /*GdkEventExpose            expose*/
    if ((v = Py_BuildValue("(iiii)",
			 obj->expose.area.x,
			 obj->expose.area.y,
			 obj->expose.area.width,
			 obj->expose.area.height)) == NULL)
	return NULL;
    PyDict_SetItemString(self->attrs, "area", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->expose.count)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "count", v);
    Py_DECREF(v);
    break;
  case GDK_MOTION_NOTIFY:     /*GdkEventMotion            motion*/
    if ((v = PyInt_FromLong(obj->motion.time)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "time", v);
    Py_DECREF(v);
    if ((v = PyFloat_FromDouble(obj->motion.x)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs,"x", v);
    Py_DECREF(v);
    if ((v = PyFloat_FromDouble(obj->motion.y)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs,"y", v);
    Py_DECREF(v);
    if ((v = PyFloat_FromDouble(obj->motion.pressure)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "pressure", v);
    Py_DECREF(v);
    if ((v = PyFloat_FromDouble(obj->motion.xtilt)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "xtilt", v);
    Py_DECREF(v);
    if ((v = PyFloat_FromDouble(obj->motion.ytilt)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "ytilt", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->motion.state)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "state", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->motion.is_hint)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "is_hint", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->motion.source)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "source", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->motion.deviceid)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "deviceid", v);
    Py_DECREF(v);
    if ((v = PyFloat_FromDouble(obj->motion.x_root)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "x_root", v);
    Py_DECREF(v);
    if ((v = PyFloat_FromDouble(obj->motion.y_root)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "y_root", v);
    Py_DECREF(v);
    break;
  case GDK_BUTTON_PRESS:      /*GdkEventButton            button*/
  case GDK_2BUTTON_PRESS:     /*GdkEventButton            button*/
  case GDK_3BUTTON_PRESS:     /*GdkEventButton            button*/
  case GDK_BUTTON_RELEASE:    /*GdkEventButton            button*/
    if ((v = PyInt_FromLong(obj->button.time)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "time", v);
    Py_DECREF(v);
    if ((v = PyFloat_FromDouble(obj->button.x)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs,"x", v);
    Py_DECREF(v);
    if ((v = PyFloat_FromDouble(obj->button.y)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs,"y", v);
    Py_DECREF(v);
    if ((v = PyFloat_FromDouble(obj->button.pressure)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "pressure", v);
    Py_DECREF(v);
    if ((v = PyFloat_FromDouble(obj->button.xtilt)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "xtilt", v);
    Py_DECREF(v);
    if ((v = PyFloat_FromDouble(obj->button.ytilt)) == NULL)
      return NULL; 
    PyDict_SetItemString(self->attrs, "ytilt", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->button.state)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "state", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->button.button)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "button", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->button.source)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "source", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->button.deviceid)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "deviceid", v);
    Py_DECREF(v);
    if ((v = PyFloat_FromDouble(obj->button.x_root)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "x_root", v);
    Py_DECREF(v);
    if ((v = PyFloat_FromDouble(obj->button.y_root)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "y_root", v);
    Py_DECREF(v);
    break;
  case GDK_KEY_PRESS:         /*GdkEventKey               key*/
  case GDK_KEY_RELEASE:       /*GdkEventKey               key*/
    if ((v = PyInt_FromLong(obj->key.time)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "time", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->key.state)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "state", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->key.keyval)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "keyval", v);
    Py_DECREF(v);
    if ((v = PyString_FromStringAndSize(obj->key.string,
					obj->key.length)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "string", v);
    Py_DECREF(v);
    break;
  case GDK_ENTER_NOTIFY:      /*GdkEventCrossing          crossing*/
  case GDK_LEAVE_NOTIFY:      /*GdkEventCrossing          crossing*/
    if (obj->crossing.subwindow) {
      if ((v = PyGdkWindow_New(obj->crossing.subwindow)) == NULL)
	return NULL;
      PyDict_SetItemString(self->attrs, "subwindow", v);
      Py_DECREF(v);
    } else
      PyDict_SetItemString(self->attrs, "subwindow", Py_None);
    if ((v = PyInt_FromLong(obj->crossing.time)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "time", v);
    Py_DECREF(v);
    if ((v = PyFloat_FromDouble(obj->crossing.x)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "x", v);
    Py_DECREF(v);
    if ((v = PyFloat_FromDouble(obj->crossing.y)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "y", v);
    Py_DECREF(v);
    if ((v = PyFloat_FromDouble(obj->crossing.x_root)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "x_root", v);
    Py_DECREF(v);
    if ((v = PyFloat_FromDouble(obj->crossing.y_root)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "y_root", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->crossing.mode)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "mode", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->crossing.detail)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "detail", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->crossing.focus)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "focus", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->crossing.state)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "state", v);
    Py_DECREF(v);
    break;
  case GDK_FOCUS_CHANGE:      /*GdkEventFocus             focus_change*/
    if ((v = PyInt_FromLong(obj->focus_change.in)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "_in", v);
    Py_DECREF(v);
    break;
  case GDK_CONFIGURE:         /*GdkEventConfigure         configure*/
    if ((v = PyInt_FromLong(obj->configure.x)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "x", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->configure.y)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "y", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->configure.width)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "width", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->configure.height)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "height", v);
    Py_DECREF(v);
    break;
  case GDK_MAP: break;
  case GDK_UNMAP: break;
  case GDK_PROPERTY_NOTIFY:   /*GdkEventProperty          property*/
    if ((v = PyGdkAtom_New(obj->property.atom)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "atom", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->property.time)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "time", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->property.state)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "state", v);
    Py_DECREF(v);
    break;
  case GDK_SELECTION_CLEAR:   /*GdkEventSelection         selection*/
  case GDK_SELECTION_REQUEST: /*GdkEventSelection         selection*/
  case GDK_SELECTION_NOTIFY:  /*GdkEventSelection         selection*/
    if ((v = PyGdkAtom_New(obj->selection.selection)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "selection", v);
    Py_DECREF(v);
    if ((v = PyGdkAtom_New(obj->selection.target)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "target", v);
    Py_DECREF(v);
    if ((v = PyGdkAtom_New(obj->selection.property)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "property", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->selection.requestor)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "requestor", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->selection.time)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "time", v);
    Py_DECREF(v);
    break;
  case GDK_PROXIMITY_IN:      /*GdkEventProximity         proximity*/
  case GDK_PROXIMITY_OUT:     /*GdkEventProximity         proximity*/
    if ((v = PyInt_FromLong(obj->proximity.time)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "time", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->proximity.source)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "source", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->proximity.deviceid)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "deviceid", v);
    Py_DECREF(v);
    break;
  case GDK_DRAG_ENTER:        /*GdkEventDND               dnd*/
  case GDK_DRAG_LEAVE:        /*GdkEventDND               dnd*/
  case GDK_DRAG_MOTION:       /*GdkEventDND               dnd*/
  case GDK_DRAG_STATUS:       /*GdkEventDND               dnd*/
  case GDK_DROP_START:        /*GdkEventDND               dnd*/
  case GDK_DROP_FINISHED:     /*GdkEventDND               dnd*/
    if ((v = PyGdkDragContext_New(obj->dnd.context)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "context", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->dnd.time)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "time", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->dnd.x_root)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "x_root", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->dnd.y_root)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "y_root", v);
    Py_DECREF(v);
    break;
  case GDK_CLIENT_EVENT:      /*GdkEventClient            client*/
    if ((v = PyGdkAtom_New(obj->client.message_type)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "message_type", v);
    Py_DECREF(v);
    if ((v = PyInt_FromLong(obj->client.data_format)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "data_format", v);
    Py_DECREF(v);
    if ((v = PyString_FromStringAndSize(obj->client.data.b, 20)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "data", v);
    Py_DECREF(v);
    break;
  case GDK_VISIBILITY_NOTIFY: /*GdkEventVisibility        visibility*/
    if ((v = PyInt_FromLong(obj->visibility.state)) == NULL)
      return NULL;
    PyDict_SetItemString(self->attrs, "state", v);
    Py_DECREF(v);
    break;
  case GDK_NO_EXPOSE:         /*GdkEventNoExpose          no_expose*/
    break;
  default:
    break;
  }
  return (PyObject *)self;
}

static void
PyGdkEvent_Dealloc(PyGdkEvent_Object *self) {
  Py_XDECREF(self->attrs);
  PyMem_DEL(self);
}

static PyObject *
PyGdkEvent_GetAttr(PyGdkEvent_Object *self, char *attr) {
  PyObject *ret;
  ret = PyDict_GetItemString(self->attrs, attr);
  if (ret != NULL) {
    Py_INCREF(ret);
    return ret;
  }
  if (!strcmp(attr, "__members__")) {
    PyObject *members = PyDict_Keys(self->attrs);

    PyList_Sort(members);
    return members;
  }
  PyErr_SetString(PyExc_AttributeError, attr);
  return NULL;
}

static int
PyGdkEvent_Compare(PyGdkEvent_Object *self, PyGdkEvent_Object *v) {
  if (self->obj == v->obj) return 0;
  if (self->obj > v->obj) return -1;
  return 1;
}

static long PyGdkEvent_Hash(PyGdkEvent_Object *self) {
  return (long)self->obj;
}

static PyTypeObject PyGdkEvent_Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,
  "GdkEvent",
  sizeof(PyGdkEvent_Object),
  0,
  (destructor)PyGdkEvent_Dealloc,
  (printfunc)0,
  (getattrfunc)PyGdkEvent_GetAttr,
  (setattrfunc)0,
  (cmpfunc)PyGdkEvent_Compare,
  (reprfunc)0,
  0,
  0,
  0,
  (hashfunc)PyGdkEvent_Hash,
  (ternaryfunc)0,
  (reprfunc)0,
  0L,0L,0L,0L,
  NULL
};

static void
PyGdkWindow_Dealloc(PyGdkWindow_Object *self) {
  if (gdk_window_get_type(self->obj) == GDK_WINDOW_PIXMAP)
    gdk_pixmap_unref(self->obj);
  else
    gdk_window_unref(self->obj);
  PyMem_DEL(self);
}

static int
PyGdkWindow_Compare(PyGdkWindow_Object *self, PyGdkWindow_Object *v) {
  if (self->obj == v->obj) return 0;
  if (self->obj > v->obj) return -1;
  return 1;
}

static long PyGdkWindow_Hash(PyGdkWindow_Object *self) {
  return (long)self->obj;
}

static PyObject *
PyGdkWindow_Repr(PyGdkWindow_Object *self) {
  char buf[100];
  if (gdk_window_get_type(self->obj) == GDK_WINDOW_PIXMAP)
    sprintf(buf, "<GdkPixmap at %lx>", (long)PyGdkWindow_Get(self));
  else
    sprintf(buf, "<GdkWindow at %lx>", (long)PyGdkWindow_Get(self));
  return PyString_FromString(buf);
}

static PyObject *
PyGdkWindow_NewGC(PyGdkWindow_Object *self, PyObject *args, PyObject *kws) {
  int i = 0;
  PyObject *key, *value;
  char *strkey;
  GdkGCValues values;
  GdkGCValuesMask mask = 0;
  GdkGC *gc;

  if (kws != NULL)
    while (PyDict_Next(kws, &i, &key, &value)) {
      strkey = PyString_AsString(key);
      if (!strcmp(strkey, "foreground")) {
	if (!PyGdkColor_Check(value)) {
	  PyErr_SetString(PyExc_TypeError, "foreground argument takes a GdkColor");
	  return NULL;
	}
	mask |= GDK_GC_FOREGROUND;
	values.foreground.red = PyGdkColor_Get(value)->red;
	values.foreground.green = PyGdkColor_Get(value)->green;
	values.foreground.blue = PyGdkColor_Get(value)->blue;
	values.foreground.pixel = PyGdkColor_Get(value)->pixel;
      } else if (!strcmp(strkey, "background")) {
	if (!PyGdkColor_Check(value)) {
	  PyErr_SetString(PyExc_TypeError, "background argument takes a GdkColor");
	  return NULL;
	}
	mask |= GDK_GC_BACKGROUND;
	values.background.red = PyGdkColor_Get(value)->red;
	values.background.green = PyGdkColor_Get(value)->green;
	values.background.blue = PyGdkColor_Get(value)->blue;
	values.background.pixel = PyGdkColor_Get(value)->pixel;
      } else if (!strcmp(strkey, "font")) {
	if (!PyGdkFont_Check(value)) {
	  PyErr_SetString(PyExc_TypeError, "font argument takes a GdkFont");
	  return NULL;
	}
	mask |= GDK_GC_FONT;
	values.font = PyGdkFont_Get(value);
      } else if (!strcmp(strkey, "tile")) {
	if (!PyGdkWindow_Check(value)) {
	  PyErr_SetString(PyExc_TypeError, "tile argument takes a GdkPixmap");
	  return NULL;
	}
	mask |= GDK_GC_TILE;
	values.tile = PyGdkWindow_Get(value);
      } else if (!strcmp(strkey, "stipple")) {
	if (!PyGdkWindow_Check(value)) {
	  PyErr_SetString(PyExc_TypeError, "stipple argument takes a GdkPixmap");
	  return NULL;
	}
	mask |= GDK_GC_STIPPLE;
	values.stipple = PyGdkWindow_Get(value);
      } else if (!strcmp(strkey, "clip_mask")) {
	if (!PyGdkWindow_Check(value)) {
	  PyErr_SetString(PyExc_TypeError, "clip_mask argument takes a GdkPixmap");
	  return NULL;
	}
	mask |= GDK_GC_CLIP_MASK;
	values.clip_mask = PyGdkWindow_Get(value);
      } else {
	int i = 0;
#ifndef offsetof
#define offsetof(type, member) ( (int) &((type*)0)->member)
#endif
#define OFF(x) offsetof(GdkGCValues, x)
	static struct {char *name;GdkGCValuesMask mask;int offs; } others[] = {
	  {"function", GDK_GC_FUNCTION, OFF(function)},
	  {"fill",     GDK_GC_FILL,     OFF(fill)},
	  {"subwindow_mode", GDK_GC_SUBWINDOW, OFF(subwindow_mode)},
	  {"ts_x_origin", GDK_GC_TS_X_ORIGIN, OFF(ts_x_origin)},
	  {"ts_y_origin", GDK_GC_TS_Y_ORIGIN, OFF(ts_y_origin)},
	  {"clip_x_origin", GDK_GC_CLIP_X_ORIGIN, OFF(clip_x_origin)},
	  {"clip_y_origin", GDK_GC_CLIP_Y_ORIGIN, OFF(clip_y_origin)},
	  {"graphics_exposures", GDK_GC_EXPOSURES, OFF(graphics_exposures)},
	  {"line_width", GDK_GC_LINE_WIDTH, OFF(line_width)},
	  {"line_style", GDK_GC_LINE_STYLE, OFF(line_style)},
	  {"cap_style", GDK_GC_CAP_STYLE, OFF(cap_style)},
	  {"join_style", GDK_GC_JOIN_STYLE, OFF(join_style)},
	  {NULL, 0, 0}
	};
#undef OFF
	while (others[i].name != NULL) {
	  if (!strcmp(strkey, others[i].name)) {
	    if (!PyInt_Check(value)) {
	      char buf[80];
	      g_snprintf(buf, 79, "%s argument expects an integer",
			 others[i].name);
	      PyErr_SetString(PyExc_TypeError, buf);
	      return NULL;
	    }
	    mask |= others[i].mask;
	    *((int *)((char *)&values + others[i].offs)) = PyInt_AsLong(value);
	    break;
	  }
	  i++;
	}
	if (others[i].name == NULL) {
	  PyErr_SetString(PyExc_TypeError, "unknown argument");
	  return NULL;
	}
      }
    }
  if (!PyArg_ParseTuple(args, ":GdkWindow.new_gc"))
    return NULL;
  gc = gdk_gc_new_with_values(PyGdkWindow_Get(self), &values, mask);
  value = PyGdkGC_New(gc);
  gdk_gc_unref(gc);
  return value;
}

static PyObject *PyGdkWindow_SetCursor(PyGdkWindow_Object *self,
				       PyObject *args) {
  PyObject *py_cursor;
  GdkCursor *cursor = NULL;

  if (!PyArg_ParseTuple(args, "O:GdkWindow.set_cursor", &py_cursor))
    return NULL;
  if (PyGdkCursor_Check(py_cursor))
      cursor = PyGdkCursor_Get(py_cursor);
  else if (py_cursor != Py_None) {
      PyErr_SetString(PyExc_TypeError, "argument must be a GdkCursor or None");
      return NULL;
  }
  gdk_window_set_cursor(self->obj, cursor);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *PyGdkWindow_PropertyGet(PyGdkWindow_Object *self,
					 PyObject *args) {
    GdkAtom property, type = 0;
    gint pdelete = FALSE;

    GdkAtom atype;
    gint aformat, alength;
    guchar *data;

    if (!PyArg_ParseTuple(args, "i|ii:GdkWindow.property_get", &property,
			  &type, &pdelete)) {
	gchar *propname;

	PyErr_Clear();
	if (!PyArg_ParseTuple(args, "s|ii:GdkWindow.property_get", &propname,
			      &type, &pdelete))
	    return NULL;
	property = gdk_atom_intern(propname, FALSE);
    }
    if (gdk_property_get(self->obj, property, type, 0, 9999, pdelete,
			 &atype, &aformat, &alength, &data)) {
	/* success */
	PyObject *pdata = NULL;
	gint i;
	guint16 *data16;
	guint32 *data32;
	switch (aformat) {
	case 8:
	    if ((pdata = PyString_FromStringAndSize(data, alength)) == NULL)
	        return NULL;
	    break;
	case 16:
	    data16 = (guint16 *)data;
	    if ((pdata = PyTuple_New(alength)) == NULL)
	        return NULL;
	    for (i = 0; i < alength; i++)
		PyTuple_SetItem(pdata, i, PyInt_FromLong(data16[i]));
	    break;
	case 32:
	    data32 = (guint32 *)data;
	    if ((pdata = PyTuple_New(alength)) == NULL)
	        return NULL;
	    for (i = 0; i < alength; i++)
		PyTuple_SetItem(pdata, i, PyInt_FromLong(data32[i]));
	    break;
	default:
	    g_warning("got a property format != 8, 16 or 32");
	    g_assert_not_reached();
	}
	g_free(data);
	return Py_BuildValue("(NiN)", PyGdkAtom_New(atype), aformat, pdata);
    } else {
	Py_INCREF(Py_None);
	return Py_None;
    }
}

static PyObject *PyGdkWindow_PropertyChange(PyGdkWindow_Object *self,
					    PyObject *args) {
    GdkAtom property, type;
    gint format;
    PyObject *py_mode, *pdata;
    GdkPropMode mode;
    guchar *data = NULL;
    gint nelements;

    if (!PyArg_ParseTuple(args, "iiiOO:GdkWindow.property_change", &property,
			  &type, &format, &py_mode, &pdata)) {
	gchar *propname;

	PyErr_Clear();
	if (!PyArg_ParseTuple(args, "siiOO:GdkWindow.property_change",
			      &propname, &type, &format, &py_mode, &pdata))
	    return NULL;
	property = gdk_atom_intern(propname, FALSE);
    }
    if (PyGtkEnum_get_value(GTK_TYPE_GDK_PROP_MODE, py_mode, (gint *)&mode))
	return NULL;
    switch (format) {
    case 8:
	if (!PyString_Check(pdata)) {
	    PyErr_SetString(PyExc_TypeError, "data not a string and format=8");
	    return NULL;
	}
	data = PyString_AsString(pdata);
	nelements = PyString_Size(pdata);
	break;
    case 16:
	{
	    guint16 *data16;
	    gint i;

	    if (!PySequence_Check(pdata)) {
		PyErr_SetString(PyExc_TypeError, "data not a sequence and format=16");
		return NULL;
	    }
	    nelements = PySequence_Length(pdata);
	    data16 = g_new(guint16, nelements);
	    data = (guchar *)data16;
	    for (i = 0; i < nelements; i++) {
		PyObject *item = PySequence_GetItem(pdata, i);
		Py_DECREF(item);
		item = PyNumber_Int(item);
		if (!item) {
		    g_free(data16);
		    PyErr_Clear();
		    PyErr_SetString(PyExc_TypeError,"data element not an int");
		    return NULL;
		}
		data16[i] = PyInt_AsLong(item);
		Py_DECREF(item);
	    }
	}
	break;
    case 32:
	{
	    guint32 *data32;
	    gint i;

	    if (!PySequence_Check(pdata)) {
		PyErr_SetString(PyExc_TypeError, "data not a sequence and format=32");
		return NULL;
	    }
	    nelements = PySequence_Length(pdata);
	    data32 = g_new(guint32, nelements);
	    data = (guchar *)data32;
	    for (i = 0; i < nelements; i++) {
		PyObject *item = PySequence_GetItem(pdata, i);
		Py_DECREF(item);
		item = PyNumber_Int(item);
		if (!item) {
		    g_free(data32);
		    PyErr_Clear();
		    PyErr_SetString(PyExc_TypeError,"data element not an int");
		    return NULL;
		}
		data32[i] = PyInt_AsLong(item);
		Py_DECREF(item);
	    }
	}
	break;
    default:
	PyErr_SetString(PyExc_TypeError, "format must be 8, 16 or 32");
	return NULL;
	break;
    }
    gdk_property_change(self->obj, property, type, format, mode,
			data, nelements);
    if (format != 8)
	g_free(data);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *PyGdkWindow_PropertyDelete(PyGdkWindow_Object *self,
					    PyObject *args) {
    GdkAtom property;

    if (!PyArg_ParseTuple(args, "i:GdkWindow.property_delete", &property)) {
	gchar *propname;

	PyErr_Clear();
	if (!PyArg_ParseTuple(args, "s:GdkWindow.property_delete", &propname))
	    return NULL;
	property = gdk_atom_intern(propname, FALSE);
    }
    gdk_property_delete(self->obj, property);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *PyGdkWindow_Raise(PyGdkWindow_Object *self,
				   PyObject *args) {
  if (!PyArg_ParseTuple(args, ":GdkWindow.raise_"))
    return NULL;
  gdk_window_raise(self->obj);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *PyGdkWindow_Lower(PyGdkWindow_Object *self,
				   PyObject *args) {
  if (!PyArg_ParseTuple(args, ":GdkWindow.lower"))
    return NULL;
  gdk_window_lower(self->obj);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *PyGdkWindow_InputGetPointer(PyGdkWindow_Object *self,
					     PyObject *args) {
  guint32 deviceid;
  gdouble x = 0.0, y = 0.0, pressure = 0.0, xtilt = 0.0, ytilt = 0.0;
  GdkModifierType mask = 0;

  if (!PyArg_ParseTuple(args, "i:GdkWindow.input_get_pointer", &deviceid))
    return NULL;
  gdk_input_window_get_pointer(self->obj, deviceid, &x, &y, &pressure,
			       &xtilt, &ytilt, &mask);
  return Py_BuildValue("(dddddi)", x, y, pressure, xtilt, ytilt, mask);
}

static PyObject *PyGdkWindow_PointerGrab(PyGdkWindow_Object *self,
					 PyObject *args) {
  gint owner_events, time = GDK_CURRENT_TIME;
  GdkEventMask event_mask;
  GdkWindow *confine_to = NULL;
  GdkCursor *cursor = NULL;
  PyObject *py_confine_to = Py_None, *py_cursor = Py_None;

  if (!PyArg_ParseTuple(args, "ii|OOi:GdkWindow.pointer_grab", &owner_events,
			&event_mask, &py_confine_to, &py_cursor, &time))
    return NULL;
  if (PyGdkWindow_Check(py_confine_to))
    confine_to = PyGdkWindow_Get(py_confine_to);
  else if (py_confine_to != Py_None) {
    PyErr_SetString(PyExc_TypeError,
		    "third argument should be a GdkWindow or None");
    return NULL;
  }
  if (PyGdkCursor_Check(py_cursor))
    cursor = PyGdkCursor_Get(py_cursor);
  else if (py_cursor != Py_None) {
    PyErr_SetString(PyExc_TypeError,
		    "forth argument should be a GdkCursor or None");
    return NULL;
  }
  return PyInt_FromLong(gdk_pointer_grab(self->obj, owner_events, event_mask,
					 confine_to, cursor, time));
}

static PyObject *PyGdkWindow_PointerUngrab(PyGdkWindow_Object *self,
					   PyObject *args) {
  gint time = GDK_CURRENT_TIME;

  if (!PyArg_ParseTuple(args, "|i:GdkWindow.pointer_ungrab", &time))
    return NULL;
  gdk_pointer_ungrab(time);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *PyGdkWindow_KeyboardGrab(PyGdkWindow_Object *self,
					  PyObject *args) {
  gint owner_events, time = GDK_CURRENT_TIME;

  if (!PyArg_ParseTuple(args, "i|i:GdkWindow.keyboard_grab", &owner_events,
			&time))
    return NULL;
  return PyInt_FromLong(gdk_keyboard_grab(self->obj, owner_events, time));
}

static PyObject *PyGdkWindow_KeyboardUngrab(PyGdkWindow_Object *self,
					    PyObject *args) {
  gint time = GDK_CURRENT_TIME;

  if (!PyArg_ParseTuple(args, "|i:GdkWindow.keyboard_ungrab", &time))
    return NULL;
  gdk_keyboard_ungrab(time);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *PyGdkWindow_Show(PyGdkWindow_Object *self,
				  PyObject *args) {
  if (!PyArg_ParseTuple(args, ":GdkWindow._show"))
    return NULL;
  gdk_window_show(self->obj);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *PyGdkWindow_Hide(PyGdkWindow_Object *self,
				  PyObject *args) {
  if (!PyArg_ParseTuple(args, ":GdkWindow._hide"))
    return NULL;
  gdk_window_hide(self->obj);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *PyGdkWindow_Destroy(PyGdkWindow_Object *self,
				     PyObject *args) {
  if (!PyArg_ParseTuple(args, ":GdkWindow._destroy"))
    return NULL;
  gdk_window_destroy(self->obj);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *PyGdkWindow_Clear(PyGdkWindow_Object *self,
				   PyObject *args) {
  if (!PyArg_ParseTuple(args, ":GdkWindow.clear"))
    return NULL;
  gdk_window_clear(self->obj);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef PyGdkWindow_methods[] = {
  {"new_gc", (PyCFunction)PyGdkWindow_NewGC, METH_VARARGS|METH_KEYWORDS, NULL},
  {"set_cursor", (PyCFunction)PyGdkWindow_SetCursor, METH_VARARGS, NULL},
  {"property_get", (PyCFunction)PyGdkWindow_PropertyGet, METH_VARARGS, NULL},
  {"property_change", (PyCFunction)PyGdkWindow_PropertyChange, METH_VARARGS, NULL},
  {"property_delete", (PyCFunction)PyGdkWindow_PropertyDelete, METH_VARARGS, NULL},
  {"raise_", (PyCFunction)PyGdkWindow_Raise, METH_VARARGS, NULL},
  {"_raise", (PyCFunction)PyGdkWindow_Raise, METH_VARARGS, NULL},
  {"lower", (PyCFunction)PyGdkWindow_Lower, METH_VARARGS, NULL},
  {"input_get_pointer", (PyCFunction)PyGdkWindow_InputGetPointer, METH_VARARGS, NULL},
  {"pointer_grab", (PyCFunction)PyGdkWindow_PointerGrab, METH_VARARGS, NULL},
  {"pointer_ungrab", (PyCFunction)PyGdkWindow_PointerUngrab, METH_VARARGS, NULL},
  {"keyboard_grab", (PyCFunction)PyGdkWindow_KeyboardGrab, METH_VARARGS, NULL},
  {"keyboard_ungrab", (PyCFunction)PyGdkWindow_KeyboardUngrab, METH_VARARGS, NULL},
  {"show", (PyCFunction)PyGdkWindow_Show, METH_VARARGS, NULL},
  {"hide", (PyCFunction)PyGdkWindow_Hide, METH_VARARGS, NULL},
  {"_destroy", (PyCFunction)PyGdkWindow_Destroy, METH_VARARGS, NULL},
  {"clear", (PyCFunction)PyGdkWindow_Clear, METH_VARARGS, NULL},
  {NULL, 0, 0, NULL}
};

static PyObject *
PyGdkWindow_GetAttr(PyGdkWindow_Object *self, char *key)
{
    GdkWindow *win = PyGdkWindow_Get(self);
    gint x, y;
    GdkModifierType p_mask;

    if (!strcmp(key, "__members__")) {
	if (gdk_window_get_type(win) != GDK_WINDOW_PIXMAP)
	    return Py_BuildValue("[sssss]", "colormap", "depth", "height",
				 "type", "width");
	else
	    return Py_BuildValue("[ssssssssssssssss]", "children", "colormap",
				 "depth", "deskrelative_origin", "height",
				 "origin", "parent", "pointer",
				 "pointer_state", "root_origin", "toplevel",
				 "type", "width", "x", "xid", "y");
    }
    if (!strcmp(key, "width")) {
	gdk_window_get_size(win, &x, NULL);
	return PyInt_FromLong(x);
    }
    if (!strcmp(key, "height")) {
	gdk_window_get_size(win, NULL, &y);
	return PyInt_FromLong(y);
    }
    if (!strcmp(key, "colormap"))
	return PyGdkColormap_New(gdk_window_get_colormap(win));
    if (!strcmp(key, "type"))
	return PyInt_FromLong(gdk_window_get_type(win));
    if (!strcmp(key, "depth")) {
	gdk_window_get_geometry(win, NULL, NULL, NULL, NULL, &x);
	return PyInt_FromLong(x);
    }
#ifdef WITH_XSTUFF
    if (!strcmp(key, "xid"))
	return PyInt_FromLong(GDK_WINDOW_XWINDOW(win));
#endif

    if (gdk_window_get_type(win) != GDK_WINDOW_PIXMAP) {
	if (!strcmp(key, "x")) {
	    gdk_window_get_position(win, &x, NULL);
	    return PyInt_FromLong(x);
	}
	if (!strcmp(key, "y")) {
	    gdk_window_get_position(win, NULL, &y);
	    return PyInt_FromLong(y);
	}
	if (!strcmp(key, "pointer")) {
	    gdk_window_get_pointer(win, &x, &y, NULL);
	    return Py_BuildValue("(ii)", x, y);
	}
	if (!strcmp(key, "pointer_state")) {
	    gdk_window_get_pointer(win, NULL, NULL, &p_mask);
	    return PyInt_FromLong(p_mask);
	}
	if (!strcmp(key, "parent")) {
	    GdkWindow *par = gdk_window_get_parent(win);
	    if (par)
		return PyGdkWindow_New(par);
	    Py_INCREF(Py_None);
	    return Py_None;
	}
	if (!strcmp(key, "toplevel"))
	    return PyGdkWindow_New(gdk_window_get_toplevel(win));
	if (!strcmp(key, "children")) {
	    GList *children, *tmp;
	    PyObject *ret;
	    children = gdk_window_get_children(win);
	    if ((ret = PyList_New(0)) == NULL)
		return NULL;
	    for (tmp = children; tmp != NULL; tmp = tmp->next) {
		PyObject *win = PyGdkWindow_New(tmp->data);
		if (win == NULL) {
		    Py_DECREF(ret);
		    return NULL;
		}
		PyList_Append(ret, win);
		Py_DECREF(win);
	    }
	    g_list_free(children);
	    return ret;
	}
	if (!strcmp(key, "origin")) {
	    gint x, y;
	    gdk_window_get_origin(win, &x, &y);
	    return Py_BuildValue("(ii)", x, y);
	}
	if (!strcmp(key, "deskrelative_origin")) {
	    gint x, y;
	    gdk_window_get_deskrelative_origin(win, &x, &y);
	    return Py_BuildValue("(ii)", x, y);
	}
	if (!strcmp(key, "root_origin")) {
	    gint x, y;
	    gdk_window_get_root_origin(win, &x, &y);
	    return Py_BuildValue("(ii)", x, y);
	}
    }

    return Py_FindMethod(PyGdkWindow_methods, (PyObject *)self, key);
}

static PyTypeObject PyGdkWindow_Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,
  "GdkWindow",
  sizeof(PyGdkWindow_Object),
  0,
  (destructor)PyGdkWindow_Dealloc,
  (printfunc)0,
  (getattrfunc)PyGdkWindow_GetAttr,
  (setattrfunc)0,
  (cmpfunc)PyGdkWindow_Compare,
  (reprfunc)PyGdkWindow_Repr,
  0,
  0,
  0,
  (hashfunc)PyGdkWindow_Hash,
  (ternaryfunc)0,
  (reprfunc)0,
  0L,0L,0L,0L,
  NULL
};

static void
PyGdkGC_Dealloc(PyGdkGC_Object *self) {
  gdk_gc_unref(self->obj);
  PyMem_DEL(self);
}

static PyObject *PyGdkGC_set_dashes(PyGdkGC_Object *self, PyObject *args) {
  gint dash_offset, n, i;
  PyObject *list;
  guchar *dash_list;

  if (!PyArg_ParseTuple(args, "iO:GdkGC.set_dashes", &dash_offset, &list))
    return NULL;
  if (!PySequence_Check(list)) {
    PyErr_SetString(PyExc_TypeError, "second argument must be a sequence");
    return NULL;
  }
  n = PySequence_Length(list);
  dash_list = g_new(char, n);
  for (i = 0; i < n; i++) {
    PyObject *item = PySequence_GetItem(list, i);
    Py_DECREF(item);

    if (!PyInt_Check(item)) {
      PyErr_SetString(PyExc_TypeError, "sequence member must be an int");
      g_free(dash_list);
      return NULL;
    }
    dash_list[i] = (guchar)PyInt_AsLong(item);
    if (dash_list[i] == 0) {
      PyErr_SetString(PyExc_TypeError, "sequence member must not be 0");
      g_free(dash_list);
      return NULL;
    }
  }
  gdk_gc_set_dashes(self->obj, dash_offset, dash_list, n);
  g_free(dash_list);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *PyGdkGC_set_clip_rectangle(PyGdkGC_Object *self, PyObject *args) {
    int x, y, width, height;
    PyObject *gc;
    GdkRectangle clip;

    if (!PyArg_ParseTuple(args, "iiii:gdk_gc_set_clip_rectangle", &x, &y, &width, &height))
        return NULL;

    clip.x = x;
    clip.y = y;
    clip.width = width;
    clip.height = height;

    gdk_gc_set_clip_rectangle(self->obj, &clip);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef PyGdkGC_methods[] = {
  {"set_dashes", (PyCFunction)PyGdkGC_set_dashes, METH_VARARGS, NULL},
  {"set_clip_rectangle", (PyCFunction)PyGdkGC_set_clip_rectangle, METH_VARARGS, NULL},
  {NULL, 0, 0, NULL}
};

static PyObject *
PyGdkGC_GetAttr(PyGdkGC_Object *self, char *key) {
  GdkGCValues gc;

  if (!strcmp(key, "__members__"))
    return Py_BuildValue("[ssssssssssssssssss]", "background", "cap_style",
			 "clip_mask", "clip_x_origin", "clip_y_origin",
			 "fill", "font", "foreground", "function",
			 "graphics_exposures", "join_style", "line_style",
			 "line_width", "stipple", "sub_window", "tile",
			 "ts_x_origin", "ts_y_origin");
  gdk_gc_get_values(self->obj, &gc);
  if (!strcmp(key, "foreground")) return PyGdkColor_New(&(gc.foreground));
  if (!strcmp(key, "background")) return PyGdkColor_New(&(gc.background));
  if (!strcmp(key, "font")) return PyGdkFont_New(gc.font);
  if (!strcmp(key, "function")) return PyInt_FromLong(gc.function);
  if (!strcmp(key, "fill")) return PyInt_FromLong(gc.fill);
  if (!strcmp(key, "tile")) {
    if (gc.tile != NULL) return PyGdkWindow_New(gc.tile);
    Py_INCREF(Py_None); return Py_None;
  }
  if (!strcmp(key, "stipple")) {
    if (gc.stipple != NULL) return PyGdkWindow_New(gc.stipple);
    Py_INCREF(Py_None); return Py_None;
  }
  if (!strcmp(key, "clip_mask")) {
    if (gc.clip_mask!=NULL) return PyGdkWindow_New(gc.clip_mask);
    Py_INCREF(Py_None); return Py_None;
  }
  if (!strcmp(key, "subwindow_mode"))
    return PyInt_FromLong(gc.subwindow_mode);
  if (!strcmp(key, "ts_x_origin")) return PyInt_FromLong(gc.ts_x_origin);
  if (!strcmp(key, "ts_y_origin")) return PyInt_FromLong(gc.ts_y_origin);
  if (!strcmp(key, "clip_x_origin")) return PyInt_FromLong(gc.clip_x_origin);
  if (!strcmp(key, "clip_y_origin")) return PyInt_FromLong(gc.clip_y_origin);
  if (!strcmp(key, "graphics_exposures"))
    return PyInt_FromLong(gc.graphics_exposures);
  if (!strcmp(key, "line_width")) return PyInt_FromLong(gc.line_width);
  if (!strcmp(key, "line_style")) return PyInt_FromLong(gc.line_style);
  if (!strcmp(key, "cap_style")) return PyInt_FromLong(gc.cap_style);
  if (!strcmp(key, "join_style")) return PyInt_FromLong(gc.join_style);

  return Py_FindMethod(PyGdkGC_methods, (PyObject *)self, key);
}

static int
PyGdkGC_SetAttr(PyGdkGC_Object *self, char *key, PyObject *value) {
  GdkGC *gc = self->obj;

  if (value == NULL) {
    PyErr_SetString(PyExc_TypeError, "can't delete attributes");
    return -1;
  }
  if (PyInt_Check(value)) {
    int i = PyInt_AsLong(value);
    GdkGCValues v;
    gdk_gc_get_values(gc, &v);
    if (!strcmp(key, "function")) gdk_gc_set_function(gc, i);
    else if (!strcmp(key, "fill")) gdk_gc_set_fill(gc, i);
    else if (!strcmp(key, "subwindow_mode")) gdk_gc_set_subwindow(gc, i);
    else if (!strcmp(key, "ts_x_origin"))
      gdk_gc_set_ts_origin(gc, i, v.ts_y_origin);
    else if (!strcmp(key, "ts_y_origin"))
      gdk_gc_set_ts_origin(gc, v.ts_x_origin, i);
    else if (!strcmp(key, "clip_x_origin"))
      gdk_gc_set_clip_origin(gc, i, v.clip_y_origin);
    else if (!strcmp(key, "clip_y_origin"))
      gdk_gc_set_clip_origin(gc, v.clip_x_origin, i);
    else if (!strcmp(key, "graphics_exposures")) gdk_gc_set_exposures(gc, i);
    else if (!strcmp(key, "line_width"))
      gdk_gc_set_line_attributes(gc, i,v.line_style,v.cap_style,v.join_style);
    else if (!strcmp(key, "line_style"))
      gdk_gc_set_line_attributes(gc, v.line_width,i,v.cap_style,v.join_style);
    else if (!strcmp(key, "cap_style"))
      gdk_gc_set_line_attributes(gc, v.line_width,v.line_style,i,v.join_style);
    else if (!strcmp(key, "join_style"))
      gdk_gc_set_line_attributes(gc, v.line_width,v.line_style,v.cap_style,i);
    else {
      PyErr_SetString(PyExc_TypeError,"type mismatch");
      return -1;
    }
  } else if (PyGdkColor_Check(value)) {
    GdkColor *c = PyGdkColor_Get(value);
    if (!strcmp(key, "foreground")) gdk_gc_set_foreground(gc, c);
    else if (!strcmp(key, "background")) gdk_gc_set_background(gc, c);
    else {
      PyErr_SetString(PyExc_TypeError,"type mismatch");
      return -1;
    }
  } else if (PyGdkFont_Check(value)) {
    if (!strcmp(key, "font")) gdk_gc_set_font(gc, PyGdkFont_Get(value));
    else{
      PyErr_SetString(PyExc_TypeError,"type mismatch");
      return -1;
    }
  } else if (PyGdkWindow_Check(value) || value == Py_None) {
    GdkWindow *w = (value==Py_None)?NULL:PyGdkWindow_Get(value);
    if (!strcmp(key, "tile")) gdk_gc_set_tile(gc, w);
    else if (!strcmp(key, "stipple")) gdk_gc_set_stipple(gc, w);
    else if (!strcmp(key, "clip_mask")) gdk_gc_set_clip_mask(gc, w);
    else {
      PyErr_SetString(PyExc_TypeError,"type mismatch");
      return -1;
    }
  } else {
    PyErr_SetString(PyExc_TypeError,"type mismatch");
      return -1;
  }
  return 0;
}

static int
PyGdkGC_Compare(PyGdkGC_Object *self, PyGdkGC_Object *v) {
  if (self->obj == v->obj) return 0;
  if (self->obj > v->obj) return -1;
  return 1;
}

static long PyGdkGC_Hash(PyGdkGC_Object *self) {
  return (long)self->obj;
}

static PyTypeObject PyGdkGC_Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,
  "GdkGC",
  sizeof(PyGdkGC_Object),
  0,
  (destructor)PyGdkGC_Dealloc,
  (printfunc)0,
  (getattrfunc)PyGdkGC_GetAttr,
  (setattrfunc)PyGdkGC_SetAttr,
  (cmpfunc)PyGdkGC_Compare,
  (reprfunc)0,
  0,
  0,
  0,
  (hashfunc)PyGdkGC_Hash,
  (ternaryfunc)0,
  (reprfunc)0,
  0L,0L,0L,0L,
  NULL
};

static void
PyGdkColormap_Dealloc(PyGdkColormap_Object *self) {
  gdk_colormap_unref(self->obj);
  PyMem_DEL(self);
}

static PyObject *
PyGdkColor_Alloc(PyGdkColormap_Object *self, PyObject *args) {
  gint red = 0, green = 0, blue = 0;
  GdkColor color = {0, 0, 0, 0};
  gchar *color_name;
  if (!PyArg_ParseTuple(args, "iii:GdkColormap.alloc",
			&red, &green, &blue)) {
    PyErr_Clear();
    if (!PyArg_ParseTuple(args, "s:GdkColormap.alloc", &color_name))
      return NULL;
    if (!gdk_color_parse(color_name, &color)) {
      PyErr_SetString(PyExc_TypeError, "unable to parse color specification");
      return NULL;
    }
  } else {
    color.red = red;
    color.green = green;
    color.blue = blue;
  }
  if (!gdk_color_alloc(self->obj, &color)) {
    PyErr_SetString(PyExc_RuntimeError, "couldn't allocate color");
    return NULL;
  }
  return PyGdkColor_New(&color);
}

static PyMethodDef PyGdkColormap_methods[] = {
  {"alloc", (PyCFunction)PyGdkColor_Alloc, METH_VARARGS, NULL},
  {NULL, 0, 0, NULL}
};

static PyObject *
PyGdkColormap_GetAttr(PyObject *self, char *key) {
  return Py_FindMethod(PyGdkColormap_methods, self, key);
}

static int
PyGdkColormap_Compare(PyGdkColormap_Object *self, PyGdkColormap_Object *v) {
  if (self->obj == v->obj) return 0;
  if (self->obj > v->obj) return -1;
  return 1;
}

static long PyGdkColormap_Hash(PyGdkColormap_Object *self) {
  return (long)self->obj;
}

static int
PyGdkColormap_Length(PyGdkColormap_Object *self) {
  return self->obj->size;
}

static PyObject *
PyGdkColormap_GetItem(PyGdkColormap_Object *self, int pos) {
  if (pos < 0 || pos >= self->obj->size) {
    PyErr_SetString(PyExc_IndexError, "index out of range");
    return NULL;
  }
  return PyGdkColor_New(&(self->obj->colors[pos]));
}

static PyObject *
PyGdkColormap_GetSlice(PyGdkColormap_Object *self, int lo, int up) {
  PyObject *ret;
  int i;

  if (lo < 0) lo = 0;
  if (up > self->obj->size) up = self->obj->size;
  if (up < lo) up = lo;

  ret = PyTuple_New(up - lo);
  if (ret == NULL) return NULL;
  for (i = lo; i < up; i++)
    PyTuple_SetItem(ret, i - lo, PyGdkColor_New(&(self->obj->colors[i])));
  return ret;
}

static PySequenceMethods PyGdkColormap_Sequence = {
  (inquiry)PyGdkColormap_Length,
  (binaryfunc)0,
  (intargfunc)0,
  (intargfunc)PyGdkColormap_GetItem,
  (intintargfunc)PyGdkColormap_GetSlice,
  (intobjargproc)0,
  (intintobjargproc)0
};

static PyTypeObject PyGdkColormap_Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,
  "GdkColormap",
  sizeof(PyGdkColormap_Object),
  0,
  (destructor)PyGdkColormap_Dealloc,
  (printfunc)0,
  (getattrfunc)PyGdkColormap_GetAttr,
  (setattrfunc)0,
  (cmpfunc)PyGdkColormap_Compare,
  (reprfunc)0,
  0,
  &PyGdkColormap_Sequence,
  0,
  (hashfunc)PyGdkColormap_Hash,
  (ternaryfunc)0,
  (reprfunc)0,
  0L,0L,0L,0L,
  NULL
};

static void
PyGdkDragContext_Dealloc(PyGdkDragContext_Object *self) {
  gdk_drag_context_unref(self->obj);
  PyMem_DEL(self);
}

static int
PyGdkDragContext_Compare(PyGdkDragContext_Object *self,
			 PyGdkDragContext_Object *v) {
  if (self->obj == v->obj) return 0;
  if (self->obj > v->obj) return -1;
  return 1;
}

static long PyGdkDragContext_Hash(PyGdkDragContext_Object *self) {
  return (long)self->obj;
}

static PyObject *
PyGdkDragContext_GetAttr(PyGdkDragContext_Object *self, char *key) {
  if (!strcmp(key, "__members__"))
    return Py_BuildValue("[sssssssss]", "action", "actions", "dest_window",
			 "is_source", "protocol", "source_window",
			 "start_window", "suggested_action", "targets");
  else if (!strcmp(key, "protocol"))
    return PyInt_FromLong(self->obj->protocol);
  else if (!strcmp(key, "is_source"))
    return PyInt_FromLong(self->obj->is_source);
  else if (!strcmp(key, "source_window"))
    if (self->obj->source_window)
      return PyGdkWindow_New(self->obj->source_window);
    else {
      Py_INCREF(Py_None);
      return Py_None;
    }
  else if (!strcmp(key, "dest_window"))
    if (self->obj->dest_window)
      return PyGdkWindow_New(self->obj->dest_window);
    else {
      Py_INCREF(Py_None);
      return Py_None;
    }
  else if (!strcmp(key, "targets")) {
    PyObject *atom, *ret = PyList_New(0);
    GList *tmp;

    if (ret == NULL)
      return NULL;
    for (tmp = self->obj->targets; tmp != NULL; tmp = tmp->next) {
      if ((atom = PyGdkAtom_New(GPOINTER_TO_INT(tmp->data))) == NULL) {
	Py_DECREF(ret);
	return NULL;
      }
      PyList_Append(ret, atom);
      Py_DECREF(atom);
    }
    return ret;
  } else if (!strcmp(key, "actions"))
    return PyInt_FromLong(self->obj->actions);
  else if (!strcmp(key, "suggested_action"))
    return PyInt_FromLong(self->obj->suggested_action);
  else if (!strcmp(key, "action"))
    return PyInt_FromLong(self->obj->action);
  else if (!strcmp(key, "start_time"))
    return PyInt_FromLong(self->obj->start_time);
  PyErr_SetString(PyExc_AttributeError, key);
  return NULL;
}

static PyTypeObject PyGdkDragContext_Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,
  "GdkDragContext",
  sizeof(PyGdkDragContext_Object),
  0,
  (destructor)PyGdkDragContext_Dealloc,
  (printfunc)0,
  (getattrfunc)PyGdkDragContext_GetAttr,
  (setattrfunc)0,
  (cmpfunc)PyGdkDragContext_Compare,
  (reprfunc)0,
  0,
  0,
  0,
  (hashfunc)PyGdkDragContext_Hash,
  (ternaryfunc)0,
  (reprfunc)0,
  0L,0L,0L,0L,
  NULL
};

static void
PyGtkSelectionData_Dealloc(PyGtkSelectionData_Object *self) {
  PyMem_DEL(self);
}

static int
PyGtkSelectionData_Compare(PyGtkSelectionData_Object *self,
			   PyGtkSelectionData_Object *v) {
  if (self->obj == v->obj) return 0;
  if (self->obj > v->obj) return -1;
  return 1;
}

static long PyGtkSelectionData_Hash(PyGtkSelectionData_Object *self) {
  return (long)self->obj;
}

static PyObject *
PyGtkSelectionData_Set(PyGtkSelectionData_Object *self, PyObject *args) {
  GdkAtom type;
  int format, length;
  guchar *data;

  if (!PyArg_ParseTuple(args, "iis#:GtkSelectionData.set", &type, &format,
			&data, &length))
    return NULL;
  gtk_selection_data_set(self->obj, type, format, data, length);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef PyGtkSelectionData_methods[] = {
  {"set", (PyCFunction)PyGtkSelectionData_Set, METH_VARARGS, NULL},
  {NULL, 0, 0, NULL}
};

static PyObject *
PyGtkSelectionData_GetAttr(PyGtkSelectionData_Object *self, char *key) {
  if (!strcmp(key, "__members__"))
    return Py_BuildValue("[ssssss]", "data", "format", "selection", "target",
			 "type", "length");

  if (!strcmp(key, "selection"))
    return PyGdkAtom_New(self->obj->selection);
  else if (!strcmp(key, "target"))
    return PyGdkAtom_New(self->obj->target);
  else if (!strcmp(key, "type"))
      return PyGdkAtom_New(self->obj->type);
  else if (!strcmp(key, "format"))
    return PyInt_FromLong(self->obj->format);
  else if (!strcmp(key, "length"))
    return PyInt_FromLong(self->obj->length);
  else if (!strcmp(key, "data")) {
    if (self->obj->length > -1) {
      return PyString_FromStringAndSize(self->obj->data, self->obj->length);
    }
    else {
      Py_INCREF (Py_None);
      return Py_None;
    }
  }

  return Py_FindMethod(PyGtkSelectionData_methods, (PyObject *)self, key);
}

static PyTypeObject PyGtkSelectionData_Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,
  "GtkSelectionData",
  sizeof(PyGtkSelectionData_Object),
  0,
  (destructor)PyGtkSelectionData_Dealloc,
  (printfunc)0,
  (getattrfunc)PyGtkSelectionData_GetAttr,
  (setattrfunc)0,
  (cmpfunc)PyGtkSelectionData_Compare,
  (reprfunc)0,
  0,
  0,
  0,
  (hashfunc)PyGtkSelectionData_Hash,
  (ternaryfunc)0,
  (reprfunc)0,
  0L,0L,0L,0L,
  NULL
};

static void
PyGdkAtom_Dealloc(PyGdkAtom_Object *self) {
  if (self->name) g_free(self->name);
  PyMem_DEL(self);
}

static int
PyGdkAtom_Compare(PyGdkAtom_Object *self, PyGdkAtom_Object *v) {
  if (self->atom == v->atom) return 0;
  if (self->atom > v->atom) return -1;
  return 1;
}

static long PyGdkAtom_Hash(PyGdkAtom_Object *self) {
  return (long)self->atom;
}

static PyObject *
PyGdkAtom_Repr(PyGdkAtom_Object *self) {
  char buf[256];
  if (!self->name) self->name = gdk_atom_name(self->atom);
  g_snprintf(buf, 256, "<GdkAtom 0x%lx = '%s'>", (unsigned long)self->atom,
	     self->name?self->name:"(null)");
  return PyString_FromString(buf);
}

static int
PyGdkAtom_Coerce(PyObject **self, PyObject **other) {
  PyGdkAtom_Object *old = (PyGdkAtom_Object *)*self;
  if (PyInt_Check(*other)) {
    *self = PyInt_FromLong(old->atom);
    Py_INCREF(*other);
    return 0;
  } else if (PyFloat_Check(*other)) {
    *self = PyFloat_FromDouble((double)old->atom);
    Py_INCREF(*other);
    return 0;
  } else if (PyLong_Check(*other)) {
    *self = PyLong_FromUnsignedLong(old->atom);
    Py_INCREF(*other);
    return 0;
  } else if (PyString_Check(*other)) {
    if (!old->name)
      old->name = gdk_atom_name(old->atom);
    if (old->name) {
      *self = PyString_FromString(old->name);
      Py_INCREF(*other);
      return 0;
    }
  }
  return 1;  /* don't know how to convert */
}

static PyObject *
PyGdkAtom_Int(PyGdkAtom_Object *self) {
  return PyInt_FromLong(self->atom);
}

static PyObject *
PyGdkAtom_Long(PyGdkAtom_Object *self) {
  return PyLong_FromUnsignedLong(self->atom);
}

static PyObject *
PyGdkAtom_Float(PyGdkAtom_Object *self) {
  return PyFloat_FromDouble(self->atom);
}

static PyObject *
PyGdkAtom_Oct(PyGdkAtom_Object *self) {
  char buf[100];
  if (self->atom == 0) return PyString_FromString("0");
  g_snprintf(buf, 100, "0%lo", self->atom);
  return PyString_FromString(buf);
}

static PyObject *
PyGdkAtom_Hex(PyGdkAtom_Object *self) {
  char buf[100];
  g_snprintf(buf, 100, "0x%lx", self->atom);
  return PyString_FromString(buf);
}

static PyObject *
PyGdkAtom_Str(PyGdkAtom_Object *self) {
  if (!self->name) self->name = gdk_atom_name(self->atom);
  if (self->name)
    return PyString_FromString(self->name);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyNumberMethods PyGdkAtom_Number = {
  (binaryfunc)0,
  (binaryfunc)0,
  (binaryfunc)0,
  (binaryfunc)0,
  (binaryfunc)0,
  (binaryfunc)0,
  (ternaryfunc)0,
  (unaryfunc)0,
  (unaryfunc)0,
  (unaryfunc)0,
  (inquiry)0,
  (unaryfunc)0,
  (binaryfunc)0,
  (binaryfunc)0,
  (binaryfunc)0,
  (binaryfunc)0,
  (binaryfunc)0,
  (coercion)PyGdkAtom_Coerce,
  (unaryfunc)PyGdkAtom_Int,
  (unaryfunc)PyGdkAtom_Long,
  (unaryfunc)PyGdkAtom_Float,
  (unaryfunc)PyGdkAtom_Oct,
  (unaryfunc)PyGdkAtom_Hex
};

static PyTypeObject PyGdkAtom_Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,
  "GdkAtom",
  sizeof(PyGdkAtom_Object),
  0,
  (destructor)PyGdkAtom_Dealloc,
  (printfunc)0,
  (getattrfunc)0,
  (setattrfunc)0,
  (cmpfunc)PyGdkAtom_Compare,
  (reprfunc)PyGdkAtom_Repr,
  &PyGdkAtom_Number,
  0,
  0,
  (hashfunc)PyGdkAtom_Hash,
  (ternaryfunc)0,
  (reprfunc)PyGdkAtom_Str,
  0L,0L,0L,0L,
  NULL
};

static void
PyGdkCursor_Dealloc(PyGdkCursor_Object *self) {
  gdk_cursor_destroy(self->obj);
  PyMem_DEL(self);
}

static int
PyGdkCursor_Compare(PyGdkCursor_Object *self, PyGdkCursor_Object *v) {
  if (self->obj == v->obj) return 0;
  if (self->obj > v->obj) return -1;
  return 1;
}

static long PyGdkCursor_Hash(PyGdkCursor_Object *self) {
  return (long)self->obj;
}

static PyObject *
PyGdkCursor_Repr(PyGdkCursor_Object *self) {
  char buf[256], *cname = NULL;

  if (self->obj->type == GDK_CURSOR_IS_PIXMAP)
    cname = "*pixmap*";
  else {
    GtkEnumValue *vals = gtk_type_enum_get_values(GTK_TYPE_GDK_CURSOR_TYPE);
    while (vals->value_name != NULL && vals->value != self->obj->type)
      vals++;
    if (vals->value_nick) cname = vals->value_nick;
    else cname = "*unknown*";
  }

  g_snprintf(buf, 256, "<GdkCursor '%s'>", cname);
  return PyString_FromString(buf);
}

static PyObject *PyGdkCursor_GetAttr(PyGdkCursor_Object *self, char *attr) {
  if (!strcmp(attr, "type"))
    return PyInt_FromLong(self->obj->type);
  else if (!strcmp(attr, "name")) {
    GtkEnumValue *vals = gtk_type_enum_get_values(GTK_TYPE_GDK_CURSOR_TYPE);

    while (vals->value_name != NULL && vals->value != self->obj->type)
      vals++;
    if (vals->value_nick) return PyString_FromString(vals->value_nick);
    return PyString_FromString("*unknown*");
  }
  PyErr_SetString(PyExc_AttributeError, attr);
  return NULL;
}

static PyTypeObject PyGdkCursor_Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,
  "GdkCursor",
  sizeof(PyGdkCursor_Object),
  0,
  (destructor)PyGdkCursor_Dealloc,
  (printfunc)0,
  (getattrfunc)PyGdkCursor_GetAttr,
  (setattrfunc)0,
  (cmpfunc)PyGdkCursor_Compare,
  (reprfunc)PyGdkCursor_Repr,
  0,
  0,
  0,
  (hashfunc)PyGdkCursor_Hash,
  (ternaryfunc)0,
  (reprfunc)0,
  0L,0L,0L,0L,
  NULL
};

static void PyGtkCTreeNode_Dealloc(PyGtkCTreeNode_Object *self) {
  PyMem_DEL(self);
}

static int PyGtkCTreeNode_Compare(PyGtkCTreeNode_Object *self,
				  PyGtkCTreeNode_Object *v) {
  if (self->node == v->node) return 0;
  if (self->node > v->node) return -1;
  return 1;
}

static long PyGtkCTreeNode_Hash(PyGtkCTreeNode_Object *self) {
  return (long)self->node;
}

static PyObject *PyGtkCTreeNode_GetAttr(PyGtkCTreeNode_Object *self,
					char *key) {
  if (!strcmp(key, "__members__"))
    return Py_BuildValue("[ssssss]", "children", "expanded", "is_leaf",
			 "level", "parent", "sibling");
  if (!strcmp(key, "parent")) {
    GtkCTreeNode *node = GTK_CTREE_ROW(self->node)->parent;
    if (node)
      return PyGtkCTreeNode_New(node);
    Py_INCREF(Py_None);
    return Py_None;
  } else if (!strcmp(key, "sibling")) {
    GtkCTreeNode *node = GTK_CTREE_ROW(self->node)->sibling;
    if (node)
      return PyGtkCTreeNode_New(node);
    Py_INCREF(Py_None);
    return Py_None;
  } else if (!strcmp(key, "children")) {
    GtkCTreeNode *node = GTK_CTREE_ROW(self->node)->children;
    PyObject *ret = PyList_New(0);
    if (ret == NULL)
      return NULL;
    while (node) {
      PyObject *py_node = PyGtkCTreeNode_New(node);
      if (py_node == NULL) {
	Py_DECREF(ret);
	return NULL;
      }
      PyList_Append(ret, py_node);
      Py_DECREF(py_node);
      node = GTK_CTREE_ROW(node)->sibling;
    }
    return ret;
  } else if (!strcmp(key, "level"))
    return PyInt_FromLong(GTK_CTREE_ROW(self->node)->level);
  else if (!strcmp(key, "is_leaf"))
    return PyInt_FromLong(GTK_CTREE_ROW(self->node)->is_leaf);
  else if (!strcmp(key, "expanded"))
    return PyInt_FromLong(GTK_CTREE_ROW(self->node)->expanded);
  PyErr_SetString(PyExc_AttributeError, key);
  return NULL;
}

static PyTypeObject PyGtkCTreeNode_Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,
  "GtkCTreeNode",
  sizeof(PyGtkCTreeNode_Object),
  0,
  (destructor)PyGtkCTreeNode_Dealloc,
  (printfunc)0,
  (getattrfunc)PyGtkCTreeNode_GetAttr,
  (setattrfunc)0,
  (cmpfunc)PyGtkCTreeNode_Compare,
  (reprfunc)0,
  0,
  0,
  0,
  (hashfunc)PyGtkCTreeNode_Hash,
  (ternaryfunc)0,
  (reprfunc)0,
  0L,0L,0L,0L,
  NULL
};

/* This file contains routines used to convert GtkArg structures and arrays
 * to and from Python objects.  This code is used for the callback marshalling
 * code, signal handling, and GtkObject properties (gtk_object_[gs]et).
 *
 * It is collected here, so I don't accidentally reproduce it elsewhere in
 * pygtk. */

/* destroy notify for PyObject */
static void PyGtk_DestroyNotify(gpointer data) {
  PyGTK_BLOCK_THREADS
  Py_DECREF((PyObject *)data);
  PyGTK_UNBLOCK_THREADS
}

/* this hash table is used to let these functions know about new boxed
 * types (eg. maybe some from GNOME). The functions return TRUE on error */
typedef PyObject *(*fromargfunc)(gpointer boxed);
typedef int (*toargfunc)(gpointer *boxed, PyObject *obj);
typedef struct {
  fromargfunc fromarg;
  toargfunc toarg;
} PyGtk_BoxFuncs;
/* keys are GUINT_TO_POINTER(GtkType) */
static GHashTable *boxed_funcs;

static void PyGtk_RegisterBoxed(GtkType boxed_type,
				 fromargfunc fromarg, toargfunc toarg) {
  PyGtk_BoxFuncs *fs = g_new(PyGtk_BoxFuncs, 1);

  fs->fromarg = fromarg;
  fs->toarg = toarg;
  g_hash_table_insert(boxed_funcs, GUINT_TO_POINTER(boxed_type), fs);
}

#define PyGtk_get_boxed(type) (PyGtk_BoxFuncs *)g_hash_table_lookup( \
  boxed_funcs, GUINT_TO_POINTER(type))

/* create a GtkArg from a PyObject, using the GTK_VALUE_* routines.
 * returns -1 if it couldn't set the argument. */
static int GtkArg_FromPyObject(GtkArg *arg, PyObject *obj) {
  PyObject *tmp;
  switch (GTK_FUNDAMENTAL_TYPE(arg->type)) {
  case GTK_TYPE_NONE:
  case GTK_TYPE_INVALID:
    GTK_VALUE_INT(*arg) = 0;
    break;
  case GTK_TYPE_BOOL:
    if ((tmp = PyNumber_Int(obj)))
      GTK_VALUE_BOOL(*arg) = (PyInt_AsLong(tmp) != 0);
    else {
      PyErr_Clear();
      return -1;
    }
    Py_DECREF(tmp);
    break;
  case GTK_TYPE_CHAR:
  case GTK_TYPE_UCHAR:
    if ((tmp = PyObject_Str(obj)))
      GTK_VALUE_CHAR(*arg) = PyString_AsString(tmp)[0];
    else {
      PyErr_Clear();
      return -1;
    }
    Py_DECREF(tmp);
    break;
  case GTK_TYPE_ENUM:
    if (PyGtkEnum_get_value(arg->type, obj, &(GTK_VALUE_ENUM(*arg))))
      return -1;
    break;
  case GTK_TYPE_FLAGS:
    if (PyGtkFlag_get_value(arg->type, obj, &(GTK_VALUE_FLAGS(*arg))))
      return -1;
    break;
  case GTK_TYPE_INT:
    if ((tmp = PyNumber_Int(obj)))
      GTK_VALUE_INT(*arg) = PyInt_AsLong(tmp);
    else {
      PyErr_Clear();
      return -1;
    }
    Py_DECREF(tmp);
    break;
  case GTK_TYPE_UINT:
    if ((tmp = PyNumber_Int(obj)))
      GTK_VALUE_UINT(*arg) = PyInt_AsLong(tmp);
    else {
      PyErr_Clear();
      return -1;
    }
    Py_DECREF(tmp);
    break;
  case GTK_TYPE_LONG:
    if ((tmp = PyNumber_Int(obj)))
      GTK_VALUE_LONG(*arg) = PyInt_AsLong(tmp);
    else {
      PyErr_Clear();
      return -1;
    }
    Py_DECREF(tmp);
    break;
  case GTK_TYPE_ULONG:
    if ((tmp = PyNumber_Int(obj)))
      GTK_VALUE_ULONG(*arg) = PyInt_AsLong(tmp);
    else {
      PyErr_Clear();
      return -1;
    }
    Py_DECREF(tmp);
    break;
  case GTK_TYPE_FLOAT:
    if ((tmp = PyNumber_Float(obj)))
      GTK_VALUE_FLOAT(*arg) = PyFloat_AsDouble(tmp);
    else {
      PyErr_Clear();
      return -1;
    }
    Py_DECREF(tmp);
    break;
  case GTK_TYPE_DOUBLE:
    if ((tmp = PyNumber_Float(obj)))
      GTK_VALUE_DOUBLE(*arg) = PyFloat_AsDouble(tmp);
    else {
      PyErr_Clear();
      return -1;
    }
    Py_DECREF(tmp);
    break;
  case GTK_TYPE_STRING:
    if ((tmp = PyObject_Str(obj)))
      GTK_VALUE_STRING(*arg) = PyString_AsString(tmp);
    else {
      PyErr_Clear();
      return -1;
    }
    Py_DECREF(tmp);
    break;
  case GTK_TYPE_OBJECT:
    if (PyGtk_Check(obj))
      GTK_VALUE_OBJECT(*arg) = PyGtk_Get(obj);
    else
      return -1;
    break;
  case GTK_TYPE_BOXED:
    if (arg->type == GTK_TYPE_ACCEL_GROUP) {
      if (PyGtkAccelGroup_Check(obj))
	GTK_VALUE_BOXED(*arg) = PyGtkAccelGroup_Get(obj);
      else
	return -1;
    } else if (arg->type == GTK_TYPE_STYLE) {
      if (PyGtkStyle_Check(obj))
	GTK_VALUE_BOXED(*arg) = PyGtkStyle_Get(obj);
      else
	return -1;
    } else if (arg->type == GTK_TYPE_GDK_EVENT) {
      if (PyGdkEvent_Check(obj))
	GTK_VALUE_BOXED(*arg) = PyGdkEvent_Get(obj);
      else
	return -1;
    } else if (arg->type == GTK_TYPE_GDK_FONT) {
      if (PyGdkFont_Check(obj))
	GTK_VALUE_BOXED(*arg) = PyGdkFont_Get(obj);
      else
	return -1;
    } else if (arg->type == GTK_TYPE_GDK_COLOR) {
      if (PyGdkColor_Check(obj))
	GTK_VALUE_BOXED(*arg) = PyGdkColor_Get(obj);
      else if (obj == Py_None)
	GTK_VALUE_BOXED(*arg) = NULL;
      else
	return -1;
    } else if (arg->type == GTK_TYPE_GDK_WINDOW) {
      if (PyGdkWindow_Check(obj))
	GTK_VALUE_BOXED(*arg) = PyGdkWindow_Get(obj);
      else if (obj == Py_None)
	GTK_VALUE_BOXED(*arg) = NULL;
      else
	return -1;
    } else if (arg->type == GTK_TYPE_GDK_COLORMAP) {
      if (PyGdkColormap_Check(obj))
	GTK_VALUE_BOXED(*arg) = PyGdkColormap_Get(obj);
      else if (obj == Py_None)
	GTK_VALUE_BOXED(*arg) = NULL;
      else
	return -1;
    } else if (arg->type == GTK_TYPE_GDK_DRAG_CONTEXT) {
      if (PyGdkDragContext_Check(obj))
	GTK_VALUE_BOXED(*arg) = PyGdkDragContext_Get(obj);
      else
	return -1;
    } else if (arg->type == GTK_TYPE_SELECTION_DATA) {
      if (PyGtkSelectionData_Check(obj))
	GTK_VALUE_BOXED(*arg) = PyGtkSelectionData_Get(obj);
      else
	return -1;
    } else if (arg->type == GTK_TYPE_CTREE_NODE) {
      if (obj == Py_None)
	GTK_VALUE_BOXED(*arg) = NULL;
      else if (PyGtkSelectionData_Check(obj))
	GTK_VALUE_BOXED(*arg) = PyGtkSelectionData_Get(obj);
      else
	return -1;
    } else {
      PyGtk_BoxFuncs *fs= PyGtk_get_boxed(arg->type);
      if (fs && fs->toarg) {
	if (fs->toarg(&(GTK_VALUE_BOXED(*arg)), obj))
	  return -1;
      } else if (PyCObject_Check(obj))
	GTK_VALUE_BOXED(*arg) = PyCObject_AsVoidPtr(obj);
      else
	return -1;
    }
    break;
  case GTK_TYPE_POINTER:
    if (PyCObject_Check(obj))
      GTK_VALUE_BOXED(*arg) = PyCObject_AsVoidPtr(obj);
    else
      return -1;
    break;
  case GTK_TYPE_FOREIGN:
    Py_INCREF(obj);
    GTK_VALUE_FOREIGN(*arg).data = obj;
    GTK_VALUE_FOREIGN(*arg).notify = PyGtk_DestroyNotify;
    break;
  case GTK_TYPE_SIGNAL:
    if (PyCallable_Check(obj)) {
      Py_INCREF(obj);
      GTK_VALUE_SIGNAL(*arg).f = NULL;
      GTK_VALUE_SIGNAL(*arg).d = obj;
    } else
      return -1;
    break;
  case GTK_TYPE_CALLBACK:
    if (PyCallable_Check(obj)) {
      Py_INCREF(obj);
      GTK_VALUE_CALLBACK(*arg).marshal =
	(GtkCallbackMarshal)PyGtk_CallbackMarshal;
      GTK_VALUE_CALLBACK(*arg).data = obj;
      GTK_VALUE_CALLBACK(*arg).notify = PyGtk_DestroyNotify;
    } else
      return -1;
    break;
  case GTK_TYPE_ARGS:
  case GTK_TYPE_C_CALLBACK:
    fprintf(stderr, "unsupported type");
    g_assert_not_reached();
    return -1;
  }
  return 0;
}

/* convert a GtkArg structure to a PyObject, using the GTK_VALUE_* routines.
 * conversion is always possible. */
static PyObject *GtkArg_AsPyObject(GtkArg *arg) {
  switch (GTK_FUNDAMENTAL_TYPE(arg->type)) {
  case GTK_TYPE_INVALID:
  case GTK_TYPE_NONE:
    Py_INCREF(Py_None);
    return Py_None;
  case GTK_TYPE_CHAR:
  case GTK_TYPE_UCHAR:
    return PyString_FromStringAndSize(&GTK_VALUE_CHAR(*arg), 1);
  case GTK_TYPE_BOOL:
    return PyInt_FromLong(GTK_VALUE_BOOL(*arg));
  case GTK_TYPE_ENUM:
  case GTK_TYPE_FLAGS:
  case GTK_TYPE_INT:
    return PyInt_FromLong(GTK_VALUE_INT(*arg));
  case GTK_TYPE_UINT:
    return PyInt_FromLong(GTK_VALUE_UINT(*arg));
  case GTK_TYPE_LONG:
    return PyInt_FromLong(GTK_VALUE_LONG(*arg));
  case GTK_TYPE_ULONG:
    return PyInt_FromLong(GTK_VALUE_ULONG(*arg));
  case GTK_TYPE_FLOAT:
    return PyFloat_FromDouble(GTK_VALUE_FLOAT(*arg));
  case GTK_TYPE_DOUBLE:
    return PyFloat_FromDouble(GTK_VALUE_DOUBLE(*arg));
  case GTK_TYPE_STRING:
    if (GTK_VALUE_STRING(*arg) != NULL)
      return PyString_FromString(GTK_VALUE_STRING(*arg));
    else {
      Py_INCREF(Py_None);
      return Py_None;
    }
  case GTK_TYPE_ARGS:
    return GtkArgs_AsTuple(GTK_VALUE_ARGS(*arg).n_args,
			   GTK_VALUE_ARGS(*arg).args);
  case GTK_TYPE_OBJECT:
    if (GTK_VALUE_OBJECT(*arg) != NULL)
      return PyGtk_New(GTK_VALUE_OBJECT(*arg));
    else {
      Py_INCREF(Py_None);
      return Py_None;
    }
  case GTK_TYPE_POINTER:
    return PyCObject_FromVoidPtr(GTK_VALUE_POINTER(*arg), NULL);
  case GTK_TYPE_BOXED:
    if (arg->type == GTK_TYPE_ACCEL_GROUP)
      return PyGtkAccelGroup_New(GTK_VALUE_BOXED(*arg));
    else if (arg->type == GTK_TYPE_STYLE)
      return PyGtkStyle_New(GTK_VALUE_BOXED(*arg));
    else if (arg->type == GTK_TYPE_GDK_EVENT)
      return PyGdkEvent_New(GTK_VALUE_BOXED(*arg));
    else if (arg->type == GTK_TYPE_GDK_FONT)
      return PyGdkFont_New(GTK_VALUE_BOXED(*arg));
    else if (arg->type == GTK_TYPE_GDK_COLOR)
      return PyGdkColor_New(GTK_VALUE_BOXED(*arg));
    else if (arg->type == GTK_TYPE_GDK_WINDOW)
      return PyGdkWindow_New(GTK_VALUE_BOXED(*arg));
    else if (arg->type == GTK_TYPE_GDK_COLORMAP)
      return PyGdkColormap_New(GTK_VALUE_BOXED(*arg));
    else if (arg->type == GTK_TYPE_GDK_DRAG_CONTEXT)
      return PyGdkDragContext_New(GTK_VALUE_BOXED(*arg));
    else if (arg->type == GTK_TYPE_SELECTION_DATA)
      return PyGtkSelectionData_New(GTK_VALUE_BOXED(*arg));
    else if (arg->type == GTK_TYPE_CTREE_NODE) {
      if (GTK_VALUE_BOXED(*arg))
	return PyGtkCTreeNode_New(GTK_VALUE_BOXED(*arg));
      Py_INCREF(Py_None);
      return Py_None;
    } else {
      PyGtk_BoxFuncs *fs = PyGtk_get_boxed(arg->type);
      if (fs && fs->fromarg)
	return fs->fromarg(GTK_VALUE_BOXED(*arg));
      return PyCObject_FromVoidPtr(GTK_VALUE_BOXED(*arg), NULL);
    }
  case GTK_TYPE_FOREIGN:
    Py_INCREF((PyObject *)GTK_VALUE_FOREIGN(*arg).data);
    return (PyObject *)GTK_VALUE_FOREIGN(*arg).data;
  case GTK_TYPE_CALLBACK:
    Py_INCREF((PyObject *)GTK_VALUE_CALLBACK(*arg).data);
    return (PyObject *)GTK_VALUE_CALLBACK(*arg).data;
  case GTK_TYPE_SIGNAL:
    Py_INCREF((PyObject *)GTK_VALUE_SIGNAL(*arg).d);
    return (PyObject *)GTK_VALUE_SIGNAL(*arg).d;
  default:
    g_assert_not_reached();
    break;
  }
  return NULL;
}

/* set a GtkArg structure's data from a PyObject, using the GTK_RETLOC_*
 * routines.  If it can't make the conversion, set the return to a zero
 * equivalent. */
static void GtkRet_FromPyObject(GtkArg *ret, PyObject *py_ret) {
  PyObject *tmp;
  switch (GTK_FUNDAMENTAL_TYPE(ret->type)) {
  case GTK_TYPE_NONE:
  case GTK_TYPE_INVALID:
    break;
  case GTK_TYPE_BOOL:
    if ((tmp = PyNumber_Int(py_ret))) {
      *GTK_RETLOC_BOOL(*ret) = (PyInt_AsLong(tmp) != 0);
      Py_DECREF(tmp);
    } else {
      PyErr_Clear();
      *GTK_RETLOC_BOOL(*ret) = FALSE;
    }
    break;
  case GTK_TYPE_CHAR:
    if ((tmp = PyObject_Str(py_ret))) {
      *GTK_RETLOC_CHAR(*ret) = PyString_AsString(tmp)[0];
      Py_DECREF(tmp);
    } else {
      PyErr_Clear();
      *GTK_RETLOC_CHAR(*ret) = '\0';
    }
    break;
  case GTK_TYPE_ENUM:
    if (PyGtkEnum_get_value(ret->type, py_ret, GTK_RETLOC_ENUM(*ret))) {
      PyErr_Clear();
      *GTK_RETLOC_ENUM(*ret) = 0;
    }
    break;
  case GTK_TYPE_FLAGS:
    if (PyGtkFlag_get_value(ret->type, py_ret, GTK_RETLOC_FLAGS(*ret))) {
      PyErr_Clear();
      *GTK_RETLOC_FLAGS(*ret) = 0;
    }
    break;
  case GTK_TYPE_INT:
    if ((tmp = PyNumber_Int(py_ret))) {
      *GTK_RETLOC_INT(*ret) = PyInt_AsLong(tmp);
      Py_DECREF(tmp);
    } else {
      PyErr_Clear();
      *GTK_RETLOC_INT(*ret) = 0;
    }
    break;
  case GTK_TYPE_UINT:
    if ((tmp = PyNumber_Int(py_ret))) {
      *GTK_RETLOC_UINT(*ret) = PyInt_AsLong(tmp);
      Py_DECREF(tmp);
    } else {
      PyErr_Clear();
      *GTK_RETLOC_UINT(*ret) = 0;
    }
    break;
  case GTK_TYPE_LONG:
    if ((tmp = PyNumber_Int(py_ret))) {
      *GTK_RETLOC_LONG(*ret) = PyInt_AsLong(tmp);
      Py_DECREF(tmp);
    } else {
      PyErr_Clear();
      *GTK_RETLOC_LONG(*ret) = 0;
    }
    break;
  case GTK_TYPE_ULONG:
    if ((tmp = PyNumber_Int(py_ret))) {
      *GTK_RETLOC_ULONG(*ret) = PyInt_AsLong(tmp);
      Py_DECREF(tmp);
    } else {
      PyErr_Clear();
      *GTK_RETLOC_ULONG(*ret) = 0;
    }
    break;
  case GTK_TYPE_FLOAT:
    if ((tmp = PyNumber_Float(py_ret))) {
      *GTK_RETLOC_FLOAT(*ret) = PyFloat_AsDouble(tmp);
      Py_DECREF(tmp);
    } else {
      PyErr_Clear();
      *GTK_RETLOC_FLOAT(*ret) = 0;
    }
    break;
  case GTK_TYPE_DOUBLE:
    if ((tmp = PyNumber_Float(py_ret))) {
      *GTK_RETLOC_DOUBLE(*ret) = PyFloat_AsDouble(tmp);
      Py_DECREF(tmp);
    } else {
      PyErr_Clear();
      *GTK_RETLOC_DOUBLE(*ret) = 0;
    }
    break;
  case GTK_TYPE_STRING:
    if ((tmp = PyObject_Str(py_ret))) {
      *GTK_RETLOC_STRING(*ret) = g_strdup(PyString_AsString(py_ret));
      Py_DECREF(tmp);
    } else {
      PyErr_Clear();
      *GTK_RETLOC_STRING(*ret) = NULL;
    }
    break;
  case GTK_TYPE_OBJECT:
    if (PyGtk_Check(py_ret))
      *GTK_RETLOC_OBJECT(*ret) = PyGtk_Get(py_ret);
    else
      *GTK_RETLOC_OBJECT(*ret) = NULL;
    break;
  case GTK_TYPE_BOXED:
    if (ret->type == GTK_TYPE_ACCEL_GROUP) {
      if (PyGtkAccelGroup_Check(py_ret))
	*GTK_RETLOC_BOXED(*ret) = PyGtkAccelGroup_Get(py_ret);
      else
	*GTK_RETLOC_BOXED(*ret) = NULL;
    } else if (ret->type == GTK_TYPE_STYLE) {
      if (PyGtkStyle_Check(py_ret))
	*GTK_RETLOC_BOXED(*ret) = PyGtkStyle_Get(py_ret);
      else
	*GTK_RETLOC_BOXED(*ret) = NULL;
    } else if (ret->type == GTK_TYPE_GDK_EVENT) {
      if (PyGdkEvent_Check(py_ret))
	*GTK_RETLOC_BOXED(*ret) = PyGdkEvent_Get(py_ret);
      else
	*GTK_RETLOC_BOXED(*ret) = NULL;
    } else if (ret->type == GTK_TYPE_GDK_FONT) {
      if (PyGdkFont_Check(py_ret))
	*GTK_RETLOC_BOXED(*ret) = PyGdkFont_Get(py_ret);
      else
	*GTK_RETLOC_BOXED(*ret) = NULL;
    } else if (ret->type == GTK_TYPE_GDK_COLOR) {
      if (PyGdkColor_Check(py_ret))
	*GTK_RETLOC_BOXED(*ret) = PyGdkColor_Get(py_ret);
      else
	*GTK_RETLOC_BOXED(*ret) = NULL;
    } else if (ret->type == GTK_TYPE_GDK_WINDOW) {
      if (PyGdkWindow_Check(py_ret))
	*GTK_RETLOC_BOXED(*ret) = PyGdkWindow_Get(py_ret);
      else
	*GTK_RETLOC_BOXED(*ret) = NULL;
    } else if (ret->type == GTK_TYPE_GDK_COLORMAP) {
      if (PyGdkColormap_Check(py_ret))
	*GTK_RETLOC_BOXED(*ret) = PyGdkColormap_Get(py_ret);
      else
	*GTK_RETLOC_BOXED(*ret) = NULL;
    } else if (ret->type == GTK_TYPE_GDK_DRAG_CONTEXT) {
      if (PyGdkDragContext_Check(py_ret))
	*GTK_RETLOC_BOXED(*ret) = PyGdkDragContext_Get(py_ret);
      else
	*GTK_RETLOC_BOXED(*ret) = NULL;
    } else if (ret->type == GTK_TYPE_SELECTION_DATA) {
      if (PyGtkSelectionData_Check(py_ret))
	*GTK_RETLOC_BOXED(*ret) = PyGtkSelectionData_Get(py_ret);
      else
	*GTK_RETLOC_BOXED(*ret) = NULL;
    } else if (ret->type == GTK_TYPE_CTREE_NODE) {
      if (PyGtkSelectionData_Check(py_ret))
	*GTK_RETLOC_BOXED(*ret) = PyGtkCTreeNode_Get(py_ret);
      else
	*GTK_RETLOC_BOXED(*ret) = NULL;
    } else {
      PyGtk_BoxFuncs *fs = PyGtk_get_boxed(ret->type);
      if (fs && fs->toarg) {
	if (fs->toarg(GTK_RETLOC_BOXED(*ret), py_ret))
	  *GTK_RETLOC_BOXED(*ret) = NULL;
      } else
	if (PyCObject_Check(py_ret))
	  *GTK_RETLOC_BOXED(*ret) = PyCObject_AsVoidPtr(py_ret);
	else
	  *GTK_RETLOC_BOXED(*ret) = NULL;
    }
    break;
  case GTK_TYPE_POINTER:
    if (PyCObject_Check(py_ret))
      *GTK_RETLOC_POINTER(*ret) = PyCObject_AsVoidPtr(py_ret);
    else
      *GTK_RETLOC_POINTER(*ret) = NULL;
    break;
  default:
    g_assert_not_reached();
    break;
  }
}

/* convert a GtkArg structure to a PyObject, using the GTK_RETLOC_* routines.
 * conversion is always possible. */
static PyObject *GtkRet_AsPyObject(GtkArg *arg) {
  switch (GTK_FUNDAMENTAL_TYPE(arg->type)) {
  case GTK_TYPE_INVALID:
  case GTK_TYPE_NONE:
    Py_INCREF(Py_None);
    return Py_None;
  case GTK_TYPE_CHAR:
    return PyString_FromStringAndSize(GTK_RETLOC_CHAR(*arg), 1);
  case GTK_TYPE_BOOL:
    return PyInt_FromLong(*GTK_RETLOC_BOOL(*arg));
  case GTK_TYPE_ENUM:
  case GTK_TYPE_FLAGS:
  case GTK_TYPE_INT:
    return PyInt_FromLong(*GTK_RETLOC_INT(*arg));
  case GTK_TYPE_UINT:
    return PyInt_FromLong(*GTK_RETLOC_UINT(*arg));
  case GTK_TYPE_LONG:
    return PyInt_FromLong(*GTK_RETLOC_LONG(*arg));
  case GTK_TYPE_ULONG:
    return PyInt_FromLong(*GTK_RETLOC_ULONG(*arg));
  case GTK_TYPE_FLOAT:
    return PyFloat_FromDouble(*GTK_RETLOC_FLOAT(*arg));
  case GTK_TYPE_DOUBLE:
    return PyFloat_FromDouble(*GTK_RETLOC_DOUBLE(*arg));
  case GTK_TYPE_STRING:
    if (*GTK_RETLOC_STRING(*arg) != NULL)
      return PyString_FromString(*GTK_RETLOC_STRING(*arg));
    else {
      Py_INCREF(Py_None);
      return Py_None;
    }
  case GTK_TYPE_ARGS:
    break;
  case GTK_TYPE_OBJECT:
    return PyGtk_New(*GTK_RETLOC_OBJECT(*arg));
  case GTK_TYPE_POINTER:
    return PyCObject_FromVoidPtr(*GTK_RETLOC_POINTER(*arg), NULL);
  case GTK_TYPE_BOXED:
    if (arg->type == GTK_TYPE_ACCEL_GROUP)
      return PyGtkAccelGroup_New(*GTK_RETLOC_BOXED(*arg));
    else if (arg->type == GTK_TYPE_STYLE)
      return PyGtkStyle_New(*GTK_RETLOC_BOXED(*arg));
    else if (arg->type == GTK_TYPE_GDK_EVENT)
      return PyGdkEvent_New(*GTK_RETLOC_BOXED(*arg));
    else if (arg->type == GTK_TYPE_GDK_FONT)
      return PyGdkFont_New(*GTK_RETLOC_BOXED(*arg));
    else if (arg->type == GTK_TYPE_GDK_COLOR)
      return PyGdkColor_New(*GTK_RETLOC_BOXED(*arg));
    else if (arg->type == GTK_TYPE_GDK_WINDOW)
      return PyGdkWindow_New(*GTK_RETLOC_BOXED(*arg));
    else if (arg->type == GTK_TYPE_GDK_COLORMAP)
      return PyGdkColormap_New(*GTK_RETLOC_BOXED(*arg));
    else if (arg->type == GTK_TYPE_GDK_DRAG_CONTEXT)
      return PyGdkDragContext_New(*GTK_RETLOC_BOXED(*arg));
    else if (arg->type == GTK_TYPE_SELECTION_DATA)
      return PyGtkSelectionData_New(*GTK_RETLOC_BOXED(*arg));
    else if (arg->type == GTK_TYPE_CTREE_NODE) {
      if (*GTK_RETLOC_BOXED(*arg))
	return PyGtkCTreeNode_New(*GTK_RETLOC_BOXED(*arg));
      Py_INCREF(Py_None);
      return Py_None;
    } else {
      PyGtk_BoxFuncs *fs = PyGtk_get_boxed(arg->type);
      if (fs && fs->fromarg)
	return fs->fromarg(*GTK_RETLOC_BOXED(*arg));
      return PyCObject_FromVoidPtr(*GTK_RETLOC_BOXED(*arg), NULL);
    }
  default:
    g_assert_not_reached();
    break;
  }
  return NULL;
}

/* convert the GtkArg array ARGS to a python tuple */
static PyObject *GtkArgs_AsTuple(int nparams, GtkArg *args) {
  PyObject *tuple, *item;
  int i;

  if ((tuple = PyTuple_New(nparams)) == NULL)
    return NULL;
  for (i = 0; i < nparams; i++) {
    item = GtkArg_AsPyObject(&args[i]);
    if (item == NULL) {
      Py_INCREF(Py_None);
      item = Py_None;
    }
    PyTuple_SetItem(tuple, i, item);
  }
  return tuple;
}

/* converts a Python sequence to a GtkArg array.  Returns -1 if the sequence
 * doesn't match the specification in ARGS */
static int GtkArgs_FromSequence(GtkArg *args, int nparams, PyObject *seq) {
  PyObject *item;
  int i;
  if (!PySequence_Check(seq))
    return -1;
  for (i = 0; i < nparams; i++) {
    item = PySequence_GetItem(seq, i);
    Py_DECREF(item);
    if (GtkArg_FromPyObject(&args[i], item)) {
      gchar buf[512];
      if (args[i].name == NULL)
	g_snprintf(buf, 511, "argument %d: expected %s, %s found", i+1,
		   gtk_type_name(args[i].type), item->ob_type->tp_name);
      else
	g_snprintf(buf, 511, "argument %s: expected %s, %s found",
		   args[i].name, gtk_type_name(args[i].type),
		   item->ob_type->tp_name);
      PyErr_SetString(PyExc_TypeError, buf);
      return -1;
    }
  }
  return 0;
}

/* generic callback marshal */
static void PyGtk_CallbackMarshal(GtkObject *o, gpointer data, guint nargs,
				  GtkArg *args) {
    PyObject *tuple = data, *func, *extra=NULL, *obj=NULL, *ret, *a, *params;

    PyGTK_BLOCK_THREADS
    a = GtkArgs_AsTuple(nargs, args);
    if (a == NULL) {
	PyErr_Clear();
	fprintf(stderr, "can't decode params -- callback not run\n");
	PyGTK_UNBLOCK_THREADS
	return;
    }

    if (PyTuple_Check(tuple)) {
	func = PyTuple_GetItem(tuple, 0);
	extra = PyTuple_GetItem(tuple, 1);
	if (PyTuple_Size(tuple) > 2) {
	    obj = PyTuple_GetItem(tuple, 2);
	    Py_INCREF(obj);
	}
    } else
	func = tuple;
    if (!obj && o != NULL)
	obj = PyGtk_New(o);

    if (obj) {
	tuple = PyTuple_New(1);
	PyTuple_SetItem(tuple, 0, obj);
	params = PySequence_Concat(tuple, a);
	Py_DECREF(tuple); Py_DECREF(a);
    } else
	params = a;

    if (extra) {
	tuple = params;
	params = PySequence_Concat(tuple, extra);
	Py_DECREF(tuple);
    }
    ret = PyObject_CallObject(func, params);
    Py_DECREF(params);
    if (ret == NULL) {
	if (PyGtk_FatalExceptions)
	    gtk_main_quit();
	else {
	    PyErr_Print();
	    PyErr_Clear();
	}
	PyGTK_UNBLOCK_THREADS
	return;
    }
    GtkRet_FromPyObject(&args[nargs], ret);
    Py_DECREF(ret);
    PyGTK_UNBLOCK_THREADS
}



static
void PyGtk_SignalMarshal(GtkObject *object, /*gpointer*/ PyObject *func,
                       int nparams, GtkArg *args, GtkType *arg_types,
                                                 GtkType return_type) {
    PyObject *arg_list, *params, *ret;

    PyGTK_BLOCK_THREADS
    ret = PyTuple_New(1);
    PyTuple_SetItem(ret, 0, PyGtk_New(object));
    arg_list = GtkArgs_AsTuple(nparams, args);
    params = PySequence_Concat(ret, arg_list);
    Py_DECREF(ret);
    Py_DECREF(arg_list);
    if (PyTuple_Check(func)) { /* extra arguments given with func */
      arg_list = PyTuple_GetItem(func, 1);
      func = PyTuple_GetItem(func, 0);
      if (PyTuple_Check(arg_list)) {
	ret = params;
	params = PySequence_Concat(ret, arg_list);
	Py_DECREF(ret);
      }
    }
    ret = PyObject_CallObject(func, params);
    Py_DECREF(params);

    if (ret == NULL) {
        if (PyGtk_FatalExceptions)
	    gtk_main_quit();
	else {
	    PyErr_Print();
	    PyErr_Clear();
	}
	PyGTK_UNBLOCK_THREADS
	return;
    }
    GtkRet_FromPyObject(&args[nparams], ret);
    Py_DECREF(ret);
    PyGTK_UNBLOCK_THREADS
}

static
void PyGtk_SignalDestroy(/*gpointer*/ PyObject *func) {
    PyGTK_BLOCK_THREADS
    Py_DECREF(func);
    PyGTK_UNBLOCK_THREADS
}

/* simple callback handler -- this one actually looks at the return type */
/* used for timeout and idle functions */
static void PyGtk_HandlerMarshal(gpointer a, PyObject *func, int nargs,
                                                          GtkArg *args) {
    PyObject *ret;

    PyGTK_BLOCK_THREADS

    if (PyTuple_Check(func))
	ret = PyObject_CallObject(PyTuple_GetItem(func, 0),
				  PyTuple_GetItem(func, 1));
    else
	ret = PyObject_CallObject(func, NULL);
    if (ret == NULL) {
	if (PyGtk_FatalExceptions)
	    gtk_main_quit();
	else {
	    PyErr_Print();
	    PyErr_Clear();
	}
	*GTK_RETLOC_BOOL(args[0]) = FALSE;
	PyGTK_UNBLOCK_THREADS
	    return;
    }
    if (ret == Py_None || (PyInt_Check(ret) && PyInt_AsLong(ret) == 0))
	*GTK_RETLOC_BOOL(args[0]) = FALSE;
    else
	*GTK_RETLOC_BOOL(args[0]) = TRUE;
    Py_DECREF(ret);
    PyGTK_UNBLOCK_THREADS
}

/* callback for input handlers */
static void PyGtk_InputMarshal(gpointer a, PyObject *func, int nargs,
			                                GtkArg *args) {
  PyObject *tuple, *ret;

  PyGTK_BLOCK_THREADS
  tuple = Py_BuildValue("(ii)", GTK_VALUE_INT(args[0]),
			GTK_VALUE_FLAGS(args[1]));
  ret = PyObject_CallObject(func, tuple);
  Py_DECREF(tuple);
  if (ret == NULL) {
    if (PyGtk_FatalExceptions)
      gtk_main_quit();
    else {
      PyErr_Print();
      PyErr_Clear();
    }
  } else
    Py_DECREF(ret);
  PyGTK_UNBLOCK_THREADS
}

static GtkArg *PyDict_AsGtkArgs(PyObject *dict, GtkType type, gint *nargs) {
    PyObject *key, *item;
    gint i = 0, pos;
    GtkArg *arg;
    GtkArgInfo *info;
    gchar *err, buf[128];

    gtk_type_class(type); /* initialise the class structure (and setup args) */
    *nargs = PyDict_Size(dict);
    arg = g_new(GtkArg, *nargs);
    pos = 0;
    while (PyDict_Next(dict, &i, &key, &item)) {
        if (!PyString_Check(key)) {
	    PyErr_SetString(PyExc_TypeError,"dictionary keys must be strings");
	    g_free(arg);
	    return NULL;
        }
	arg[pos].name = PyString_AsString(key);
	err = gtk_object_arg_get_info(type, arg[pos].name, &info);
	if (info == NULL) {
	    PyErr_SetString(PyExc_TypeError, err);
	    g_free(err);
	    g_free(arg);
	    return NULL;
	}
	arg[pos].type = info->type;
	arg[pos].name = info->name;
	if (GtkArg_FromPyObject(&(arg[pos]), item)) {
	    g_snprintf(buf, 255, "arg %s: expected type %s, found %s",
		       arg[pos].name, gtk_type_name(arg[pos].type),
		       item->ob_type->tp_name);
	    PyErr_SetString(PyExc_TypeError, buf);
	    g_free(arg);
	    return NULL;
	}
	pos++;
    }
    return arg;
}

#if 0 /* not used */
static void PyGtk_object_set(GtkObject *o, PyObject *dict) {
    int i = 0;
    PyObject *key, *item;
    char *name, *err;
    GtkType obtype;
    GtkArg arg;
    GtkArgInfo *info;
 
    obtype = GTK_OBJECT_TYPE(o);

    while (PyDict_Next(dict, &i, &key, &item)) {
        if (!PyString_Check(key)) {
            fprintf(stderr, "Key not a string\n");
            continue;
        }
        name = PyString_AsString(key);
	err = gtk_object_arg_get_info(obtype, name, &info);
        if (info == NULL) {
            fprintf(stderr, err);
	    g_free(err);
            continue;
        }

        arg.type = info->type;
        arg.name = info->name;
	if (GtkArg_FromPyObject(&arg, item)) {
	  fprintf(stderr, "%s: expected %s, %s found\n", name,
		  gtk_type_name(arg.type), item->ob_type->tp_name);
	  continue;
	}
        gtk_object_setv(o, 1, &arg);
    }
}
#endif /* not used */

static GtkArg *PyDict_AsContainerArgs(PyObject *dict,GtkType type,gint *nargs){
    PyObject *key, *item;
    gint i = 0, pos;
    GtkArg *arg;
    GtkArgInfo *info;
    gchar *err, buf[128];

    *nargs = PyDict_Size(dict);
    arg = g_new(GtkArg, *nargs);
    pos = 0;
    while (PyDict_Next(dict, &i, &key, &item)) {
        if (!PyString_Check(key)) {
	    PyErr_SetString(PyExc_TypeError,"dictionary keys must be strings");
	    g_free(arg);
	    return NULL;
        }
	arg[pos].name = PyString_AsString(key);
	err = gtk_container_child_arg_get_info(type, arg[pos].name, &info);
	if (info == NULL) {
	    PyErr_SetString(PyExc_TypeError, err);
	    g_free(err);
	    g_free(arg);
	    return NULL;
	}
	arg[pos].type = info->type;
	arg[pos].name = info->name;
	if (GtkArg_FromPyObject(&(arg[pos]), item)) {
	    g_snprintf(buf, 255, "arg %s: expected type %s, found %s",
		       arg[pos].name, gtk_type_name(arg[pos].type),
		       item->ob_type->tp_name);
	    PyErr_SetString(PyExc_TypeError, buf);
	    g_free(arg);
	    return NULL;
	}
	pos++;
    }
   return arg;
}

static PyObject *_wrap_gtk_signal_connect(PyObject *self, PyObject *args) {
    PyGtk_Object *obj;
    char *name;
    PyObject *func, *extra = NULL, *data;
    int signum;

    if (!PyArg_ParseTuple(args, "O!sO|O!:gtk_signal_connect", &PyGtk_Type,
			  &obj, &name, &func, &PyTuple_Type, &extra))
        return NULL;
    if (!PyCallable_Check(func)) {
        PyErr_SetString(PyExc_TypeError, "third argument must be callable");
        return NULL;
    }

    if (extra)
      Py_INCREF(extra);
    else
      extra = PyTuple_New(0);

    if (extra == NULL)
      return NULL;
    data = Py_BuildValue("(ON)", func, extra);
    signum = gtk_signal_connect_full(PyGtk_Get(obj), name, NULL,
				     (GtkCallbackMarshal)PyGtk_CallbackMarshal,
				     data, PyGtk_DestroyNotify, FALSE, FALSE);
    return PyInt_FromLong(signum);
}

static PyObject *_wrap_gtk_signal_connect_after(PyObject *self, PyObject *args) {
    PyGtk_Object *obj;
    char *name;
    PyObject *func, *extra = NULL, *data;
    int signum;

    if (!PyArg_ParseTuple(args, "O!sO|O!:gtk_signal_connect_after",
                    &PyGtk_Type, &obj, &name, &func, &PyTuple_Type, &extra))
        return NULL;
    if (!PyCallable_Check(func)) {
        PyErr_SetString(PyExc_TypeError, "third argument must be callable");
        return NULL;
    }
    if (extra)
        Py_INCREF(extra);
    else
        extra = PyTuple_New(0);

    if (extra == NULL)
      return NULL;
    data = Py_BuildValue("(ON)", func, extra);

    signum = gtk_signal_connect_full(PyGtk_Get(obj), name, NULL,
				     (GtkCallbackMarshal)PyGtk_CallbackMarshal,
				     data, PyGtk_DestroyNotify, FALSE, TRUE);
    return PyInt_FromLong(signum);
}

static PyObject *_wrap_gtk_signal_connect_object(PyObject *self, PyObject *args) {
    PyGtk_Object *obj;
    char *name;
    PyObject *func, *extra = NULL, *other, *data;
    int signum;

    if (!PyArg_ParseTuple(args, "O!sOO!|O!:gtk_signal_connect_object",
			  &PyGtk_Type, &obj, &name, &func, &PyGtk_Type, &other,
			  &PyTuple_Type, &extra))
        return NULL;
    if (!PyCallable_Check(func)) {
        PyErr_SetString(PyExc_TypeError, "third argument must be callable");
        return NULL;
    }
    
    if (extra)
	Py_INCREF(extra);
    else
	extra = PyTuple_New(0);
    
    if (extra == NULL)
      return NULL;
    data = Py_BuildValue("(ONO)", func, extra, other);
    if (data == NULL)
      return NULL;

    signum = gtk_signal_connect_full(PyGtk_Get(obj), name, NULL,
				     (GtkCallbackMarshal)PyGtk_CallbackMarshal,
				     data, PyGtk_DestroyNotify, FALSE, FALSE);
    return PyInt_FromLong(signum);
}

static PyObject *_wrap_gtk_signal_connect_object_after(PyObject *self, PyObject *args) {
    PyGtk_Object *obj;
    char *name;
    PyObject *func, *extra = NULL, *other, *data;
    int signum;

    if (!PyArg_ParseTuple(args, "O!sOO!|O!:gtk_signal_connect_object_after",
			  &PyGtk_Type, &obj, &name, &func, &PyGtk_Type, &other,
			  &PyTuple_Type, &extra))
        return NULL;
    if (!PyCallable_Check(func)) {
        PyErr_SetString(PyExc_TypeError, "third argument must be callable");
        return NULL;
    }

    if (extra)
	Py_INCREF(extra);
    else
	extra = PyTuple_New(0);

    data = Py_BuildValue("(ONO)", func, extra, other);

    signum = gtk_signal_connect_full(PyGtk_Get(obj), name, NULL,
				     (GtkCallbackMarshal)PyGtk_CallbackMarshal,
				     data, PyGtk_DestroyNotify, FALSE, TRUE);
    return PyInt_FromLong(signum);
}

static PyObject *_wrap_gtk_signal_disconnect_by_data(PyObject *self, PyObject *args) {
  PyObject *obj;
  PyObject *data;
  
  self = self;
  if(!PyArg_ParseTuple(args,"O!O:gtk_signal_disconnect_by_data",
		       &PyGtk_Type, &obj, &data)) 
    return NULL;
  gtk_signal_disconnect_by_data(PyGtk_Get(obj), data);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *_wrap_gtk_signal_handler_block_by_data(PyObject *self, PyObject *args) {
    PyObject *obj;
    PyObject *data;

    if(!PyArg_ParseTuple(args,"O!O:gtk_signal_handler_block_by_data",
			 &PyGtk_Type, &obj, &data)) 
        return NULL;
    gtk_signal_handler_block_by_data(PyGtk_Get(obj), data);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_signal_handler_unblock_by_data(PyObject *self, PyObject *args) {
    PyObject *obj;
    PyObject *data;

    if(!PyArg_ParseTuple(args,"O!O:gtk_signal_handler_unblock_by_data",
			 &PyGtk_Type, &obj, &data)) 
        return NULL;
    gtk_signal_handler_unblock_by_data(PyGtk_Get(obj), data);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_signal_emitv_by_name(PyObject *self, PyObject *args) {
  guint signal_id, i, nparams;
  GtkSignalQuery *query;
  GtkArg *params;
  PyObject *obj, *py_params;
  gchar *name, buf[sizeof(GtkArg)]; /* large enough to hold any return value */

  if (!PyArg_ParseTuple(args, "O!sO:gtk_signal_emitv_by_name", &PyGtk_Type,
			&obj, &name, &py_params))
    return NULL;
  if (!PySequence_Check(py_params)) {
    PyErr_SetString(PyExc_TypeError, "third argument not a sequence");
    return NULL;
  }

  signal_id = gtk_signal_lookup(name, GTK_OBJECT_TYPE(PyGtk_Get(obj)));
  if (signal_id <= 0) {
    PyErr_SetString(PyExc_KeyError, "can't find signal in classes ancestry");
    return NULL;
  }
  query = gtk_signal_query(signal_id);
  params = g_new(GtkArg, query->nparams + 1);
  nparams = query->nparams;
  for (i = 0; i < nparams; i++) {
    params[i].type = query->params[i];
    params[i].name = NULL;
  }
  params[i].type = query->return_val;
  params[i].name = NULL;
  params[i].d.pointer_data = buf; /* buffer to hold return value */
  g_free(query);
  if (GtkArgs_FromSequence(params, nparams, py_params)) {
    g_free(params);
    return NULL;
  }
  gtk_signal_emitv(PyGtk_Get(obj), signal_id, params);
  obj = GtkRet_AsPyObject(&params[nparams]);
  g_free(params);
  if (obj == NULL) { Py_INCREF(Py_None); obj = Py_None; }
  return obj;
}

static PyObject * _wrap_gtk_init(PyObject *self, PyObject *args) {
    PyObject *av;
    int argc, i;
    char **argv;

    /* ensure that this procedure only gets run once */
    static int run_already = 0;
    if (run_already) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    run_already = 1;

    if (!PyArg_ParseTuple(args, ":gtk_init"))
        return NULL;

    av = PySys_GetObject("argv");
    argc = PyList_Size(av);
    argv = malloc(argc * sizeof(char *));
    for (i = 0; i < argc; i++)
        argv[i] = strdup(PyString_AsString(PyList_GetItem(av, i)));

    if (!gtk_init_check(&argc, &argv)) {
        if (argv != NULL) {
            for (i = 0; i < argc; i++)
                if (argv[i] != NULL)
                    free(argv[i]);
            free(argv);
        }
	PyErr_SetString(PyExc_RuntimeError, "cannot open display");
	return NULL;
    }
    PySys_SetArgv(argc, argv);

    if (argv != NULL) {
        for (i = 0; i < argc; i++)
            if (argv[i] != NULL)
                free(argv[i]);
        free(argv);
    }
    gtk_signal_set_funcs((GtkSignalMarshal)PyGtk_SignalMarshal,
                         (GtkSignalDestroy)PyGtk_SignalDestroy);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_set_locale(PyObject *self, PyObject *args) {
    char *locale;
    PyObject *py_locale;

    if (!PyArg_ParseTuple(args, ":gtk_set_locale"))
        return NULL;
    locale = gtk_set_locale();
    py_locale = PyString_FromString(locale);
    setlocale(LC_NUMERIC, "C");
    return py_locale;
}

static PyObject *_wrap_gtk_main(PyObject *self, PyObject *args) {
    if (!PyArg_ParseTuple(args, ":gtk_main"))
        return NULL;
    PyGTK_UNBLOCK_THREADS
    gtk_main();
    PyGTK_BLOCK_THREADS

    if (PyErr_Occurred())
        return NULL;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_main_iteration(PyObject *self, PyObject *args) {
    int block = 1, ret;

    if(!PyArg_ParseTuple(args,"|i:gtk_main_iteration", &block)) 
        return NULL;
    PyGTK_UNBLOCK_THREADS
    ret = gtk_main_iteration_do(block);
    PyGTK_BLOCK_THREADS
    return PyInt_FromLong(ret);
}

static PyObject *_wrap_gtk_events_pending(PyObject *self, PyObject *args) {
    int ret;
    if (!PyArg_ParseTuple(args, ":gtk_events_pending"))
        return NULL;
    ret = gtk_events_pending();
    return PyInt_FromLong(ret);
}

static PyObject *
_wrap_gtk_timeout_add(PyObject *self, PyObject *args) {
    guint32 interval;
    PyObject *callback, *cbargs = NULL, *data;
    if (!PyArg_ParseTuple(args, "iO|O!:gtk_timeout_add", &interval, &callback,
			  &PyTuple_Type, &cbargs))
        return NULL;
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "second arg not callable");
        return NULL;
    }
    if (cbargs)
	Py_INCREF(cbargs);
    else
	cbargs = PyTuple_New(0);
    if (cbargs == NULL)
      return NULL;
    data = Py_BuildValue("(ON)", callback, cbargs);
    if (data == NULL)
      return NULL;
    return PyInt_FromLong(gtk_timeout_add_full(interval, NULL,
        (GtkCallbackMarshal)PyGtk_HandlerMarshal,
	data,
        (GtkDestroyNotify)PyGtk_DestroyNotify));
}

static PyObject *
_wrap_gtk_idle_add(PyObject *self, PyObject *args) {
    PyObject *callback, *cbargs = NULL, *data;

    if (!PyArg_ParseTuple(args, "O|O!:gtk_idle_add", &callback,
			  &PyTuple_Type, &cbargs))
        return NULL;
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "arg not callable");
        return NULL;
    }
    if (cbargs)
	Py_INCREF(cbargs);
    else
	cbargs = PyTuple_New(0);
    if (cbargs == NULL)
      return NULL;
    data = Py_BuildValue("(ON)", callback, cbargs);
    if (data == NULL)
      return NULL;
    return PyInt_FromLong(gtk_idle_add_full(GTK_PRIORITY_DEFAULT, NULL,
        (GtkCallbackMarshal)PyGtk_HandlerMarshal,
	data,
        (GtkDestroyNotify)PyGtk_DestroyNotify));
}

static PyObject *
_wrap_gtk_idle_add_priority(PyObject *self, PyObject *args) {
    int p;
    PyObject *callback;
    if (!PyArg_ParseTuple(args, "iO:gtk_idle_add_priority", &p, &callback))
        return NULL;
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "2nd arg not callable");
        return NULL;
    }
    Py_INCREF(callback);
    return PyInt_FromLong(gtk_idle_add_full(p, NULL,
        (GtkCallbackMarshal)PyGtk_HandlerMarshal, callback, 
        (GtkDestroyNotify)PyGtk_DestroyNotify));
}

static PyObject *_wrap_gtk_idle_remove_function(PyObject *self, PyObject *args) {
    PyObject *data;

    if(!PyArg_ParseTuple(args,"O:gtk_idle_remove_function", &data)) 
        return NULL;
    gtk_idle_remove_by_data(data);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gtk_quit_add(PyObject *self, PyObject *args) {
    int main_level;
    PyObject *callback, *cbargs = NULL, *data;
    if (!PyArg_ParseTuple(args, "iO|O!:gtk_quit_add", &main_level, &callback,
			  &PyTuple_Type, &cbargs))
        return NULL;
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "2nd arg not callable");
        return NULL;
    }
    if (cbargs)
	Py_INCREF(cbargs);
    else
	cbargs = PyTuple_New(0);
    if (cbargs == NULL)
      return NULL;
    data = Py_BuildValue("(ON)", callback, cbargs);
    if (data == NULL)
      return NULL;
    return PyInt_FromLong(gtk_quit_add_full(main_level, NULL,
        (GtkCallbackMarshal)PyGtk_HandlerMarshal,
					    data, 
        (GtkDestroyNotify)PyGtk_DestroyNotify));
}

static PyObject *_wrap_gtk_quit_remove_function(PyObject *self, PyObject *args) {
    PyObject *data;

    if(!PyArg_ParseTuple(args,"O:gtk_quit_remove_function",&data)) 
        return NULL;
    gtk_quit_remove_by_data(data);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gtk_input_add(PyObject *self, PyObject *args) {
    int source;
    GdkInputCondition condition;
    PyObject *callback;
    if (!PyArg_ParseTuple(args, "iiO:gtk_input_add", &source, &condition,
			  &callback))
        return NULL;
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "3rd arg not callable.");
	return NULL;
    }
    Py_INCREF(callback);
    return PyInt_FromLong(gtk_input_add_full(source, condition, NULL,
			(GtkCallbackMarshal)PyGtk_InputMarshal, callback,
			(GtkDestroyNotify)PyGtk_DestroyNotify));
}

static PyObject *_wrap_gtk_object_set(PyObject *self, PyObject *args) {
    PyObject *obj, *dict;
    GtkArg *arg;
    gint nargs;

    if (!PyArg_ParseTuple(args, "O!O!:gtk_object_set", &PyGtk_Type, &obj,
                  &PyDict_Type, &dict))
        return NULL;
    arg = PyDict_AsGtkArgs(dict, GTK_OBJECT_TYPE(PyGtk_Get(obj)), &nargs);
    if (arg == NULL && nargs != 0)
      return NULL;
    gtk_object_setv(PyGtk_Get(obj), nargs, arg);
    g_free(arg);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_object_new(PyObject *self, PyObject *args) {
    GtkType type;
    gint nargs;
    GtkArg *arg;
    PyObject *dict;
    GtkObject *obj;

    if (!PyArg_ParseTuple(args, "iO!:gtk_object_new", &type, &PyDict_Type,
			  &dict))
        return NULL;
    arg = PyDict_AsGtkArgs(dict, type, &nargs);
    if (arg == NULL && nargs != 0)
        return NULL;
    obj = gtk_object_newv(type, nargs, arg);
    g_free(arg);
    return PyGtk_New(obj);
}

static PyObject *_wrap_gtk_object_get(PyObject *self, PyObject *args) {
    PyGtk_Object *o;
    char *name;
    GtkArg garg;
    PyObject *ret;

    if (!PyArg_ParseTuple(args, "O!s:gtk_object_get", &PyGtk_Type, &o, &name))
        return NULL;
    garg.name = name;
    gtk_object_getv(PyGtk_Get(o), 1, &garg);

    if (garg.type == GTK_TYPE_INVALID) {
        PyErr_SetString(PyExc_AttributeError, "invalid property");
        return NULL;
    }
    ret = GtkArg_AsPyObject(&garg);
    if (ret == NULL) {
      PyErr_SetString(PyExc_TypeError, "couldn't translate type");
      return NULL;
    }
    return ret;
}

static PyObject *_wrap_gtk_object_set_data(PyObject *self, PyObject *args) {
	PyGtk_Object *obj;
	char *key;
	PyObject *data;
	if (!PyArg_ParseTuple(args, "O!sO:gtk_object_set_data", &PyGtk_Type,
			&obj, &key, &data))
		return NULL;
	Py_INCREF(data);
	gtk_object_set_data_full(PyGtk_Get(obj), key, data,
		(GtkDestroyNotify)PyGtk_DestroyNotify);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *_wrap_gtk_object_get_data(PyObject *self, PyObject *args) {
	PyGtk_Object *obj;
	char *key;
	PyObject *data;
	if (!PyArg_ParseTuple(args, "O!s:gtk_object_get_data", &PyGtk_Type,
			&obj, &key))
		return NULL;
	data = gtk_object_get_data(PyGtk_Get(obj), key);
	if (data != NULL) {
		Py_INCREF(data);
		return data;
	} else {
		Py_INCREF(Py_None);
		return Py_None;
	}
}

static PyObject *_wrap_gtk_object_remove_data(PyObject *self, PyObject *args) {
    char *key;
    PyObject *obj;

    if(!PyArg_ParseTuple(args,"O!s:gtk_object_remove_data", &PyGtk_Type,
			 &obj, &key)) 
        return NULL;
    gtk_object_remove_data(PyGtk_Get(obj), key);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_adjustment_set_all(PyObject *self, PyObject *args) {
    PyObject *obj;
    double value, lower, upper, step_increment, page_increment, page_size;
    GtkAdjustment *adj;
    if (!PyArg_ParseTuple(args, "O!dddddd:gtk_adjustment_set_all", &PyGtk_Type,
			  &obj, &value, &lower, &upper, &step_increment,
			  &page_increment, &page_size))
	return NULL;
    adj = GTK_ADJUSTMENT(PyGtk_Get(obj));
    adj->value = value;
    adj->lower = lower;
    adj->upper = upper;
    adj->step_increment = step_increment;
    adj->page_increment = page_increment;
    adj->page_size = page_size;
    gtk_adjustment_changed(adj);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_widget_get_window(PyObject *self, PyObject *args) {
    GdkWindow *win;
    PyObject *obj;

    if(!PyArg_ParseTuple(args,"O!:gtk_widget_get_window", &PyGtk_Type, &obj)) 
        return NULL;
    win = GTK_WIDGET(PyGtk_Get(obj))->window;
    if (win == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    } else
        return PyGdkWindow_New(win);
}

static PyObject *_wrap_gtk_widget_get_allocation(PyObject *self, PyObject *args) {
  GtkAllocation allocation;
  PyObject *obj;

  if (!PyArg_ParseTuple(args, "O!:gtk_widget_get_allocation", &PyGtk_Type,
			&obj))
    return NULL;
  allocation = GTK_WIDGET(PyGtk_Get(obj))->allocation;
  return Py_BuildValue("(iiii)", (int)allocation.x, (int)allocation.y,
		       (int)allocation.width, (int)allocation.height);
}

static PyObject *_wrap_gtk_widget_draw(PyObject *self, PyObject *args) {
    GdkRectangle rect;
    PyObject *obj;

    if (!PyArg_ParseTuple(args,"O!(hhhh):gtk_widget_draw", &PyGtk_Type, &obj,
			 &(rect.x), &(rect.y), &(rect.width), &(rect.height)))
        return NULL;
    gtk_widget_draw(GTK_WIDGET(PyGtk_Get(obj)), &rect);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_widget_size_request(PyObject *self, PyObject *args) {
    GtkRequisition requisition;
    PyObject *obj;

    if(!PyArg_ParseTuple(args,"O!:gtk_widget_size_request", &PyGtk_Type, &obj))
        return NULL;
    gtk_widget_size_request(GTK_WIDGET(PyGtk_Get(obj)), &requisition);
    return Py_BuildValue("(ii)", requisition.width, requisition.height);
}

static PyObject *_wrap_gtk_widget_size_allocate(PyObject *self, PyObject *args) {
    GtkAllocation allocation;
    PyObject *obj;

    if(!PyArg_ParseTuple(args,"O!(hhhh):gtk_widget_size_allocate", &PyGtk_Type,
			 &obj, &(allocation.x), &(allocation.y),
			 &(allocation.width), &(allocation.height)))
        return NULL;
    gtk_widget_size_allocate(GTK_WIDGET(PyGtk_Get(obj)), &allocation);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_widget_get_child_requisition(PyObject *self, PyObject *args) {
    GtkRequisition requisition;
    PyObject *obj;

    if(!PyArg_ParseTuple(args,"O!:gtk_widget_get_child_requisition", &PyGtk_Type, &obj))
        return NULL;
    gtk_widget_get_child_requisition(GTK_WIDGET(PyGtk_Get(obj)), &requisition);
    return Py_BuildValue("(ii)", requisition.width, requisition.height);
}

static PyObject *_wrap_gtk_widget_intersect(PyObject *self, PyObject *args) {
    PyObject *obj;
    GdkRectangle area;
    GdkRectangle intersect;

    if(!PyArg_ParseTuple(args,"O!(hhhh):gtk_widget_intersect", &PyGtk_Type,
			 &obj, &(area.x), &(area.y), &(area.width), &(area.height))) 
        return NULL;
    if (gtk_widget_intersect(GTK_WIDGET(PyGtk_Get(obj)), &area, &intersect))
        return Py_BuildValue("(iiii)", (int)intersect.x, (int)intersect.y,
			     (int)intersect.width, (int)intersect.height);
    else {
        Py_INCREF(Py_None);
        return Py_None;
    }
}

static PyObject *_wrap_gtk_widget_get_pointer(PyObject *self, PyObject *args) {
  int x, y;
  PyObject *w;
  if (!PyArg_ParseTuple(args, "O!:gtk_widget_get_pointer", &PyGtk_Type, &w))
    return NULL;
  gtk_widget_get_pointer(GTK_WIDGET(PyGtk_Get(w)), &x, &y);
  return Py_BuildValue("(ii)", x, y);
}

static PyObject *_wrap_gtk_container_add(PyObject *self, PyObject *args) {
    PyObject *obj, *child, *dict = NULL;

    if(!PyArg_ParseTuple(args,"O!O!|O!:gtk_container_add", &PyGtk_Type, &obj,
			 &PyGtk_Type, &child, &PyDict_Type, &dict)) 
        return NULL;
    if (dict == NULL)
      gtk_container_add(GTK_CONTAINER(PyGtk_Get(obj)),
			GTK_WIDGET(PyGtk_Get(child)));
    else {
      gint nargs;
      GtkArg *arg = PyDict_AsContainerArgs(dict,
					   GTK_OBJECT_TYPE(PyGtk_Get(obj)),
					   &nargs);
      if (arg == NULL && nargs != 0)
	return NULL;
      gtk_container_addv(GTK_CONTAINER(PyGtk_Get(obj)),
			 GTK_WIDGET(PyGtk_Get(child)), nargs, arg);
      g_free(arg);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_container_child_set(PyObject *self, PyObject *args){
    PyObject *obj, *child, *dict;
    gint nargs;
    GtkArg *arg;

    if (!PyArg_ParseTuple(args, "O!O!O!:gtk_container_child_set", &PyGtk_Type,
                          &obj, &PyGtk_Type, &child, &PyDict_Type, &dict))
        return NULL;
    arg = PyDict_AsContainerArgs(dict, GTK_OBJECT_TYPE(PyGtk_Get(obj)),&nargs);
    if (arg == NULL && nargs != 0)
        return NULL;
    gtk_container_child_setv(GTK_CONTAINER(PyGtk_Get(obj)),
                             GTK_WIDGET(PyGtk_Get(child)), nargs, arg);
    g_free(arg);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_container_child_get(PyObject *self, PyObject *args){
    PyObject *obj, *child, *ret;
    GtkArg arg;

    if (!PyArg_ParseTuple(args, "O!O!s:gtk_container_child_get", &PyGtk_Type,
			  &obj, &PyGtk_Type, &child, &(arg.name)))
        return NULL;
    gtk_container_child_getv(GTK_CONTAINER(PyGtk_Get(obj)),
			     GTK_WIDGET(PyGtk_Get(child)), 1, &arg);
    if (arg.type == GTK_TYPE_INVALID) {
        PyErr_SetString(PyExc_AttributeError, "invalid property");
	return NULL;
    }
    ret = GtkArg_AsPyObject(&arg);
    if (ret == NULL) {
        PyErr_SetString(PyExc_TypeError, "couldn't translate type");
	return NULL;
    }
    return ret;
}

static PyObject *_wrap_gtk_container_children(PyObject *self, PyObject *args) {
    PyObject *obj;
    GList *list, *tmp;
    PyObject *py_list;
    PyObject *gtk_obj;

    if(!PyArg_ParseTuple(args,"O!:gtk_container_children", &PyGtk_Type, &obj))
        return NULL;
    list = gtk_container_children(GTK_CONTAINER(PyGtk_Get(obj)));

    if ((py_list = PyList_New(0)) == NULL) {
      g_list_free(list);
      return NULL;
    }

    for (tmp = list; tmp != NULL; tmp = tmp->next) {
      gtk_obj = PyGtk_New(GTK_OBJECT(tmp->data));
      if (gtk_obj == NULL) {
	g_list_free(list);
	Py_DECREF(py_list);
	return NULL;
      }
      PyList_Append(py_list, gtk_obj);
      Py_DECREF(gtk_obj);
    }
    g_list_free(list);
    return py_list;
}

static PyObject *_wrap_gtk_window_set_icon(PyObject *self, PyObject *args) {
	PyGtk_Object *o, *pix, *mask;
        GtkWidget *win;
	if (!PyArg_ParseTuple(args, "O!O!O!:gtk_window_set_icon", &PyGtk_Type,
			&o, &PyGdkWindow_Type, &pix, &PyGdkWindow_Type, &mask))
		return NULL;
	win = GTK_WIDGET(PyGtk_Get(o));
	gtk_widget_realize(win);
	gdk_window_set_icon(win->window, NULL, PyGdkWindow_Get(pix),
			    PyGdkWindow_Get(mask));
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *_wrap_gtk_window_set_icon_name(PyObject *self, PyObject *args) {
	PyGtk_Object *o;
	char *name;
	if (!PyArg_ParseTuple(args, "O!s:gtk_window_set_icon_name",
			&PyGtk_Type, &o, &name))
		return NULL;
	gdk_window_set_icon_name(GTK_WIDGET(PyGtk_Get(o))->window, name);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *_wrap_gtk_window_set_geometry_hints(PyObject *self, PyObject *args) {
  PyObject *window, *py_geom_wid, *hints, *key, *value;
  GtkWidget *geom_wid = NULL;
  GdkGeometry geometry = { -1,-1, -1,-1, -1,-1, -1,-1, -1.0,-1.0 };
  GdkWindowHints geom_mask = 0;
  int i = 0;

  if (!PyArg_ParseTuple(args, "O!OO!:gtk_window_set_geometry_hints",
			&PyGtk_Type, &window, &py_geom_wid, &PyDict_Type,
			&hints))
    return NULL;
  if (PyGtk_Check(py_geom_wid))
    geom_wid = GTK_WIDGET(PyGtk_Get(py_geom_wid));
  else if (py_geom_wid != Py_None) {
    PyErr_SetString(PyExc_TypeError, "second argument must be a GtkWidget or none");
    return NULL;
  }
  while (PyDict_Next(hints, &i, &key, &value)) {
    gchar *name;
    if (!PyString_Check(key)) {
      PyErr_SetString(PyExc_TypeError, "hint keys must be strings");
      return NULL;
    }
    name = PyString_AsString(key);
    if (PyInt_Check(value)) {
      int val = PyInt_AsLong(value);
      if (!strcmp(name, "min_width")) {
	geometry.min_width = val;
	geom_mask |= GDK_HINT_MIN_SIZE;
      } else if (!strcmp(name, "min_height")) {
	geometry.min_height = val;
	geom_mask |= GDK_HINT_MIN_SIZE;
      } else if (!strcmp(name, "max_width")) {
	geometry.max_width = val;
	geom_mask |= GDK_HINT_MAX_SIZE;
      } else if (!strcmp(name, "max_height")) {
	geometry.max_height = val;
	geom_mask |= GDK_HINT_MAX_SIZE;
      } else if (!strcmp(name, "base_width")) {
	geometry.base_width = val;
	geom_mask |= GDK_HINT_BASE_SIZE;
      } else if (!strcmp(name, "base_height")) {
	geometry.base_height = val;
	geom_mask |= GDK_HINT_BASE_SIZE;
      } else if (!strcmp(name, "width_inc")) {
	geometry.width_inc = val;
	geom_mask |= GDK_HINT_RESIZE_INC;
      } else if (!strcmp(name, "height_inc")) {
	geometry.height_inc = val;
	geom_mask |= GDK_HINT_RESIZE_INC;
      } else if (!strcmp(name, "min_aspect")) {
	geometry.min_aspect = val;
	geom_mask |= GDK_HINT_ASPECT;
      } else if (!strcmp(name, "max_aspect")) {
	geometry.max_aspect = val;
	geom_mask |= GDK_HINT_ASPECT;
      } else {
	gchar *err = g_strdup_printf("unknown hint name or wrong type for %s",
				     name);
	PyErr_SetString(PyExc_TypeError, err);
	g_free(err);
	return NULL;
      }
    } else if (PyFloat_Check(value)) {
      double val = PyFloat_AsDouble(value);
      if (!strcmp(name, "min_aspect")) {
	geometry.min_aspect = val;
	geom_mask |= GDK_HINT_ASPECT;
      } else if (!strcmp(name, "max_aspect")) {
	geometry.max_aspect = val;
	geom_mask |= GDK_HINT_ASPECT;
      } else {
	gchar *err = g_strdup_printf("unknown hint name or wrong type for %s",
				     name);
	PyErr_SetString(PyExc_TypeError, err);
	g_free(err);
	return NULL;
      }
    } else {
      gchar *err = g_strdup_printf("unknown hint name or wrong type for %s",
				   name);
      PyErr_SetString(PyExc_TypeError, err);
      g_free(err);
      return NULL;
    }
  }
  if (geom_mask & GDK_HINT_ASPECT && (geometry.min_aspect == 0.0 ||
				      geometry.max_aspect == 0.0)) {
    PyErr_SetString(PyExc_TypeError, "setting aspect to zero is a Bad Thing");
    return NULL;
  }
  gtk_window_set_geometry_hints(GTK_WINDOW(PyGtk_Get(window)), geom_wid,
				&geometry, geom_mask);
  Py_INCREF(Py_None);
  return Py_None;
}

/* --- */
static PyObject *
_wrap_gtk_box_query_child_packing(PyObject *self, PyObject *args) {
  PyObject *box, *child;
  gboolean expand, fill;
  guint padding;
  GtkPackType pack_type;
  if (!PyArg_ParseTuple(args, "O!O!:gtk_box_query_child_packing", &PyGtk_Type,
			&box, &PyGtk_Type, &child))
    return NULL;
  gtk_box_query_child_packing(GTK_BOX(PyGtk_Get(box)),
			      GTK_WIDGET(PyGtk_Get(child)), &expand, &fill,
			      &padding, &pack_type);
  return Py_BuildValue("(iiii)", (int)expand, (int)fill, padding, pack_type);
}

static PyObject *
_wrap_gtk_button_box_get_child_size_default(PyObject *self, PyObject *args) {
    int mw, mh;
    if (!PyArg_ParseTuple(args, ":gtk_button_box_get_child_size_default"))
        return NULL;
    gtk_button_box_get_child_size_default(&mw, &mh);
    return Py_BuildValue("(ii)", mw, mh);
}
static PyObject *
_wrap_gtk_button_box_get_child_ipadding_default(PyObject *self, PyObject *args) {
    int ipx, ipy;
    if (!PyArg_ParseTuple(args, ":gtk_button_box_get_child_ipadding_default"))
        return NULL;
    gtk_button_box_get_child_ipadding_default(&ipx, &ipy);
    return Py_BuildValue("(ii)", ipx, ipy);
}
static PyObject *
_wrap_gtk_button_box_get_child_size(PyObject *self, PyObject *args) {
    int mw, mh;
    PyGtk_Object *o;
    if (!PyArg_ParseTuple(args, "O!:gtk_button_box_get_child_size",
            &PyGtk_Type, &o))
        return NULL;
    gtk_button_box_get_child_size(GTK_BUTTON_BOX(PyGtk_Get(o)), &mw, &mh);
    return Py_BuildValue("(ii)", mw, mh);
}
static PyObject *
_wrap_gtk_button_box_get_child_ipadding(PyObject *self, PyObject *args) {
    int ipx, ipy;
    PyGtk_Object *o;
    if (!PyArg_ParseTuple(args, "O!:gtk_button_box_get_child_ipadding",
            &PyGtk_Type, &o))
        return NULL;
    gtk_button_box_get_child_ipadding(GTK_BUTTON_BOX(PyGtk_Get(o)),&ipx,&ipy);
    return Py_BuildValue("(ii)", ipx, ipy);
}

static PyObject *_wrap_gtk_clist_get_selection(PyObject *self, PyObject *args) {
  GList *selection;
  guint row;
  PyObject *clist, *ret, *py_int;
  if (!PyArg_ParseTuple(args, "O!:gtk_clist_get_selection", &PyGtk_Type,
			&clist))
    return NULL;
  ret = PyList_New(0);
  if (ret == NULL)
    return NULL;

  for (selection = GTK_CLIST(PyGtk_Get(clist))->selection; selection != NULL;
       selection = selection->next) {
    row = GPOINTER_TO_UINT(selection->data);
    py_int = PyInt_FromLong(row);
    if (!py_int) {
      Py_DECREF(ret);
      return NULL;
    }

    PyList_Append(ret, py_int);
    Py_DECREF(py_int);
  }
  return ret;
}

static PyObject *_wrap_gtk_clist_new_with_titles(PyObject *self, PyObject *args) {
	int c, i;
	PyObject *l, *item;
	char **list;
	if (!PyArg_ParseTuple(args, "iO:gtk_clist_new_with_titles",
			&c, &l))
		return NULL;
	if (!PySequence_Check(l)) {
		PyErr_SetString(PyExc_TypeError,"3rd argument not a sequence");
		return NULL;
	}
	if (PySequence_Length(l) < c) {
		PyErr_SetString(PyExc_TypeError, "sequence not long enough");
		return NULL;
	}
	list = malloc(sizeof(char *)*c);
	for (i = 0; i < c; i++) {
		item = PySequence_GetItem(l, i);
		Py_DECREF(item);
		if (!PyString_Check(item)) {
			PyErr_SetString(PyExc_TypeError,
				 "sequence item not a string");
			free(list);
			return NULL;
		}
		list[i] = PyString_AsString(item);
	}
	item = (PyObject *) PyGtk_New((GtkObject *)gtk_clist_new_with_titles
		(c, list));
	free(list);
	return item;
}

static PyObject *_wrap_gtk_clist_get_text(PyObject *self, PyObject *args) {
  PyObject *w;
  int r, c;
  char *text;

  if (!PyArg_ParseTuple(args, "O!ii:gtk_clist_get_text", &PyGtk_Type,
			&w, &r, &c))
    return NULL;
  if (!gtk_clist_get_text(GTK_CLIST(PyGtk_Get(w)), r, c, &text)) {
    PyErr_SetString(PyExc_ValueError, "can't get text value");
    return NULL;
  }
  return PyString_FromString(text);
}

static PyObject *_wrap_gtk_clist_get_pixmap(PyObject *self, PyObject *args) {
  PyObject *o, *pixmap, *mask;
  int r, c;
  GdkPixmap *p;
  GdkBitmap *m;

  if (!PyArg_ParseTuple(args, "O!ii:gtk_clist_get_pixmap", &PyGtk_Type,
			&o, &r, &c))
    return NULL;
  if (!gtk_clist_get_pixmap(GTK_CLIST(PyGtk_Get(o)), r, c, &p, &m)) {
    PyErr_SetString(PyExc_ValueError, "can't get pixmap value");
    return NULL;
  }
  if (p)
    pixmap = PyGdkWindow_New(p);
  else {
    Py_INCREF(Py_None);
    pixmap = Py_None;
  }
  if (m)
    mask = PyGdkWindow_New(m);
  else {
    Py_INCREF(Py_None);
    mask = Py_None;
  }
  return Py_BuildValue("(NN)", pixmap, mask);
}

static PyObject *_wrap_gtk_clist_get_pixtext(PyObject *self, PyObject *args) {
  PyObject *o, *pixmap, *mask;
  int r, c;
  guint8 spacing;
  char *text;
  GdkPixmap *p;
  GdkBitmap *m;

  if (!PyArg_ParseTuple(args, "O!ii:gtk_clist_get_pixtext",
			&PyGtk_Type, &o, &r, &c))
    return NULL;
  if (!gtk_clist_get_pixtext(GTK_CLIST(PyGtk_Get(o)), r, c,
			     &text, &spacing, &p, &m)) {
    PyErr_SetString(PyExc_ValueError, "can't get pixtext value");
    return NULL;
  }
  if (p)
    pixmap = PyGdkWindow_New(p);
  else {
    Py_INCREF(Py_None);
    pixmap = Py_None;
  }
  if (m)
    mask = PyGdkWindow_New(m);
  else {
    Py_INCREF(Py_None);
    mask = Py_None;
  }
  return Py_BuildValue("(siNN)", text, (int)spacing, pixmap, mask);
}

static PyObject *_wrap_gtk_clist_prepend(PyObject *self, PyObject *args) {
	PyGtk_Object *cl;
	int col, i;
	PyObject *l, *item;
	char **list;
	if (!PyArg_ParseTuple(args, "O!O:gtk_clist_prepend", &PyGtk_Type, &cl,
			&l))
		return NULL;
	if (!PySequence_Check(l)) {
		PyErr_SetString(PyExc_TypeError, "argument not a sequence");
		return NULL;
	}
	col = GTK_CLIST(PyGtk_Get(cl))->columns;
	if (PySequence_Length(l) < col) {
		PyErr_SetString(PyExc_TypeError, "sequnce too short");
		return NULL;
	}
	list = g_new(gchar *, col);
	for (i = 0; i < col; i++) {
		item = PySequence_GetItem(l, i);
		Py_DECREF(item);
		if (!PyString_Check(item)) {
			PyErr_SetString(PyExc_TypeError,
				"sequence item not a string");
			g_free(list);
			return NULL;
		}
		list[i] = PyString_AsString(item);
	}
	i = gtk_clist_prepend(GTK_CLIST(PyGtk_Get(cl)), list);
	g_free(list);
	return PyInt_FromLong(i);
}

static PyObject *_wrap_gtk_clist_append(PyObject *self, PyObject *args) {
	PyGtk_Object *cl;
	int col, i;
	PyObject *l, *item;
	char **list;
	if (!PyArg_ParseTuple(args, "O!O:gtk_clist_append", &PyGtk_Type, &cl,
			&l))
		return NULL;
	if (!PySequence_Check(l)) {
		PyErr_SetString(PyExc_TypeError, "argument not a sequence");
		return NULL;
	}
	col = GTK_CLIST(PyGtk_Get(cl))->columns;
	if (PySequence_Length(l) < col) {
		PyErr_SetString(PyExc_TypeError, "sequnce too short");
		return NULL;
	}
	list = g_new(gchar *, col);
	for (i = 0; i < col; i++) {
		item = PySequence_GetItem(l, i);
		Py_DECREF(item);
		if (!PyString_Check(item)) {
			PyErr_SetString(PyExc_TypeError,
				"sequence item not a string");
			g_free(list);
			return NULL;
		}
		list[i] = PyString_AsString(item);
	}
	i = gtk_clist_append(GTK_CLIST(PyGtk_Get(cl)), list);
	g_free(list);
	return PyInt_FromLong(i);
}

static PyObject *_wrap_gtk_clist_insert(PyObject *self, PyObject *args) {
	PyGtk_Object *cl;
	int col, r, i;
	PyObject *l, *item;
	char **list;
	if (!PyArg_ParseTuple(args, "O!iO:gtk_clist_insert", &PyGtk_Type, &cl,
			&r, &l))
		return NULL;
	if (!PySequence_Check(l)) {
		PyErr_SetString(PyExc_TypeError, "argument not a sequence");
		return NULL;
	}
	col = GTK_CLIST(PyGtk_Get(cl))->columns;
	if (PySequence_Length(l) < col) {
		PyErr_SetString(PyExc_TypeError, "sequnce too short");
		return NULL;
	}
	list = g_new(gchar *, col);
	for (i = 0; i < col; i++) {
		item = PySequence_GetItem(l, i);
		Py_DECREF(item);
		if (!PyString_Check(item)) {
			PyErr_SetString(PyExc_TypeError,
				"sequence item not a string");
			g_free(list);
			return NULL;
		}
		list[i] = PyString_AsString(item);
	}
	i = gtk_clist_insert(GTK_CLIST(PyGtk_Get(cl)), r, list);
	g_free(list);
	return PyInt_FromLong(i);
}
static PyObject *_wrap_gtk_clist_set_row_data(PyObject *self, PyObject *args) {
        PyGtk_Object *obj;
	int row;
	PyObject *data;
	if (!PyArg_ParseTuple(args, "O!iO:gtk_clist_set_row_data", &PyGtk_Type,
			      &obj, &row, &data))
	    return NULL;
	Py_INCREF(data);
	gtk_clist_set_row_data_full(GTK_CLIST(PyGtk_Get(obj)), row, data,
				    (GtkDestroyNotify)PyGtk_DestroyNotify);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *_wrap_gtk_clist_get_row_data(PyObject *s, PyObject *args) {
    PyObject *clist, *ret;
    int row;

    if (!PyArg_ParseTuple(args, "O!i:gtk_clist_get_row_data", &PyGtk_Type,
			  &clist, &row))
        return NULL;
    ret = gtk_clist_get_row_data(GTK_CLIST(PyGtk_Get(clist)), row);
    if (!ret) ret = Py_None;
    Py_INCREF(ret);
    return ret;
}
static PyObject *_wrap_gtk_clist_find_row_from_data(PyObject *s, PyObject *args) {
    PyObject *clist, *data;

    if (!PyArg_ParseTuple(args, "O!O:gtk_clist_find_row_from_data",
			  &PyGtk_Type, &clist, &data))
        return NULL;
    return PyInt_FromLong(gtk_clist_find_row_from_data(
				GTK_CLIST(PyGtk_Get(clist)), data));
}
static PyObject *_wrap_gtk_clist_get_selection_info(PyObject *s, PyObject *args) {
  PyObject *clist;
  gint x, y, row, column;
  if (!PyArg_ParseTuple(args, "O!ii:gtk_clist_get_selection_info", &PyGtk_Type,
			&clist, &x, &y))
    return NULL;
  if (gtk_clist_get_selection_info(GTK_CLIST(PyGtk_Get(clist)), x, y,
				   &row, &column))
    return Py_BuildValue("(ii)", row, column);
  else {
    Py_INCREF(Py_None);
    return Py_None;
  }
}   

static PyObject *_wrap_gtk_clist_get_column_width(PyObject *self, PyObject *args) {
    PyObject *obj;
    gint col;
    if (!PyArg_ParseTuple(args, "O!i:gtk_clist_get_column_width", &PyGtk_Type,
			  &obj, &col))
	return NULL;
    if (col >= 0 && col < GTK_CLIST(PyGtk_Get(obj))->columns)
	return PyInt_FromLong(GTK_CLIST(PyGtk_Get(obj))->column[col].width);
    else
	return PyInt_FromLong(-1);
}


static PyObject *
_wrap_gtk_combo_set_popdown_strings(PyObject *self, PyObject *args) {
  PyObject *obj, *list, *item;
  GList *glist = NULL;
  int len, i;
  if (!PyArg_ParseTuple(args, "O!O:gtk_combo_set_popdown_strings",
			&PyGtk_Type, &obj, &list))
    return NULL;
  if (!PySequence_Check(list)) {
    PyErr_SetString(PyExc_TypeError, "second argument must be a sequence");
    return NULL;
  }
  len = PySequence_Length(list);
  for (i = 0; i < len; i++) {
    item = PySequence_GetItem(list, i);
    Py_DECREF(item);
    if (!PyString_Check(item)) {
      PyErr_SetString(PyExc_TypeError, "sequence item not a string");
      g_list_free(glist);
      return NULL;
    }
    glist = g_list_append(glist, PyString_AsString(item));
  }
  gtk_combo_set_popdown_strings(GTK_COMBO(PyGtk_Get(obj)), glist);
  g_list_free(glist);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *_wrap_gtk_curve_get_vector(PyObject *self, PyObject *args) {
  PyGtk_Object *obj;
  int size=-1, i;
  gfloat *vector;
  PyObject *ret;

  if (!PyArg_ParseTuple(args, "O!|i:gtk_curve_get_vector", &PyGtk_Type, &obj,
			&size))
    return NULL;
  if (size < 0) size = GTK_CURVE(PyGtk_Get(obj))->num_points;
  vector = g_new(gfloat, size);
  gtk_curve_get_vector(GTK_CURVE(PyGtk_Get(obj)), size, vector);
  ret = PyTuple_New(size);
  for (i = 0; i < size; i++)
    PyTuple_SetItem(ret, i, PyFloat_FromDouble(vector[i]));
  g_free(vector);
  return ret;
}
static PyObject *_wrap_gtk_curve_set_vector(PyObject *self, PyObject *args) {
  PyGtk_Object *obj;
  PyObject *seq, *item;
  int size, i;
  gfloat *vector;

  if (!PyArg_ParseTuple(args, "O!O:gtk_curve_set_vector", &PyGtk_Type, &obj,
			&seq))
    return NULL;
  if (!PySequence_Check(seq)) {
    PyErr_SetString(PyExc_TypeError, "2nd argument is not a sequence");
    return NULL;
  }
  size = PySequence_Length(seq);
  vector = g_new(gfloat, size);
  for (i = 0; i < size; i++) {
    item = PySequence_GetItem(seq, i);
    Py_DECREF(item);
    item = PyNumber_Float(item);
    if (item == NULL) {
      g_free(vector);
      return NULL;
    }
    vector[i] = PyFloat_AsDouble(item);
    Py_DECREF(item);
  }
  gtk_curve_set_vector(GTK_CURVE(PyGtk_Get(obj)), size, vector);
  g_free(vector);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *_wrap_gtk_editable_insert_text(PyObject *self, PyObject *args) {
    PyGtk_Object *o;
    char *text;
    int len, pos;
    if (!PyArg_ParseTuple(args, "O!s#:gtk_editable_insert_text",
			  &PyGtk_Type, &o, &text, &len))
        return NULL;
    gtk_editable_insert_text(GTK_EDITABLE(PyGtk_Get(o)), text, len, &pos);
    return PyInt_FromLong(pos);
}

static void PyGtk_item_factory_cb(PyObject *callback, guint action,
				  GtkWidget *widget) {
  PyObject *ret;

  PyGTK_BLOCK_THREADS
  ret = PyObject_CallFunction(callback, "iO", action,
			      PyGtk_New((GtkObject *)widget));
  if (ret == NULL) {
    if (PyGtk_FatalExceptions)
      gtk_main_quit();
    else {
      PyErr_Print();
      PyErr_Clear();
    }
  } else
    Py_DECREF(ret);
  PyGTK_UNBLOCK_THREADS
}

static PyObject *_wrap_gtk_item_factory_create_items(PyObject *self,PyObject *args){
  PyObject *obj, *list, *item, *cb;
  GtkItemFactoryEntry ent;
  GtkItemFactory *ifact;
  int i, len;
  if (!PyArg_ParseTuple(args, "O!O!:gtk_item_factory_create_items",
			&PyGtk_Type, &obj, &PyList_Type, &list))
    return NULL;
  ifact = GTK_ITEM_FACTORY(PyGtk_Get(obj));
  len = PyList_Size(list);
  for (i = 0; i < len; i++) {
    item = PyList_GetItem(list, i);
    if (!PyArg_ParseTuple(item, "zzOiz", &(ent.path), &(ent.accelerator),
			  &cb, &(ent.callback_action), &(ent.item_type)))
      return NULL;
    if (cb == Py_None)
      ent.callback = NULL;
    else {
      Py_INCREF(cb);
      ent.callback = (GtkItemFactoryCallback)PyGtk_item_factory_cb;
    }
    gtk_item_factory_create_item(ifact, &ent, cb, 1);
  }
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *_wrap_gtk_item_factory_get_widget(PyObject *self, PyObject *args) {
    char *path;
    PyObject *ifactory;
    GtkWidget *ret;

    if (!PyArg_ParseTuple(args, "O!s:gtk_item_factory_get_widget",
			  &PyGtk_Type, &ifactory, &path))
        return NULL;
    ret = gtk_item_factory_get_widget(GTK_ITEM_FACTORY(PyGtk_Get(ifactory)),
				      path);
    if (ret)
      return PyGtk_New((GtkObject *)ret);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_item_factory_get_widget_by_action(PyObject *self, PyObject *args) {
    int action;
    PyObject *ifactory;
    GtkWidget *ret;

    if (!PyArg_ParseTuple(args, "O!i:gtk_item_factory_get_widget_by_action",
			  &PyGtk_Type, &ifactory, &action))
        return NULL;
    ret = gtk_item_factory_get_widget_by_action(
			GTK_ITEM_FACTORY(PyGtk_Get(ifactory)), action);
    if (ret)
      return PyGtk_New((GtkObject *)ret);
    Py_INCREF(Py_None);
    return Py_None;
}

static void PyGtk_MenuPosition(GtkMenu *menu, int *x, int *y, PyObject *func) {
    PyObject *ret;

    PyGTK_BLOCK_THREADS
    ret = PyObject_CallFunction(func, "Oii", PyGtk_New(GTK_OBJECT(menu)),
				*x, *y);
    if (ret == NULL || !PyArg_ParseTuple(ret, "ii", x, y)) {
        if (PyGtk_FatalExceptions)
	    gtk_main_quit();
        else {
	    PyErr_Print();
	    PyErr_Clear();
	}
        if (ret) {
	  Py_DECREF(ret);
	}
    } else {
        Py_DECREF(ret);
    }
    PyGTK_UNBLOCK_THREADS
}
static PyObject *_wrap_gtk_menu_popup(PyObject *self, PyObject *args) {
    PyObject *m, *py_pms, *py_pmi;
    GtkWidget *pms = NULL, *pmi = NULL;
    PyObject *func;
    int button, time;
    if (!PyArg_ParseTuple(args, "O!OOOii:gtk_menu_item", &PyGtk_Type, &m,
            &py_pms, &py_pmi, &func, &button, &time))
        return NULL;
    if (PyGtk_Check(py_pms))
        pms = GTK_WIDGET(PyGtk_Get(py_pms));
    else if (py_pms != Py_None) {
        PyErr_SetString(PyExc_TypeError,
			"second argument must be a GtkWidget or None");
        return NULL;
    }
    if (PyGtk_Check(py_pmi))
        pmi = GTK_WIDGET(PyGtk_Get(py_pmi));
    else if (py_pmi != Py_None) {
        PyErr_SetString(PyExc_TypeError,
			"third argument must be a GtkWidget or None");
        return NULL;
    }
    if (!PyCallable_Check(func) && func != Py_None) {
        PyErr_SetString(PyExc_TypeError, "forth argument not callable");
        return NULL;
    }
    if (func != Py_None) {
      Py_INCREF(func);
      gtk_menu_popup(GTK_MENU(PyGtk_Get(m)), pms, pmi,
		     (GtkMenuPositionFunc)PyGtk_MenuPosition,
		     func, button, time);
    } else
      gtk_menu_popup(GTK_MENU(PyGtk_Get(m)), pms, pmi, NULL,NULL, button,time);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_pixmap_new_from_xpm(PyObject *self, PyObject *args) {
    PyGtk_Object *win;
    PyGdkColor_Object *tc = NULL;
    char *name;
    GdkColor *bg;
    GdkPixmap *pm;
    GdkBitmap *bm;
    PyObject *ret;

    if (!PyArg_ParseTuple(args, "O!s|O!:gtk_pixmap_new_from_xpm", &PyGtk_Type,
            &win, &name, &PyGdkColor_Type, &tc))
        return  NULL;
    if (tc == NULL)
        bg = NULL;
    else
        bg = PyGdkColor_Get(tc);
    pm = gdk_pixmap_create_from_xpm(GTK_WIDGET(PyGtk_Get(win))->window, &bm,
        bg, name);
    if (pm == NULL) {
      PyErr_SetString(PyExc_IOError, "can't load pixmap");
      return NULL;
    }
    ret = PyGtk_New(GTK_OBJECT(gtk_pixmap_new(pm, bm)));
    /* gtk_pixmap_new ref's the pixmap and mask, so we can unref out vars */
    gdk_pixmap_unref(pm);
    gdk_bitmap_unref(bm);
    return ret;
}

static PyObject *_wrap_gtk_pixmap_get(PyObject *self, PyObject *args) {
  PyObject *pix, *pymask;
  GdkPixmap *pixmap;
  GdkBitmap *mask;
  if (!PyArg_ParseTuple(args, "O!:gtk_pixmap_get", &PyGtk_Type, &pix))
    return NULL;
  gtk_pixmap_get(GTK_PIXMAP(PyGtk_Get(pix)), &pixmap, &mask);
  if (mask)
    pymask = PyGdkWindow_New(mask);
  else {
    Py_INCREF(Py_None);
    pymask = Py_None;
  }
  return Py_BuildValue("(NN)", PyGdkWindow_New(pixmap), pymask);
}

static PyObject *_wrap_gtk_preview_draw_row(PyObject *self, PyObject *args) {
  PyObject *preview;
  char *data;
  int dummy_len, x, y, width;

  if (!PyArg_ParseTuple(args, "O!s#iii:gtk_preview_draw_row", &PyGtk_Type,
			&preview, &data, &dummy_len, &x, &y, &width))
    return NULL;
  gtk_preview_draw_row(GTK_PREVIEW(PyGtk_Get(preview)), data, x, y, width);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *_wrap_gtk_radio_menu_item_new(PyObject *self, PyObject *args) {
    PyGtk_Object *g = NULL;
    GSList *l = NULL;
    if (!PyArg_ParseTuple(args, "|O!:gtk_radio_menu_item_new",&PyGtk_Type, &g))
        return NULL;
    if (g)
        l = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(PyGtk_Get(g)));
    return PyGtk_New(GTK_OBJECT(gtk_radio_menu_item_new(l)));
}
static PyObject *_wrap_gtk_radio_menu_item_new_with_label(PyObject *s,PyObject *a) {
    PyGtk_Object *g = NULL;
    char *label = "";
    GSList *l = NULL;
    if (!PyArg_ParseTuple(a, "|Os:gtk_radio_menu_item_new_with_label",
            &g, &label))
        return NULL;
    if (g != NULL)
        if (!PyGtk_Check(g) && (PyObject *)g != Py_None) {
            PyErr_SetString(PyExc_TypeError,
                "first argument not a GtkObject or None");
            return NULL;
        }
    if ((PyObject *)g == Py_None) g = NULL;
    if (g)
        l = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(PyGtk_Get(g)));
    return PyGtk_New(GTK_OBJECT(gtk_radio_menu_item_new_with_label(l, label)));
}

static PyObject *_wrap_gtk_radio_button_new(PyObject *self, PyObject *args) {
    if (!PyArg_ParseTuple(args, ":gtk_radio_button_new"))
        return NULL;
    return PyGtk_New((GtkObject *)gtk_radio_button_new(NULL));
}

static PyObject *_wrap_gtk_radio_button_new_with_label(PyObject *self, PyObject *args) {
    PyObject *nothing;
    char *label;

    if (!PyArg_ParseTuple(args, "Os:gtk_radio_button_new_with_label",
			  &nothing, &label))
        return NULL;
    return PyGtk_New((GtkObject *)gtk_radio_button_new_with_label(NULL,label));
}

static PyObject *_wrap_gtk_text_insert_defaults(PyObject *self, PyObject *args) {
    PyGtk_Object *txt;
    char *chars;
    int l;
    GtkStyle *style;
    if (!PyArg_ParseTuple(args, "O!s#:gtk_text_insert_defaults", &PyGtk_Type,
                                     &txt, &chars, &l))
        return NULL;
    style = gtk_widget_get_style(GTK_WIDGET(PyGtk_Get(txt)));
    gtk_text_insert(GTK_TEXT(PyGtk_Get(txt)), NULL,
        &style->fg[GTK_STATE_NORMAL], NULL, chars, l);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_toolbar_append_item(PyObject *self, PyObject *args) {
  PyGtk_Object *t, *icon;
  char *text, *tooltip, *tip_private;
  GtkWidget *ret;
  PyObject *callback;
  if (!PyArg_ParseTuple(args, "O!zzzO!O:gtk_toolbar_append_item",
			&PyGtk_Type, &t, &text, &tooltip, &tip_private,
		       &PyGtk_Type, &icon, &callback))
    return NULL;
  if (!PyCallable_Check(callback) && callback != Py_None) {
    PyErr_SetString(PyExc_TypeError,"sixth argument not callable");
    return NULL;
  }
  if (callback != Py_None) Py_INCREF(callback);
  /* if you set sigfunc to NULL, no signal is connected, rather than
     the default signal handler being used */
  ret = gtk_toolbar_append_item(GTK_TOOLBAR(PyGtk_Get(t)), text, tooltip,
				tip_private, GTK_WIDGET(PyGtk_Get(icon)),
				NULL,NULL);
  if (callback != Py_None) {
    gtk_signal_connect_full(GTK_OBJECT(ret), "clicked", NULL,
			    PyGtk_CallbackMarshal, callback,
			    PyGtk_DestroyNotify, FALSE, FALSE);
  }
  return PyGtk_New((GtkObject *)ret);
}
static PyObject *_wrap_gtk_toolbar_prepend_item(PyObject *self, PyObject *args) {
  PyGtk_Object *t, *icon;
  char *text, *tooltip, *tip_private;
  GtkWidget *ret;
  PyObject *callback;
  if (!PyArg_ParseTuple(args, "O!zzzO!O:gtk_toolbar_prepend_item",
			&PyGtk_Type, &t, &text, &tooltip, &tip_private,
			&PyGtk_Type, &icon, &callback))
    return NULL;
  if (!PyCallable_Check(callback) && callback != Py_None) {
    PyErr_SetString(PyExc_TypeError,"sixth argument not callable");
    return NULL;
  }
  if (callback != Py_None) Py_INCREF(callback);
  ret = gtk_toolbar_prepend_item(GTK_TOOLBAR(PyGtk_Get(t)), text,
				 tooltip, tip_private,
				 GTK_WIDGET(PyGtk_Get(icon)),
				 NULL, NULL);
  if (callback != Py_None)
    gtk_signal_connect_full(GTK_OBJECT(ret), "clicked", NULL,
			    PyGtk_CallbackMarshal, callback,
			    PyGtk_DestroyNotify, FALSE, FALSE);
  return PyGtk_New((GtkObject *)ret);
}
static PyObject *_wrap_gtk_toolbar_insert_item(PyObject *self, PyObject *args) {
  PyGtk_Object *t, *icon;
  char *text, *tooltip, *tip_private;
  GtkWidget *ret;
  PyObject *callback;
  int pos;
  if (!PyArg_ParseTuple(args, "O!zzzO!Oi:gtk_toolbar_insert_item",
			&PyGtk_Type, &t, &text, &tooltip, &tip_private,
			&PyGtk_Type, &icon, &callback, &pos))
    return NULL;
  if (!PyCallable_Check(callback) && callback != Py_None) {
    PyErr_SetString(PyExc_TypeError,"sixth argument not callable");
    return NULL;
  }
  if (callback != Py_None) Py_INCREF(callback);
  ret = gtk_toolbar_insert_item(GTK_TOOLBAR(PyGtk_Get(t)), text, tooltip,
				tip_private, GTK_WIDGET(PyGtk_Get(icon)),
				NULL, NULL, pos);
  if (callback != Py_None) {
    gtk_signal_connect_full(GTK_OBJECT(ret), "clicked", NULL,
			    PyGtk_CallbackMarshal, callback,
			    PyGtk_DestroyNotify, FALSE, FALSE);
  }
  return PyGtk_New((GtkObject *)ret);
}

static PyObject *
_wrap_gtk_toolbar_append_element(PyObject *self, PyObject *args)
{
  PyGtk_Object *t, *icon;
  GtkToolbarChildType type;
  char *text, *tooltip, *tip_private;
  GtkWidget *widget = NULL, *ret;
  PyObject *py_widget, *callback;

  if (!PyArg_ParseTuple(args, "O!iOzzzO!O:gtk_toolbar_append_element",
			&PyGtk_Type, &t, &type, &py_widget, &text,
			&tooltip, &tip_private,	&PyGtk_Type, &icon,
			&callback))
    return NULL;
  if (PyGtk_Check(py_widget))
    widget = GTK_WIDGET(PyGtk_Get(py_widget));
  else if (py_widget != Py_None) {
    PyErr_SetString(PyExc_TypeError, "third argument not a GtkWidget or None");
    return NULL;
  }
  if (!PyCallable_Check(callback) && callback != Py_None) {
    PyErr_SetString(PyExc_TypeError, "seventh argument not callable");
    return NULL;
  }
  if (callback != Py_None) Py_INCREF(callback);
  ret = gtk_toolbar_append_element(GTK_TOOLBAR(PyGtk_Get(t)), type, widget,
				   text, tooltip, tip_private,
				   GTK_WIDGET(PyGtk_Get(icon)), NULL, NULL);
  if (callback != Py_None) {
    gtk_signal_connect_full(GTK_OBJECT(ret), "clicked", NULL,
			    PyGtk_CallbackMarshal, callback,
			    PyGtk_DestroyNotify, FALSE, FALSE);
  }
  return PyGtk_New((GtkObject *)ret);
}

static PyObject *
_wrap_gtk_toolbar_prepend_element(PyObject *self, PyObject *args)
{
  PyGtk_Object *t, *icon;
  GtkToolbarChildType type;
  char *text, *tooltip, *tip_private;
  GtkWidget *widget = NULL, *ret;
  PyObject *py_widget, *callback;

  if (!PyArg_ParseTuple(args, "O!iOzzzO!Oi:gtk_toolbar_prepend_element",
			&PyGtk_Type, &t, &type, &py_widget, &text,
			&tooltip, &tip_private,	&PyGtk_Type, &icon,
			&callback))
    return NULL;
  if (PyGtk_Check(py_widget))
    widget = GTK_WIDGET(PyGtk_Get(py_widget));
  else if (py_widget != Py_None) {
    PyErr_SetString(PyExc_TypeError, "third argument not a GtkWidget or None");
    return NULL;
  }
  if (!PyCallable_Check(callback) && callback != Py_None) {
    PyErr_SetString(PyExc_TypeError, "seventh argument not callable");
    return NULL;
  }
  if (callback != Py_None) Py_INCREF(callback);
  ret = gtk_toolbar_prepend_element(GTK_TOOLBAR(PyGtk_Get(t)), type, widget,
				    text, tooltip, tip_private,
				    GTK_WIDGET(PyGtk_Get(icon)), NULL, NULL);
  if (callback != Py_None) {
    gtk_signal_connect_full(GTK_OBJECT(ret), "clicked", NULL,
			    PyGtk_CallbackMarshal, callback,
			    PyGtk_DestroyNotify, FALSE, FALSE);
  }
  return PyGtk_New((GtkObject *)ret);
}

static PyObject *
_wrap_gtk_toolbar_insert_element(PyObject *self, PyObject *args)
{
  PyGtk_Object *t, *icon;
  GtkToolbarChildType type;
  char *text, *tooltip, *tip_private;
  GtkWidget *widget = NULL, *ret;
  PyObject *py_widget, *callback;
  int pos;
  if (!PyArg_ParseTuple(args, "O!iOzzzO!Oi:gtk_toolbar_insert_element",
			&PyGtk_Type, &t, &type, &py_widget, &text,
			&tooltip, &tip_private,	&PyGtk_Type, &icon,
			&callback, &pos))
    return NULL;
  if (PyGtk_Check(py_widget))
    widget = GTK_WIDGET(PyGtk_Get(py_widget));
  else if (py_widget != Py_None) {
    PyErr_SetString(PyExc_TypeError, "third argument not a GtkWidget or None");
    return NULL;
  }
  if (!PyCallable_Check(callback) && callback != Py_None) {
    PyErr_SetString(PyExc_TypeError, "seventh argument not callable");
    return NULL;
  }
  if (callback != Py_None) Py_INCREF(callback);
  ret = gtk_toolbar_insert_element(GTK_TOOLBAR(PyGtk_Get(t)), type, widget,
				   text, tooltip, tip_private,
				   GTK_WIDGET(PyGtk_Get(icon)), NULL, NULL,
				   pos);
  if (callback != Py_None) {
    gtk_signal_connect_full(GTK_OBJECT(ret), "clicked", NULL,
			    PyGtk_CallbackMarshal, callback,
			    PyGtk_DestroyNotify, FALSE, FALSE);
  }
  return PyGtk_New((GtkObject *)ret);
}

static PyObject *_wrap_gtk_drag_dest_set(PyObject *self, PyObject *args) {
  PyObject *widget, *py_flags, *py_actions, *py_list;
  GtkDestDefaults flags;
  GdkDragAction actions;
  GtkTargetEntry *targets;
  gint n_targets, i;

  if (!PyArg_ParseTuple(args, "O!OOO:gtk_drag_dest_set", &PyGtk_Type, &widget,
			&py_flags, &py_list, &py_actions))
    return NULL;
  if (PyGtkFlag_get_value(GTK_TYPE_DEST_DEFAULTS, py_flags, (gint *)&flags))
    return NULL;
  if (PyGtkFlag_get_value(GTK_TYPE_GDK_DRAG_ACTION,py_actions,(gint*)&actions))
    return NULL;
  if (!PySequence_Check(py_list)) {
    PyErr_SetString(PyExc_TypeError, "third argument must be a sequence");
    return NULL;
  }
  n_targets = PySequence_Length(py_list);
  targets = g_new(GtkTargetEntry, n_targets);
  for (i = 0; i < n_targets; i++) {
    PyObject *item = PySequence_GetItem(py_list, i);
    Py_DECREF(item);
    if (!PyArg_ParseTuple(item, "zii", &(targets[i].target),
			  &(targets[i].flags), &(targets[i].info))) {
      PyErr_Clear();
      PyErr_SetString(PyExc_TypeError,"list items should be (string,int,int)");
      g_free(targets);
      return NULL;
    }
  }
  gtk_drag_dest_set(GTK_WIDGET(PyGtk_Get(widget)), flags, targets, n_targets,
		    actions);
  g_free(targets);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *_wrap_gtk_drag_source_set(PyObject *self, PyObject *args) {
  PyObject *widget, *py_bmask, *py_list, *py_actions;
  GdkModifierType bmask;
  GtkTargetEntry *targets;
  gint n_targets, i;
  GdkDragAction actions;

  if (!PyArg_ParseTuple(args, "O!OOO:gtk_drag_source_set", &PyGtk_Type,
			&widget, &py_bmask, &py_list, &py_actions))
    return NULL;
  if (PyGtkFlag_get_value(GTK_TYPE_GDK_MODIFIER_TYPE, py_bmask, (gint*)&bmask))
    return NULL;
  if (PyGtkFlag_get_value(GTK_TYPE_GDK_DRAG_ACTION,py_actions,(gint*)&actions))
    return NULL;
  if (!PySequence_Check(py_list)) {
    PyErr_SetString(PyExc_TypeError, "third argument must be a sequence");
    return NULL;
  }
  n_targets = PySequence_Length(py_list);
  targets = g_new(GtkTargetEntry, n_targets);
  for (i = 0; i < n_targets; i++) {
    PyObject *item = PySequence_GetItem(py_list, i);
    Py_DECREF(item);
    if (!PyArg_ParseTuple(item, "zii", &(targets[i].target),
			  &(targets[i].flags), &(targets[i].info))) {
      PyErr_Clear();
      PyErr_SetString(PyExc_TypeError,"list items should be (string,int,int)");
      g_free(targets);
      return NULL;
    }
  }
  gtk_drag_source_set(GTK_WIDGET(PyGtk_Get(widget)), bmask, targets, n_targets,
		      actions);
  g_free(targets);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *_wrap_gtk_drag_get_source_widget(PyObject *self, PyObject *args) {
    PyObject *context;
    GtkWidget *ret;

    if (!PyArg_ParseTuple(args, "O!:gtk_drag_get_source_widget", &PyGdkDragContext_Type, &context))
        return NULL;
    ret = gtk_drag_get_source_widget(PyGdkDragContext_Get(context));
    if (ret)
      return PyGtk_New((GtkObject *)ret);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_drag_begin(PyObject *self, PyObject *args) {
  PyObject *widget, *py_list, *py_actions, *event;
  GdkDragAction actions;
  gint button, n_targets, i;
  GtkTargetEntry *targets;
  GtkTargetList *list;
  GdkDragContext *context;

  if (!PyArg_ParseTuple(args, "O!OOiO!:gtk_drag_begin", &PyGtk_Type, &widget,
			&py_list, &py_actions, &button,
			&PyGdkEvent_Type, &event))
    return NULL;
  if (PyGtkFlag_get_value(GTK_TYPE_GDK_DRAG_ACTION,py_actions,(gint*)&actions))
    return NULL;
  if (!PySequence_Check(py_list)) {
    PyErr_SetString(PyExc_TypeError, "third argument must be a sequence");
    return NULL;
  }
  n_targets = PySequence_Length(py_list);
  targets = g_new(GtkTargetEntry, n_targets);
  for (i = 0; i < n_targets; i++) {
    PyObject *item = PySequence_GetItem(py_list, i);
    Py_DECREF(item);
    if (!PyArg_ParseTuple(item, "zii", &(targets[i].target),
			  &(targets[i].flags), &(targets[i].info))) {
      PyErr_Clear();
      PyErr_SetString(PyExc_TypeError,"list items should be (string,int,int)");
      g_free(targets);
      return NULL;
    }
  }
  list = gtk_target_list_new(targets, n_targets);
  g_free(targets);
  context = gtk_drag_begin(GTK_WIDGET(PyGtk_Get(widget)), list, actions,
			   button, PyGdkEvent_Get(event));
  gtk_target_list_unref(list);
  return PyGdkDragContext_New(context);
}

#include "gtkmodule_impl.c"

static PyObject *_wrap_gdk_char_width(PyObject *self, PyObject *args) {
  PyObject *font;
  char ch;
  
  if (!PyArg_ParseTuple(args, "O!c:gdk_char_width", &PyGdkFont_Type, &font, &ch))
    return NULL;

  return Py_BuildValue("i", gdk_char_width(PyGdkFont_Get(font), ch));
}

static PyObject *_wrap_gdk_flush(PyObject *self, PyObject *args) {
  if (!PyArg_ParseTuple(args, ":gdk_flush"))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  gdk_flush ();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *_wrap_gdk_beep(PyObject *self, PyObject *args) {
  if (!PyArg_ParseTuple(args, ":gdk_beep"))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  gdk_beep ();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *_wrap_gdk_pixmap_new(PyObject *self, PyObject *args) {
  GdkPixmap *pix;
  GdkWindow *win = NULL;
  PyObject *py_win, *ret;
  int w, h, d;
  if (!PyArg_ParseTuple(args, "Oiii:gdk_pixmap_new", &py_win, &w, &h, &d))
    return NULL;
  if (PyGdkWindow_Check(py_win)) win = PyGdkWindow_Get(py_win);
  else if (py_win != Py_None) {
    PyErr_SetString(PyExc_TypeError,
		    "first argument must be a GdkWindow or None");
    return NULL;
  }
  pix = gdk_pixmap_new(win, w, h, d);
  if (!pix) {
    PyErr_SetString(PyExc_ValueError, "invalid parameters");
    return NULL;
  }
  ret = PyGdkWindow_New(pix);
  gdk_pixmap_unref(pix);
  return ret;
}

static PyObject *_wrap_gdk_pixmap_create_from_xpm(PyObject *self, PyObject *args) {
  GdkPixmap *pix;
  GdkBitmap *mask;
  PyObject *parent_win, *colour, *ret;
  GdkColor *col = NULL;
  gchar *fname;

  if (!PyArg_ParseTuple(args, "O!Os:gdk_pixmap_create_from_xpm",
			&PyGdkWindow_Type, &parent_win, &colour, &fname))
    return NULL;
  if (PyGdkColor_Check(colour)) col = PyGdkColor_Get(colour);
  else if (colour != Py_None) {
    PyErr_SetString(PyExc_TypeError,
		    "second argument must be a colour or None");
    return NULL;
  }
  pix = gdk_pixmap_create_from_xpm(PyGdkWindow_Get(parent_win), &mask, col,
				   fname);
  if (pix == NULL) {
    PyErr_SetString(PyExc_IOError, "can't load pixmap");
    return NULL;
  }
  ret = Py_BuildValue("(NN)", PyGdkWindow_New(pix), PyGdkWindow_New(mask));
  gdk_pixmap_unref(pix);
  gdk_bitmap_unref(mask);
  return ret;
}

static PyObject *_wrap_gdk_pixmap_create_from_xpm_d(PyObject *self, PyObject *args) {
  GdkPixmap *pix;
  GdkBitmap *mask;
  PyObject *parent_win, *colour, *ret, *py_lines;
  GdkColor *col = NULL;
  int len, i;
  char **data;

  if (!PyArg_ParseTuple(args, "O!OO!:gdk_pixmap_create_from_xpm_d",
			&PyGdkWindow_Type, &parent_win, &colour,
			&PyList_Type, &py_lines))
    return NULL;
  if (PyGdkColor_Check(colour)) col = PyGdkColor_Get(colour);
  else if (colour != Py_None) {
    PyErr_SetString(PyExc_TypeError,
		    "second argument must be a colour or None");
    return NULL;
  }
  len = PyList_Size(py_lines);
  data = g_new(char *, len);
  for (i = 0; i < len; i++) {
    PyObject *item = PyList_GetItem(py_lines, i);
    if (!PyString_Check(item)) {
      PyErr_SetString(PyExc_TypeError, "list items must be strings");
      g_free(data);
      return NULL;
    }
    data[i] = PyString_AsString(item);
  }
  pix = gdk_pixmap_create_from_xpm_d(PyGdkWindow_Get(parent_win), &mask, col,
				     data);
  g_free(data);
  if (pix == NULL) {
    PyErr_SetString(PyExc_IOError, "can't load pixmap");
    return NULL;
  }
  ret = Py_BuildValue("(NN)", PyGdkWindow_New(pix), PyGdkWindow_New(mask));
  gdk_pixmap_unref(pix);
  gdk_bitmap_unref(mask);
  return ret;
}

static PyObject *_wrap_gdk_pixmap_colormap_create_from_xpm(PyObject *self, PyObject *args) {
  GdkPixmap *pix;
  GdkBitmap *mask;
  GdkWindow *win = NULL;
  GdkColor *colour = NULL;
  GdkColormap *cmap = NULL;
  PyObject *py_win, *py_colour, *py_cmap, *ret;
  gchar *fname;

  if (!PyArg_ParseTuple(args, "OOOs:gdk_pixmap_colormap_create_from_xpm",
			&py_win, &py_cmap, &py_colour, &fname))
    return NULL;
  if (PyGdkWindow_Check(py_win))   win = PyGdkWindow_Get(py_win);
  else if (py_win != Py_None) {
    PyErr_SetString(PyExc_TypeError,
		    "first argument must be a GdkWindow or None");
    return NULL;
  }
  if (PyGdkColormap_Check(py_cmap)) cmap = PyGdkColormap_Get(py_cmap);
  else if (py_cmap != Py_None) {
    PyErr_SetString(PyExc_TypeError,
		    "second argument must be a GdkColourmap or None");
    return NULL;
  }
  if (PyGdkColor_Check(py_colour)) colour = PyGdkColor_Get(py_colour);
  else if (py_colour != Py_None) {
    PyErr_SetString(PyExc_TypeError,
		    "third argument must be a GdkColour or None");
    return NULL;
  }
  pix = gdk_pixmap_colormap_create_from_xpm(win, cmap, &mask, colour, fname);
  if (pix == NULL) {
    PyErr_SetString(PyExc_IOError, "can't load pixmap");
    return NULL;
  }
  ret = Py_BuildValue("(NN)", PyGdkWindow_New(pix), PyGdkWindow_New(mask));
  gdk_pixmap_unref(pix);
  gdk_bitmap_unref(mask);
  return ret;
}

static PyObject *_wrap_gdk_pixmap_colormap_create_from_xpm_d(PyObject *self, PyObject *args) {
  GdkPixmap *pix;
  GdkBitmap *mask;
  GdkWindow *win = NULL;
  GdkColormap *cmap = NULL;
  GdkColor *colour = NULL;
  PyObject *py_win, *py_cmap, *py_colour, *ret, *py_lines;
  int len, i;
  char **data;

  if (!PyArg_ParseTuple(args, "OOOO!:gdk_pixmap_colormap_create_from_xpm_d",
			&py_win, &py_cmap, &py_colour, &PyList_Type,&py_lines))
    return NULL;
  if (PyGdkWindow_Check(py_win))   win = PyGdkWindow_Get(py_win);
  else if (py_win != Py_None) {
    PyErr_SetString(PyExc_TypeError,
		    "first argument must be a GdkWindow or None");
    return NULL;
  }
  if (PyGdkColormap_Check(py_cmap)) cmap = PyGdkColormap_Get(py_cmap);
  else if (py_cmap != Py_None) {
    PyErr_SetString(PyExc_TypeError,
		    "second argument must be a GdkColourmap or None");
    return NULL;
  }
  if (PyGdkColor_Check(py_colour)) colour = PyGdkColor_Get(py_colour);
  else if (py_colour != Py_None) {
    PyErr_SetString(PyExc_TypeError,
		    "third argument must be a GdkColour or None");
    return NULL;
  }
  len = PyList_Size(py_lines);
  data = g_new(char *, len);
  for (i = 0; i < len; i++) {
    PyObject *item = PyList_GetItem(py_lines, i);
    if (!PyString_Check(item)) {
      PyErr_SetString(PyExc_TypeError, "list items must be strings");
      g_free(data);
      return NULL;
    }
    data[i] = PyString_AsString(item);
  }
  pix = gdk_pixmap_colormap_create_from_xpm_d(win, cmap, &mask, colour, data);
  g_free(data);
  if (pix == NULL) {
    PyErr_SetString(PyExc_IOError, "can't load pixmap");
    return NULL;
  }
  ret = Py_BuildValue("(NN)", PyGdkWindow_New(pix), PyGdkWindow_New(mask));
  gdk_pixmap_unref(pix);
  gdk_bitmap_unref(mask);
  return ret;
}

static PyObject *_wrap_gdk_bitmap_create_from_data(PyObject *self, PyObject *args) {
  PyObject *window;
  char *data;
  int length, width, height;
  GdkBitmap *bitmap;

  if (!PyArg_ParseTuple(args, "O!s#ii:gdk_bitmap_create_from_data",
			&PyGdkWindow_Type, &window, &data, &length,
			&width, &height))
    return NULL;
  bitmap = gdk_bitmap_create_from_data(PyGdkWindow_Get(window), data, width,
				       height);
  if (bitmap) {
    PyObject *ret = PyGdkWindow_New(bitmap);
    gdk_bitmap_unref(bitmap);
    return ret;
  }
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *_wrap_gdk_font_load(PyObject *self, PyObject *args) {
  gchar *name;
  GdkFont *font;
  PyObject *ret;

  if (!PyArg_ParseTuple(args, "s:gdk_font_load", &name))
    return NULL;
  font = gdk_font_load(name);
  if (font) {
    ret = PyGdkFont_New(font);
    gdk_font_unref(font);
    return ret;
  } else {
    PyErr_SetString(PyExc_RuntimeError, "couldn't load the font");
    return NULL;
  }
}

static PyObject *_wrap_gdk_fontset_load(PyObject *self, PyObject *args) {
  gchar *name;
  GdkFont *font;
  PyObject *ret;

  if (!PyArg_ParseTuple(args, "s:gdk_fontset_load", &name))
    return NULL;
  font = gdk_fontset_load(name);
  if (font) {
    ret = PyGdkFont_New(font);
    gdk_font_unref(font);
    return ret;
  } else {
    PyErr_SetString(PyExc_RuntimeError, "couldn't load the font");
    return NULL;
  }
}

static PyObject *_wrap_gdk_draw_polygon(PyObject *self, PyObject *args) {
  PyObject *win, *gc, *seq, *item;
  gint filled, npoints, i;
  GdkPoint *points;

  if (!PyArg_ParseTuple(args, "O!O!iO:gdk_draw_polygon", &PyGdkWindow_Type,
			&win, &PyGdkGC_Type, &gc, &filled, &seq))
    return NULL;
  if (!PySequence_Check(seq)) {
    PyErr_SetString(PyExc_TypeError, "forth argument not a sequence");
    return NULL;
  }
  npoints = PySequence_Length(seq);
  points = g_new(GdkPoint, npoints);
  for (i = 0; i < npoints; i++) {
    item = PySequence_GetItem(seq, i);
    Py_DECREF(item);
    if (!PyArg_ParseTuple(item, "hh", &(points[i].x), &(points[i].y))) {
      PyErr_Clear();
      PyErr_SetString(PyExc_TypeError, "sequence member not a 2-tuple");
      g_free(points);
      return NULL;
    }
  }
  gdk_draw_polygon(PyGdkWindow_Get(win),PyGdkGC_Get(gc),filled,points,npoints);
  g_free(points);
  Py_INCREF(Py_None);
  return Py_None;
}
static PyObject *_wrap_gdk_draw_text(PyObject *self, PyObject *args) {
  PyObject *win, *font, *gc;
  int x, y, len;
  char *str;
  if (!PyArg_ParseTuple(args, "O!O!O!iis#:gdk_draw_text", &PyGdkWindow_Type,
			&win, &PyGdkFont_Type, &font, &PyGdkGC_Type, &gc,
			&x, &y, &str, &len))
    return NULL;
  gdk_draw_text(PyGdkWindow_Get(win), PyGdkFont_Get(font), PyGdkGC_Get(gc),
		x, y, str, len);
  Py_INCREF(Py_None);
  return Py_None;
}
static PyObject *_wrap_gdk_draw_points(PyObject *self, PyObject *args) {
  PyObject *win, *gc, *seq, *item;
  gint npoints, i;
  GdkPoint *points;

  if (!PyArg_ParseTuple(args, "O!O!O:gdk_draw_points", &PyGdkWindow_Type,
			&win, &PyGdkGC_Type, &gc, &seq))
    return NULL;
  if (!PySequence_Check(seq)) {
    PyErr_SetString(PyExc_TypeError, "third argument not a sequence");
    return NULL;
  }
  npoints = PySequence_Length(seq);
  points = g_new(GdkPoint, npoints);
  for (i = 0; i < npoints; i++) {
    item = PySequence_GetItem(seq, i);
    Py_DECREF(item);
    if (!PyArg_ParseTuple(item, "hh", &(points[i].x), &(points[i].y))) {
      PyErr_Clear();
      PyErr_SetString(PyExc_TypeError, "sequence member not a 2-tuple");
      g_free(points);
      return NULL;
    }
  }
  gdk_draw_points(PyGdkWindow_Get(win), PyGdkGC_Get(gc), points, npoints);
  g_free(points);
  Py_INCREF(Py_None);
  return Py_None;
}
static PyObject *_wrap_gdk_draw_segments(PyObject *self, PyObject *args) {
  PyObject *win, *gc, *seq, *item;
  gint nsegs, i;
  GdkSegment *segs;

  if (!PyArg_ParseTuple(args, "O!O!O:gdk_draw_segments", &PyGdkWindow_Type,
			&win, &PyGdkGC_Type, &gc, &seq))
    return NULL;
  if (!PySequence_Check(seq)) {
    PyErr_SetString(PyExc_TypeError, "third argument not a sequence");
    return NULL;
  }
  nsegs = PySequence_Length(seq);
  segs = g_new(GdkSegment, nsegs);
  for (i = 0; i < nsegs; i++) {
    item = PySequence_GetItem(seq, i);
    Py_DECREF(item);
    if (!PyArg_ParseTuple(item, "hhhh", &(segs[i].x1), &(segs[i].y1),
			  &(segs[i].x2), &(segs[i].y2))) {
      PyErr_Clear();
      PyErr_SetString(PyExc_TypeError, "sequence member not a 4-tuple");
      g_free(segs);
      return NULL;
    }
  }
  gdk_draw_segments(PyGdkWindow_Get(win), PyGdkGC_Get(gc), segs, nsegs);
  g_free(segs);
  Py_INCREF(Py_None);
  return Py_None;
}
static PyObject *_wrap_gdk_draw_lines(PyObject *self, PyObject *args) {
  PyObject *win, *gc, *seq, *item;
  gint npoints, i;
  GdkPoint *points;

  if (!PyArg_ParseTuple(args, "O!O!O:gdk_draw_lines", &PyGdkWindow_Type,
			&win, &PyGdkGC_Type, &gc, &seq))
    return NULL;
  if (!PySequence_Check(seq)) {
    PyErr_SetString(PyExc_TypeError, "third argument not a sequence");
    return NULL;
  }
  npoints = PySequence_Length(seq);
  points = g_new(GdkPoint, npoints);
  for (i = 0; i < npoints; i++) {
    item = PySequence_GetItem(seq, i);
    Py_DECREF(item);
    if (!PyArg_ParseTuple(item, "hh", &(points[i].x), &(points[i].y))) {
      PyErr_Clear();
      PyErr_SetString(PyExc_TypeError, "sequence member not a 2-tuple, or coordinates out of range");
      g_free(points);
      return NULL;
    }
  }
  gdk_draw_lines(PyGdkWindow_Get(win), PyGdkGC_Get(gc), points, npoints);
  g_free(points);
  Py_INCREF(Py_None);
  return Py_None;
}
/* gdk_color_alloc analog */
static PyObject *_wrap_gdk_color_alloc(PyObject *self, PyObject *args) {
    GdkColor gdk_color;
    GdkColormap *colormap;
    PyGtkStyle_Object *style;
    PyGtk_Object *obj;
    if (PyArg_ParseTuple(args, "O!hhh:gdk_color_alloc", &PyGtkStyle_Type,
			 &style, &(gdk_color.red), &(gdk_color.green),
			 &(gdk_color.blue)))
        colormap = PyGtkStyle_Get(style)->colormap;
    else {
        PyErr_Clear();
	if (!PyArg_ParseTuple(args, "O!hhh:gdk_color_alloc", &PyGtk_Type,
			      &obj, &(gdk_color.red), &(gdk_color.green),
			      &(gdk_color.blue)))
	    return NULL;
	if (GTK_WIDGET_NO_WINDOW(PyGtk_Get(obj))) {
	    PyErr_SetString(PyExc_TypeError, "widget has no window");
	    return NULL;
	}
	colormap = gdk_window_get_colormap(GTK_WIDGET(PyGtk_Get(obj))->window);
    }
    gdk_color_alloc(colormap, &gdk_color);
    return PyGdkColor_New(&gdk_color);
}

static PyObject *_wrap_gdk_color_new(PyObject *self, PyObject *args) {
    glong red, blue, green;
    GdkColor gdk_color;
    
    if (!PyArg_ParseTuple(args, "llll:gdk_color_new", 
			  &red, &green, blue, &(gdk_color.pixel)))
	return NULL;

    g_warning("you should be using GdkColormap.alloc() to allocate colours");

    gdk_color.red = red;
    gdk_color.blue = blue;
    gdk_color.green = green;

    return PyGdkColor_New(&gdk_color);
}

#ifdef WITH_XSTUFF
static PyObject *_wrap_gdk_window_foreign_new(PyObject *self, PyObject *args) {
    guint32 winid;
    GdkWindow *window;

    if (!PyArg_ParseTuple(args, "i:gdk_window_foreign_new", &winid))
	return NULL;

    window = gdk_window_foreign_new(winid);
    if (window)
	return PyGdkWindow_New(window);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_get_root_win(PyObject *self, PyObject *args) {
    if (!PyArg_ParseTuple(args, ":gdk_get_root_win"))
	return NULL;
    return PyGdkWindow_New(gdk_window_foreign_new(GDK_ROOT_WINDOW()));
}
#endif

static PyObject *_wrap_gtk_label_get(PyObject *self, PyObject *args) {
    PyObject *label;
    char *text;

    if (!PyArg_ParseTuple(args, "O!:gtk_label_get", &PyGtk_Type, &label))
        return NULL;
    gtk_label_get(GTK_LABEL(PyGtk_Get(label)), &text);
    return PyString_FromString(text);
}

static PyObject *_wrap_gtk_color_selection_get_color(PyObject *self, PyObject *args) {
    PyObject *colorsel;
    double value[4];

    if (!PyArg_ParseTuple(args, "O!:gtk_color_selection_get_color", &PyGtk_Type, &colorsel))
        return NULL;
    gtk_color_selection_get_color(GTK_COLOR_SELECTION(PyGtk_Get(colorsel)),
				  value);
    if (GTK_COLOR_SELECTION(PyGtk_Get(colorsel))->use_opacity)
        return Py_BuildValue("(dddd)", value[0],value[1],value[2],value[3]);
    else
        return Py_BuildValue("(ddd)", value[0], value[1], value[2]);
}
    
static PyObject *_wrap_gtk_color_selection_set_color(PyObject *self, PyObject *args) {
    PyObject *colorsel;
    double value[4];

    value[3] = 1.0;
    if (!PyArg_ParseTuple(args, "O!(ddd):gtk_color_selection_set_color",
			  &PyGtk_Type, &colorsel, &value[0], &value[1],
			  &value[2])) {
        PyErr_Clear();
        if (!PyArg_ParseTuple(args, "O!(dddd):gtk_color_selection_set_color",
			      &PyGtk_Type, &colorsel, &value[0], &value[1],
			      &value[2], &value[3]))
	    return NULL;
    }
    gtk_color_selection_set_color(GTK_COLOR_SELECTION(PyGtk_Get(colorsel)),
				  value);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static GList *GList_FromPyList(PyObject *list) {
    GList *ret = NULL;
    PyObject *item;
    int i, len;

    len = PyList_Size(list);
    for (i = 0; i < len; i++) {
        item = PyList_GetItem(list, i);
        if (!PyGtk_Check(item)) {
	    PyErr_SetString(PyExc_TypeError, "list item not a GtkObject");
	    g_list_free(ret);
	    return NULL;
	}
	ret = g_list_append(ret, PyGtk_Get(item));
    }
    return ret;
}

static PyObject *_wrap_gtk_list_insert_items(PyObject *self, PyObject *args) {
    PyObject *list, *py_items;
    GList *items = NULL;
    int pos;

    if (!PyArg_ParseTuple(args, "O!O!i:gtk_list_insert_items", &PyGtk_Type,
			  &list, &PyList_Type, &py_items, &pos))
        return NULL;
    if (PyList_Size(py_items) && (items = GList_FromPyList(py_items)) == NULL)
      return NULL;
    gtk_list_insert_items(GTK_LIST(PyGtk_Get(list)), items, pos);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_list_append_items(PyObject *self, PyObject *args) {
    PyObject *list, *py_items;
    GList *items = NULL;

    if (!PyArg_ParseTuple(args, "O!O!:gtk_list_append_items", &PyGtk_Type,
			  &list, &PyList_Type, &py_items))
        return NULL;
    if (PyList_Size(py_items) && (items = GList_FromPyList(py_items)) == NULL)
      return NULL;
    gtk_list_append_items(GTK_LIST(PyGtk_Get(list)), items);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_list_prepend_items(PyObject *self, PyObject *args) {
    PyObject *list, *py_items;
    GList *items = NULL;

    if (!PyArg_ParseTuple(args, "O!O!:gtk_list_prepend_items", &PyGtk_Type,
			  &list, &PyList_Type, &py_items))
        return NULL;
    if (PyList_Size(py_items) && (items = GList_FromPyList(py_items)) == NULL)
      return NULL;
    gtk_list_prepend_items(GTK_LIST(PyGtk_Get(list)), items);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_list_remove_items(PyObject *self, PyObject *args) {
    PyObject *list, *py_items;
    GList *items = NULL;

    if (!PyArg_ParseTuple(args, "O!O!:gtk_list_remove_items", &PyGtk_Type,
			  &list, &PyList_Type, &py_items))
        return NULL;
    if (PyList_Size(py_items) && (items = GList_FromPyList(py_items)) == NULL)
      return NULL;
    gtk_list_remove_items(GTK_LIST(PyGtk_Get(list)), items);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_list_get_selection(PyObject *self, PyObject *args) {
    PyObject *list, *ret;
    GList *tmp;

    if (!PyArg_ParseTuple(args, "O!:gtk_list_get_selection",&PyGtk_Type,&list))
        return NULL;
    tmp = GTK_LIST(PyGtk_Get(list))->selection;
    if ((ret = PyList_New(0)) == NULL)
      return NULL;
    for (; tmp; tmp = tmp->next) {
        PyObject *obj = PyGtk_New(tmp->data);
	if (obj == NULL) {
	  Py_DECREF(ret);
	  return NULL;
	}
        PyList_Append(ret, obj);
	Py_DECREF(obj);
    }
    return ret;
}

static PyObject *_wrap_gtk_tree_remove_items(PyObject *self, PyObject *args) {
    PyObject *tree, *py_items;
    GList *items = NULL;

    if (!PyArg_ParseTuple(args, "O!O!:gtk_tree_remove_items", &PyGtk_Type,
			  &tree, &PyList_Type, &py_items))
        return NULL;
    if (PyList_Size(py_items) && (items = GList_FromPyList(py_items)) == NULL)
      return NULL;
    gtk_tree_remove_items(GTK_TREE(PyGtk_Get(tree)), items);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_tree_get_selection(PyObject *self, PyObject *args) {
    PyObject *tree, *ret;
    GList *tmp;

    if (!PyArg_ParseTuple(args, "O!:gtk_tree_get_selection",&PyGtk_Type,&tree))
        return NULL;
    tmp = GTK_TREE(PyGtk_Get(tree))->selection;
    if ((ret = PyList_New(0)) == NULL)
      return NULL;
    for (; tmp; tmp = tmp->next) {
        PyObject *obj = PyGtk_New(tmp->data);
	if (obj == NULL) {
	  Py_DECREF(ret);
	  return NULL;
	}
        PyList_Append(ret, obj);
	Py_DECREF(obj);
    }	
    return ret;
}

static PyObject *_wrap_gtk_calendar_get_date(PyObject *self, PyObject *args) {
  PyObject *cal;
  guint year, month, day;

  if (!PyArg_ParseTuple(args, "O!:gtk_calendar_get_date", &PyGtk_Type, &cal))
    return NULL;
  gtk_calendar_get_date(GTK_CALENDAR(PyGtk_Get(cal)), &year, &month, &day);
  return Py_BuildValue("(iii)", year, month, day);
}

static PyObject *_wrap_gtk_notebook_query_tab_label_packing(PyObject *self, PyObject *args) {
  PyObject *notebook, *child;
  gboolean expand, fill;
  GtkPackType pack_type;

  if (!PyArg_ParseTuple(args, "O!O!:gtk_notebook_query_tab_label_packing",
			&PyGtk_Type, &notebook, &PyGtk_Type, &child))
    return NULL;
  gtk_notebook_query_tab_label_packing(GTK_NOTEBOOK(PyGtk_Get(notebook)),
				       GTK_WIDGET(PyGtk_Get(child)),
				       &expand, &fill, &pack_type);
  return Py_BuildValue("(iii)", (int)expand, (int)fill, pack_type);
}

static PyObject *_wrap_gtk_ctree_new_with_titles(PyObject *self, PyObject *args) {
  int c, tree_col, i;
  PyObject *l, *item;
  char **list;
  if (!PyArg_ParseTuple(args, "iiO:gtk_ctree_new_with_titles",
			&c, &tree_col, &l))
    return NULL;
  if (!PySequence_Check(l)) {
    PyErr_SetString(PyExc_TypeError,"3rd argument not a sequence");
    return NULL;
  }
  if (PySequence_Length(l) < c) {
    PyErr_SetString(PyExc_TypeError, "sequence not long enough");
    return NULL;
  }
  list = malloc(sizeof(char *)*c);
  for (i = 0; i < c; i++) {
    item = PySequence_GetItem(l, i);
    Py_DECREF(item);
    if (!PyString_Check(item)) {
      PyErr_SetString(PyExc_TypeError,
		      "sequence item not a string");
      free(list);
      return NULL;
    }
    list[i] = PyString_AsString(item);
  }
  item = (PyObject *) PyGtk_New((GtkObject *)gtk_ctree_new_with_titles(
						c, tree_col, list));
  free(list);
  return item;
}

static PyObject *_wrap_gtk_ctree_get_selection(PyObject *self, PyObject *args) {
  GList *selection;
  GtkCTreeNode *node;
  PyObject *clist, *ret, *py_node;
  if (!PyArg_ParseTuple(args, "O!:gtk_ctree_get_selection", &PyGtk_Type,
			&clist))
    return NULL;
  if ((ret = PyList_New(0)) == NULL)
    return NULL;

  for (selection = GTK_CLIST(PyGtk_Get(clist))->selection; selection != NULL;
       selection = selection->next) {
    node = selection->data;
    if ((py_node = PyGtkCTreeNode_New(node)) == NULL) {
      Py_DECREF(ret);
      return NULL;
    }
    PyList_Append(ret, py_node);
    Py_DECREF(py_node);
  }
  return ret;
}

static PyObject *_wrap_gtk_ctree_insert_node(PyObject *self, PyObject *args) {
  PyObject *ctree, *py_parent, *py_sibling, *py_text;
  PyObject *py_pixmap_closed, *py_mask_closed;
  PyObject *py_pixmap_opened, *py_mask_opened;
  GtkCTreeNode *parent = NULL, *sibling = NULL, *ret;
  gchar **text = NULL;
  GdkPixmap *pixmap_closed = NULL, *pixmap_opened = NULL;
  GdkBitmap *mask_closed = NULL, *mask_opened = NULL;
  gint spacing, is_leaf, expanded, col, i;

  /* just about all the arguments can be substituted with None */
  if (!PyArg_ParseTuple(args, "O!OOOiOOOOii:gtk_ctree_insert_node",
			&PyGtk_Type, &ctree, &py_parent, &py_sibling, &py_text,
			&spacing, &py_pixmap_closed, &py_mask_closed,
			&py_pixmap_opened, &py_mask_opened, &is_leaf,
			&expanded))
    return NULL;
  if (PyGtkCTreeNode_Check(py_parent))
    parent = PyGtkCTreeNode_Get(py_parent);
  else if (py_parent != Py_None) {
    PyErr_SetString(PyExc_TypeError, "2nd arg must be a CTreeNode or None");
    return NULL;
  }
  if (PyGtkCTreeNode_Check(py_sibling))
    sibling = PyGtkCTreeNode_Get(py_sibling);
  else if (py_sibling != Py_None) {
    PyErr_SetString(PyExc_TypeError, "3rd arg must be a CTreeNode or None");
    return NULL;
  }
  if (PyGdkWindow_Check(py_pixmap_closed))
    pixmap_closed = PyGdkWindow_Get(py_pixmap_closed);
  else if (py_pixmap_closed != Py_None) {
    PyErr_SetString(PyExc_TypeError, "6th arg must be a GdkWindow or None");
    return NULL;
  }
  if (PyGdkWindow_Check(py_mask_closed))
    mask_closed = PyGdkWindow_Get(py_mask_closed);
  else if (py_mask_closed != Py_None) {
    PyErr_SetString(PyExc_TypeError, "7th arg must be a GdkWindow or None");
    return NULL;
  }
  if (PyGdkWindow_Check(py_pixmap_opened))
    pixmap_opened = PyGdkWindow_Get(py_pixmap_opened);
  else if (py_pixmap_opened != Py_None) {
    PyErr_SetString(PyExc_TypeError, "8th arg must be a GdkWindow or None");
    return NULL;
  }
  if (PyGdkWindow_Check(py_mask_opened))
    mask_opened = PyGdkWindow_Get(py_mask_opened);
  else if (py_mask_opened != Py_None) {
    PyErr_SetString(PyExc_TypeError, "9th arg must be a GdkWindow or None");
    return NULL;
  }
  if (!PySequence_Check(py_text)) {
    PyErr_SetString(PyExc_TypeError, "4th arg must be a sequence");
    return NULL;
  }
  col = GTK_CLIST(PyGtk_Get(ctree))->columns;
  if (PySequence_Length(py_text) < col) {
    PyErr_SetString(PyExc_TypeError, "sequence too short");
    return NULL;
  }
  text = g_new(gchar *, col);
  for (i = 0; i < col; i++) {
    PyObject *item = PySequence_GetItem(py_text, i);
    Py_DECREF(item);
    if (!PyString_Check(item)) {
      PyErr_SetString(PyExc_TypeError, "sequence item not a string");
      g_free(text);
      return NULL;
    }
    text[i] = PyString_AsString(item);
  }
  ret = gtk_ctree_insert_node(GTK_CTREE(PyGtk_Get(ctree)), parent, sibling,
			      text, spacing, pixmap_closed, mask_closed,
			      pixmap_opened, mask_opened, is_leaf, expanded);
  g_free(text);
  return PyGtkCTreeNode_New(ret);
}

static PyObject *_wrap_gtk_ctree_find_by_row_data(PyObject *self, PyObject *args) {
  PyObject *ctree, *py_node, *data;
  GtkCTreeNode *node = NULL, *ret;
  
  if (!PyArg_ParseTuple(args, "O!OO:gtk_ctree_find_by_row_data", &PyGtk_Type,
			&ctree, &py_node, &data))
    return NULL;
  if (PyGtkCTreeNode_Check(py_node))
    node = PyGtkCTreeNode_Get(py_node);
  else if (py_node != Py_None) {
    PyErr_SetString(PyExc_TypeError, "2nd arg must be a GTreeNode or None");
    return NULL;
  }
  ret = gtk_ctree_find_by_row_data(GTK_CTREE(PyGtk_Get(ctree)), node, data);
  if (ret)
    return PyGtkCTreeNode_New(ret);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *_wrap_gtk_ctree_find_all_by_row_data(PyObject *self, PyObject *args) {
  PyObject *ctree, *py_node, *data, *list;
  GtkCTreeNode *node = NULL;
  GList *ret, *tmp;
  
  if (!PyArg_ParseTuple(args, "O!OO:gtk_ctree_find_all_by_row_data",
			&PyGtk_Type, &ctree, &py_node, &data))
    return NULL;
  if (PyGtkCTreeNode_Check(py_node))
    node = PyGtkCTreeNode_Get(py_node);
  else if (py_node != Py_None) {
    PyErr_SetString(PyExc_TypeError, "2nd arg must be a GTreeNode or None");
    return NULL;
  }
  ret = gtk_ctree_find_all_by_row_data(GTK_CTREE(PyGtk_Get(ctree)), node,data);
  if ((list = PyList_New(0)) == NULL)
    return NULL;
  for (tmp = ret; tmp; tmp = tmp->next) {
    PyObject *obj = PyGtkCTreeNode_New(ret->data);
    if (obj == NULL) {
      Py_DECREF(list);
      return NULL;
    }
    PyList_Append(list, obj);
    Py_DECREF(obj);
  }
  g_list_free(ret);
  return list;
}

static PyObject *_wrap_gtk_ctree_move(PyObject *self, PyObject *args) {
  PyObject *ctree, *node, *py_parent, *py_sibling;
  GtkCTreeNode *parent = NULL, *sibling = NULL;

  if (!PyArg_ParseTuple(args, "O!O!OO:gtk_ctree_move", &PyGtk_Type, &ctree,
			&PyGtkCTreeNode_Type, &node, &py_parent, &py_sibling))
    return NULL;
  if (PyGtkCTreeNode_Check(py_parent))
    parent = PyGtkCTreeNode_Get(py_parent);
  else if (py_parent != Py_None) {
    PyErr_SetString(PyExc_TypeError, "3rd arg must be a CTreeNode or None");
    return NULL;
  }
  if (PyGtkCTreeNode_Check(py_sibling))
    sibling = PyGtkCTreeNode_Get(py_sibling);
  else if (py_sibling != Py_None) {
    PyErr_SetString(PyExc_TypeError, "4th arg must be a CTreeNode or None");
    return NULL;
  }
  gtk_ctree_move(GTK_CTREE(PyGtk_Get(ctree)), PyGtkCTreeNode_Get(node),
		 parent, sibling);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *_wrap_gtk_ctree_node_get_text(PyObject *self, PyObject *args) {
  PyObject *ctree, *node;
  int col;
  char *text;

  if (!PyArg_ParseTuple(args, "O!O!i:gtk_ctree_node_get_text", &PyGtk_Type,
			&ctree, &PyGtkCTreeNode_Type, &node, &col))
    return NULL;
  if (!gtk_ctree_node_get_text(GTK_CTREE(PyGtk_Get(ctree)),
			       PyGtkCTreeNode_Get(node), col, &text)) {
    PyErr_SetString(PyExc_ValueError, "can't get text value");
    return NULL;
  }
  return PyString_FromString(text);
}

static PyObject *_wrap_gtk_ctree_node_get_pixmap(PyObject *self, PyObject *args) {
  PyObject *ctree, *node, *pixmap, *mask;
  int col;
  GdkPixmap *p;
  GdkBitmap *m;

  if (!PyArg_ParseTuple(args, "O!O!i:gtk_ctree_node_get_pixmap", &PyGtk_Type,
			&ctree, &PyGtkCTreeNode_Type, &node, &col))
    return NULL;
  if (!gtk_ctree_node_get_pixmap(GTK_CTREE(PyGtk_Get(ctree)), 
				 PyGtkCTreeNode_Get(node), col, &p, &m)) {
    PyErr_SetString(PyExc_ValueError, "can't get pixmap value");
    return NULL;
  }
  if (p)
    pixmap = PyGdkWindow_New(p);
  else {
    Py_INCREF(Py_None);
    pixmap = Py_None;
  }
  if (m)
    mask = PyGdkWindow_New(m);
  else {
    Py_INCREF(Py_None);
    mask = Py_None;
  }
  return Py_BuildValue("(NN)", pixmap, mask);
}

static PyObject *_wrap_gtk_ctree_node_get_pixtext(PyObject *self, PyObject *args) {
  PyObject *ctree, *node, *pixmap, *mask;
  int col;
  guint8 spacing;
  char *text;
  GdkPixmap *p;
  GdkBitmap *m;

  if (!PyArg_ParseTuple(args, "O!O!i:gtk_ctree_node_get_pixtext", &PyGtk_Type,
			&ctree, &PyGtkCTreeNode_Type, &node, &col))
    return NULL;
  if (!gtk_ctree_node_get_pixtext(GTK_CTREE(PyGtk_Get(ctree)),
				  PyGtkCTreeNode_Get(node), col,
				  &text, &spacing, &p, &m)) {
    PyErr_SetString(PyExc_ValueError, "can't get pixtext value");
    return NULL;
  }
  if (p)
    pixmap = PyGdkWindow_New(p);
  else {
    Py_INCREF(Py_None);
    pixmap = Py_None;
  }
  if (m)
    mask = PyGdkWindow_New(m);
  else {
    Py_INCREF(Py_None);
    mask = Py_None;
  }
  return Py_BuildValue("(siNN)", text, (int)spacing, pixmap, mask);
}

static PyObject *_wrap_gtk_ctree_get_node_info(PyObject *self, PyObject *args) {
  PyObject *ctree, *node, *p_c, *m_c, *p_o, *m_o;
  gchar *text;
  guint8 spacing;
  GdkPixmap *pixmap_closed, *pixmap_opened;
  GdkBitmap *mask_closed, *mask_opened;
  gboolean is_leaf, expanded;

  if (!PyArg_ParseTuple(args, "O!O!:gtk_ctree_get_node_info", &PyGtk_Type,
			&ctree, &PyGtkCTreeNode_Type, &node))
    return NULL;
  if (!gtk_ctree_get_node_info(GTK_CTREE(PyGtk_Get(ctree)),
			       PyGtkCTreeNode_Get(node), &text, &spacing,
			       &pixmap_closed, &mask_closed, &pixmap_opened,
			       &mask_opened, &is_leaf, &expanded)) {
    PyErr_SetString(PyExc_ValueError, "can't retrieve node info");
    return NULL;
  }
  if (pixmap_closed)
    p_c = PyGdkWindow_New(pixmap_closed);
  else {
    Py_INCREF(Py_None);
    p_c = Py_None;
  }
  if (mask_closed)
    m_c = PyGdkWindow_New(mask_closed);
  else {
    Py_INCREF(Py_None);
    m_c = Py_None;
  }
  if (pixmap_opened)
    p_o = PyGdkWindow_New(pixmap_opened);
  else {
    Py_INCREF(Py_None);
    p_o = Py_None;
  }
  if (mask_opened)
    m_o = PyGdkWindow_New(mask_opened);
  else {
    Py_INCREF(Py_None);
    m_o = Py_None;
  }
  return Py_BuildValue("(siNNNNii)", text, (int)spacing, p_c, m_c, p_o, m_o,
		       (int)is_leaf, (int)expanded);
}

static PyObject *_wrap_gtk_ctree_node_set_row_data(PyObject *self, PyObject *args) {
  PyObject *ctree, *node, *data;

  if (!PyArg_ParseTuple(args, "O!O!O:gtk_ctree_node_set_row_data", &PyGtk_Type,
			&ctree, &PyGtkCTreeNode_Type, &node, &data))
    return NULL;
  Py_INCREF(data);
  gtk_ctree_node_set_row_data_full(GTK_CTREE(PyGtk_Get(ctree)),
				   PyGtkCTreeNode_Get(node), data,
				   (GtkDestroyNotify)PyGtk_DestroyNotify);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *_wrap_gtk_ctree_node_get_row_data(PyObject *self, PyObject *args) {
  PyObject *ctree, *node, *data;

  if (!PyArg_ParseTuple(args, "O!O!:gtk_ctree_node_get_row_data", &PyGtk_Type,
			&ctree, &PyGtkCTreeNode_Type, &node))
    return NULL;
  data = gtk_ctree_node_get_row_data(GTK_CTREE(PyGtk_Get(ctree)),
				     PyGtkCTreeNode_Get(node));
  if (!data) data = Py_None;
  Py_INCREF(data);
  return data;
}

static PyObject *_wrap_gtk_ctree_base_nodes(PyObject *self, PyObject *args) {
  PyObject *ctree, *ret;
  GtkCTreeNode *node;

  if (!PyArg_ParseTuple(args, "O!:gtk_ctree_base_nodes", &PyGtk_Type, &ctree))
    return NULL;
  /* the first row is always a base node */
  node = GTK_CTREE_NODE(GTK_CLIST(PyGtk_Get(ctree))->row_list);
  if ((ret = PyList_New(0)) == NULL)
    return NULL;
  while (node) {
    PyObject *obj = PyGtkCTreeNode_New(node);
    if (obj == NULL) {
      Py_DECREF(ret);
      return NULL;
    }
    PyList_Append(ret, obj);
    Py_DECREF(obj);
    node = GTK_CTREE_ROW(node)->sibling;
  }
  return ret;
}

static PyObject *_wrap_gdk_threads_enter(PyObject *self, PyObject *args) {
  if (!PyArg_ParseTuple(args, ":gdk_threads_enter"))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  gdk_threads_enter();
  Py_END_ALLOW_THREADS
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *_wrap_gdk_threads_leave(PyObject *self, PyObject *args) {
  if (!PyArg_ParseTuple(args, ":gdk_threads_leave"))
    return NULL;
  gdk_threads_leave();
  Py_INCREF(Py_None);
  return Py_None;
}

/* gdkrgb stuff -- will be needed when gdk_pixbuf stuff is added */

static PyObject *_wrap_gtk_rgb_push_visual(PyObject *self, PyObject *args) {
    if (!PyArg_ParseTuple(args, ":gtk_rgb_push_visual"))
	return NULL;
    gtk_widget_push_colormap(gdk_rgb_get_cmap());
    gtk_widget_push_visual(gdk_rgb_get_visual());
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gtk_pop_visual(PyObject *self, PyObject *args) {
    if (!PyArg_ParseTuple(args, ":gtk_pop_visual"))
	return NULL;
    gtk_widget_pop_colormap();
    gtk_widget_pop_visual();
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_draw_rgb_image(PyObject *self, PyObject *args) {
    PyObject *drawable, *gc, *py_dith;
    gint x, y, width, height, rowstride = -1, len;
    GdkRgbDither dith;
    guchar *rgb_buf;

    if (!PyArg_ParseTuple(args, "O!O!iiiiOs#|i:gdk_draw_rgb_image",
			  &PyGdkWindow_Type, &drawable, &PyGdkGC_Type, &gc,
			  &x, &y, &width, &height, &py_dith, &rgb_buf, &len,
			  &rowstride))
	return NULL;
    if (PyGtkEnum_get_value(GTK_TYPE_GDK_RGB_DITHER, py_dith, (gint *)&dith))
	return NULL;
    if (rowstride == -1)
	rowstride = width * 3;
    gdk_draw_rgb_image(PyGdkWindow_Get(drawable), PyGdkGC_Get(gc), x, y,
		       width, height, dith, rgb_buf, rowstride);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_draw_rgb_32_image(PyObject *self, PyObject *args) {
    PyObject *drawable, *gc, *py_dith;
    gint x, y, width, height, rowstride = -1, len;
    GdkRgbDither dith;
    guchar *rgb_buf;

    if (!PyArg_ParseTuple(args, "O!O!iiiiOs#|i:gdk_draw_rgb_32_image",
			  &PyGdkWindow_Type, &drawable, &PyGdkGC_Type, &gc,
			  &x, &y, &width, &height, &py_dith, &rgb_buf, &len,
			  &rowstride))
	return NULL;
    if (PyGtkEnum_get_value(GTK_TYPE_GDK_RGB_DITHER, py_dith, (gint *)&dith))
	return NULL;
    if (rowstride == -1)
	rowstride = width * 4;
    gdk_draw_rgb_32_image(PyGdkWindow_Get(drawable), PyGdkGC_Get(gc), x, y,
			  width, height, dith, rgb_buf, rowstride);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_gdk_draw_gray_image(PyObject *self, PyObject *args) {
    PyObject *drawable, *gc, *py_dith;
    gint x, y, width, height, rowstride = -1, len;
    GdkRgbDither dith;
    guchar *buf;

    if (!PyArg_ParseTuple(args, "O!O!iiiiOs#|i:gdk_draw_gray_image",
			  &PyGdkWindow_Type, &drawable, &PyGdkGC_Type, &gc,
			  &x, &y, &width, &height, &py_dith, &buf, &len,
			  &rowstride))
	return NULL;
    if (PyGtkEnum_get_value(GTK_TYPE_GDK_RGB_DITHER, py_dith, (gint *)&dith))
	return NULL;
    if (rowstride == -1)
	rowstride = width;
    gdk_draw_gray_image(PyGdkWindow_Get(drawable), PyGdkGC_Get(gc), x, y,
			width, height, dith, buf, rowstride);
    Py_INCREF(Py_None);
    return Py_None;
}

#ifdef HAVE_NUMPY
/* this function renders an array to a drawable.  If it is a MxN or MxNx1
 * array, it is considered to be a grey image.  If it is an MxNx3 or MxNx4
 * array, it is considered to be an rgb image.  The second two axes of the
 * array must be contiguous (ie. array->strides[1] == 1,3 or 4,
 * array->strides[2] == 1).  The array->strides[0] is used to deduce the
 * rowstride value for the data.  This allows you to draw a slice of the image
 * data. */
static PyObject *_wrap_gdk_draw_array(PyObject *self, PyObject *args) {
    PyObject *drawable, *gc, *py_dith;
    PyArrayObject *array;
    gint x, y, width, height, rowstride = -1, len;
    GdkRgbDither dith;
    guchar *buf;

    if (!PyArg_ParseTuple(args, "O!O!iiOO!:gdk_draw_array",
			  &PyGdkWindow_Type, &drawable, &PyGdkGC_Type, &gc,
			  &x, &y, &py_dith, &PyArray_Type, &array))
	return NULL;
    if (PyGtkEnum_get_value(GTK_TYPE_GDK_RGB_DITHER, py_dith, (gint *)&dith))
	return NULL;
    if (array->descr->type_num != PyArray_UBYTE) {
	PyErr_SetString(PyExc_TypeError, "array data must be unsigned bytes");
	return NULL;
    }
    if (array->nd < 2) {
	PyErr_SetString(PyExc_TypeError, "array must have at least 2 axes");
	return NULL;
    }
    width = array->dimensions[1];
    height = array->dimensions[0];
    buf = (guchar *)array->data;
    rowstride = array->strides[0];
    if (array->nd == 2 || array->nd == 3 && array->dimensions[2] == 1) {
	/* grey image */
	if (array->strides[1] != 1) {
	    PyErr_SetString(PyExc_TypeError, "second axis must be contiguous");
	    return NULL;
	}
	gdk_draw_gray_image(PyGdkWindow_Get(drawable), PyGdkGC_Get(gc), x, y,
			    width, height, dith, buf, rowstride);
    } else if (array->nd == 3 && array->dimensions[2] == 3) {
	if (array->strides[1] != 3) {
	    PyErr_SetString(PyExc_TypeError, "second axis must be contiguous");
	    return NULL;
	}
	gdk_draw_rgb_image(PyGdkWindow_Get(drawable), PyGdkGC_Get(gc), x, y,
			   width, height, dith, buf, rowstride);
    } else if (array->nd == 3 && array->dimensions[2] == 4) {
	if (array->strides[1] != 4) {
	    PyErr_SetString(PyExc_TypeError, "second axis must be contiguous");
	    return NULL;
	}
	gdk_draw_rgb_32_image(PyGdkWindow_Get(drawable), PyGdkGC_Get(gc), x, y,
			      width, height, dith, buf, rowstride);
    } else {
	PyErr_SetString(PyExc_TypeError,
			"array must be MxN or MxNxP where P is 1, 3 or 4");
	return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
#endif

static PyObject *_wrap_gdk_window_new(PyObject *self, PyObject *args) {
    PyObject *parent;
    PyObject *attr_map;
    GdkWindow* gdk_parent;
    GdkWindow* gdk_win;
    PyObject* pygdk_win;
    GdkWindowAttr attr;
    guint attr_mask = 0;
    PyObject *tmp;
     
    // Parse args
    if (!PyArg_ParseTuple(args, "OO!:gdk_window_new", &parent, &PyDict_Type,
			  &attr_map))
	return NULL;
 
    if (parent == Py_None) 
	gdk_parent = NULL;
    else if (PyGdkWindow_Check(parent))
	gdk_parent = ((PyGdkWindow_Object *) parent)->obj;
    else {
	PyErr_SetString(PyExc_TypeError, "window must be a PyGdkWindow or None");
	return NULL;
    }

    // Fill in attribs
    tmp = PyDict_GetItemString(attr_map, "window_type");
    if (tmp == NULL) {
	PyErr_SetString(PyExc_ValueError, "window_type value must be specified");
	return NULL;
    }
    else {
	if (!PyInt_Check(tmp)) {
	    PyErr_SetString(PyExc_TypeError, "window_type value must be an int");
	    return NULL;
	}
	attr.window_type = PyInt_AsLong(tmp);
    }
    tmp = PyDict_GetItemString(attr_map, "event_mask");
    if (tmp == NULL) {
	PyErr_SetString(PyExc_ValueError, "event_mask value must be specified");
	return NULL;
    }
    else {
	if (!PyInt_Check(tmp)) {
	    PyErr_SetString(PyExc_TypeError, "event_mask value must be an int");
	    return NULL;
	}
	attr.event_mask = PyInt_AsLong(tmp);
    }
    tmp = PyDict_GetItemString(attr_map, "width");
    if (tmp == NULL) {
	PyErr_SetString(PyExc_ValueError, "width value must be specified");
	return NULL;
    }
    else {
	if (!PyInt_Check(tmp)) {
	    PyErr_SetString(PyExc_TypeError, "width value must be an int");
	    return NULL;
	}
	attr.width = PyInt_AsLong(tmp);
    }
    tmp = PyDict_GetItemString(attr_map, "height");
    if (tmp == NULL) {
	PyErr_SetString(PyExc_ValueError, "height value must be specified");
	return NULL;
    }
    else {
	if (!PyInt_Check(tmp)) {
	    PyErr_SetString(PyExc_TypeError, "height value must be an int");
	    return NULL;
	}
	attr.height = PyInt_AsLong(tmp);
    }
    tmp = PyDict_GetItemString(attr_map, "wclass");
    if (tmp != NULL) {
	if (!PyInt_Check(tmp)) {
	    PyErr_SetString(PyExc_TypeError, "wmclass value must be an int");
	    return NULL;
	}
	attr.wclass = PyInt_AsLong(tmp);
	attr_mask |= GDK_WA_WMCLASS;
    }
    /*
      tmp = PyDict_GetItemString(attr_map, "visual");
      if (tmp != NULL) {
      if (!PyGtkVisual_Check(tmp)) {
      PyErr_SetString(PyExc_TypeError, "visual value must be a PyGtkVisual");
      return NULL;
      }
      attr.visual = ((PyGtkVisual*)tmp)->obj;
      attr_mask |= GDK_WA_VISUAL;
      }
    */
    tmp = PyDict_GetItemString(attr_map, "colormap");
    if (tmp != NULL) {
	if (!PyGdkColormap_Check(tmp)) {
	    PyErr_SetString(PyExc_TypeError, "colormap value must be a PyGdkColormap");
	    return NULL;
	}
	attr.colormap = ((PyGdkColormap_Object*)tmp)->obj;
	attr_mask |= GDK_WA_COLORMAP;
    }
    tmp = PyDict_GetItemString(attr_map, "override_redirect");
    if (tmp != NULL) {
	if (!PyInt_Check(tmp)) {
	    PyErr_SetString(PyExc_TypeError, "override_redirect value must be an int");
	    return NULL;
	}
	attr.override_redirect = PyInt_AsLong(tmp);
	attr_mask |= GDK_WA_NOREDIR;
    }
    tmp = PyDict_GetItemString(attr_map, "x");
    if (tmp != NULL) {
	if (!PyInt_Check(tmp)) {
	    PyErr_SetString(PyExc_TypeError, "x value must be an int");
	    return NULL;
	}
	attr.x = PyInt_AsLong(tmp);
	attr_mask |= GDK_WA_X;
    }
    tmp = PyDict_GetItemString(attr_map, "y");
    if (tmp != NULL) {
	if (!PyInt_Check(tmp)) {
	    PyErr_SetString(PyExc_TypeError, "y value must be an int");
	    return NULL;
	}
	attr.y = PyInt_AsLong(tmp);
	attr_mask |= GDK_WA_Y;
    }
 
    gdk_win = gdk_window_new(gdk_parent, &attr, attr_mask);
    pygdk_win = PyGdkWindow_New(gdk_win);
    if (pygdk_win == NULL) {
	gdk_window_destroy(gdk_win);
	return NULL;
    }
    return pygdk_win;
}

static PyObject *
_wrap__disable_gdk_threading(PyObject *self, PyObject *args)
{
  if (!PyArg_ParseTuple(args, ":_disable_gdk_threading"))
    return NULL;
#if ENABLE_PYGTK_THREADING
  disable_threads = TRUE;
  gdk_threads_mutex = NULL;
#endif
  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef _gtkmoduleMethods[] = {
    { "_disable_gdk_threading", _wrap__disable_gdk_threading, 1 },
    { "gtk_signal_connect", _wrap_gtk_signal_connect, 1 },
    { "gtk_signal_connect_after", _wrap_gtk_signal_connect_after, 1 },
    { "gtk_signal_connect_object", _wrap_gtk_signal_connect_object, 1 },
    { "gtk_signal_connect_object_after", _wrap_gtk_signal_connect_object_after, 1 },
    { "gtk_signal_disconnect_by_data", _wrap_gtk_signal_disconnect_by_data, 1 },
    { "gtk_signal_handler_block_by_data", _wrap_gtk_signal_handler_block_by_data, 1 },
    { "gtk_signal_handler_unblock_by_data", _wrap_gtk_signal_handler_unblock_by_data, 1 },
    { "gtk_signal_emitv_by_name", _wrap_gtk_signal_emitv_by_name, 1 },
    { "gtk_init", _wrap_gtk_init, 1 },
    { "gtk_set_locale", _wrap_gtk_set_locale, 1 },
    { "gtk_main", _wrap_gtk_main, 1 },
    { "gtk_main_iteration", _wrap_gtk_main_iteration, 1 },
    { "gtk_events_pending", _wrap_gtk_events_pending, 1 },
    { "gtk_timeout_add", _wrap_gtk_timeout_add, 1 },
    { "gtk_idle_add", _wrap_gtk_idle_add, 1 },
    { "gtk_idle_add_priority", _wrap_gtk_idle_add_priority, 1 },
    { "gtk_idle_remove_function", _wrap_gtk_idle_remove_function, 1 },
    { "gtk_quit_add", _wrap_gtk_quit_add, 1 },
    { "gtk_quit_remove_function", _wrap_gtk_quit_remove_function, 1 },
    { "gtk_input_add", _wrap_gtk_input_add, 1 },
    { "gtk_object_set", _wrap_gtk_object_set, 1 },
    { "gtk_object_new", _wrap_gtk_object_new, 1 },
    { "gtk_object_get", _wrap_gtk_object_get, 1 },
    { "gtk_object_set_data", _wrap_gtk_object_set_data, 1 },
    { "gtk_object_get_data", _wrap_gtk_object_get_data, 1 },
    { "gtk_object_remove_data", _wrap_gtk_object_remove_data, 1 },
    { "gtk_adjustment_set_all", _wrap_gtk_adjustment_set_all, 1 },
    { "gtk_widget_get_window", _wrap_gtk_widget_get_window, 1 },
    { "gtk_widget_get_allocation", _wrap_gtk_widget_get_allocation, 1 },
    { "gtk_widget_draw", _wrap_gtk_widget_draw, 1 },
    { "gtk_widget_size_request", _wrap_gtk_widget_size_request, 1 },
    { "gtk_widget_size_allocate", _wrap_gtk_widget_size_allocate, 1 },
    { "gtk_widget_get_child_requisition", _wrap_gtk_widget_get_child_requisition, 1 },
    { "gtk_widget_intersect", _wrap_gtk_widget_intersect, 1 },
    { "gtk_widget_get_pointer", _wrap_gtk_widget_get_pointer, 1 },
    { "gtk_container_add", _wrap_gtk_container_add, 1 },
    { "gtk_container_child_set", _wrap_gtk_container_child_set, 1 },
    { "gtk_container_child_get", _wrap_gtk_container_child_get, 1 },
    { "gtk_container_children", _wrap_gtk_container_children, 1 },
    { "gtk_window_set_icon", _wrap_gtk_window_set_icon, 1 },
    { "gtk_window_set_icon_name", _wrap_gtk_window_set_icon_name, 1 },
    { "gtk_window_set_geometry_hints", _wrap_gtk_window_set_geometry_hints, 1 },
    { "gtk_box_query_child_packing", _wrap_gtk_box_query_child_packing, 1 },
    { "gtk_button_box_get_child_size_default", _wrap_gtk_button_box_get_child_size_default, 1 },
    { "gtk_button_box_get_child_ipadding_default", _wrap_gtk_button_box_get_child_ipadding_default, 1 },
    { "gtk_button_box_get_child_size", _wrap_gtk_button_box_get_child_size, 1 },
    { "gtk_button_box_get_child_ipadding", _wrap_gtk_button_box_get_child_ipadding, 1 },
    { "gtk_clist_get_selection", _wrap_gtk_clist_get_selection, 1 },
    { "gtk_clist_new_with_titles", _wrap_gtk_clist_new_with_titles, 1 },
    { "gtk_clist_get_text", _wrap_gtk_clist_get_text, 1 },
    { "gtk_clist_get_pixmap", _wrap_gtk_clist_get_pixmap, 1 },
    { "gtk_clist_get_pixtext", _wrap_gtk_clist_get_pixtext, 1 },
    { "gtk_clist_prepend", _wrap_gtk_clist_prepend, 1 },
    { "gtk_clist_append", _wrap_gtk_clist_append, 1 },
    { "gtk_clist_insert", _wrap_gtk_clist_insert, 1 },
    { "gtk_clist_set_row_data", _wrap_gtk_clist_set_row_data, 1 },
    { "gtk_clist_get_row_data", _wrap_gtk_clist_get_row_data, 1 },
    { "gtk_clist_find_row_from_data", _wrap_gtk_clist_find_row_from_data, 1 },
    { "gtk_clist_get_selection_info", _wrap_gtk_clist_get_selection_info, 1 },
    { "gtk_clist_get_column_width", _wrap_gtk_clist_get_column_width, 1 },
    { "gtk_combo_set_popdown_strings", _wrap_gtk_combo_set_popdown_strings,1 },
    { "gtk_curve_get_vector", _wrap_gtk_curve_get_vector, 1 },
    { "gtk_curve_set_vector", _wrap_gtk_curve_set_vector, 1 },
    { "gtk_editable_insert_text", _wrap_gtk_editable_insert_text, 1 },
    { "gtk_item_factory_create_items", _wrap_gtk_item_factory_create_items,1 },
    { "gtk_item_factory_get_widget", _wrap_gtk_item_factory_get_widget, 1 },
    { "gtk_item_factory_get_widget_by_action", _wrap_gtk_item_factory_get_widget_by_action, 1 },
    { "gtk_menu_popup", _wrap_gtk_menu_popup, 1 },
    { "gtk_pixmap_new_from_xpm", _wrap_gtk_pixmap_new_from_xpm, 1 },
    { "gtk_pixmap_get", _wrap_gtk_pixmap_get, 1 },
    { "gtk_preview_draw_row", _wrap_gtk_preview_draw_row, 1 },
    { "gtk_radio_menu_item_new", _wrap_gtk_radio_menu_item_new, 1 },
    { "gtk_radio_menu_item_new_with_label", _wrap_gtk_radio_menu_item_new_with_label, 1 },
    { "gtk_radio_button_new", _wrap_gtk_radio_button_new, 1 },
    { "gtk_radio_button_new_with_label", _wrap_gtk_radio_button_new_with_label, 1 },
    { "gtk_text_insert_defaults", _wrap_gtk_text_insert_defaults, 1 },
    { "gtk_toolbar_append_item", _wrap_gtk_toolbar_append_item, 1 },
    { "gtk_toolbar_prepend_item", _wrap_gtk_toolbar_prepend_item, 1 },
    { "gtk_toolbar_insert_item", _wrap_gtk_toolbar_insert_item, 1 },
    { "gtk_toolbar_append_element", _wrap_gtk_toolbar_append_element, 1 },
    { "gtk_toolbar_prepend_element", _wrap_gtk_toolbar_prepend_element, 1 },
    { "gtk_toolbar_insert_element", _wrap_gtk_toolbar_insert_element, 1 },
    { "gtk_label_get", _wrap_gtk_label_get, 1 },
    { "gtk_color_selection_get_color", _wrap_gtk_color_selection_get_color,1 },
    { "gtk_color_selection_set_color", _wrap_gtk_color_selection_set_color,1 },
    { "gtk_list_insert_items", _wrap_gtk_list_insert_items, 1 },
    { "gtk_list_append_items", _wrap_gtk_list_append_items, 1 },
    { "gtk_list_prepend_items", _wrap_gtk_list_prepend_items, 1 },
    { "gtk_list_remove_items", _wrap_gtk_list_remove_items, 1 },
    { "gtk_list_get_selection", _wrap_gtk_list_get_selection, 1 },
    { "gtk_tree_remove_items", _wrap_gtk_tree_remove_items, 1 },
    { "gtk_tree_get_selection", _wrap_gtk_tree_get_selection, 1 },
    { "gtk_calendar_get_date", _wrap_gtk_calendar_get_date, 1 },
    { "gtk_notebook_query_tab_label_packing", _wrap_gtk_notebook_query_tab_label_packing, 1 },
    { "gtk_drag_dest_set", _wrap_gtk_drag_dest_set, 1 },
    { "gtk_drag_source_set", _wrap_gtk_drag_source_set, 1 },
    { "gtk_drag_get_source_widget", _wrap_gtk_drag_get_source_widget, 1 },
    { "gtk_drag_begin", _wrap_gtk_drag_begin, 1 },
    { "gtk_ctree_new_with_titles", _wrap_gtk_ctree_new_with_titles, 1 },
    { "gtk_ctree_get_selection", _wrap_gtk_ctree_get_selection, 1 },
    { "gtk_ctree_insert_node", _wrap_gtk_ctree_insert_node, 1 },
    { "gtk_ctree_find_by_row_data", _wrap_gtk_ctree_find_by_row_data, 1 },
    { "gtk_ctree_find_all_by_row_data", _wrap_gtk_ctree_find_all_by_row_data, 1 },
    { "gtk_ctree_move", _wrap_gtk_ctree_move, 1 },
    { "gtk_ctree_node_get_text", _wrap_gtk_ctree_node_get_text, 1 },
    { "gtk_ctree_node_get_pixmap", _wrap_gtk_ctree_node_get_pixmap, 1 },
    { "gtk_ctree_node_get_pixtext", _wrap_gtk_ctree_node_get_pixtext, 1 },
    { "gtk_ctree_get_node_info", _wrap_gtk_ctree_get_node_info, 1 },
    { "gtk_ctree_node_set_row_data", _wrap_gtk_ctree_node_set_row_data, 1 },
    { "gtk_ctree_node_get_row_data", _wrap_gtk_ctree_node_get_row_data, 1 },
    { "gtk_ctree_base_nodes", _wrap_gtk_ctree_base_nodes, 1 },
#include "gtkmodule_defs.c"
    { "gdk_char_width", _wrap_gdk_char_width, 1 },
    { "gdk_flush", _wrap_gdk_flush, 1 },
    { "gdk_beep", _wrap_gdk_beep, 1 },
    { "gdk_pixmap_new", _wrap_gdk_pixmap_new, 1 },
    { "gdk_pixmap_create_from_xpm", _wrap_gdk_pixmap_create_from_xpm, 1 },
    { "gdk_pixmap_create_from_xpm_d", _wrap_gdk_pixmap_create_from_xpm_d, 1 },
    { "gdk_pixmap_colormap_create_from_xpm", _wrap_gdk_pixmap_colormap_create_from_xpm, 1 },
    { "gdk_pixmap_colormap_create_from_xpm_d", _wrap_gdk_pixmap_colormap_create_from_xpm_d, 1 },
    { "gdk_bitmap_create_from_data", _wrap_gdk_bitmap_create_from_data, 1 },
    { "gdk_font_load", _wrap_gdk_font_load, 1 },
    { "gdk_fontset_load", _wrap_gdk_fontset_load, 1 },
    { "gdk_draw_polygon", _wrap_gdk_draw_polygon, 1 },
    { "gdk_draw_text", _wrap_gdk_draw_text, 1 },
    { "gdk_draw_points", _wrap_gdk_draw_points, 1 },
    { "gdk_draw_segments", _wrap_gdk_draw_segments, 1 },
    { "gdk_draw_lines", _wrap_gdk_draw_lines, 1 },
    { "gdk_color_alloc", _wrap_gdk_color_alloc, 1 },
    { "gdk_color_new", _wrap_gdk_color_new, 1 },
    { "gdk_threads_enter", _wrap_gdk_threads_enter, 1 },
    { "gdk_threads_leave", _wrap_gdk_threads_leave, 1 },
#ifdef WITH_XSTUFF
    { "gdk_window_foreign_new", _wrap_gdk_window_foreign_new, 1 },
    { "gdk_get_root_win", _wrap_gdk_get_root_win, 1 },
#endif
    { "gtk_rgb_push_visual", _wrap_gtk_rgb_push_visual, 1 },
    { "gtk_pop_visual", _wrap_gtk_pop_visual, 1 },
    { "gdk_draw_rgb_image", _wrap_gdk_draw_rgb_image, 1 },
    { "gdk_draw_rgb_32_image", _wrap_gdk_draw_rgb_32_image, 1 },
    { "gdk_draw_gray_image", _wrap_gdk_draw_gray_image, 1 },
#ifdef HAVE_NUMPY
    { "gdk_draw_array", _wrap_gdk_draw_array, 1 },
#endif
    { "gdk_window_new", _wrap_gdk_window_new, 1 },
    { NULL, NULL }
};

static struct _PyGtk_FunctionStruct functions = {
    VERSION,
    FALSE,

    PyGtk_BlockThreads,
    PyGtk_UnblockThreads,

    PyGtk_DestroyNotify,
    PyGtk_CallbackMarshal,
    GtkArgs_AsTuple,
    GtkArgs_FromSequence,
    GtkArg_FromPyObject,
    GtkArg_AsPyObject,
    GtkRet_FromPyObject,
    GtkRet_AsPyObject,
    PyDict_AsGtkArgs,
    PyGtk_RegisterBoxed,

    PyGtkEnum_get_value,
    PyGtkFlag_get_value,

    &PyGtk_Type,  PyGtk_New,
    &PyGtkAccelGroup_Type,  PyGtkAccelGroup_New,
    &PyGtkStyle_Type,  PyGtkStyle_New,
    &PyGdkFont_Type,  PyGdkFont_New,
    &PyGdkColor_Type,  PyGdkColor_New,
    &PyGdkEvent_Type,  PyGdkEvent_New,
    &PyGdkWindow_Type,  PyGdkWindow_New,
    &PyGdkGC_Type,  PyGdkGC_New,
    &PyGdkColormap_Type,  PyGdkColormap_New,
    &PyGdkDragContext_Type,  PyGdkDragContext_New,
    &PyGtkSelectionData_Type,  PyGtkSelectionData_New,
    &PyGdkAtom_Type,  PyGdkAtom_New,
    &PyGdkCursor_Type,  PyGdkCursor_New,
    &PyGtkCTreeNode_Type,  PyGtkCTreeNode_New
};

DL_EXPORT(void)
init_gtk(void) {
     PyObject *m, *d, *tuple, *private;

     m = Py_InitModule("_gtk", _gtkmoduleMethods);
     d = PyModule_GetDict(m);

#ifdef HAVE_NUMPY
     import_array();
#endif

     /* initialise the boxed_funcs hash_table */
     boxed_funcs = g_hash_table_new(g_direct_hash, g_direct_equal);

     PyDict_SetItemString(d, "GtkObjectType", (PyObject *)&PyGtk_Type);
     PyDict_SetItemString(d, "GtkAccelGroupType",
                          (PyObject *)&PyGtkAccelGroup_Type);
     PyDict_SetItemString(d, "GtkStyleType", (PyObject *)&PyGtkStyle_Type);
     PyDict_SetItemString(d, "GdkFontType", (PyObject *)&PyGdkFont_Type);
     PyDict_SetItemString(d, "GdkColorType", (PyObject *)&PyGdkColor_Type);
     PyDict_SetItemString(d, "GdkEventType", (PyObject *)&PyGdkEvent_Type);
     PyDict_SetItemString(d, "GdkWindowType", (PyObject *)&PyGdkWindow_Type);
     PyDict_SetItemString(d, "GdkGCType", (PyObject *)&PyGdkGC_Type);
     PyDict_SetItemString(d, "GdkColormapType",(PyObject*)&PyGdkColormap_Type);
     PyDict_SetItemString(d, "GdkDragContextType",
			  (PyObject *)&PyGdkDragContext_Type);
     PyDict_SetItemString(d, "GtkSelectionDataType",
			  (PyObject *)&PyGtkSelectionData_Type);
     PyDict_SetItemString(d, "GdkAtomType", (PyObject *)&PyGdkAtom_Type);
     PyDict_SetItemString(d, "GdkCursorType", (PyObject *)&PyGdkCursor_Type);
     PyDict_SetItemString(d, "GtkCTreeNodeType",
			  (PyObject *)&PyGtkCTreeNode_Type);

     PyDict_SetItemString(d, "_PyGtk_API",
			  PyCObject_FromVoidPtr(&functions, NULL));

     /* gtk+ version */
     tuple = Py_BuildValue ("(iii)", gtk_major_version, gtk_minor_version,
			    gtk_micro_version);
     PyDict_SetItemString(d, "gtk_version", tuple);    
     Py_DECREF(tuple);
        
     /* pygtk version */
     tuple = Py_BuildValue ("(iii)", PYGTK_MAJOR_VERSION, PYGTK_MINOR_VERSION,
			    PYGTK_MICRO_VERSION);
     PyDict_SetItemString(d, "pygtk_version", tuple);
     Py_DECREF(tuple);

     m = PyImport_ImportModule("os");
     if (m == NULL) {
         Py_FatalError("couldn't import os");
	 return;
     }
     d = PyModule_GetDict(m);
     Py_DECREF(m);
     d = PyDict_GetItemString(d, "environ");
     d = PyMapping_GetItemString(d, "PYGTK_FATAL_EXCEPTIONS");
     if (d == NULL)
         PyErr_Clear();
     else {
         functions.fatalExceptions=PyGtk_FatalExceptions = PyObject_IsTrue(d);
	 Py_DECREF(d);
     }

#ifdef ENABLE_PYGTK_THREADING
     /* it is required that this function be called to enable the
      * thread safety functions.  The _gtk._disable_gdk_threading()
      * function can be used to set the GDK thread lock mutex back to
      * NULL, if threaded behaviour is not desired. */
     if ( !g_threads_got_initialized)
         g_thread_init(NULL);
#endif

     if (PyErr_Occurred())
         Py_FatalError("can't initialise module _gtk");
}
