#include <stdio.h>
#include <gio/gio.h>
#include <jabra/Common.h>

#include "jabra-generated.h"

#define OBJPATH_ROOT     "/com/github/borgoat/Jabra1/"
#define OBJPATH(OBJECT)  OBJPATH_ROOT #OBJECT
#define OBJPATH_MANAGER  OBJPATH(Manager)

static gbjManager *mgrInterface;
static GDBusObjectManagerServer *objectManager;


static void first_scan_cb() {
    printf ("first scan done!\n");

    Jabra_DeviceInfo *devices = NULL;
    int count = 0;

    Jabra_GetAttachedJabraDevices (&count, devices);

    printf ("found %d devices\n", count);
}

static gchar *device_object_path(unsigned short deviceID) {
    gchar *buf = NULL;
    buf = g_new (gchar, 256);
    g_snprintf(buf, 255, "%s/%hu", OBJPATH_MANAGER, deviceID);
    return buf;
}

static void configure_device_with_device_info(gbjDevice *d, const Jabra_DeviceInfo *info) {
    gbj_device_set_device_id        (d, info->deviceID);
    gbj_device_set_product_id       (d, info->productID);
    gbj_device_set_vendor_id        (d, info->vendorID);
    gbj_device_set_device_name      (d, info->deviceName);
    gbj_device_set_usb_device_path  (d, info->usbDevicePath);
    gbj_device_set_is_dongle        (d, info->isDongle);
}

static void device_attached_cb(Jabra_DeviceInfo deviceInfo) {
    printf ("attached: %s\n", deviceInfo.deviceName);

    gbjDevice *device = gbj_device_skeleton_new();
    configure_device_with_device_info(device, &deviceInfo);

    gchar *path = device_object_path(deviceInfo.deviceID);
    GDBusObjectSkeleton *skeleton = g_dbus_object_skeleton_new (path);

    g_dbus_object_skeleton_add_interface (G_DBUS_OBJECT_SKELETON (skeleton), G_DBUS_INTERFACE_SKELETON (device));
    g_dbus_object_manager_server_export (G_DBUS_OBJECT_MANAGER_SERVER (objectManager), skeleton);
    gbj_manager_emit_device_attached (GBJ_MANAGER (mgrInterface), deviceInfo.deviceName);

    g_free (path);
    Jabra_FreeDeviceInfo (deviceInfo);
}

static void device_removed_cb(unsigned short deviceID) {
    gchar *path = device_object_path (deviceID);
    g_dbus_object_manager_server_unexport (G_DBUS_OBJECT_MANAGER_SERVER (objectManager), path);
    gbj_manager_emit_device_removed (GBJ_MANAGER (mgrInterface), deviceID);

    g_free (path);
}

static void on_name_acquired (GDBusConnection *connection,
                              const gchar *name,
                              gpointer user_data)
{
    objectManager = g_dbus_object_manager_server_new (OBJPATH_MANAGER);
    g_dbus_object_manager_server_set_connection (objectManager, connection);

    mgrInterface = gbj_manager_skeleton_new ();
    GError *err = NULL;
    if (!g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (mgrInterface),
                                           connection,
                                           OBJPATH_MANAGER,
                                           &err))
    {
        fprintf (stderr, "Could not export D-Bus interface: %s\n", err->message);
        g_error_free (err);
    }

    Jabra_SetAppID ("com.github.borgoat.Jabra");
    bool init = Jabra_InitializeV2 (first_scan_cb,
                                    device_attached_cb,
                                    device_removed_cb,
                                    NULL,
                                    NULL,
                                    false,
                                    NULL);

    if (!init) {
        fprintf (stderr, "Could not initialize Jabra SDK\n");
    }
}

int main (int argc, char **argv)
{
    GMainLoop *loop;
    loop = g_main_loop_new (NULL, FALSE);

    g_bus_own_name (G_BUS_TYPE_SESSION,
                    "com.github.borgoat.Jabra1",
                    G_BUS_NAME_OWNER_FLAGS_NONE,
                    NULL,
                    on_name_acquired,
                    NULL,
                    NULL,
                    NULL);

    g_main_loop_run (loop);

    return 0;
}
