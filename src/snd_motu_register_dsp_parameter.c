// SPDX-License-Identifier: LGPL-3.0-or-later
#include "alsa_firewire_private.h"

/**
 * HitakiSndMotuRegisterDspParameter:
 * A boxed object for parameters in MOTU register DSP model.
 *
 * A [struct@SndMotuRegisterDspParameter] is a boxed object for container of parameter in register
 * DSP model of Mark of the Unicorn (MOTU) FireWire series.
 */
HitakiSndMotuRegisterDspParameter *register_dsp_parameter_copy(const HitakiSndMotuRegisterDspParameter *self)
{
#ifdef g_memdup2
    return g_memdup2(self, sizeof(*self));
#else
    // GLib v2.68 deprecated g_memdup() with concern about overflow by narrow conversion from size_t to
    // unsigned int however it's safe in the local case.
    gpointer ptr = g_malloc(sizeof(*self));
    memcpy(ptr, self, sizeof(*self));
    return ptr;
#endif
}

G_DEFINE_BOXED_TYPE(HitakiSndMotuRegisterDspParameter, hitaki_snd_motu_register_dsp_parameter, register_dsp_parameter_copy, g_free)

/**
 * hitaki_snd_motu_register_dsp_parameter_new:
 *
 * Instantiate [struct@SndMotuRegisterDspParameter] object and return the instance.
 *
 * Returns: an instance of [struct@SndMotuRegisterDspParameter].
 */
HitakiSndMotuRegisterDspParameter *hitaki_snd_motu_register_dsp_parameter_new(void)
{
    return g_malloc0(sizeof(HitakiSndMotuRegisterDspParameter));
}

/**
 * hitaki_snd_motu_register_dsp_parameter_get_mixer_source_gain:
 * @self: A [struct@SndMotuRegisterDspParameter].
 * @mixer: the numeric index of mixer, up to 4.
 * @gain: (array fixed-size=20)(out)(transfer none): The array with elements for the data of source
 *        gains.
 *
 * Get the array with elements for the data of source gains in indicated mixer. The data has gain
 * value between 0x00 and 0x80.
 */
void hitaki_snd_motu_register_dsp_parameter_get_mixer_source_gain(
    const HitakiSndMotuRegisterDspParameter *self, gsize mixer, const guint8 *gain[20])
{
    struct snd_firewire_motu_register_dsp_parameter *param;

    g_return_if_fail(self != NULL);
    g_return_if_fail(mixer < SNDRV_FIREWIRE_MOTU_REGISTER_DSP_MIXER_COUNT);
    g_return_if_fail(gain != NULL);

    param = (struct snd_firewire_motu_register_dsp_parameter *)self;
    *gain = param->mixer.source[mixer].gain;
}

/**
 * hitaki_snd_motu_register_dsp_parameter_get_mixer_source_pan:
 * @self: A [struct@SndMotuRegisterDspParameter].
 * @mixer: the numeric index of mixer, up to 4.
 * @pan: (array fixed-size=20)(out)(transfer none): The array with elements for the data of source
 *       pan.
 *
 * Get the array with elements for the data of source pans in indicated mixer. The data has pan
 * value between 0x00 and 0x80.
 */
void hitaki_snd_motu_register_dsp_parameter_get_mixer_source_pan(
    const HitakiSndMotuRegisterDspParameter *self, gsize mixer, const guint8 *pan[20])
{
    struct snd_firewire_motu_register_dsp_parameter *param;

    g_return_if_fail(self != NULL);
    g_return_if_fail(mixer < SNDRV_FIREWIRE_MOTU_REGISTER_DSP_MIXER_COUNT);
    g_return_if_fail(pan != NULL);

    param = (struct snd_firewire_motu_register_dsp_parameter *)self;
    *pan = param->mixer.source[mixer].pan;
}

/**
 * hitaki_snd_motu_register_dsp_parameter_get_mixer_source_flag:
 * @self: A [struct@SndMotuRegisterDspParameter].
 * @mixer: the numeric index of mixer, up to 4.
 * @flag: (array fixed-size=20)(out)(transfer none): The array with elements for the data of source
 *        flag.
 *
 * Get the array with elements for the data of source flags in indicated mixer. The data consists of
 * bit flags below:
 *
 *  - 0x01: whether to enable mute function for the source.
 *  - 0x02: whether to enable solo function for the source.
 */
void hitaki_snd_motu_register_dsp_parameter_get_mixer_source_flag(
    const HitakiSndMotuRegisterDspParameter *self, gsize mixer, const guint8 *flag[20])
{
    struct snd_firewire_motu_register_dsp_parameter *param;

    g_return_if_fail(self != NULL);
    g_return_if_fail(mixer < SNDRV_FIREWIRE_MOTU_REGISTER_DSP_MIXER_COUNT);
    g_return_if_fail(flag != NULL);

    param = (struct snd_firewire_motu_register_dsp_parameter *)self;
    *flag = param->mixer.source[mixer].flag;
}

/**
 * hitaki_snd_motu_register_dsp_parameter_get_mixer_source_paired_balance:
 * @self: A [struct@SndMotuRegisterDspParameter].
 * @mixer: the numeric index of mixer, up to 4.
 * @balance: (array fixed-size=20)(out)(transfer none): The array with elements for the data of
 *           paired source L/R balance.
 *
 * Get the array with elements for the data of paired source L/R balance in indicated mixer. The
 * data has L/R balance value between 0x00 and 0x80.
 */
void hitaki_snd_motu_register_dsp_parameter_get_mixer_source_paired_balance(
    const HitakiSndMotuRegisterDspParameter *self, gsize mixer, const guint8 *balance[20])
{
    struct snd_firewire_motu_register_dsp_parameter *param;

    g_return_if_fail(self != NULL);
    g_return_if_fail(mixer < SNDRV_FIREWIRE_MOTU_REGISTER_DSP_MIXER_COUNT);
    g_return_if_fail(balance != NULL);

    param = (struct snd_firewire_motu_register_dsp_parameter *)self;
    *balance = param->mixer.source[mixer].paired_balance;
}

/**
 * hitaki_snd_motu_register_dsp_parameter_get_mixer_source_paired_width:
 * @self: A [struct@SndMotuRegisterDspParameter].
 * @mixer: the numeric index of mixer, up to 4.
 * @width: (array fixed-size=20)(out)(transfer none): The array with elements for the data of
 *         paired source width.
 *
 * Get the array with elements for the data of paired source width in indicated mixer. The data
 * has width value between 0x00 and 0x80.
 */
void hitaki_snd_motu_register_dsp_parameter_get_mixer_source_paired_width(
    const HitakiSndMotuRegisterDspParameter *self, gsize mixer, const guint8 *width[20])
{
    struct snd_firewire_motu_register_dsp_parameter *param;

    g_return_if_fail(self != NULL);
    g_return_if_fail(mixer < SNDRV_FIREWIRE_MOTU_REGISTER_DSP_MIXER_COUNT);
    g_return_if_fail(width != NULL);

    param = (struct snd_firewire_motu_register_dsp_parameter *)self;
    *width = param->mixer.source[mixer].paired_width;
}

/**
 * hitaki_snd_motu_register_dsp_parameter_get_mixer_output_paired_volume:
 * @self: A [struct@SndMotuRegisterDspParameter].
 * @volume: (array fixed-size=4)(out)(transfer none): The array with elements for the data of
*           paired output volume.
 *
 * Get the array with elements for the data of paired output volume in indicated mixer. The data
 * has gain value between 0x00 and 0x80.
 */
void hitaki_snd_motu_register_dsp_parameter_get_mixer_output_paired_volume(
    const HitakiSndMotuRegisterDspParameter *self, const guint8 *volume[4])
{
    struct snd_firewire_motu_register_dsp_parameter *param ;

    g_return_if_fail(self != NULL);
    g_return_if_fail(volume != NULL);

    param = (struct snd_firewire_motu_register_dsp_parameter *)self;
    *volume = param->mixer.output.paired_volume;
}

/**
 * hitaki_snd_motu_register_dsp_parameter_get_mixer_output_paired_flag:
 * @self: A [struct@SndMotuRegisterDspParameter].
 * @flag: (array fixed-size=4)(out)(transfer none): The array with elements for the data of paired
 *        output flag.
 *
 * Get the array with elements for the data of paired output flags in indicated mixer. The data
 * consists of bit flags and masks below:
 *
 *  - 0x0f: the mask for destination of paired output
 *  - 0x10: whether to enable mute for paired output
 */
void hitaki_snd_motu_register_dsp_parameter_get_mixer_output_paired_flag(
    const HitakiSndMotuRegisterDspParameter *self, const guint8 *flag[4])
{
    struct snd_firewire_motu_register_dsp_parameter *param;

    g_return_if_fail(self != NULL);
    g_return_if_fail(flag != NULL);

    param = (struct snd_firewire_motu_register_dsp_parameter *)self;
    *flag= param->mixer.output.paired_flag;
}

/**
 * hitaki_snd_motu_register_dsp_parameter_get_main_output_paired_volume:
 * @self: A [struct@SndMotuRegisterDspParameter].
 * @volume: (out): The value of paired main output.
 *
 * Get the array with elements for the data of paired main output volume. The data has volume value
 * between 0x00 and 0x80.
 */
void hitaki_snd_motu_register_dsp_parameter_get_main_output_paired_volume(
    const HitakiSndMotuRegisterDspParameter *self, guint8 *volume)
{
    struct snd_firewire_motu_register_dsp_parameter *param;

    g_return_if_fail(self != NULL);
    g_return_if_fail(volume != NULL);

    param = (struct snd_firewire_motu_register_dsp_parameter *)self;
    *volume = param->output.main_paired_volume;
}

/**
 * hitaki_snd_motu_register_dsp_parameter_get_headphone_output_paired_volume:
 * @self: A [struct@SndMotuRegisterDspParameter].
 * @volume: (out): The value of paired headphone output.
 *
 * Get the array with elements for the data of paired headphone output volume. The data has volume
 * value between 0x00 and 0x80.
 */
void hitaki_snd_motu_register_dsp_parameter_get_headphone_output_paired_volume(
    const HitakiSndMotuRegisterDspParameter *self, guint8 *volume)
{
    struct snd_firewire_motu_register_dsp_parameter *param;

    g_return_if_fail(self != NULL);
    g_return_if_fail(volume != NULL);

    param = (struct snd_firewire_motu_register_dsp_parameter *)self;
    *volume = param->output.hp_paired_volume;
}

/**
 * hitaki_snd_motu_register_dsp_parameter_get_headphone_output_paired_assignment:
 * @self: A [struct@SndMotuRegisterDspParameter].
 * @assignment: (out): The value of paired headphone assignment.
 *
 * Get the array with elements for the data of paired headphone output source. The data has index
 * value of source.
 */
void hitaki_snd_motu_register_dsp_parameter_get_headphone_output_paired_assignment(
    const HitakiSndMotuRegisterDspParameter *self, guint8 *assignment)
{
    struct snd_firewire_motu_register_dsp_parameter *param;

    g_return_if_fail(self != NULL);
    g_return_if_fail(assignment != NULL);

    param = (struct snd_firewire_motu_register_dsp_parameter *)self;
    *assignment = param->output.hp_paired_assignment;
}

/**
 * hitaki_snd_motu_register_dsp_parameter_get_line_input_boost_flag:
 * @self: A [struct@SndMotuRegisterDspParameter].
 * @boost_flag: (out): The flag of boost for line input.
 *
 * Get the data for flags of line input boost. The data consists of bit flags for corresponding line
 * input channel. When the flag stands, the input is boosted.
 */
void hitaki_snd_motu_register_dsp_parameter_get_line_input_boost_flag(
    const HitakiSndMotuRegisterDspParameter *self, guint8 *boost_flag)
{
    struct snd_firewire_motu_register_dsp_parameter *param;

    g_return_if_fail(self != NULL);
    g_return_if_fail(boost_flag != NULL);

    param = (struct snd_firewire_motu_register_dsp_parameter *)self;
    *boost_flag = param->line_input.boost_flag;
}

/**
 * hitaki_snd_motu_register_dsp_parameter_get_line_input_nominal_level_flag:
 * @self: A [struct@SndMotuRegisterDspParameter].
 * @nominal_level_flag: (out): The flag of boost for line input.
 *
 * Get the data for flags of line input nominal level. The data consists of bit flags for
 * corresponding line input channel. When the flag stands, the nominal level of input is +4 dBu,
 * else -10 dBV.
 */
void hitaki_snd_motu_register_dsp_parameter_get_line_input_nominal_level_flag(
    const HitakiSndMotuRegisterDspParameter *self, guint8 *nominal_level_flag)
{
    struct snd_firewire_motu_register_dsp_parameter *param;

    g_return_if_fail(self != NULL);
    g_return_if_fail(nominal_level_flag != NULL);

    param = (struct snd_firewire_motu_register_dsp_parameter *)self;
    *nominal_level_flag = param->line_input.nominal_level_flag;
}

/**
 * hitaki_snd_motu_register_dsp_parameter_get_input_gain_and_invert:
 * @self: A [struct@SndMotuRegisterDspParameter].
 * @gain_and_invert: (array fixed-size=10)(out)(transfer none): The array with elements for the data
 *                   of input gain and invert flags.
 *
 * Get the array with elements for input gain and invert flags. The interpretation of data is
 * different in below two groups:
 *
 * - Ultralite
 *     - 0x1f: the gain.
 *     - 0x20: whether to invert phase of the input
 * - Audio Express and 4 pre
 *     - 0x3f: the gain
 *     - 0x40: whether to invert phase of the input
 */
void hitaki_snd_motu_register_dsp_parameter_get_input_gain_and_invert(
    const HitakiSndMotuRegisterDspParameter *self, const guint8 *gain_and_invert[10])
{
    struct snd_firewire_motu_register_dsp_parameter *param;

    g_return_if_fail(self != NULL);
    g_return_if_fail(gain_and_invert != NULL);

    param = (struct snd_firewire_motu_register_dsp_parameter *)self;
    *gain_and_invert = param->input.gain_and_invert;
}

/**
 * hitaki_snd_motu_register_dsp_parameter_get_input_flag:
 * @self: A [struct@SndMotuRegisterDspParameter].
 * @flag: (array fixed-size=10)(out)(transfer none): The array with elements for the data of input
 *        flags.
 *
 * Get the array with elements for the data of input flags. The data consists of bit flags below:
 *
 * - 0x01: whether to make paired input
 * - 0x02: whether to enable phantom powering
 * - 0x04: whether to enable attenuation pad
 * - 0x08: whether to detect plug insert to jack
 */
void hitaki_snd_motu_register_dsp_parameter_get_input_flag(
    const HitakiSndMotuRegisterDspParameter *self, const guint8 *flag[10])
{
    struct snd_firewire_motu_register_dsp_parameter *param;

    g_return_if_fail(self != NULL);
    g_return_if_fail(flag != NULL);

    param = (struct snd_firewire_motu_register_dsp_parameter *)self;
    *flag = param->input.flag;
}
