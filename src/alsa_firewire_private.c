// SPDX-License-Identifier: LGPL-2.1-or-later
#include "alsa_firewire_private.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct {
    GSource src;
    int fd;
    HitakiAlsaFirewire *unit;
    gpointer tag;
    void *buf;
    size_t len;
    void (*handle_event)(HitakiAlsaFirewire *self, union snd_firewire_event *event, size_t length);
} AlsaFirewireSource;

const char *const alsa_firewire_error_msgs[7] = {
    [HITAKI_ALSA_FIREWIRE_ERROR_IS_OPENED] = "The instance is already associated to unit",
    [HITAKI_ALSA_FIREWIRE_ERROR_IS_NOT_OPENED] = "The instance is not associated to unit yet",
    [HITAKI_ALSA_FIREWIRE_ERROR_IS_USED] = "The ALSA HwDep character device is already used",
    [HITAKI_ALSA_FIREWIRE_ERROR_IS_LOCKED] = "The functionality of packet streaming is prohibited",
    [HITAKI_ALSA_FIREWIRE_ERROR_IS_UNLOCKED] = "The functionality of packet streamin is available",
    [HITAKI_ALSA_FIREWIRE_ERROR_IS_DISCONNECTED] = "The sound card is disconnected",
};

void alsa_firewire_class_override_properties(GObjectClass *gobject_class)
{
    g_object_class_override_property(gobject_class,
                                     ALSA_FIREWIRE_PROP_UNIT_TYPE, UNIT_TYPE_PROP_NAME);
    g_object_class_override_property(gobject_class,
                                     ALSA_FIREWIRE_PROP_CARD_ID, CARD_ID_PROP_NAME);
    g_object_class_override_property(gobject_class,
                                     ALSA_FIREWIRE_PROP_NODE_DEVICE, NODE_DEVICE_PROP_NAME);
    g_object_class_override_property(gobject_class,
                                     ALSA_FIREWIRE_PROP_IS_LOCKED, IS_LOCKED_PROP_NAME);
    g_object_class_override_property(gobject_class,
                                     ALSA_FIREWIRE_PROP_GUID, GUID_PROP_NAME);
    g_object_class_override_property(gobject_class,
                                     ALSA_FIREWIRE_PROP_IS_DISCONNECTED, IS_DISCONNECTED_PROP_NAME);
}

void alsa_firewire_state_set_property(struct alsa_firewire_state *state, GObject *self, guint id,
                                      const GValue *val, GParamSpec *spec)
{
    switch (id) {
    case ALSA_FIREWIRE_PROP_IS_LOCKED:
        state->is_locked = g_value_get_boolean(val);
        break;
    case ALSA_FIREWIRE_PROP_IS_DISCONNECTED:
        state->is_disconnected = g_value_get_boolean(val);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(self, id, spec);
        break;
    }
}

void alsa_firewire_state_get_property(const struct alsa_firewire_state *state, GObject *self,
                                      guint id, GValue *val, GParamSpec *spec)
{
    switch (id) {
    case ALSA_FIREWIRE_PROP_UNIT_TYPE:
        g_value_set_enum(val, (HitakiAlsaFirewireType)state->info.type);
        break;
    case ALSA_FIREWIRE_PROP_CARD_ID:
        g_value_set_uint(val, state->info.card);
        break;
    case ALSA_FIREWIRE_PROP_NODE_DEVICE:
        g_value_set_static_string(val, (const gchar *)state->info.device_name);
        break;
    case ALSA_FIREWIRE_PROP_IS_LOCKED:
        g_value_set_boolean(val, state->is_locked);
        break;
    case ALSA_FIREWIRE_PROP_GUID:
        g_value_set_uint64(val, GUINT64_FROM_BE(*((guint64 *)state->info.guid)));
        break;
    case ALSA_FIREWIRE_PROP_IS_DISCONNECTED:
        g_value_set_boolean(val, state->is_disconnected);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(self, id, spec);
        break;
    }
}

void alsa_firewire_state_init(struct alsa_firewire_state *state)
{
    state->fd = -1;
    state->is_locked = FALSE;
    state->is_disconnected = FALSE;
}

void alsa_firewire_state_release(struct alsa_firewire_state *state)
{
    if (state->fd >= 0)
        close(state->fd);
    state->fd = -1;
}

gboolean alsa_firewire_state_open(struct alsa_firewire_state *state, const gchar *path,
                                  gint open_flag, GError **error)
{
    g_return_val_if_fail(state != NULL, FALSE);
    g_return_val_if_fail(path != NULL && strlen(path) > 0, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (state->fd >= 0) {
        generate_alsa_firewire_error(error, HITAKI_ALSA_FIREWIRE_ERROR_IS_OPENED);
        return FALSE;
    }

    // Open ALSA HwDep character device.
    open_flag |= O_RDONLY;
    state->fd = open(path, open_flag);
    if (state->fd < 0) {
        if (errno == ENODEV) {
            generate_alsa_firewire_error(error, HITAKI_ALSA_FIREWIRE_ERROR_IS_DISCONNECTED);
        } else if (errno == EBUSY) {
            generate_alsa_firewire_error(error, HITAKI_ALSA_FIREWIRE_ERROR_IS_USED);
        } else {
            GFileError code = g_file_error_from_errno(errno);

            if (code != G_FILE_ERROR_FAILED)
                g_set_error(error, G_FILE_ERROR, code, "open(%s)", path);
            else
                generate_alsa_firewire_syscall_error(error, errno, "open(%s)", path);
        }
        return FALSE;
    }

    // Get FireWire sound device information.
    if (ioctl(state->fd, SNDRV_FIREWIRE_IOCTL_GET_INFO, &state->info) < 0) {
        if (errno == ENODEV)
            generate_alsa_firewire_error(error, HITAKI_ALSA_FIREWIRE_ERROR_IS_DISCONNECTED);
        else
            generate_alsa_firewire_syscall_error(error, errno, "ioctl(%s)", "SNDRV_FIREWIRE_IOCTL_GET_INFO");

        alsa_firewire_state_release(state);
        return FALSE;
    }

    return TRUE;
}

gboolean alsa_firewire_state_lock(struct alsa_firewire_state *state, GError **error)
{
    if (state->fd < 0) {
        generate_alsa_firewire_error(error, HITAKI_ALSA_FIREWIRE_ERROR_IS_NOT_OPENED);
        return FALSE;
    }

    if (ioctl(state->fd, SNDRV_FIREWIRE_IOCTL_LOCK, NULL) < 0) {
        if (errno == ENODEV)
            generate_alsa_firewire_error(error, HITAKI_ALSA_FIREWIRE_ERROR_IS_DISCONNECTED);
        else if (errno == EBUSY)
            generate_alsa_firewire_error(error, HITAKI_ALSA_FIREWIRE_ERROR_IS_LOCKED);
        else
            generate_alsa_firewire_syscall_error(error, errno, "ioctl(%s)", "SNDRV_FIREWIRE_IOCTL_LOCK");

        return FALSE;
    }

    return TRUE;
}

gboolean alsa_firewire_state_unlock(struct alsa_firewire_state *state, GError **error)
{
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (state->fd < 0) {
        generate_alsa_firewire_error(error, HITAKI_ALSA_FIREWIRE_ERROR_IS_NOT_OPENED);
        return FALSE;
    }

    if (ioctl(state->fd, SNDRV_FIREWIRE_IOCTL_UNLOCK, NULL) < 0) {
        if (errno == ENODEV)
            generate_alsa_firewire_error(error, HITAKI_ALSA_FIREWIRE_ERROR_IS_DISCONNECTED);
        else if (errno == EBADFD)
            generate_alsa_firewire_error(error, HITAKI_ALSA_FIREWIRE_ERROR_IS_UNLOCKED);
        else
            generate_alsa_firewire_syscall_error(error, errno, "ioctl(%s)", "SNDRV_FIREWIRE_IOCTL_UNLOCK");

        return FALSE;
    }

    return TRUE;
}

static gboolean check_src(GSource *source)
{
    AlsaFirewireSource *src = (AlsaFirewireSource *)source;
    GIOCondition condition;

    // Don't go to dispatch if nothing available. As an error, return
    // TRUE for POLLERR to call .dispatch for internal destruction.
    condition = g_source_query_unix_fd(source, src->tag);
    return !!(condition & (G_IO_IN | G_IO_ERR));
}

static void handle_lock_status(HitakiAlsaFirewire *self,
                               const struct snd_firewire_event_lock_status *event)
{
    GValue value = G_VALUE_INIT;
    gboolean is_disconnected;

    g_value_init(&value, G_TYPE_BOOLEAN);
    g_object_get_property(G_OBJECT(self), IS_LOCKED_PROP_NAME, &value);
    is_disconnected = g_value_get_boolean(&value);

    g_value_set_boolean(&value, event->status);
    g_object_set_property(G_OBJECT(self), IS_LOCKED_PROP_NAME, &value);

    if (is_disconnected != event->status)
        g_object_notify(G_OBJECT(self), IS_LOCKED_PROP_NAME);
}

static gboolean dispatch_src(GSource *source, GSourceFunc cb, gpointer user_data)
{
    AlsaFirewireSource *src = (AlsaFirewireSource *)source;
    HitakiAlsaFirewire *unit = src->unit;
    GIOCondition condition;
    union snd_firewire_event *event;
    ssize_t len;

    condition = g_source_query_unix_fd(source, src->tag);
    if (condition & G_IO_ERR) {
        GValue value = G_VALUE_INIT;

        g_value_init(&value, G_TYPE_BOOLEAN);
        g_value_set_boolean(&value, TRUE);
        g_object_set_property(G_OBJECT(unit), IS_DISCONNECTED_PROP_NAME, &value);
        g_object_notify(G_OBJECT(unit), IS_DISCONNECTED_PROP_NAME);

        return G_SOURCE_REMOVE;
    }

    len = read(src->fd, src->buf, src->len);
    if (len <= 0) {
        if (errno != EAGAIN)
            return G_SOURCE_REMOVE;
        else
            return G_SOURCE_CONTINUE;
    }

    event = (union snd_firewire_event *)src->buf;

    if (event->common.type == SNDRV_FIREWIRE_EVENT_LOCK_STATUS)
        handle_lock_status(unit, &event->lock_status);
    else
        src->handle_event(unit, event, len);

    return G_SOURCE_CONTINUE;
}

static void finalize_src(GSource *source)
{
    AlsaFirewireSource *src = (AlsaFirewireSource *)source;

    g_free(src->buf);
}

gboolean alsa_firewire_state_create_source(struct alsa_firewire_state *state,
                                           HitakiAlsaFirewire *self,
                                           void (*handle_event)(HitakiAlsaFirewire *self,
                                                                union snd_firewire_event *event,
                                                                size_t length),
                                           GSource **source, GError **error)
{
    static GSourceFuncs funcs = {
        .check      = check_src,
        .dispatch   = dispatch_src,
        .finalize   = finalize_src,
    };
    AlsaFirewireSource *src;
    gboolean is_locked;

    g_return_val_if_fail(HITAKI_IS_ALSA_FIREWIRE(self), FALSE);
    g_return_val_if_fail(state != NULL, FALSE);
    g_return_val_if_fail(handle_event != NULL, FALSE);
    g_return_val_if_fail(source != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (state->fd < 0) {
        generate_alsa_firewire_error(error, HITAKI_ALSA_FIREWIRE_ERROR_IS_NOT_OPENED);
        return FALSE;
    }

    *source = g_source_new(&funcs, sizeof(AlsaFirewireSource));

    g_source_set_name(*source, "HitakiAlsaFirewire");

    // MEMO: allocate one page because we cannot assume the size of data.
    src = (AlsaFirewireSource *)(*source);
    src->len = sysconf(_SC_PAGESIZE);
    src->buf = g_malloc(src->len);

    src->fd = state->fd;
    src->unit = self;
    src->tag = g_source_add_unix_fd(*source, state->fd, G_IO_IN);
    src->handle_event = handle_event;

    // Check locked or not.
    is_locked = state->is_locked;
    if (!hitaki_alsa_firewire_lock(self, error)) {
        if (*error != NULL) {
            if ((*error)->code == HITAKI_ALSA_FIREWIRE_ERROR_IS_LOCKED)
                state->is_locked = TRUE;
            g_clear_error(error);
        }
    } else {
        hitaki_alsa_firewire_unlock(self, error);
        g_clear_error(error);
        state->is_locked = FALSE;
    }
    if (is_locked != state->is_locked)
        g_object_notify(G_OBJECT(self), IS_LOCKED_PROP_NAME);

    return TRUE;
}
