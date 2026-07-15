#ifndef __WAVE_H
#define __WAVE_H

#include "stm32f10x.h"
#include <stdint.h>

#define SAMPLE_POINTS   64

#define FREQ_MIN        500
#define FREQ_MAX        2000

#define DUTY_MIN        20
#define DUTY_MAX        80

typedef enum
{
    WAVE_SQUARE = 0,
    WAVE_SAW    = 1,
    WAVE_SINE   = 2,
    WAVE_MAX
} WaveTypeDef;

extern volatile WaveTypeDef current_wave;
extern volatile uint32_t wave_freq;
extern volatile uint8_t square_duty;
extern volatile uint8_t wave_output_enabled;
extern volatile uint16_t wave_index;

void Wave_Init(void);
void Wave_SetType(WaveTypeDef wave);
void Wave_SetFreq(uint32_t freq);
void Wave_SetDuty(uint8_t duty);
void Wave_SetOutput(uint8_t enable);
void Wave_ToggleOutput(void);
void Wave_ResetIndex(void);
void Wave_OutputISR(void);

#endif
