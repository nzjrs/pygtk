/* -*- Mode: C; c-basic-offset: 4 -*- */
#include <gtk/gtk.h>
#include "pygtk-private.h"
#include <structmember.h>

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
#endif

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


/* style helper code */
#define NUM_STATES 5
staticforward PyTypeObject PyGtkStyleHelper_Type;

PyObject *
_pygtk_style_helper_new(GtkStyle *style, int type, gpointer array)
{
    PyGtkStyleHelper_Object *self;

    self = (PyGtkStyleHelper_Object *)PyObject_NEW(PyGtkStyleHelper_Object,
						   &PyGtkStyleHelper_Type);
    if (self == NULL)
	return NULL;
    self->style = g_object_ref(style);
    self->type = type;
    self->array = array;
    return (PyObject *)self;
}

static void
pygtk_style_helper_dealloc(PyGtkStyleHelper_Object *self)
{
    g_object_unref(self->style);
    PyObject_DEL(self);
}

static int
pygtk_style_helper_length(PyGtkStyleHelper_Object *self)
{
    return NUM_STATES;
}

static PyObject *
pygtk_style_helper_getitem(PyGtkStyleHelper_Object *self, int pos)
{
    if (pos < 0) pos += NUM_STATES;
    if (pos < 0 || pos >= NUM_STATES) {
	PyErr_SetString(PyExc_IndexError, "index out of range");
	return NULL;
    }
    switch (self->type) {
    case STYLE_COLOUR_ARRAY:
	{
	    GdkColor *array = (GdkColor *)self->array;
	    return pyg_boxed_new(GDK_TYPE_COLOR, &array[pos], TRUE, TRUE);
	}
    case STYLE_GC_ARRAY:
	{
	    GdkGC **array = (GdkGC **)self->array;
	    return pygobject_new((GObject *)array[pos]);
	}
    case STYLE_PIXMAP_ARRAY:
	{
	    GdkPixmap **array = (GdkPixmap **)self->array;
	    return pygobject_new((GObject *)array[pos]);
	}
    }
    g_assert_not_reached();
    return NULL;
}

static int
pygtk_style_helper_setitem(PyGtkStyleHelper_Object *self, int pos,
			   PyObject *value)
{
    extern PyTypeObject PyGdkGC_Type;
    extern PyTypeObject PyGdkPixmap_Type;

    if (pos < 0) pos += NUM_STATES;
    if (pos < 0 || pos >= NUM_STATES) {
	PyErr_SetString(PyExc_IndexError, "index out of range");
	return -1;
    }
    switch (self->type) {
    case STYLE_COLOUR_ARRAY:
	{
	    GdkColor *array = (GdkColor *)self->array;

	    if (!pyg_boxed_check(value, GDK_TYPE_COLOR)) {
		PyErr_SetString(PyExc_TypeError, "can only assign a GdkColor");
		return -1;
	    }
	    array[pos] = *pyg_boxed_get(value, GdkColor);
	    return 0;
	}
    case STYLE_GC_ARRAY:
	{
	    GdkGC **array = (GdkGC **)self->array;

	    if (!pygobject_check(value, &PyGdkGC_Type)) {
		PyErr_SetString(PyExc_TypeError, "can only assign a GdkGC");
		return -1;
	    }
	    if (array[pos]) g_object_unref(array[pos]);
	    array[pos] = GDK_GC(g_object_ref(pygobject_get(value)));
	    return 0;
	}
    case STYLE_PIXMAP_ARRAY:
	{
	    GdkPixmap **array = (GdkPixmap **)self->array;

	    if (!pygobject_check(value, &PyGdkPixmap_Type) && value!=Py_None) {
		PyErr_SetString(PyExc_TypeError,
				"can only assign a GdkPixmap or None");
		return -1;
	    }
	    if (array[pos]) g_object_unref(array[pos]);
	    if (value != Py_None)
		array[pos] = GDK_PIXMAP(g_object_ref(pygobject_get(value)));
	    else
		array[pos] = NULL;
	    return 0;
	}
    }
    g_assert_not_reached();
    return -1;
}

static PySequenceMethods pygtk_style_helper_seqmethods = {
    (inquiry)pygtk_style_helper_length,
    (binaryfunc)0,
    (intargfunc)0,
    (intargfunc)pygtk_style_helper_getitem,
    (intintargfunc)0,
    (intobjargproc)pygtk_style_helper_setitem,
    (intintobjargproc)0
};
static PyTypeObject PyGtkStyleHelper_Type = {
    PyObject_HEAD_INIT(NULL)
    0,
    "gtk.GtkStyleHelper",
    sizeof(PyGtkStyleHelper_Object),
    0,
    (destructor)pygtk_style_helper_dealloc,
    (printfunc)0,
    (getattrfunc)0,
    (setattrfunc)0,
    (cmpfunc)0,
    (reprfunc)0,
    0,
    &pygtk_style_helper_seqmethods,
    0,
    (hashfunc)0,
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
    PyObject_DEL(self);
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
    PyObject *py_property, py_type = NULL;
    gint pdelete = FALSE;

    GdkAtom atype, property, type;
    gint aformat, alength;
    guchar *data;

    if (!PyArg_ParseTuple(args, "O|Oi:GdkWindow.property_get", &py_property,
			  &py_type, &pdelete)) {
	return NULL;
    }

    property = pygdk_atom_from_pyobject(py_property);
    if (Pyerr_Occurred())
	return NULL;

    type = pygdk_atom_from_pyobject(py_type);
    if (Pyerr_Occurred())
	return NULL;
	
    if (gdk_property_get(self->obj, property, type, 0, 9999,
			 pdelete, &atype, &aformat, &alength, &data)) {
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
    PyObject *py_property, *py_type;
    GdkAtom property, type;
    gint format;
    PyObject *py_mode, *pdata;
    GdkPropMode mode;
    guchar *data = NULL;
    gint nelements;

    if (!PyArg_ParseTuple(args, "OOiOO:GdkWindow.property_change",
			  &py_property, &py_type, &format, &py_mode, &pdata)) {
	return NULL;
    }

    property = pygdk_atom_from_pyobject(py_property);
    if (Pyerr_Occurred())
	return NULL;

    type = pygdk_atom_from_pyobject(py_type);
    if (Pyerr_Occurred())
	return NULL;
    
    if (pygtk_enum_get_value(GDK_TYPE_PROP_MODE, py_mode, (gint *)&mode))
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
    gdk_property_change(self->obj, property, type, format, mode, data,
			nelements);
    if (format != 8)
	g_free(data);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
PyGdkWindow_PropertyDelete(PyGdkWindow_Object *self, PyObject *args)
{
    PyObject py_property;
    GdkAtom property;

    if (!PyArg_ParseTuple(args, "O:GdkWindow.property_delete", &property)) {
	return NULL;
    }

    property = pygdk_atom_from_pyobject(py_property);
    if (Pyerr_Occurred())
	return NULL;
    
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
#endif

GdkAtom
pygdk_atom_from_pyobject(PyObject *object)
{
    if (object == NULL)
	return NULL;
    if (PyString_Check(object))
	return gdk_atom_intern(PyString_AsString(object), FALSE);
    if (PyGdkAtom_Check(object))
	return PyGdkAtom_Get(object);
    PyErr_SetString(PyExc_TypeError, "unable to convert argument to GdkAtom");
    return NULL;
}

static void
pygdk_atom_dealloc(PyGdkAtom_Object *self)
{
    if (self->name) g_free(self->name);
    PyObject_DEL(self);
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

static PyObject *
pygdk_atom_str(PyGdkAtom_Object *self)
{
    if (!self->name) self->name = gdk_atom_name(self->atom);
    if (self->name)
	return PyString_FromString(self->name);
    return pygdk_atom_repr(self);
}

PyTypeObject PyGdkAtom_Type = {
    PyObject_HEAD_INIT(NULL)
    0,
    "gtk.gdk.GdkAtom",
    sizeof(PyGdkAtom_Object),
    0,
    (destructor)pygdk_atom_dealloc,
    (printfunc)0,
    (getattrfunc)0,
    (setattrfunc)0,
    (cmpfunc)pygdk_atom_compare,
    (reprfunc)pygdk_atom_repr,
    0,
    0,
    0,
    (hashfunc)pygdk_atom_hash,
    (ternaryfunc)0,
    (reprfunc)pygdk_atom_str,
    0L,0L,0L,0L,
    NULL
};

typedef struct {
    PyObject_HEAD
    GtkListStore *list_store;
    GtkTreeIter iter;
} PyGtkListStoreRow;
staticforward PyTypeObject PyGtkListStoreRow_Type;

PyObject *
_pygtk_list_store_row_new(GtkListStore *list_store, GtkTreeIter *iter)
{
    PyGtkListStoreRow *self;

    self = (PyGtkListStoreRow *) PyObject_NEW(PyGtkListStoreRow,
					      &PyGtkListStoreRow_Type);
    if (self == NULL)
	return NULL;
    self->list_store = g_object_ref(list_store);
    self->iter = *iter;
    return (PyObject *)self;
}

static void
pygtk_list_store_row_dealloc(PyGtkListStoreRow *self)
{
    g_object_unref(self->list_store);
    PyObject_DEL(self);
}

static int
pygtk_list_store_row_length(PyGtkListStoreRow *self)
{
    return gtk_tree_model_get_n_columns(GTK_TREE_MODEL(self->list_store));
}

static PyObject *
pygtk_list_store_row_getitem(PyGtkListStoreRow *self, gint column)
{
    gint n_columns;
    GValue value = { 0, };
    PyObject *ret;

    n_columns = gtk_tree_model_get_n_columns(GTK_TREE_MODEL(self->list_store));
    if (column < 0)
	column += n_columns;
    if (column < 0 || column >= n_columns) {
	PyErr_SetString(PyExc_IndexError, "column index out of range");
        return NULL;
    }
    gtk_tree_model_get_value(GTK_TREE_MODEL(self->list_store), &self->iter,
			     column, &value);
    ret = pyg_value_as_pyobject(&value, TRUE);
    g_value_unset(&value);
    return ret;
}

static int
pygtk_list_store_row_setitem(PyGtkListStoreRow *self, gint column,
			     PyObject *pyvalue)
{
    gint n_columns;
    GValue value = { 0, };

    n_columns = gtk_tree_model_get_n_columns(GTK_TREE_MODEL(self->list_store));
    if (column < 0)
	column += n_columns;
    if (column < 0 || column >= n_columns) {
	PyErr_SetString(PyExc_IndexError, "column index out of range");
        return -1;
    }
    g_value_init(&value, gtk_tree_model_get_column_type(
		           GTK_TREE_MODEL(self->list_store), column));
    if (pyg_value_from_pyobject(&value, pyvalue)) {
	PyErr_SetString(PyExc_TypeError,
			"value is of wrong type for this column");
	return -1;
    }
    gtk_list_store_set_value(self->list_store, &self->iter, column, &value);
    g_value_unset(&value);
    return 0;
}

static PySequenceMethods pygtk_list_store_row_seqmethods = {
    (inquiry)pygtk_list_store_row_length,
    (binaryfunc)0,
    (intargfunc)0,
    (intargfunc)pygtk_list_store_row_getitem,
    (intintargfunc)0,
    (intobjargproc)pygtk_list_store_row_setitem,
    (intintobjargproc)0
};

static PyTypeObject PyGtkListStoreRow_Type = {
    PyObject_HEAD_INIT(NULL)
    0,
    "gtk.gtk.ListStoreRow",
    sizeof(PyGtkListStoreRow),
    0,
    (destructor)pygtk_list_store_row_dealloc,
    (printfunc)0,
    (getattrfunc)0,
    (setattrfunc)0,
    (cmpfunc)0,
    (reprfunc)0,
    0,
    &pygtk_list_store_row_seqmethods,
    0,
    (hashfunc)0,
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
    if (PyInt_Check(object)) {
	GtkTreePath *path;

	path = gtk_tree_path_new();
	gtk_tree_path_append_index(path, PyInt_AsLong(object));
	return path;
    } else if (PyTuple_Check(object)) {
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

gboolean
pygdk_rectangle_from_pyobject(PyObject *object, GdkRectangle *rectangle)
{
    g_return_val_if_fail(rectangle != NULL, FALSE);

    if (pyg_boxed_check(object, GDK_TYPE_RECTANGLE)) {
	*rectangle = *pyg_boxed_get(object, GdkRectangle);
	return TRUE;
    }
    if (PyArg_ParseTuple(object, "iiii", &rectangle->x, &rectangle->y,
				&rectangle->width, &rectangle->height)) {
	return TRUE;
    }
    PyErr_Clear();
    PyErr_SetString(PyExc_TypeError, "could not convert to GdkRectangle");
    return FALSE;
}

static PyObject *
PyGdkRectangle_from_value(const GValue *value)
{
    GdkRectangle *rect = (GtkTreePath *)g_value_get_boxed(value);

    return pyg_boxed_new(rect, GDK_TYPE_RECTANGLE, TRUE, TRUE);
}
static int
PyGdkRectangle_to_value(GValue *value, PyObject *object)
{
    GdkRectangle rect;

    if (!pygdk_rectangle_from_pyobject(object, &rect))
	return -1;

    g_value_set_boxed(value, &rect);
    return 0;
}

/* We have to set ob_type here because stupid win32 does not allow you
 * to use variables from another dll in a global variable initialisation.
 */
void
_pygtk_register_boxed_types(PyObject *moddict)
{
#define register_tp(x) Py##x##_Type.ob_type = &PyType_Type; \
    PyDict_SetItemString(moddict, #x "Type", (PyObject *)&Py##x##_Type)

    PyGtkStyleHelper_Type.ob_type = &PyType_Type;
    PyGdkAtom_Type.ob_type = &PyType_Type;
    PyGtkListStoreRow_Type.ob_type = &PyType_Type;
#if 0
    register_tp(GdkWindow);
#endif
    register_tp(GdkAtom);

    pyg_register_boxed_custom(GTK_TYPE_TREE_PATH,
			      PyGtkTreePath_from_value,
			      PyGtkTreePath_to_value);
    pyg_register_boxed_custom(GDK_TYPE_RECTANGLE,
			      PyGdkRectangle_from_value,
			      PyGdkRectangle_to_value);
}
