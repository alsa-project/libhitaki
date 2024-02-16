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

static void handle_event(HitakiAlsaFirewire *inst, const union snd_firewire_event *event,
                         size_t length)
{
    HitakiSndEfw *self;

    const __be32 *buf;

    g_return_if_fail(HITAKI_IS_SND_EFW(inst));
    self = HITAKI_SND_EFW(inst);

    length -= sizeof(event->efw_response.type);
    buf = event->efw_response.response;

    // MEMO: Old version of ALSA fireworks driver has a bug to report the reading size shorter than
    // expected by 4 bytes, while the driver ensure copying the content per each response. This is a
    // workaround.
    length += 4;

    hitaki_efw_protocol_receive_response(HITAKI_EFW_PROTOCOL(self), (const guint8 *)buf, length);
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

static gboolean snd_efw_transmit_request(HitakiEfwProtocol *inst, const guint8 *buffer,
                                         gsize length, GError **error)
{
    HitakiSndEfw *self;
    HitakiSndEfwPrivate *priv;

    ssize_t len;

    g_return_val_if_fail(HITAKI_IS_SND_EFW(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_EFW(inst);
    priv = hitaki_snd_efw_get_instance_private(self);

    len = write(priv->state.fd, (const void *)buffer, length);
    if (len != length) {
        if (len < 0)
            generate_alsa_firewire_syscall_error(error, errno, "write(%ld)", length);
        else
            generate_efw_protocol_error(error, HITAKI_EFW_PROTOCOL_ERROR_COMM_ERR);
        return FALSE;
    }

    return TRUE;
}

static void snd_efw_get_seqnum(HitakiEfwProtocol *inst, guint32 *seqnum)
{
    HitakiSndEfw *self;
    HitakiSndEfwPrivate *priv;

    self = HITAKI_SND_EFW(inst);
    priv = hitaki_snd_efw_get_instance_private(self);

    // Increment the sequence number for next transaction.
    g_mutex_lock(&priv->lock);
    *seqnum = priv->seqnum;
    priv->seqnum += 2;
    if (priv->seqnum > SND_EFW_TRANSACTION_USER_SEQNUM_MAX)
        priv->seqnum = 0;
    g_mutex_unlock(&priv->lock);
}

static void efw_protocol_iface_init(HitakiEfwProtocolInterface *iface)
{
    iface->transmit_request = snd_efw_transmit_request;
    iface->get_seqnum = snd_efw_get_seqnum;
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
