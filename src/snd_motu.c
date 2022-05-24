// SPDX-License-Identifier: LGPL-3.0-or-later
#include "alsa_firewire_private.h"

/**
 * HitakiSndMotu:
 * A GObject-derived object for sound unit of MOTU FireWire series.
 *
 * The [class@SndMotu] is an object class derived from [class@GObject.Object] with protocol
 * implementation for Mark of the Unicorn (MOTU) FireWire series supported by ALSA firewire-motu
 * driver (`snd-firewire-motu`).
 */

typedef struct {
    struct alsa_firewire_state state;
} HitakiSndMotuPrivate;

static void alsa_firewire_iface_init(HitakiAlsaFirewireInterface *iface);
static void quadlet_notification_iface_init(HitakiQuadletNotificationInterface *iface);
static void motu_register_dsp_iface_init(HitakiMotuRegisterDspInterface *iface);
static void motu_command_dsp_iface_init(HitakiMotuCommandDspInterface *iface);

G_DEFINE_TYPE_WITH_CODE(HitakiSndMotu, hitaki_snd_motu, G_TYPE_OBJECT,
                        G_ADD_PRIVATE(HitakiSndMotu)
                        G_IMPLEMENT_INTERFACE(HITAKI_TYPE_ALSA_FIREWIRE, alsa_firewire_iface_init)
                        G_IMPLEMENT_INTERFACE(HITAKI_TYPE_QUADLET_NOTIFICATION,
                                              quadlet_notification_iface_init)
                        G_IMPLEMENT_INTERFACE(HITAKI_TYPE_MOTU_REGISTER_DSP,
                                              motu_register_dsp_iface_init)
                        G_IMPLEMENT_INTERFACE(HITAKI_TYPE_MOTU_COMMAND_DSP,
                                              motu_command_dsp_iface_init));

static void snd_motu_set_property(GObject *inst, guint id, const GValue *val, GParamSpec *spec)
{
    HitakiSndMotu *self = HITAKI_SND_MOTU(inst);
    HitakiSndMotuPrivate *priv = hitaki_snd_motu_get_instance_private(self);

    alsa_firewire_state_set_property(&priv->state, inst, id, val, spec);
}

static void snd_motu_get_property(GObject *inst, guint id, GValue *val, GParamSpec *spec)
{
    HitakiSndMotu *self = HITAKI_SND_MOTU(inst);
    HitakiSndMotuPrivate *priv = hitaki_snd_motu_get_instance_private(self);

    alsa_firewire_state_get_property(&priv->state, inst, id, val, spec);
}

static void snd_motu_finalize(GObject *obj)
{
    HitakiSndMotu *self = HITAKI_SND_MOTU(obj);
    HitakiSndMotuPrivate *priv = hitaki_snd_motu_get_instance_private(self);

    alsa_firewire_state_release(&priv->state);

    G_OBJECT_CLASS(hitaki_snd_motu_parent_class)->finalize(obj);
}

static void hitaki_snd_motu_class_init(HitakiSndMotuClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = snd_motu_set_property;
    gobject_class->get_property = snd_motu_get_property;
    gobject_class->finalize = snd_motu_finalize;

    alsa_firewire_class_override_properties(gobject_class);
}

static void hitaki_snd_motu_init(HitakiSndMotu *self)
{
    HitakiSndMotuPrivate *priv = hitaki_snd_motu_get_instance_private(self);

    alsa_firewire_state_init(&priv->state);
}

static gboolean snd_motu_open(HitakiAlsaFirewire *inst, const gchar *path, gint open_flag,
                              GError **error)
{
    HitakiSndMotu *self;
    HitakiSndMotuPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_MOTU(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_MOTU(inst);
    priv = hitaki_snd_motu_get_instance_private(self);

    if (!alsa_firewire_state_open(&priv->state, path, open_flag, error))
        return FALSE;

    if (priv->state.info.type != SNDRV_FIREWIRE_TYPE_MOTU) {
        generate_alsa_firewire_error(error, HITAKI_ALSA_FIREWIRE_ERROR_WRONG_CLASS);
        alsa_firewire_state_release(&priv->state);
        return FALSE;
    }

    return TRUE;
}

static gboolean snd_motu_lock(HitakiAlsaFirewire *inst, GError **error)
{
    HitakiSndMotu *self;
    HitakiSndMotuPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_MOTU(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_MOTU(inst);
    priv = hitaki_snd_motu_get_instance_private(self);

    return alsa_firewire_state_lock(&priv->state, error);
}

static gboolean snd_motu_unlock(HitakiAlsaFirewire *inst, GError **error)
{
    HitakiSndMotu *self;
    HitakiSndMotuPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_MOTU(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_MOTU(inst);
    priv = hitaki_snd_motu_get_instance_private(self);

    return alsa_firewire_state_unlock(&priv->state, error);
}

static void handle_event(HitakiAlsaFirewire *inst, const union snd_firewire_event *event,
                         size_t length)
{
    if (event->common.type == SNDRV_FIREWIRE_EVENT_MOTU_NOTIFICATION) {
        g_signal_emit_by_name(inst, "notified", event->motu_notification.message);
    } else if (event->common.type == SNDRV_FIREWIRE_EVENT_MOTU_REGISTER_DSP_CHANGE) {
        const struct snd_firewire_event_motu_register_dsp_change *ev;
        unsigned int count;

        ev = &event->motu_register_dsp_change;
        length -= sizeof(ev->type) + sizeof(ev->count);
        count = MIN(length / sizeof(*ev->changes), ev->count);

        g_signal_emit_by_name(inst, "changed", &ev->changes, count);
    }
}

static gboolean snd_motu_create_source(HitakiAlsaFirewire *inst, GSource **source, GError **error)
{
    HitakiSndMotu *self;
    HitakiSndMotuPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_MOTU(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_MOTU(inst);
    priv = hitaki_snd_motu_get_instance_private(self);

    return alsa_firewire_state_create_source(&priv->state, inst, handle_event, source, error);
}

static void alsa_firewire_iface_init(HitakiAlsaFirewireInterface *iface)
{
    iface->open = snd_motu_open;
    iface->lock = snd_motu_lock;
    iface->unlock = snd_motu_unlock;
    iface->create_source = snd_motu_create_source;
}

static void quadlet_notification_iface_init(HitakiQuadletNotificationInterface *iface)
{
    return;
}

static gboolean operate_ioctl(HitakiSndMotu *self, unsigned long request, void *arg,
                              const char *request_label, GError **error)
{
    HitakiSndMotuPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_MOTU(self), FALSE);
    g_return_val_if_fail(arg != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    priv = hitaki_snd_motu_get_instance_private(self);
    if (priv->state.fd < 0) {
        generate_alsa_firewire_error(error, HITAKI_ALSA_FIREWIRE_ERROR_IS_NOT_OPENED);
        return FALSE;
    }

    if (ioctl(priv->state.fd, request, arg) < 0) {
        generate_alsa_firewire_syscall_error(error, errno, "ioctl(%s)", request_label);
        return FALSE;
    }

    return TRUE;
}

static gboolean snd_motu_register_dsp_read_parameter(HitakiMotuRegisterDsp *inst,
                                HitakiSndMotuRegisterDspParameter *const *param, GError **error)
{
    HitakiSndMotu *self;

    g_return_val_if_fail(HITAKI_IS_SND_MOTU(inst), FALSE);
    g_return_val_if_fail(param != NULL && *param != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_MOTU(inst);

    return operate_ioctl(self, SNDRV_FIREWIRE_IOCTL_MOTU_REGISTER_DSP_PARAMETER, (void *)*param,
                         "SNDRV_FIREWIRE_IOCTL_MOTU_REGISTER_DSP_PARAMETER", error);
}

static gboolean snd_motu_register_dsp_read_byte_meter(HitakiMotuRegisterDsp *inst,
                                                 guint8 *const meter[48], GError **error)
{
    HitakiSndMotu *self;

    g_return_val_if_fail(HITAKI_IS_SND_MOTU(inst), FALSE);
    g_return_val_if_fail(meter != NULL && *meter != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_MOTU(inst);

    return operate_ioctl(self, SNDRV_FIREWIRE_IOCTL_MOTU_REGISTER_DSP_METER, (void *)*meter,
                         "SNDRV_FIREWIRE_IOCTL_MOTU_REGISTER_DSP_METER", error);
}

static void motu_register_dsp_iface_init(HitakiMotuRegisterDspInterface *iface)
{
    iface->read_parameter = snd_motu_register_dsp_read_parameter;
    iface->read_byte_meter = snd_motu_register_dsp_read_byte_meter;
}

static gboolean snd_motu_command_dsp_read_float_meter(HitakiMotuCommandDsp *inst,
                                                      gfloat *const meter[400], GError **error)
{
    HitakiSndMotu *self;

    g_return_val_if_fail(HITAKI_IS_SND_MOTU(inst), FALSE);
    g_return_val_if_fail(meter != NULL && *meter != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_MOTU(inst);

    return operate_ioctl(self, SNDRV_FIREWIRE_IOCTL_MOTU_COMMAND_DSP_METER, (void *)*meter,
                         "SNDRV_FIREWIRE_IOCTL_MOTU_COMMAND_DSP_METER", error);
}

static void motu_command_dsp_iface_init(HitakiMotuCommandDspInterface *iface)
{
    iface->read_float_meter = snd_motu_command_dsp_read_float_meter;
}

/**
 * hitaki_snd_motu_new:
 *
 * Instantiate [class@SndMotu] object and return the instance.
 *
 * Returns: an instance of [class@SndMotu].
 */
HitakiSndMotu *hitaki_snd_motu_new(void)
{
    return g_object_new(HITAKI_TYPE_SND_MOTU, NULL);
}
