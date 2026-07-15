#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"
#include <stdint.h>

#define KEY_NONE    0
#define KEY_1       1
#define KEY_2       2
#define KEY_3       3
#define KEY_4       4
#define KEY_5       5
#define KEY_1_LONG  6
#define KEY_STEP_SWITCH  7
#define KEY_SWEEP_TOGGLE 8

void KEY_Init(void);
uint8_t KEY_Scan(void);

#endif
