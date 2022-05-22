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

/**
 * HitakiAlsaFirewireError:
 * @HITAKI_ALSA_FIREWIRE_ERROR_FAILED:          The system call fails.
 * @HITAKI_ALSA_FIREWIRE_ERROR_IS_OPENED:       The instance is already associated to any ALSA HwDep
 *                                              character device.
 * @HITAKI_ALSA_FIREWIRE_ERROR_IS_NOT_OPENED:   The instance is not associated to any ALSA HwDep
 *                                              character device yet.
 * @HITAKI_ALSA_FIREWIRE_ERROR_IS_USED:         The ALSA HwDep character device is used by the other
 *                                              process.
 * @HITAKI_ALSA_FIREWIRE_ERROR_IS_LOCKED:       The functionality of packet streaming is prohibited.
 * @HITAKI_ALSA_FIREWIRE_ERROR_IS_UNLOCKED:     The functionality of packet streaming is available.
 * @HITAKI_ALSA_FIREWIRE_ERROR_IS_DISCONNECTED: The sound card is disconnected.
 */
typedef enum {
    HITAKI_ALSA_FIREWIRE_ERROR_FAILED = 0,
    HITAKI_ALSA_FIREWIRE_ERROR_IS_OPENED,
    HITAKI_ALSA_FIREWIRE_ERROR_IS_NOT_OPENED,
    HITAKI_ALSA_FIREWIRE_ERROR_IS_USED,
    HITAKI_ALSA_FIREWIRE_ERROR_IS_LOCKED,
    HITAKI_ALSA_FIREWIRE_ERROR_IS_UNLOCKED,
    HITAKI_ALSA_FIREWIRE_ERROR_IS_DISCONNECTED,
} HitakiAlsaFirewireError;

#endif
