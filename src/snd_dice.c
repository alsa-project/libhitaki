// SPDX-License-Identifier: LGPL-2.1-or-later
#include "alsa_firewire_private.h"

/**
 * HitakiSndDice:
 * A GObject-derived object for sound unit of TCAT DICE ASICs.
 *
 * The [class@SndDice] is object class derived from [class@GObject.Object] for sound unit of TCAT
 * DICE ASICs supported by ALSA dice driver (`snd-dice`).
 */

typedef struct {
    struct alsa_firewire_state state;
} HitakiSndDicePrivate;

static void alsa_firewire_iface_init(HitakiAlsaFirewireInterface *iface);
static void quadlet_notification_iface_init(HitakiQuadletNotificationInterface *iface);

G_DEFINE_TYPE_WITH_CODE(HitakiSndDice, hitaki_snd_dice, G_TYPE_OBJECT,
                        G_ADD_PRIVATE(HitakiSndDice)
                        G_IMPLEMENT_INTERFACE(HITAKI_TYPE_ALSA_FIREWIRE, alsa_firewire_iface_init)
                        G_IMPLEMENT_INTERFACE(HITAKI_TYPE_QUADLET_NOTIFICATION,
                                              quadlet_notification_iface_init));

static void snd_dice_set_property(GObject *inst, guint id, const GValue *val, GParamSpec *spec)
{
    HitakiSndDice *self = HITAKI_SND_DICE(inst);
    HitakiSndDicePrivate *priv = hitaki_snd_dice_get_instance_private(self);

    alsa_firewire_state_set_property(&priv->state, inst, id, val, spec);
}

static void snd_dice_get_property(GObject *inst, guint id, GValue *val, GParamSpec *spec)
{
    HitakiSndDice *self = HITAKI_SND_DICE(inst);
    HitakiSndDicePrivate *priv = hitaki_snd_dice_get_instance_private(self);

    alsa_firewire_state_get_property(&priv->state, inst, id, val, spec);
}

static void snd_dice_finalize(GObject *obj)
{
    HitakiSndDice *self = HITAKI_SND_DICE(obj);
    HitakiSndDicePrivate *priv = hitaki_snd_dice_get_instance_private(self);

    alsa_firewire_state_release(&priv->state);

    G_OBJECT_CLASS(hitaki_snd_dice_parent_class)->finalize(obj);
}

static void hitaki_snd_dice_class_init(HitakiSndDiceClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = snd_dice_set_property;
    gobject_class->get_property = snd_dice_get_property;
    gobject_class->finalize = snd_dice_finalize;

    alsa_firewire_class_override_properties(gobject_class);
}

static void hitaki_snd_dice_init(HitakiSndDice *self)
{
    HitakiSndDicePrivate *priv = hitaki_snd_dice_get_instance_private(self);

    alsa_firewire_state_init(&priv->state);
}

static gboolean snd_dice_open(HitakiAlsaFirewire *inst, const gchar *path, gint open_flag,
                              GError **error)
{
    HitakiSndDice *self;
    HitakiSndDicePrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_DICE(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_DICE(inst);
    priv = hitaki_snd_dice_get_instance_private(self);

    if (!alsa_firewire_state_open(&priv->state, path, open_flag, error))
        return FALSE;

    if (priv->state.info.type != SNDRV_FIREWIRE_TYPE_DICE) {
        generate_alsa_firewire_error(error, HITAKI_ALSA_FIREWIRE_ERROR_WRONG_CLASS);
        alsa_firewire_state_release(&priv->state);
        return FALSE;
    }

    return TRUE;
}

static gboolean snd_dice_lock(HitakiAlsaFirewire *inst, GError **error)
{
    HitakiSndDice *self;
    HitakiSndDicePrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_DICE(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_DICE(inst);
    priv = hitaki_snd_dice_get_instance_private(self);

    return alsa_firewire_state_lock(&priv->state, error);
}

static gboolean snd_dice_unlock(HitakiAlsaFirewire *inst, GError **error)
{
    HitakiSndDice *self;
    HitakiSndDicePrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_DICE(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_DICE(inst);
    priv = hitaki_snd_dice_get_instance_private(self);

    return alsa_firewire_state_unlock(&priv->state, error);
}

static void handle_event(HitakiAlsaFirewire *inst, const union snd_firewire_event *event,
                         size_t length)
{
    if (event->common.type == SNDRV_FIREWIRE_EVENT_DICE_NOTIFICATION)
        g_signal_emit_by_name(inst, "notified", event->dice_notification.notification);
}

static gboolean snd_dice_create_source(HitakiAlsaFirewire *inst, GSource **source, GError **error)
{
    HitakiSndDice *self;
    HitakiSndDicePrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_DICE(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_DICE(inst);
    priv = hitaki_snd_dice_get_instance_private(self);

    return alsa_firewire_state_create_source(&priv->state, inst, handle_event, source, error);
}

static void alsa_firewire_iface_init(HitakiAlsaFirewireInterface *iface)
{
    iface->open = snd_dice_open;
    iface->lock = snd_dice_lock;
    iface->unlock = snd_dice_unlock;
    iface->create_source = snd_dice_create_source;
}

static void quadlet_notification_iface_init(HitakiQuadletNotificationInterface *iface)
{
    return;
}

/**
 * hitaki_snd_dice_new:
 *
 * Instantiate [class@SndDice] object and return the instance.
 *
 * Returns: an instance of [class@SndDice].
 */
HitakiSndDice *hitaki_snd_dice_new(void)
{
    return g_object_new(HITAKI_TYPE_SND_DICE, NULL);
}
