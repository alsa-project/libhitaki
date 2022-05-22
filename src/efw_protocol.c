// SPDX-License-Identifier: LGPL-2.1-or-later
#include "efw_protocol_private.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/**
 * HitakiEfwProtocol:
 * An interface for Fireworks Protocol.
 *
 * Echo Audio Fireworks devices listen to specific address space for specific command frame. When
 * accepting and handling the command frame, it transfers response frame to specific address in
 * command requester. The [iface@EfwProtocol] is an object interface for the Fireworks protocol.
 */
G_DEFINE_INTERFACE(HitakiEfwProtocol, hitaki_efw_protocol, G_TYPE_OBJECT)

/**
 * hitaki_efw_protocol_error_quark:
 *
 * Return the [alias@GLib.Quark] for [struct@GLib.Error] with code in [enum@EfwProtocolError]
 * enumerations.
 *
 * Returns: A [alias@GLib.Quark].
 */
G_DEFINE_QUARK(hitaki-efw-protocol-error-quark, hitaki_efw_protocol_error)

const char *efw_protocol_err_msgs[16] = {
    [HITAKI_EFW_PROTOCOL_ERROR_OK]              = "The transaction finished successfully",
    [HITAKI_EFW_PROTOCOL_ERROR_BAD]             = "The request or response includes invalid header",
    [HITAKI_EFW_PROTOCOL_ERROR_BAD_COMMAND]     = "The request includes invalid category or command",
    [HITAKI_EFW_PROTOCOL_ERROR_COMM_ERR]        = "The transaction fails due to communication error",
    [HITAKI_EFW_PROTOCOL_ERROR_BAD_QUAD_COUNT]  = "The number of quadlets in transaction is invalid",
    [HITAKI_EFW_PROTOCOL_ERROR_UNSUPPORTED]     = "The request is not supported",
    [HITAKI_EFW_PROTOCOL_ERROR_TIMEOUT]         = "The transaction is canceled due to response timeout",
    [HITAKI_EFW_PROTOCOL_ERROR_DSP_TIMEOUT]     = "The operation for DSP did not finish within timeout",
    [HITAKI_EFW_PROTOCOL_ERROR_BAD_RATE]        = "The request includes invalid value for sampling frequency",
    [HITAKI_EFW_PROTOCOL_ERROR_BAD_CLOCK]       = "The request includes invalid value for source of clock",
    [HITAKI_EFW_PROTOCOL_ERROR_BAD_CHANNEL]     = "The request includes invalid value for the number of channel",
    [HITAKI_EFW_PROTOCOL_ERROR_BAD_PAN]         = "The request includes invalid value for panning",
    [HITAKI_EFW_PROTOCOL_ERROR_FLASH_BUSY]      = "The on-board flash is busy and not operable",
    [HITAKI_EFW_PROTOCOL_ERROR_BAD_MIRROR]      = "The request includes invalid value for mirroring channel",
    [HITAKI_EFW_PROTOCOL_ERROR_BAD_LED]         = "The request includes invalid value for LED",
    [HITAKI_EFW_PROTOCOL_ERROR_BAD_PARAMETER]   = "The request includes invalid value of parameter",
};

static void hitaki_efw_protocol_default_init(HitakiEfwProtocolInterface *iface)
{
    /**
     * HitakiEfwProtocol::responded:
     * @self: A [iface@EfwProtocol].
     * @version: The version of transaction protocol.
     * @seqnum: The sequence number of response.
     * @category: The value of category field in the response.
     * @command: The value of command field in the response.
     * @status: The status of response.
     * @params: (array length=param_count) (element-type guint32): The array with quadlet elements
     *          of parameters in response of Fireworks protocol.
     * @param_count: The number of elements of the array.
     *
     * Emitted when the unit transfers asynchronous packet as response of Echo Audio Efw
     * transaction and the process successfully reads the content of response from ALSA Efw
     * driver.
     */
    g_signal_new("responded",
                 G_TYPE_FROM_INTERFACE(iface),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                 G_STRUCT_OFFSET(HitakiEfwProtocolInterface, responded),
                 NULL, NULL,
                 hitaki_sigs_marshal_VOID__UINT_UINT_UINT_UINT_ENUM_POINTER_UINT,
                 G_TYPE_NONE,
                 7, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT,
                 HITAKI_TYPE_EFW_PROTOCOL_ERROR, G_TYPE_POINTER, G_TYPE_UINT);
}

/**
 * hitaki_efw_protocol_request:
 * @self: A [iface@EfwProtocol].
 * @category: One of category for the transaction.
 * @command: One of commands for the transaction.
 * @args: (array length=arg_count) (in) (nullable): An array with elements of quadlet data for
 *        arguments of command.
 * @arg_count: The number of quadlets in the args array.
 * @resp_seqnum: (out): The sequence number to match response.
 * @error: A [struct@GLib.Error] with Hitaki.EfwProtocolError domain.
 *
 * Transfer asynchronous transaction for command frame of Echo Efw protocol. The call results
 * in [signal@EfwProtocol::responded] signal with data of response.
 *
 * Returns: TRUE if the overall operation finished successfully, else FALSE.
 */
gboolean hitaki_efw_protocol_request(HitakiEfwProtocol *self,
                                     guint category, guint command,
                                     const guint32 *args, gsize arg_count,
                                     guint32 *resp_seqnum, GError **error)
{
    HitakiEfwProtocolInterface *iface;

    g_return_val_if_fail(HITAKI_IS_EFW_PROTOCOL(self), FALSE);
    g_return_val_if_fail(resp_seqnum != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    iface = HITAKI_EFW_PROTOCOL_GET_IFACE(self);

    return iface->request(self, category, command, args, arg_count, resp_seqnum, error);
}

struct waiter {
    guint32 seqnum;

    guint32 category;
    guint32 command;
    guint32 status;
    guint32 *params;
    gsize *param_count;

    GCond cond;
    GMutex mutex;
};

static void handle_signal(HitakiEfwProtocol *self, guint version, guint seqnum, guint category,
                          guint command, HitakiEfwProtocolError status,
                          const guint32 *params, guint param_count, gpointer user_data)
{
    struct waiter *w = (struct waiter *)user_data;

    if (seqnum == w->seqnum && category == w->category && command == w->command) {
        g_mutex_lock(&w->mutex);
        w->status = status;
        if (status == HITAKI_EFW_PROTOCOL_ERROR_OK) {
            if (param_count > 0) {
                if (*w->param_count < param_count || w->params == NULL)
                    w->status = HITAKI_EFW_PROTOCOL_ERROR_BAD_QUAD_COUNT;
                else
                    memcpy(w->params, params, sizeof(w->params[0]) * param_count);
            }
            *w->param_count = param_count;
        }
        g_cond_signal(&w->cond);
        g_mutex_unlock(&w->mutex);
    }
}

#define INVALID_STATUS      0xffffffff

/**
 * hitaki_efw_protocol_transaction:
 * @self: A [iface@EfwProtocol].
 * @category: One of category for the transaction.
 * @command: One of commands for the transaction.
 * @args: (array length=arg_count) (in) (nullable): An array with elements for quadlet data as
 *        arguments for command.
 * @arg_count: The number of quadlets in the args array.
 * @params: (array length=param_count) (inout) (nullable): An array with elements for quadlet data
 *          to save parameters in response. Callers should give it for buffer with enough space
 *          against the request since this library performs no reallocation. Due to the reason, the
 *          value of this argument should point to the pointer to the array and immutable. The
 *          content of array is mutable for parameters in response.
 * @param_count: The number of quadlets in the params array.
 * @timeout_ms: The timeout to wait for response.
 * @error: A [struct@GLib.Error] with Hitaki.EfwProtocolError domain.
 *
 * Transfer asynchronous transaction for command frame of Echo Efw protocol and wait for
 * response matched to the command. The call results in [signal@EfwProtocol::responded] signal with data
 * of response.
 *
 * Returns: TRUE if the overall operation finished successfully, else FALSE.
 */
gboolean hitaki_efw_protocol_transaction(HitakiEfwProtocol *self, guint category, guint command,
                                    const guint32 *args, gsize arg_count,
                                    guint32 *const *params, gsize *param_count,
                                    guint timeout_ms, GError **error)
{
    struct waiter w;
    gulong handler_id;
    guint64 expiration;

    g_return_val_if_fail(HITAKI_IS_EFW_PROTOCOL(self), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    w.category = category;
    w.command = command;
    w.status = INVALID_STATUS;
    w.params = *params;
    w.param_count = param_count;
    g_cond_init(&w.cond);
    g_mutex_init(&w.mutex);

    handler_id = g_signal_connect(self, "responded", G_CALLBACK(handle_signal), &w);

    expiration = g_get_monotonic_time() + timeout_ms * G_TIME_SPAN_MILLISECOND;

    if (!hitaki_efw_protocol_request(self, category, command, args, arg_count, &w.seqnum, error)) {
        g_signal_handler_disconnect(self, handler_id);
        return FALSE;
    }

    g_mutex_lock(&w.mutex);
    while (w.status == INVALID_STATUS) {
        if (!g_cond_wait_until(&w.cond, &w.mutex, expiration))
            break;
    }
    g_signal_handler_disconnect(self, handler_id);
    g_mutex_unlock(&w.mutex);

    switch (w.status) {
    case HITAKI_EFW_PROTOCOL_ERROR_OK:
        return TRUE;
    case HITAKI_EFW_PROTOCOL_ERROR_BAD:
    case HITAKI_EFW_PROTOCOL_ERROR_BAD_COMMAND:
    case HITAKI_EFW_PROTOCOL_ERROR_COMM_ERR:
    case HITAKI_EFW_PROTOCOL_ERROR_BAD_QUAD_COUNT:
    case HITAKI_EFW_PROTOCOL_ERROR_UNSUPPORTED:
    case HITAKI_EFW_PROTOCOL_ERROR_TIMEOUT:
    case HITAKI_EFW_PROTOCOL_ERROR_DSP_TIMEOUT:
    case HITAKI_EFW_PROTOCOL_ERROR_BAD_RATE:
    case HITAKI_EFW_PROTOCOL_ERROR_BAD_CLOCK:
    case HITAKI_EFW_PROTOCOL_ERROR_BAD_CHANNEL:
    case HITAKI_EFW_PROTOCOL_ERROR_BAD_PAN:
    case HITAKI_EFW_PROTOCOL_ERROR_FLASH_BUSY:
    case HITAKI_EFW_PROTOCOL_ERROR_BAD_MIRROR:
    case HITAKI_EFW_PROTOCOL_ERROR_BAD_LED:
    case HITAKI_EFW_PROTOCOL_ERROR_BAD_PARAMETER:
        generate_efw_protocol_error(error, w.status);
        return FALSE;
    case INVALID_STATUS:
    default:
        generate_efw_protocol_error(error, HITAKI_EFW_PROTOCOL_ERROR_TIMEOUT);
        return FALSE;
    }
}
