#include "dac8830.h"

/* DAC 片选拉低 */
static void DAC_CS_Low(void)
{
    GPIO_ResetBits(DAC_CS_PORT, DAC_CS_PIN);
}

/* DAC 片选拉高 */
static void DAC_CS_High(void)
{
    GPIO_SetBits(DAC_CS_PORT, DAC_CS_PIN);
}

/* 初始化 SPI1，用于控制 DAC8830 */
static void SPI1_DAC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_SPI1  |
                           RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = DAC_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DAC_CS_PORT, &GPIO_InitStructure);

    DAC_CS_High();

    SPI_I2S_DeInit(SPI1);

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;

    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Cmd(SPI1, ENABLE);
}

/* SPI1 发送 16 位数据 */
static void SPI1_Send16(uint16_t dat)
{
    uint32_t timeout;

    timeout = 100000;

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
    {
        if (timeout == 0)
        {
            return;
        }

        timeout--;
    }

    SPI_I2S_SendData(SPI1, dat);

    timeout = 100000;

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
    {
        if (timeout == 0)
        {
            return;
        }

        timeout--;
    }
}

/* 向 DAC8830 写入一个 16 位数值 */
void DAC8830_Write(uint16_t dat)
{
    DAC_CS_Low();
    SPI1_Send16(dat);
    DAC_CS_High();
}

/* 初始化 DAC8830 */
void DAC8830_Init(void)
{
    SPI1_DAC_Init();

    DAC8830_Write(0x0000);
}
