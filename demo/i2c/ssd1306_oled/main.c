// Copyright 2021 IOsetting <iosetting(at)outlook.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/******************************************************************************
** 
 * \brief       Demo code of SSD1306 OLED
 * \remarks     test-board: HLK-W806-KIT-V1.0
 * 
 *              In ssd1306.h define SSD1306_MODE_I2C: 0 for SPI Mode, 1 for I2C Mode
 *
 *              I2C Mode:
 *                PA1   -> SCL
 *                PA4   -> SDA
 *                GND   -> GND
 *                3.3V  -> VCC
 * 
 *              SPI Mode:
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
#include "ssd1306.h"

void Error_Handler(void);

#if SSD1306_MODE_I2C

    I2C_HandleTypeDef hi2c;

    void I2C_Init(void)
    {
        hi2c.Instance = I2C;
        // frequency range [100KHz, 1MHz]
        hi2c.Frequency = 400000;
        HAL_I2C_Init(&hi2c);
    }

    static void GPIO_Init(void)
    {
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        __HAL_RCC_GPIO_CLK_ENABLE();
        GPIO_InitStruct.Pin = SSD1306_SCL_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(SSD1306_SCL_PORT, &GPIO_InitStruct);
        GPIO_InitStruct.Pin = SSD1306_SDA_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(SSD1306_SDA_PORT, &GPIO_InitStruct);
    }
#else
    SPI_HandleTypeDef hspi;

    static void SPI_Init(void)
    {
        hspi.Instance = SPI;
        hspi.Init.Mode = SPI_MODE_MASTER;
        hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
        hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
        hspi.Init.NSS = SPI_NSS_SOFT;
        hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_40;
        hspi.Init.FirstByte = SPI_LITTLEENDIAN;

        if (HAL_SPI_Init(&hspi) != HAL_OK)
        {
            Error_Handler();
        }
    }

    static void GPIO_Init(void)
    {
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        __HAL_RCC_GPIO_CLK_ENABLE();
        GPIO_InitStruct.Pin = SSD1306_RES_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(SSD1306_RES_PORT, &GPIO_InitStruct);
        GPIO_InitStruct.Pin = SSD1306_DC_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(SSD1306_DC_PORT, &GPIO_InitStruct);
    }
#endif

int main(void)
{
    int y1, y2;
    uint8_t d1, d2;

    SystemClock_Config(CPU_CLK_160M);
    GPIO_Init();
#if SSD1306_MODE_I2C
    I2C_Init();
#else
    SPI_Init();
#endif
    printf("enter main\r\n");
    uint8_t res = SSD1306_Init();
    printf("OLED init: %d\n", res);

    SSD1306_DrawLine(0,   0, 127,  0, 1);
    SSD1306_DrawLine(0,   0,   0, 63, 1);
    SSD1306_DrawLine(127, 0, 127, 63, 1);
    SSD1306_DrawLine(0,  63, 127, 63, 1);
    SSD1306_GotoXY(5, 5);
    SSD1306_Puts("OLED:11x18", &Font_11x18, 1);
    SSD1306_GotoXY(10, 52);
    SSD1306_Puts("Font size: 7x10", &Font_7x10, 1);
    SSD1306_UpdateScreen(); // display
    HAL_Delay(2000);

    SSD1306_Fill(0);

    SSD1306_GotoXY(5, 5);
    SSD1306_Puts("OLED", &Font_16x26, 1);
    SSD1306_GotoXY(10, 52);
    SSD1306_Puts("W806 SDK Demo", &Font_6x12, 1);
    SSD1306_UpdateScreen();
    HAL_Delay(2000);

    SSD1306_ToggleInvert(); // Invert display
    SSD1306_UpdateScreen();
    HAL_Delay(2000);

    SSD1306_Fill(0);

    SSD1306_GotoXY(5, 5);
    SSD1306_Puts("OLED", &Font_16x32, 1);
    SSD1306_GotoXY(10, 45);
    SSD1306_Puts("W806 SDK Demo", &Font_8x16, 1);
    SSD1306_UpdateScreen();
    HAL_Delay(3000);

    SSD1306_ToggleInvert(); // Invert display
    SSD1306_UpdateScreen();
    HAL_Delay(2000);

    SSD1306_Fill(0);
    y1 = 64, y2 = 0;
    while (y1 > 0)
    {
        SSD1306_DrawLine(0, y1, 127, y2, 1);
        SSD1306_UpdateScreen();
        y1 -= 2;
        y2 += 2;
    }
    HAL_Delay(1000);

    SSD1306_Fill(0);
    y1 = 127, y2 = 0;
    while (y1 > 0)
    {
        SSD1306_DrawLine(y1, 0, y2, 63, 1);
        SSD1306_UpdateScreen();
        y1 -= 2;
        y2 += 2;
    }
    HAL_Delay(1000);

    SSD1306_Fill(1);
    SSD1306_UpdateScreen();
    SSD1306_DrawCircle(64, 32, 25, 0);
    SSD1306_UpdateScreen();
    SSD1306_DrawCircle(128, 32, 25, 0);
    SSD1306_UpdateScreen();
    SSD1306_DrawCircle(0, 32, 25, 0);
    SSD1306_UpdateScreen();
    SSD1306_DrawCircle(32, 32, 25, 0);
    SSD1306_UpdateScreen();
    SSD1306_DrawCircle(96, 32, 25, 0);
    SSD1306_UpdateScreen();
    HAL_Delay(1000);

    SSD1306_Fill(0);
    SSD1306_UpdateScreen();
    int32_t i = -100;
    char buf[10];
    while (i <= 100)
    {
        memset(&buf[0], 0, sizeof(buf));
        sprintf(buf, "%d", i);
        SSD1306_GotoXY(50, 27);
        SSD1306_Puts(buf, &Font_7x10, 1);
        SSD1306_DrawLine(64, 10, (i + 100) * 128 / 200, (i + 100) * 64 / 200, 1);
        SSD1306_UpdateScreen();
        SSD1306_Fill(0);
        i++;
    }
    SSD1306_GotoXY(50, 27);
    sprintf(buf, "END");
    SSD1306_Puts(buf, &Font_7x10, 1);
    SSD1306_UpdateScreen();
    SSD1306_Fill(0);
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
            SSD1306_GotoXY(y1, y2);
            sprintf(buf, "fps: %f", 1000.0 / (double)(cu - lst));
            SSD1306_Puts(buf, &Font_6x12, 1);
            SSD1306_GotoXY(y1+10, y2+17);
            SSD1306_Puts("W806 SDK", &Font_7x10, 1);
            SSD1306_GotoXY(y1+20, y2+33);
            SSD1306_Puts("IOsetting", &Font_7x10, 1);
            SSD1306_UpdateScreen();
            SSD1306_Fill(0);
            lst = cu;
        }
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