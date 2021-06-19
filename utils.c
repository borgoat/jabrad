#include "utils.h"

// TODO Move somewhere else
static gboolean on_handle_get_firmware_version(gbjDevice *d, GDBusMethodInvocation *invocation, gpointer user_data) {
    gchar *firmware_version = g_new(gchar, MAXLEN + 1);
    Jabra_GetFirmwareVersion(gbj_device_get_device_id(d), firmware_version, MAXLEN);
    gbj_device_complete_get_firmware_version(d, invocation, firmware_version);
    g_free(firmware_version);
    return TRUE;
}

void configure_device_with_device_info(gbjDevice *d, const Jabra_DeviceInfo *info) {
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

gchar *device_object_path(unsigned short deviceID) {
    gchar *buf = NULL;
    buf = g_new(gchar, MAXLEN + 1);
    g_snprintf(buf, MAXLEN, "%s/%hu", OBJPATH_MANAGER, deviceID);
    return buf;
}
