/* -*- Mode: C; c-basic-offset: 4 -*- */
#ifndef _PYGTK_H_
#define _PYGTK_H_

#define NO_IMPORT_PYGOBJECT
#include <pygobject.h>
#include <Python.h>
#include <gtk/gtk.h>

struct _PyGtk_FunctionStruct {
    char *pygtkVersion;

    PyTypeObject *gdkAtom_type;
    PyObject *(* gdkAtom_new)(GdkAtom atom);
};

/* structure definitions for the various object types in PyGTK */
typedef struct {
    PyObject_HEAD
    gchar *name;
    GdkAtom atom;
} PyGdkAtom_Object;

/* routines to get the C object value out of the PyObject wrapper */
#define PyGdkAtom_Get(v) (((PyGdkAtom_Object *)(v))->atom)

/* this section is dependent on whether we are being included from gtkmodule.c
 * or not.  A similar source level interface should be provided in both
 * instances. */
#ifndef _INSIDE_PYGTK_

/* for multi file extensions, define one of these in all but the main file
 * of the module */
#if defined(NO_IMPORT) || defined(NO_IMPORT_PYGTK)
extern struct _PyGtk_FunctionStruct *_PyGtk_API;
#else
struct _PyGtk_FunctionStruct *_PyGtk_API;
#endif

/* type objects */
#define PyGdkAtom_Type          *(_PyGtk_API->gdkAtom_type)

/* type checking routines */
#define PyGdkAtom_Check(v) ((v)->ob_type == _PyGtk_API->gdkAtom_type)

/* type objects */
#define PyGdkAtom_New          (_PyGtk_API->gdkAtom_new)

/* some variables */
#define PYGTK_VERSION (_PyGtk_API->pygtkVersion)

/* a function to initialise the pygtk functions */
#define init_pygtk() { \
    PyObject *pygtk = PyImport_ImportModule("gtk._gtk"); \
    if (pygtk != NULL) { \
	PyObject *module_dict = PyModule_GetDict(pygtk); \
	PyObject *cobject = PyDict_GetItemString(module_dict, "_PyGtk_API"); \
	if (PyCObject_Check(cobject)) \
	    _PyGtk_API = (struct _PyGtk_FunctionStruct*) \
		PyCObject_AsVoidPtr(cobject); \
	else { \
	    Py_FatalError("could not find _PyGtk_API object"); \
	    return; \
	} \
    } else { \
	Py_FatalError("could not import _gtk"); \
	return; \
    } \
}

#endif

#endif /* !_PYGTK_H_ */
