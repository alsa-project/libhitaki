// SPDX-License-Identifier: LGPL-2.1-or-later
#include "tascam_protocol.h"

/**
 * HitakiTascamProtocol:
 * An interface for protocol of TASCAM FireWire series.
 *
 * TASCAM FireWire series transfer image of device state by either isochronous or asynchronous
 * packets. The [iface@TascamProtocol] is an object interface for the image and the change of state
 * in the TASCAM FireWire protocol.
 */
G_DEFINE_INTERFACE(HitakiTascamProtocol, hitaki_tascam_protocol, G_TYPE_OBJECT)

static void hitaki_tascam_protocol_default_init(HitakiTascamProtocolInterface *iface)
{
    /**
     * HitakiTascamProtocol::changed:
     * @self: A [iface@TascamProtocol]
     * @index: the numeric index on image of status and control info.
     * @before: the value of info before changed.
     * @after: the value of info after changed.
     *
     * Emitted when the part of image differed for the change of device state.
     */
    g_signal_new("changed",
                 G_TYPE_FROM_INTERFACE(iface),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                 G_STRUCT_OFFSET(HitakiTascamProtocolInterface, changed),
                 NULL, NULL,
                 hitaki_sigs_marshal_VOID__UINT_UINT_UINT,
                 G_TYPE_NONE,
                 3, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT);
}

/**
 * hitaki_tascam_protocol_read_state:
 * @self: A [iface@TascamProtocol].
 * @state: (array length=count) (inout): The image of state.
 * @count: (inout): The length of image for state.
 * @error: A [struct@GLib.Error] with Hitaki.AlsaFirewireError domain.
 *
 * Read the latest image of device state.
 *
 * Returns: TRUE if the overall operation finished successfully, else FALSE.
 */
gboolean hitaki_tascam_protocol_read_state(HitakiTascamProtocol *self, guint32 *const *state,
                                           gsize *count, GError **error)
{
    HitakiTascamProtocolInterface *iface;

    g_return_val_if_fail(HITAKI_IS_TASCAM_PROTOCOL(self), FALSE);
    g_return_val_if_fail(state != NULL && *state != NULL, FALSE);
    g_return_val_if_fail(count != NULL && *count > 0, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    iface = HITAKI_TASCAM_PROTOCOL_GET_IFACE(self);

    return iface->read_state(self, state, count, error);
}
