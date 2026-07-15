#include "lcd1602.h"
#include "Delay.h"

/*
 * LCD1602 8 位模式连接：
 *
 * PA0  -> RS
 * PB8  -> RW
 * PB7  -> EN
 *
 * PB6  -> DB0
 * PB5  -> DB1
 * PB4  -> DB2
 * PB3  -> DB3
 * PA15 -> DB4
 * PA12 -> DB5
 * PA11 -> DB6
 * PA10 -> DB7
 *
 * VO 接 GND 或电位器中间端，不接 STM32。
 */


static void LCD_Delay_Tiny(void)
{
    volatile uint16_t i;

    for (i = 0; i < 500; i++)
    {
        ;
    }
}


static void LCD_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_GPIOB, ENABLE);

    /*
     * PB3、PB4、PA15 与 JTAG 复用。
     * 关闭 JTAG，保留 SWD。
     */
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    /* PA0、PA10、PA11、PA12、PA15 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  |
                                  GPIO_Pin_10 |
                                  GPIO_Pin_11 |
                                  GPIO_Pin_12 |
                                  GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* PB3、PB4、PB5、PB6、PB7、PB8 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 |
                                  GPIO_Pin_4 |
                                  GPIO_Pin_5 |
                                  GPIO_Pin_6 |
                                  GPIO_Pin_7 |
                                  GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOA, GPIO_Pin_0);
    GPIO_ResetBits(GPIOB, GPIO_Pin_7);
    GPIO_ResetBits(GPIOB, GPIO_Pin_8);
}


static void LCD_RS(uint8_t state)
{
    if (state)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_0);
    }
    else
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_0);
    }
}


static void LCD_RW(uint8_t state)
{
    if (state)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_8);
    }
    else
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_8);
    }
}


static void LCD_EN(uint8_t state)
{
    if (state)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_7);
    }
    else
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_7);
    }
}


static void LCD_Set_Data(uint8_t data)
{
    if (data & 0x01)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_6);
    }
    else
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_6);
    }

    if (data & 0x02)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_5);
    }
    else
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_5);
    }

    if (data & 0x04)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_4);
    }
    else
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_4);
    }

    if (data & 0x08)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_3);
    }
    else
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_3);
    }

    if (data & 0x10)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_15);
    }
    else
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_15);
    }

    if (data & 0x20)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_12);
    }
    else
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_12);
    }

    if (data & 0x40)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_11);
    }
    else
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_11);
    }

    if (data & 0x80)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_10);
    }
    else
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_10);
    }
}


static void LCD_Write_Byte(uint8_t rs, uint8_t data)
{
    LCD_RS(rs);

    /* 固定写模式 */
    LCD_RW(0);

    LCD_Set_Data(data);

    LCD_EN(1);
    LCD_Delay_Tiny();
    LCD_EN(0);

    Delay_ms(2);
}


static void LCD_Write_Command(uint8_t command)
{
    LCD_Write_Byte(0, command);
}


static void LCD_Write_Data(uint8_t data)
{
    LCD_Write_Byte(1, data);
}


void LCD1602_Init(void)
{
    LCD_GPIO_Init();

    Delay_ms(20);

    /* 8 位数据总线，2 行显示，5x7 点阵 */
    LCD_Write_Command(0x38);

    /* 显示开，光标关，闪烁关 */
    LCD_Write_Command(0x0C);

    /* 光标右移，显示不移动 */
    LCD_Write_Command(0x06);

    LCD_Clear();
}


void LCD_Init(void)
{
    LCD1602_Init();
}


void LCD_Clear(void)
{
    LCD_Write_Command(0x01);
    Delay_ms(2);
}


void LCD_Show_Char(uint8_t row, uint8_t col, char ch)
{
    if (row == 0)
    {
        LCD_Write_Command(0x80 + col);
    }
    else
    {
        LCD_Write_Command(0xC0 + col);
    }

    LCD_Write_Data((uint8_t)ch);
}


void LCD_Show_String(uint8_t row, uint8_t col, char *str)
{
    while (*str != '\0')
    {
        LCD_Show_Char(row, col, *str);
        str++;
        col++;

        if (col >= 16)
        {
            break;
        }
    }
}


static uint32_t LCD_Pow10(uint8_t len)
{
    uint32_t result = 1;

    while (len--)
    {
        result *= 10;
    }

    return result;
}


void LCD_Show_Num(uint8_t row, uint8_t col, uint32_t num, uint8_t len)
{
    uint8_t i;
    uint32_t div;
    uint8_t digit;

    for (i = 0; i < len; i++)
    {
        div = LCD_Pow10(len - i - 1);
        digit = (num / div) % 10;

        LCD_Show_Char(row, col + i, (char)(digit + '0'));
    }
}
