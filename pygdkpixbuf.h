/* -*- Mode: C; c-basic-offset: 4 -*- */
#ifndef _PYGDKPIXBUF_H_
#define _PYGDKPIXBUF_H_

#include <Python.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

struct _PyGdkPixbuf_FunctionStruct {
  PyTypeObject *pixbuf_type;
  PyObject *(* pixbuf_new)(GdkPixbuf *pixbuf);
  PyTypeObject *pixbuf_animation_type;
  PyObject *(* pixbuf_animation_new)(GdkPixbufAnimation *pixbuf_animation);
};

typedef struct {
  PyObject_HEAD
  GdkPixbuf *pixbuf;
} PyGdkPixbuf_Object;

typedef struct {
  PyObject_HEAD
  GdkPixbufAnimation *animation;
} PyGdkPixbufAnimation_Object;

#define PyGdkPixbuf_Get(ob) (((PyGdkPixbuf_Object *)(ob))->pixbuf)
#define PyGdkPixbufAnimation_Get(ob) (((PyGdkPixbufAnimation_Object *)(ob))->animation)

#ifdef _INSIDE_PYGDKPIXBUF_

#define PyGdkPixbuf_Check(ob) ((ob)->ob_type == &PyGdkPixbuf_Type)
staticforward PyTypeObject PyGdkPixbuf_Type;
static PyObject *PyGdkPixbuf_New(GdkPixbuf *pixbuf);

#define PyGdkPixbufAnimation_Check(ob) ((ob)->ob_type == &PyGdkPixbufAnimation_Type)
staticforward PyTypeObject PyGdkPixbufAnimation_Type;
static PyObject *PyGdkPixbufAnimation_New(GdkPixbufAnimation *animation);

#else

#if defined(NO_IMPORT) || defined(NO_IMPORT_PYGDKPIXBUF)
extern struct _PyGdkPixbuf_FunctionStruct *_PyGdkPixbuf_API;
#else
struct _PyGdkPixbuf_FunctionStruct *_PyGdkPixbuf_API;
#endif

#define PyGdkPixbuf_Check(ob) ((ob)->ob_type == _PyGdkPixbuf_API->pixbuf_type)
#define PyGdkPixbuf_Type *(_PyGdkPixbuf_API->pixbuf_type)
#define PyGdkPixbuf_New (_PyGdkPixbuf_API->pixbuf_new)
#define PyGdkPixbufAnimation_Check(ob) ((ob)->ob_type == _PyGdkPixbuf_API->pixbuf_animation_type)
#define PyGdkPixbufAnimation_Type *(_PyGdkPixbuf_API->pixbuf_animation_type)
#define PyGdkPixbufAnimation_New (_PyGdkPixbuf_API->pixbuf_animation_new)

#define init_pygdkpixbuf() { \
    PyObject *pygtk = PyImport_ImportModule("gdkpixbuf"); \
    if (pygtk != NULL) { \
        PyObject *module_dict = PyModule_GetDict(pygtk); \
        PyObject *cobject = PyDict_GetItemString(module_dict, "_PyGdkPixbuf_API"); \
        if (PyCObject_Check(cobject)) \
            _PyGdkPixbuf_API = PyCObject_AsVoidPtr(cobject); \
        else { \
            Py_FatalError("could not find _PyGdkPixbuf_API object"); \
            return; \
        } \
    } else { \
        Py_FatalError("could not import gdkpixbuf"); \
        return; \
    } \
}

#endif

#endif /* !_PYGDKPIXBUF_H_ */
