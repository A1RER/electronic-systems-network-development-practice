#include "wave.h"
#include "timer.h"
#include "dac8830.h"

volatile WaveTypeDef current_wave = WAVE_SQUARE;
volatile uint32_t wave_freq = 1000;
volatile uint8_t square_duty = 50;
volatile uint8_t wave_amplitude = 100;
volatile uint8_t wave_output_enabled = 1;
volatile uint16_t wave_index = 0;

static const uint16_t sine_table[SAMPLE_POINTS] =
{
    32768, 35979, 39160, 42279, 45307, 48214, 50972, 53552,
    55938, 58105, 60031, 61700, 63094, 64200, 65010, 65535,
    65010, 64200, 63094, 61700, 60031, 58105, 55938, 53552,
    50972, 48214, 45307, 42279, 39160, 35979, 32768, 29557,
    26376, 23257, 20229, 17322, 14564, 11984,  9598,  7431,
     5505,  3836,  2442,  1336,   526,    22,     0,    22,
      526,  1336,  2442,  3836,  5505,  7431,  9598, 11984,
    14564, 17322, 20229, 23257, 26376, 29557, 32768, 32768
};

static uint16_t Wave_GetSquareValue(uint16_t index)
{
    uint16_t high_points;

    high_points = (uint16_t)((uint32_t)SAMPLE_POINTS * square_duty / 100);

    if (high_points < 1)
    {
        high_points = 1;
    }

    if (high_points > SAMPLE_POINTS - 1)
    {
        high_points = SAMPLE_POINTS - 1;
    }

    if (index < high_points)
    {
        return 65535;
    }

    return 0;
}

static uint16_t Wave_GetSawValue(uint16_t index)
{
    return (uint16_t)((uint32_t)index * 65535 / (SAMPLE_POINTS - 1));
}

static uint16_t Wave_GetSineValue(uint16_t index)
{
    return sine_table[index];
}

static uint16_t Wave_ApplyAmplitude(uint16_t value)
{
    int32_t scaled;

    scaled = (int32_t)value - 32768;
    scaled = scaled * wave_amplitude / 100;
    scaled += 32768;

    if (scaled < 0)
    {
        return 0;
    }

    if (scaled > 65535)
    {
        return 65535;
    }

    return (uint16_t)scaled;
}

void Wave_Init(void)
{
    current_wave = WAVE_SQUARE;
    wave_freq = 1000;
    square_duty = 50;
    wave_amplitude = 100;
    wave_output_enabled = 1;
    wave_index = 0;
}

void Wave_SetType(WaveTypeDef wave)
{
    if (wave >= WAVE_MAX)
    {
        wave = WAVE_SQUARE;
    }

    current_wave = wave;
    wave_index = 0;
}

void Wave_SetFreq(uint32_t freq)
{
    uint32_t sample_rate;

    if (freq < FREQ_MIN)
    {
        freq = FREQ_MIN;
    }

    if (freq > FREQ_MAX)
    {
        freq = FREQ_MAX;
    }

    wave_freq = freq;
    sample_rate = wave_freq * SAMPLE_POINTS;
    TIM2_SetFreq(sample_rate);
}

void Wave_SetDuty(uint8_t duty)
{
    if (duty < DUTY_MIN)
    {
        duty = DUTY_MIN;
    }

    if (duty > DUTY_MAX)
    {
        duty = DUTY_MAX;
    }

    square_duty = duty;
    wave_index = 0;
}

void Wave_SetAmplitude(uint8_t amplitude)
{
    if (amplitude < AMPLITUDE_MIN)
    {
        amplitude = AMPLITUDE_MIN;
    }

    if (amplitude > AMPLITUDE_MAX)
    {
        amplitude = AMPLITUDE_MAX;
    }

    wave_amplitude = amplitude;
    wave_index = 0;
}

void Wave_SetOutput(uint8_t enable)
{
    wave_output_enabled = enable ? 1 : 0;
    wave_index = 0;

    if (wave_output_enabled == 0)
    {
        DAC8830_Write(0);
    }
}

void Wave_ToggleOutput(void)
{
    Wave_SetOutput((uint8_t)(wave_output_enabled == 0));
}

void Wave_ResetIndex(void)
{
    wave_index = 0;
}

void Wave_OutputISR(void)
{
    uint16_t dac_value;

    if (wave_output_enabled == 0)
    {
        DAC8830_Write(0);
        return;
    }

    if (current_wave == WAVE_SQUARE)
    {
        dac_value = Wave_GetSquareValue(wave_index);
    }
    else if (current_wave == WAVE_SAW)
    {
        dac_value = Wave_GetSawValue(wave_index);
        dac_value = Wave_ApplyAmplitude(dac_value);
    }
    else if (current_wave == WAVE_SINE)
    {
        dac_value = Wave_GetSineValue(wave_index);
        dac_value = Wave_ApplyAmplitude(dac_value);
    }
    else
    {
        dac_value = Wave_GetSquareValue(wave_index);
    }

    DAC8830_Write(dac_value);

    wave_index++;
    if (wave_index >= SAMPLE_POINTS)
    {
        wave_index = 0;
    }
}
