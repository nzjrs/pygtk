/* -*- Mode: C; c-basic-offset: 4 -*- */
#ifndef _PYGDKIMLIB_H_
#define _PYGDKIMLIB_H_

#include <Python.h>
#include <gdk_imlib.h>

struct _PyGdkImlib_FunctionStruct {
    PyTypeObject *image_type;
    PyObject *(* image_new)(GdkImlibImage *img);
};

typedef struct {
    PyObject_HEAD
    GdkImlibImage *obj;
} PyGdkImlibImage_Object;

#define PyGdkImlibImage_Get(ob) (((PyGdkImlibImage_Object *)(ob))->obj)

#ifdef _INSIDE_PYGDKIMLIB_

#define PyGdkImlibImage_Check(ob) ((ob)->ob_type == &PyGdkImlibImage_Type)
staticforward PyTypeObject PyGdkImlibImage_Type;
static PyObject *PyGdkImlibImage_New(GdkImlibImage *img);

#else

#if defined(NO_IMPORT) || defined(NO_IMPORT_PYGDKIMLIB)
extern struct _PyGdkImlib_FunctionStruct *_PyGdkI_API;
#else
struct _PyGdkImlib_FunctionStruct *_PyGdkI_API;
#endif

#define PyGdkImlibImage_Check(ob) ((ob)->ob_type == _PyGdkI_API->image_type)
#define PyGdkImlibImage_Type *(_PyGdkI_API->image_type)
#define PyGdkImlibImage_New (_PyGdkI_API->image_new)

#define init_pygdkimlib() { \
    PyObject *pygtk = PyImport_ImportModule("_gdkimlib"); \
    if (pygtk != NULL) { \
	PyObject *module_dict = PyModule_GetDict(pygtk); \
	PyObject *cobject = PyDict_GetItemString(module_dict, "_PyGdkI_API"); \
	if (PyCObject_Check(cobject)) \
	    _PyGdkI_API = PyCObject_AsVoidPtr(cobject); \
	else { \
	    Py_FatalError("could not find _PyGdkI_API object"); \
	    return; \
	} \
    } else { \
	Py_FatalError("could not import _gdkimlib"); \
	return; \
    } \
}

#endif

#endif /* !_PYGDKIMLIB_H_ */
