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



