#include "handler-dbus.h"

GDBusObjectManagerServer *object_manager_server = NULL;
gbjManager *manager_interface = NULL;

int main(int argc, char **argv) {
    GMainLoop *loop;
    loop = g_main_loop_new(NULL, FALSE);

    g_bus_own_name(G_BUS_TYPE_SESSION,
                   "com.github.borgoat.Jabra1",
                   G_BUS_NAME_OWNER_FLAGS_NONE,
                   NULL,
                   dbus_on_name_acquired,
                   NULL,
                   NULL,
                   NULL);

    g_main_loop_run(loop);

    return 0;
}
