// SPDX-License-Identifier: LGPL-2.1-or-later
#ifndef __HITAKI_SND_UNIT_H__
#define __HITAKI_SND_UNIT_H__

#include <hitaki.h>

G_BEGIN_DECLS

#define HITAKI_TYPE_SND_UNIT        (hitaki_snd_unit_get_type())

G_DECLARE_DERIVABLE_TYPE(HitakiSndUnit, hitaki_snd_unit, HITAKI, SND_UNIT, GObject);

struct _HitakiSndUnitClass {
    GObjectClass parent_class;
};

HitakiSndUnit *hitaki_snd_unit_new(void);

G_END_DECLS

#endif
