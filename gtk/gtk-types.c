/* -*- Mode: C; c-basic-offset: 4 -*- */
#include <gtk/gtk.h>
#include "pygtk-private.h"
#include <structmember.h>

/* these aren't ExtensionClass's */
#undef Py_FindMethod

PyObject *
PyGtkAccelGroup_New(GtkAccelGroup *obj)
{
    PyGtkAccelGroup_Object  *self;

    self = (PyGtkAccelGroup_Object *)PyObject_NEW(PyGtkAccelGroup_Object ,
						  &PyGtkAccelGroup_Type);
    if (self == NULL)
	return NULL;
    self->obj = obj;
    gtk_accel_group_ref(self->obj);
    return (PyObject *)self;
}

#if 0
PyObject *
PyGtkStyle_New(GtkStyle *obj)
{
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
#endif

PyObject *
PyGdkFont_New(GdkFont *obj)
{
    PyGdkFont_Object *self;

    self = (PyGdkFont_Object *)PyObject_NEW(PyGdkFont_Object, &PyGdkFont_Type);
    if (self == NULL)
	return NULL;
    self->obj = obj;
    gdk_font_ref(self->obj);
    return (PyObject *)self;
}

PyObject *
PyGdkColor_New(GdkColor *obj)
{
    PyGdkColor_Object *self;

    self = (PyGdkColor_Object *)PyObject_NEW(PyGdkColor_Object,
					     &PyGdkColor_Type);
    if (self == NULL)
	return NULL;
    self->obj.pixel = obj->pixel;
    self->obj.red   = obj->red;
    self->obj.green = obj->green;
    self->obj.blue  = obj->blue;
    return (PyObject *)self;
}

#if 0
PyObject *
PyGdkWindow_New(GdkWindow *win)
{
    PyGdkWindow_Object *self;

    self = (PyGdkWindow_Object *)PyObject_NEW(PyGdkWindow_Object,
					      &PyGdkWindow_Type);
    if (self == NULL)
	return NULL;
    self->obj = win;
    gdk_window_ref(self->obj);
    return (PyObject *)self;
}

PyObject *
PyGdkGC_New(GdkGC *gc)
{
    PyGdkGC_Object *self;

    self = (PyGdkGC_Object *)PyObject_NEW(PyGdkGC_Object, &PyGdkGC_Type);
    if (self == NULL)
	return NULL;
    self->obj = gc;
    gdk_gc_ref(self->obj);
    return (PyObject *)self;
}
#endif

PyObject *
PyGdkVisual_New(GdkVisual *visual)
{
    PyGdkVisual_Object *self;

    self = (PyGdkVisual_Object *)PyObject_NEW(PyGdkVisual_Object,
					      &PyGdkVisual_Type);
    if (self == NULL)
	return NULL;
    self->obj = visual;
    gdk_visual_ref(self->obj);
    return (PyObject *)self;
}

#if 0
PyObject *
PyGdkColormap_New(GdkColormap *cmap)
{
    PyGdkColormap_Object *self;

    self = (PyGdkColormap_Object *)PyObject_NEW(PyGdkColormap_Object,
						&PyGdkColormap_Type);
    if (self == NULL)
	return NULL;
    self->obj = cmap;
    gdk_colormap_ref(self->obj);
    return (PyObject *)self;
}

PyObject *
PyGdkDragContext_New(GdkDragContext *ctx)
{
    PyGdkDragContext_Object *self;

    self = (PyGdkDragContext_Object *)PyObject_NEW(PyGdkDragContext_Object,
						   &PyGdkDragContext_Type);
    if (self == NULL)
	return NULL;
    self->obj = ctx;
    gdk_drag_context_ref(self->obj);
    return (PyObject *)self;
}
#endif

PyObject *
PyGtkSelectionData_New(GtkSelectionData *data)
{
    PyGtkSelectionData_Object *self;

    self = (PyGtkSelectionData_Object *)PyObject_NEW(PyGtkSelectionData_Object,
						     &PyGtkSelectionData_Type);
    if (self == NULL)
	return NULL;
    self->obj = data;
    return (PyObject *)self;
}

PyObject *
PyGdkAtom_New(GdkAtom atom)
{
    PyGdkAtom_Object *self;

    self = (PyGdkAtom_Object *)PyObject_NEW(PyGdkAtom_Object, &PyGdkAtom_Type);
    if (self == NULL)
	return NULL;
    self->atom = atom;
    self->name = NULL;
    return (PyObject *)self;
}

PyObject *
PyGdkCursor_New(GdkCursor *obj)
{
    PyGdkCursor_Object *self;

    self = (PyGdkCursor_Object *)PyObject_NEW(PyGdkCursor_Object,
					      &PyGdkCursor_Type);
    if (self == NULL)
	return NULL;
    self->obj = obj;
    return (PyObject *)self;
}

PyObject *
PyGtkCTreeNode_New(GtkCTreeNode *node)
{
    PyGtkCTreeNode_Object *self;

    self = (PyGtkCTreeNode_Object *)PyObject_NEW(PyGtkCTreeNode_Object,
						 &PyGtkCTreeNode_Type);
    if (self == NULL)
	return NULL;
    self->node = node;
    return (PyObject *)self;
}

PyObject *
PyGdkDevice_New(GdkDevice *device)
{
    PyGdkDevice_Object *self;

    self = (PyGdkDevice_Object *)PyObject_NEW(PyGdkDevice_Object,
					      &PyGdkDevice_Type);
    if (self == NULL)
	return NULL;
    self->obj = device;
    return (PyObject *)self;
}

PyObject *
PyGtkTextIter_New(GtkTextIter *iter)
{
    PyGtkTextIter_Object *self;

    self = (PyGtkTextIter_Object *)PyObject_NEW(PyGtkTextIter_Object,
						&PyGtkTextIter_Type);
    if (self == NULL)
	return NULL;
    if (iter)
	self->iter = *iter;
    else
	memset(&self->iter, 0, sizeof(GtkTextIter));
    return (PyObject *)self;
}

PyObject *
PyGtkTreeIter_New(GtkTreeIter *iter)
{
    PyGtkTreeIter_Object *self;

    self = (PyGtkTreeIter_Object *)PyObject_NEW(PyGtkTreeIter_Object,
						&PyGtkTreeIter_Type);
    if (self == NULL)
	return NULL;
    if (iter)
	self->iter = *iter;
    else
	memset(&self->iter, 0, sizeof(GtkTreeIter));
    return (PyObject *)self;
}

static void
pygtk_accel_group_dealloc(PyGtkAccelGroup_Object *self)
{
    gtk_accel_group_unref(self->obj); 
    PyMem_DEL(self);
}

static int
pygtk_accel_group_compare(PyGtkAccelGroup_Object *self,
			PyGtkAccelGroup_Object *v)
{
    if (self->obj == v->obj) return 0;
    if (self->obj > v->obj) return -1;
    return 1;
}

static long
pygtk_accel_group_hash(PyGtkAccelGroup_Object *self)
{
    return (long)self->obj;
}

static PyObject *
pygtk_accel_group_lock(PyGtkAccelGroup_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkAccelGroup.lock"))
	return NULL;
    gtk_accel_group_lock(self->obj);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
pygtk_accel_group_unlock(PyGtkAccelGroup_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkAccelGroup.unlock"))
	return NULL;
    gtk_accel_group_unlock(self->obj);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef pygtk_accel_group_methods[] = {
    { "lock", (PyCFunction)pygtk_accel_group_lock, METH_VARARGS },
    { "unlock", (PyCFunction)pygtk_accel_group_unlock, METH_VARARGS },
    { NULL, NULL, 0 }
};

static PyObject *
pygtk_accel_group_getattr(PyGtkAccelGroup_Object *self, gchar *attr)
{
    return Py_FindMethod(pygtk_accel_group_methods, (PyObject *)self, attr);
}

PyTypeObject PyGtkAccelGroup_Type = {
    PyObject_HEAD_INIT(NULL)
    0,
    "GtkAccelGroup",
    sizeof(PyGtkAccelGroup_Object),
    0,
    (destructor)pygtk_accel_group_dealloc,
    (printfunc)0,
    (getattrfunc)pygtk_accel_group_getattr,
    (setattrfunc)0,
    (cmpfunc)pygtk_accel_group_compare,
    (reprfunc)0,
    0,
    0,
    0,
    (hashfunc)pygtk_accel_group_hash,
    (ternaryfunc)0,
    (reprfunc)0,
    0L,0L,0L,0L,
    NULL
};

#if 0
typedef struct {
    PyObject_HEAD
    GtkStyle *style; /* parent style */
    enum {STYLE_COLOUR_ARRAY, STYLE_GC_ARRAY, STYLE_PIXMAP_ARRAY} type;
    gpointer array;
} PyGtkStyleHelper_Object;
staticforward PyTypeObject PyGtkStyleHelper_Type;

static PyObject *
PyGtkStyleHelper_New(GtkStyle *style, int type, gpointer array)
{
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

static void
PyGtkStyleHelper_Dealloc(PyGtkStyleHelper_Object *self)
{
    gtk_style_unref(self->style);
    PyMem_DEL(self);
}

static int
PyGtkStyleHelper_Length(PyGtkStyleHelper_Object *self)
{
    return 5;
}
static PyObject *
PyGtkStyleHelper_GetItem(PyGtkStyleHelper_Object *self, int pos)
{
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
	    return PyGdkGC_New(array[pos]);
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
static int
PyGtkStyleHelper_SetItem(PyGtkStyleHelper_Object *self, int pos,
			 PyObject *value)
{
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
		PyErr_SetString(PyExc_TypeError,
				"can only assign a GdkPixmap or None");
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
    PyObject_HEAD_INIT(NULL)
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
PyGtkStyle_Dealloc(PyGtkStyle_Object *self)
{
    gtk_style_unref(self->obj);
    PyMem_DEL(self);
}

static int
PyGtkStyle_Compare(PyGtkStyle_Object *self, PyGtkStyle_Object *v)
{
    if (self->obj == v->obj) return 0;
    if (self->obj > v->obj) return -1;
    return 1;
}

static long
PyGtkStyle_Hash(PyGtkStyle_Object *self)
{
    return (long)self->obj;
}

static PyObject *
PyGtkStyle_copy(PyGtkStyle_Object *self, PyObject *args)
{
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

static PyObject *
PyGtkStyle_GetAttr(PyGtkStyle_Object *self, char *attr)
{
    GtkStyle *style = self->obj;

    if (!strcmp(attr, "__members__"))

	return Py_BuildValue("[sssssssssssssssssssss]", "base",
			     "base_gc", "bg", "bg_gc", "bg_pixmap", "black",
			     "black_gc", "colormap", "dark", "dark_gc", "fg",
			     "fg_gc", "font", "light", "light_gc", "mid",
			     "mid_gc", "text", "text_gc", "white", "white_gc");

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
    if (!strcmp(attr, "black_gc"))
	return PyGdkGC_New(style->black_gc);
    if (!strcmp(attr, "white_gc"))
	return PyGdkGC_New(style->white_gc);
    if (!strcmp(attr, "bg_pixmap"))
	return PyGtkStyleHelper_New(style,STYLE_PIXMAP_ARRAY,style->bg_pixmap);
    if (!strcmp(attr, "colormap")) {
	if (style->colormap)
	    return PyGdkColormap_New(style->colormap);
	Py_INCREF(Py_None);
	return Py_None;
    }

    return Py_FindMethod(PyGtkStyle_methods, (PyObject *)self, attr);
}

static int
PyGtkStyle_SetAttr(PyGtkStyle_Object *self, char *key, PyObject *value)
{
    GtkStyle *style = self->obj;
    if (!strcmp(key, "font")) {
	if (PyGdkFont_Check(value)) {
	    if (style->font)
		gdk_font_unref(style->font);
	    style->font = gdk_font_ref(PyGdkFont_Get(value));
	} else {
	    PyErr_SetString(PyExc_TypeError,
			    "font attribute must be type GdkFont");
	    return -1;
	}
    } else if (!strcmp(key, "black")) {
	if (PyGdkColor_Check(value))
	    style->black = *PyGdkColor_Get(value);
	else {
	    PyErr_SetString(PyExc_TypeError,
			    "black attribute must be a GdkColor");
	    return -1;
	}
    } else if (!strcmp(key, "white")) {
	if (PyGdkColor_Check(value))
	    style->white = *PyGdkColor_Get(value);
	else {
	    PyErr_SetString(PyExc_TypeError,
			    "white attribute must be a GdkColor");
	    return -1;
	}
    } else if (!strcmp(key, "black_gc")) {
	if (PyGdkGC_Check(value)) {
	    if (style->black_gc)
		gdk_gc_unref(style->black_gc);
	    style->black_gc = gdk_gc_ref(PyGdkGC_Get(value));
	} else {
	    PyErr_SetString(PyExc_TypeError,
			    "black_gc attribute must be a GdkColor");
	    return -1;
	}
    } else if (!strcmp(key, "white_gc")) {
	if (PyGdkGC_Check(value)) {
	    if (style->white_gc)
		gdk_gc_unref(style->white_gc);
	    style->white_gc = gdk_gc_ref(PyGdkGC_Get(value));
	} else {
	    PyErr_SetString(PyExc_TypeError,
			    "white_gc attribute must be a GdkColor");
	    return -1;
	}
    } else {
	PyErr_SetString(PyExc_AttributeError, key);
	return -1;
    }
    return 0;
}

PyTypeObject PyGtkStyle_Type = {
    PyObject_HEAD_INIT(NULL)
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
#endif

static void
pygdk_font_dealloc(PyGdkFont_Object *self)
{
    gdk_font_unref(self->obj);
    PyMem_DEL(self);
}

static PyObject *
pygdk_font_width(PyGdkFont_Object *self, PyObject *args)
{
    char *text;
    int length;

    if (!PyArg_ParseTuple(args, "s#:GdkFont.width", &text, &length))
	return NULL;
    return PyInt_FromLong(gdk_text_width(self->obj, text, length));
}

static PyObject *
pygdk_font_measure(PyGdkFont_Object *self, PyObject *args)
{
    char *text;
    int length;

    if (!PyArg_ParseTuple(args, "s#:GdkFont.measure", &text, &length))
	return NULL;
    return PyInt_FromLong(gdk_text_measure(self->obj, text, length));
}

static PyObject *
pygdk_font_height(PyGdkFont_Object *self, PyObject *args)
{
    char *text;
    int length;

    if (!PyArg_ParseTuple(args, "s#:GdkFont.height", &text, &length))
	return NULL;
    return PyInt_FromLong(gdk_text_height(self->obj, text, length));
}

static PyObject *
pygdk_font_extents(PyGdkFont_Object *self, PyObject *args)
{
    char *text;
    int length, lbearing, rbearing, width, ascent, descent;
    if (!PyArg_ParseTuple(args, "s#:GdkFont.extents", &text, &length))
	return NULL;
    gdk_text_extents(self->obj, text, length, &lbearing, &rbearing,
		     &width, &ascent, &descent);
    return Py_BuildValue("(iiiii)", lbearing, rbearing, width, ascent, descent);
}

static PyMethodDef PyGdkFont_methods[] = {
    {"width",   (PyCFunction)pygdk_font_width,   METH_VARARGS, NULL},
    {"measure", (PyCFunction)pygdk_font_measure, METH_VARARGS, NULL},
    {"height",  (PyCFunction)pygdk_font_height,  METH_VARARGS, NULL},
    {"extents", (PyCFunction)pygdk_font_extents, METH_VARARGS, NULL},
    {NULL, 0, 0, NULL}
};

static PyObject *
pygdk_font_getattr(PyGdkFont_Object *self, char *key)
{
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
pygdk_font_compare(PyGdkFont_Object *self, PyGdkFont_Object *v)
{
    if (self->obj == v->obj) return 0;
    if (self->obj > v->obj) return -1;
    return 1;
}

static long
pygdk_font_hash(PyGdkFont_Object *self)
{
    return (long)self->obj;
}

PyTypeObject PyGdkFont_Type = {
    PyObject_HEAD_INIT(NULL)
    0,
    "GdkFont",
    sizeof(PyGdkFont_Object),
    0,
    (destructor)pygdk_font_dealloc,
    (printfunc)0,
    (getattrfunc)pygdk_font_getattr,
    (setattrfunc)0,
    (cmpfunc)pygdk_font_compare,
    (reprfunc)0,
    0,
    0,
    0,
    (hashfunc)pygdk_font_hash,
    (ternaryfunc)0,
    (reprfunc)0,
    0L,0L,0L,0L,
    NULL
};

static void
pygdk_color_dealloc(PyGdkColor_Object *self)
{
    PyMem_DEL(self);
}

static PyObject *
pygdk_color_getattr(PyGdkColor_Object *self, char *key)
{
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
pygdk_color_setattr(PyGdkColor_Object *self, char *key, PyObject * val)
{
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
pygdk_color_compare(PyGdkColor_Object *self, PyGdkColor_Object *v)
{
    if (self->obj.pixel == v->obj.pixel) return 0;
    if (self->obj.pixel > v->obj.pixel) return -1;
    return 1;
}

static long
pygdk_color_hash(PyGdkColor_Object *self)
{
    return (long)self->obj.pixel;
}

static PyObject *
pygdk_color_repr(PyGdkColor_Object *self)
{
    char buf[80];

    g_snprintf(buf, 79, "<GdkColor (%hu, %hu, %hu)>", self->obj.red,
	       self->obj.green, self->obj.blue);
    return PyString_FromString(buf);
}

PyTypeObject PyGdkColor_Type = {
    PyObject_HEAD_INIT(NULL)
    0,
    "GdkColor",
    sizeof(PyGdkColor_Object),
    0,
    (destructor)pygdk_color_dealloc,
    (printfunc)0,
    (getattrfunc)pygdk_color_getattr,
    (setattrfunc)pygdk_color_setattr,
    (cmpfunc)pygdk_color_compare,
    (reprfunc)pygdk_color_repr,
    0,
    0,
    0,
    (hashfunc)pygdk_color_hash,
    (ternaryfunc)0,
    (reprfunc)0,
    0L,0L,0L,0L,
    NULL
};

PyObject *
PyGdkEvent_New(GdkEvent *obj)
{
    PyGdkEvent_Object *self;
    PyObject *v;
    guint i;

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
	if ((v = pygobject_new((GObject *)obj->any.window)) == NULL)
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
	if (obj->motion.axes) {
	    v = PyTuple_New(obj->motion.device->num_axes);
	    if (!v) return NULL;
	    for (i = 0; i < obj->motion.device->num_axes; i++)
		PyTuple_SetItem(v, i, PyFloat_FromDouble(obj->motion.axes[i]));
	} else {
	    Py_INCREF(Py_None);
	    v = Py_None;
	}
	PyDict_SetItemString(self->attrs,"axes", v);
	Py_DECREF(v);
	if ((v = PyInt_FromLong(obj->motion.state)) == NULL)
	    return NULL;
	PyDict_SetItemString(self->attrs, "state", v);
	Py_DECREF(v);
	if ((v = PyInt_FromLong(obj->motion.is_hint)) == NULL)
	    return NULL;
	PyDict_SetItemString(self->attrs, "is_hint", v);
	Py_DECREF(v);
	if ((v = PyGdkDevice_New(obj->motion.device)) == NULL)
	    return NULL;
	PyDict_SetItemString(self->attrs, "device", v);
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
	if (obj->button.axes) {
	    v = PyTuple_New(obj->button.device->num_axes);
	    if (!v) return NULL;
	    for (i = 0; i < obj->button.device->num_axes; i++)
		PyTuple_SetItem(v, i, PyFloat_FromDouble(obj->button.axes[i]));
	} else {
	    Py_INCREF(Py_None);
	    v = Py_None;
	}
	PyDict_SetItemString(self->attrs,"axes", v);
	Py_DECREF(v);
	if ((v = PyInt_FromLong(obj->button.state)) == NULL)
	    return NULL;
	PyDict_SetItemString(self->attrs, "state", v);
	Py_DECREF(v);
	if ((v = PyInt_FromLong(obj->button.button)) == NULL)
	    return NULL;
	PyDict_SetItemString(self->attrs, "button", v);
	Py_DECREF(v);
	if ((v = PyGdkDevice_New(obj->button.device)) == NULL)
	    return NULL;
	PyDict_SetItemString(self->attrs, "device", v);
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
	    if ((v = pygobject_new((GObject*)obj->crossing.subwindow)) == NULL)
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
	if ((v = PyGdkDevice_New(obj->proximity.device)) == NULL)
	    return NULL;
	PyDict_SetItemString(self->attrs, "device", v);
	Py_DECREF(v);
	break;
    case GDK_DRAG_ENTER:        /*GdkEventDND               dnd*/
    case GDK_DRAG_LEAVE:        /*GdkEventDND               dnd*/
    case GDK_DRAG_MOTION:       /*GdkEventDND               dnd*/
    case GDK_DRAG_STATUS:       /*GdkEventDND               dnd*/
    case GDK_DROP_START:        /*GdkEventDND               dnd*/
    case GDK_DROP_FINISHED:     /*GdkEventDND               dnd*/
	if ((v = pygobject_new((GObject *)obj->dnd.context)) == NULL)
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
pygdk_event_dealloc(PyGdkEvent_Object *self)
{
    Py_XDECREF(self->attrs);
    PyMem_DEL(self);
}

static PyObject *
pygdk_event_getattr(PyGdkEvent_Object *self, char *attr)
{
    PyObject *ret;
    ret = PyDict_GetItemString(self->attrs, attr);
    if (ret != NULL) {
	Py_INCREF(ret);
	return ret;
    }
    PyErr_SetString(PyExc_AttributeError, attr);
    return NULL;
}

static int
pygdk_event_compare(PyGdkEvent_Object *self, PyGdkEvent_Object *v)
{
    if (self->obj == v->obj) return 0;
    if (self->obj > v->obj) return -1;
    return 1;
}

static long
pygdk_event_hash(PyGdkEvent_Object *self)
{
    return (long)self->obj;
}

PyTypeObject PyGdkEvent_Type = {
    PyObject_HEAD_INIT(NULL)
    0,
    "GdkEvent",
    sizeof(PyGdkEvent_Object),
    0,
    (destructor)pygdk_event_dealloc,
    (printfunc)0,
    (getattrfunc)pygdk_event_getattr,
    (setattrfunc)0,
    (cmpfunc)pygdk_event_compare,
    (reprfunc)0,
    0,
    0,
    0,
    (hashfunc)pygdk_event_hash,
    (ternaryfunc)0,
    (reprfunc)0,
    0L,0L,0L,0L,
    NULL
};

#if 0
static void
PyGdkWindow_Dealloc(PyGdkWindow_Object *self)
{
    if (gdk_window_get_type(self->obj) == GDK_WINDOW_PIXMAP)
	gdk_pixmap_unref(self->obj);
    else
	gdk_window_unref(self->obj);
    PyMem_DEL(self);
}

static int
PyGdkWindow_Compare(PyGdkWindow_Object *self, PyGdkWindow_Object *v)
{
    if (self->obj == v->obj) return 0;
    if (self->obj > v->obj) return -1;
    return 1;
}

static long
PyGdkWindow_Hash(PyGdkWindow_Object *self)
{
    return (long)self->obj;
}

static PyObject *
PyGdkWindow_Repr(PyGdkWindow_Object *self)
{
    char buf[100];
    if (gdk_window_get_type(self->obj) == GDK_WINDOW_PIXMAP)
	sprintf(buf, "<GdkPixmap at %lx>", (long)PyGdkWindow_Get(self));
    else
	sprintf(buf, "<GdkWindow at %lx>", (long)PyGdkWindow_Get(self));
    return PyString_FromString(buf);
}

static PyObject *
PyGdkWindow_NewGC(PyGdkWindow_Object *self, PyObject *args, PyObject *kws)
{
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
		    PyErr_SetString(PyExc_TypeError,
				    "foreground argument takes a GdkColor");
		    return NULL;
		}
		mask |= GDK_GC_FOREGROUND;
		values.foreground.red = PyGdkColor_Get(value)->red;
		values.foreground.green = PyGdkColor_Get(value)->green;
		values.foreground.blue = PyGdkColor_Get(value)->blue;
		values.foreground.pixel = PyGdkColor_Get(value)->pixel;
	    } else if (!strcmp(strkey, "background")) {
		if (!PyGdkColor_Check(value)) {
		    PyErr_SetString(PyExc_TypeError,
				    "background argument takes a GdkColor");
		    return NULL;
		}
		mask |= GDK_GC_BACKGROUND;
		values.background.red = PyGdkColor_Get(value)->red;
		values.background.green = PyGdkColor_Get(value)->green;
		values.background.blue = PyGdkColor_Get(value)->blue;
		values.background.pixel = PyGdkColor_Get(value)->pixel;
	    } else if (!strcmp(strkey, "font")) {
		if (!PyGdkFont_Check(value)) {
		    PyErr_SetString(PyExc_TypeError,
				    "font argument takes a GdkFont");
		    return NULL;
		}
		mask |= GDK_GC_FONT;
		values.font = PyGdkFont_Get(value);
	    } else if (!strcmp(strkey, "tile")) {
		if (!PyGdkWindow_Check(value)) {
		    PyErr_SetString(PyExc_TypeError,
				    "tile argument takes a GdkPixmap");
		    return NULL;
		}
		mask |= GDK_GC_TILE;
		values.tile = PyGdkWindow_Get(value);
	    } else if (!strcmp(strkey, "stipple")) {
		if (!PyGdkWindow_Check(value)) {
		    PyErr_SetString(PyExc_TypeError,
				    "stipple argument takes a GdkPixmap");
		    return NULL;
		}
		mask |= GDK_GC_STIPPLE;
		values.stipple = PyGdkWindow_Get(value);
	    } else if (!strcmp(strkey, "clip_mask")) {
		if (!PyGdkWindow_Check(value)) {
		    PyErr_SetString(PyExc_TypeError,
				    "clip_mask argument takes a GdkPixmap");
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
			    g_snprintf(buf, sizeof(buf),
				       "%s argument expects an integer",
				       others[i].name);
			    PyErr_SetString(PyExc_TypeError, buf);
			    return NULL;
			}
			mask |= others[i].mask;
			*((int *)((char *)&values + others[i].offs)) =
			    PyInt_AsLong(value);
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

static PyObject *
PyGdkWindow_SetCursor(PyGdkWindow_Object *self, PyObject *args)
{
    PyObject *cursor;

    if (!PyArg_ParseTuple(args, "O!:GdkWindow.set_cursor", &PyGdkCursor_Type,
			  &cursor))
	return NULL;
    gdk_window_set_cursor(self->obj, PyGdkCursor_Get(cursor));
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
PyGdkWindow_PropertyGet(PyGdkWindow_Object *self, PyObject *args)
{
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

static PyObject *
PyGdkWindow_PropertyChange(PyGdkWindow_Object *self, PyObject *args)
{
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
    if (pygtk_enum_get_value(GTK_TYPE_GDK_PROP_MODE, py_mode, (gint *)&mode))
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
		PyErr_SetString(PyExc_TypeError,
				"data not a sequence and format=16");
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
		PyErr_SetString(PyExc_TypeError,
				"data not a sequence and format=32");
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

static PyObject *
PyGdkWindow_PropertyDelete(PyGdkWindow_Object *self, PyObject *args)
{
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

static PyObject *
PyGdkWindow_Raise(PyGdkWindow_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GdkWindow._raise"))
	return NULL;
    gdk_window_raise(self->obj);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
PyGdkWindow_Lower(PyGdkWindow_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GdkWindow.lower"))
	return NULL;
    gdk_window_lower(self->obj);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
PyGdkWindow_InputGetPointer(PyGdkWindow_Object *self, PyObject *args)
{
    guint32 deviceid;
    gdouble x = 0.0, y = 0.0, pressure = 0.0, xtilt = 0.0, ytilt = 0.0;
    GdkModifierType mask = 0;

    if (!PyArg_ParseTuple(args, "i:GdkWindow.input_get_pointer", &deviceid))
	return NULL;
    gdk_input_window_get_pointer(self->obj, deviceid, &x, &y, &pressure,
				 &xtilt, &ytilt, &mask);
    return Py_BuildValue("(dddddi)", x, y, pressure, xtilt, ytilt, mask);
}

static PyMethodDef PyGdkWindow_methods[] = {
    {"new_gc", (PyCFunction)PyGdkWindow_NewGC, METH_VARARGS|METH_KEYWORDS, NULL},
    {"set_cursor", (PyCFunction)PyGdkWindow_SetCursor, METH_VARARGS, NULL},
    {"property_get", (PyCFunction)PyGdkWindow_PropertyGet, METH_VARARGS, NULL},
    {"property_change", (PyCFunction)PyGdkWindow_PropertyChange, METH_VARARGS, NULL},
    {"property_delete", (PyCFunction)PyGdkWindow_PropertyDelete, METH_VARARGS, NULL},
    {"_raise", (PyCFunction)PyGdkWindow_Raise, METH_VARARGS, NULL},
    {"lower", (PyCFunction)PyGdkWindow_Lower, METH_VARARGS, NULL},
    {"input_get_pointer", (PyCFunction)PyGdkWindow_InputGetPointer, METH_VARARGS, NULL},
    {NULL, 0, 0, NULL}
};

static PyObject *
PyGdkWindow_GetAttr(PyGdkWindow_Object *self, char *key)
{
    GdkWindow *win = PyGdkWindow_Get(self);
    gint x, y;
    GdkModifierType p_mask;

    if (!strcmp(key, "__members__"))
	return Py_BuildValue("[sssssssssssss]", "children", "colormap", "depth",
			     "height", "parent", "pointer", "pointer_state",
			     "toplevel", "type", "width", "x", "xid", "y");
    if (!strcmp(key, "width")) {
	gdk_window_get_size(win, &x, NULL);
	return PyInt_FromLong(x);
    }
    if (!strcmp(key, "height")) {
	gdk_window_get_size(win, NULL, &y);
	return PyInt_FromLong(y);
    }
    if (!strcmp(key, "x")) {
	gdk_window_get_position(win, &x, NULL);
	return PyInt_FromLong(x);
    }
    if (!strcmp(key, "y")) {
	gdk_window_get_position(win, NULL, &y);
	return PyInt_FromLong(y);
    }
    if (!strcmp(key, "colormap"))
	return PyGdkColormap_New(gdk_window_get_colormap(win));
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

    return Py_FindMethod(PyGdkWindow_methods, (PyObject *)self, key);
}

PyTypeObject PyGdkWindow_Type = {
    PyObject_HEAD_INIT(NULL)
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
PyGdkGC_Dealloc(PyGdkGC_Object *self)
{
    gdk_gc_unref(self->obj);
    PyMem_DEL(self);
}

static PyObject *
PyGdkGC_set_dashes(PyGdkGC_Object *self, PyObject *args)
{
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

static PyMethodDef PyGdkGC_methods[] = {
    {"set_dashes", (PyCFunction)PyGdkGC_set_dashes, METH_VARARGS, NULL},
    {NULL, 0, 0, NULL}
};

static PyObject *
PyGdkGC_GetAttr(PyGdkGC_Object *self, char *key)
{
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
PyGdkGC_SetAttr(PyGdkGC_Object *self, char *key, PyObject *value)
{
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
PyGdkGC_Compare(PyGdkGC_Object *self, PyGdkGC_Object *v)
{
    if (self->obj == v->obj) return 0;
    if (self->obj > v->obj) return -1;
    return 1;
}

static long
PyGdkGC_Hash(PyGdkGC_Object *self)
{
    return (long)self->obj;
}

PyTypeObject PyGdkGC_Type = {
    PyObject_HEAD_INIT(NULL)
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
#endif

static void
pygdk_visual_dealloc(PyGdkVisual_Object *self)
{
    gdk_visual_unref(self->obj);
    PyMem_DEL(self);
}

static struct memberlist pygdk_visual_members[] = {
    { "type",          T_INT, offsetof(GdkVisual, type),          READONLY },
    { "depth",         T_INT, offsetof(GdkVisual, depth),         READONLY },
    { "byte_order",    T_INT, offsetof(GdkVisual, byte_order),    READONLY },
    { "colormap_size", T_INT, offsetof(GdkVisual, colormap_size), READONLY },
    { "bits_per_rgb",  T_INT, offsetof(GdkVisual, bits_per_rgb),  READONLY },
    { "red_mask",      T_INT, offsetof(GdkVisual, red_mask),      READONLY },
    { "red_shift",     T_INT, offsetof(GdkVisual, red_shift),     READONLY },
    { "red_prec",      T_INT, offsetof(GdkVisual, red_prec),      READONLY },
    { "green_mask",    T_INT, offsetof(GdkVisual, green_mask),    READONLY },
    { "green_shift",   T_INT, offsetof(GdkVisual, green_shift),   READONLY },
    { "green_prec",    T_INT, offsetof(GdkVisual, green_prec),    READONLY },
    { "blue_mask",     T_INT, offsetof(GdkVisual, blue_mask),     READONLY },
    { "blue_shift",    T_INT, offsetof(GdkVisual, blue_shift),    READONLY },
    { "blue_prec",     T_INT, offsetof(GdkVisual, blue_prec),     READONLY },
    { NULL, 0, 0, 0 }
};

static PyObject *
pygdk_visual_getattr(PyGdkVisual_Object *self, char *attr)
{
    /* handle non int attributes first */
    if (!strcmp(attr, "type"))
	return PyInt_FromLong(self->obj->type);
    else if (!strcmp(attr, "byte_order"))
	return PyInt_FromLong(self->obj->byte_order);
    else if (!strcmp(attr, "red_mask"))
	return PyInt_FromLong(self->obj->red_mask);
    else if (!strcmp(attr, "green_mask"))
	return PyInt_FromLong(self->obj->green_mask);
    else if (!strcmp(attr, "blue_mask"))
	return PyInt_FromLong(self->obj->blue_mask);
    /* rest are all ints, so PyMember_Get should have not problems */
    return PyMember_Get((char *)self->obj, pygdk_visual_members, attr);
}

static int
pygdk_visual_compare(PyGdkVisual_Object *self, PyGdkVisual_Object *v)
{
    if (self->obj == v->obj) return 0;
    if (self->obj > v->obj) return -1;
    return 1;
}

static long
pygdk_visual_hash(PyGdkVisual_Object *self)
{
    return (long)self->obj;
}

PyTypeObject PyGdkVisual_Type = {
    PyObject_HEAD_INIT(NULL)
    0,
    "GdkVisual",
    sizeof(PyGdkVisual_Object),
    0,
    (destructor)pygdk_visual_dealloc,
    (printfunc)0,
    (getattrfunc)pygdk_visual_getattr,
    (setattrfunc)0,
    (cmpfunc)pygdk_visual_compare,
    (reprfunc)0,
    0,
    0,
    0,
    (hashfunc)pygdk_visual_hash,
    (ternaryfunc)0,
    (reprfunc)0,
    0L,0L,0L,0L,
    NULL
};

#if 0
static void
PyGdkColormap_Dealloc(PyGdkColormap_Object *self)
{
    gdk_colormap_unref(self->obj);
    PyMem_DEL(self);
}

static PyObject *
PyGdkColor_Alloc(PyGdkColormap_Object *self, PyObject *args)
{
    GdkColor color = {0, 0, 0, 0};
    gchar *color_name;
    if (!PyArg_ParseTuple(args, "iii:GdkColormap.alloc",
			  &(color.red), &(color.green), &(color.blue))) {
	PyErr_Clear();
	if (!PyArg_ParseTuple(args, "s:GdkColormap.alloc", &color_name))
	    return NULL;
	if (!gdk_color_parse(color_name, &color)) {
	    PyErr_SetString(PyExc_TypeError,
			    "unable to parse color specification");
	    return NULL;
	}
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
PyGdkColormap_GetAttr(PyObject *self, char *key)
{
    return Py_FindMethod(PyGdkColormap_methods, self, key);
}

static int
PyGdkColormap_Compare(PyGdkColormap_Object *self, PyGdkColormap_Object *v)
{
    if (self->obj == v->obj) return 0;
    if (self->obj > v->obj) return -1;
    return 1;
}

static long
PyGdkColormap_Hash(PyGdkColormap_Object *self)
{
    return (long)self->obj;
}

static int
PyGdkColormap_Length(PyGdkColormap_Object *self)
{
    return self->obj->size;
}

static PyObject *
PyGdkColormap_GetItem(PyGdkColormap_Object *self, int pos)
{
    if (pos < 0 || pos >= self->obj->size) {
	PyErr_SetString(PyExc_IndexError, "index out of range");
	return NULL;
    }
    return PyGdkColor_New(&(self->obj->colors[pos]));
}

static PyObject *
PyGdkColormap_GetSlice(PyGdkColormap_Object *self, int lo, int up)
{
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

PyTypeObject PyGdkColormap_Type = {
    PyObject_HEAD_INIT(NULL)
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
PyGdkDragContext_Dealloc(PyGdkDragContext_Object *self)
{
    gdk_drag_context_unref(self->obj);
    PyMem_DEL(self);
}

static int
PyGdkDragContext_Compare(PyGdkDragContext_Object *self,
			 PyGdkDragContext_Object *v)
{
    if (self->obj == v->obj) return 0;
    if (self->obj > v->obj) return -1;
    return 1;
}

static long
PyGdkDragContext_Hash(PyGdkDragContext_Object *self)
{
    return (long)self->obj;
}

static PyObject *
PyGdkDragContext_GetAttr(PyGdkDragContext_Object *self, char *key)
{
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

PyTypeObject PyGdkDragContext_Type = {
    PyObject_HEAD_INIT(NULL)
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
#endif

static void
pygtk_selection_data_dealloc(PyGtkSelectionData_Object *self)
{
    PyMem_DEL(self);
}

static int
pygtk_selection_data_compare(PyGtkSelectionData_Object *self,
			     PyGtkSelectionData_Object *v)
{
    if (self->obj == v->obj) return 0;
    if (self->obj > v->obj) return -1;
    return 1;
}

static long
pygtk_selection_data_hash(PyGtkSelectionData_Object *self)
{
    return (long)self->obj;
}

static PyObject *
pygtk_selection_data_set(PyGtkSelectionData_Object *self, PyObject *args)
{
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

static PyMethodDef pygtk_selection_data_methods[] = {
    {"set", (PyCFunction)pygtk_selection_data_set, METH_VARARGS, NULL},
    {NULL, 0, 0, NULL}
};

static PyObject *
pygtk_selection_data_getattr(PyGtkSelectionData_Object *self, char *key)
{
    if (!strcmp(key, "__members__"))
	return Py_BuildValue("[sssss]", "data", "format", "selection",
			     "target", "type", "length");

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

    return Py_FindMethod(pygtk_selection_data_methods, (PyObject *)self, key);
}

PyTypeObject PyGtkSelectionData_Type = {
    PyObject_HEAD_INIT(NULL)
    0,
    "GtkSelectionData",
    sizeof(PyGtkSelectionData_Object),
    0,
    (destructor)pygtk_selection_data_dealloc,
    (printfunc)0,
    (getattrfunc)pygtk_selection_data_getattr,
    (setattrfunc)0,
    (cmpfunc)pygtk_selection_data_compare,
    (reprfunc)0,
    0,
    0,
    0,
    (hashfunc)pygtk_selection_data_hash,
    (ternaryfunc)0,
    (reprfunc)0,
    0L,0L,0L,0L,
    NULL
};

static void
pygdk_atom_dealloc(PyGdkAtom_Object *self)
{
    if (self->name) g_free(self->name);
    PyMem_DEL(self);
}

static int
pygdk_atom_compare(PyGdkAtom_Object *self, PyGdkAtom_Object *v)
{
    if (self->atom == v->atom) return 0;
    if (self->atom > v->atom) return -1;
    return 1;
}

static long
pygdk_atom_hash(PyGdkAtom_Object *self)
{
    return (long)self->atom;
}

static PyObject *
pygdk_atom_repr(PyGdkAtom_Object *self)
{
    char buf[256];
    if (!self->name) self->name = gdk_atom_name(self->atom);
    g_snprintf(buf, 256, "<GdkAtom 0x%lx = '%s'>", (unsigned long)self->atom,
	       self->name?self->name:"(null)");
    return PyString_FromString(buf);
}

static int
pygdk_atom_coerce(PyObject **self, PyObject **other)
{
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
pygdk_atom_int(PyGdkAtom_Object *self)
{
    return PyInt_FromLong(self->atom);
}

static PyObject *
pygdk_atom_long(PyGdkAtom_Object *self)
{
    return PyLong_FromUnsignedLong(self->atom);
}

static PyObject *
pygdk_atom_float(PyGdkAtom_Object *self)
{
    return PyFloat_FromDouble(self->atom);
}

static PyObject *
pygdk_atom_oct(PyGdkAtom_Object *self)
{
    char buf[100];
    if (self->atom == 0) return PyString_FromString("0");
    g_snprintf(buf, 100, "0%lo", self->atom);
    return PyString_FromString(buf);
}

static PyObject *
pygdk_atom_hex(PyGdkAtom_Object *self)
{
    char buf[100];
    g_snprintf(buf, 100, "0x%lx", self->atom);
    return PyString_FromString(buf);
}

static PyObject *
pygdk_atom_str(PyGdkAtom_Object *self)
{
    if (!self->name) self->name = gdk_atom_name(self->atom);
    if (self->name)
	return PyString_FromString(self->name);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyNumberMethods pygdk_atom_number = {
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
    (coercion)pygdk_atom_coerce,
    (unaryfunc)pygdk_atom_int,
    (unaryfunc)pygdk_atom_long,
    (unaryfunc)pygdk_atom_float,
    (unaryfunc)pygdk_atom_oct,
    (unaryfunc)pygdk_atom_hex
};

PyTypeObject PyGdkAtom_Type = {
    PyObject_HEAD_INIT(NULL)
    0,
    "GdkAtom",
    sizeof(PyGdkAtom_Object),
    0,
    (destructor)pygdk_atom_dealloc,
    (printfunc)0,
    (getattrfunc)0,
    (setattrfunc)0,
    (cmpfunc)pygdk_atom_compare,
    (reprfunc)pygdk_atom_repr,
    &pygdk_atom_number,
    0,
    0,
    (hashfunc)pygdk_atom_hash,
    (ternaryfunc)0,
    (reprfunc)pygdk_atom_str,
    0L,0L,0L,0L,
    NULL
};

static void
pygdk_cursor_dealloc(PyGdkCursor_Object *self)
{
    gdk_cursor_destroy(self->obj);
    PyMem_DEL(self);
}

static int
pygdk_cursor_compare(PyGdkCursor_Object *self, PyGdkCursor_Object *v)
{
    if (self->obj == v->obj) return 0;
    if (self->obj > v->obj) return -1;
    return 1;
}

static long
pygdk_cursor_hash(PyGdkCursor_Object *self)
{
    return (long)self->obj;
}

static PyObject *
pygdk_cursor_repr(PyGdkCursor_Object *self)
{
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

static PyObject *
pygdk_cursor_getattr(PyGdkCursor_Object *self, char *attr)
{
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

PyTypeObject PyGdkCursor_Type = {
    PyObject_HEAD_INIT(NULL)
    0,
    "GdkCursor",
    sizeof(PyGdkCursor_Object),
    0,
    (destructor)pygdk_cursor_dealloc,
    (printfunc)0,
    (getattrfunc)pygdk_cursor_getattr,
    (setattrfunc)0,
    (cmpfunc)pygdk_cursor_compare,
    (reprfunc)pygdk_cursor_repr,
    0,
    0,
    0,
    (hashfunc)pygdk_cursor_hash,
    (ternaryfunc)0,
    (reprfunc)0,
    0L,0L,0L,0L,
    NULL
};

static void
pygtk_ctree_node_dealloc(PyGtkCTreeNode_Object *self)
{
    PyMem_DEL(self);
}

static int
pygtk_ctree_node_compare(PyGtkCTreeNode_Object *self, PyGtkCTreeNode_Object *v)
{
    if (self->node == v->node) return 0;
    if (self->node > v->node) return -1;
    return 1;
}

static long
pygtk_ctree_node_hash(PyGtkCTreeNode_Object *self)
{
    return (long)self->node;
}

static PyObject *
pygtk_ctree_node_getattr(PyGtkCTreeNode_Object *self, char *key)
{
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

PyTypeObject PyGtkCTreeNode_Type = {
    PyObject_HEAD_INIT(NULL)
    0,
    "GtkCTreeNode",
    sizeof(PyGtkCTreeNode_Object),
    0,
    (destructor)pygtk_ctree_node_dealloc,
    (printfunc)0,
    (getattrfunc)pygtk_ctree_node_getattr,
    (setattrfunc)0,
    (cmpfunc)pygtk_ctree_node_compare,
    (reprfunc)0,
    0,
    0,
    0,
    (hashfunc)pygtk_ctree_node_hash,
    (ternaryfunc)0,
    (reprfunc)0,
    0L,0L,0L,0L,
    NULL
};

static void
pygdk_device_dealloc(PyGdkDevice_Object *self)
{
    PyMem_DEL(self);
}

static int
pygdk_device_compare(PyGdkDevice_Object *self, PyGdkDevice_Object *v)
{
    if (self->obj == v->obj) return 0;
    if (self->obj > v->obj) return -1;
    return 1;
}

static long
pygdk_device_hash(PyGdkDevice_Object *self)
{
    return (long)self->obj;
}

static PyObject *
pygdk_device_set_source(PyGdkDevice_Object *self, PyObject *args)
{
    GdkInputSource source;

    if (!PyArg_ParseTuple(args, "i:GdkDevice.set_source", &source))
	return NULL;
    gdk_device_set_source(self->obj, source);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
pygdk_device_set_mode(PyGdkDevice_Object *self, PyObject *args)
{
    GdkInputMode mode;

    if (!PyArg_ParseTuple(args, "i:GdkDevice.set_mode", &mode))
	return NULL;
    gdk_device_set_mode(self->obj, mode);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
pygdk_device_set_key(PyGdkDevice_Object *self, PyObject *args)
{
    guint index, keyval;
    GdkModifierType modifiers;

    if (!PyArg_ParseTuple(args, "iii:GdkDevice.set_key", &index, &keyval,
			  &modifiers))
	return NULL;
    gdk_device_set_key(self->obj, index, keyval, modifiers);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
pygdk_device_set_axis_use(PyGdkDevice_Object *self, PyObject *args)
{
    guint index;
    GdkAxisUse use;

    if (!PyArg_ParseTuple(args, "ii:GdkDevice.set_axis_use", &index, &use))
	return NULL;
    gdk_device_set_axis_use(self->obj, index, use);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
pygdk_device_get_state(PyGdkDevice_Object *self, PyObject *args)
{
    PyGObject *window;
    gdouble *axes;
    GdkModifierType mask;
    PyObject *pyaxes;
    guint i;
    extern PyExtensionClass PyGdkWindow_Type;

    if (!PyArg_ParseTuple(args, "O:GdkDevice.get_state", &window))
	return NULL;
    if (!pygobject_check(window, &PyGdkWindow_Type)) {
	PyErr_SetString(PyExc_TypeError, "window should be a GdkWindow");
	return NULL;
    }
    axes = g_new0(gdouble, self->obj->num_axes);
    gdk_device_get_state(self->obj, GDK_WINDOW(window->obj), axes, &mask);
    pyaxes = PyTuple_New(self->obj->num_axes);
    for (i = 0; i < self->obj->num_axes; i++)
	PyTuple_SetItem(pyaxes, i, PyFloat_FromDouble(axes[i]));
    g_free(axes);
    return Py_BuildValue("(Ni)", pyaxes, mask);
}

static PyObject *
pygdk_device_get_history(PyGdkDevice_Object *self, PyObject *args)
{
    PyGObject *window;
    guint start, stop;
    GdkTimeCoord **events;
    gint n_events;
    PyObject *pyevents;
    guint i;
    extern PyExtensionClass PyGdkWindow_Type;

    if (!PyArg_ParseTuple(args, "Oii:GdkDevice.get_history", &window,
			  &start, &stop))
	return NULL;
    if (!pygobject_check(window, &PyGdkWindow_Type)) {
	PyErr_SetString(PyExc_TypeError, "window should be a GdkWindow");
	return NULL;
    }
    gdk_device_get_history(self->obj, GDK_WINDOW(window->obj), start, stop,
			   &events, &n_events);
    pyevents = PyTuple_New(n_events);
    for (i = 0; i < n_events; i++) {
	PyObject *axes;
	gint j;

	axes = PyTuple_New(self->obj->num_axes);
	for (j = 0; j < self->obj->num_axes; j++)
	    PyTuple_SetItem(axes, j, PyFloat_FromDouble(events[i]->axes[j]));
	PyTuple_SetItem(pyevents, i, Py_BuildValue("(iN)", events[i]->time,
						 axes));
    }
    gdk_device_free_history(events, n_events);
    return pyevents;
}

static PyObject *
pygdk_device_get_axis(PyGdkDevice_Object *self, PyObject *args)
{
    PyObject *pyaxes;
    gdouble *axes, value;
    GdkAxisUse use;
    gboolean ret;
    gint i;

    if (!PyArg_ParseTuple(args, "Oi:GdkDevice.get_axis", &pyaxes, &use))
	return NULL;
    if (!PySequence_Check(pyaxes)) {
	PyErr_SetString(PyExc_TypeError, "axes must be a sequence");
	return NULL;
    }
    if (PySequence_Length(pyaxes) != self->obj->num_axes) {
	PyErr_SetString(PyExc_TypeError, "axes has the wrong length");
	return NULL;
    }
    axes = g_new(gdouble, self->obj->num_axes);
    for (i = 0; i < self->obj->num_axes; i++) {
	PyObject *item = PySequence_GetItem(pyaxes, i);

	axes[i] = PyFloat_AsDouble(item);
	Py_DECREF(item);
	if (PyErr_Occurred()) {
	    g_free(axes);
	    return NULL;
	}
    }
    ret = gdk_device_get_axis(self->obj, axes, use, &value);
    g_free(axes);
    if (ret)
	return PyFloat_FromDouble(value);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef pygdk_device_methods[] = {
    { "set_source", (PyCFunction)pygdk_device_set_source, METH_VARARGS },
    { "set_mode", (PyCFunction)pygdk_device_set_mode, METH_VARARGS },
    { "set_key", (PyCFunction)pygdk_device_set_key, METH_VARARGS },
    { "set_axis_use", (PyCFunction)pygdk_device_set_axis_use, METH_VARARGS },
    { "get_state", (PyCFunction)pygdk_device_get_state, METH_VARARGS },
    { "get_history", (PyCFunction)pygdk_device_get_history, METH_VARARGS },
    { "get_axis", (PyCFunction)pygdk_device_get_axis, METH_VARARGS },
    { NULL, NULL, 0 }
};

static PyObject *
pygdk_device_getattr(PyGdkDevice_Object *self, char *attr)
{
    if (!strcmp(attr, "__members__"))
	return Py_BuildValue("[ssssss]", "axes", "has_cursor", "keys",
			     "mode", "name", "source");
    else if (!strcmp(attr, "name"))
	return PyString_FromString(self->obj->name);
    else if (!strcmp(attr, "source"))
	return PyInt_FromLong(self->obj->source);
    else if (!strcmp(attr, "mode"))
	return PyInt_FromLong(self->obj->mode);
    else if (!strcmp(attr, "axes")) {
	PyObject *ret;
	gint i;

	ret = PyTuple_New(self->obj->num_axes);
	for (i = 0; i < self->obj->num_axes; i++)
	    PyTuple_SetItem(ret, i, Py_BuildValue("(idd)",
						  self->obj->axes[i].use,
						  self->obj->axes[i].min,
						  self->obj->axes[i].max));
	return ret;
    } else if (!strcmp(attr, "keys")) {
	PyObject *ret;
	gint i;

	ret = PyTuple_New(self->obj->num_keys);
	for (i = 0; i < self->obj->num_keys; i++)
	    PyTuple_SetItem(ret, i, Py_BuildValue("(ii)",
						self->obj->keys[i].keyval,
						self->obj->keys[i].modifiers));
	return ret;
    }
    return Py_FindMethod(pygdk_device_methods, (PyObject *)self, attr);
}

PyTypeObject PyGdkDevice_Type = {
    PyObject_HEAD_INIT(NULL)
    0,
    "GdkDevice",
    sizeof(PyGdkDevice_Object),
    0,
    (destructor)pygdk_device_dealloc,
    (printfunc)0,
    (getattrfunc)pygdk_device_getattr,
    (setattrfunc)0,
    (cmpfunc)pygdk_device_compare,
    (reprfunc)0,
    0,
    0,
    0,
    (hashfunc)pygdk_device_hash,
    (ternaryfunc)0,
    (reprfunc)0,
    0L,0L,0L,0L,
    NULL
};

static void
pygtk_text_iter_dealloc(PyGtkTextIter_Object *self)
{
    PyMem_DEL(self);
}

static int
pygtk_text_iter_compare(PyGtkTextIter_Object *self, PyGtkTextIter_Object *v)
{
    return gtk_text_iter_compare(&self->iter, &v->iter);
}

static long
pygtk_text_iter_hash(PyGtkTextIter_Object *self)
{
    long x;

    /* a hash function for the GtkTextIter object */
    x = 0x345678L;
    x = (1000003*x) ^ (long)self->iter.dummy1;
    x = (1000003*x) ^ (long)self->iter.dummy2;
    x = (1000003*x) ^ (long)self->iter.dummy3;
    x = (1000003*x) ^ (long)self->iter.dummy4;
    x = (1000003*x) ^ (long)self->iter.dummy5;
    x = (1000003*x) ^ (long)self->iter.dummy6;
    x = (1000003*x) ^ (long)self->iter.dummy7;
    x = (1000003*x) ^ (long)self->iter.dummy8;
    x = (1000003*x) ^ (long)self->iter.dummy9;
    x = (1000003*x) ^ (long)self->iter.dummy10;
    x = (1000003*x) ^ (long)self->iter.dummy11;
    x = (1000003*x) ^ (long)self->iter.dummy12;
    if (x == -1) /* -1 indicates error condition */
	x = -2;
    return x;
}

static PyObject *
pygtk_text_iter_copy(PyGtkTextIter_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkTextIter.copy"))
	return NULL;
    /* don't use gtk_text_iter_copy, as it makes a new copy of the
     * iter, and PyGtkTextIter_New does the same when creating the new
     * PyObject */
    return PyGtkTextIter_New(&self->iter);
}

static PyObject *
pygtk_text_iter_get_offset(PyGtkTextIter_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkTextIter.get_offset"))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_get_offset(&self->iter));
}

static PyObject *
pygtk_text_iter_get_line(PyGtkTextIter_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkTextIter.get_line"))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_get_line(&self->iter));
}

static PyObject *
pygtk_text_iter_get_line_offset(PyGtkTextIter_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkTextIter.get_line_offset"))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_get_line_offset(&self->iter));
}

static PyObject *
pygtk_text_iter_get_line_index(PyGtkTextIter_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkTextIter.get_line_index"))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_get_line_index(&self->iter));
}

static PyObject *
pygtk_text_iter_get_char(PyGtkTextIter_Object *self, PyObject *args)
{
    gunichar chr;
    Py_UNICODE pychr;

    if (!PyArg_ParseTuple(args, ":GtkTextIter.get_char"))
	return NULL;
    chr = gtk_text_iter_get_char(&self->iter);
    if (chr > 0xffff) {
	PyErr_SetString(PyExc_RuntimeError, "python's broken unicode handling only supports 16 bit unicode, and you just hit this limit");
	return NULL;
    }
    pychr = (Py_UNICODE)chr;
    return PyUnicode_FromUnicode(&pychr, 1);
}

static PyObject *
pygtk_text_iter_get_slice(PyGtkTextIter_Object *self, PyObject *args)
{
    PyGtkTextIter_Object *end;
    gchar *text;
    PyObject *pytext;

    if (!PyArg_ParseTuple(args, "O!:GtkTextIter.get_slice",
			  &PyGtkTextIter_Type, &end))
	return NULL;
    text = gtk_text_iter_get_slice(&self->iter, &end->iter);
    pytext = PyUnicode_DecodeUTF8(text, strlen(text), "strict");
    g_free(text);
    return pytext;
}

static PyObject *
pygtk_text_iter_get_text(PyGtkTextIter_Object *self, PyObject *args)
{
    PyGtkTextIter_Object *end;
    gchar *text;
    PyObject *pytext;

    if (!PyArg_ParseTuple(args, "O!:GtkTextIter.get_text",
			  &PyGtkTextIter_Type, &end))
	return NULL;
    text = gtk_text_iter_get_text(&self->iter, &end->iter);
    pytext = PyUnicode_DecodeUTF8(text, strlen(text), "strict");
    g_free(text);
    return pytext;
}

static PyObject *
pygtk_text_iter_get_visible_slice(PyGtkTextIter_Object *self, PyObject *args)
{
    PyGtkTextIter_Object *end;
    gchar *text;
    PyObject *pytext;

    if (!PyArg_ParseTuple(args, "O!:GtkTextIter.get_visible_slice",
			  &PyGtkTextIter_Type, &end))
	return NULL;
    text = gtk_text_iter_get_visible_slice(&self->iter, &end->iter);
    pytext = PyUnicode_DecodeUTF8(text, strlen(text), "strict");
    g_free(text);
    return pytext;
}

static PyObject *
pygtk_text_iter_get_visible_text(PyGtkTextIter_Object *self, PyObject *args)
{
    PyGtkTextIter_Object *end;
    gchar *text;
    PyObject *pytext;

    if (!PyArg_ParseTuple(args, "O!:GtkTextIter.get_visible_text",
			  &PyGtkTextIter_Type, &end))
	return NULL;
    text = gtk_text_iter_get_visible_text(&self->iter, &end->iter);
    pytext = PyUnicode_DecodeUTF8(text, strlen(text), "strict");
    g_free(text);
    return pytext;
}

static PyObject *
pygtk_text_iter_get_pixbuf(PyGtkTextIter_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkTextIter.get_pixbuf"))
	return NULL;
    return pygobject_new((GObject *)gtk_text_iter_get_pixbuf(&self->iter));
}

static PyObject *
pygtk_text_iter_get_marks(PyGtkTextIter_Object *self, PyObject *args)
{
    GSList *ret, *tmp;
    PyObject *pyret;

    if (!PyArg_ParseTuple(args, ":GtkTextIter.get_marks"))
	return NULL;
    ret = gtk_text_iter_get_marks(&self->iter);
    pyret = PyList_New(0);
    for (tmp = ret; tmp != NULL; tmp = tmp->next) {
	PyObject *mark = pygobject_new((GObject *)tmp->data);
	PyList_Append(pyret, mark);
	Py_DECREF(mark);
    }
    g_slist_free(ret);
    return pyret;
}

static PyObject *
pygtk_text_iter_get_toggled_tags(PyGtkTextIter_Object *self, PyObject *args)
{
    GSList *ret, *tmp;
    int toggled_on;
    PyObject *pyret;

    if (!PyArg_ParseTuple(args, "i:GtkTextIter.get_toggled_tags", &toggled_on))
	return NULL;
    ret = gtk_text_iter_get_toggled_tags(&self->iter, toggled_on);
    pyret = PyList_New(0);
    for (tmp = ret; tmp != NULL; tmp = tmp->next) {
	PyObject *tag = pygobject_new((GObject *)tmp->data);
	PyList_Append(pyret, tag);
	Py_DECREF(tag);
    }
    g_slist_free(ret);
    return pyret;
}

static PyObject *
pygtk_text_iter_begins_tag(PyGtkTextIter_Object *self, PyObject *args)
{
    PyGObject *tag;
    extern PyExtensionClass PyGtkTextTag_Type;

    if (!PyArg_ParseTuple(args, "O:GtkTextIter.begins_tag", &tag))
	return NULL;
    if (!pygobject_check(tag, &PyGtkTextTag_Type)) {
	PyErr_SetString(PyExc_TypeError, "tag should be a GtkTextTag");
	return NULL;
    }
    return PyInt_FromLong(gtk_text_iter_begins_tag(&self->iter,
						   GTK_TEXT_TAG(tag->obj)));
}

static PyObject *
pygtk_text_iter_ends_tag(PyGtkTextIter_Object *self, PyObject *args)
{
    PyGObject *tag;
    extern PyExtensionClass PyGtkTextTag_Type;

    if (!PyArg_ParseTuple(args, "O:GtkTextIter.ends_tag", &tag))
	return NULL;
    if (!pygobject_check(tag, &PyGtkTextTag_Type)) {
	PyErr_SetString(PyExc_TypeError, "tag should be a GtkTextTag");
	return NULL;
    }
    return PyInt_FromLong(gtk_text_iter_ends_tag(&self->iter,
						 GTK_TEXT_TAG(tag->obj)));
}

static PyObject *
pygtk_text_iter_toggles_tag(PyGtkTextIter_Object *self, PyObject *args)
{
    PyGObject *tag;
    extern PyExtensionClass PyGtkTextTag_Type;

    if (!PyArg_ParseTuple(args, "O:GtkTextIter.toggles_tag", &tag))
	return NULL;
    if (!pygobject_check(tag, &PyGtkTextTag_Type)) {
	PyErr_SetString(PyExc_TypeError, "tag should be a GtkTextTag");
	return NULL;
    }
    return PyInt_FromLong(gtk_text_iter_toggles_tag(&self->iter,
						    GTK_TEXT_TAG(tag->obj)));
}

static PyObject *
pygtk_text_iter_has_tag(PyGtkTextIter_Object *self, PyObject *args)
{
    PyGObject *tag;
    extern PyExtensionClass PyGtkTextTag_Type;

    if (!PyArg_ParseTuple(args, "O:GtkTextIter.has_tag", &tag))
	return NULL;
    if (!pygobject_check(tag, &PyGtkTextTag_Type)) {
	PyErr_SetString(PyExc_TypeError, "tag should be a GtkTextTag");
	return NULL;
    }
    return PyInt_FromLong(gtk_text_iter_has_tag(&self->iter,
						GTK_TEXT_TAG(tag->obj)));
}

static PyObject *
pygtk_text_iter_editable(PyGtkTextIter_Object *self, PyObject *args)
{
    int default_setting;

    if (!PyArg_ParseTuple(args, "i:GtkTextIter.editable", &default_setting))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_editable(&self->iter,default_setting));
}

static PyObject *
pygtk_text_iter_starts_line(PyGtkTextIter_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkTextIter.starts_line"))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_starts_line(&self->iter));
}

static PyObject *
pygtk_text_iter_ends_line(PyGtkTextIter_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkTextIter.ends_line"))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_ends_line(&self->iter));
}

static PyObject *
pygtk_text_iter_get_chars_in_line(PyGtkTextIter_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkTextIter.get_chars_in_line"))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_starts_line(&self->iter));
}

/* XXXX - gtk_text_iter_get_attributes */

static PyObject *
pygtk_text_iter_is_last(PyGtkTextIter_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkTextIter.is_last"))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_is_last(&self->iter));
}

static PyObject *
pygtk_text_iter_is_first(PyGtkTextIter_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkTextIter.is_first"))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_is_first(&self->iter));
}

static PyObject *
pygtk_text_iter_forward_char(PyGtkTextIter_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkTextIter.forward_char"))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_forward_char(&self->iter));
}

static PyObject *
pygtk_text_iter_backward_char(PyGtkTextIter_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkTextIter.backward_char"))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_backward_char(&self->iter));
}

static PyObject *
pygtk_text_iter_forward_chars(PyGtkTextIter_Object *self, PyObject *args)
{
    gint count;

    if (!PyArg_ParseTuple(args, "i:GtkTextIter.forward_chars", &count))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_forward_chars(&self->iter, count));
}

static PyObject *
pygtk_text_iter_backward_chars(PyGtkTextIter_Object *self, PyObject *args)
{
    gint count;

    if (!PyArg_ParseTuple(args, "i:GtkTextIter.backward_chars", &count))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_backward_chars(&self->iter, count));
}

static PyObject *
pygtk_text_iter_forward_line(PyGtkTextIter_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkTextIter.forward_line"))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_forward_line(&self->iter));
}

static PyObject *
pygtk_text_iter_backward_line(PyGtkTextIter_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkTextIter.backward_line"))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_backward_line(&self->iter));
}

static PyObject *
pygtk_text_iter_forward_lines(PyGtkTextIter_Object *self, PyObject *args)
{
    gint count;

    if (!PyArg_ParseTuple(args, "i:GtkTextIter.forward_lines", &count))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_forward_lines(&self->iter, count));
}

static PyObject *
pygtk_text_iter_backward_lines(PyGtkTextIter_Object *self, PyObject *args)
{
    gint count;

    if (!PyArg_ParseTuple(args, "i:GtkTextIter.backward_lines", &count))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_backward_lines(&self->iter, count));
}

static PyObject *
pygtk_text_iter_forward_word_ends(PyGtkTextIter_Object *self, PyObject *args)
{
    gint count;

    if (!PyArg_ParseTuple(args, "i:GtkTextIter.forward_word_ends", &count))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_forward_word_ends(&self->iter, count));
}

static PyObject *
pygtk_text_iter_backward_word_starts(PyGtkTextIter_Object *self, PyObject *args)
{
    gint count;

    if (!PyArg_ParseTuple(args, "i:GtkTextIter.backward_word_starts", &count))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_backward_word_starts(&self->iter, count));
}

static PyObject *
pygtk_text_iter_forward_word_end(PyGtkTextIter_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkTextIter.forward_word_end"))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_forward_word_end(&self->iter));
}

static PyObject *
pygtk_text_iter_backward_word_start(PyGtkTextIter_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkTextIter.backward_word_start"))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_backward_word_start(&self->iter));
}

static PyObject *
pygtk_text_iter_set_offset(PyGtkTextIter_Object *self, PyObject *args)
{
    int char_offset;

    if (!PyArg_ParseTuple(args, "i:GtkTextIter.set_offset", &char_offset))
	return NULL;
    gtk_text_iter_set_offset(&self->iter, char_offset);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
pygtk_text_iter_set_line(PyGtkTextIter_Object *self, PyObject *args)
{
    int line_number;

    if (!PyArg_ParseTuple(args, "i:GtkTextIter.set_line", &line_number))
	return NULL;
    gtk_text_iter_set_line(&self->iter, line_number);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
pygtk_text_iter_set_line_offset(PyGtkTextIter_Object *self, PyObject *args)
{
    int char_on_line;

    if (!PyArg_ParseTuple(args, "i:GtkTextIter.set_line_offset",&char_on_line))
	return NULL;
    gtk_text_iter_set_line_offset(&self->iter, char_on_line);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
pygtk_text_iter_set_line_index(PyGtkTextIter_Object *self, PyObject *args)
{
    int byte_on_line;

    if (!PyArg_ParseTuple(args, "i:GtkTextIter.set_line_index", &byte_on_line))
	return NULL;
    gtk_text_iter_set_line_index(&self->iter, byte_on_line);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
pygtk_text_iter_forward_to_end(PyGtkTextIter_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkTextIter.forward_to_end"))
	return NULL;
    gtk_text_iter_forward_to_end(&self->iter);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
pygtk_text_iter_forward_to_newline(PyGtkTextIter_Object *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ":GtkTextIter.forward_to_newline"))
	return NULL;
    return PyInt_FromLong(gtk_text_iter_forward_to_newline(&self->iter));
}

static PyObject *
pygtk_text_iter_forward_to_tag_toggle(PyGtkTextIter_Object *self,
				      PyObject *args)
{
    PyObject *pytag = Py_None;
    GtkTextTag *tag = NULL;
    extern PyExtensionClass PyGtkTextTag_Type;

    if (!PyArg_ParseTuple(args, "|O:GtkTextIter.forward_to_tag_toggle",
			  &pytag))
	return NULL;
    if (pygobject_check(pytag, &PyGtkTextTag_Type))
	tag = GTK_TEXT_TAG(pygobject_get(pytag));
    else if (pytag != Py_None) {
	PyErr_SetString(PyExc_TypeError, "tag must be a GtkTextTag or None");
	return NULL;
    }
    return PyInt_FromLong(gtk_text_iter_forward_to_tag_toggle(&self->iter,
							      tag));
}

static PyObject *
pygtk_text_iter_backward_to_tag_toggle(PyGtkTextIter_Object *self,
				       PyObject *args)
{
    PyObject *pytag = Py_None;
    GtkTextTag *tag = NULL;
    extern PyExtensionClass PyGtkTextTag_Type;

    if (!PyArg_ParseTuple(args, "|O:GtkTextIter.backward_to_tag_toggle",
			  &pytag))
	return NULL;
    if (pygobject_check(pytag, &PyGtkTextTag_Type))
	tag = GTK_TEXT_TAG(pygobject_get(pytag));
    else if (pytag != Py_None) {
	PyErr_SetString(PyExc_TypeError, "tag must be a GtkTextTag or None");
	return NULL;
    }
    return PyInt_FromLong(gtk_text_iter_backward_to_tag_toggle(&self->iter,
							       tag));
}

/* XXXX - gtk_text_iter_{fore,back}ward_find_char */

static PyObject *
pygtk_text_iter_forward_search(PyGtkTextIter_Object *self, PyObject *args)
{
    gchar *str;
    gint visible_only, slice;
    PyObject *py_limit = Py_None;
    GtkTextIter match_start, match_end, *limit = NULL;

    if (!PyArg_ParseTuple(args, "sii|O:GtkTextIter.forward_search",
			  &str, &visible_only, &slice, &py_limit))
	return NULL;
    if (PyGtkTextIter_Check(py_limit))
	limit = PyGtkTextIter_Get(py_limit);
    else if (py_limit != Py_None) {
	PyErr_SetString(PyExc_TypeError,"limit must be a GtkTextIter or None");
	return NULL;
    }
    if (gtk_text_iter_forward_search(&self->iter, str, visible_only, slice,
				     &match_start, &match_end, limit))
	return Py_BuildValue("(NN)", PyGtkTextIter_New(&match_start),
			     PyGtkTextIter_New(&match_end));
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
pygtk_text_iter_backward_search(PyGtkTextIter_Object *self, PyObject *args)
{
    gchar *str;
    gint visible_only, slice;
    PyObject *py_limit = Py_None;
    GtkTextIter match_start, match_end, *limit = NULL;

    if (!PyArg_ParseTuple(args, "sii|O:GtkTextIter.backward_search",
			  &str, &visible_only, &slice, &py_limit))
	return NULL;
    if (PyGtkTextIter_Check(py_limit))
	limit = PyGtkTextIter_Get(py_limit);
    else if (py_limit != Py_None) {
	PyErr_SetString(PyExc_TypeError,"limit must be a GtkTextIter or None");
	return NULL;
    }
    if (gtk_text_iter_backward_search(&self->iter, str, visible_only, slice,
				     &match_start, &match_end, limit))
	return Py_BuildValue("(NN)", PyGtkTextIter_New(&match_start),
			     PyGtkTextIter_New(&match_end));
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef pygtk_text_iter_methods[] = {
    { "copy", (PyCFunction)pygtk_text_iter_copy, METH_VARARGS },
    { "get_offset", (PyCFunction)pygtk_text_iter_get_offset, METH_VARARGS },
    { "get_line", (PyCFunction)pygtk_text_iter_get_line, METH_VARARGS },
    { "get_line_offset", (PyCFunction)pygtk_text_iter_get_line_offset, METH_VARARGS },
    { "get_line_index", (PyCFunction)pygtk_text_iter_get_line_index, METH_VARARGS },
    { "get_char", (PyCFunction)pygtk_text_iter_get_char, METH_VARARGS },
    { "get_slice", (PyCFunction)pygtk_text_iter_get_slice, METH_VARARGS },
    { "get_text", (PyCFunction)pygtk_text_iter_get_text, METH_VARARGS },
    { "get_visible_slice", (PyCFunction)pygtk_text_iter_get_visible_slice, METH_VARARGS },
    { "get_visible_text", (PyCFunction)pygtk_text_iter_get_visible_text, METH_VARARGS },
    { "get_pixbuf", (PyCFunction)pygtk_text_iter_get_pixbuf, METH_VARARGS },
    { "get_marks", (PyCFunction)pygtk_text_iter_get_marks, METH_VARARGS },
    { "get_toggled_tags", (PyCFunction)pygtk_text_iter_get_toggled_tags, METH_VARARGS },
    { "begins_tag", (PyCFunction)pygtk_text_iter_begins_tag, METH_VARARGS },
    { "ends_tag", (PyCFunction)pygtk_text_iter_ends_tag, METH_VARARGS },
    { "toggles_tag", (PyCFunction)pygtk_text_iter_toggles_tag, METH_VARARGS },
    { "has_tag", (PyCFunction)pygtk_text_iter_has_tag, METH_VARARGS },
    { "editable", (PyCFunction)pygtk_text_iter_editable, METH_VARARGS },
    { "starts_line", (PyCFunction)pygtk_text_iter_starts_line, METH_VARARGS },
    { "ends_line", (PyCFunction)pygtk_text_iter_ends_line, METH_VARARGS },
    { "get_chars_in_line", (PyCFunction)pygtk_text_iter_get_chars_in_line, METH_VARARGS },
    { "is_last", (PyCFunction)pygtk_text_iter_is_last, METH_VARARGS },
    { "is_first", (PyCFunction)pygtk_text_iter_is_first, METH_VARARGS },
    { "forward_char", (PyCFunction)pygtk_text_iter_forward_char, METH_VARARGS },
    { "backward_char", (PyCFunction)pygtk_text_iter_backward_char, METH_VARARGS },
    { "forward_chars", (PyCFunction)pygtk_text_iter_forward_chars, METH_VARARGS },
    { "backward_chars", (PyCFunction)pygtk_text_iter_backward_chars, METH_VARARGS },
    { "forward_line", (PyCFunction)pygtk_text_iter_forward_line, METH_VARARGS },
    { "backward_line", (PyCFunction)pygtk_text_iter_backward_line, METH_VARARGS },
    { "forward_lines", (PyCFunction)pygtk_text_iter_forward_lines, METH_VARARGS },
    { "backward_lines", (PyCFunction)pygtk_text_iter_backward_lines, METH_VARARGS },
    { "forward_word_ends", (PyCFunction)pygtk_text_iter_forward_word_ends, METH_VARARGS },
    { "backward_word_starts", (PyCFunction)pygtk_text_iter_backward_word_starts, METH_VARARGS },
    { "forward_word_end", (PyCFunction)pygtk_text_iter_forward_word_end, METH_VARARGS },
    { "backward_word_start", (PyCFunction)pygtk_text_iter_backward_word_start, METH_VARARGS },
    { "set_offset", (PyCFunction)pygtk_text_iter_set_offset, METH_VARARGS },
    { "set_line", (PyCFunction)pygtk_text_iter_set_line, METH_VARARGS },
    { "set_line_offset", (PyCFunction)pygtk_text_iter_set_line_offset, METH_VARARGS },
    { "set_line_index", (PyCFunction)pygtk_text_iter_set_line_index, METH_VARARGS },
    { "forward_to_end", (PyCFunction)pygtk_text_iter_forward_to_end, METH_VARARGS },
    { "forward_to_newline", (PyCFunction)pygtk_text_iter_forward_to_newline, METH_VARARGS },
    { "forward_to_tag_toggle", (PyCFunction)pygtk_text_iter_forward_to_tag_toggle, METH_VARARGS },
    { "backward_to_tag_toggle", (PyCFunction)pygtk_text_iter_backward_to_tag_toggle, METH_VARARGS },
    { "forward_search", (PyCFunction)pygtk_text_iter_forward_search, METH_VARARGS },
    { "backward_search", (PyCFunction)pygtk_text_iter_backward_search, METH_VARARGS },
    { NULL, NULL, 0 }
};

static PyObject *
pygtk_text_iter_getattr(PyGtkTextIter_Object *self, char *attr)
{
    return Py_FindMethod(pygtk_text_iter_methods, (PyObject *)self, attr);
}

PyTypeObject PyGtkTextIter_Type = {
    PyObject_HEAD_INIT(NULL)
    0,
    "GtkTextIter",
    sizeof(PyGtkTextIter_Object),
    0,
    (destructor)pygtk_text_iter_dealloc,
    (printfunc)0,
    (getattrfunc)pygtk_text_iter_getattr,
    (setattrfunc)0,
    (cmpfunc)pygtk_text_iter_compare,
    (reprfunc)0,
    0,
    0,
    0,
    (hashfunc)pygtk_text_iter_hash,
    (ternaryfunc)0,
    (reprfunc)0,
    0L,0L,0L,0L,
    NULL
};

static void
pygtk_tree_iter_dealloc(PyGtkTreeIter_Object *self)
{
    PyMem_DEL(self);
}

static int
pygtk_tree_iter_compare(PyGtkTreeIter_Object *self, PyGtkTreeIter_Object *v)
{
    if (self->iter.tree_node == v->iter.tree_node) return 0;
    if (self->iter.tree_node > v->iter.tree_node) return -1;
    return 1;
}
static long
pygtk_tree_iter_hash(PyGtkTreeIter_Object *self)
{
    return (long)self->iter.tree_node;
}

PyTypeObject PyGtkTreeIter_Type = {
    PyObject_HEAD_INIT(NULL)
    0,
    "GtkTreeIter",
    sizeof(PyGtkTreeIter_Object),
    0,
    (destructor)pygtk_tree_iter_dealloc,
    (printfunc)0,
    (getattrfunc)0,
    (setattrfunc)0,
    (cmpfunc)pygtk_tree_iter_compare,
    (reprfunc)0,
    0,
    0,
    0,
    (hashfunc)pygtk_tree_iter_hash,
    (ternaryfunc)0,
    (reprfunc)0,
    0L,0L,0L,0L,
    NULL
};

PyObject *
pygtk_tree_path_to_pyobject(GtkTreePath *path)
{
    gint len, i, *indices;
    PyObject *ret;

    len = gtk_tree_path_get_depth(path);
    indices = gtk_tree_path_get_indices(path);
    ret = PyTuple_New(len);
    for (i = 0; i < len; i++)
	PyTuple_SetItem(ret, i, PyInt_FromLong(indices[i]));
    return ret;
}

GtkTreePath *
pygtk_tree_path_from_pyobject(PyObject *object)
{
    if (PyTuple_Check(object)) {
	GtkTreePath *path;
	guint len, i;

	len = PyTuple_Size(object);
	if (len < 1)
	    return NULL;
	path = gtk_tree_path_new();
	for (i = 0; i < len; i++) {
	    PyObject *item = PyTuple_GetItem(object, i);
	    gint index = PyInt_AsLong(item);
	    if (PyErr_Occurred()) {
		gtk_tree_path_free(path);
		PyErr_Clear();
		return NULL;
	    }
	    gtk_tree_path_append_index(path, index);
	}
	return path;
    }
    return NULL;
}

/* marshalers for the boxed types.  Uses uppercase notation so that
 * the macro below can automatically install them. */
static PyObject *
PyGtkAccelGroup_from_value(const GValue *value)
{
    return PyGtkAccelGroup_New(g_value_get_boxed(value));
}
static int
PyGtkAccelGroup_to_value(GValue *value, PyObject *object)
{
    if (PyGtkAccelGroup_Check(object)) {
	g_value_set_boxed(value, PyGtkAccelGroup_Get(object));
	return 0;
    }
    return -1;
}
static PyObject *
PyGdkFont_from_value(const GValue *value)
{
    return PyGdkFont_New(g_value_get_boxed(value));
}
static int
PyGdkFont_to_value(GValue *value, PyObject *object)
{
    if (PyGdkFont_Check(object)) {
	g_value_set_boxed(value, PyGdkFont_Get(object));
	return 0;
    }
    return -1;
}
static PyObject *
PyGdkColor_from_value(const GValue *value)
{
    return PyGdkColor_New(g_value_get_boxed(value));
}
static int
PyGdkColor_to_value(GValue *value, PyObject *object)
{
    if (PyGdkColor_Check(object)) {
	g_value_set_boxed(value, PyGdkColor_Get(object));
	return 0;
    }
    return -1;
}
static PyObject *
PyGdkEvent_from_value(const GValue *value)
{
    return PyGdkEvent_New(g_value_get_boxed(value));
}
static int
PyGdkEvent_to_value(GValue *value, PyObject *object)
{
    if (PyGdkEvent_Check(object)) {
	g_value_set_boxed(value, PyGdkEvent_Get(object));
	return 0;
    }
    return -1;
}
static PyObject *
PyGdkVisual_from_value(const GValue *value)
{
    return PyGdkVisual_New(g_value_get_boxed(value));
}
static int
PyGdkVisual_to_value(GValue *value, PyObject *object)
{
    if (PyGdkVisual_Check(object)) {
	g_value_set_boxed(value, PyGdkVisual_Get(object));
	return 0;
    }
    return -1;
}
static PyObject *
PyGtkSelectionData_from_value(const GValue *value)
{
    return PyGtkSelectionData_New(g_value_get_boxed(value));
}
static int
PyGtkSelectionData_to_value(GValue *value, PyObject *object)
{
    if (PyGtkSelectionData_Check(object)) {
	g_value_set_boxed(value, PyGtkSelectionData_Get(object));
	return 0;
    }
    return -1;
}
static PyObject *
PyGtkTextIter_from_value(const GValue *value)
{
    return PyGtkTextIter_New(g_value_get_boxed(value));
}
static int
PyGtkTextIter_to_value(GValue *value, PyObject *object)
{
    if (PyGtkTextIter_Check(object)) {
	g_value_set_boxed(value, PyGtkTextIter_Get(object));
	return 0;
    }
    return -1;
}
static PyObject *
PyGtkTreeIter_from_value(const GValue *value)
{
    return PyGtkTreeIter_New(g_value_get_boxed(value));
}
static int
PyGtkTreeIter_to_value(GValue *value, PyObject *object)
{
    if (PyGtkTreeIter_Check(object)) {
	g_value_set_boxed(value, PyGtkTreeIter_Get(object));
	return 0;
    }
    return -1;
}
static PyObject *
PyGtkTreePath_from_value(const GValue *value)
{
    GtkTreePath *path = (GtkTreePath *)g_value_get_boxed(value);

    return pygtk_tree_path_to_pyobject(path);
}
static int
PyGtkTreePath_to_value(GValue *value, PyObject *object)
{
    GtkTreePath *path = pygtk_tree_path_from_pyobject(object);

    if (path) {
	g_value_set_boxed(value, path);
	gtk_tree_path_free(path);
	return 0;
    }
    return -1;
}

/* We have to set ob_type here because stupid win32 does not allow you
 * to use variables from another dll in a global variable initialisation.
 */
void
_pygtk_register_boxed_types(PyObject *moddict)
{
#define register_tp(x) Py##x##_Type.ob_type = &PyType_Type; \
    PyDict_SetItemString(moddict, #x "Type", (PyObject *)&Py##x##_Type)
#define register_tp2(x, tp) Py##x##_Type.ob_type = &PyType_Type; \
    PyDict_SetItemString(moddict, #x "Type", (PyObject *)&Py##x##_Type); \
    pyg_boxed_register(tp, Py##x##_from_value, Py##x##_to_value)

    ExtensionClassImported;
    register_tp2(GtkAccelGroup, GTK_TYPE_ACCEL_GROUP);
#if 0
    register_tp(GtkStyle);
    PyGtkStyleHelper_Type.ob_type = &PyType_Type;
#endif
    register_tp2(GdkFont, GTK_TYPE_GDK_FONT);
    register_tp2(GdkColor, GTK_TYPE_GDK_COLOR);
    register_tp2(GdkEvent, GTK_TYPE_GDK_EVENT);
#if 0
    register_tp(GdkWindow);
    register_tp(GdkGC);
#endif
    register_tp2(GdkVisual, GTK_TYPE_GDK_VISUAL);
#if 0
    register_tp(GdkColormap);
    register_tp(GdkDragContext);
#endif
    register_tp2(GtkSelectionData, GTK_TYPE_SELECTION_DATA);
    register_tp(GdkAtom);
    register_tp(GdkCursor);
    register_tp(GtkCTreeNode);
    register_tp(GdkDevice);
    register_tp2(GtkTextIter, GTK_TYPE_TEXT_ITER);
    register_tp2(GtkTreeIter, GTK_TYPE_TREE_ITER);
    pyg_boxed_register(GTK_TYPE_TREE_PATH,
		       PyGtkTreePath_from_value,
		       PyGtkTreePath_to_value);
}
