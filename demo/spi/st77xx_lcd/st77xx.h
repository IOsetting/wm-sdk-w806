/******************************************************************************
** 
 * \file        st77xx.h
 * \author      IOsetting | iosetting@outlook.com
 * \date        
 * \brief       Library of ST77XX TFT LCD on W806
 * \note        
 * \version     v0.1
 * \ingroup     demo
 * \remarks     test-board: HLK-W806-KIT-V1.0
 *              
******************************************************************************/

#ifndef __ST77XX_H_
#define __ST77XX_H_

#include "wm_hal.h"
#include "ascii_fonts.h"

#define ST77XX_BUF_SIZE         1024
#define ST77XX_HARDWARE_SPI     1

// CS: B4, B14
#define ST77XX_CS_PORT      GPIOB
#define ST77XX_CS_PIN       GPIO_PIN_14
// SCK: B1, B2, B15, B24
#define ST77XX_SCK_PORT     GPIOB
#define ST77XX_SCK_PIN      GPIO_PIN_15
// MOSI: B5, B17, B26, PA7
#define ST77XX_MOSI_PORT    GPIOB
#define ST77XX_MOSI_PIN     GPIO_PIN_17
// MISO: B0, B3, B16, B25

#define ST77XX_RES_PORT     GPIOB
#define ST77XX_RES_PIN      GPIO_PIN_10
#define ST77XX_DC_PORT      GPIOB
#define ST77XX_DC_PIN       GPIO_PIN_11
#define ST77XX_BL_PORT      GPIOB
#define ST77XX_BL_PIN       GPIO_PIN_16

#if ST77XX_HARDWARE_SPI
    extern SPI_HandleTypeDef    hspi;
    #define ST77XX_CS_LOW       __HAL_SPI_SET_CS_LOW(&hspi)
    #define ST77XX_CS_HIGH      __HAL_SPI_SET_CS_HIGH(&hspi)
#else
    #define ST77XX_SCK_LOW      HAL_GPIO_WritePin(ST77XX_SCK_PORT, ST77XX_SCK_PIN, GPIO_PIN_RESET)
    #define ST77XX_SCK_HIGH     HAL_GPIO_WritePin(ST77XX_SCK_PORT, ST77XX_SCK_PIN, GPIO_PIN_SET)
    #define ST77XX_CS_LOW       HAL_GPIO_WritePin(ST77XX_CS_PORT, ST77XX_CS_PIN, GPIO_PIN_RESET)
    #define ST77XX_CS_HIGH      HAL_GPIO_WritePin(ST77XX_CS_PORT, ST77XX_CS_PIN, GPIO_PIN_SET)
    #define ST77XX_MOSI_LOW     HAL_GPIO_WritePin(ST77XX_MOSI_PORT, ST77XX_MOSI_PIN, GPIO_PIN_RESET)
    #define ST77XX_MOSI_HIGH    HAL_GPIO_WritePin(ST77XX_MOSI_PORT, ST77XX_MOSI_PIN, GPIO_PIN_SET)
#endif

#define ST77XX_BL_LOW           HAL_GPIO_WritePin(ST77XX_BL_PORT, ST77XX_BL_PIN, GPIO_PIN_RESET)
#define ST77XX_BL_HIGH          HAL_GPIO_WritePin(ST77XX_BL_PORT, ST77XX_BL_PIN, GPIO_PIN_SET)
#define ST77XX_DC_LOW           HAL_GPIO_WritePin(ST77XX_DC_PORT, ST77XX_DC_PIN, GPIO_PIN_RESET)
#define ST77XX_DC_HIGH          HAL_GPIO_WritePin(ST77XX_DC_PORT, ST77XX_DC_PIN, GPIO_PIN_SET)
#define ST77XX_RESET_LOW        HAL_GPIO_WritePin(ST77XX_RES_PORT, ST77XX_RES_PIN, GPIO_PIN_RESET)
#define ST77XX_RESET_HIGH       HAL_GPIO_WritePin(ST77XX_RES_PORT, ST77XX_RES_PIN, GPIO_PIN_SET)

// ST7789V-based 2.4" display, default orientation
/*
#define ST77XX_WIDTH  240
#define ST77XX_HEIGHT 320
#define ST77XX_XSTART 0
#define ST77XX_YSTART 0
#define ST77XX_ROTATION (ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB)
*/

// ST7735-based 1.8" display, default orientation
/*
#define ST77XX_WIDTH  128
#define ST77XX_HEIGHT 160
#define ST77XX_XSTART 0
#define ST77XX_YSTART 0
#define ST77XX_ROTATION (ST77XX_MADCTL_MX | ST77XX_MADCTL_MY)
*/

// ST7735-based 1.8" display, rotate right
/*
#define ST77XX_WIDTH  160
#define ST77XX_HEIGHT 128
#define ST77XX_XSTART 0
#define ST77XX_YSTART 0
#define ST77XX_ROTATION (ST77XX_MADCTL_MY | ST77XX_MADCTL_MV)
*/

// ST7735-based 1.8" display, rotate left
/*
#define ST77XX_WIDTH  160
#define ST77XX_HEIGHT 128
#define ST77XX_XSTART 0
#define ST77XX_YSTART 0
#define ST77XX_ROTATION (ST77XX_MADCTL_MX | ST77XX_MADCTL_MV)
*/

// ST7735-based 1.8" display, upside down
/*
#define ST77XX_WIDTH  128
#define ST77XX_HEIGHT 160
#define ST77XX_XSTART 0
#define ST77XX_YSTART 0
#define ST77XX_ROTATION (0)
*/

// WaveShare ST7735S-based 1.8" display, default orientation

#define ST77XX_WIDTH  128
#define ST77XX_HEIGHT 160
#define ST77XX_XSTART 2
#define ST77XX_YSTART 1
#define ST77XX_ROTATION (ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB)


// WaveShare ST7735S-based 1.8" display, rotate right
/*
#define ST77XX_WIDTH  160
#define ST77XX_HEIGHT 128
#define ST77XX_XSTART 1
#define ST77XX_YSTART 2
#define ST77XX_ROTATION (ST77XX_MADCTL_MY | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB)
*/

// WaveShare ST7735S-based 1.8" display, rotate left
/*
#define ST77XX_WIDTH  160
#define ST77XX_HEIGHT 128
#define ST77XX_XSTART 1
#define ST77XX_YSTART 2
#define ST77XX_ROTATION (ST77XX_MADCTL_MX | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB)
*/

// WaveShare ST7735S-based 1.8" display, upside down
/*
#define ST77XX_WIDTH  128
#define ST77XX_HEIGHT 160
#define ST77XX_XSTART 2
#define ST77XX_YSTART 1
#define ST77XX_ROTATION (ST77XX_MADCTL_RGB)
*/

#define ST77XX_CMD_DELAY 0x80 // special signifier for command lists

#define ST77XX_NOP       0x00
#define ST77XX_SWRESET   0x01
#define ST77XX_RDDID     0x04
#define ST77XX_RDDST     0x09

#define ST77XX_SLPIN     0x10
#define ST77XX_SLPOUT    0x11
#define ST77XX_PTLON     0x12
#define ST77XX_NORON     0x13

#define ST77XX_INVOFF    0x20
#define ST77XX_INVON     0x21
#define ST77XX_DISPOFF   0x28
#define ST77XX_DISPON    0x29
#define ST77XX_CASET     0x2A
#define ST77XX_RASET     0x2B
#define ST77XX_RAMWR     0x2C
#define ST77XX_RAMRD     0x2E

#define ST77XX_PTLAR     0x30
#define ST77XX_TEOFF     0x34
#define ST77XX_TEON      0x35
#define ST77XX_MADCTL    0x36
#define ST77XX_COLMOD    0x3A

#define ST77XX_MADCTL_MY 0x80
#define ST77XX_MADCTL_MX 0x40
#define ST77XX_MADCTL_MV 0x20
#define ST77XX_MADCTL_ML 0x10
#define ST77XX_MADCTL_RGB 0x00
#define ST77XX_MADCTL_BGR 0x08
#define ST77XX_MADCTL_MH  0x04

#define ST77XX_RDID1     0xDA
#define ST77XX_RDID2     0xDB
#define ST77XX_RDID3     0xDC
#define ST77XX_RDID4     0xDD

// Some ready-made 16-bit ('565') color settings:
#define ST77XX_BLACK     0x0000
#define ST77XX_WHITE     0xFFFF
#define ST77XX_RED       0x00F8
#define ST77XX_GREEN     0xE007
#define ST77XX_BLUE      0x1F00
#define ST77XX_CYAN      0xFF07
#define ST77XX_MAGENTA   0x1FF8
#define ST77XX_YELLOW    0xE0FF
#define ST77XX_ORANGE    0x00FC
#define ST77XX_BROWN     0X40BC

void ST77XX_GPIO_Init(void);
void ST77XX_Reset(void);
void ST77XX_BackLight_On(void);
void ST77XX_BackLight_Off(void);
void ST77XX_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
void ST77XX_DrawLine(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);
void ST77XX_DrawRectangle(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);
void ST77XX_DrawCircle(uint16_t x, uint16_t y, uint8_t radius, uint16_t color);
void ST77XX_DrawChar(uint16_t x, uint16_t y, const char ch, FontDef_t* font, uint16_t color, uint16_t bgcolor);
void ST77XX_DrawString(uint16_t x, uint16_t y, const char *p, FontDef_t* font, uint16_t color, uint16_t bgcolor);
void ST77XX_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);
void ST77XX_ExecuteCommandList(const uint8_t *addr);
void ST77XX_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);

#endif // __ST77XX_H_
