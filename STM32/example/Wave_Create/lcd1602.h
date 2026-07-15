#ifndef __LCD1602_H
#define __LCD1602_H

#include "stm32f10x.h"
#include <stdint.h>

void LCD1602_Init(void);
void LCD_Init(void);

void LCD_Clear(void);
void LCD_Show_Char(uint8_t row, uint8_t col, char ch);
void LCD_Show_String(uint8_t row, uint8_t col, char *str);
void LCD_Show_Num(uint8_t row, uint8_t col, uint32_t num, uint8_t len);

#endif
