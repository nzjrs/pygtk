/* -*- Mode: C; c-basic-offset: 4 -*- */
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include <Python.h>
#include <pygobject.h>

#include <pango/pango-font.h>

/* include any extra headers needed here */

void pypango_register_classes(PyObject *d);
void pypango_add_constants(PyObject *module, const gchar *strip_prefix);
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
    pypango_add_constants(m, "PANGO_");
    
    PyModule_AddObject(m, "SCALE_XX_SMALL",
		       PyFloat_FromDouble(PANGO_SCALE_XX_SMALL));
    PyModule_AddObject(m, "SCALE_X_SMALL",
		       PyFloat_FromDouble(PANGO_SCALE_X_SMALL));
    PyModule_AddObject(m, "SCALE_SMALL",
		       PyFloat_FromDouble(PANGO_SCALE_SMALL));
    PyModule_AddObject(m, "SCALE_MEDIUM",
		       PyFloat_FromDouble(PANGO_SCALE_MEDIUM));
    PyModule_AddObject(m, "SCALE_LARGE",
		       PyFloat_FromDouble(PANGO_SCALE_LARGE));
    PyModule_AddObject(m, "SCALE_X_LARGE",
		       PyFloat_FromDouble(PANGO_SCALE_LARGE));
    PyModule_AddObject(m, "SCALE_XX_LARGE",
		       PyFloat_FromDouble(PANGO_SCALE_LARGE));    

    /* add anything else to the module dictionary (such as constants) */

    if (PyErr_Occurred())
        Py_FatalError("could not initialise module pango");
}
