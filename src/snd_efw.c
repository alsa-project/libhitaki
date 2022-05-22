// SPDX-License-Identifier: LGPL-2.1-or-later
#include "alsa_firewire_private.h"
#include "efw_protocol_private.h"

#include <fcntl.h>

/**
 * HitakiSndEfw:
 * A GObject-derived object for sound unit of Echo Audio Fireworks.
 *
 * The [class@SndEfw] is an object class derived from [class@GObject.Object] with protocol
 * implementation for Echo Audio Fireworks devices supported by ALSA fireworks driver
 * (`snd-fireworks`).
 */

typedef struct {
    struct alsa_firewire_state state;

    guint32 seqnum;
    GMutex lock;
} HitakiSndEfwPrivate;

static void alsa_firewire_iface_init(HitakiAlsaFirewireInterface *iface);
static void efw_protocol_iface_init(HitakiEfwProtocolInterface *iface);

G_DEFINE_TYPE_WITH_CODE(HitakiSndEfw, hitaki_snd_efw, G_TYPE_OBJECT,
                        G_ADD_PRIVATE(HitakiSndEfw)
                        G_IMPLEMENT_INTERFACE(HITAKI_TYPE_ALSA_FIREWIRE, alsa_firewire_iface_init)
                        G_IMPLEMENT_INTERFACE(HITAKI_TYPE_EFW_PROTOCOL, efw_protocol_iface_init));

static void snd_efw_set_property(GObject *inst, guint id, const GValue *val, GParamSpec *spec)
{
    HitakiSndEfw *self = HITAKI_SND_EFW(inst);
    HitakiSndEfwPrivate *priv = hitaki_snd_efw_get_instance_private(self);

    alsa_firewire_state_set_property(&priv->state, inst, id, val, spec);
}

static void snd_efw_get_property(GObject *inst, guint id, GValue *val, GParamSpec *spec)
{
    HitakiSndEfw *self = HITAKI_SND_EFW(inst);
    HitakiSndEfwPrivate *priv = hitaki_snd_efw_get_instance_private(self);

    alsa_firewire_state_get_property(&priv->state, inst, id, val, spec);
}

static void snd_efw_finalize(GObject *obj)
{
    HitakiSndEfw *self = HITAKI_SND_EFW(obj);
    HitakiSndEfwPrivate *priv = hitaki_snd_efw_get_instance_private(self);

    alsa_firewire_state_release(&priv->state);

    G_OBJECT_CLASS(hitaki_snd_efw_parent_class)->finalize(obj);
}

static void hitaki_snd_efw_class_init(HitakiSndEfwClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = snd_efw_set_property;
    gobject_class->get_property = snd_efw_get_property;
    gobject_class->finalize = snd_efw_finalize;

    alsa_firewire_class_override_properties(gobject_class);
}

static void hitaki_snd_efw_init(HitakiSndEfw *self)
{
    HitakiSndEfwPrivate *priv = hitaki_snd_efw_get_instance_private(self);

    alsa_firewire_state_init(&priv->state);

    priv->seqnum = 0;
    g_mutex_init(&priv->lock);
}

static gboolean snd_efw_open(HitakiAlsaFirewire *inst, const gchar *path, gint open_flag,
                             GError **error)
{
    HitakiSndEfw *self;
    HitakiSndEfwPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_EFW(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_EFW(inst);
    priv = hitaki_snd_efw_get_instance_private(self);

    open_flag |= O_RDWR;
    if (!alsa_firewire_state_open(&priv->state, path, open_flag, error))
        return FALSE;

    if (priv->state.info.type != SNDRV_FIREWIRE_TYPE_FIREWORKS) {
        generate_alsa_firewire_error(error, HITAKI_ALSA_FIREWIRE_ERROR_WRONG_CLASS);
        alsa_firewire_state_release(&priv->state);
        return FALSE;
    }

    return TRUE;
}

static gboolean snd_efw_lock(HitakiAlsaFirewire *inst, GError **error)
{
    HitakiSndEfw *self;
    HitakiSndEfwPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_EFW(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_EFW(inst);
    priv = hitaki_snd_efw_get_instance_private(self);

    return alsa_firewire_state_lock(&priv->state, error);
}

static gboolean snd_efw_unlock(HitakiAlsaFirewire *inst, GError **error)
{
    HitakiSndEfw *self;
    HitakiSndEfwPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_EFW(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_EFW(inst);
    priv = hitaki_snd_efw_get_instance_private(self);

    return alsa_firewire_state_unlock(&priv->state, error);
}

static void handle_response(HitakiSndEfw *self, struct snd_efw_transaction *response,
                            unsigned int param_count)
{
    unsigned int version = GUINT32_FROM_BE(response->version);
    unsigned int seqnum = GUINT32_FROM_BE(response->seqnum);
    unsigned int category = GUINT32_FROM_BE(response->category);
    unsigned int command = GUINT32_FROM_BE(response->command);
    unsigned int status = GUINT32_FROM_BE(response->status);
    int i;

    if (status != HITAKI_EFW_PROTOCOL_ERROR_OK) {
        if (status >= HITAKI_EFW_PROTOCOL_ERROR_BAD_PARAMETER)
            status = HITAKI_EFW_PROTOCOL_ERROR_BAD;
    }

    for (i = 0; i < param_count; ++i)
        response->params[i] = GUINT32_FROM_BE(response->params[i]);

    g_signal_emit_by_name(self, "responded", version, seqnum, category, command, status,
                          response->params, param_count);
}

static void handle_event(HitakiAlsaFirewire *inst, union snd_firewire_event *event, size_t length)
{
    HitakiSndEfw *self;

    __be32 *buf;

    g_return_if_fail(HITAKI_IS_SND_EFW(inst));
    self = HITAKI_SND_EFW(inst);

    length -= sizeof(event->efw_response.type);
    buf = event->efw_response.response;

    // MEMO: Old version of ALSA fireworks driver has a bug to report the reading size shorter than
    // expected by 4 bytes, while the driver ensure copying the content per each response. This is a
    // workaround.
    length += 4;

    while (length >= HEADER_SIZE) {
        struct snd_efw_transaction *response = (struct snd_efw_transaction *)buf;
        unsigned int quadlet_count;
        unsigned int param_count;

        quadlet_count = GUINT32_FROM_BE(response->length);

        param_count = quadlet_count - HEADER_QUADLET_COUNT;
        handle_response(self, response, param_count);

        buf += quadlet_count;
        length -= quadlet_count * sizeof(__be32);
    }
}

static gboolean snd_efw_create_source(HitakiAlsaFirewire *inst, GSource **source, GError **error)
{
    HitakiSndEfw *self;
    HitakiSndEfwPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_EFW(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_EFW(inst);
    priv = hitaki_snd_efw_get_instance_private(self);

    return alsa_firewire_state_create_source(&priv->state, inst, handle_event, source, error);
}

static void alsa_firewire_iface_init(HitakiAlsaFirewireInterface *iface)
{
    iface->open = snd_efw_open;
    iface->lock = snd_efw_lock;
    iface->unlock = snd_efw_unlock;
    iface->create_source = snd_efw_create_source;
}

static gboolean snd_efw_request(HitakiEfwProtocol *inst, guint category, guint command,
                                const guint32 *args, gsize arg_count, guint32 *resp_seqnum,
                                GError **error)
{
    HitakiSndEfw *self;
    HitakiSndEfwPrivate *priv;

    struct snd_efw_transaction *frame;
    unsigned int quads;
    size_t length;
    ssize_t len;

    g_return_val_if_fail(HITAKI_IS_SND_EFW(inst), FALSE);
    g_return_val_if_fail(resp_seqnum != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_EFW(inst);
    priv = hitaki_snd_efw_get_instance_private(self);

    quads = sizeof(*frame) / 4;
    if (args != NULL)
        quads += arg_count;
    length = quads * sizeof(__be32);

    if (length > MAXIMUM_FRAME_BYTES) {
        generate_efw_protocol_error(error, HITAKI_EFW_PROTOCOL_ERROR_BAD_PARAMETER);
        return FALSE;
    }

    frame = g_malloc0(length);

    frame->length = GUINT32_TO_BE(quads);
    frame->version = GUINT32_TO_BE(MINIMUM_SUPPORTED_VERSION);
    frame->category = GUINT32_TO_BE(category);
    frame->command = GUINT32_TO_BE(command);
    frame->status = 0xffffffff;

    if (args != NULL) {
        int i;

        for (i = 0; i < arg_count; ++i)
            frame->params[i] = GUINT32_TO_BE(args[i]);
    }

    // Increment the sequence number for next transaction.
    g_mutex_lock(&priv->lock);
    frame->seqnum = GUINT32_TO_BE(priv->seqnum);
    *resp_seqnum = priv->seqnum + 1;
    priv->seqnum += 2;
    if (priv->seqnum > SND_EFW_TRANSACTION_USER_SEQNUM_MAX)
        priv->seqnum = 0;
    g_mutex_unlock(&priv->lock);

    len = write(priv->state.fd, (const void *)frame, length);
    g_free(frame);
    if (len != length) {
        if (len < 0)
            generate_alsa_firewire_syscall_error(error, errno, "write(%ld)", length);
        else
            generate_efw_protocol_error(error, HITAKI_EFW_PROTOCOL_ERROR_COMM_ERR);
        return FALSE;
    }

    return TRUE;
}

static void efw_protocol_iface_init(HitakiEfwProtocolInterface *iface)
{
    iface->request = snd_efw_request;
}

/**
 * hitaki_snd_efw_new:
 *
 * Instantiate [class@SndEfw] object and return it.
 *
 * Returns: an instance of [class@SndEfw].
 */
HitakiSndEfw *hitaki_snd_efw_new(void)
{
    return g_object_new(HITAKI_TYPE_SND_EFW, NULL);
}
