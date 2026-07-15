#include "timer.h"
#include "wave.h"

/* 初始化 TIM2，用于产生固定采样率中断 */
void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = 1124;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);
}

/* 设置 TIM2 中断频率，也就是 DAC 采样率 */
void TIM2_SetFreq(uint32_t sample_rate)
{
    uint32_t arr;

    if (sample_rate < 1000)
    {
        sample_rate = 1000;
    }

    if (sample_rate > 200000)
    {
        sample_rate = 200000;
    }

    arr = 72000000 / sample_rate;

    if (arr > 0)
    {
        arr -= 1;
    }

    TIM_Cmd(TIM2, DISABLE);

    TIM_SetAutoreload(TIM2, arr);
    TIM_SetCounter(TIM2, 0);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);

    TIM_Cmd(TIM2, ENABLE);
}

/* TIM2 中断服务函数，只负责调用波形输出函数 */
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

        Wave_OutputISR();
    }
}
