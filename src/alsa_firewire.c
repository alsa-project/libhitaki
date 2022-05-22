// SPDX-License-Identifier: LGPL-2.1-or-later
#include "alsa_firewire_private.h"

/**
 * HitakiAlsaFirewire:
 * An interface to operate ALSA HwDep character device for Audio and Music unit in IEEE 1394 bus.
 *
 * All of driver in ALSA firewire stack allow userspace application to use specific function via
 * ALSA HwDep character device. The function includes common and specific parts. The
 * [iface@AlsaFirewire] is an object interface to operate the character device for the common
 * functions.
 */
G_DEFINE_INTERFACE(HitakiAlsaFirewire, hitaki_alsa_firewire, G_TYPE_OBJECT)

/**
 * hitaki_alsa_firewire_error_quark:
 *
 * Return the [alias@GLib.Quark] for [struct@GLib.Error] with code in [enum@AlsaFirewireError]
 * enumerations.
 *
 * Returns: A [alias@GLib.Quark].
 */
G_DEFINE_QUARK(hitaki-alsa-firewire-error-quark, hitaki_alsa_firewire_error)

static void hitaki_alsa_firewire_default_init(HitakiAlsaFirewireInterface *iface)
{
    /**
     * HitakiAlsaFirewire:unit-type:
     *
     * The type of sound unit.
     */
    g_object_interface_install_property(iface,
        g_param_spec_enum(UNIT_TYPE_PROP_NAME, UNIT_TYPE_PROP_NAME,
                          "The type of sound unit",
                          HITAKI_TYPE_ALSA_FIREWIRE_TYPE,
                          HITAKI_ALSA_FIREWIRE_TYPE_DICE,
                          G_PARAM_READABLE));

    /**
     * HitakiAlsaFirewire:card-id:
     *
     * The numeric identifier for sound card.
     */
    g_object_interface_install_property(iface,
        g_param_spec_uint(CARD_ID_PROP_NAME, CARD_ID_PROP_NAME,
                          "The numeric identifier for sound card",
                          0, G_MAXUINT,
                          0,
                          G_PARAM_READABLE));

    /**
     * HitakiAlsaFirewire:node-device:
     *
     * The name of node device in Linux FireWire subsystem which owns the unit; e.g. `fw1`.
     */
    g_object_interface_install_property(iface,
        g_param_spec_string(NODE_DEVICE_PROP_NAME, NODE_DEVICE_PROP_NAME,
                            "The name of node device in Linux FireWire subsystem",
                            NULL,
                            G_PARAM_READABLE));

    /**
     * HitakiAlsaFirewire:is-locked:
     *
     * Whether the associated unit is locked or not to start packet streaming.
     */
    g_object_interface_install_property(iface,
        g_param_spec_boolean(IS_LOCKED_PROP_NAME, IS_LOCKED_PROP_NAME,
                             "Whether the associated unit is locked or not",
                             FALSE,
                             G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY));

    /**
     * HitakiAlsaFirewire:guid:
     *
     * Global unique identifier for the node in IEEE 1394 bus.
     */
    g_object_interface_install_property(iface,
        g_param_spec_uint64(GUID_PROP_NAME, GUID_PROP_NAME,
                            "Global unique identifier for the node in IEEE 1394 bus.",
                            0, G_MAXUINT64, 0,
                            G_PARAM_READABLE));

    /**
     * HitakiAlsaFirewire:is-disconnected:
     *
     * Whether the sound card is unavailable. It becomes FALSE when the sound card is removed or
     * driver is unbound to it. Then the owner of this object should call
     * [method@GObject.Object.unref] as quickly as possible to release ALSA hwdep character device.
     */
    g_object_interface_install_property(iface,
        g_param_spec_boolean(IS_DISCONNECTED_PROP_NAME, IS_DISCONNECTED_PROP_NAME,
                             "Whether the sound card is unavailable",
                             FALSE,
                             G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY));
}

/**
 * hitaki_alsa_firewire_open:
 * @self: A [iface@AlsaFirewire]
 * @path: A path to special file for ALSA HwDep character device.
 * @open_flag: The flag of `open(2)` system call. O_RDONLY is forced to fulfil internally.
 * @error: A [struct@GLib.Error].
 *
 * Open the special file for ALSA HwDep character device.
 *
 * Returns: TRUE if the overall operation finished successfully, else FALSE.
 */
gboolean hitaki_alsa_firewire_open(HitakiAlsaFirewire *self, const gchar *path, gint open_flag,
                                  GError **error)
{
    g_return_val_if_fail(HITAKI_IS_ALSA_FIREWIRE(self), FALSE);
    g_return_val_if_fail(path != NULL && strlen(path) > 0, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    return HITAKI_ALSA_FIREWIRE_GET_IFACE(self)->open(self, path, open_flag, error);
}

/**
 * hitaki_alsa_firewire_lock:
 * @self: A [iface@AlsaFirewire]
 * @error: A [struct@GLib.Error].
 *
 * Lock kernel driver bound to the associated ALSA HwDep character device so that it is prohibited
 * to start packet streaming.
 *
 * Returns: TRUE if the overall operation finished successfully, else FALSE.
 */
gboolean hitaki_alsa_firewire_lock(HitakiAlsaFirewire *self, GError **error)
{
    g_return_val_if_fail(HITAKI_IS_ALSA_FIREWIRE(self), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    return HITAKI_ALSA_FIREWIRE_GET_IFACE(self)->lock(self, error);
}

/**
 * hitaki_alsa_firewire_unlock:
 * @self: A [iface@AlsaFirewire]
 * @error: A [struct@GLib.Error].
 *
 * Unlock kernel driver bound to the associated ALSA HwDep character device so that it can start
 * packet streaming.
 *
 * Returns: TRUE if the overall operation finished successfully, else FALSE.
 */
gboolean hitaki_alsa_firewire_unlock(HitakiAlsaFirewire *self, GError **error)
{
    g_return_val_if_fail(HITAKI_IS_ALSA_FIREWIRE(self), FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    return HITAKI_ALSA_FIREWIRE_GET_IFACE(self)->unlock(self, error);
}

/**
 * hitaki_alsa_firewire_create_source:
 * @self: A [iface@AlsaFirewire].
 * @source: (out): A [struct@GLib.Source] to handle events from ALSA HwDep character device.
 * @error: A [struct@GLib.Error].
 *
 * Allocate [struct@GLib.Source]  to handle events from ALSA HwDep character device.
 *
 * Returns: TRUE if the overall operation finished successfully, else FALSE.
 */
gboolean hitaki_alsa_firewire_create_source(HitakiAlsaFirewire *self, GSource **source,
                                           GError **error)
{
    g_return_val_if_fail(HITAKI_IS_ALSA_FIREWIRE(self), FALSE);
    g_return_val_if_fail(source != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    return HITAKI_ALSA_FIREWIRE_GET_IFACE(self)->create_source(self, source, error);
}
