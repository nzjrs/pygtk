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


#ifdef ENABLE_PYGTK_THREADING
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
	if (pyg_enum_get_value(arg->type, obj, &(GTK_VALUE_ENUM(*arg))))
	    return -1;
	break;
    case GTK_TYPE_FLAGS:
	if (pyg_flags_get_value(arg->type, obj, &(GTK_VALUE_FLAGS(*arg))))
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
    case G_TYPE_OBJECT:
	if (pygobject_check(obj, pygobject_lookup_class(arg->type)))
	    GTK_VALUE_OBJECT(*arg) = GTK_OBJECT(pygobject_get(obj));
	else
	    return -1;
	break;
    case GTK_TYPE_BOXED:
	if (arg->type == GTK_TYPE_ACCEL_GROUP) {
	    if (PyGtkAccelGroup_Check(obj))
		GTK_VALUE_BOXED(*arg) = PyGtkAccelGroup_Get(obj);
	    else
		return -1;
	} else if (arg->type == GDK_TYPE_EVENT) {
	    if (PyGdkEvent_Check(obj))
		GTK_VALUE_BOXED(*arg) = PyGdkEvent_Get(obj);
	    else
		return -1;
	} else if (arg->type == GDK_TYPE_FONT) {
	    if (PyGdkFont_Check(obj))
		GTK_VALUE_BOXED(*arg) = PyGdkFont_Get(obj);
	    else
		return -1;
	} else if (arg->type == GDK_TYPE_COLOR) {
	    if (PyGdkColor_Check(obj))
		GTK_VALUE_BOXED(*arg) = PyGdkColor_Get(obj);
	    else if (obj == Py_None)
		GTK_VALUE_BOXED(*arg) = NULL;
	    else
		return -1;
	} else if (arg->type == GDK_TYPE_VISUAL) {
	    if (PyGdkVisual_Check(obj))
		GTK_VALUE_BOXED(*arg) = PyGdkVisual_Get(obj);
	    else if (obj == Py_None)
		GTK_VALUE_BOXED(*arg) = NULL;
	    else
		return -1;
	} else if (arg->type == GTK_TYPE_SELECTION_DATA) {
	    if (PyGtkSelectionData_Check(obj))
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
    case G_TYPE_OBJECT:
	if (GTK_VALUE_OBJECT(*arg) != NULL)
	    return pygobject_new((GObject *)GTK_VALUE_OBJECT(*arg));
	else {
	    Py_INCREF(Py_None);
	    return Py_None;
	}
    case GTK_TYPE_POINTER:
	return PyCObject_FromVoidPtr(GTK_VALUE_POINTER(*arg), NULL);
    case GTK_TYPE_BOXED:
	if (arg->type == GTK_TYPE_ACCEL_GROUP)
	    return PyGtkAccelGroup_New(GTK_VALUE_BOXED(*arg));
	else if (arg->type == GDK_TYPE_EVENT)
	    return PyGdkEvent_New(GTK_VALUE_BOXED(*arg));
	else if (arg->type == GDK_TYPE_FONT)
	    return PyGdkFont_New(GTK_VALUE_BOXED(*arg));
	else if (arg->type == GDK_TYPE_COLOR)
	    return PyGdkColor_New(GTK_VALUE_BOXED(*arg));
	else if (arg->type == GDK_TYPE_VISUAL)
	    return PyGdkVisual_New(GTK_VALUE_BOXED(*arg));
	else if (arg->type == GTK_TYPE_SELECTION_DATA)
	    return PyGtkSelectionData_New(GTK_VALUE_BOXED(*arg));
	else {
	    PyGtk_BoxFuncs *fs = pygtk_get_boxed(arg->type);
	    if (fs && fs->fromarg)
		return fs->fromarg(GTK_VALUE_BOXED(*arg));
	    return PyCObject_FromVoidPtr(GTK_VALUE_BOXED(*arg), NULL);
	}
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
	if (pyg_enum_get_value(ret->type, py_ret, GTK_RETLOC_ENUM(*ret))) {
	    PyErr_Clear();
	    *GTK_RETLOC_ENUM(*ret) = 0;
	}
	break;
    case GTK_TYPE_FLAGS:
	if (pyg_flags_get_value(ret->type, py_ret, GTK_RETLOC_FLAGS(*ret))) {
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
    case G_TYPE_OBJECT:
	if (pygobject_check(py_ret, pygobject_lookup_class(ret->type)))
	    *GTK_RETLOC_OBJECT(*ret) = GTK_OBJECT(pygobject_get(py_ret));
	else
	    *GTK_RETLOC_OBJECT(*ret) = NULL;
	break;
    case GTK_TYPE_BOXED:
	if (ret->type == GTK_TYPE_ACCEL_GROUP) {
	    if (PyGtkAccelGroup_Check(py_ret))
		*GTK_RETLOC_BOXED(*ret) = PyGtkAccelGroup_Get(py_ret);
	    else
		*GTK_RETLOC_BOXED(*ret) = NULL;
	} else if (ret->type == GDK_TYPE_EVENT) {
	    if (PyGdkEvent_Check(py_ret))
		*GTK_RETLOC_BOXED(*ret) = PyGdkEvent_Get(py_ret);
	    else
		*GTK_RETLOC_BOXED(*ret) = NULL;
	} else if (ret->type == GDK_TYPE_FONT) {
	    if (PyGdkFont_Check(py_ret))
		*GTK_RETLOC_BOXED(*ret) = PyGdkFont_Get(py_ret);
	    else
		*GTK_RETLOC_BOXED(*ret) = NULL;
	} else if (ret->type == GDK_TYPE_COLOR) {
	    if (PyGdkColor_Check(py_ret))
		*GTK_RETLOC_BOXED(*ret) = PyGdkColor_Get(py_ret);
	    else
		*GTK_RETLOC_BOXED(*ret) = NULL;
	} else if (ret->type == GDK_TYPE_VISUAL) {
	    if (PyGdkVisual_Check(py_ret))
		*GTK_RETLOC_BOXED(*ret) = PyGdkVisual_Get(py_ret);
	    else
		*GTK_RETLOC_BOXED(*ret) = NULL;
	} else if (ret->type == GTK_TYPE_SELECTION_DATA) {
	    if (PyGtkSelectionData_Check(py_ret))
		*GTK_RETLOC_BOXED(*ret) = PyGtkSelectionData_Get(py_ret);
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
    case G_TYPE_OBJECT:
	return pygobject_new((GObject *)*GTK_RETLOC_OBJECT(*arg));
    case GTK_TYPE_POINTER:
	return PyCObject_FromVoidPtr(*GTK_RETLOC_POINTER(*arg), NULL);
    case GTK_TYPE_BOXED:
	if (arg->type == GTK_TYPE_ACCEL_GROUP)
	    return PyGtkAccelGroup_New(*GTK_RETLOC_BOXED(*arg));
	else if (arg->type == GDK_TYPE_EVENT)
	    return PyGdkEvent_New(*GTK_RETLOC_BOXED(*arg));
	else if (arg->type == GDK_TYPE_FONT)
	    return PyGdkFont_New(*GTK_RETLOC_BOXED(*arg));
	else if (arg->type == GDK_TYPE_COLOR)
	    return PyGdkColor_New(*GTK_RETLOC_BOXED(*arg));
	else if (arg->type == GDK_TYPE_VISUAL)
	    return PyGdkVisual_New(*GTK_RETLOC_BOXED(*arg));
	else if (arg->type == GTK_TYPE_SELECTION_DATA)
	    return PyGtkSelectionData_New(*GTK_RETLOC_BOXED(*arg));
	else {
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
	obj = pygobject_new((GObject *)o);

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
    PyTuple_SetItem(ret, 0, pygobject_new((GObject *)object));
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

#if 0
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
#endif


