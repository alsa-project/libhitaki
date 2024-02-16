// SPDX-License-Identifier: LGPL-2.1-or-later
#ifndef __HITAKI_SND_FIREFACE_H__
#define __HITAKI_SND_FIREFACE_H__

#include <hitaki.h>

G_BEGIN_DECLS

#define HITAKI_TYPE_SND_FIREFACE        (hitaki_snd_fireface_get_type())

G_DECLARE_DERIVABLE_TYPE(HitakiSndFireface, hitaki_snd_fireface, HITAKI, SND_FIREFACE, GObject);

struct _HitakiSndFirefaceClass {
    GObjectClass parent_class;
};

HitakiSndFireface *hitaki_snd_fireface_new(void);

G_END_DECLS

#endif
