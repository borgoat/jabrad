#include "handler-jabra.h"

#include <stdio.h>
#include "utils.h"

void jabra_on_first_scan_for_devices_done() {
    printf("first scan done!\n");

    Jabra_DeviceInfo *devices = NULL;
    int count = 0;

    Jabra_GetAttachedJabraDevices(&count, devices);

    printf("found %d devices\n", count);
}

void jabra_on_device_attached(Jabra_DeviceInfo deviceInfo) {
    printf("attached: %s\n", deviceInfo.deviceName);

    gbjDevice *device = gbj_device_skeleton_new();
    configure_device_with_device_info(device, &deviceInfo);

    gchar *path = device_object_path(deviceInfo.deviceID);
    GDBusObjectSkeleton *skeleton = g_dbus_object_skeleton_new(path);

    g_dbus_object_skeleton_add_interface(G_DBUS_OBJECT_SKELETON(skeleton), G_DBUS_INTERFACE_SKELETON(device));
    g_dbus_object_manager_server_export(G_DBUS_OBJECT_MANAGER_SERVER(object_manager_server), skeleton);

    g_free(path);
    Jabra_FreeDeviceInfo(deviceInfo);
}

void jabra_on_device_removed(unsigned short deviceID) {
    gchar *path = device_object_path(deviceID);
    g_dbus_object_manager_server_unexport(G_DBUS_OBJECT_MANAGER_SERVER(object_manager_server), path);

    g_free(path);
}

void jabra_on_button_in_data_translated(unsigned short deviceID, Jabra_HidInput translatedInData, bool buttonInData) {
    gchar *path = device_object_path(deviceID);

    GDBusInterface *device_interface = g_dbus_object_manager_get_interface(G_DBUS_OBJECT_MANAGER(object_manager_server),
                                                                           path,
                                                                           "com.github.borgoat.Jabra1.Device");
    gbjDevice *device = GBJ_DEVICE(device_interface);

    gbj_device_emit_button_in_data_translated(device, translatedInData, buttonInData);

    g_object_unref(device_interface);
    g_free(path);
}
