/******************************************************************************
** 
 * \file        main.c
 * \author      IOsetting | iosetting@outlook.com
 * \date        
 * \brief       Demo code of SSD1306 OLED
 * \note        
 * \version     v0.1
 * \ingroup     demo
 * \remarks     test-board: HLK-W806-KIT-V1.0
 *              
 *              PA1   -> SCL
 *              PA4   -> SDA
 *              GND   -> GND
 *              3.3V  -> VCC
 *
******************************************************************************/

#include <stdio.h>
#include "wm_hal.h"
#include "ssd1306.h"

I2C_HandleTypeDef hi2c;

void I2C_Init(void)
{
    hi2c.SCL_Port = GPIOA;
    hi2c.SCL_Pin = GPIO_PIN_1;
    hi2c.SDA_Port = GPIOA;
    hi2c.SDA_Pin = GPIO_PIN_4;

    HAL_I2C_Init(&hi2c);
}

int main(void)
{
    SystemClock_Config(CPU_CLK_160M);
    I2C_Init();
    printf("enter main\r\n");
    uint8_t res = SSD1306_Init();
    printf("OLED init: %d\n", res);
    SSD1306_GotoXY(5, 5);
    SSD1306_Puts("OLED:11x18", &Font_11x18, 1);
    SSD1306_GotoXY(10, 52);
    SSD1306_Puts("Font size: 7x10", &Font_7x10, 1);
    SSD1306_UpdateScreen(); // display
    HAL_Delay(2000);

    SSD1306_Fill(0); // clear oled

    SSD1306_GotoXY(5, 5);
    SSD1306_Puts("OLED", &Font_16x26, 1);
    SSD1306_GotoXY(10, 52);
    SSD1306_Puts("W806 SDK Demo", &Font_6x12, 1);
    SSD1306_UpdateScreen(); // display
    HAL_Delay(2000);

    SSD1306_Fill(0); // clear oled

    SSD1306_GotoXY(5, 5);
    SSD1306_Puts("OLED", &Font_16x32, 1);
    SSD1306_GotoXY(10, 45);
    SSD1306_Puts("W806 SDK Demo", &Font_8x16, 1);
    SSD1306_UpdateScreen(); // display
    HAL_Delay(3000);

    SSD1306_Fill(0); // clear oled

    int y1 = 64, y2 = 0;
    while (y1 > 0)
    {
        SSD1306_DrawLine(0, y1, 128, y2, 1);
        SSD1306_UpdateScreen();
        y1 -= 2;
        y2 += 2;
    }
    HAL_Delay(1000);
    SSD1306_Fill(1); // clear oled
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

    SSD1306_Fill(0); // clear oled
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
        SSD1306_Fill(0); // clear oled
        i++;
    }
    SSD1306_GotoXY(50, 27);
    sprintf(buf, "END");
    SSD1306_Puts(buf, &Font_7x10, 1);
    SSD1306_GotoXY(10, 52);
    SSD1306_Puts("Lutsai Alexander", &Font_7x10, 1);
    SSD1306_UpdateScreen();
    SSD1306_Fill(0); // clear oled
    HAL_Delay(1000);

    /* Infinite loop */
    uint32_t lst, cu, loop;
    y1 = 0;
    y2 = 0;
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
            SSD1306_Puts("W806 SDK DEMO", &Font_7x10, 1);
            SSD1306_GotoXY(y1+20, y2+33);
            SSD1306_Puts("IOsetting", &Font_7x10, 1);
            SSD1306_UpdateScreen();
            SSD1306_Fill(0); // clear oled
            lst = cu;
        }
        y1 = (++y1) % 44;
        y2 = (++y2) % 21;
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