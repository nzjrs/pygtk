/* -*- Mode: C; c-basic-offset: 4 -*- */

/* this module provides some of the base functionality of the GtkObject
 * wrapper system */

#include "pygtk-private.h"

/* ------------------- object support */

void
pygtk_custom_destroy_notify(gpointer user_data)
{
    PyGtkCustomNotify *cunote = user_data;

    pyg_block_threads();
    Py_XDECREF(cunote->func);
    Py_XDECREF(cunote->data);
    pyg_unblock_threads();
    g_free(cunote);
}

/* used for timeout and idle functions */
void
pygtk_handler_marshal(gpointer a, PyObject *func, int nargs, GtkArg *args)
{
    PyObject *ret;

    pyg_block_threads();

    if (PyTuple_Check(func))
	ret = PyObject_CallObject(PyTuple_GetItem(func, 0),
				  PyTuple_GetItem(func, 1));
    else
	ret = PyObject_CallObject(func, NULL);
    if (ret == NULL) {
	PyErr_Print();
	PyErr_Clear();
	*GTK_RETLOC_BOOL(args[0]) = FALSE;
	pyg_unblock_threads();
	return;
    }
    if (ret == Py_None || (PyInt_Check(ret) && PyInt_AsLong(ret) == 0))
	*GTK_RETLOC_BOOL(args[0]) = FALSE;
    else
	*GTK_RETLOC_BOOL(args[0]) = TRUE;
    Py_DECREF(ret);
    pyg_unblock_threads();
}

/* callback for input handlers */
void
pygtk_input_marshal(gpointer a, PyObject *func, int nargs, GtkArg *args)
{
    PyObject *tuple, *ret;

    pyg_block_threads();
    tuple = Py_BuildValue("(ii)", GTK_VALUE_INT(args[0]),
			  GTK_VALUE_FLAGS(args[1]));
    ret = PyObject_CallObject(func, tuple);
    Py_DECREF(tuple);
    if (ret == NULL) {
	PyErr_Print();
	PyErr_Clear();
    } else
	Py_DECREF(ret);
    pyg_unblock_threads();
}


