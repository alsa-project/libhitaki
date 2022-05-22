// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __HITAKI_ALSA_FIREWIRE_H__
#define __HITAKI_ALSA_FIREWIRE_H__

#include <hitaki.h>

G_BEGIN_DECLS

#define HITAKI_TYPE_ALSA_FIREWIRE        (hitaki_alsa_firewire_get_type())

G_DECLARE_INTERFACE(HitakiAlsaFirewire, hitaki_alsa_firewire, HITAKI, ALSA_FIREWIRE, GObject);

#define HITAKI_ALSA_FIREWIRE_ERROR       (hitaki_alsa_firewire_error_quark())

GQuark hitaki_alsa_firewire_error_quark();

struct _HitakiAlsaFirewireInterface {
    GTypeInterface iface;

    gboolean (*open)(HitakiAlsaFirewire *self, const gchar *path, gint open_flag, GError **error);

    gboolean (*lock)(HitakiAlsaFirewire *self, GError **error);

    gboolean (*unlock)(HitakiAlsaFirewire *self, GError **error);

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
