// SPDX-License-Identifier: LGPL-2.1-or-later
#ifndef __HITAKI_ENUM_TYPES_H__
#define __HITAKI_ENUM_TYPES_H__

/**
 * HitakiAlsaFirewireType:
 * @HITAKI_ALSA_FIREWIRE_TYPE_DICE:      The type of DICE ASIC.
 * @HITAKI_ALSA_FIREWIRE_TYPE_FIREWORKS: The type of Fireworks board module.
 * @HITAKI_ALSA_FIREWIRE_TYPE_BEBOB:     The type of BeBoB ASIC.
 * @HITAKI_ALSA_FIREWIRE_TYPE_OXFW:      The type of OXFW ASIC
 * @HITAKI_ALSA_FIREWIRE_TYPE_DIGI00X:   The type of Digi00x series.
 * @HITAKI_ALSA_FIREWIRE_TYPE_TASCAM:    The type of Tascam FireWire series.
 * @HITAKI_ALSA_FIREWIRE_TYPE_MOTU:      The type of MOTU FireWire series.
 * @HITAKI_ALSA_FIREWIRE_TYPE_FIREFACE:  The type of RME Fireface series.
 *
 * The enumerations for type of sound unit defined by ALSA firewire stack.
 */
typedef enum {
    HITAKI_ALSA_FIREWIRE_TYPE_DICE = 1,
    HITAKI_ALSA_FIREWIRE_TYPE_FIREWORKS,
    HITAKI_ALSA_FIREWIRE_TYPE_BEBOB,
    HITAKI_ALSA_FIREWIRE_TYPE_OXFW,
    HITAKI_ALSA_FIREWIRE_TYPE_DIGI00X,
    HITAKI_ALSA_FIREWIRE_TYPE_TASCAM,
    HITAKI_ALSA_FIREWIRE_TYPE_MOTU,
    HITAKI_ALSA_FIREWIRE_TYPE_FIREFACE,
} HitakiAlsaFirewireType;

#endif
