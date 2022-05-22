// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __HITAKI_EFW_PROTOCOL_H__
#define __HITAKI_EFW_PROTOCOL_H__

#include <hitaki.h>

G_BEGIN_DECLS

#define HITAKI_TYPE_EFW_PROTOCOL        (hitaki_efw_protocol_get_type())

G_DECLARE_INTERFACE(HitakiEfwProtocol, hitaki_efw_protocol, HITAKI, EFW_PROTOCOL, GObject);

#define HITAKI_EFW_PROTOCOL_ERROR       (hitaki_efw_protocol_error_quark())

GQuark hitaki_efw_protocol_error_quark();

struct _HitakiEfwProtocolInterface {
    GTypeInterface iface;

    gboolean (*request)(HitakiEfwProtocol *self, guint category, guint command,
                        const guint32 *args, gsize arg_count, guint32 *resp_seqnum,
                        GError **error);
    /**
     * HitakiEfwProtocolInterface::responded:
     * @self: A [iface@EfwProtocol].
     * @version: The version of transaction protocol.
     * @seqnum: The sequence number of response.
     * @category: The value of category field in the response.
     * @command: The value of command field in the response.
     * @status: The status of response.
     * @params: (array length=param_count)(element-type guint32): The array with quadlet elements
     *          of parameters in response of Fireworks protocol.
     * @param_count: The number of elements of the array.
     *
     * Class closure for the [signal@EfwProtocol::responded] signal.
     */
    void (*responded)(HitakiEfwProtocol *self, guint version, guint seqnum, guint category,
                      guint command, HitakiEfwProtocolError status, const guint32 *params, guint param_count);
};

gboolean hitaki_efw_protocol_request(HitakiEfwProtocol *self, guint category, guint command,
                                     const guint32 *args, gsize arg_count, guint32 *resp_seqnum,
                                     GError **error);

gboolean hitaki_efw_protocol_transaction(HitakiEfwProtocol *self, guint category, guint command,
                                         const guint32 *args, gsize arg_count,
                                         guint32 *const *params, gsize *param_count,
                                         guint timeout_ms, GError **error);

G_END_DECLS

#endif
