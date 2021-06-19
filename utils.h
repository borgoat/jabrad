#ifndef JABRAD_UTILS_H
#define JABRAD_UTILS_H

#include <gio/gio.h>
#include <jabra/Common.h>
#include "dbus-gen.h"

#define OBJPATH_ROOT     "/com/github/borgoat/Jabra1/"
#define OBJPATH(OBJECT)  OBJPATH_ROOT #OBJECT
#define OBJPATH_MANAGER  OBJPATH(Manager)
#define MAXLEN  256

extern gbjManager *manager_interface;
extern GDBusObjectManagerServer *object_manager_server;

/**
 *
 * @param d
 * @param info
 */
void configure_device_with_device_info(gbjDevice *d, const Jabra_DeviceInfo *info);

/**
 *
 * @param deviceID
 * @return
 */
gchar *device_object_path(unsigned short deviceID);

#endif //JABRAD_UTILS_H
