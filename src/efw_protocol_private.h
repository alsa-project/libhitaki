// SPDX-License-Identifier: LGPL-2.1-or-later
#ifndef __HITAKI_EFW_PROTOCOL_PRIVATE_H__
#define __HITAKI_EFW_PROTOCOL_PRIVATE_H__

#include "hitaki.h"

static inline void generate_efw_protocol_error(GError **error, HitakiEfwProtocolError code)
{
    const char *label;

    hitaki_efw_protocol_error_to_label(code, &label);
    g_set_error_literal(error, HITAKI_EFW_PROTOCOL_ERROR, code, label);
}

#endif
