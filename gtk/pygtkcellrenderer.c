/* -*- Mode: C; c-basic-offset: 4 -*- */
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "pygtkcellrenderer.h"
#include <Python.h>
#include "pygobject.h"
#include "pygtk-private.h"

/* define this to print out debug messages */
#undef DEBUG_CELL_RENDERER

#ifndef _
#  define _(s) (s)
#endif

static void            pygtk_generic_cell_renderer_class_init    (PyGtkGenericCellRendererClass *klass);
static void            pygtk_generic_cell_renderer_get_size      (GtkCellRenderer               *cell,
								  GtkWidget                     *widget,
								  GdkRectangle                  *cell_area,
								  gint                          *x_offset,
								  gint                          *y_offset,
								  gint                          *width,
								  gint                          *height);
static void            pygtk_generic_cell_renderer_render        (GtkCellRenderer               *cell,
								  GdkWindow                     *window,
								  GtkWidget                     *widget,
								  GdkRectangle                  *background_area,
								  GdkRectangle                  *cell_area,
								  GdkRectangle                  *expose_area,
								  GtkCellRendererState           flags);
static gboolean        pygtk_generic_cell_renderer_activate      (GtkCellRenderer               *cell,
								  GdkEvent                      *event,
								  GtkWidget                     *widget,
								  const gchar                   *path,
								  GdkRectangle                  *background_area,
								  GdkRectangle                  *cell_area,
								  GtkCellRendererState           flags);
static GtkCellEditable *pygtk_generic_cell_renderer_start_editing (GtkCellRenderer               *cell,
								  GdkEvent                      *event,
								  GtkWidget                     *widget,
								  const gchar                   *path,
								  GdkRectangle                  *background_area,
								  GdkRectangle                  *cell_area,
								  GtkCellRendererState           flags);



GType
pygtk_generic_cell_renderer_get_type(void)
{
    static GType object_type = 0;

    if (!object_type) {
	static const GTypeInfo object_info = {
	    sizeof(PyGtkGenericCellRendererClass),
	    (GBaseInitFunc) NULL,
	    (GBaseFinalizeFunc) NULL,
	    (GClassInitFunc) pygtk_generic_cell_renderer_class_init,
	    NULL, /* class_finalize */
	    NULL, /* class_data */
	    sizeof(PyGtkGenericCellRenderer),
	    0, /* n_preallocs */
	    (GInstanceInitFunc) NULL,
	};
	object_type = g_type_register_static(GTK_TYPE_CELL_RENDERER,
					     "PyGtkGenericCellRenderer",
					     &object_info, 0);
    }

    return object_type;
}

static void
pygtk_generic_cell_renderer_class_init(PyGtkGenericCellRendererClass *klass)
{
    GtkCellRendererClass *cell_renderer_class = (GtkCellRendererClass*) klass;
    cell_renderer_class->get_size = pygtk_generic_cell_renderer_get_size;
    cell_renderer_class->render = pygtk_generic_cell_renderer_render;
    cell_renderer_class->activate = pygtk_generic_cell_renderer_activate;
    cell_renderer_class->start_editing = pygtk_generic_cell_renderer_start_editing;
}

#define METHOD_PREFIX "on_"

static void
pygtk_generic_cell_renderer_get_size (GtkCellRenderer *cell,
				      GtkWidget       *widget,
				      GdkRectangle    *cell_area,
				      gint            *x_offset,
				      gint            *y_offset,
				      gint            *width,
				      gint            *height)
{
    PyObject *self, *py_ret, *py_cell_area;

    g_return_if_fail(PYGTK_IS_GENERIC_CELL_RENDERER (cell));

    self = pygobject_new((GObject *)cell);

#ifdef DEBUG_CELL_RENDERER
    g_message ("get_size()");
#endif
    py_ret = PyObject_CallMethod(self, METHOD_PREFIX "get_size", "N",
				 pygobject_new ((GObject *)widget));

    if (!py_ret) {
	PyErr_Print();
	PyErr_Clear();
	return;
    }

    if (!PyArg_ParseTuple(py_ret, "Oiiii", &py_cell_area, x_offset, y_offset,
			  width, height)) {
	PyErr_Clear();
	Py_DECREF(py_ret);
	g_warning("could not parse return value of get_size() method.  "
		  "Should be of form(cell_area, x_offset, y_offset, "
		  "width, height)");
	return;
    }
    if (!pygdk_rectangle_from_pyobject(py_cell_area, cell_area)) {
	PyErr_Print();
	PyErr_Clear();
	return;
    }
    /* success */
}

static void
pygtk_generic_cell_renderer_render (GtkCellRenderer      *cell,
				    GdkWindow            *window,
				    GtkWidget            *widget,
				    GdkRectangle         *background_area,
				    GdkRectangle         *cell_area,
				    GdkRectangle         *expose_area,
				    GtkCellRendererState  flags)
{
    PyObject *self, *py_ret;

    g_return_if_fail(PYGTK_IS_GENERIC_CELL_RENDERER (cell));

    self = pygobject_new((GObject *)cell);

#ifdef DEBUG_CELL_RENDERER
    g_message ("render()");
#endif
    py_ret = PyObject_CallMethod(self, METHOD_PREFIX "render", "NNNNNi",
		pygobject_new((GObject *)window),
		pygobject_new((GObject *)widget),
		pyg_boxed_new(GDK_TYPE_RECTANGLE, background_area, TRUE, TRUE),
		pyg_boxed_new(GDK_TYPE_RECTANGLE, cell_area, TRUE, TRUE),
		pyg_boxed_new(GDK_TYPE_RECTANGLE, expose_area, TRUE, TRUE),
		flags);
    if (!py_ret) {
	PyErr_Print();
	PyErr_Clear();
	return;
    }
    /* success */
}

static gboolean
pygtk_generic_cell_renderer_activate (GtkCellRenderer      *cell,
				      GdkEvent             *event,
				      GtkWidget            *widget,
				      const gchar          *path,
				      GdkRectangle         *background_area,
				      GdkRectangle         *cell_area,
				      GtkCellRendererState  flags)
{
    PyObject *self, *py_ret;
    gboolean ret;

    g_return_val_if_fail(PYGTK_IS_GENERIC_CELL_RENDERER (cell), FALSE);

    self = pygobject_new((GObject *)cell);

#ifdef DEBUG_CELL_RENDERER
    g_message ("activate()");
#endif
    py_ret = PyObject_CallMethod(self, METHOD_PREFIX "activate", "NNzNNNi",
		pyg_boxed_new(GDK_TYPE_EVENT, event, FALSE, FALSE),
		pygobject_new((GObject *)widget),
		path,
		pyg_boxed_new(GDK_TYPE_RECTANGLE, background_area, TRUE, TRUE),
		pyg_boxed_new(GDK_TYPE_RECTANGLE, cell_area, TRUE, TRUE),
		flags);
    if (!py_ret) {
	PyErr_Print();
	PyErr_Clear();
	return FALSE;
    }
    ret = PyObject_IsTrue(py_ret);
    Py_DECREF(py_ret);
    return ret;
}

static GtkCellEditable *
pygtk_generic_cell_renderer_start_editing (GtkCellRenderer      *cell,
					   GdkEvent             *event,
					   GtkWidget            *widget,
					   const gchar          *path,
					   GdkRectangle         *background_area,
					   GdkRectangle         *cell_area,
					   GtkCellRendererState  flags)
{
    PyObject *self, *py_ret;
    GtkCellEditable *ret = NULL;
    extern PyTypeObject PyGtkCellEditable_Type;

    g_return_val_if_fail(PYGTK_IS_GENERIC_CELL_RENDERER (cell), NULL);

    self = pygobject_new((GObject *)cell);

#ifdef DEBUG_CELL_RENDERER
    g_message ("start_editing()");
#endif
    py_ret = PyObject_CallMethod(self, METHOD_PREFIX"start_editing", "NNzNNNi",
		pyg_boxed_new(GDK_TYPE_EVENT, event, FALSE, FALSE),
		pygobject_new((GObject *)widget),
		path,
		pyg_boxed_new(GDK_TYPE_RECTANGLE, background_area, TRUE, TRUE),
		pyg_boxed_new(GDK_TYPE_RECTANGLE, cell_area, TRUE, TRUE),
		flags);
    if (!py_ret) {
	PyErr_Print();
	PyErr_Clear();
	return FALSE;
    }
    if (pygobject_check(py_ret, &PyGtkCellEditable_Type)) {
	ret = GTK_CELL_EDITABLE(g_object_ref(pygobject_get(py_ret)));
    } else {
	g_warning("return of start_editing() was not a GtkCellEditable");
    }
    Py_DECREF(py_ret);
    return ret;
}

GtkCellRenderer *
pygtk_generic_cell_renderer_new(void)
{
    return GTK_CELL_RENDERER(
        g_object_new(PYGTK_TYPE_GENERIC_CELL_RENDERER, NULL));
}
