/******************************************************************************
** 
 * \file        st7735.c
 * \author      IOsetting | iosetting@outlook.com
 * \date        
 * \brief       Library of ST7735 TFT LCD on W806
 * \note        
 * \version     v0.1
 * \ingroup     demo
 * \remarks     test-board: HLK-W806-KIT-V1.0
 *              
 *              B10   -> RES, RESET
 *              B11   -> DC, CD
 *              B14   -> CS, Chip Select
 *              B15   -> SCK, SCL, CLK, Clock
 *              B16   -> BL, Back Light
 *              B17   -> MOSI, SDA
 *              GND   -> GND
 *              3.3V  -> VCC
 * 
 *              ST7735_HARDWARE_SPI - 0: Software SPI, 1: Hardware SPI
 * 
******************************************************************************/

#include "st7735.h"

static uint8_t st7735_buf[ST7735_BUF_SIZE];
static uint16_t st7735_buf_pt = 0;

static const uint8_t
    init_cmds1[] = {                    // Init for 7735R, part 1 (red or green tab)
        15,                             // 15 commands in list:
        ST77XX_SWRESET, ST7735_DELAY,   //  1: Software reset, 0 args, w/delay
        150,                            //     150 ms delay
        ST77XX_SLPOUT,  ST7735_DELAY,   //  2: Out of sleep mode, 0 args, w/delay
        255,                            //     500 ms delay
        ST7735_FRMCTR1, 3,              //  3: Frame rate ctrl - normal mode, 3 args:
        0x05, 0x3C, 0x3C,               //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
        ST7735_FRMCTR2, 3,              //  4: Frame rate control - idle mode, 3 args:
        0x05, 0x3C, 0x3C,               //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
        ST7735_FRMCTR3, 6,              //  5: Frame rate ctrl - partial mode, 6 args:
        0x05, 0x3C, 0x3C,               //     Dot inversion mode
        0x05, 0x3C, 0x3C,               //     Line inversion mode
        ST7735_INVCTR , 1,              //  6: Display inversion ctrl, 1 arg, no delay:
        0x03,                           //     
        ST7735_PWCTR1 , 3,              //  7: Power control, 3 args, no delay:
        0x28,
        0x08,                           //     
        0x04,                           //     
        ST7735_PWCTR2 , 1,              //  8: Power control, 1 arg, no delay:
        0xC0,                           //     
        ST7735_PWCTR3 , 2,              //  9: Power control, 2 args, no delay:
        0x0D,                           //     
        0x00,                           //     
        ST7735_PWCTR4 , 2,              // 10: Power control, 2 args, no delay:
        0x8D,                           //
        0x2A,  
        ST7735_PWCTR5 , 2,              // 11: Power control, 2 args, no delay:
        0x8D, 0xEE,
        ST7735_VMCTR1 , 1,              // 12: Power control, 1 arg, no delay:
        0x1A,
        ST77XX_INVOFF , 0,              // 13: Don't invert display, no args, no delay
        ST77XX_MADCTL , 1,              // 14: Memory access control (directions), 1 arg:
        ST7735_ROTATION,                //     row addr/col addr, bottom to top refresh
        ST77XX_COLMOD , 1,              // 15: set color mode, 1 arg, no delay:
        0x05                            //     16-bit color
    },

    init_cmds2[] = {
        4,                              //  4 commands in list:
        ST7735_GMCTRP1, 16,             //  1: Magical unicorn dust, 16 args, no delay:
        0x04, 0x22, 0x07, 0x0A,
        0x2E, 0x30, 0x25, 0x2A,
        0x28, 0x26, 0x2E, 0x3A,
        0x00, 0x01, 0x03, 0x13,
        ST7735_GMCTRN1, 16,             //  2: Sparkles and rainbows, 16 args, no delay:
        0x04, 0x16, 0x06, 0x0D,
        0x2D, 0x26, 0x23, 0x27,
        0x27, 0x25, 0x2D, 0x3B,
        0x00, 0x01, 0x04, 0x13,
        ST77XX_NORON, ST7735_DELAY,     //  3: Normal display on, no args, w/delay
        10,                             //     10 ms delay
        ST77XX_DISPON, ST7735_DELAY,    //  4: Main screen turn on, no args w/delay
        100                             //     100 ms delay
    };

static void ST7735_Reset() {
    ST7735_RESET_LOW;
    HAL_Delay(5);
    ST7735_RESET_HIGH;
}

static void ST7735_TransmitByte(uint8_t dat)
{
#if ST7735_HARDWARE_SPI
    ST7735_CS_LOW;
    HAL_SPI_Transmit(&hspi, &dat, 1, 100);
    ST7735_CS_HIGH;
#else
    uint8_t i;
    ST7735_CS_LOW;
    for (i = 0; i < 8; i++)
    {
        ST7735_SCK_LOW;
        if (dat & 0x80)
        {
            ST7735_MOSI_HIGH;
        }
        else
        {
            ST7735_MOSI_LOW;
        }
        ST7735_SCK_HIGH;
        dat <<= 1;
    }
    ST7735_CS_HIGH;
#endif
}

static void ST7735_Transmit(const uint8_t *pData, uint32_t Size, uint32_t Timeout)
{
#if ST7735_HARDWARE_SPI
    ST7735_CS_LOW;
    HAL_SPI_Transmit(&hspi, pData, Size, Timeout);
    ST7735_CS_HIGH;
#else
    while (Size-- > 0)
    {
        ST7735_TransmitByte(*(pData++));
    }
#endif
}

static void ST7735_WriteCommand(uint8_t dat)
{
    ST7735_DC_LOW;
    ST7735_TransmitByte(dat);
    ST7735_DC_HIGH;
}

static void ST7735_WriteData(const uint8_t* buff, size_t buff_size)
{
    ST7735_Transmit(buff, buff_size, HAL_MAX_DELAY);
}

static void ST7735_WriteBuff(uint8_t* buff, size_t buff_size)
{
    while (buff_size--)
    {
        st7735_buf[st7735_buf_pt++] = *buff++;
        if (st7735_buf_pt == ST7735_BUF_SIZE)
        {
            ST7735_Transmit(st7735_buf, st7735_buf_pt, HAL_MAX_DELAY);
            st7735_buf_pt = 0;
        }
    }
}

static void ST7735_FlushBuff(void)
{
    if (st7735_buf_pt > 0)
    {
        ST7735_Transmit(st7735_buf, st7735_buf_pt, HAL_MAX_DELAY);
        st7735_buf_pt = 0;
    }
}

static void ST7735_ExecuteCommandList(const uint8_t *addr)
{
    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;
    while (numCommands--)
    {
        uint8_t cmd = *addr++;
        ST7735_WriteCommand(cmd);

        numArgs = *addr++;
        // If high bit set, delay follows args
        ms = numArgs & ST7735_DELAY;
        numArgs &= ~ST7735_DELAY;
        if (numArgs)
        {
            ST7735_WriteData(addr, numArgs);
            addr += numArgs;
        }

        if (ms)
        {
            ms = *addr++;
            if (ms == 255)
                ms = 500;
            HAL_Delay(ms);
        }
    }
}

static void ST7735_SetAddrWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    // column address set
    ST7735_WriteCommand(ST77XX_CASET);
    uint8_t data[] = { 0x00, x1 + ST7735_XSTART, 0x00, x2 + ST7735_XSTART };
    ST7735_WriteData(data, sizeof(data));

    // row address set
    ST7735_WriteCommand(ST77XX_RASET);
    data[1] = y1 + ST7735_YSTART;
    data[3] = y2 + ST7735_YSTART;
    ST7735_WriteData(data, sizeof(data));

    // write to RAM
    ST7735_WriteCommand(ST77XX_RAMWR);
}

void ST7735_BackLight_On(void)
{
    ST7735_BL_HIGH;
}

void ST7735_BackLight_Off(void)
{
    ST7735_BL_LOW;
}

void ST7735_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIO_CLK_ENABLE();

#if !ST7735_HARDWARE_SPI
    GPIO_InitStruct.Pin = ST7735_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ST7735_CS_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = ST7735_SCK_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ST7735_SCK_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = ST7735_MOSI_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ST7735_MOSI_PORT, &GPIO_InitStruct);
#endif
    GPIO_InitStruct.Pin = ST7735_BL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ST7735_BL_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = ST7735_RES_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ST7735_RES_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = ST7735_DC_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ST7735_DC_PORT, &GPIO_InitStruct);
}


void ST7735_Init(void)
{
    ST7735_Reset();
    ST7735_ExecuteCommandList(init_cmds1);
    ST7735_ExecuteCommandList(init_cmds2);
}

void ST7735_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color)
{
    uint16_t i,j;
    ST7735_SetAddrWindow(x_start, y_start, x_end - 1, y_end - 1);
    for(i = y_start; i < y_end; i++)
    {
        for( j = x_start; j < x_end; j++)
        {
            ST7735_WriteBuff((uint8_t *)&color, 2);
        }
    }
    ST7735_FlushBuff();
}

void ST7735_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    ST7735_SetAddrWindow( x, y, x, y);
    ST7735_WriteData((uint8_t *)&color, 2);
}

void ST7735_DrawLine(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr=0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x_end - x_start; //计算坐标增量 
    delta_y = y_end - y_start;
    uRow = x_start;//画线起点坐标
    uCol = y_start;

    if (delta_x > 0)
        incx = 1; //设置单步方向 
    else if (delta_x == 0)
        incx=0; //垂直线
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
        incy = 1;
    else if (delta_y == 0)
        incy=0; //水平线 
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    
    if (delta_x > delta_y)
        distance = delta_x; //选取基本增量坐标轴 
    else
        distance = delta_y;

    for (t = 0; t < distance + 1; t++)
    {
        ST7735_DrawPoint(uRow, uCol, color);
        xerr += delta_x;
        yerr += delta_y;
        if(xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if(yerr>distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

void ST7735_DrawRectangle(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color)
{
    ST7735_DrawLine(x_start, y_start, x_end,   y_start, color);
    ST7735_DrawLine(x_start, y_start, x_start, y_end,   color);
    ST7735_DrawLine(x_start, y_end,   x_end,   y_end,   color);
    ST7735_DrawLine(x_end,   y_start, x_end,   y_end,   color);
}

void ST7735_DrawCircle(uint16_t x, uint16_t y, uint8_t radius, uint16_t color)
{
    int a = 0, b = radius;
    while (a <= b)
    {
        ST7735_DrawPoint(x - b, y - a, color);
        ST7735_DrawPoint(x - b, y + a, color);
        ST7735_DrawPoint(x + b, y - a, color);
        ST7735_DrawPoint(x + b, y + a, color);
        ST7735_DrawPoint(x + a, y - b, color);
        ST7735_DrawPoint(x - a, y - b, color);
        ST7735_DrawPoint(x + a, y + b, color);
        ST7735_DrawPoint(x - a, y + b, color);
        a++;
        if ((a * a + b * b) > (radius * radius))
        {
            b--;
        }
    }
}

void ST7735_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data)
{
    if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT))
        return;
    if ((x + w - 1) >= ST7735_WIDTH)
        return;
    if ((y + h - 1) >= ST7735_HEIGHT)
        return;

    ST7735_SetAddrWindow(x, y, x + w - 1, y + h - 1);
    ST7735_WriteData((uint8_t *)data, sizeof(uint16_t) * w * h);
}

void ST7735_DrawChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor)
{
    uint8_t b, i, j, k, bytes;

    ST7735_SetAddrWindow(x, y, x + font.width - 1, y + font.height - 1);
    bytes = font.width / 8 + ((font.width % 8)? 1 : 0);

    for (i = 0; i < font.height; i++)
    {
        for (j = 0; j < bytes; j++)
        {
            b = font.data[((ch - 32) * font.height + i) * bytes + j];
            if (font.order == 0)
            {
                for (k = 0; k < 8 && k < font.width - j * 8; k++)
                {
                    if ((b << k) & 0x80)
                    {
                        ST7735_WriteBuff((uint8_t *)&color, 2);
                    }
                    else
                    {
                        ST7735_WriteBuff((uint8_t *)&bgcolor, 2);
                    }
                }
            }
            else
            {
                for (k = 0; k < 8 && k < font.width - j * 8; k++)
                {
                    if (b & (0x0001 << k))
                    {
                        ST7735_WriteBuff((uint8_t *)&color, 2);
                    }
                    else
                    {
                        ST7735_WriteBuff((uint8_t *)&bgcolor, 2);
                    }
                }
            }
        }

    }
    ST7735_FlushBuff();
}

void ST7735_DrawString(uint16_t x, uint16_t y, const char *p, FontDef font, uint16_t color, uint16_t bgcolor)
{
    while (*p != '\0')
    {
        ST7735_DrawChar(x, y, *p, font, color, bgcolor);
        x += font.width;
        p++;
    }
}
