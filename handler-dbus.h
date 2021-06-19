#ifndef JABRAD_HANDLER_DBUS_H
#define JABRAD_HANDLER_DBUS_H

#include <gio/gio.h>
#include "dbus-gen.h"

/**
 * \brief Handler for g_bus_own_name
 */
void dbus_on_name_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data);

#endif // JABRAD_HANDLER_DBUS_H
