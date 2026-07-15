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

#define SETTINGS_FLASH_ADDR       0x0800FC00
#define SETTINGS_MAGIC            0x5347
#define SETTINGS_VERSION          0x0001
#define SETTINGS_SAVE_IDLE_LOOPS  50

static uint8_t settings_dirty = 0;
static uint16_t settings_save_countdown = 0;

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

static uint16_t Settings_Checksum(const uint16_t *data)
{
    uint16_t checksum;

    checksum = 0xA55A;
    checksum ^= data[0];
    checksum ^= data[1];
    checksum ^= data[2];
    checksum ^= data[3];
    checksum ^= data[4];
    checksum ^= data[5];

    return checksum;
}

static void Settings_MarkDirty(void)
{
    settings_dirty = 1;
    settings_save_countdown = SETTINGS_SAVE_IDLE_LOOPS;
}

static void Settings_SaveNow(void)
{
    uint16_t data[7];
    uint8_t i;
    uint32_t address;
    FLASH_Status status;

    data[0] = SETTINGS_MAGIC;
    data[1] = SETTINGS_VERSION;
    data[2] = (uint16_t)current_wave;
    data[3] = (uint16_t)wave_freq;
    data[4] = (uint16_t)square_duty;
    data[5] = (uint16_t)wave_output_enabled;
    data[6] = Settings_Checksum(data);

    FLASH_Unlock();
    status = FLASH_ErasePage(SETTINGS_FLASH_ADDR);

    if (status == FLASH_COMPLETE)
    {
        address = SETTINGS_FLASH_ADDR;

        for (i = 0; i < 7; i++)
        {
            status = FLASH_ProgramHalfWord(address, data[i]);
            if (status != FLASH_COMPLETE)
            {
                break;
            }

            address += 2;
        }
    }

    FLASH_Lock();

    if (status == FLASH_COMPLETE)
    {
        settings_dirty = 0;
    }
}

static void Settings_Task(void)
{
    if (settings_dirty == 0)
    {
        return;
    }

    if (settings_save_countdown > 0)
    {
        settings_save_countdown--;
        return;
    }

    Settings_SaveNow();
}

static uint8_t Settings_Load(void)
{
    const uint16_t *data;
    WaveTypeDef saved_wave;
    uint32_t saved_freq;
    uint8_t saved_duty;
    uint8_t saved_output;

    data = (const uint16_t *)SETTINGS_FLASH_ADDR;

    if (data[0] != SETTINGS_MAGIC)
    {
        return 0;
    }

    if (data[1] != SETTINGS_VERSION)
    {
        return 0;
    }

    if (data[6] != Settings_Checksum(data))
    {
        return 0;
    }

    if (data[2] >= WAVE_MAX)
    {
        return 0;
    }

    saved_freq = data[3];
    if (saved_freq < FREQ_MIN || saved_freq > FREQ_MAX)
    {
        return 0;
    }

    if (data[4] < DUTY_MIN || data[4] > DUTY_MAX)
    {
        return 0;
    }

    saved_wave = (WaveTypeDef)data[2];
    saved_duty = (uint8_t)data[4];
    saved_output = (uint8_t)(data[5] ? 1 : 0);

    Wave_SetType(saved_wave);
    Wave_SetFreq(saved_freq);
    Wave_SetDuty(saved_duty);
    Wave_SetOutput(saved_output);
    Wave_ResetIndex();

    settings_dirty = 0;
    settings_save_countdown = 0;

    return 1;
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

    if (Settings_Load() == 0)
    {
        Wave_SetType(WAVE_SQUARE);
        Wave_SetFreq(DEFAULT_FREQ);
        Wave_SetDuty(DEFAULT_DUTY);
        Wave_SetOutput(1);
        Wave_ResetIndex();
    }

    LCD_Show_Status();

    while (1)
    {
        key = KEY_Scan();

        if (key == KEY_1_LONG)
        {
            Wave_ToggleOutput();
            LCD_Show_Status();
            Settings_MarkDirty();
        }
        else if (key == KEY_1)
        {
            Wave_Switch_Next();
            LCD_Show_Status();
            Settings_MarkDirty();
        }
        else if (key == KEY_2)
        {
            Wave_SetFreq(wave_freq + FREQ_STEP);
            LCD_Show_Status();
            Settings_MarkDirty();
        }
        else if (key == KEY_3)
        {
            Wave_SetFreq(wave_freq - FREQ_STEP);
            LCD_Show_Status();
            Settings_MarkDirty();
        }
        else if (key == KEY_4)
        {
            if (current_wave == WAVE_SQUARE)
            {
                Wave_SetDuty(square_duty + DUTY_STEP);
                LCD_Show_Status();
                Settings_MarkDirty();
            }
        }
        else if (key == KEY_5)
        {
            if (current_wave == WAVE_SQUARE)
            {
                Wave_SetDuty(square_duty - DUTY_STEP);
                LCD_Show_Status();
                Settings_MarkDirty();
            }
        }

        Settings_Task();
        Delay_ms(30);
    }
}
