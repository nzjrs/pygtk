/* -*- Mode: C; c-basic-offset: 4 -*- */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>

/* include this first, before NO_IMPORT_PYGOBJECT is defined */
#include <pygobject.h>
#include "pygtk-private.h"
#include <pyerrors.h>

void _pygtk_register_boxed_types(PyObject *moddict);
void pygtk_register_classes(PyObject *d);
void pygdk_register_classes(PyObject *d);
void pygtk_add_constants(PyObject *module, const gchar *strip_prefix);
void pygdk_add_constants(PyObject *module, const gchar *strip_prefix);

extern PyMethodDef pygtk_functions[];
extern PyMethodDef pygdk_functions[];

static struct _PyGtk_FunctionStruct functions = {
    VERSION,

    &PyGdkAtom_Type,  PyGdkAtom_New,
};

static void
sink_gtkobject(GObject *object)
{
    if (GTK_OBJECT_FLOATING(object)) {
	g_object_ref(object);
	gtk_object_sink(GTK_OBJECT(object));
    }
}

static gboolean
python_do_pending_calls(gpointer data)
{
    gboolean quit = FALSE;

    pyg_block_threads();
    if (PyErr_CheckSignals() == -1) {
	PyErr_SetNone(PyExc_KeyboardInterrupt);
	quit = TRUE;
    }
    pyg_unblock_threads();

    if (quit)
	gtk_main_quit();
    
    return TRUE;
}

DL_EXPORT(void)
init_gtk(void)
{
    PyObject *m, *d, *tuple, *o;
    PyObject *av;
    int argc, i;
    char **argv;
    GSList *stock_ids, *cur;
    char buf[128];
 
    /* initialise gobject */
    init_pygobject();
    g_assert(pygobject_register_class != NULL);

    pygobject_register_sinkfunc(GTK_TYPE_OBJECT, sink_gtkobject);

    /* set the default python encoding to utf-8 */
    PyUnicode_SetDefaultEncoding("utf-8");

    /* initialise GTK ... */
    av = PySys_GetObject("argv");
    if (av != NULL) {
	argc = PyList_Size(av);
	argv = g_new(char *, argc);
	for (i = 0; i < argc; i++)
	    argv[i] = g_strdup(PyString_AsString(PyList_GetItem(av, i)));
    } else {
	    argc = 0;
	    argv = NULL;
    }

    if (!gtk_init_check(&argc, &argv)) {
	if (argv != NULL) {
	    for (i = 0; i < argc; i++)
		g_free(argv[i]);
	    g_free(argv);
	}
	PyErr_SetString(PyExc_RuntimeError, "could not open display");
	/* set the LC_NUMERIC locale back to "C", as Python requires
	 * that it be set that way. */
	setlocale(LC_NUMERIC, "C");
	return;
    }
    /* set the LC_NUMERIC locale back to "C", as Python requires that
     * it be set that way. */
    setlocale(LC_NUMERIC, "C");
    if (argv != NULL) {
	PySys_SetArgv(argc, argv);
	for (i = 0; i < argc; i++)
	    g_free(argv[i]);
	g_free(argv);
    }

    /* now initialise pygtk */
    m = Py_InitModule("gtk._gtk", pygtk_functions);
    d = PyModule_GetDict(m);

    /* gtk+ version */
    tuple = Py_BuildValue ("(iii)", gtk_major_version, gtk_minor_version,
			   gtk_micro_version);
    PyDict_SetItemString(d, "gtk_version", tuple);    
    Py_DECREF(tuple);
	
    /* pygtk version */
    tuple = Py_BuildValue ("(iii)", PYGTK_MAJOR_VERSION, PYGTK_MINOR_VERSION,
			   PYGTK_MICRO_VERSION);
    PyDict_SetItemString(d, "pygtk_version", tuple);
    Py_DECREF(tuple);
	
    _pygtk_register_boxed_types(d);
    pygtk_register_classes(d);
    pygtk_add_constants(m, "GTK_");
    
    /* for addon libraries ... */
    PyDict_SetItemString(d, "_PyGtk_API",
			 o=PyCObject_FromVoidPtr(&functions, NULL));
    Py_DECREF(o);
	
    stock_ids = gtk_stock_list_ids();
    strcpy(buf, "STOCK_");
    for (cur = stock_ids; cur; cur = stock_ids) {
	char *ctmp;
	PyObject *obj;
	int i;
	
	ctmp = cur->data;
	if(strncmp(ctmp, "gtk-", 4)) {
		g_free(cur->data);
		stock_ids = cur->next;
		g_slist_free_1(cur);
		continue;
	}
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
    
    gtk_timeout_add(100, python_do_pending_calls, NULL);
}
