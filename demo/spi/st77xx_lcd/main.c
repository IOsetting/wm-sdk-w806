/******************************************************************************
** 
 * \file        main.c
 * \author      IOsetting | iosetting@outlook.com
 * \date        
 * \brief       Demo code of ST7735 and ST7789 TFT LCD
 * \note        
 * \version     v0.1
 * \ingroup     demo
 * \remarks     test-board: HLK-W806-KIT-V1.0
 *              
******************************************************************************/

#include "wm_hal.h"
#include "st7735.h"
#include "testimg.h"

void Error_Handler(void);

#if ST77XX_HARDWARE_SPI
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

int main(void)
{
    uint8_t y = 10;
    SystemClock_Config(CPU_CLK_240M);
    ST77XX_GPIO_Init();

#if ST77XX_HARDWARE_SPI
    SPI_Init();
#endif

    ST7735_Init();
    while(1)
    {
        ST77XX_BackLight_On();
        HAL_Delay(500);
        ST77XX_Fill(0, 0, ST77XX_WIDTH, ST77XX_HEIGHT, ST77XX_RED);
        HAL_Delay(500);
        ST77XX_Fill(0, 0, ST77XX_WIDTH, ST77XX_HEIGHT, ST77XX_YELLOW);
        HAL_Delay(500);
        ST77XX_Fill(0, 0, ST77XX_WIDTH, ST77XX_HEIGHT, ST77XX_BLUE);
        HAL_Delay(500);
        ST77XX_Fill(0, 0, ST77XX_WIDTH, ST77XX_HEIGHT, ST77XX_GREEN);
        HAL_Delay(500);

        y = 10;

        ST77XX_DrawString(5, y, (const char *)"0123456789ABCDE", Font_6x12, ST77XX_YELLOW, ST77XX_RED);
        HAL_Delay(500);
        y += Font_6x12.height + 2;

        ST77XX_DrawString(5, y, (const char *)"0123456789ABCDE", Font_7x10, ST77XX_YELLOW, ST77XX_BLUE);
        HAL_Delay(500);
        y += Font_7x10.height + 2;

        ST77XX_DrawString(5, y, (const char *)"0123456789ABCDE", Font_8x16, ST77XX_YELLOW, ST77XX_BLUE);
        HAL_Delay(500);
        y += Font_8x16.height + 2;

        ST77XX_DrawString(5, y, (const char *)"0123456ABC", Font_11x18, ST77XX_YELLOW, ST77XX_BLUE);
        HAL_Delay(500);
        y += Font_11x18.height + 2;

        ST77XX_DrawString(5, y, (const char *)"0123456AB", Font_12x24, ST77XX_YELLOW, ST77XX_BLUE);
        HAL_Delay(500);
        y += Font_12x24.height + 2;

        ST77XX_DrawString(5, y, (const char *)"ST7735", Font_16x26, ST77XX_WHITE, ST77XX_BLUE);
        HAL_Delay(500);
        y += Font_16x26.height + 2;

        ST77XX_DrawString(0, y, (const char *)"W806 SDK", Font_16x32, ST77XX_WHITE, ST77XX_BLUE);
        HAL_Delay(3000);

        ST77XX_DrawImage(0, 0, 128, 128, (uint16_t *)test_img_128x128);
        HAL_Delay(2000);

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