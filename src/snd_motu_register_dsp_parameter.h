// SPDX-License-Identifier: LGPL-2.1-or-later
#ifndef __HITAKI_SND_MOTU_REGISTER_DSP_PARAMETER_H__
#define __HITAKI_SND_MOTU_REGISTER_DSP_PARAMETER_H__

#include <hitaki.h>

G_BEGIN_DECLS

#define HITAKI_TYPE_MOTU_REGISTER_DSP_PARAMETER (hitaki_snd_motu_register_dsp_parameter_get_type())

typedef struct {
    /*< private >*/
    guint8 reserved[512];
    // TODO: when UAPI header of ALSA firewire stack is enough disseminated, the above 512 byte
    // data can be replaced with actual structure.
} HitakiSndMotuRegisterDspParameter;

GType hitaki_snd_motu_register_dsp_parameter_get_type() G_GNUC_CONST;

HitakiSndMotuRegisterDspParameter *hitaki_snd_motu_register_dsp_parameter_new(void);

void hitaki_snd_motu_register_dsp_parameter_get_mixer_source_gain(
    const HitakiSndMotuRegisterDspParameter *self, gsize mixer, const guint8 *gain[20]);

void hitaki_snd_motu_register_dsp_parameter_get_mixer_source_pan(
    const HitakiSndMotuRegisterDspParameter *self, gsize mixer, const guint8 *pan[20]);

void hitaki_snd_motu_register_dsp_parameter_get_mixer_source_flag(
    const HitakiSndMotuRegisterDspParameter *self, gsize mixer, const guint8 *flag[20]);

void hitaki_snd_motu_register_dsp_parameter_get_mixer_source_paired_balance(
    const HitakiSndMotuRegisterDspParameter *self, gsize mixer, const guint8 *balance[20]);

void hitaki_snd_motu_register_dsp_parameter_get_mixer_source_paired_width(
    const HitakiSndMotuRegisterDspParameter *self, gsize mixer, const guint8 *width[20]);

void hitaki_snd_motu_register_dsp_parameter_get_mixer_output_paired_volume(
    const HitakiSndMotuRegisterDspParameter *self, const guint8 *volume[4]);

void hitaki_snd_motu_register_dsp_parameter_get_mixer_output_paired_flag(
    const HitakiSndMotuRegisterDspParameter *self, const guint8 *flag[4]);

void hitaki_snd_motu_register_dsp_parameter_get_main_output_paired_volume(
    const HitakiSndMotuRegisterDspParameter *self, guint8 *volume);

void hitaki_snd_motu_register_dsp_parameter_get_headphone_output_paired_volume(
    const HitakiSndMotuRegisterDspParameter *self, guint8 *volume);

void hitaki_snd_motu_register_dsp_parameter_get_headphone_output_paired_assignment(
    const HitakiSndMotuRegisterDspParameter *self, guint8 *assignment);

void hitaki_snd_motu_register_dsp_parameter_get_line_input_boost_flag(
    const HitakiSndMotuRegisterDspParameter *self, guint8 *boost_flag);

void hitaki_snd_motu_register_dsp_parameter_get_line_input_nominal_level_flag(
    const HitakiSndMotuRegisterDspParameter *self, guint8 *nominal_level_flag);

void hitaki_snd_motu_register_dsp_parameter_get_input_gain_and_invert(
    const HitakiSndMotuRegisterDspParameter *self, const guint8 *gain_and_invert[10]);

void hitaki_snd_motu_register_dsp_parameter_get_input_flag(
    const HitakiSndMotuRegisterDspParameter *self, const guint8 *flag[10]);

G_END_DECLS

#endif
