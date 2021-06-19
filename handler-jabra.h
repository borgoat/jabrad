#ifndef JABRAD_HANDLER_JABRA_H
#define JABRAD_HANDLER_JABRA_H

#include <jabra/Common.h>
#include "handler-dbus.h"

#define OBJPATH_ROOT     "/com/github/borgoat/Jabra1/"
#define OBJPATH(OBJECT)  OBJPATH_ROOT #OBJECT
#define OBJPATH_MANAGER  OBJPATH(Manager)
#define MAXLEN  256

extern gbjManager *manager_interface;
extern GDBusObjectManagerServer *object_manager_server;

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
