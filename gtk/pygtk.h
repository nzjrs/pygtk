/* -*- Mode: C; c-basic-offset: 4 -*- */
#ifndef _PYGTK_H_
#define _PYGTK_H_

#define NO_IMPORT_PYGOBJECT
#include <pygobject.h>
#include <Python.h>
#include <ExtensionClass.h>
#include <gtk/gtk.h>

struct _PyGtk_FunctionStruct {
    char *pygtkVersion;
    gboolean fatalExceptions;

    void (* block_threads)(void);
    void (* unblock_threads)(void);

    GtkDestroyNotify destroy_notify;

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

/* miscelaneous other functions */
#define pygtk_block_threads (_PyGtk_API->block_threads)
#define pygtk_unblock_threads (_PyGtk_API->unblock_threads)
#define pygtk_destroy_notify (_PyGtk_API->destroy_notify)

/* some variables */
#define PyGtk_FatalExceptions (_PyGtk_API->fatalExceptions)
#define PYGTK_VERSION (_PyGtk_API->pygtkVersion)

/* a function to initialise the pygtk functions */
#define init_pygtk() { \
    PyObject *pygtk = PyImport_ImportModule("gtk._gtk"); \
    if (pygtk != NULL) { \
	PyObject *module_dict = PyModule_GetDict(pygtk); \
	PyObject *cobject = PyDict_GetItemString(module_dict, "_PyGtk_API"); \
	if (PyCObject_Check(cobject)) \
	    _PyGtk_API = PyCObject_AsVoidPtr(cobject); \
	else { \
	    Py_FatalError("could not find _PyGtk_API object"); \
	    return; \
	} \
    } else { \
	Py_FatalError("could not import _gtk"); \
	return; \
    } \
    ExtensionClassImported; \
}

#endif

#endif /* !_PYGTK_H_ */
