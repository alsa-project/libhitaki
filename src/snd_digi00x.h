// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __HITAKI_SND_DIGI00X_H__
#define __HITAKI_SND_DIGI00X_H__

#include <hitaki.h>

G_BEGIN_DECLS

#define HITAKI_TYPE_SND_DIGI00X         (hitaki_snd_digi00x_get_type())

G_DECLARE_DERIVABLE_TYPE(HitakiSndDigi00x, hitaki_snd_digi00x, HITAKI, SND_DIGI00X, GObject);

struct _HitakiSndDigi00xClass {
    GObjectClass parent_class;
};

HitakiSndDigi00x *hitaki_snd_digi00x_new(void);

G_END_DECLS

#endif
