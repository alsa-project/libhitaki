// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __HITAKI_SND_EFW_H__
#define __HITAKI_SND_EFW_H__

#include <hitaki.h>

G_BEGIN_DECLS

#define HITAKI_TYPE_SND_EFW         (hitaki_snd_efw_get_type())

G_DECLARE_DERIVABLE_TYPE(HitakiSndEfw, hitaki_snd_efw, HITAKI, SND_EFW, GObject);

struct _HitakiSndEfwClass {
    GObjectClass parent_class;
};

HitakiSndEfw *hitaki_snd_efw_new(void);

G_END_DECLS

#endif
