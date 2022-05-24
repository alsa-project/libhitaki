// SPDX-License-Identifier: LGPL-3.0-or-later
#include "quadlet_notification.h"

/**
 * HitakiQuadletNotification:
 * An interface to operate notification with quadlet message.
 *
 * Some of units supported by drivers in ALSA firewire stack have the function to notify quadlet
 * message for specific purposes. The [iface@QuadletNotification] is an interface to operate
 * the notification.
 */
G_DEFINE_INTERFACE(HitakiQuadletNotification, hitaki_quadlet_notification, G_TYPE_OBJECT)

static void hitaki_quadlet_notification_default_init(HitakiQuadletNotificationInterface *iface)
{
    /**
     * HitakiQuadletNotification::notified:
     * @self: A [iface@QuadletNotification]
     * @message: A quadlet message in notification.
     *
     * Emitted when the target unit transfers notification.
     */
    g_signal_new("notified",
                 G_TYPE_FROM_INTERFACE(iface),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                 G_STRUCT_OFFSET(HitakiQuadletNotificationInterface, notified),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__UINT,
                 G_TYPE_NONE, 1, G_TYPE_UINT);
}
