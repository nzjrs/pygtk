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
    GtkCallbackMarshal callback_marshal;

    PyObject *(* args_as_tuple)(int nparams, GtkArg *args);
    int (* args_from_sequence)(GtkArg *args, int nparams, PyObject *seq);
    int (* arg_from_pyobject)(GtkArg *arg, PyObject *obj);
    PyObject *(* arg_as_pyobject)(GtkArg *arg);
    void (* ret_from_pyobject)(GtkArg *ret, PyObject *obj);
    PyObject *(* ret_as_pyobject)(GtkArg *ret);

    void (* register_boxed)(GtkType boxed_type,
			    PyObject *(*from_func)(gpointer boxed),
			    int (*to_func)(gpointer *boxed, PyObject *obj));

    PyTypeObject *gdkFont_type;
    PyObject *(* gdkFont_new)(GdkFont *font);

    PyTypeObject *gdkColor_type;
    PyObject *(* gdkColor_new)(GdkColor *colour);

    PyTypeObject *gdkEvent_type;
    PyObject *(* gdkEvent_new)(GdkEvent *event);

    PyTypeObject *gdkVisual_type;
    PyObject *(* gdkVisual_new)(GdkVisual *visual);

    PyTypeObject *gtkSelectionData_type;
    PyObject *(* gtkSelectionData_new)(GtkSelectionData *data);

    PyTypeObject *gdkAtom_type;
    PyObject *(* gdkAtom_new)(GdkAtom atom);

    PyTypeObject *gdkCursor_type;
    PyObject *(* gdkCursor_new)(GdkCursor *cursor);

    PyTypeObject *gtkCTreeNode_type;
    PyObject *(* gtkCTreeNode_new)(GtkCTreeNode *node);

    PyTypeObject *gdkDevice_type;
    PyObject *(* gdkDevice_new)(GdkDevice *device);

    PyTypeObject *gtkTextIter_type;
    PyObject *(* gtkTextIter_new)(GtkTextIter *iter);

    PyTypeObject *gtkTreeIter_type;
    PyObject *(* gtkTreeIter_new)(GtkTreeIter *iter);
};

/* structure definitions for the various object types in PyGTK */
typedef struct {
    PyObject_HEAD
    GdkFont *obj;
} PyGdkFont_Object;

typedef struct {
    PyObject_HEAD
    GdkColor obj;
} PyGdkColor_Object;

typedef struct {
    PyObject_HEAD
    GdkEvent *obj;
    PyObject *attrs;
} PyGdkEvent_Object;

typedef struct {
    PyObject_HEAD
    GdkVisual *obj;
} PyGdkVisual_Object;

typedef struct {
    PyObject_HEAD
    GtkSelectionData *obj;
} PyGtkSelectionData_Object;

typedef struct {
    PyObject_HEAD
    gchar *name;
    GdkAtom atom;
} PyGdkAtom_Object;

typedef struct {
    PyObject_HEAD
    GdkCursor *obj;
} PyGdkCursor_Object;

typedef struct {
    PyObject_HEAD
    GtkCTreeNode *node;
} PyGtkCTreeNode_Object;

typedef struct {
    PyObject_HEAD
    GdkDevice *obj;
} PyGdkDevice_Object;

typedef struct {
    PyObject_HEAD
    GtkTextIter iter;
} PyGtkTextIter_Object;

typedef struct {
    PyObject_HEAD
    GtkTreeIter iter;
} PyGtkTreeIter_Object;

/* routines to get the C object value out of the PyObject wrapper */
#define PyGdkFont_Get(v) (((PyGdkFont_Object *)(v))->obj)
#define PyGdkColor_Get(v) (&((PyGdkColor_Object *)(v))->obj)
#define PyGdkEvent_Get(v) (((PyGdkEvent_Object *)(v))->obj)
#define PyGdkVisual_Get(v) (((PyGdkVisual_Object *)(v))->obj)
#define PyGtkSelectionData_Get(v) (((PyGtkSelectionData_Object *)(v))->obj)
#define PyGdkAtom_Get(v) (((PyGdkAtom_Object *)(v))->atom)
#define PyGdkCursor_Get(v) (((PyGdkCursor_Object *)(v))->obj)
#define PyGtkCTreeNode_Get(v) (((PyGtkCTreeNode_Object *)(v))->node)
#define PyGdkDevice_Get(v) (((PyGdkDevice_Object *)(v))->obj)
#define PyGtkTextIter_Get(v) (&((PyGtkTextIter_Object *)(v))->iter)
#define PyGtkTreeIter_Get(v) (&((PyGtkTreeIter_Object *)(v))->iter)

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
#define PyGdkFont_Type          *(_PyGtk_API->gdkFont_type)
#define PyGdkColor_Type         *(_PyGtk_API->gdkColor_type)
#define PyGdkEvent_Type         *(_PyGtk_API->gdkEvent_type)
#define PyGdkVisual_Type        *(_PyGtk_API->gdkVisual_type)
#define PyGtkSelectionData_Type *(_PyGtk_API->gtkSelectionData_type)
#define PyGdkAtom_Type          *(_PyGtk_API->gdkAtom_type)
#define PyGdkCursor_Type        *(_PyGtk_API->gdkCursor_type)
#define PyGtkCTreeNode_Type     *(_PyGtk_API->gtkCTreeNode_type)
#define PyGdkDevice_Type        *(_PyGtk_API->gdkDevice_type)
#define PyGtkTextIter_Type      *(_PyGtk_API->gtkTextIter_type)
#define PyGtkTreeIter_Type      *(_PyGtk_API->gtkTreeIter_type)

/* type checking routines */
#define PyGdkFont_Check(v) ((v)->ob_type == _PyGtk_API->gdkFont_type)
#define PyGdkColor_Check(v) ((v)->ob_type == _PyGtk_API->gdkColor_type)
#define PyGdkEvent_Check(v) ((v)->ob_type == _PyGtk_API->gdkEvent_type)
#define PyGdkVisual_Check(v) ((v)->ob_type == _PyGtk_API->gdkVisual_type)
#define PyGtkSelectionData_Check(v) ((v)->ob_type == _PyGtk_API->gtkSelectionData_type)
#define PyGdkAtom_Check(v) ((v)->ob_type == _PyGtk_API->gdkAtom_type)
#define PyGdkCursor_Check(v) ((v)->ob_type == _PyGtk_API->gdkCursor_type)
#define PyGtkCTreeNode_Check(v) ((v)->ob_type == _PyGtk_API->gtkCTreeNode_type)
#define PyGdkDevice_Check(v) ((v)->ob_type == _PyGtk_API->gdkDevice_type)
#define PyGtkTextIter_Check(v) ((v)->ob_type == _PyGtk_API->gtkTextIter_type)
#define PyGtkTreeIter_Check(v) ((v)->ob_type == _PyGtk_API->gtkTreeIter_type)

/* type objects */
#define PyGdkFont_New          (_PyGtk_API->gdkFont_new)
#define PyGdkColor_New         (_PyGtk_API->gdkColor_new)
#define PyGdkEvent_New         (_PyGtk_API->gdkEvent_new)
#define PyGdkVisual_New        (_PyGtk_API->gdkVisual_new)
#define PyGtkSelectionData_New (_PyGtk_API->gtkSelectionData_new)
#define PyGdkAtom_New          (_PyGtk_API->gdkAtom_new)
#define PyGdkCursor_New        (_PyGtk_API->gdkCursor_new)
#define PyGtkCTreeNode_New     (_PyGtk_API->gtkCTreeNode_new)
#define PyGdkDevice_New        (_PyGtk_API->gdkDevice_new)
#define PyGtkTextIter_New      (_PyGtk_API->gtkTextIter_new)
#define PyGtkTreeIter_New      (_PyGtk_API->gtkTreeIter_new)

/* miscelaneous other functions */
#define pygtk_block_threads (_PyGtk_API->block_threads)
#define pygtk_unblock_threads (_PyGtk_API->unblock_threads)
#define pygtk_destroy_notify (_PyGtk_API->destroy_notify)
#define pygtk_callback_marshal (_PyGtk_API->callback_marshal)
#define pygtk_args_as_tuple (_PyGtk_API->args_as_tuple)
#define pygtk_args_from_sequence (_PyGtk_API->args_from_sequence)
#define pygtk_arg_from_pyobject (_PyGtk_API->arg_from_pyobject)
#define pygtk_arg_as_pyobject (_PyGtk_API->arg_as_pyobject)
#define pygtk_ret_from_pyobject (_PyGtk_API->ret_from_pyobject)
#define pygtk_ret_as_pyobject (_PyGtk_API->ret_as_pyobject)
#define pygtk_register_boxed (_PyGtk_API->register_boxed)

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
