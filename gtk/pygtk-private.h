#ifndef _PYGTK_PRIVATE_H_
#define _PYGTK_PRIVATE_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef _PYGTK_H_
#error "don't include pygtk.h and pygtk-private.h together"
#endif

#define _INSIDE_PYGTK_
#include "pygtk.h"
#undef WITH_THREAD

/* type objects */
extern PyTypeObject PyGdkEvent_Type;
extern PyTypeObject PyGtkSelectionData_Type;
extern PyTypeObject PyGdkAtom_Type;
extern PyTypeObject PyGdkCursor_Type;
extern PyTypeObject PyGtkCTreeNode_Type;
extern PyTypeObject PyGdkDevice_Type;
extern PyTypeObject PyGtkTextIter_Type;

/* check the type of a PyObject */
#define PyGdkEvent_Check(v) ((v)->ob_type == &PyGdkEvent_Type)
#define PyGtkSelectionData_Check(v) ((v)->ob_type == &PyGtkSelectionData_Type)
#define PyGdkAtom_Check(v) ((v)->ob_type == &PyGdkAtom_Type)
#define PyGdkCursor_Check(v) ((v)->ob_type == &PyGdkCursor_Type)
#define PyGtkCTreeNode_Check(v) ((v)->ob_type == &PyGtkCTreeNode_Type)
#define PyGdkDevice_Check(v) ((v)->ob_type == &PyGdkDevice_Type)
#define PyGtkTextIter_Check(v) ((v)->ob_type == &PyGtkTextIter_Type)

/* constructors for PyObject wrappers ... */
PyObject *PyGdkEvent_New(GdkEvent *event);
PyObject *PyGtkSelectionData_New(GtkSelectionData *data);
PyObject *PyGdkAtom_New(GdkAtom atom);
PyObject *PyGdkCursor_New(GdkCursor *cursor);
PyObject *PyGtkCTreeNode_New(GtkCTreeNode *node);
PyObject *PyGdkDevice_New(GdkDevice *device);
PyObject *PyGtkTextIter_New(GtkTextIter *iter);

/* miscelaneous functions */
void pygtk_block_threads(void);
void pygtk_unblock_threads(void);

void pygtk_destroy_notify(gpointer data);

void pygtk_handler_marshal(gpointer a, PyObject *func, int nargs,GtkArg *args);
void pygtk_input_marshal(gpointer a, PyObject *func, int nargs, GtkArg *args);

/* private */
PyObject    *pygtk_tree_path_to_pyobject(GtkTreePath *path);
GtkTreePath *pygtk_tree_path_from_pyobject(PyObject *object);

static gboolean PyGtk_FatalExceptions = FALSE;

#endif
