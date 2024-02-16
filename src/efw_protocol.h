// SPDX-License-Identifier: LGPL-2.1-or-later
#ifndef __HITAKI_EFW_PROTOCOL_H__
#define __HITAKI_EFW_PROTOCOL_H__

#include <hitaki.h>

G_BEGIN_DECLS

#define HITAKI_TYPE_EFW_PROTOCOL        (hitaki_efw_protocol_get_type())

G_DECLARE_INTERFACE(HitakiEfwProtocol, hitaki_efw_protocol, HITAKI, EFW_PROTOCOL, GObject);

#define HITAKI_EFW_PROTOCOL_ERROR       (hitaki_efw_protocol_error_quark())

GQuark hitaki_efw_protocol_error_quark();

void hitaki_efw_protocol_error_to_label(HitakiEfwProtocolError code, const char **label);

struct _HitakiEfwProtocolInterface {
    GTypeInterface iface;

    /**
     * HitakiEfwProtocolInterface::transmit_request:
     * @self: A [iface@EfwProtocol].
     * @buffer: (array length=length): The content of request frame for transaction.
     * @length: The length of content.
     * @error: A [struct@GLib.Error] with error domain depending on implementation.
     *
     * Virtual function to transmit the content of request frame to target device.
     */
    gboolean (*transmit_request)(HitakiEfwProtocol *self, const guint8 *buffer, gsize length,
                                 GError **error);

    /**
     * HitakiEfwProtocolInterface::get_seqnum:
     * @self: A [iface@EfwProtocol].
     * @seqnum: (out): The sequence number for request frame of transaction.
     *
     * Virtual function to retrieve the sequence number for request frame of transaction. The number
     * is used for the frame transmitted to device, then the device is expected to transmit response
     * frame with the number incremented by 1.
     */
    void (*get_seqnum)(HitakiEfwProtocol *self, guint32 *seqnum);

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

gboolean hitaki_efw_protocol_transmit_request(HitakiEfwProtocol *self, guint category,
                                              guint command, const guint32 *args, gsize arg_count,
                                              guint32 *resp_seqnum, GError **error);

void hitaki_efw_protocol_receive_response(HitakiEfwProtocol *self, const guint8 *buffer, gsize length);

gboolean hitaki_efw_protocol_transaction(HitakiEfwProtocol *self, guint category, guint command,
                                         const guint32 *args, gsize arg_count,
                                         guint32 *const *params, gsize *param_count,
                                         guint timeout_ms, GError **error);

G_END_DECLS

#endif
