#include <gtk/gtksocket.h>
#include <gtk/gtkplug.h>

GtkType
gtk_plug_get_type ()
{
  static int n = 0;

  if (n > 0) /* first call causes no harm */
    g_warning ("GtkPlug not yet available on win32!");
  n++;

  return 0;
}

GtkType
gtk_socket_get_type ()
{
  static int n = 0;

  if (n > 0)
    g_warning ("GtkSocket not yet available on win32!");
  n++;

  return 0;
}