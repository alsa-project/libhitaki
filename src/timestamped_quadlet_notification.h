// SPDX-License-Identifier: LGPL-2.1-or-later
#ifndef __HITAKI_TIMESTAMPED_QUADLET_NOTIFICATION_H__
#define __HITAKI_TIMESTAMPED_QUADLET_NOTIFICATION_H__

#include <hitaki.h>

G_BEGIN_DECLS

#define HITAKI_TYPE_TIMESTAMPED_QUADLET_NOTIFICATION    (hitaki_timestamped_quadlet_notification_get_type())

G_DECLARE_INTERFACE(HitakiTimestampedQuadletNotification, hitaki_timestamped_quadlet_notification, HITAKI, TIMESTAMPED_QUADLET_NOTIFICATION, GObject);

struct _HitakiTimestampedQuadletNotificationInterface {
    GTypeInterface iface;

    /**
     * HitakiQuadletNotificationInterface::notified_at:
     * @self: A [iface@QuadletNotification]
     * @message: A quadlet message in notification.
     * @tstamp: The isochronous cycle at which the request arrived.
     *
     * Class closure for the [signal@QuadletNotification::notified-at] signal.
     */
    void (*notified_at)(HitakiTimestampedQuadletNotification *self, guint message, guint tstamp);
};

G_END_DECLS

#endif
