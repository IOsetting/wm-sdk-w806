#ifndef __ST7735_FONTS_H__
#define __ST7735_FONTS_H__

#include <stdint.h>

typedef struct {
    const uint8_t width;
    const uint8_t height;
    const uint8_t order;
    const uint8_t *data;
} FontDef;

extern FontDef Font_6x12;
extern FontDef Font_7x10;
extern FontDef Font_8x16;
extern FontDef Font_11x18;
extern FontDef Font_12x24;
extern FontDef Font_16x26;
extern FontDef Font_16x32;

#endif // __FONTS_H__
