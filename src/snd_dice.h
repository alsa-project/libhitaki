// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __HITAKI_SND_DICE_H__
#define __HITAKI_SND_DICE_H__

#include <hitaki.h>

G_BEGIN_DECLS

#define HITAKI_TYPE_SND_DICE        (hitaki_snd_dice_get_type())

G_DECLARE_DERIVABLE_TYPE(HitakiSndDice, hitaki_snd_dice, HITAKI, SND_DICE, GObject);

struct _HitakiSndDiceClass {
    GObjectClass parent_class;
};

HitakiSndDice *hitaki_snd_dice_new(void);

G_END_DECLS

#endif
