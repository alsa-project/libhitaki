// SPDX-License-Identifier: LGPL-2.1-or-later
#ifndef __HITAKI_ALSA_FIREWIRE_PRIVATE_H__
#define __HITAKI_ALSA_FIREWIRE_PRIVATE_H__

#include "hitaki.h"

#include <sound/firewire.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

extern const char *const alsa_firewire_error_msgs[7];

static inline void generate_alsa_firewire_error(GError **error, HitakiAlsaFirewireError code)
{
    g_set_error_literal(error, HITAKI_ALSA_FIREWIRE_ERROR, code, alsa_firewire_error_msgs[code]);
}

#define generate_alsa_firewire_syscall_error(error, errno, format, arg)                 \
    g_set_error(error, HITAKI_ALSA_FIREWIRE_ERROR, HITAKI_ALSA_FIREWIRE_ERROR_FAILED,   \
                format " %d(%s)", arg, errno, strerror(errno))

enum alsa_firewire_prop_type {
    ALSA_FIREWIRE_PROP_UNIT_TYPE = 1,
    ALSA_FIREWIRE_PROP_CARD_ID,
    ALSA_FIREWIRE_PROP_NODE_DEVICE,
    ALSA_FIREWIRE_PROP_IS_LOCKED,
    ALSA_FIREWIRE_PROP_GUID,
    ALSA_FIREWIRE_PROP_IS_DISCONNECTED,
    ALSA_FIREWIRE_PROP_COUNT,
};

#define UNIT_TYPE_PROP_NAME         "unit-type"
#define CARD_ID_PROP_NAME           "card-id"
#define NODE_DEVICE_PROP_NAME       "node-device"
#define IS_LOCKED_PROP_NAME         "is-locked"
#define GUID_PROP_NAME              "guid"
#define IS_DISCONNECTED_PROP_NAME   "is-disconnected"

struct alsa_firewire_state {
    int fd;
    struct snd_firewire_get_info info;
    gboolean is_locked;
    gboolean is_disconnected;
};

void alsa_firewire_class_override_properties(GObjectClass *gobject_class);
void alsa_firewire_state_set_property(struct alsa_firewire_state *state, GObject *self, guint id,
                                      const GValue *val, GParamSpec *spec);
void alsa_firewire_state_get_property(const struct alsa_firewire_state *state, GObject *self,
                                      guint id, GValue *val, GParamSpec *spec);

void alsa_firewire_state_init(struct alsa_firewire_state *state);
void alsa_firewire_state_release(struct alsa_firewire_state *state);

gboolean alsa_firewire_state_open(struct alsa_firewire_state *state, const gchar *path, gint open_flag,
                                  GError **error);

gboolean alsa_firewire_state_lock(struct alsa_firewire_state *state, GError **error);

gboolean alsa_firewire_state_unlock(struct alsa_firewire_state *state, GError **error);

gboolean alsa_firewire_state_create_source(struct alsa_firewire_state *state,
                                           HitakiAlsaFirewire *self,
                                           void (*handle_event)(HitakiAlsaFirewire *self,
                                                                union snd_firewire_event *event,
                                                                size_t length),
                                           GSource **source, GError **error);

#endif
