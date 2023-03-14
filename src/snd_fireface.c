// SPDX-License-Identifier: LGPL-3.0-or-later
#include "alsa_firewire_private.h"

/**
 * HitakiSndFireface:
 * A GObject-derived object for sound unit of RME Fireface series.
 *
 * The [class@SndFireface] is object class derived from [class@GObject.Object] for sound unit of
 * RME Fireface series supported by ALSA fireface driver (`snd-fireface`).
 *
 * For Fireface 400, the object class implements [iface@TimestampedQuadletNotification] interface
 * to dispatch event of knob control. The message parameter of event includes unsigned 32 bit
 * integer value with encoded data. The kind of data can be detected by below flags:
 *
 * - `0x00000100`: MIDI message byte received in physical MIDI port 0
 * - `0x01000000`: MIDI message byte received in physical MIDI port 1
 * - `0x04000000`: signal level of channel
 *
 * When it is for MIDI port 0, the value includes the message byte masked by `0x000000ff`. When it
 * is for MIDI port 1, the value includes the message byte masked by `0x00ff0000`.
 *
 * When it is for the signal level, the value includes paired channel position masked by
 * `0xf0000000`.
 *
 * - `0x00000000`: Microphone input 0/1
 * - `0x10000000`: Line input 0/1
 * - `0x20000000`: Line output 0/1
 * - `0x30000000`: Line output 2/3
 * - `0x40000000`: Line output 4/5
 * - `0x50000000`: Headphone output 0/1
 * - `0x60000000`: S/PDIF output 0/1
 * - `0x70000000`: ADAT output 0/1
 * - `0x80000000`: ADAT output 2/3
 * - `0x90000000`: ADAT output 4/5
 * - `0xa0000000`: ADAT output 6/7
 *
 * When the value includes flag of `0x02000000`, the value includes signal level for both of
 * channels in the pair. Unless, when the value includes flag of `0x08000000`, the signal level is
 * just for right channel in the pair. When both flags are not found, the signal level is just for
 * left channel in the pair.
 *
 * The value includes signal level masked by `0x00fffc00`. The range of signal level differs
 * depending on channels. For signal level of microphone input:
 *
 * - `0x00000000`: zero
 * - `0x00002800`: +10.0 dB
 * - `0x00002c00`: +11.0 dB
 * - `0x00003000`: +12.0 dB
 * - ...
 * - `0x0000fc00`: +63.0 dB:
 * - `0x00010000`: +64.0 dB:
 * - `0x00010400`: +65.0 dB:
 *
 * For signal level of line input:
 *
 * - `0x00000000`: zero
 * - `0x00000400`: +0.5 dB
 * - `0x00000800`: +1.0 dB
 * - `0x00000c00`: +1.5 dB
 * - ...
 * - `0x00008800`: +17.0 dB:
 * - `0x000008c0`: +17.5 dB:
 * - `0x00000900`: +18.0 dB:
 *
 * For signal level of any type of output:
 *
 * - `0x0000fc00`: -infinite
 * - `0x0000f800`: -58.0 dB
 * - `0x0000f400`: -56.0 dB
 * - `0x0000f000`: -54.0 dB
 * - `0x0000ec00`: -53.0 dB
 * - `0x0000e800`: -52.0 dB
 * - ...
 * - `0x00001c00`: -1.0 dB
 * - `0x00001800`: 0.0 dB
 * - `0x00001400`: 1.0 dB
 * - ...
 * - `0x00000800`: +4.0 dB
 * - `0x00000400`: +5.0 dB
 * - `0x00000000`: +6.0 dB
 */

typedef struct {
    struct alsa_firewire_state state;
} HitakiSndFirefacePrivate;

static void alsa_firewire_iface_init(HitakiAlsaFirewireInterface *iface);
static void timestamped_quadlet_notification_iface_init(HitakiTimestampedQuadletNotification *iface);

G_DEFINE_TYPE_WITH_CODE(HitakiSndFireface, hitaki_snd_fireface, G_TYPE_OBJECT,
                        G_ADD_PRIVATE(HitakiSndFireface)
                        G_IMPLEMENT_INTERFACE(HITAKI_TYPE_ALSA_FIREWIRE, alsa_firewire_iface_init)
                        G_IMPLEMENT_INTERFACE(HITAKI_TYPE_TIMESTAMPED_QUADLET_NOTIFICATION,
                                              timestamped_quadlet_notification_iface_init));

static void snd_fireface_set_property(GObject *inst, guint id, const GValue *val, GParamSpec *spec)
{
    HitakiSndFireface *self = HITAKI_SND_FIREFACE(inst);
    HitakiSndFirefacePrivate *priv = hitaki_snd_fireface_get_instance_private(self);

    alsa_firewire_state_set_property(&priv->state, inst, id, val, spec);
}

static void snd_fireface_get_property(GObject *inst, guint id, GValue *val, GParamSpec *spec)
{
    HitakiSndFireface *self = HITAKI_SND_FIREFACE(inst);
    HitakiSndFirefacePrivate *priv = hitaki_snd_fireface_get_instance_private(self);

    alsa_firewire_state_get_property(&priv->state, inst, id, val, spec);
}

static void snd_fireface_finalize(GObject *obj)
{
    HitakiSndFireface *self = HITAKI_SND_FIREFACE(obj);
    HitakiSndFirefacePrivate *priv = hitaki_snd_fireface_get_instance_private(self);

    alsa_firewire_state_release(&priv->state);

    G_OBJECT_CLASS(hitaki_snd_fireface_parent_class)->finalize(obj);
}

static void hitaki_snd_fireface_class_init(HitakiSndFirefaceClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = snd_fireface_set_property;
    gobject_class->get_property = snd_fireface_get_property;
    gobject_class->finalize = snd_fireface_finalize;

    alsa_firewire_class_override_properties(gobject_class);
}

static void hitaki_snd_fireface_init(HitakiSndFireface *self)
{
    HitakiSndFirefacePrivate *priv = hitaki_snd_fireface_get_instance_private(self);

    alsa_firewire_state_init(&priv->state);
}

static gboolean snd_fireface_open(HitakiAlsaFirewire *inst, const gchar *path, gint open_flag,
                                  GError **error)
{
    HitakiSndFireface *self;
    HitakiSndFirefacePrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_FIREFACE(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_FIREFACE(inst);
    priv = hitaki_snd_fireface_get_instance_private(self);

    if (!alsa_firewire_state_open(&priv->state, path, open_flag, error))
        return FALSE;

    if (priv->state.info.type != SNDRV_FIREWIRE_TYPE_FIREFACE) {
        generate_alsa_firewire_error(error, HITAKI_ALSA_FIREWIRE_ERROR_WRONG_CLASS);
        alsa_firewire_state_release(&priv->state);
        return FALSE;
    }

    return TRUE;
}

static gboolean snd_fireface_lock(HitakiAlsaFirewire *inst, GError **error)
{
    HitakiSndFireface *self;
    HitakiSndFirefacePrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_FIREFACE(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_FIREFACE(inst);
    priv = hitaki_snd_fireface_get_instance_private(self);

    return alsa_firewire_state_lock(&priv->state, error);
}

static gboolean snd_fireface_unlock(HitakiAlsaFirewire *inst, GError **error)
{
    HitakiSndFireface *self;
    HitakiSndFirefacePrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_FIREFACE(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_FIREFACE(inst);
    priv = hitaki_snd_fireface_get_instance_private(self);

    return alsa_firewire_state_unlock(&priv->state, error);
}

static void handle_event(HitakiAlsaFirewire *inst, const union snd_firewire_event *event,
                         size_t length)
{
    if (event->common.type == SNDRV_FIREWIRE_EVENT_FF400_MESSAGE) {
        const struct snd_firewire_event_ff400_message *ev = &event->ff400_message;
        int i;

        for (i = 0; i < ev->message_count; ++i)
            g_signal_emit_by_name(inst, "notified-at", ev->messages[i].message, ev->messages[i].tstamp);
    }
}

static gboolean snd_fireface_create_source(HitakiAlsaFirewire *inst, GSource **source, GError **error)
{
    HitakiSndFireface *self;
    HitakiSndFirefacePrivate *priv;

    g_return_val_if_fail(HITAKI_IS_SND_FIREFACE(inst), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    self = HITAKI_SND_FIREFACE(inst);
    priv = hitaki_snd_fireface_get_instance_private(self);

    return alsa_firewire_state_create_source(&priv->state, inst, handle_event, source, error);
}

static void alsa_firewire_iface_init(HitakiAlsaFirewireInterface *iface)
{
    iface->open = snd_fireface_open;
    iface->lock = snd_fireface_lock;
    iface->unlock = snd_fireface_unlock;
    iface->create_source = snd_fireface_create_source;
}

static void timestamped_quadlet_notification_iface_init(HitakiTimestampedQuadletNotification *iface)
{
    return;
}

/**
 * hitaki_snd_fireface_new:
 *
 * Instantiate [class@SndFireface] object and return the instance.
 *
 * Returns: an instance of [class@SndFireface].
 */
HitakiSndFireface *hitaki_snd_fireface_new(void)
{
    return g_object_new(HITAKI_TYPE_SND_FIREFACE, NULL);
}
