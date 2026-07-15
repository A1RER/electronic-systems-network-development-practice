#include "stm32f10x.h"
#include "Delay.h"
#include "lcd1602.h"
#include "dac8830.h"
#include "key.h"
#include "timer.h"
#include "wave.h"

#define FREQ_STEP       100
#define DUTY_STEP       5

#define DEFAULT_FREQ    1000
#define DEFAULT_DUTY    50

static void LED_PC13_Init(void)
{
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_13;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &gpio);

    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

static void LCD_Show_Status(void)
{
    LCD_Clear();

    LCD_Show_String(0, 0, "W:");

    if (current_wave == WAVE_SQUARE)
    {
        LCD_Show_String(0, 2, "SQUARE");
    }
    else if (current_wave == WAVE_SAW)
    {
        LCD_Show_String(0, 2, "SAW   ");
    }
    else if (current_wave == WAVE_SINE)
    {
        LCD_Show_String(0, 2, "SINE  ");
    }
    else
    {
        LCD_Show_String(0, 2, "ERROR ");
    }

    LCD_Show_String(0, 10, "O:");
    if (wave_output_enabled)
    {
        LCD_Show_String(0, 12, "ON ");
    }
    else
    {
        LCD_Show_String(0, 12, "OFF");
    }

    LCD_Show_String(1, 0, "F:");
    LCD_Show_Num(1, 2, wave_freq, 4);
    LCD_Show_String(1, 6, "Hz");

    LCD_Show_String(1, 9, "D:");

    if (current_wave == WAVE_SQUARE)
    {
        LCD_Show_Num(1, 11, square_duty, 2);
        LCD_Show_String(1, 13, "%");
    }
    else
    {
        LCD_Show_String(1, 11, "-- ");
    }
}

static void Wave_Load_Default_Param(void)
{
    Wave_SetFreq(DEFAULT_FREQ);

    if (current_wave == WAVE_SQUARE)
    {
        Wave_SetDuty(DEFAULT_DUTY);
    }

    Wave_ResetIndex();
}

static void Wave_Switch_Next(void)
{
    if (current_wave == WAVE_SQUARE)
    {
        Wave_SetType(WAVE_SAW);
    }
    else if (current_wave == WAVE_SAW)
    {
        Wave_SetType(WAVE_SINE);
    }
    else
    {
        Wave_SetType(WAVE_SQUARE);
    }

    Wave_Load_Default_Param();
}

int main(void)
{
    uint8_t key;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    Delay_Init();
    LED_PC13_Init();

    LCD1602_Init();
    DAC8830_Init();
    KEY_Init();
    Wave_Init();
    TIM2_Init();

    Wave_SetType(WAVE_SQUARE);
    Wave_SetFreq(DEFAULT_FREQ);
    Wave_SetDuty(DEFAULT_DUTY);
    Wave_ResetIndex();

    LCD_Show_Status();

    while (1)
    {
        key = KEY_Scan();

        if (key == KEY_1_LONG)
        {
            Wave_ToggleOutput();
            LCD_Show_Status();
        }
        else if (key == KEY_1)
        {
            Wave_Switch_Next();
            LCD_Show_Status();
        }
        else if (key == KEY_2)
        {
            Wave_SetFreq(wave_freq + FREQ_STEP);
            LCD_Show_Status();
        }
        else if (key == KEY_3)
        {
            Wave_SetFreq(wave_freq - FREQ_STEP);
            LCD_Show_Status();
        }
        else if (key == KEY_4)
        {
            if (current_wave == WAVE_SQUARE)
            {
                Wave_SetDuty(square_duty + DUTY_STEP);
                LCD_Show_Status();
            }
        }
        else if (key == KEY_5)
        {
            if (current_wave == WAVE_SQUARE)
            {
                Wave_SetDuty(square_duty - DUTY_STEP);
                LCD_Show_Status();
            }
        }

        Delay_ms(30);
    }
}
