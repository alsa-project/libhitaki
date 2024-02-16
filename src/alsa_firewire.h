// SPDX-License-Identifier: LGPL-2.1-or-later
#ifndef __HITAKI_ALSA_FIREWIRE_H__
#define __HITAKI_ALSA_FIREWIRE_H__

#include <hitaki.h>

G_BEGIN_DECLS

#define HITAKI_TYPE_ALSA_FIREWIRE        (hitaki_alsa_firewire_get_type())

G_DECLARE_INTERFACE(HitakiAlsaFirewire, hitaki_alsa_firewire, HITAKI, ALSA_FIREWIRE, GObject);

#define HITAKI_ALSA_FIREWIRE_ERROR       (hitaki_alsa_firewire_error_quark())

GQuark hitaki_alsa_firewire_error_quark();

void hitaki_alsa_firewire_error_to_label(HitakiAlsaFirewireError code, const char **label);

struct _HitakiAlsaFirewireInterface {
    GTypeInterface iface;

    /**
     * HitakiAlsaFirewireInterface::open:
     * @self: A [iface@AlsaFirewire]
     * @path: A path to special file for ALSA HwDep character device.
     * @open_flag: The flag of `open(2)` system call.
     * @error: A [struct@GLib.Error].
     *
     * Virtual function to open the special file for ALSA HwDep character device.
     *
     * Returns: TRUE if the overall operation finished successfully, else FALSE.
     */
    gboolean (*open)(HitakiAlsaFirewire *self, const gchar *path, gint open_flag, GError **error);

    /**
     * HitakiAlsaFirewireInterface::lock:
     * @self: A [iface@AlsaFirewire]
     * @error: A [struct@GLib.Error].
     *
     * Virtual function to lock kernel driver bound to the associated ALSA HwDep character device
     * so that it is prohibited to start packet streaming.
     *
     * Returns: TRUE if the overall operation finished successfully, else FALSE.
     */
    gboolean (*lock)(HitakiAlsaFirewire *self, GError **error);

    /**
     * HitakiAlsaFirewireInterface::unlock:
     * @self: A [iface@AlsaFirewire]
     * @error: A [struct@GLib.Error].
     *
     * Virtual function to unlock kernel driver bound to the associated ALSA HwDep character device
     * so that it can start packet streaming.
     *
     * Returns: TRUE if the overall operation finished successfully, else FALSE.
     */
    gboolean (*unlock)(HitakiAlsaFirewire *self, GError **error);

    /**
     * HitakiAlsaFirewireInterface::create_source:
     * @self: A [iface@AlsaFirewire].
     * @source: (out): A [struct@GLib.Source] to handle events from ALSA HwDep character device.
     * @error: A [struct@GLib.Error].
     *
     * Virtual function to allocate [struct@GLib.Source] to handle events from ALSA HwDep
     * character device.
     *
     * Returns: TRUE if the overall operation finished successfully, else FALSE.
     */
    gboolean (*create_source)(HitakiAlsaFirewire *self, GSource **source, GError **error);
};

gboolean hitaki_alsa_firewire_open(HitakiAlsaFirewire *self, const gchar *path, gint open_flag,
                                  GError **error);

gboolean hitaki_alsa_firewire_lock(HitakiAlsaFirewire *self, GError **error);

gboolean hitaki_alsa_firewire_unlock(HitakiAlsaFirewire *self, GError **error);

gboolean hitaki_alsa_firewire_create_source(HitakiAlsaFirewire *self, GSource **source,
                                           GError **error);

G_END_DECLS

#endif
