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

/* type objects */
extern PyTypeObject PyGtkAccelGroup_Type;
extern PyTypeObject PyGdkFont_Type;
extern PyTypeObject PyGdkColor_Type;
extern PyTypeObject PyGdkEvent_Type;
extern PyTypeObject PyGdkVisual_Type;
extern PyTypeObject PyGtkSelectionData_Type;
extern PyTypeObject PyGdkAtom_Type;
extern PyTypeObject PyGdkCursor_Type;
extern PyTypeObject PyGtkCTreeNode_Type;

/* check the type of a PyObject */
#define PyGtkAccelGroup_Check(v) ((v)->ob_type == &PyGtkAccelGroup_Type)
#define PyGdkFont_Check(v) ((v)->ob_type == &PyGdkFont_Type)
#define PyGdkColor_Check(v) ((v)->ob_type == &PyGdkColor_Type)
#define PyGdkEvent_Check(v) ((v)->ob_type == &PyGdkEvent_Type)
#define PyGdkVisual_Check(v) ((v)->ob_type == &PyGdkVisual_Type)
#define PyGtkSelectionData_Check(v) ((v)->ob_type == &PyGtkSelectionData_Type)
#define PyGdkAtom_Check(v) ((v)->ob_type == &PyGdkAtom_Type)
#define PyGdkCursor_Check(v) ((v)->ob_type == &PyGdkCursor_Type)
#define PyGtkCTreeNode_Check(v) ((v)->ob_type == &PyGtkCTreeNode_Type)

/* constructors for PyObject wrappers ... */
PyObject *PyGtkAccelGroup_New(GtkAccelGroup *obj);
PyObject *PyGdkFont_New(GdkFont *font);
PyObject *PyGdkColor_New(GdkColor *colour);
PyObject *PyGdkEvent_New(GdkEvent *event);
PyObject *PyGdkVisual_New(GdkVisual *visual);
PyObject *PyGtkSelectionData_New(GtkSelectionData *data);
PyObject *PyGdkAtom_New(GdkAtom atom);
PyObject *PyGdkCursor_New(GdkCursor *cursor);
PyObject *PyGtkCTreeNode_New(GtkCTreeNode *node);

/* miscelaneous functions */
void pygtk_block_threads(void);
void pygtk_unblock_threads(void);

void pygtk_destroy_notify(gpointer data);
void pygtk_callback_marshal(GtkObject *o, gpointer d, guint nargs,
			    GtkArg *args);
void pygtk_signal_marshal(GtkObject *object, gpointer user_data,
			  int nparams, GtkArg *args, GtkType *arg_types,
			  GtkType return_type);
void pygtk_handler_marshal(gpointer a, PyObject *func, int nargs,GtkArg *args);
void pygtk_input_marshal(gpointer a, PyObject *func, int nargs, GtkArg *args);

PyObject *pygtk_args_as_tuple(int nparams, GtkArg *args);
int pygtk_args_from_sequence(GtkArg *args, int nparams, PyObject *seq);
int pygtk_arg_from_pyobject(GtkArg *arg, PyObject *obj);
PyObject *pygtk_arg_as_pyobject(GtkArg *arg);
void pygtk_ret_from_pyobject(GtkArg *ret, PyObject *py_ret);
PyObject *pygtk_ret_as_pyobject(GtkArg *arg);
GtkArg *pygtk_dict_as_args(PyObject *dict, GtkType type, gint *nargs);
GtkArg *pygtk_dict_as_container_args(PyObject *dict, GtkType type,
					    gint *nargs);

void pygtk_register_boxed(GtkType boxed_type,
			  PyObject *(*fromarg)(gpointer boxed),
			  int (*toarg)(gpointer *boxed, PyObject *obj));

static gboolean PyGtk_FatalExceptions = FALSE;

#endif
