#ifndef __ST7789_H
#define __ST7789_H

#include "wm_hal.h"
#include "st77xx.h"

// Some register settings
#define ST7789_INVCTR     0xB4
#define ST7789_DISSET5    0xB6

#define ST7789_PWCTR1     0xC0
#define ST7789_PWCTR2     0xC1
#define ST7789_PWCTR3     0xC2
#define ST7789_PWCTR4     0xC3
#define ST7789_PWCTR5     0xC4
#define ST7789_VMCTR1     0xC5

#define ST7789_PWCTR6     0xFC

#define ST7789_GMCTRP1    0xE0
#define ST7789_GMCTRN1    0xE1

// Color Mode for COLMOD
#define ST7789_COLMOD_65K    0x50
#define ST7789_COLMOD_262K   0x60
#define ST7789_COLMOD_12BIT  0x03
#define ST7789_COLMOD_16BIT  0x05
#define ST7789_COLMOD_18BIT  0x06
#define ST7789_COLMOD_16M    0x07


void ST7789_Init(void);


#endif
