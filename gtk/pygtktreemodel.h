/* -*- Mode: C; c-basic-offset: 4 -*- */
#include <gtk/gtk.h>

#define PYGTK_TYPE_TREE_MODEL (pygtk_tree_model_get_type())
#define PYGTK_TREE_MODEL(object) (G_TYPE_CHECK_INSTANCE_CAST((object), PYGTK_TYPE_TREE_MODEL, PyGtkTreeModel))
#define PYGTK_TREE_MODEL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), PYGTK_TYPE_TREE_MODEL, PyGtkTreeModelClass))
#define PYGTK_IS_TREE_MODEL(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), PYGTK_TYPE_TREE_MODEL))
#define PYGTK_IS_TREE_MODEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), PYGTK_TYPE_TREE_MODEL))
#define PYGTK_TREE_MODEL_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), PYGTK_TYPE_TREE_MODEL, PyGtkTreeModelClass))

typedef struct _PyGtkTreeModel PyGtkTreeModel;
typedef struct _PyGtkTreeModelClass PyGtkTreeModelClass;

struct _PyGtkTreeModel {
    GObject parent_instance;
};

struct _PyGtkTreeModelClass {
    GObjectClass parent_class;

};

GType pygtk_tree_model_get_type(void);

PyGtkTreeModel *pygtk_tree_model_new(void);
