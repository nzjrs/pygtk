/* -*- Mode: C; c-basic-offset: 4 -*- */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "pygtk-private.h"

void _pygtk_register_boxed_types(PyObject *moddict);
void pygtk_register_classes(PyObject *d);

extern PyMethodDef pygtk_functions[];

extern PyExtensionClass PyGtkObject_Type;
extern GHashTable *_pygtk_boxed_funcs;

static struct _PyGtk_FunctionStruct functions = {
    VERSION,
    FALSE,

    pygtk_block_threads,
    pygtk_unblock_threads,

    pygtk_destroy_notify,
    pygtk_callback_marshal,

    pygtk_args_as_tuple,
    pygtk_args_from_sequence,
    pygtk_arg_from_pyobject,
    pygtk_arg_as_pyobject,
    pygtk_ret_from_pyobject,
    pygtk_ret_as_pyobject,
    pygtk_dict_as_args,

    pygtk_register_boxed,

    pygtk_enum_get_value,
    pygtk_flag_get_value,

    pygtk_register_class,
    pygtk_register_wrapper,
    pygtk_no_constructor,

    &PyGtkObject_Type, PyGtk_New,
    &PyGtkAccelGroup_Type,  PyGtkAccelGroup_New,
    &PyGtkStyle_Type,  PyGtkStyle_New,
    &PyGdkFont_Type,  PyGdkFont_New,
    &PyGdkColor_Type,  PyGdkColor_New,
    &PyGdkEvent_Type,  PyGdkEvent_New,
    &PyGdkWindow_Type,  PyGdkWindow_New,
    &PyGdkGC_Type,  PyGdkGC_New,
    &PyGdkVisual_Type, PyGdkVisual_New,
    &PyGdkColormap_Type,  PyGdkColormap_New,
    &PyGdkDragContext_Type,  PyGdkDragContext_New,
    &PyGtkSelectionData_Type,  PyGtkSelectionData_New,
    &PyGdkAtom_Type,  PyGdkAtom_New,
    &PyGdkCursor_Type,  PyGdkCursor_New,
    &PyGtkCTreeNode_Type,  PyGtkCTreeNode_New
};

DL_EXPORT(void)
init_gtk(void)
{
    PyObject *m, *d, *private;
    PyObject *av;
    int argc, i;
    char **argv;

    /* initialise gthread if appropriate ... */
#ifdef WITH_THREAD
    /* it is required that this function be called to enable the thread
     * safety functions */
    g_thread_init(NULL);
#endif

    /* initialise GTK ... */
    av = PySys_GetObject("argv");
    argc = PyList_Size(av);
    argv = g_new(char *, argc);
    for (i = 0; i < argc; i++)
	argv[i] = g_strdup(PyString_AsString(PyList_GetItem(av, i)));
    if (!gtk_init_check(&argc, &argv)) {
	if (argv != NULL) {
	    for (i = 0; i < argc; i++)
		g_free(argv[i]);
	    g_free(argv);
	}
	Py_FatalError("could not open display");
	return;
    }
    PySys_SetArgv(argc, argv);
    if (argv != NULL) {
	for (i = 0; i < argc; i++)
	    g_free(argv[i]);
	g_free(argv);
    }
    gtk_signal_set_funcs((GtkSignalMarshal)pygtk_signal_marshal,
			 (GtkSignalDestroy)pygtk_destroy_notify);

    /* now initialise pygtk */
    m = Py_InitModule("_gtk", pygtk_functions);
    d = PyModule_GetDict(m);

    _pygtk_boxed_funcs = g_hash_table_new(g_direct_hash, g_direct_equal);
    _pygtk_register_boxed_types(d);
    pygtk_register_classes(d);

    /* for addon libraries ... */
    PyDict_SetItemString(d, "_PyGtk_API",
			 PyCObject_FromVoidPtr(&functions, NULL));

    if (PyErr_Occurred())
	Py_FatalError("can't initialise module _gtk");

}
