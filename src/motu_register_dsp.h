// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __HITAKI_MOTU_REGISTER_DSP_H__
#define __HITAKI_MOTU_REGISTER_DSP_H__

#include <hitaki.h>

G_BEGIN_DECLS

#define HITAKI_TYPE_MOTU_REGISTER_DSP       (hitaki_motu_register_dsp_get_type())

G_DECLARE_INTERFACE(HitakiMotuRegisterDsp, hitaki_motu_register_dsp, HITAKI, MOTU_REGISTER_DSP, GObject);

struct _HitakiMotuRegisterDspInterface {
    GTypeInterface iface;

    /**
     * HitakiMotuRegisterDspInterface::read_parameter:
     * @self: A [iface@MotuRegisterDsp].
     * @param: (inout): A [struct@SndMotuRegisterDspParameter].
     * @error: A [struct@GLib.Error]. Error can be generated with error domain depending on
     *         implementation.
     *
     * Virtual function to read cached parameter for register DSP models.
     *
     * Returns: TRUE if the overall operation finished successfully, else FALSE.
     */
    gboolean (*read_parameter)(HitakiMotuRegisterDsp *self,
                               HitakiSndMotuRegisterDspParameter *const *param, GError **error);

    /**
     * HitakiMotuRegisterDspInterface::read_byte_meter:
     * @self: A [iface@MotuRegisterDsp]
     * @meter: (array fixed-size=48) (inout): The data of meter. Index 0 to 23 for inputs and index
     *         24 to 47 for outputs.
     * @error: A [struct@GLib.Error]. Error can be generated with error domain depending on
     *         implementation.
     *
     * Virtual function to read cached data of meter information for register DSP models.
     *
     * Returns: TRUE if the overall operation finished successfully, else FALSE.
     */
    gboolean (*read_byte_meter)(HitakiMotuRegisterDsp *self, guint8 *const meter[48],
                                GError **error);

    /**
     * HitakiMotuRegisterDspInterface::changed:
     * @self: A [iface@MotuRegisterDsp]
     * @events: (element-type guint32)(array length=length): The array with element for unsigned
     *          32 bit encoded data.
     * @length: The length of events.
     *
     * Class closure for the [signal@MotuRegisterDsp::changed] signal.
     */
    void (*changed)(HitakiMotuRegisterDsp *self, const guint32 *events, guint length);
};

gboolean hitaki_motu_register_dsp_read_parameter(HitakiMotuRegisterDsp *self,
                                                 HitakiSndMotuRegisterDspParameter *const *param,
                                                 GError **error);

gboolean hitaki_motu_register_dsp_read_byte_meter(HitakiMotuRegisterDsp *self,
                                                  guint8 *const meter[48], GError **error);

G_END_DECLS

#endif
