#include "handler-jabra.h"

#include <stdio.h>

static gboolean on_handle_get_firmware_version(gbjDevice *d, GDBusMethodInvocation *invocation, gpointer user_data) {
    gchar *firmware_version = g_new(gchar, MAXLEN + 1);
    Jabra_GetFirmwareVersion(gbj_device_get_device_id(d), firmware_version, MAXLEN);
    gbj_device_complete_get_firmware_version(d, invocation, firmware_version);
    g_free(firmware_version);
    return TRUE;
}

static void configure_device_with_device_info(gbjDevice *d, const Jabra_DeviceInfo *info) {
    gbj_device_set_device_id(d, info->deviceID);
    gbj_device_set_product_id(d, info->productID);
    gbj_device_set_vendor_id(d, info->vendorID);
    gbj_device_set_device_name(d, info->deviceName);
    gbj_device_set_usb_device_path(d, info->usbDevicePath);
    gbj_device_set_is_dongle(d, info->isDongle);

    g_signal_connect(d,
                     "handle-get-firmware-version",
                     G_CALLBACK(on_handle_get_firmware_version),
                     NULL);
}

static gchar *device_object_path(unsigned short deviceID) {
    gchar *buf = NULL;
    buf = g_new(gchar, MAXLEN + 1);
    g_snprintf(buf, MAXLEN, "%s/%hu", OBJPATH_MANAGER, deviceID);
    return buf;
}

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
