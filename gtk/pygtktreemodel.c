/* -*- Mode: C; c-basic-offset: 4 -*- */
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "pygtktreemodel.h"
#include <Python.h>
#include "pygobject.h"

static void pygtk_tree_model_class_init(PyGtkTreeModelClass *klass);
static void pygtk_tree_model_init(PyGtkTreeModel *self);
static void pygtk_tree_model_iface_init(GtkTreeModelIface *iface);

GType
pygtk_tree_model_get_type(void)
{
    static GType object_type = 0;

    if (!object_type) {
	static const GTypeInfo object_info = {
	    sizeof(PyGtkTreeModelClass),
	    (GBaseInitFunc) NULL,
	    (GBaseFinalizeFunc) NULL,
	    (GClassInitFunc) pygtk_tree_model_class_init,
	    NULL, /* class_finalize */
	    NULL, /* class_data */
	    sizeof(PyGtkTreeModel),
	    0, /* n_preallocs */
	    (GInstanceInitFunc) pygtk_tree_model_init,
	};
	static const GInterfaceInfo tree_model_info = {
	    (GInterfaceInitFunc) pygtk_tree_model_iface_init,
	    NULL,
	    NULL,
	};

	object_type = g_type_register_static(G_TYPE_OBJECT,
					     "PyGtkTreeModel",
					     &object_info, 0);
	g_type_add_interface_static(object_type,
				    GTK_TYPE_TREE_MODEL,
				    &tree_model_info);
    }
    return object_type;
}

enum {
  CHANGED,
  INSERTED,
  CHILD_TOGGLED,
  DELETED,
  LAST_SIGNAL
};

static void
pygtk_tree_model_class_init(PyGtkTreeModelClass *klass)
{
    GType changed_params[] = { GTK_TYPE_TREE_PATH, GTK_TYPE_TREE_ITER };
    GType inserted_params[] = { GTK_TYPE_TREE_PATH, GTK_TYPE_TREE_ITER };
    GType child_toggled_params[] = { GTK_TYPE_TREE_PATH, GTK_TYPE_TREE_ITER };
    GType deleted_params[] = { GTK_TYPE_TREE_PATH };

    g_signal_newv("changed",
		  G_OBJECT_CLASS_TYPE(klass),
		  G_SIGNAL_RUN_FIRST,
		  NULL, /* class_closure */
		  NULL, /* accumulator */
		  gtk_marshal_VOID__BOXED_BOXED,
		  G_TYPE_NONE,
		  2, changed_params);
    g_signal_newv("inserted",
		  G_OBJECT_CLASS_TYPE(klass),
		  G_SIGNAL_RUN_FIRST,
		  NULL, /* class_closure */
		  NULL, /* accumulator */
		  gtk_marshal_VOID__BOXED_BOXED,
		  G_TYPE_NONE,
		  2, inserted_params);
    g_signal_newv("child_toggled",
		  G_OBJECT_CLASS_TYPE(klass),
		  G_SIGNAL_RUN_FIRST,
		  NULL, /* class_closure */
		  NULL, /* accumulator */
		  gtk_marshal_VOID__BOXED_BOXED,
		  G_TYPE_NONE,
		  2, child_toggled_params);
    g_signal_newv("deleted",
		  G_OBJECT_CLASS_TYPE(klass),
		  G_SIGNAL_RUN_FIRST,
		  NULL, /* class_closure */
		  NULL, /* accumulator */
		  gtk_marshal_VOID__BOXED,
		  G_TYPE_NONE,
		  1, deleted_params);
}

static guint pygtk_tree_model_get_flags(GtkTreeModel *tree_model);
static gint pygtk_tree_model_get_n_columns(GtkTreeModel *tree_model);
static GType pygtk_tree_model_get_column_type(GtkTreeModel *tree_model,
					      gint index);
static GtkTreePath *pygtk_tree_model_get_path(GtkTreeModel *tree_model,
					      GtkTreeIter *iter);
static void pygtk_tree_model_get_value(GtkTreeModel*tree_model,
				       GtkTreeIter *iter,
				       gint column, GValue *value);
static gboolean pygtk_tree_model_iter_next(GtkTreeModel *tree_model,
					   GtkTreeIter *iter);
static gboolean pygtk_tree_model_iter_children(GtkTreeModel *tree_model,
					       GtkTreeIter *iter,
					       GtkTreeIter *parent);
static gboolean pygtk_tree_model_iter_has_child(GtkTreeModel *tree_model,
						GtkTreeIter *iter);
static gint pygtk_tree_model_iter_n_children(GtkTreeModel *tree_model,
					     GtkTreeIter *iter);
static gboolean pygtk_tree_model_iter_nth_child(GtkTreeModel *tree_model,
						GtkTreeIter  *iter,
						GtkTreeIter  *parent,
						gint n);
static gboolean pygtk_tree_model_iter_parent(GtkTreeModel *tree_model,
					     GtkTreeIter *iter,
					     GtkTreeIter *child);
static void pygtk_tree_model_ref_iter(GtkTreeModel *tree_model,
				      GtkTreeIter *iter);
static void pygtk_tree_model_unref_iter(GtkTreeModel *tree_model,
					GtkTreeIter *iter);

static void
pygtk_tree_model_iface_init(GtkTreeModelIface *iface)
{
  iface->get_flags = pygtk_tree_model_get_flags;
  iface->get_n_columns = pygtk_tree_model_get_n_columns;
  iface->get_column_type = pygtk_tree_model_get_column_type;
  iface->get_path = pygtk_tree_model_get_path;
  iface->get_value = pygtk_tree_model_get_value;
  iface->iter_next = pygtk_tree_model_iter_next;
  iface->iter_children = pygtk_tree_model_iter_children;
  iface->iter_has_child = pygtk_tree_model_iter_has_child;
  iface->iter_n_children = pygtk_tree_model_iter_n_children;
  iface->iter_nth_child = pygtk_tree_model_iter_nth_child;
  iface->iter_parent = pygtk_tree_model_iter_parent;
  iface->ref_iter = pygtk_tree_model_ref_iter;
  iface->unref_iter = pygtk_tree_model_unref_iter;
}

static void
pygtk_tree_model_init(PyGtkTreeModel *self)
{
}

PyGtkTreeModel *
pygtk_tree_model_new(void)
{
    return PYGTK_TREE_MODEL(g_object_new(PYGTK_TYPE_TREE_MODEL, NULL));
}


/* format of GtkTreeIter's for PyGtkTreeModel:
 *  tree_node1 == python object
 *  tree_node2 == floating reference?
 *
 * I haven't worked out how everything should work.  For now I will
 * leak references.
 */

#define METHOD_PREFIX "on_"

static guint
pygtk_tree_model_get_flags(GtkTreeModel *tree_model)
{
    PyObject *self, *py_ret;

    g_return_val_if_fail(PYGTK_IS_TREE_MODEL(tree_model));
    /* this call finds the wrapper for this GObject */
    self = pygobject_new((GObject *)tree_model);

    py_ret = PyObject_CallMethod(self, METHOD_PREFIX "get_flags", "");
    if (py_ret) {
	guint ret = PyInt_FromLong(py_ret);

	Py_DECREF(Py_None);
	return ret;
    } else {
	PyErr_Print();
	PyErr_Clear();
	return 0;
    }
}

static gint
pygtk_tree_model_get_n_columns(GtkTreeModel *tree_model)
{
    PyObject *self, *py_ret;

    g_return_val_if_fail(tree_model != NULL, 0);
    g_return_val_if_fail(PYGTK_IS_TREE_MODEL(tree_model), 0);
    /* this call finds the wrapper for this GObject */
    self = pygobject_new((GObject *)tree_model);

    py_ret = PyObject_CallMethod(self, METHOD_PREFIX "get_n_columns", "");
    if (py_ret) {
	gint ret = PyInt_FromLong(py_ret);

	Py_DECREF(Py_None);
	return ret;
    } else {
	PyErr_Print();
	PyErr_Clear();
	return 0;
    }
}

static GType
pygtk_tree_model_get_column_type(GtkTreeModel *tree_model, gint index)
{
    PyObject *self, *py_ret;

    g_return_val_if_fail(tree_model != NULL, G_TYPE_INVALID);
    g_return_val_if_fail(PYGTK_IS_TREE_MODEL(tree_model), G_TYPE_INVALID);
    /* this call finds the wrapper for this GObject */
    self = pygobject_new((GObject *)tree_model);

    py_ret = PyObject_CallMethod(self, METHOD_PREFIX "get_column_type",
				 "i", index);
    if (py_ret) {
	GType ret = PyInt_FromLong(py_ret);

	Py_DECREF(Py_None);
	return ret;
    } else {
	PyErr_Print();
	PyErr_Clear();
	return G_TYPE_INVALID;
    }
}

static GtkTreePath *
pygtk_tree_model_get_path(GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    PyObject *self, *py_ret;

    g_return_val_if_fail(tree_model != NULL, NULL);
    g_return_val_if_fail(PYGTK_IS_TREE_MODEL(tree_model), NULL);
    g_return_val_if_fail(iter != NULL, NULL);
    /* this call finds the wrapper for this GObject */
    self = pygobject_new((GObject *)tree_model);

    py_ret = PyObject_CallMethod(self, METHOD_PREFIX "get_column_type",
				 "O", (PyObject *)iter->tree_node1);
    if (py_ret) {
	GtkTreePath *path = pygtk_tree_path_from_pyobject(py_ret);

	Py_DECREF(Py_None);
	return path;
    } else {
	PyErr_Print();
	PyErr_Clear();
	return NULL;
    }
}

static void
pygtk_tree_model_get_value(GtkTreeModel*tree_model, GtkTreeIter *iter,
			   gint column, GValue *value)
{
    PyObject *self, *py_value;

    g_return_if_fail(tree_model != NULL);
    g_return_if_fail(PYGTK_IS_TREE_MODEL(tree_model));
    g_return_if_fail(iter != NULL);

    /* this call finds the wrapper for this GObject */
    self = pygobject_new((GObject *)tree_model);

    /* init value to column type */
    g_value_init(value, pygtk_tree_model_get_column_type(tree_model, column));

    py_value = PyObject_CallMethod(self, METHOD_PREFIX "get_value",
				   "Oi", (PyObject *)iter->tree_node1, column);

    if (py_value) {
	pyg_value_from_pyobject(value, py_value);
	Py_DECREF(py_value);
    } else {
	PyErr_Print();
	PyErr_Clear();
    }
}

static gboolean
pygtk_tree_model_iter_next(GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    PyObject *self, *py_ret;

    g_return_val_if_fail(tree_model != NULL, FALSE);
    g_return_val_if_fail(PYGTK_IS_TREE_MODEL(tree_model), FALSE);
    g_return_val_if_fail(iter != NULL, FALSE);
    /* this call finds the wrapper for this GObject */
    self = pygobject_new((GObject *)tree_model);

    py_ret = PyObject_CallMethod(self, METHOD_PREFIX "iter_next",
				 "O", (PyObject *)iter->tree_node1);
    if (py_ret) {
	if (py_ret != Py_None) {
	    /* XXXX handle reference counting here */
	    iter->tree_node1 = py_ret;
	    return TRUE;
	} else {
	    Py_DECREF(py_ret);
	    return FALSE;
	}
    } else {
	PyErr_Print();
	PyErr_Clear();
	return FALSE;
    }
}

static gboolean
pygtk_tree_model_iter_children(GtkTreeModel *tree_model, GtkTreeIter *iter,
			       GtkTreeIter *parent)
{
    PyObject *self, *py_ret, *py_parent = Py_None;

    g_return_val_if_fail(tree_model != NULL, FALSE);
    g_return_val_if_fail(PYGTK_IS_TREE_MODEL(tree_model), FALSE);
    g_return_val_if_fail(iter != NULL, FALSE);
    /* this call finds the wrapper for this GObject */
    self = pygobject_new((GObject *)tree_model);

    if (parent) py_parent = (PyObject *)parent->tree_node1;
    py_ret = PyObject_CallMethod(self, METHOD_PREFIX "iter_children",
				 "O", py_parent);
    if (py_ret) {
	if (py_ret != Py_None) {
	    /* XXXX handle reference counting here */
	    iter->tree_node1 = py_ret;
	    return TRUE;
	} else {
	    Py_DECREF(py_ret);
	    return FALSE;
	}
    } else {
	PyErr_Print();
	PyErr_Clear();
	return FALSE;
    }
}

static gboolean
pygtk_tree_model_iter_has_child(GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    PyObject *self, *py_ret;

    g_return_val_if_fail(tree_model != NULL, FALSE);
    g_return_val_if_fail(PYGTK_IS_TREE_MODEL(tree_model), FALSE);
    g_return_val_if_fail(iter != NULL, FALSE);
    /* this call finds the wrapper for this GObject */
    self = pygobject_new((GObject *)tree_model);

    py_ret = PyObject_CallMethod(self, METHOD_PREFIX "iter_has_child",
				 "O", (PyObject *)iter->tree_node1);
    if (py_ret) {
	gboolean ret = PyObject_IsTrue(py_ret);

	Py_DECREF(py_ret);
	return ret;
    } else {
	PyErr_Print();
	PyErr_Clear();
	return FALSE;
    }
}

static gint
pygtk_tree_model_iter_n_children(GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    PyObject *self, *py_ret;

    g_return_val_if_fail(tree_model != NULL, FALSE);
    g_return_val_if_fail(PYGTK_IS_TREE_MODEL(tree_model), FALSE);
    g_return_val_if_fail(iter != NULL, FALSE);
    /* this call finds the wrapper for this GObject */
    self = pygobject_new((GObject *)tree_model);

    py_ret = PyObject_CallMethod(self, METHOD_PREFIX "iter_n_children",
				 "O", (PyObject *)iter->tree_node1);
    if (py_ret) {
	gint ret = PyInt_AsLong(py_ret);

	Py_DECREF(py_ret);
	return ret;
    } else {
	PyErr_Print();
	PyErr_Clear();
	return 0;
    }
}

static gboolean
pygtk_tree_model_iter_nth_child(GtkTreeModel *tree_model, GtkTreeIter  *iter,
				GtkTreeIter  *parent, gint n)
{
    PyObject *self, *py_ret, *py_parent = Py_None;

    g_return_val_if_fail(tree_model != NULL, FALSE);
    g_return_val_if_fail(PYGTK_IS_TREE_MODEL(tree_model), FALSE);
    g_return_val_if_fail(iter != NULL, FALSE);
    /* this call finds the wrapper for this GObject */
    self = pygobject_new((GObject *)tree_model);

    if (parent) py_parent = (PyObject *)parent->tree_node1;
    py_ret = PyObject_CallMethod(self, METHOD_PREFIX "iter_nth_child",
				 "Oi", py_parent, n);
    if (py_ret) {
	if (py_ret != Py_None) {
	    /* XXXX handle reference counting here */
	    iter->tree_node1 = py_ret;
	    return TRUE;
	} else {
	    Py_DECREF(py_ret);
	    return FALSE;
	}
    } else {
	PyErr_Print();
	PyErr_Clear();
	return FALSE;
    }
}

static gboolean
pygtk_tree_model_iter_parent(GtkTreeModel *tree_model, GtkTreeIter *iter,
			     GtkTreeIter *child)
{
    PyObject *self, *py_ret, *py_child = Py_None;

    g_return_val_if_fail(tree_model != NULL, FALSE);
    g_return_val_if_fail(PYGTK_IS_TREE_MODEL(tree_model), FALSE);
    g_return_val_if_fail(iter != NULL, FALSE);
    /* this call finds the wrapper for this GObject */
    self = pygobject_new((GObject *)tree_model);

    if (child) py_child = (PyObject *)child->tree_node1;
    py_ret = PyObject_CallMethod(self, METHOD_PREFIX "iter_parent",
				 "O", py_child);
    if (py_ret) {
	if (py_ret != Py_None) {
	    /* XXXX handle reference counting here */
	    iter->tree_node1 = py_ret;
	    return TRUE;
	} else {
	    Py_DECREF(py_ret);
	    return FALSE;
	}
    } else {
	PyErr_Print();
	PyErr_Clear();
	return FALSE;
    }
}

static void
pygtk_tree_model_ref_iter(GtkTreeModel *tree_model, GtkTreeIter *iter)
{
}

static void
pygtk_tree_model_unref_iter(GtkTreeModel *tree_model, GtkTreeIter *iter)
{
}

