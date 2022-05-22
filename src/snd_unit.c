// SPDX-License-Identifier: LGPL-2.1-or-later
#include "alsa_firewire_private.h"

/**
 * HitakiSndUnit:
 * A GObject-derived object for sound unit without specific function.
 *
 * The [class@SndUnit] is an object class derived from [class@GObject.Object] for sound unit with
 * common functions supported by any driver in ALSA firewire stack.
 */

typedef struct {
    struct alsa_firewire_state state;
} HitakiSndUnitPrivate;

static void alsa_firewire_iface_init(HitakiAlsaFirewireInterface *iface);

G_DEFINE_TYPE_WITH_CODE(HitakiSndUnit, hitaki_snd_unit, G_TYPE_OBJECT,
                        G_ADD_PRIVATE(HitakiSndUnit)
                        G_IMPLEMENT_INTERFACE(HITAKI_TYPE_ALSA_FIREWIRE, alsa_firewire_iface_init));

static void snd_unit_set_property(GObject *inst, guint id, const GValue *val, GParamSpec *spec)
{
    HitakiSndUnit *self = HITAKI_SND_UNIT(inst);
    HitakiSndUnitPrivate *priv = hitaki_snd_unit_get_instance_private(self);

    alsa_firewire_state_set_property(&priv->state, inst, id, val, spec);
}

static void snd_unit_get_property(GObject *inst, guint id, GValue *val, GParamSpec *spec)
{
    HitakiSndUnit *self = HITAKI_SND_UNIT(inst);
    HitakiSndUnitPrivate *priv = hitaki_snd_unit_get_instance_private(self);

    alsa_firewire_state_get_property(&priv->state, inst, id, val, spec);
}

static void snd_unit_finalize(GObject *obj)
{
    HitakiSndUnit *self = HITAKI_SND_UNIT(obj);
    HitakiSndUnitPrivate *priv = hitaki_snd_unit_get_instance_private(self);

    alsa_firewire_state_release(&priv->state);

    G_OBJECT_CLASS(hitaki_snd_unit_parent_class)->finalize(obj);
}

static void hitaki_snd_unit_class_init(HitakiSndUnitClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = snd_unit_set_property;
    gobject_class->get_property = snd_unit_get_property;
    gobject_class->finalize = snd_unit_finalize;

    alsa_firewire_class_override_properties(gobject_class);
}

static void hitaki_snd_unit_init(HitakiSndUnit *self)
{
    HitakiSndUnitPrivate *priv = hitaki_snd_unit_get_instance_private(self);

    alsa_firewire_state_init(&priv->state);
}

static gboolean snd_unit_open(HitakiAlsaFirewire *inst, const gchar *path, gint open_flag,
                              GError **error)
{
    HitakiSndUnit *self;
    HitakiSndUnitPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_UNIT(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_UNIT(inst);
    priv = hitaki_snd_unit_get_instance_private(self);

    return alsa_firewire_state_open(&priv->state, path, open_flag, error);
}

static gboolean snd_unit_lock(HitakiAlsaFirewire *inst, GError **error)
{
    HitakiSndUnit *self;
    HitakiSndUnitPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_UNIT(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_UNIT(inst);
    priv = hitaki_snd_unit_get_instance_private(self);

    return alsa_firewire_state_lock(&priv->state, error);
}

static gboolean snd_unit_unlock(HitakiAlsaFirewire *inst, GError **error)
{
    HitakiSndUnit *self;
    HitakiSndUnitPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_UNIT(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_UNIT(inst);
    priv = hitaki_snd_unit_get_instance_private(self);

    return alsa_firewire_state_unlock(&priv->state, error);
}

static void handle_event(HitakiAlsaFirewire *inst, union snd_firewire_event *event, size_t length)
{
    // Nothing specific.
    return;
}

static gboolean snd_unit_create_source(HitakiAlsaFirewire *inst, GSource **source, GError **error)
{
    HitakiSndUnit *self;
    HitakiSndUnitPrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_UNIT(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_UNIT(inst);
    priv = hitaki_snd_unit_get_instance_private(self);

    return alsa_firewire_state_create_source(&priv->state, inst, handle_event, source, error);
}

static void alsa_firewire_iface_init(HitakiAlsaFirewireInterface *iface)
{
    iface->open = snd_unit_open;
    iface->lock = snd_unit_lock;
    iface->unlock = snd_unit_unlock;
    iface->create_source = snd_unit_create_source;
}

/**
 * hitaki_snd_unit_new:
 *
 * Instantiate [class@SndUnit] object and return the instance.
 *
 * Returns: an instance of [class@SndUnit].
 */
HitakiSndUnit *hitaki_snd_unit_new(void)
{
    return g_object_new(HITAKI_TYPE_SND_UNIT, NULL);
}
