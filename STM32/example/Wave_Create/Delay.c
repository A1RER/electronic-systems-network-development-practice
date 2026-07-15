#include "Delay.h"

static uint8_t fac_us = 0;

/* 初始化 SysTick 延时 */
void Delay_Init(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

    fac_us = (uint8_t)(SystemCoreClock / 8000000);
}

/* 微秒延时 */
void Delay_us(uint32_t us)
{
    uint32_t temp;

    SysTick->LOAD = us * fac_us;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

    do
    {
        temp = SysTick->CTRL;
    }
    while ((temp & 0x01) && !(temp & (1 << 16)));

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;
}

/* 毫秒延时 */
void Delay_ms(uint32_t ms)
{
    while (ms)
    {
        Delay_us(1000);
        ms--;
    }
}
