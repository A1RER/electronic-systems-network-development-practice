#include "key.h"
#include "Delay.h"

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

uint8_t KEY_Scan(void)
{
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == RESET)
    {
        Delay_ms(20);
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == RESET)
        {
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == RESET);
            return KEY_1;
        }
    }

    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == RESET)
    {
        Delay_ms(20);
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == RESET)
        {
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == RESET);
            return KEY_2;
        }
    }

    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == RESET)
    {
        Delay_ms(20);
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == RESET)
        {
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == RESET);
            return KEY_3;
        }
    }

    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == RESET)
    {
        Delay_ms(20);
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == RESET)
        {
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == RESET);
            return KEY_4;
        }
    }

    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == RESET)
    {
        Delay_ms(20);
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == RESET)
        {
            while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == RESET);
            return KEY_5;
        }
    }

    return KEY_NONE;
}
