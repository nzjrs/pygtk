/* -*- Mode: C; c-basic-offset: 4 -*- */
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include <Python.h>
//#include <pygtk.h>
#include <pygobject.h>

#include <gtkgl/gtkglarea.h>

void pygtkgl_register_classes(PyObject *d);
extern PyMethodDef pygtkgl_functions[];

DL_EXPORT(void)
initgl(void)
{
    PyObject *m, *d, *c;

    init_pygobject();
    //init_pygtk();

    m = Py_InitModule("gl", pygtkgl_functions);
    d = PyModule_GetDict(m);

    pygtkgl_register_classes(d);

    PyDict_SetItemString(d, "NONE",
			 c=PyInt_FromLong(GDK_GL_NONE));
    Py_DECREF(c);
    PyDict_SetItemString(d, "USE_GL", c=PyInt_FromLong(GDK_GL_USE_GL));
    Py_DECREF(c);
    PyDict_SetItemString(d, "BUFFER_SIZE",
			 c=PyInt_FromLong(GDK_GL_BUFFER_SIZE));
    Py_DECREF(c);
    PyDict_SetItemString(d, "LEVEL", c=PyInt_FromLong(GDK_GL_LEVEL));
    Py_DECREF(c);
    PyDict_SetItemString(d, "RGBA", c=PyInt_FromLong(GDK_GL_RGBA));
    Py_DECREF(c);
    PyDict_SetItemString(d, "DOUBLEBUFFER",
			 c=PyInt_FromLong(GDK_GL_DOUBLEBUFFER));
    Py_DECREF(c);
    PyDict_SetItemString(d, "STEREO", c=PyInt_FromLong(GDK_GL_STEREO));
    Py_DECREF(c);
    PyDict_SetItemString(d, "AUX_BUFFERS",
			 c=PyInt_FromLong(GDK_GL_AUX_BUFFERS));
    Py_DECREF(c);
    PyDict_SetItemString(d, "RED_SIZE", c=PyInt_FromLong(GDK_GL_RED_SIZE));
    Py_DECREF(c);
    PyDict_SetItemString(d, "GREEN_SIZE", c=PyInt_FromLong(GDK_GL_GREEN_SIZE));
    Py_DECREF(c);
    PyDict_SetItemString(d, "BLUE_SIZE", c=PyInt_FromLong(GDK_GL_BLUE_SIZE));
    Py_DECREF(c);
    PyDict_SetItemString(d, "ALPHA_SIZE", c=PyInt_FromLong(GDK_GL_ALPHA_SIZE));
    Py_DECREF(c);
    PyDict_SetItemString(d, "DEPTH_SIZE", c=PyInt_FromLong(GDK_GL_DEPTH_SIZE));
    Py_DECREF(c);
    PyDict_SetItemString(d, "STENCIL_SIZE",
			 c=PyInt_FromLong(GDK_GL_STENCIL_SIZE));
    Py_DECREF(c);
    PyDict_SetItemString(d, "ACCUM_RED_SIZE",
			 c=PyInt_FromLong(GDK_GL_ACCUM_RED_SIZE));
    Py_DECREF(c);
    PyDict_SetItemString(d, "ACCUM_GREEN_SIZE",
			 c=PyInt_FromLong(GDK_GL_ACCUM_GREEN_SIZE));
    Py_DECREF(c);
    PyDict_SetItemString(d, "ACCUM_BLUE_SIZE",
			 c=PyInt_FromLong(GDK_GL_ACCUM_BLUE_SIZE));
    Py_DECREF(c);
    PyDict_SetItemString(d, "ACCUM_ALPHA_SIZE",
			 c=PyInt_FromLong(GDK_GL_ACCUM_ALPHA_SIZE));
    Py_DECREF(c);

    if (PyErr_Occurred())
        Py_FatalError("could not initialise module gtk.gl");
}
