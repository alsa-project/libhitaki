// SPDX-License-Identifier: LGPL-2.1-or-later
#ifndef __HITAKI_SND_MOTU_H__
#define __HITAKI_SND_MOTU_H__

#include <hitaki.h>

G_BEGIN_DECLS

#define HITAKI_TYPE_SND_MOTU        (hitaki_snd_motu_get_type())

G_DECLARE_DERIVABLE_TYPE(HitakiSndMotu, hitaki_snd_motu, HITAKI, SND_MOTU, GObject);

struct _HitakiSndMotuClass {
    GObjectClass parent_class;
};

HitakiSndMotu *hitaki_snd_motu_new(void);

G_END_DECLS

#endif
