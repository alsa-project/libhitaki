// SPDX-License-Identifier: LGPL-3.0-or-later
#include "motu_register_dsp.h"

/**
 * HitakiMotuRegisterDsp:
 * An interface for protocol of register DSP models in MOTU FireWire series.
 *
 * The register DSP models in Mark of the Unicorn (MOTU) FireWire series transfer isochronous
 * packets to delivers PCM frames and MIDI messages as well as DSP parameters and metering
 * information. The [iface@MotuRegisterDsp] is an object interface for the parameters and
 * metering information in the register DSP protocol.
 */
G_DEFINE_INTERFACE(HitakiMotuRegisterDsp, hitaki_motu_register_dsp, G_TYPE_OBJECT)

static void hitaki_motu_register_dsp_default_init(HitakiMotuRegisterDspInterface *iface)
{
    /**
     * HitakiMotuRegisterDsp::changed:
     * @self: A [iface@MotuRegisterDsp]
     * @events: (element-type guint32) (array length=length): The array with element for unsigned
     *          32 bit encoded data.
     * @length: The length of events.
     *
     * Emitted when MOTU register DSP models transfer events by messages in the sequence of
     * isochronous packet. The event consists of encoded data. The most significant byte is the
     * type of message. The next two bytes are two identifiers. The least significant byte is
     * value. The meaning of identifier 0, 1 and value is decided depending on the type. For
     * detail, see `sound/firewire/motu/motu-register-dsp-message-parser.c` in Linux kernel.
     */
    g_signal_new("changed",
            G_TYPE_FROM_INTERFACE(iface),
            G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
            G_STRUCT_OFFSET(HitakiMotuRegisterDspInterface, changed),
            NULL, NULL,
            hitaki_sigs_marshal_VOID__POINTER_UINT,
            G_TYPE_NONE,
            2, G_TYPE_POINTER, G_TYPE_UINT);
}

/**
 * hitaki_motu_register_dsp_read_parameter:
 * @self: A [iface@MotuRegisterDsp].
 * @param: (inout): A [struct@SndMotuRegisterDspParameter].
 * @error: A [struct@GLib.Error]. Error can be generated with error domain depending on
 *         implementation.
 *
 * Read cached parameter for register DSP models.
 *
 * Returns: TRUE if the overall operation finished successfully, else FALSE.
 */
gboolean hitaki_motu_register_dsp_read_parameter(HitakiMotuRegisterDsp *self,
                                                 HitakiSndMotuRegisterDspParameter *const *param,
                                                 GError **error)
{
    g_return_val_if_fail(HITAKI_IS_MOTU_REGISTER_DSP(self), FALSE);
    g_return_val_if_fail(param != NULL && *param != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    return HITAKI_MOTU_REGISTER_DSP_GET_IFACE(self)->read_parameter(self, param, error);
}

/**
 * hitaki_motu_register_dsp_read_byte_meter:
 * @self: A [iface@MotuRegisterDsp]
 * @meter: (array fixed-size=48) (inout): The data of meter. Index 0 to 23 for inputs and index 24
 *         to 47 for outputs.
 * @error: A [struct@GLib.Error]. Error can be generated with error domain depending on
 *         implementation.
 *
 * Read cached data of meter information for register DSP models.
 *
 * Returns: TRUE if the overall operation finished successfully, else FALSE.
 */
gboolean hitaki_motu_register_dsp_read_byte_meter(HitakiMotuRegisterDsp *self,
                                                  guint8 *const meter[48], GError **error)
{
    g_return_val_if_fail(HITAKI_IS_MOTU_REGISTER_DSP(self), FALSE);
    g_return_val_if_fail(meter != NULL && *meter != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    return HITAKI_MOTU_REGISTER_DSP_GET_IFACE(self)->read_byte_meter(self, meter, error);
}
