/* -*- Mode: C; c-basic-offset: 4 -*- */

/* this module provides some of the base functionality of the GtkObject
 * wrapper system */

#include "pygtk-private.h"

/* ----------------- Thread stuff -------------------- */
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


#ifdef WITH_THREAD
static GStaticPrivate pythreadstate_key = G_STATIC_PRIVATE_INIT;
static GStaticPrivate counter_key = G_STATIC_PRIVATE_INIT;

/* The global Python lock will be grabbed by Python when entering a
 * Python/C function; thus, the initial lock count will always be one.
 */
#  define INITIAL_LOCK_COUNT 1
#  define PyGTK_BLOCK_THREADS                                              \
   {                                                                       \
     gint counter = GPOINTER_TO_INT(g_static_private_get(&counter_key));   \
     if (counter == -INITIAL_LOCK_COUNT) {                                 \
       PyThreadState *_save;                                               \
       _save = g_static_private_get(&pythreadstate_key);                   \
       Py_BLOCK_THREADS;                                                   \
     }                                                                     \
     counter++;                                                            \
     g_static_private_set(&counter_key, GINT_TO_POINTER(counter), NULL);   \
   }

#  define PyGTK_UNBLOCK_THREADS                                            \
   {                                                                       \
     gint counter = GPOINTER_TO_INT(g_static_private_get(&counter_key));   \
     counter--;                                                            \
     if (counter == -INITIAL_LOCK_COUNT) {                                 \
       PyThreadState *_save;                                               \
       Py_UNBLOCK_THREADS;                                                 \
       g_static_private_set(&pythreadstate_key, _save, NULL);              \
     }                                                                     \
     g_static_private_set(&counter_key, GINT_TO_POINTER(counter), NULL);   \
   }


#else /* !WITH_THREADS */
#  define PyGTK_BLOCK_THREADS
#  define PyGTK_UNBLOCK_THREADS
#endif

void pygtk_block_threads(void) { PyGTK_BLOCK_THREADS }
void pygtk_unblock_threads(void) { PyGTK_UNBLOCK_THREADS }

/* ------------------- object support */

void
pygtk_destroy_notify(gpointer user_data)
{
    PyObject *obj = (PyObject *)user_data;

    PyGTK_BLOCK_THREADS
    Py_DECREF(obj);
    PyGTK_UNBLOCK_THREADS
}


static GHashTable *class_hash = NULL;

static const char *pygtk_wrapper_key = "pygtk::wrapper";
static const char *pygtk_ownedref_key = "pygtk::ownedref";

void
pygtk_register_class(PyObject *dict, const gchar *class_name,
		     PyExtensionClass *ec, PyExtensionClass *parent)
{
    if (!class_hash)
	class_hash = g_hash_table_new(g_str_hash, g_str_equal);

    /* set standard pygtk class functions if they aren't already set */
    if (!ec->tp_dealloc) ec->tp_dealloc = (destructor)pygtk_dealloc;
    if (!ec->tp_getattr) ec->tp_getattr = (getattrfunc)pygtk_getattr;
    if (!ec->tp_setattr) ec->tp_setattr = (setattrfunc)pygtk_setattr;
    if (!ec->tp_compare) ec->tp_compare = (cmpfunc)pygtk_compare;
    if (!ec->tp_repr)    ec->tp_repr    = (reprfunc)pygtk_repr;
    if (!ec->tp_hash)    ec->tp_hash    = (hashfunc)pygtk_hash;

    if (parent) {
	PyExtensionClass_ExportSubclassSingle(dict, (char *)class_name,
					      *ec, *parent);
    } else {
	PyExtensionClass_Export(dict, (char *)class_name, *ec);
    }

    g_hash_table_insert(class_hash, g_strdup(class_name), ec);
}

void
pygtk_register_wrapper(PyObject *self)
{
    GtkObject *obj = ((PyGtk_Object *)self)->obj;

    gtk_object_ref(obj);
    gtk_object_sink(obj);
    gtk_object_set_data(obj, pygtk_wrapper_key, self);

#if 1
    /*
    ((PyGtk_Object *)self)->inst_dict = PyDict_New();
    */
#endif
}

PyObject *
pygtk_no_constructor(PyObject *self, PyObject *args)
{
    gchar buf[512];

    g_snprintf(buf, sizeof(buf), "%s is an abstract widget", self->ob_type->tp_name);
    PyErr_SetString(PyExc_NotImplementedError, buf);
    return NULL;
}

static PyExtensionClass *
pygtk_lookup_class(GtkType type)
{
    PyExtensionClass *ec;

    /* find the python type for this object.  If not found, use parent. */
    while ((ec = g_hash_table_lookup(class_hash, gtk_type_name(type))) == NULL
	   && type != 0)
	type = gtk_type_parent(type);
    g_assert(ec != NULL);
    return ec;
}

PyObject *
PyGtk_New(GtkObject *obj)
{
    PyGtk_Object *self;
    PyTypeObject *tp;
    GtkType type;

    if (obj == NULL) {
	Py_INCREF(Py_None);
	return Py_None;
    }

    /* we already have a wrapper for this object -- return it. */
    if ((self = (PyGtk_Object *)gtk_object_get_data(obj, pygtk_wrapper_key))) {
	/* if the gtk object currently owns the wrapper reference ... */
	if (self->hasref) {
	    self->hasref = FALSE;
	    gtk_object_remove_no_notify(obj, pygtk_ownedref_key);
	    gtk_object_ref(obj);
	}
	Py_INCREF(self);
	return (PyObject *)self;
    }

    tp = (PyTypeObject *)pygtk_lookup_class(GTK_OBJECT_TYPE(obj));

    /* can't use PyObject_NEW, as we want to create a slightly larger struct */
    self = malloc(sizeof(PyGtk_Object));
    if (self == NULL)
	return PyErr_NoMemory();
    self->ob_type = tp;
    _Py_NewReference((PyObject *)self);

    self->obj = obj;
    gtk_object_ref(obj);
    /* save the wrapper pointer so we can access it later */
    gtk_object_set_data(obj, pygtk_wrapper_key, self);
#if 1
    /* set up the class dictionary */
    self->inst_dict = PyDict_New();
#endif
    return (PyObject *)self;
}

void
pygtk_dealloc(PyGtk_Object *self)
{
    GtkObject *obj = self->obj;

    /* this bit of code has been handled in pygtk_subclass_dealloc ... */
    if (obj && !(((PyExtensionClass *)self->ob_type)->class_flags &
		 EXTENSIONCLASS_PYSUBCLASS_FLAG)) {
	/* save reference to python wrapper if there are still
	 * references to the gtk object in such a way that it will be
	 * freed when the gtk object is destroyed, so is the python
	 * wrapper, but if a python wrapper can be */
	if (obj->ref_count > 1) {
	    Py_INCREF(self); /* grab a reference on the wrapper */
	    self->hasref = TRUE;
	    gtk_object_set_data_full(obj, pygtk_ownedref_key,
				     self, pygtk_destroy_notify);
	    gtk_object_unref(obj);
	    return;
	}
	if (!self->hasref) /* don't unref the gtk object if it owns us */
	    gtk_object_unref(obj);
    }
    /* subclass_dealloc (ExtensionClass.c) does this for us for python
     * subclasses */
    if (self->inst_dict &&
	!(((PyExtensionClass *)self->ob_type)->class_flags &
	  EXTENSIONCLASS_PYSUBCLASS_FLAG))
	Py_DECREF(self->inst_dict);
    PyMem_DEL(self);
}

/* standard getattr method */
PyObject *
pygtk_getattr(PyGtk_Object *self, char *attr)
{
    ExtensionClassImported;

    return Py_FindAttrString((PyObject *)self, attr);
}

int
pygtk_setattr(PyGtk_Object *self, char *attr, PyObject *value)
{
    PyDict_SetItemString(INSTANCE_DICT(self), attr, value);
    return 0;
}

int
pygtk_compare(PyGtk_Object *self, PyGtk_Object *v)
{
    if (self->obj == v->obj) return 0;
    if (self->obj > v->obj)  return -1;
    return 1;
}

long
pygtk_hash(PyGtk_Object *self)
{
    return (long)self->obj;
}

PyObject *
pygtk_repr(PyGtk_Object *self)
{
    gchar buf[128];

    g_snprintf(buf, sizeof(buf), "<%s at %lx>",
	       gtk_type_name(GTK_OBJECT_TYPE(self->obj)), (long)self->obj);
    return PyString_FromString(buf);
}


/* ------------ stuff for converting from gtk <-> python types --------- */

/* this hash table is used to let these functions know about new boxed
 * types (eg. maybe some from GNOME). The functions return TRUE on error */
typedef PyObject *(*fromargfunc)(gpointer boxed);
typedef int (*toargfunc)(gpointer *boxed, PyObject *obj);
typedef struct {
    fromargfunc fromarg;
    toargfunc toarg;
} PyGtk_BoxFuncs;
/* keys are GUINT_TO_POINTER(GtkType) */
GHashTable *_pygtk_boxed_funcs = NULL;

void
pygtk_register_boxed(GtkType boxed_type, fromargfunc fromarg, toargfunc toarg)
{
    PyGtk_BoxFuncs *fs = g_new(PyGtk_BoxFuncs, 1);

    fs->fromarg = fromarg;
    fs->toarg = toarg;
    g_hash_table_insert(_pygtk_boxed_funcs, GUINT_TO_POINTER(boxed_type), fs);
}

#define pygtk_get_boxed(type) (PyGtk_BoxFuncs *)g_hash_table_lookup(_pygtk_boxed_funcs, GUINT_TO_POINTER(type))

/* create a GtkArg from a PyObject, using the GTK_VALUE_* routines.
 * returns -1 if it couldn't set the argument. */
int
pygtk_arg_from_pyobject(GtkArg *arg, PyObject *obj)
{
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
	if (pygtk_enum_get_value(arg->type, obj, &(GTK_VALUE_ENUM(*arg))))
	    return -1;
	break;
    case GTK_TYPE_FLAGS:
	if (pygtk_flag_get_value(arg->type, obj, &(GTK_VALUE_FLAGS(*arg))))
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
	if (PyGtk_Check(obj, pygtk_lookup_class(arg->type)))
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
	} else if (arg->type == GTK_TYPE_GDK_VISUAL) {
	    if (PyGdkVisual_Check(obj))
		GTK_VALUE_BOXED(*arg) = PyGdkVisual_Get(obj);
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
	    PyGtk_BoxFuncs *fs= pygtk_get_boxed(arg->type);
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
	GTK_VALUE_FOREIGN(*arg).notify = pygtk_destroy_notify;
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
		(GtkCallbackMarshal)pygtk_callback_marshal;
	    GTK_VALUE_CALLBACK(*arg).data = obj;
	    GTK_VALUE_CALLBACK(*arg).notify = pygtk_destroy_notify;
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
PyObject *
pygtk_arg_as_pyobject(GtkArg *arg)
{
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
	return pygtk_args_as_tuple(GTK_VALUE_ARGS(*arg).n_args,
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
	else if (arg->type == GTK_TYPE_GDK_VISUAL)
	    return PyGdkVisual_New(GTK_VALUE_BOXED(*arg));
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
	    PyGtk_BoxFuncs *fs = pygtk_get_boxed(arg->type);
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
void
pygtk_ret_from_pyobject(GtkArg *ret, PyObject *py_ret)
{
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
	if (pygtk_enum_get_value(ret->type, py_ret, GTK_RETLOC_ENUM(*ret))) {
	    PyErr_Clear();
	    *GTK_RETLOC_ENUM(*ret) = 0;
	}
	break;
    case GTK_TYPE_FLAGS:
	if (pygtk_flag_get_value(ret->type, py_ret, GTK_RETLOC_FLAGS(*ret))) {
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
	if (PyGtk_Check(py_ret, pygtk_lookup_class(ret->type)))
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
	} else if (ret->type == GTK_TYPE_GDK_VISUAL) {
	    if (PyGdkVisual_Check(py_ret))
		*GTK_RETLOC_BOXED(*ret) = PyGdkVisual_Get(py_ret);
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
	    PyGtk_BoxFuncs *fs = pygtk_get_boxed(ret->type);
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
PyObject *
pygtk_ret_as_pyobject(GtkArg *arg)
{
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
	else if (arg->type == GTK_TYPE_GDK_VISUAL)
	    return PyGdkVisual_New(*GTK_RETLOC_BOXED(*arg));
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
	    PyGtk_BoxFuncs *fs = pygtk_get_boxed(arg->type);
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
PyObject *
pygtk_args_as_tuple(int nparams, GtkArg *args)
{
    PyObject *tuple, *item;
    int i;

    if ((tuple = PyTuple_New(nparams)) == NULL)
	return NULL;
    for (i = 0; i < nparams; i++) {
	item = pygtk_arg_as_pyobject(&args[i]);
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
int
pygtk_args_from_sequence(GtkArg *args, int nparams, PyObject *seq)
{
    PyObject *item;
    int i;
    if (!PySequence_Check(seq))
	return -1;
    for (i = 0; i < nparams; i++) {
	item = PySequence_GetItem(seq, i);
	Py_DECREF(item);
	if (pygtk_arg_from_pyobject(&args[i], item)) {
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
void
pygtk_callback_marshal(GtkObject *o, gpointer data, guint nargs, GtkArg *args)
{
    PyObject *tuple = data, *func, *extra=NULL, *obj=NULL, *ret, *a, *params;

    PyGTK_BLOCK_THREADS
    a = pygtk_args_as_tuple(nargs, args);
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
    pygtk_ret_from_pyobject(&args[nargs], ret);
    Py_DECREF(ret);
    PyGTK_UNBLOCK_THREADS
}

void
pygtk_signal_marshal(GtkObject *object, gpointer user_data,
		    int nparams, GtkArg *args, GtkType *arg_types,
		    GtkType return_type)
{
    PyObject *func = user_data;
    PyObject *arg_list, *params, *ret;

    PyGTK_BLOCK_THREADS
    ret = PyTuple_New(1);
    PyTuple_SetItem(ret, 0, PyGtk_New(object));
    arg_list = pygtk_args_as_tuple(nparams, args);
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
    pygtk_ret_from_pyobject(&args[nparams], ret);
    Py_DECREF(ret);
    PyGTK_UNBLOCK_THREADS
}

/* simple callback handler -- this one actually looks at the return type */
/* used for timeout and idle functions */
void
pygtk_handler_marshal(gpointer a, PyObject *func, int nargs, GtkArg *args)
{
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
void
pygtk_input_marshal(gpointer a, PyObject *func, int nargs, GtkArg *args)
{
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

GtkArg *
pygtk_dict_as_args(PyObject *dict, GtkType type, gint *nargs)
{
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
	if (pygtk_arg_from_pyobject(&(arg[pos]), item)) {
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

GtkArg *
pygtk_dict_as_container_args(PyObject *dict, GtkType type, gint *nargs)
{
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
	if (pygtk_arg_from_pyobject(&(arg[pos]), item)) {
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

/* return 1 on failure */
gint
pygtk_enum_get_value(GtkType enum_type, PyObject *obj, int *val)
{
    /* used for default arguments */
    if (!obj)
	return 0;
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
    PyErr_SetString(PyExc_TypeError,"enum values must be integers or strings");
    return 1;
}

gint
pygtk_flag_get_value(GtkType flag_type, PyObject *obj, int *val)
{
    /* used for default arguments */
    if (!obj)
	return 0;
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
		    PyErr_SetString(PyExc_TypeError,
				    "couldn't translate string");
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


/* ------------------- Base GtkObject methods ------------------- */

static destructor real_subclass_dealloc = NULL;

static void
pygtk_subclass_dealloc(PyGtk_Object *self)
{
    GtkObject *obj = self->obj;

    if (obj) {
	/* save reference to python wrapper if there are still
	 * references to the gtk object in such a way that it will be
	 * freed when the gtk object is destroyed, so is the python
	 * wrapper, but if a python wrapper can be */
	if (obj->ref_count > 1) {
	    Py_INCREF(self); /* grab a reference on the wrapper */
	    self->hasref = TRUE;
	    gtk_object_set_data_full(obj, pygtk_ownedref_key,
				     self, pygtk_destroy_notify);
	    gtk_object_unref(obj);
	    return;
	}
	if (!self->hasref) /* don't unref the gtk object if it owns us */
	    gtk_object_unref(obj);
    }
    if (real_subclass_dealloc)
	(* real_subclass_dealloc)((PyObject *)self);
}

/* more hackery to stop segfaults caused by multi deallocs on a subclass
 * (which happens quite regularly in pygtk) */
static PyObject *
pygtk__class_init__(PyObject *something, PyObject *args)
{
    PyExtensionClass *subclass;

    if (!PyArg_ParseTuple(args, "O:GtkObject.__class_init__", &subclass))
	return NULL;
    g_message("__class_init__ called for %s", subclass->tp_name);
    if ((subclass->class_flags & EXTENSIONCLASS_PYSUBCLASS_FLAG) &&
	subclass->tp_dealloc != (destructor)pygtk_subclass_dealloc) {
	real_subclass_dealloc = subclass->tp_dealloc;
	subclass->tp_dealloc = (destructor)pygtk_subclass_dealloc;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gtk_signal_connect(PyGtk_Object *self, PyObject *args)
{
    char *name;
    PyObject *first, *func, *extra = NULL, *data;
    int signum, len;

    len = PyTuple_Size(args);
    if (len < 2) {
	PyErr_SetString(PyExc_TypeError,
			"GtkObject.connect requires at least 2 arguments");
	return NULL;
    }
    first = PySequence_GetSlice(args, 0, 2);
    if (!PyArg_ParseTuple(first, "sO:GtkObject.connect",
			  &name, &func)) {
	Py_DECREF(first);
        return NULL;
    }
    Py_DECREF(first);
    if (!PyCallable_Check(func)) {
        PyErr_SetString(PyExc_TypeError, "third argument must be callable");
        return NULL;
    }

    extra = PySequence_GetSlice(args, 2, len);

    if (extra == NULL)
      return NULL;
    data = Py_BuildValue("(ON)", func, extra);
    signum = gtk_signal_connect_full(self->obj, name, NULL,
				(GtkCallbackMarshal)pygtk_callback_marshal,
				data, pygtk_destroy_notify, FALSE, FALSE);
    return PyInt_FromLong(signum);
}

static PyObject *
_wrap_gtk_signal_connect_after(PyGtk_Object *self, PyObject *args)
{
    char *name;
    PyObject *first, *func, *extra = NULL, *data;
    int signum, len;

    len = PyTuple_Size(args);
    if (len < 2) {
	PyErr_SetString(PyExc_TypeError,
		"GtkObject.connect_after requires at least 2 arguments");
	return NULL;
    }
    first = PySequence_GetSlice(args, 0, 2);
    if (!PyArg_ParseTuple(first, "sO:GtkObject.connect_after",
			  &name, &func)) {
	Py_DECREF(first);
        return NULL;
    }
    Py_DECREF(first);
    if (!PyCallable_Check(func)) {
        PyErr_SetString(PyExc_TypeError, "third argument must be callable");
        return NULL;
    }
    extra = PySequence_GetSlice(args, 2, len);

    if (extra == NULL)
      return NULL;
    data = Py_BuildValue("(ON)", func, extra);

    signum = gtk_signal_connect_full(self->obj, name, NULL,
				(GtkCallbackMarshal)pygtk_callback_marshal,
				data, pygtk_destroy_notify, FALSE, TRUE);
    return PyInt_FromLong(signum);
}

static PyObject *
_wrap_gtk_signal_connect_object(PyGtk_Object *self, PyObject *args)
{
    char *name;
    PyObject *first, *func, *extra = NULL, *other, *data;
    int signum, len;

    len = PyTuple_Size(args);
    if (len < 3) {
	PyErr_SetString(PyExc_TypeError,
		"GtkObject.connect_object requires at least 3 arguments");
	return NULL;
    }
    first = PySequence_GetSlice(args, 0, 3);
    if (!PyArg_ParseTuple(first, "sOO:GtkObject.connect_object",
			  &name, &func, &other)) {
	Py_DECREF(first);
        return NULL;
    }
    Py_DECREF(first);
    if (!PyCallable_Check(func)) {
        PyErr_SetString(PyExc_TypeError, "third argument must be callable");
        return NULL;
    }
    if (!PyGtk_Check(other, &PyGtkObject_Type)) {
	PyErr_SetString(PyExc_TypeError, "forth argument must be a GtkObject");
	return NULL;
    }
    
    extra = PySequence_GetSlice(args, 3, len);
    
    if (extra == NULL)
      return NULL;
    data = Py_BuildValue("(ONO)", func, extra, other);
    if (data == NULL)
      return NULL;

    signum = gtk_signal_connect_full(self->obj, name, NULL,
				(GtkCallbackMarshal)pygtk_callback_marshal,
				data, pygtk_destroy_notify, FALSE, FALSE);
    return PyInt_FromLong(signum);
}

static PyObject *
_wrap_gtk_signal_connect_object_after(PyGtk_Object *self, PyObject *args)
{
    char *name;
    PyObject *first, *func, *extra = NULL, *other, *data;
    int signum, len;

    len = PyTuple_Size(args);
    if (len < 3) {
	PyErr_SetString(PyExc_TypeError,
	    "GtkObject.connect_object_after requires at least 3 arguments");
	return NULL;
    }
    first = PySequence_GetSlice(args, 0, 3);
    if (!PyArg_ParseTuple(args, "sOO:GtkObject.connect_object_after",
			  &name, &func, &other)) {
	Py_DECREF(first);
        return NULL;
    }
    Py_DECREF(first);
    if (!PyCallable_Check(func)) {
        PyErr_SetString(PyExc_TypeError, "third argument must be callable");
        return NULL;
    }
    if (!PyGtk_Check(other, &PyGtkObject_Type)) {
	PyErr_SetString(PyExc_TypeError, "forth argument must be a GtkObject");
	return NULL;
    }
    extra = PySequence_GetSlice(args, 3, len);

    if (extra == NULL)
      return NULL;
    data = Py_BuildValue("(ONO)", func, extra, other);

    signum = gtk_signal_connect_full(self->obj, name, NULL,
				(GtkCallbackMarshal)pygtk_callback_marshal,
				data, pygtk_destroy_notify, FALSE, TRUE);
    return PyInt_FromLong(signum);
}

static PyObject *
_wrap_gtk_signal_disconnect_by_data(PyGtk_Object *self, PyObject *args)
{
  PyObject *data;
  
  self = self;
  if(!PyArg_ParseTuple(args,"O:GtkObject.signal_disconnect_by_data", &data)) 
    return NULL;
  gtk_signal_disconnect_by_data(self->obj, data);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_wrap_gtk_signal_handler_block_by_data(PyGtk_Object *self, PyObject *args)
{
    PyObject *data;

    if(!PyArg_ParseTuple(args,"O:GtkObject.signal_handler_block_by_data",
			 &data)) 
        return NULL;
    gtk_signal_handler_block_by_data(self->obj, data);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gtk_signal_handler_unblock_by_data(PyGtk_Object *self, PyObject *args)
{
    PyObject *data;

    if(!PyArg_ParseTuple(args,"O:GtkObject.signal_handler_unblock_by_data",
			 &data)) 
        return NULL;
    gtk_signal_handler_unblock_by_data(self->obj, data);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gtk_signal_emitv_by_name(PyGtk_Object *self, PyObject *args)
{
    guint signal_id, i, nparams, len;
    GtkSignalQuery *query;
    GtkArg *params;
    PyObject *first, *ret, *py_params;
    gchar *name, buf[sizeof(GtkArg)]; /* large enough to hold any return value */

    len = PyTuple_Size(args);
    if (len < 1) {
	PyErr_SetString(PyExc_TypeError,"GtkObject.emit needs at least 1 arg");
	return NULL;
    }
    first = PySequence_GetSlice(args, 0, 1);
    if (!PyArg_ParseTuple(first, "s:GtkObject.emit", &name)) {
	Py_DECREF(first);
	return NULL;
    }
    Py_DECREF(first);

    signal_id = gtk_signal_lookup(name, GTK_OBJECT_TYPE(self->obj));
    if (signal_id <= 0) {
	PyErr_SetString(PyExc_KeyError,
			"can't find signal in classes ancestry");
	return NULL;
    }
    query = gtk_signal_query(signal_id);

    if (len < query->nparams + 1) {
	PyErr_SetString(PyExc_TypeError,"too few arguments to GtkObject.emit");
	g_free(query);
	return NULL;
    }

    py_params = PySequence_GetSlice(args, 1, len);
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
    if (pygtk_args_from_sequence(params, query->nparams, py_params)) {
	Py_DECREF(py_params);
	g_free(params);
	return NULL;
    }
    Py_DECREF(py_params);
    gtk_signal_emitv(self->obj, signal_id, params);
    ret = pygtk_ret_as_pyobject(&params[nparams]);
    g_free(params);
    if (ret == NULL) {
	Py_INCREF(Py_None);
	ret = Py_None;
    }
    return ret;
}

static PyObject *
_wrap_gtk_object_set(PyGtk_Object *self, PyObject *args)
{
    PyObject *dict;
    GtkArg *arg;
    gint nargs;

    if (!PyArg_ParseTuple(args, "O!:GtkObject.set", &PyDict_Type, &dict))
	return NULL;
    arg = pygtk_dict_as_args(dict, GTK_OBJECT_TYPE(self->obj), &nargs);
    if (arg == NULL && nargs != 0)
	return NULL;
    gtk_object_setv(self->obj, nargs, arg);
    g_free(arg);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gtk_object_get(PyGtk_Object *self, PyObject *args)
{
    char *name;
    GtkArg garg;
    PyObject *ret;

    if (!PyArg_ParseTuple(args, "s:GtkObject.get", &name))
        return NULL;
    garg.type = GTK_TYPE_INVALID;
    garg.name = name;
    gtk_object_getv(self->obj, 1, &garg);

    if (garg.type == GTK_TYPE_INVALID) {
        PyErr_SetString(PyExc_AttributeError, "invalid property");
        return NULL;
    }
    ret = pygtk_arg_as_pyobject(&garg);
    if (ret == NULL) {
      PyErr_SetString(PyExc_TypeError, "couldn't translate type");
      return NULL;
    }
    return ret;
}

static PyObject *
_wrap_gtk_object_set_data(PyGtk_Object *self, PyObject *args)
{
	char *key;
	PyObject *data;
	if (!PyArg_ParseTuple(args, "sO:GtkObject.set_data", &key, &data))
		return NULL;
	Py_INCREF(data);
	gtk_object_set_data_full(self->obj, key, data,
		(GtkDestroyNotify)pygtk_destroy_notify);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
_wrap_gtk_object_get_data(PyGtk_Object *self, PyObject *args)
{
	char *key;
	PyObject *data;
	if (!PyArg_ParseTuple(args, "s:GtkObject.get_data", &key))
		return NULL;
	data = gtk_object_get_data(self->obj, key);
	if (data != NULL) {
		Py_INCREF(data);
		return data;
	} else {
		Py_INCREF(Py_None);
		return Py_None;
	}
}

static PyMethodDef base_object_methods[] = {
    { "__class_init__", (PyCFunction)pygtk__class_init__, METH_VARARGS|METH_CLASS_METHOD },
    { "connect", (PyCFunction)_wrap_gtk_signal_connect, METH_VARARGS },
    { "connect_after", (PyCFunction)_wrap_gtk_signal_connect_after, METH_VARARGS },
    { "connect_object", (PyCFunction)_wrap_gtk_signal_connect_object, METH_VARARGS },
    { "connect_object_after", (PyCFunction)_wrap_gtk_signal_connect_object_after, METH_VARARGS },
    { "signal_disconnect_by_data", (PyCFunction)_wrap_gtk_signal_disconnect_by_data, METH_VARARGS },
    { "signal_handler_block_by_data", (PyCFunction)_wrap_gtk_signal_handler_block_by_data, METH_VARARGS },
    { "signal_handler_unblock_by_data", (PyCFunction)_wrap_gtk_signal_handler_unblock_by_data, METH_VARARGS },
    { "emit", (PyCFunction)_wrap_gtk_signal_emitv_by_name, METH_VARARGS },
    { "set", (PyCFunction)_wrap_gtk_object_set, METH_VARARGS },
    { "get", (PyCFunction)_wrap_gtk_object_get, METH_VARARGS },
    { "set_data", (PyCFunction)_wrap_gtk_object_set_data, METH_VARARGS },
    { "get_data", (PyCFunction)_wrap_gtk_object_get_data, METH_VARARGS },
    { NULL, NULL, 0 }
};

PyMethodChain base_object_method_chain = METHOD_CHAIN(base_object_methods);
