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

