/* -*- Mode: C; c-basic-offset: 4 -*- */
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include <Python.h>
#include <pygtk.h>

#include <glade/glade.h>


void libglade_register_classes(PyObject *d);
extern PyMethodDef libglade_functions[];

DL_EXPORT(void)
initlibglade(void)
{
    PyObject *m, *d;

    init_pygtk();
    glade_init();

    m = Py_InitModule("libglade", libglade_functions);
    d = PyModule_GetDict(m);

    libglade_register_classes(d);

    /* add anything else to the module dictionary (such as constants) */

    if (PyErr_Occurred())
        Py_FatalError("could not initialise module libglade");
}
