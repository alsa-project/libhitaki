// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __HITAKI_TIMESTAMPED_QUADLET_NOTIFICATION_H__
#define __HITAKI_TIMESTAMPED_QUADLET_NOTIFICATION_H__

#include <hitaki.h>

G_BEGIN_DECLS

#define HITAKI_TYPE_TIMESTAMPED_QUADLET_NOTIFICATION    (hitaki_timestamped_quadlet_notification_get_type())

G_DECLARE_INTERFACE(HitakiTimestampedQuadletNotification, hitaki_timestamped_quadlet_notification, HITAKI, TIMESTAMPED_QUADLET_NOTIFICATION, GObject);

struct _HitakiTimestampedQuadletNotificationInterface {
    GTypeInterface iface;
};

G_END_DECLS

#endif
