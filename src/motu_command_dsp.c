// SPDX-License-Identifier: LGPL-2.1-or-later
#include "motu_command_dsp.h"

/**
 * HitakiMotuCommandDsp:
 * An interface for protocol of command DSP model in MOTU FireWire series.
 *
 * The command DSP models in Mark of the Unicorn (MOTU) FireWire series transfer isochronous
 * packets to deliver PCM frames and MIDI messages as well as metering information. The
 * [iface@MotuCommandDsp] is an object interface for the metering information in the command DSP
 * protocol.
 */
G_DEFINE_INTERFACE(HitakiMotuCommandDsp, hitaki_motu_command_dsp, G_TYPE_OBJECT)

static void hitaki_motu_command_dsp_default_init(HitakiMotuCommandDspInterface *iface)
{
    return;
}

/**
 * hitaki_motu_command_dsp_read_float_meter:
 * @self: A [iface@MotuCommandDsp]
 * @meter: (array fixed-size=400) (inout): The data for meter.
 * @error: A [struct@GLib.Error]. Error can be generated with Hitaki.AlsaFirewireError domain.
 *
 * Read cached data of meter information for command DSP models.
 *
 * Returns: TRUE if the overall operation finished successfully, else FALSE.
 */
gboolean hitaki_motu_command_dsp_read_float_meter(HitakiMotuCommandDsp *self,
                                                  gfloat *const meter[400], GError **error)
{
    g_return_val_if_fail(HITAKI_IS_MOTU_COMMAND_DSP(self), FALSE);
    g_return_val_if_fail(meter != NULL && *meter != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    return HITAKI_MOTU_COMMAND_DSP_GET_IFACE(self)->read_float_meter(self, meter, error);
}
