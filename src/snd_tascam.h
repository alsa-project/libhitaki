// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __HITAKI_SND_TASCAM_H__
#define __HITAKI_SND_TASCAM_H__

#include <hitaki.h>

G_BEGIN_DECLS

#define HITAKI_TYPE_SND_TASCAM      (hitaki_snd_tascam_get_type())

G_DECLARE_DERIVABLE_TYPE(HitakiSndTascam, hitaki_snd_tascam, HITAKI, SND_TASCAM, GObject);

struct _HitakiSndTascamClass {
    GObjectClass parent_class;
};

HitakiSndTascam *hitaki_snd_tascam_new(void);

G_END_DECLS

#endif
