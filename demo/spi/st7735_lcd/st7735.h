#ifndef __ST7735_H
#define __ST7735_H

#include "wm_hal.h"
#include "st77xx.h"
#include "st7735_fonts.h"

#define ST7735_BUF_SIZE         1024
#define ST7735_HARDWARE_SPI     1

// CS: B4, B14
#define ST7735_CS_PORT      GPIOB
#define ST7735_CS_PIN       GPIO_PIN_14
// SCK: B1, B2, B15, B24
#define ST7735_SCK_PORT     GPIOB
#define ST7735_SCK_PIN      GPIO_PIN_15
// MOSI: B5, B17, B26, PA7
#define ST7735_MOSI_PORT    GPIOB
#define ST7735_MOSI_PIN     GPIO_PIN_17
// MISO: B0, B3, B16, B25

#define ST7735_RES_PORT     GPIOB
#define ST7735_RES_PIN      GPIO_PIN_10
#define ST7735_DC_PORT      GPIOB
#define ST7735_DC_PIN       GPIO_PIN_11
#define ST7735_BL_PORT      GPIOB
#define ST7735_BL_PIN       GPIO_PIN_16

#if ST7735_HARDWARE_SPI
    extern SPI_HandleTypeDef    hspi;
    #define ST7735_CS_LOW       __HAL_SPI_SET_CS_LOW(&hspi)
    #define ST7735_CS_HIGH      __HAL_SPI_SET_CS_HIGH(&hspi)
#else
    #define ST7735_SCK_LOW      HAL_GPIO_WritePin(ST7735_SCK_PORT, ST7735_SCK_PIN, GPIO_PIN_RESET)
    #define ST7735_SCK_HIGH     HAL_GPIO_WritePin(ST7735_SCK_PORT, ST7735_SCK_PIN, GPIO_PIN_SET)
    #define ST7735_CS_LOW       HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_RESET)
    #define ST7735_CS_HIGH      HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_SET)
    #define ST7735_MOSI_LOW     HAL_GPIO_WritePin(ST7735_MOSI_PORT, ST7735_MOSI_PIN, GPIO_PIN_RESET)
    #define ST7735_MOSI_HIGH    HAL_GPIO_WritePin(ST7735_MOSI_PORT, ST7735_MOSI_PIN, GPIO_PIN_SET)
#endif

#define ST7735_BL_LOW           HAL_GPIO_WritePin(ST7735_BL_PORT, ST7735_BL_PIN, GPIO_PIN_RESET)
#define ST7735_BL_HIGH          HAL_GPIO_WritePin(ST7735_BL_PORT, ST7735_BL_PIN, GPIO_PIN_SET)
#define ST7735_DC_LOW           HAL_GPIO_WritePin(ST7735_DC_PORT, ST7735_DC_PIN, GPIO_PIN_RESET)
#define ST7735_DC_HIGH          HAL_GPIO_WritePin(ST7735_DC_PORT, ST7735_DC_PIN, GPIO_PIN_SET)
#define ST7735_RESET_LOW        HAL_GPIO_WritePin(ST7735_RES_PORT, ST7735_RES_PIN, GPIO_PIN_RESET)
#define ST7735_RESET_HIGH       HAL_GPIO_WritePin(ST7735_RES_PORT, ST7735_RES_PIN, GPIO_PIN_SET)

#define ST7735_DELAY 0x80

// Some register settings
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MH  0x04

#define ST7735_FRMCTR1    0xB1
#define ST7735_FRMCTR2    0xB2
#define ST7735_FRMCTR3    0xB3
#define ST7735_INVCTR     0xB4
#define ST7735_DISSET5    0xB6

#define ST7735_PWCTR1     0xC0
#define ST7735_PWCTR2     0xC1
#define ST7735_PWCTR3     0xC2
#define ST7735_PWCTR4     0xC3
#define ST7735_PWCTR5     0xC4
#define ST7735_VMCTR1     0xC5

#define ST7735_PWCTR6     0xFC

#define ST7735_GMCTRP1    0xE0
#define ST7735_GMCTRN1    0xE1

// Some ready-made 16-bit ('565') color settings:
#define ST7735_BLACK      ST77XX_BLACK
#define ST7735_WHITE      ST77XX_WHITE
#define ST7735_RED        ST77XX_RED
#define ST7735_GREEN      ST77XX_GREEN
#define ST7735_BLUE       ST77XX_BLUE
#define ST7735_CYAN       ST77XX_CYAN
#define ST7735_MAGENTA    ST77XX_MAGENTA
#define ST7735_YELLOW     ST77XX_YELLOW
#define ST7735_ORANGE     ST77XX_ORANGE
#define ST7735_BROWN      ST77XX_BROWN

// AliExpress/eBay 1.8" display, default orientation
/*
#define ST7735_IS_160X128 1
#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160
#define ST7735_XSTART 0
#define ST7735_YSTART 0
#define ST7735_ROTATION (ST7735_MADCTL_MX | ST7735_MADCTL_MY)
*/

// AliExpress/eBay 1.8" display, rotate right
/*
#define ST7735_IS_160X128 1
#define ST7735_WIDTH  160
#define ST7735_HEIGHT 128
#define ST7735_XSTART 0
#define ST7735_YSTART 0
#define ST7735_ROTATION (ST7735_MADCTL_MY | ST7735_MADCTL_MV)
*/

// AliExpress/eBay 1.8" display, rotate left
/*
#define ST7735_IS_160X128 1
#define ST7735_WIDTH  160
#define ST7735_HEIGHT 128
#define ST7735_XSTART 0
#define ST7735_YSTART 0
#define ST7735_ROTATION (ST7735_MADCTL_MX | ST7735_MADCTL_MV)
*/

// AliExpress/eBay 1.8" display, upside down
/*
#define ST7735_IS_160X128 1
#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160
#define ST7735_XSTART 0
#define ST7735_YSTART 0
#define ST7735_ROTATION (0)
*/

// WaveShare ST7735S-based 1.8" display, default orientation
#define ST7735_IS_160X128 1
#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160
#define ST7735_XSTART 2
#define ST7735_YSTART 1
#define ST7735_ROTATION (ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB)

// WaveShare ST7735S-based 1.8" display, rotate right
/*
#define ST7735_IS_160X128 1
#define ST7735_WIDTH  160
#define ST7735_HEIGHT 128
#define ST7735_XSTART 1
#define ST7735_YSTART 2
#define ST7735_ROTATION (ST7735_MADCTL_MY | ST7735_MADCTL_MV | ST7735_MADCTL_RGB)
*/

// WaveShare ST7735S-based 1.8" display, rotate left
/*
#define ST7735_IS_160X128 1
#define ST7735_WIDTH  160
#define ST7735_HEIGHT 128
#define ST7735_XSTART 1
#define ST7735_YSTART 2
#define ST7735_ROTATION (ST7735_MADCTL_MX | ST7735_MADCTL_MV | ST7735_MADCTL_RGB)
*/

// WaveShare ST7735S-based 1.8" display, upside down
/*
#define ST7735_IS_160X128 1
#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160
#define ST7735_XSTART 2
#define ST7735_YSTART 1
#define ST7735_ROTATION (ST7735_MADCTL_RGB)
*/

void ST7735_GPIO_Init(void);
void ST7735_Init(void);
void ST7735_BackLight_On(void);
void ST7735_BackLight_Off(void);
void ST7735_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);
void ST7735_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
void ST7735_DrawLine(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);
void ST7735_DrawRectangle(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);
void ST7735_DrawCircle(uint16_t x, uint16_t y, uint8_t radius, uint16_t color);
void ST7735_DrawChar(uint16_t x, uint16_t y, const char ch, FontDef font, uint16_t color, uint16_t bgcolor);
void ST7735_DrawString(uint16_t x, uint16_t y, const char *p, FontDef font, uint16_t color, uint16_t bgcolor);
void ST7735_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);

#endif
