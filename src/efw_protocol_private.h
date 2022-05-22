// SPDX-License-Identifier: LGPL-2.1-or-later
#ifndef __HITAKI_EFW_PROTOCOL_INTERNAL_H__
#define __HITAKI_EFW_PROTOCOL_INTERNAL_H__

#include "hitaki.h"

#define HEADER_SIZE                 (sizeof(struct snd_efw_transaction))
#define HEADER_QUADLET_COUNT        (HEADER_SIZE / sizeof(__be32))
#define MINIMUM_SUPPORTED_VERSION   1
#define MAXIMUM_FRAME_BYTES         0x200U

extern const char *efw_protocol_err_msgs[16];

static inline void generate_efw_protocol_error(GError **error, HitakiEfwProtocolError code)
{
    g_set_error_literal(error, HITAKI_EFW_PROTOCOL_ERROR, code, efw_protocol_err_msgs[code]);
}

#endif
