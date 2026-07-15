#include "key.h"

#define KEY_DEBOUNCE_SCANS          1
#define KEY_REPEAT_DELAY_SCANS      15
#define KEY_REPEAT_INTERVAL_SCANS   3
#define KEY_LONG_PRESS_SCANS        20

void KEY_Init(void)
{
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_GPIOB, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_12 |
                    GPIO_Pin_13 |
                    GPIO_Pin_14 |
                    GPIO_Pin_15;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_8;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &gpio);
}

static uint8_t KEY_ReadCurrent(void)
{
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == RESET &&
        GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == RESET)
    {
        return KEY_STEP_SWITCH;
    }

    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == RESET &&
        GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == RESET)
    {
        return KEY_SWEEP_TOGGLE;
    }

    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == RESET)
    {
        return KEY_1;
    }

    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == RESET)
    {
        return KEY_2;
    }

    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == RESET)
    {
        return KEY_3;
    }

    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == RESET)
    {
        return KEY_4;
    }

    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == RESET)
    {
        return KEY_5;
    }

    return KEY_NONE;
}

uint8_t KEY_Scan(void)
{
    static uint8_t candidate_key = KEY_NONE;
    static uint8_t stable_key = KEY_NONE;
    static uint8_t debounce_scans = 0;
    static uint8_t repeat_scans = 0;
    static uint8_t key1_long_reported = 0;
    static uint8_t combo_active = 0;
    static uint8_t combo_release_scans = 0;
    uint8_t raw_key;
    uint8_t previous_stable_key;
    uint8_t previous_key1_long_reported;

    raw_key = KEY_ReadCurrent();

    if (combo_active)
    {
        if (raw_key == KEY_NONE)
        {
            combo_release_scans++;

            if (combo_release_scans > KEY_DEBOUNCE_SCANS)
            {
                combo_active = 0;
                combo_release_scans = 0;
                candidate_key = KEY_NONE;
                stable_key = KEY_NONE;
                debounce_scans = 0;
                repeat_scans = 0;
                key1_long_reported = 0;
            }
        }
        else
        {
            combo_release_scans = 0;
        }

        return KEY_NONE;
    }

    if (raw_key != candidate_key)
    {
        candidate_key = raw_key;
        debounce_scans = 0;
        return KEY_NONE;
    }

    if (debounce_scans < KEY_DEBOUNCE_SCANS)
    {
        debounce_scans++;
        return KEY_NONE;
    }

    if (stable_key != candidate_key)
    {
        previous_stable_key = stable_key;
        previous_key1_long_reported = key1_long_reported;
        stable_key = candidate_key;
        repeat_scans = 0;
        key1_long_reported = 0;

        if (previous_stable_key == KEY_1 && stable_key == KEY_NONE)
        {
            if (previous_key1_long_reported == 0)
            {
                return KEY_1;
            }
        }

        if (stable_key == KEY_1)
        {
            return KEY_NONE;
        }

        if (stable_key == KEY_STEP_SWITCH ||
            stable_key == KEY_SWEEP_TOGGLE)
        {
            combo_active = 1;
            combo_release_scans = 0;
        }

        return stable_key;
    }

    if (stable_key == KEY_1)
    {
        if (repeat_scans < KEY_LONG_PRESS_SCANS)
        {
            repeat_scans++;

            if (repeat_scans == KEY_LONG_PRESS_SCANS)
            {
                key1_long_reported = 1;
                return KEY_1_LONG;
            }
        }

        return KEY_NONE;
    }

    if (stable_key >= KEY_2 && stable_key <= KEY_5)
    {
        if (repeat_scans < KEY_REPEAT_DELAY_SCANS)
        {
            repeat_scans++;

            if (repeat_scans == KEY_REPEAT_DELAY_SCANS)
            {
                return stable_key;
            }
        }
        else
        {
            repeat_scans++;

            if (repeat_scans >= (KEY_REPEAT_DELAY_SCANS +
                                 KEY_REPEAT_INTERVAL_SCANS))
            {
                repeat_scans = KEY_REPEAT_DELAY_SCANS;
                return stable_key;
            }
        }
    }

    return KEY_NONE;
}
