/* -*- Mode: C; c-basic-offset: 4 -*- */
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include <Python.h>
#include <pygobject.h>

/* include any extra headers needed here */

void pypango_register_classes(PyObject *d);
extern PyMethodDef pypango_functions[];

DL_EXPORT(void)
initpango(void)
{
    PyObject *m, *d;

    /* perform any initialisation required by the library here */

    m = Py_InitModule("pango", pypango_functions);
    d = PyModule_GetDict(m);

    init_pygobject();

    pypango_register_classes(d);

    /* add anything else to the module dictionary (such as constants) */

    if (PyErr_Occurred())
        Py_FatalError("could not initialise module pango");
}
