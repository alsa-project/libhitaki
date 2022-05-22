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
 * @HITAKI_ALSA_FIREWIRE_ERROR_WRONG_CLASS:     Mismatch between GObject class and the type of sound
 *                                              unit.
 */
typedef enum {
    HITAKI_ALSA_FIREWIRE_ERROR_FAILED = 0,
    HITAKI_ALSA_FIREWIRE_ERROR_IS_OPENED,
    HITAKI_ALSA_FIREWIRE_ERROR_IS_NOT_OPENED,
    HITAKI_ALSA_FIREWIRE_ERROR_IS_USED,
    HITAKI_ALSA_FIREWIRE_ERROR_IS_LOCKED,
    HITAKI_ALSA_FIREWIRE_ERROR_IS_UNLOCKED,
    HITAKI_ALSA_FIREWIRE_ERROR_IS_DISCONNECTED,
    HITAKI_ALSA_FIREWIRE_ERROR_WRONG_CLASS,
} HitakiAlsaFirewireError;

/**
 * HitakiEfwProtocolError:
  * HITAKI_EFW_PROTOCOL_ERROR_OK:               The transaction finished successfully.
  * HITAKI_EFW_PROTOCOL_ERROR_BAD:              The request or response includes invalid header.
  * HITAKI_EFW_PROTOCOL_ERROR_BAD_COMMAND:      The request includes invalid category or command.
  * HITAKI_EFW_PROTOCOL_ERROR_COMM_ERR:         The transaction fails due to communication error.
  * HITAKI_EFW_PROTOCOL_ERROR_BAD_QUAD_COUNT:   The number of quadlets in transaction is invalid.
  * HITAKI_EFW_PROTOCOL_ERROR_UNSUPPORTED:      The request is not supported.
  * HITAKI_EFW_PROTOCOL_ERROR_TIMEOUT:          The transaction is canceled due to response timeout.
  * HITAKI_EFW_PROTOCOL_ERROR_DSP_TIMEOUT:      The operation for DSP did not finish within timeout.
  * HITAKI_EFW_PROTOCOL_ERROR_BAD_RATE:         The request includes invalid value for sampling frequency.
  * HITAKI_EFW_PROTOCOL_ERROR_BAD_CLOCK:        The request includes invalid value for source of clock.
  * HITAKI_EFW_PROTOCOL_ERROR_BAD_CHANNEL:      The request includes invalid value for the number of channel.
  * HITAKI_EFW_PROTOCOL_ERROR_BAD_PAN:          The request includes invalid value for panning.
  * HITAKI_EFW_PROTOCOL_ERROR_FLASH_BUSY:       The on-board flash is busy and not operable.
  * HITAKI_EFW_PROTOCOL_ERROR_BAD_MIRROR:       The request includes invalid value for mirroring channel.
  * HITAKI_EFW_PROTOCOL_ERROR_BAD_LED:          The request includes invalid value for LED.
  * HITAKI_EFW_PROTOCOL_ERROR_BAD_PARAMETER:    The request includes invalid value of parameter.
  *
  * The enumerations to report status of response in Fireworks protocol.
  */
typedef enum {
    HITAKI_EFW_PROTOCOL_ERROR_OK                = 0,
    HITAKI_EFW_PROTOCOL_ERROR_BAD               = 1,
    HITAKI_EFW_PROTOCOL_ERROR_BAD_COMMAND       = 2,
    HITAKI_EFW_PROTOCOL_ERROR_COMM_ERR          = 3,
    HITAKI_EFW_PROTOCOL_ERROR_BAD_QUAD_COUNT    = 4,
    HITAKI_EFW_PROTOCOL_ERROR_UNSUPPORTED       = 5,
    HITAKI_EFW_PROTOCOL_ERROR_TIMEOUT           = 6,
    HITAKI_EFW_PROTOCOL_ERROR_DSP_TIMEOUT       = 7,
    HITAKI_EFW_PROTOCOL_ERROR_BAD_RATE          = 8,
    HITAKI_EFW_PROTOCOL_ERROR_BAD_CLOCK         = 9,
    HITAKI_EFW_PROTOCOL_ERROR_BAD_CHANNEL       = 10,
    HITAKI_EFW_PROTOCOL_ERROR_BAD_PAN           = 11,
    HITAKI_EFW_PROTOCOL_ERROR_FLASH_BUSY        = 12,
    HITAKI_EFW_PROTOCOL_ERROR_BAD_MIRROR        = 13,
    HITAKI_EFW_PROTOCOL_ERROR_BAD_LED           = 14,
    HITAKI_EFW_PROTOCOL_ERROR_BAD_PARAMETER     = 15,
} HitakiEfwProtocolError;

#endif
