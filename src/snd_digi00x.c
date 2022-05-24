// SPDX-License-Identifier: LGPL-3.0-or-later
#include "alsa_firewire_private.h"

/**
 * HitakiSndDigi00x:
 * A GObject-derived object for sound unit of Digidesign Digi00x family.
 *
 * The [class@SndDigi00x] is an object class derived from [class@GObject.Object] for sound unit of
 * Digidesign Digi00x family supported by ALSA firewire-digi00x driver (`snd-firewire-digi00x`).
 */

typedef struct {
    struct alsa_firewire_state state;
} HitakiSndDigi00xPrivate;

static void alsa_firewire_iface_init(HitakiAlsaFirewireInterface *iface);
static void quadlet_notification_iface_init(HitakiQuadletNotificationInterface *iface);

G_DEFINE_TYPE_WITH_CODE(HitakiSndDigi00x, hitaki_snd_digi00x, G_TYPE_OBJECT,
                        G_ADD_PRIVATE(HitakiSndDigi00x)
                        G_IMPLEMENT_INTERFACE(HITAKI_TYPE_ALSA_FIREWIRE, alsa_firewire_iface_init)
                        G_IMPLEMENT_INTERFACE(HITAKI_TYPE_QUADLET_NOTIFICATION,
                                              quadlet_notification_iface_init));

static void snd_digi00x_set_property(GObject *inst, guint id, const GValue *val, GParamSpec *spec)
{
    HitakiSndDigi00x *self = HITAKI_SND_DIGI00X(inst);
    HitakiSndDigi00xPrivate *priv = hitaki_snd_digi00x_get_instance_private(self);

    alsa_firewire_state_set_property(&priv->state, inst, id, val, spec);
}

static void snd_digi00x_get_property(GObject *inst, guint id, GValue *val, GParamSpec *spec)
{
    HitakiSndDigi00x *self = HITAKI_SND_DIGI00X(inst);
    HitakiSndDigi00xPrivate *priv = hitaki_snd_digi00x_get_instance_private(self);

    alsa_firewire_state_get_property(&priv->state, inst, id, val, spec);
}

static void snd_digi00x_finalize(GObject *obj)
{
    HitakiSndDigi00x *self = HITAKI_SND_DIGI00X(obj);
    HitakiSndDigi00xPrivate *priv = hitaki_snd_digi00x_get_instance_private(self);

    alsa_firewire_state_release(&priv->state);

    G_OBJECT_CLASS(hitaki_snd_digi00x_parent_class)->finalize(obj);
}

static void hitaki_snd_digi00x_class_init(HitakiSndDigi00xClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = snd_digi00x_set_property;
    gobject_class->get_property = snd_digi00x_get_property;
    gobject_class->finalize = snd_digi00x_finalize;

    alsa_firewire_class_override_properties(gobject_class);
}

static void hitaki_snd_digi00x_init(HitakiSndDigi00x *self)
{
    HitakiSndDigi00xPrivate *priv = hitaki_snd_digi00x_get_instance_private(self);

    alsa_firewire_state_init(&priv->state);
}

static gboolean snd_digi00x_open(HitakiAlsaFirewire *inst, const gchar *path, gint open_flag,
                                 GError **error)
{
    HitakiSndDigi00x *self;
    HitakiSndDigi00xPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_DIGI00X(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_DIGI00X(inst);
    priv = hitaki_snd_digi00x_get_instance_private(self);

    if (!alsa_firewire_state_open(&priv->state, path, open_flag, error))
        return FALSE;

    if (priv->state.info.type != SNDRV_FIREWIRE_TYPE_DIGI00X) {
        generate_alsa_firewire_error(error, HITAKI_ALSA_FIREWIRE_ERROR_WRONG_CLASS);
        alsa_firewire_state_release(&priv->state);
        return FALSE;
    }

    return TRUE;
}

static gboolean snd_digi00x_lock(HitakiAlsaFirewire *inst, GError **error)
{
    HitakiSndDigi00x *self;
    HitakiSndDigi00xPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_DIGI00X(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_DIGI00X(inst);
    priv = hitaki_snd_digi00x_get_instance_private(self);

    return alsa_firewire_state_lock(&priv->state, error);
}

static gboolean snd_digi00x_unlock(HitakiAlsaFirewire *inst, GError **error)
{
    HitakiSndDigi00x *self;
    HitakiSndDigi00xPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_DIGI00X(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_DIGI00X(inst);
    priv = hitaki_snd_digi00x_get_instance_private(self);

    return alsa_firewire_state_unlock(&priv->state, error);
}

static void handle_event(HitakiAlsaFirewire *inst, const union snd_firewire_event *event,
                         size_t length)
{
    if (event->common.type == SNDRV_FIREWIRE_EVENT_DIGI00X_MESSAGE)
        g_signal_emit_by_name(inst, "notified", event->digi00x_message.message);
}

static gboolean snd_digi00x_create_source(HitakiAlsaFirewire *inst, GSource **source, GError **error)
{
    HitakiSndDigi00x *self;
    HitakiSndDigi00xPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_DIGI00X(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_DIGI00X(inst);
    priv = hitaki_snd_digi00x_get_instance_private(self);

    return alsa_firewire_state_create_source(&priv->state, inst, handle_event, source, error);
}

static void alsa_firewire_iface_init(HitakiAlsaFirewireInterface *iface)
{
    iface->open = snd_digi00x_open;
    iface->lock = snd_digi00x_lock;
    iface->unlock = snd_digi00x_unlock;
    iface->create_source = snd_digi00x_create_source;
}

static void quadlet_notification_iface_init(HitakiQuadletNotificationInterface *iface)
{
    return;
}

/**
 * hitaki_snd_digi00x_new:
 *
 * Instantiate [class@SndDigi00x] object and return the instance.
 *
 * Returns: an instance of [class@SndDigi00x].
 */
HitakiSndDigi00x *hitaki_snd_digi00x_new(void)
{
    return g_object_new(HITAKI_TYPE_SND_DIGI00X, NULL);
}
