// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __HITAKI_MOTU_COMMAND_DSP_H__
#define __HITAKI_MOTU_COMMAND_DSP_H__

#include <hitaki.h>

G_BEGIN_DECLS

#define HITAKI_TYPE_MOTU_COMMAND_DSP        (hitaki_motu_command_dsp_get_type())

G_DECLARE_INTERFACE(HitakiMotuCommandDsp, hitaki_motu_command_dsp, HITAKI, MOTU_COMMAND_DSP, GObject);

struct _HitakiMotuCommandDspInterface {
    GTypeInterface iface;

    gboolean (*read_float_meter)(HitakiMotuCommandDsp *self, gfloat *const meter[400],
                                 GError **error);
};

gboolean hitaki_motu_command_dsp_read_float_meter(HitakiMotuCommandDsp *self,
                                                  gfloat *const meter[400], GError **error);

G_END_DECLS

#endif
