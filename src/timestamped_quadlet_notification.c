// SPDX-License-Identifier: LGPL-3.0-or-later
#include "timestamped_quadlet_notification.h"

/**
 * HitakiTimestampedQuadletNotification:
 * An interface to operate notification with quadlet message and time stamp.
 *
 * Some of units supported by drivers in ALSA firewire stack have the function to notify quadlet
 * message and time stamp for specific purposes. The [iface@TimestampedQuadletNotification] is an
 * interface to operate the notification.
 */
G_DEFINE_INTERFACE(HitakiTimestampedQuadletNotification, hitaki_timestamped_quadlet_notification, G_TYPE_OBJECT)

static void hitaki_timestamped_quadlet_notification_default_init(HitakiTimestampedQuadletNotificationInterface *iface)
{
    /**
     * HitakiTimestampedQuadletNotification::notified-at:
     * @self: A [iface@TimestampedQuadletNotification]
     * @message: A quadlet message in notification.
     * @tstamp: The isochronous cycle at which the request arrived.
     *
     * Emitted when the target unit transfers notification.
     *
     * The value of @tstamp is unsigned 16 bit integer including higher 3 bits for three low order
     * bits of second field and the rest 13 bits for cycle field in the format of IEEE 1394
     * CYCLE_TIMER register.
     */
    g_signal_new("notified-at",
                 G_TYPE_FROM_INTERFACE(iface),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                 G_STRUCT_OFFSET(HitakiTimestampedQuadletNotificationInterface, notified_at),
                 NULL, NULL,
                 hitaki_sigs_marshal_VOID__UINT_UINT,
                 G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_UINT);
}
