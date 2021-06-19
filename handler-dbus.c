#include "handler-dbus.h"

#include <stdio.h>
#include "handler-jabra.h"

void dbus_on_name_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data) {
    object_manager_server = g_dbus_object_manager_server_new(OBJPATH_MANAGER);
    g_dbus_object_manager_server_set_connection(object_manager_server, connection);

    manager_interface = gbj_manager_skeleton_new();
    GError *err = NULL;
    if (!g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(manager_interface),
                                          connection,
                                          OBJPATH_MANAGER,
                                          &err)) {
        fprintf(stderr, "Could not export D-Bus interface: %s\n", err->message);
        g_error_free(err);
    }

    Jabra_SetAppID("com.github.borgoat.Jabra");
    bool init = Jabra_InitializeV2(jabra_on_first_scan_for_devices_done,
                                   jabra_on_device_attached,
                                   jabra_on_device_removed,
                                   NULL,
                                   jabra_on_button_in_data_translated,
                                   false,
                                   NULL);

    if (!init) {
        fprintf(stderr, "Could not initialize Jabra SDK\n");
    }
}
