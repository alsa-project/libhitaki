// SPDX-License-Identifier: LGPL-2.1-or-later
#include "alsa_firewire_private.h"
#include "tascam_protocol.h"

/**
 * HitakiSndTascam:
 * A GObject-derived object for sound unit of TASCAM FireWire series.
 *
 * The [class@SndTascam] is an object class derived from [class@GObject.Object] with protocol
 * implementation for TASCAM FireWire series supported by ALSA firewire-tascam driver
 * (`snd-firewire-tascam`). The image of state consists of 64 quadlets according to
 * `SNDRV_FIREWIRE_TASCAM_STATE_COUNT` in UAPI of ALSA firewire stack.
 */

typedef struct {
    struct alsa_firewire_state state;

    struct snd_firewire_tascam_state image;
} HitakiSndTascamPrivate;

static void alsa_firewire_iface_init(HitakiAlsaFirewireInterface *iface);
static void tascam_protocol_iface_init(HitakiTascamProtocolInterface *iface);

G_DEFINE_TYPE_WITH_CODE(HitakiSndTascam, hitaki_snd_tascam, G_TYPE_OBJECT,
                        G_ADD_PRIVATE(HitakiSndTascam)
                        G_IMPLEMENT_INTERFACE(HITAKI_TYPE_ALSA_FIREWIRE, alsa_firewire_iface_init)
                        G_IMPLEMENT_INTERFACE(HITAKI_TYPE_TASCAM_PROTOCOL, tascam_protocol_iface_init));

static void snd_tascam_set_property(GObject *inst, guint id, const GValue *val, GParamSpec *spec)
{
    HitakiSndTascam *self = HITAKI_SND_TASCAM(inst);
    HitakiSndTascamPrivate *priv = hitaki_snd_tascam_get_instance_private(self);

    alsa_firewire_state_set_property(&priv->state, inst, id, val, spec);
}

static void snd_tascam_get_property(GObject *inst, guint id, GValue *val, GParamSpec *spec)
{
    HitakiSndTascam *self = HITAKI_SND_TASCAM(inst);
    HitakiSndTascamPrivate *priv = hitaki_snd_tascam_get_instance_private(self);

    alsa_firewire_state_get_property(&priv->state, inst, id, val, spec);
}

static void snd_tascam_finalize(GObject *obj)
{
    HitakiSndTascam *self = HITAKI_SND_TASCAM(obj);
    HitakiSndTascamPrivate *priv = hitaki_snd_tascam_get_instance_private(self);

    alsa_firewire_state_release(&priv->state);

    G_OBJECT_CLASS(hitaki_snd_tascam_parent_class)->finalize(obj);
}

static void hitaki_snd_tascam_class_init(HitakiSndTascamClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = snd_tascam_set_property;
    gobject_class->get_property = snd_tascam_get_property;
    gobject_class->finalize = snd_tascam_finalize;

    alsa_firewire_class_override_properties(gobject_class);
}

static void hitaki_snd_tascam_init(HitakiSndTascam *self)
{
    HitakiSndTascamPrivate *priv = hitaki_snd_tascam_get_instance_private(self);

    alsa_firewire_state_init(&priv->state);
}

static gboolean snd_tascam_open(HitakiAlsaFirewire *inst, const gchar *path, gint open_flag,
                              GError **error)
{
    HitakiSndTascam *self;
    HitakiSndTascamPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_TASCAM(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_TASCAM(inst);
    priv = hitaki_snd_tascam_get_instance_private(self);

    if (!alsa_firewire_state_open(&priv->state, path, open_flag, error))
        return FALSE;

    if (priv->state.info.type != SNDRV_FIREWIRE_TYPE_TASCAM) {
        generate_alsa_firewire_error(error, HITAKI_ALSA_FIREWIRE_ERROR_WRONG_CLASS);
        alsa_firewire_state_release(&priv->state);
        return FALSE;
    }

    return TRUE;
}

static gboolean snd_tascam_lock(HitakiAlsaFirewire *inst, GError **error)
{
    HitakiSndTascam *self;
    HitakiSndTascamPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_TASCAM(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_TASCAM(inst);
    priv = hitaki_snd_tascam_get_instance_private(self);

    return alsa_firewire_state_lock(&priv->state, error);
}

static gboolean snd_tascam_unlock(HitakiAlsaFirewire *inst, GError **error)
{
    HitakiSndTascam *self;
    HitakiSndTascamPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_TASCAM(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_TASCAM(inst);
    priv = hitaki_snd_tascam_get_instance_private(self);

    return alsa_firewire_state_unlock(&priv->state, error);
}

static void handle_event(HitakiAlsaFirewire *inst, const union snd_firewire_event *event,
                         size_t length)
{
    if (event->common.type == SNDRV_FIREWIRE_EVENT_TASCAM_CONTROL) {
        const struct snd_firewire_event_tascam_control *ev = &event->tascam_control;
        const struct snd_firewire_tascam_change *change = ev->changes;

        length -= sizeof(ev->type);

        while (length >= sizeof(*change)) {
            g_signal_emit_by_name(inst, "changed",
                                  change->index,
                                  GUINT32_FROM_BE(change->before),
                                  GUINT32_FROM_BE(change->after));
            ++change;
            length -= sizeof(*change);
        }
    }
}

static gboolean snd_tascam_create_source(HitakiAlsaFirewire *inst, GSource **source, GError **error)
{
    HitakiSndTascam *self;
    HitakiSndTascamPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_TASCAM(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_TASCAM(inst);
    priv = hitaki_snd_tascam_get_instance_private(self);

    return alsa_firewire_state_create_source(&priv->state, inst, handle_event, source, error);
}

static void alsa_firewire_iface_init(HitakiAlsaFirewireInterface *iface)
{
    iface->open = snd_tascam_open;
    iface->lock = snd_tascam_lock;
    iface->unlock = snd_tascam_unlock;
    iface->create_source = snd_tascam_create_source;
}

static gboolean snd_tascam_read_state(HitakiTascamProtocol *inst, guint32 *const *state,
                                      gsize *count, GError **error)
{
    HitakiSndTascam *self;
    HitakiSndTascamPrivate *priv;
    int i;

    g_return_val_if_fail(HITAKI_IS_SND_TASCAM(inst), FALSE);
    g_return_val_if_fail(state != NULL && *state != NULL, FALSE);
    g_return_val_if_fail(count != NULL && *count >= SNDRV_FIREWIRE_TASCAM_STATE_COUNT, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_TASCAM(inst);
    priv = hitaki_snd_tascam_get_instance_private(self);
    if (priv->state.fd < 0) {
        generate_alsa_firewire_error(error, HITAKI_ALSA_FIREWIRE_ERROR_IS_NOT_OPENED);
        return FALSE;
    }

    if (ioctl(priv->state.fd, SNDRV_FIREWIRE_IOCTL_TASCAM_STATE, &priv->image) < 0) {
        generate_alsa_firewire_syscall_error(error, errno, "ioctl(%s)", "TASCAM_STATE");
        return FALSE;
    }

    for (i = 0; i < SNDRV_FIREWIRE_TASCAM_STATE_COUNT; ++i)
        (*state)[i] = GUINT32_FROM_BE(priv->image.data[i]);
    *count = SNDRV_FIREWIRE_TASCAM_STATE_COUNT;

    return TRUE;
}

static void tascam_protocol_iface_init(HitakiTascamProtocolInterface *iface)
{
    iface->read_state = snd_tascam_read_state;
}

/**
 * hitaki_snd_tascam_new:
 *
 * Instantiate [class@SndTascam] object and return the instance.
 *
 * Returns: an instance of [class@SndTascam].
 */
HitakiSndTascam *hitaki_snd_tascam_new(void)
{
    return g_object_new(HITAKI_TYPE_SND_TASCAM, NULL);
}
