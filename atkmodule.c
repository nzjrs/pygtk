/* -*- Mode: C; c-basic-offset: 4 -*- */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* include this first, before NO_IMPORT_PYGOBJECT is defined */
#include <pygobject.h>

void pyatk_register_classes (PyObject *d);
void pyatk_add_constants(PyObject *module, const gchar *strip_prefix);

extern PyMethodDef pyatk_functions[];

DL_EXPORT(void)
initatk(void)
{
    PyObject *m, *d;
	
    init_pygobject ();

    m = Py_InitModule ("atk", pyatk_functions);
    d = PyModule_GetDict (m);
	
    pyatk_register_classes (d);
    pyatk_add_constants(m, "ATK_");    

    if (PyErr_Occurred ()) {
	Py_FatalError ("can't initialise module atk");
    }
}
