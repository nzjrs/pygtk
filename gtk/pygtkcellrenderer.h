/* -*- Mode: C; c-basic-offset: 4 -*- */
#include <gtk/gtk.h>

#define PYGTK_TYPE_GENERIC_CELL_RENDERER (pygtk_generic_cell_renderer_get_type())
#define PYGTK_GENERIC_CELL_RENDERER(object) (G_TYPE_CHECK_INSTANCE_CAST((object), PYGTK_TYPE_GENERIC_CELL_RENDERER, PyGtkGenericCellRenderer))
#define PYGTK_GENERIC_CELL_RENDERER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), PYGTK_TYPE_GENERIC_CELL_RENDERER, PyGtkGenericCellRendererClass))
#define PYGTK_IS_GENERIC_CELL_RENDERER(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), PYGTK_TYPE_GENERIC_CELL_RENDERER))
#define PYGTK_IS_GENERIC_CELL_RENDERER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), PYGTK_TYPE_GENERIC_CELL_RENDERER))
#define PYGTK_GENERIC_CELL_RENDERER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), PYGTK_TYPE_GENERIC_CELL_RENDERER, PyGtkGenericCellRendererClass))

typedef struct _PyGtkGenericCellRenderer PyGtkGenericCellRenderer;
typedef struct _PyGtkGenericCellRendererClass PyGtkGenericCellRendererClass;

struct _PyGtkGenericCellRenderer {
    GtkCellRenderer parent_instance;
};

struct _PyGtkGenericCellRendererClass {
    GtkCellRendererClass parent_class;

};

GType            pygtk_generic_cell_renderer_get_type (void);
GtkCellRenderer *pygtk_generic_cell_renderer_new      (void);
