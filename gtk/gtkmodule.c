/* -*- Mode: C; c-basic-offset: 4 -*- */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
/* include this first, before NO_IMPORT_PYGOBJECT is defined */
#include <pygobject.h>
#include "pygtk-private.h"

void _pygtk_register_boxed_types(PyObject *moddict);
void pygtk_register_classes(PyObject *d);
void pygdk_register_classes(PyObject *d);

extern PyMethodDef pygtk_functions[];
extern PyMethodDef pygdk_functions[];

static struct _PyGtk_FunctionStruct functions = {
    VERSION,
    FALSE,

    pygtk_block_threads,
    pygtk_unblock_threads,

    pygtk_destroy_notify,

    &PyGdkAtom_Type,  PyGdkAtom_New,
    &PyGtkCTreeNode_Type,  PyGtkCTreeNode_New,
};

DL_EXPORT(void)
init_gtk(void)
{
    PyObject *m, *d;
    PyObject *av;
    int argc, i;
    char **argv;

    /* initialise gobject */
    init_pygobject();

    /* initialise gthread if appropriate ... */
#ifdef ENABLE_PYGTK_THREADING
    /* it is required that this function be called to enable the thread
     * safety functions */
    g_thread_init(NULL);
#endif

    /* set the default python encoding to utf-8 */
    PyUnicode_SetDefaultEncoding("utf-8");

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

    /* now initialise pygtk */
    m = Py_InitModule("gtk._gtk", pygtk_functions);
    d = PyModule_GetDict(m);

    _pygtk_register_boxed_types(d);
    pygtk_register_classes(d);

    /* for addon libraries ... */
    PyDict_SetItemString(d, "_PyGtk_API",
			 PyCObject_FromVoidPtr(&functions, NULL));

    /* namespace all the gdk stuff in gtk.gdk ... */
    m = Py_InitModule("gtk.gdk", pygdk_functions);
    d = PyModule_GetDict(m);

    pygdk_register_classes(d);

    if (PyErr_Occurred())
	Py_FatalError("can't initialise module _gtk");

}
