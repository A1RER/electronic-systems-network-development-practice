#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"
#include <stdint.h>

void TIM2_Init(void);
void TIM2_SetFreq(uint32_t sample_rate);

#endif
