#ifndef __DAC8830_H
#define __DAC8830_H

#include "stm32f10x.h"
#include <stdint.h>

#define DAC_CS_PORT       GPIOA
#define DAC_CS_PIN        GPIO_Pin_4

void DAC8830_Init(void);
void DAC8830_Write(uint16_t dat);

#endif
