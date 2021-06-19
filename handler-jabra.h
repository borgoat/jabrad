#ifndef JABRAD_HANDLER_JABRA_H
#define JABRAD_HANDLER_JABRA_H

#include <jabra/Common.h>
#include "handler-dbus.h"

/**
 * \brief Handler for Jabra_InitializeV2
 */
void jabra_on_first_scan_for_devices_done();

/**
 * \brief Handler for Jabra_InitializeV2
 */
void jabra_on_device_attached(Jabra_DeviceInfo deviceInfo);

/**
 * \brief Handler for Jabra_InitializeV2
 */
void jabra_on_device_removed(unsigned short deviceID);

/**
 * \brief Handler for Jabra_InitializeV2
 */
void jabra_on_button_in_data_translated(unsigned short deviceID, Jabra_HidInput translatedInData, bool buttonInData);

#endif //JABRAD_HANDLER_JABRA_H
