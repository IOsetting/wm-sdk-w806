/******************************************************************************
** 
 * \file        main.c
 * \author      IOsetting | iosetting@outlook.com
 * \date        
 * \brief       Demo code of ST7567 OLED
 * \note        
 * \version     v0.2
 * \ingroup     demo
 * \remarks     test-board: HLK-W806-KIT-V1.0
 * 
 *                PB14  -> CS
 *                PB15  -> SCK/CLK/D0
 *                PB17  -> MOSI/SDA/D1
 *                PB10  -> RES
 *                PB11  -> DC
 *                GND   -> GND
 *                3.3V  -> VCC
 *
******************************************************************************/

#include <stdio.h>
#include "wm_hal.h"
#include "st7567.h"

void Error_Handler(void);

#if ST7567_HARDWARE_SPI
SPI_HandleTypeDef hspi;

static void SPI_Init(void)
{
    hspi.Instance = SPI;
    hspi.Init.Mode = SPI_MODE_MASTER;
    hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi.Init.NSS = SPI_NSS_SOFT;
    hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi.Init.FirstByte = SPI_LITTLEENDIAN;

    if (HAL_SPI_Init(&hspi) != HAL_OK)
    {
        Error_Handler();
    }
}
#endif


void ST7567_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIO_CLK_ENABLE();

#if !ST7567_HARDWARE_SPI
    GPIO_InitStruct.Pin = ST7567_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ST7567_CS_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = ST7567_SCK_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ST7567_SCK_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = ST7567_MOSI_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ST7567_MOSI_PORT, &GPIO_InitStruct);
#endif
    GPIO_InitStruct.Pin = ST7567_BL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ST7567_BL_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = ST7567_RES_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ST7567_RES_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = ST7567_DC_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ST7567_DC_PORT, &GPIO_InitStruct);
}

int main(void)
{
    int y1, y2;
    uint8_t d1, d2;

    SystemClock_Config(CPU_CLK_160M);

    ST7567_GPIO_Init();
#if ST7567_HARDWARE_SPI
    SPI_Init();
#endif

    printf("enter main\r\n");
    ST7567_Init();

    ST7567_DrawLine(0,   0, 127,  0, 1);
    ST7567_DrawLine(0,   0,   0, 63, 1);
    ST7567_DrawLine(127, 0, 127, 63, 1);
    ST7567_DrawLine(0,  63, 127, 63, 1);

    ST7567_GotoXY(5, 5);
    ST7567_Puts("LCD:ST7567", &Font_6x12, 1);
    ST7567_GotoXY(10, 52);
    ST7567_Puts("Font size: 7x10", &Font_7x10, 1);
    ST7567_UpdateScreen(); // display
    HAL_Delay(2000);

    ST7567_Fill(0);

    ST7567_GotoXY(5, 5);
    ST7567_Puts("ST7567 is a single-", &Font_6x8, 1);
    ST7567_GotoXY(5, 14);
    ST7567_Puts("chip dot matrix LCD", &Font_6x8, 1);
    ST7567_GotoXY(5, 23);
    ST7567_Puts("driver", &Font_6x8, 1);
    ST7567_GotoXY(10, 52);
    ST7567_Puts("W806 SDK Demo", &Font_6x12, 1);
    ST7567_UpdateScreen();
    HAL_Delay(2000);

    y1 = 10;
    while (y1 <= 0x30)
    {
        ST7567_SetContrast(y1++);
        HAL_Delay(100);
    }
    while (y1 >= 10)
    {
        ST7567_SetContrast(y1--);
        HAL_Delay(100);
    }
    while (y1 <= 0x20)
    {
        ST7567_SetContrast(y1++);
        HAL_Delay(100);
    }
    HAL_Delay(2000);

    ST7567_ToggleInvert();
    ST7567_UpdateScreen();
    HAL_Delay(2000);

    ST7567_ToggleInvert();
    ST7567_UpdateScreen();
    HAL_Delay(2000);

    ST7567_Fill(0);
    y1 = 64, y2 = 0;
    while (y1 > 0)
    {
        ST7567_DrawLine(0, y1, 127, y2, 1);
        ST7567_UpdateScreen();
        y1 -= 2;
        y2 += 2;
        HAL_Delay(100);
    }
    HAL_Delay(1000);

    ST7567_Fill(0);
    y1 = 127, y2 = 0;
    while (y1 > 0)
    {
        ST7567_DrawLine(y1, 0, y2, 63, 1);
        ST7567_UpdateScreen();
        y1 -= 2;
        y2 += 2;
        HAL_Delay(100);
    }
    HAL_Delay(1000);

    ST7567_Fill(1);
    ST7567_UpdateScreen();
    ST7567_DrawCircle(64, 32, 25, 0);
    ST7567_UpdateScreen();
    HAL_Delay(100);
    ST7567_DrawCircle(128, 32, 25, 0);
    ST7567_UpdateScreen();
    HAL_Delay(100);
    ST7567_DrawCircle(0, 32, 25, 0);
    ST7567_UpdateScreen();
    HAL_Delay(100);
    ST7567_DrawCircle(32, 32, 25, 0);
    ST7567_UpdateScreen();
    HAL_Delay(100);
    ST7567_DrawCircle(96, 32, 25, 0);
    ST7567_UpdateScreen();
    HAL_Delay(1000);

    ST7567_Fill(0);
    ST7567_UpdateScreen();
    int32_t i = -100;
    char buf[10];
    while (i <= 100)
    {
        memset(&buf[0], 0, sizeof(buf));
        sprintf(buf, "%d", i);
        ST7567_GotoXY(50, 27);
        ST7567_Puts(buf, &Font_7x10, 1);
        ST7567_DrawLine(64, 10, (i + 100) * 128 / 200, (i + 100) * 64 / 200, 1);
        ST7567_UpdateScreen();
        ST7567_Fill(0);
        i++;
        HAL_Delay(100);
    }
    ST7567_Fill(0);
    HAL_Delay(1000);

    /* Infinite loop */
    uint32_t lst, cu, loop;
    y1 = 0, y2 = 0, d1 = 0, d2 = 0;
    while(1)
    {
        lst = 0, cu = 0;
        for (loop = 0; loop < 0x08; loop++)
        {
            cu = HAL_GetTick();
            ST7567_GotoXY(y1, y2);
            sprintf(buf, "fps: %f", 1000.0 / (double)(cu - lst));
            ST7567_Puts(buf, &Font_6x12, 1);
            ST7567_GotoXY(y1+10, y2+17);
            ST7567_Puts("W806 SDK", &Font_7x10, 1);
            ST7567_GotoXY(y1+20, y2+33);
            ST7567_Puts("IOsetting", &Font_7x10, 1);
            ST7567_UpdateScreen();
            ST7567_Fill(0);
            lst = cu;
        }
        HAL_Delay(100);
        if (d1 == 0)
        {
            y1++;
            if (y1 == 44)
            {
                d1 = 1;
                if (d2 == 0)
                {
                    y2 +=4;
                    if (y2 == 20) d2 = 1;
                }
                else
                {
                    y2 -= 4;
                    if (y2 == 0) d2 = 0;
                }
            }
        }
        else
        {
            y1--;
            if (y1 == 0) d1 = 0;
        }
    }
}

void Error_Handler(void)
{
    while (1)
    {
    }
}

void assert_failed(uint8_t *file, uint32_t line)
{
    printf("Wrong parameters value: file %s on line %d\r\n", file, line);
}