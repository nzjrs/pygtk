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
void pygtk_add_constants(PyObject *module, const gchar *strip_prefix);
void pygdk_add_constants(PyObject *module, const gchar *strip_prefix);

extern PyMethodDef pygtk_functions[];
extern PyMethodDef pygdk_functions[];

static struct _PyGtk_FunctionStruct functions = {
    VERSION,
    FALSE,

    pygtk_block_threads,
    pygtk_unblock_threads,

    pygtk_destroy_notify,

    &PyGdkAtom_Type,  PyGdkAtom_New,
};

static void
pygtk_main_quit(void)
{
    if (gtk_main_level())
	gtk_main_quit();
} 

DL_EXPORT(void)
init_gtk(void)
{
    PyObject *m, *d;
    PyObject *av;
    int argc, i;
    char **argv;
    GSList *stock_ids, *cur;
    char buf[128];
 
    /* initialise gobject */
    init_pygobject();

    /* initialise gthread if appropriate ... */
#ifdef ENABLE_PYGTK_THREADING
    /* it is required that this function be called to enable the thread
     * safety functions */
    g_thread_init(NULL);
#endif

    m = PyImport_ImportModule("os");
    if (m == NULL) {
	Py_FatalError("couldn't import os");
	return;
    }
    d = PyModule_GetDict(m);
    Py_DECREF(m);
    d = PyDict_GetItemString(d, "environ");
    d = PyMapping_GetItemString(d, "PYGTK_FATAL_EXCEPTIONS");
    if (d == NULL)
	PyErr_Clear();
    else {
	PyGtk_FatalExceptions = TRUE;
	pyg_fatal_exceptions_notify_add(pygtk_main_quit);
	Py_DECREF(d);
    }

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
    pygtk_add_constants(m, "GTK_");

    /* for addon libraries ... */
    PyDict_SetItemString(d, "_PyGtk_API",
			 PyCObject_FromVoidPtr(&functions, NULL));

    stock_ids = gtk_stock_list_ids();
    strcpy(buf, "STOCK_");
    for (cur = stock_ids; cur; cur = stock_ids) {
	char *ctmp;
	PyObject *obj;
	int i;
	
	ctmp = cur->data;
	if(strncmp(ctmp, "gtk-", 4))
	    continue;
	ctmp += 4;
	
	strcpy(buf + sizeof("STOCK"), ctmp);
	ctmp = buf + sizeof("STOCK");
	for (i = 0; ctmp[i]; i++) {
		if (ctmp[i] == '-')
		    ctmp[i] = '_';
		else if (ctmp[i] >= 'a' && ctmp[i] <= 'z')
		    ctmp[i] -= 'a'-'A';
	}
	
	obj = PyString_FromString(cur->data);
	PyDict_SetItemString(d, buf, obj);
	Py_DECREF(obj);
	g_free(cur->data);
	stock_ids = cur->next;
	g_slist_free_1(cur);
    }
    /* namespace all the gdk stuff in gtk.gdk ... */
    m = Py_InitModule("gtk.gdk", pygdk_functions);
    d = PyModule_GetDict(m);

    pygdk_register_classes(d);
    pygdk_add_constants(m, "GDK_");

    if (PyErr_Occurred())
	Py_FatalError("can't initialise module _gtk");

}
