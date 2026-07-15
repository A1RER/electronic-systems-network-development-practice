#include "stm32f10x.h"
#include "Delay.h"
#include "lcd1602.h"
#include "dac8830.h"
#include "key.h"
#include "timer.h"
#include "wave.h"

#define FREQ_STEP_10        10
#define FREQ_STEP_100       100
#define FREQ_STEP_500       500
#define DUTY_STEP           5
#define AMPLITUDE_STEP      25
#define SWEEP_LCD_LOOPS     5

#define DEFAULT_FREQ    1000
#define DEFAULT_DUTY    50
#define DEFAULT_AMPLITUDE 100

#define SETTINGS_FLASH_ADDR       0x0800FC00
#define SETTINGS_MAGIC            0x5347
#define SETTINGS_VERSION          0x0003
#define SETTINGS_VERSION_V1       0x0001
#define SETTINGS_VERSION_V2       0x0002
#define SETTINGS_SAVE_IDLE_LOOPS  50
#define SETTINGS_CHECKSUM_V1      6
#define SETTINGS_CHECKSUM_V2      7
#define SETTINGS_CHECKSUM_V3      8

static uint8_t settings_dirty = 0;
static uint16_t settings_save_countdown = 0;
static uint16_t frequency_step = FREQ_STEP_100;
static uint8_t sweep_enabled = 0;
static uint8_t sweep_direction_up = 1;
static uint16_t sweep_update_countdown = 0;
static uint8_t sweep_lcd_countdown = 0;

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

    if (current_wave == WAVE_SQUARE)
    {
        LCD_Show_String(0, 0, "SQR");
    }
    else if (current_wave == WAVE_SAW)
    {
        LCD_Show_String(0, 0, "SAW");
    }
    else if (current_wave == WAVE_SINE)
    {
        LCD_Show_String(0, 0, "SIN");
    }
    else
    {
        LCD_Show_String(0, 0, "ERR");
    }

    LCD_Show_String(0, 3, " O:");
    if (wave_output_enabled)
    {
        LCD_Show_String(0, 6, "ON ");
    }
    else
    {
        LCD_Show_String(0, 6, "OFF");
    }

    if (sweep_enabled)
    {
        LCD_Show_String(0, 9, "SW:");
    }
    else
    {
        LCD_Show_String(0, 9, "ST:");
    }

    if (frequency_step == FREQ_STEP_10)
    {
        LCD_Show_String(0, 12, " 10");
    }
    else
    {
        LCD_Show_Num(0, 12, frequency_step, 3);
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
    else if (current_wave == WAVE_SAW || current_wave == WAVE_SINE)
    {
        LCD_Show_String(1, 9, "A:");
        LCD_Show_Num(1, 11, wave_amplitude, 3);
        LCD_Show_String(1, 14, "%");
    }
    else
    {
        LCD_Show_String(1, 11, "-- ");
    }
}

static uint16_t Settings_Checksum(const uint16_t *data, uint8_t count)
{
    uint16_t checksum;
    uint8_t i;

    checksum = 0xA55A;

    for (i = 0; i < count; i++)
    {
        checksum ^= data[i];
    }

    return checksum;
}

static void Settings_MarkDirty(void)
{
    settings_dirty = 1;
    settings_save_countdown = SETTINGS_SAVE_IDLE_LOOPS;
}

static void Settings_SaveNow(void)
{
    uint16_t data[9];
    uint8_t i;
    uint32_t address;
    FLASH_Status status;

    data[0] = SETTINGS_MAGIC;
    data[1] = SETTINGS_VERSION;
    data[2] = (uint16_t)current_wave;
    data[3] = (uint16_t)wave_freq;
    data[4] = (uint16_t)square_duty;
    data[5] = (uint16_t)wave_output_enabled;
    data[6] = (uint16_t)wave_amplitude;
    data[7] = frequency_step;
    data[8] = Settings_Checksum(data, SETTINGS_CHECKSUM_V3);

    FLASH_Unlock();
    status = FLASH_ErasePage(SETTINGS_FLASH_ADDR);

    if (status == FLASH_COMPLETE)
    {
        address = SETTINGS_FLASH_ADDR;

        for (i = 0; i < 9; i++)
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
    if (settings_dirty == 0 || sweep_enabled)
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
    uint8_t saved_amplitude;
    uint16_t saved_frequency_step;

    data = (const uint16_t *)SETTINGS_FLASH_ADDR;

    if (data[0] != SETTINGS_MAGIC)
    {
        return 0;
    }

    if (data[1] != SETTINGS_VERSION &&
        data[1] != SETTINGS_VERSION_V2 &&
        data[1] != SETTINGS_VERSION_V1)
    {
        return 0;
    }

    if (data[1] == SETTINGS_VERSION_V1)
    {
        if (data[6] != Settings_Checksum(data, SETTINGS_CHECKSUM_V1))
        {
            return 0;
        }
    }
    else if (data[1] == SETTINGS_VERSION_V2)
    {
        if (data[7] != Settings_Checksum(data, SETTINGS_CHECKSUM_V2))
        {
            return 0;
        }
    }
    else
    {
        if (data[8] != Settings_Checksum(data, SETTINGS_CHECKSUM_V3))
        {
            return 0;
        }
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
    saved_amplitude = DEFAULT_AMPLITUDE;
    saved_frequency_step = FREQ_STEP_100;

    if (data[1] == SETTINGS_VERSION_V2 || data[1] == SETTINGS_VERSION)
    {
        if (data[6] < AMPLITUDE_MIN || data[6] > AMPLITUDE_MAX)
        {
            return 0;
        }

        saved_amplitude = (uint8_t)data[6];
    }

    if (data[1] == SETTINGS_VERSION)
    {
        if (data[7] != FREQ_STEP_10 &&
            data[7] != FREQ_STEP_100 &&
            data[7] != FREQ_STEP_500)
        {
            return 0;
        }

        saved_frequency_step = data[7];
    }

    Wave_SetType(saved_wave);
    Wave_SetFreq(saved_freq);
    Wave_SetDuty(saved_duty);
    Wave_SetAmplitude(saved_amplitude);
    Wave_SetOutput(saved_output);
    Wave_ResetIndex();
    frequency_step = saved_frequency_step;

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

static void FrequencyStep_Switch(void)
{
    if (frequency_step == FREQ_STEP_10)
    {
        frequency_step = FREQ_STEP_100;
    }
    else if (frequency_step == FREQ_STEP_100)
    {
        frequency_step = FREQ_STEP_500;
    }
    else
    {
        frequency_step = FREQ_STEP_10;
    }

    sweep_update_countdown = 0;
    LCD_Show_Status();
    Settings_MarkDirty();
}

static uint16_t Sweep_GetUpdateLoops(void)
{
    if (frequency_step == FREQ_STEP_10)
    {
        return 1;
    }

    if (frequency_step == FREQ_STEP_100)
    {
        return 7;
    }

    return 33;
}

static void Sweep_Toggle(void)
{
    sweep_enabled = (uint8_t)(sweep_enabled == 0);
    sweep_direction_up = (uint8_t)(wave_freq < FREQ_MAX);
    sweep_update_countdown = 0;
    sweep_lcd_countdown = 0;

    LCD_Show_Status();

    if (sweep_enabled == 0)
    {
        Settings_MarkDirty();
    }
}

static void Sweep_StopForManual(void)
{
    if (sweep_enabled)
    {
        sweep_enabled = 0;
        sweep_update_countdown = 0;
        sweep_lcd_countdown = 0;
    }
}

static void Sweep_Task(void)
{
    uint32_t next_frequency;

    if (sweep_enabled == 0)
    {
        return;
    }

    if (sweep_update_countdown > 0)
    {
        sweep_update_countdown--;
    }
    else
    {
        sweep_update_countdown = Sweep_GetUpdateLoops() - 1;

        if (sweep_direction_up)
        {
            next_frequency = wave_freq + frequency_step;

            if (next_frequency >= FREQ_MAX)
            {
                next_frequency = FREQ_MAX;
                sweep_direction_up = 0;
            }
        }
        else
        {
            if (wave_freq <= FREQ_MIN + frequency_step)
            {
                next_frequency = FREQ_MIN;
                sweep_direction_up = 1;
            }
            else
            {
                next_frequency = wave_freq - frequency_step;
            }
        }

        Wave_SetFreq(next_frequency);
    }

    if (sweep_lcd_countdown > 0)
    {
        sweep_lcd_countdown--;
    }
    else
    {
        sweep_lcd_countdown = SWEEP_LCD_LOOPS - 1;
        LCD_Show_Num(1, 2, wave_freq, 4);
    }
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
        Wave_SetAmplitude(DEFAULT_AMPLITUDE);
        Wave_SetOutput(1);
        Wave_ResetIndex();
        frequency_step = FREQ_STEP_100;
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
            Sweep_StopForManual();
            Wave_SetFreq(wave_freq + frequency_step);
            LCD_Show_Status();
            Settings_MarkDirty();
        }
        else if (key == KEY_3)
        {
            Sweep_StopForManual();
            Wave_SetFreq(wave_freq - frequency_step);
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
            else
            {
                Wave_SetAmplitude(wave_amplitude + AMPLITUDE_STEP);
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
            else
            {
                Wave_SetAmplitude(wave_amplitude - AMPLITUDE_STEP);
                LCD_Show_Status();
                Settings_MarkDirty();
            }
        }
        else if (key == KEY_STEP_SWITCH)
        {
            FrequencyStep_Switch();
        }
        else if (key == KEY_SWEEP_TOGGLE)
        {
            Sweep_Toggle();
        }

        Sweep_Task();
        Settings_Task();
        Delay_ms(30);
    }
}
