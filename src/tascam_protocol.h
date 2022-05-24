// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __HITAKI_TASCAM_PROTOCOL_H__
#define __HITAKI_TASCAM_PROTOCOL_H__

#include <hitaki.h>

G_BEGIN_DECLS

#define HITAKI_TYPE_TASCAM_PROTOCOL     (hitaki_tascam_protocol_get_type())

G_DECLARE_INTERFACE(HitakiTascamProtocol, hitaki_tascam_protocol, HITAKI, TASCAM_PROTOCOL, GObject);

struct _HitakiTascamProtocolInterface {
    GTypeInterface iface;

    /**
     * HitakiTascamProtocolInterface::read_state:
     * @self: A [iface@TascamProtocol].
     * @state: (array length=count) (inout): The image of state.
     * @count: (inout): The length of image for state.
     * @error: A [struct@GLib.Error] with Hitaki.AlsaFirewireError domain.
     *
     * Virtual function to read the latest image of device state.
     *
     * Returns: TRUE if the overall operation finished successfully, else FALSE.
     */
    gboolean (*read_state)(HitakiTascamProtocol *self, guint32 *const *state, gsize *count,
                           GError **error);

    /**
     * HitakiTascamProtocolInterface::changed:
     * @self: A [iface@TascamProtocol]
     * @index: the numeric index on image for status and control information.
     * @before: the value of information before changed.
     * @after: the value of information after changed.
     *
     * Class closure for the [signal@TascamProtocol::changed] signal.
     */
    void (*changed)(HitakiTascamProtocol *self, guint index, guint before, guint after);
};

gboolean hitaki_tascam_protocol_read_state(HitakiTascamProtocol *self, guint32 *const *state,
                                           gsize *count, GError **error);

G_END_DECLS

#endif
