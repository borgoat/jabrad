#include "handler-jabra.h"

#include <stdio.h>

static gboolean on_handle_get_firmware_version(gbjDevice *d, GDBusMethodInvocation *invocation, gpointer user_data) {
    gchar *firmware_version = g_new(gchar, MAXLEN + 1);
    Jabra_GetFirmwareVersion(gbj_device_get_device_id(d), firmware_version, MAXLEN);
    gbj_device_complete_get_firmware_version(d, invocation, firmware_version);
    g_free(firmware_version);
    return TRUE;
}

static gboolean on_handle_set_btpairing(gbjDevice *d, GDBusMethodInvocation *invocation, gpointer user_data) {
    Jabra_SetBTPairing(gbj_device_get_device_id(d));
    gbj_device_complete_set_btpairing(d, invocation);
    return TRUE;
}

static gboolean on_handle_stop_btpairing(gbjDevice *d, GDBusMethodInvocation *invocation, gpointer user_data) {
    Jabra_StopBTPairing(gbj_device_get_device_id(d));
    gbj_device_complete_stop_btpairing(d, invocation);
    return TRUE;
}

static gboolean on_handle_get_search_device_list(gbjDevice *d, GDBusMethodInvocation *invocation, gpointer user_data) {
    const guint16 device_id = gbj_device_get_device_id(d);

    Jabra_PairingList *devices = Jabra_GetSearchDeviceList(device_id);

    GVariantBuilder devices_array_builder;
    g_variant_builder_init(&devices_array_builder, G_VARIANT_TYPE("a(sbay)"));

    for (int i = 0; i < devices->count; ++i) {
        GVariantBuilder device_builder;
        g_variant_builder_init(&device_builder, G_VARIANT_TYPE("(sbay)"));

        Jabra_PairedDevice device = devices->pairedDevice[i];
        g_variant_builder_add(&device_builder, "(sbay)", device.deviceName, device.isConnected, device.deviceBTAddr);

        g_variant_builder_add_value(&devices_array_builder, g_variant_builder_end(&device_builder));
    }

    GVariant *devices_array = g_variant_builder_end(&devices_array_builder);
    gbj_device_complete_get_search_device_list(d, invocation, devices_array);
    g_free(devices);
    return TRUE;
}

static void configure_device_with_device_info(gbjDevice *d, const Jabra_DeviceInfo *info) {
    gbj_device_set_device_id(d, info->deviceID);
    gbj_device_set_product_id(d, info->productID);
    gbj_device_set_vendor_id(d, info->vendorID);
    gbj_device_set_device_name(d, info->deviceName);
    gbj_device_set_usb_device_path(d, info->usbDevicePath);
    gbj_device_set_is_dongle(d, info->isDongle);

    // Firmware methods
    g_signal_connect(d,
                     "handle-get-firmware-version",
                     G_CALLBACK(on_handle_get_firmware_version),
                     NULL);

    // Bluetooth methods
    g_signal_connect(d,
                     "handle-set-btpairing",
                     G_CALLBACK(on_handle_set_btpairing),
                     NULL);

    g_signal_connect(d,
                     "handle-stop-btpairing",
                     G_CALLBACK(on_handle_stop_btpairing),
                     NULL);

    // Device search methods
    g_signal_connect(d,
                     "handle-get-search-device-list",
                     G_CALLBACK(on_handle_get_search_device_list),
                     NULL);
}

static GString *device_object_path(unsigned short deviceID) {
    GString *buf = g_string_new("");
    g_string_printf(buf, "%s/%hu", OBJPATH_MANAGER, deviceID);
    return buf;
}

void jabra_on_first_scan_for_devices_done() {
    g_debug("first scan done");

    Jabra_DeviceInfo *devices = NULL;
    int count = 0;

    Jabra_GetAttachedJabraDevices(&count, devices);

    printf("found %d devices\n", count);
}

void jabra_on_device_attached(Jabra_DeviceInfo deviceInfo) {
    g_info("device attached: %s (ID %u)", deviceInfo.deviceName, deviceInfo.deviceID);

    gbjDevice *device = gbj_device_skeleton_new();
    configure_device_with_device_info(device, &deviceInfo);

    GString *path = device_object_path(deviceInfo.deviceID);
    GDBusObjectSkeleton *skeleton = g_dbus_object_skeleton_new(path->str);

    g_dbus_object_skeleton_add_interface(G_DBUS_OBJECT_SKELETON(skeleton), G_DBUS_INTERFACE_SKELETON(device));
    g_dbus_object_manager_server_export(G_DBUS_OBJECT_MANAGER_SERVER(object_manager_server), skeleton);

    g_string_free(path, TRUE);
    Jabra_FreeDeviceInfo(deviceInfo);
}

void jabra_on_device_removed(unsigned short deviceID) {
    g_info("detached device (ID %u)", deviceID);

    GString *path = device_object_path(deviceID);
    g_dbus_object_manager_server_unexport(G_DBUS_OBJECT_MANAGER_SERVER(object_manager_server), path->str);

    g_string_free(path, TRUE);
}

void jabra_on_button_in_data_translated(unsigned short deviceID, Jabra_HidInput translatedInData, bool buttonInData) {
    GString *path = device_object_path(deviceID);

    GDBusInterface *device_interface = g_dbus_object_manager_get_interface(G_DBUS_OBJECT_MANAGER(object_manager_server),
                                                                           path->str,
                                                                           "com.github.borgoat.Jabra1.Device");
    gbjDevice *device = GBJ_DEVICE(device_interface);

    gbj_device_emit_button_in_data_translated(device, translatedInData, buttonInData);

    g_object_unref(device_interface);
    g_string_free(path, TRUE);
}
