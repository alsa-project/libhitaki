// SPDX-License-Identifier: LGPL-2.1-or-later
#ifndef __HITAKI_QUADLET_NOTIFICATION_H__
#define __HITAKI_QUADLET_NOTIFICATION_H__

#include <hitaki.h>

G_BEGIN_DECLS

#define HITAKI_TYPE_QUADLET_NOTIFICATION    (hitaki_quadlet_notification_get_type())

G_DECLARE_INTERFACE(HitakiQuadletNotification, hitaki_quadlet_notification, HITAKI, QUADLET_NOTIFICATION, GObject);

struct _HitakiQuadletNotificationInterface {
    GTypeInterface iface;

    /**
     * HitakiQuadletNotificationInterface::notified:
     * @self: A [iface@QuadletNotification]
     * @message: A quadlet message in notification.
     *
     * Class closure for the [signal@QuadletNotification::notified] signal.
     */
    void (*notified)(HitakiQuadletNotification *self, guint32 message);
};

G_END_DECLS

#endif
