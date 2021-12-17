/******************************************************************************
** 
 * \file        st7789.c
 * \author      IOsetting | iosetting@outlook.com
 * \date        
 * \brief       Library of ST7789/ST7789V TFT LCD on W806
 * \note        
 * \version     v0.1
 * \ingroup     demo
 * \remarks     test-board: HLK-W806-KIT-V1.0
 *
******************************************************************************/

#include "st7789.h"

static const uint8_t init_cmds[] = {
    9,                                               // 9 commands in list:
    ST77XX_SWRESET, ST77XX_CMD_DELAY,                // 1: Software reset, no args, w/delay
    150,                                             //    150 ms delay
    ST77XX_SLPOUT, ST77XX_CMD_DELAY,                 // 2: Out of sleep mode, no args, w/delay
    10,                                              //    10 ms delay
    ST77XX_COLMOD, 1 | ST77XX_CMD_DELAY,             // 3: Set color mode, 1 arg + delay:
    (ST7789_COLMOD_65K | ST7789_COLMOD_16BIT),       //    16-bit color 0x55
    10,                                              //    10 ms delay
    ST77XX_MADCTL, 1,                                // 4: Memory access ctrl (directions), 1 arg:
    ST77XX_ROTATION,                                 //    Row addr/col addr, bottom to top refresh
    ST77XX_CASET, 4,                                 // 5: Column addr set, 4 args, no delay:
    0, 0,                                            //    XSTART
    0, ST77XX_WIDTH,                                 //    XEND
    ST77XX_RASET, 4,                                 // 6: Row addr set, 4 args, no delay:
    0, 0,                                            //    YSTART = 0
    ST77XX_HEIGHT >> 8, ST77XX_HEIGHT & 0xff,        //    YEND = 320
    ST77XX_INVOFF, ST77XX_CMD_DELAY,                 // 7: Inversion OFF
    10,
    ST77XX_NORON, ST77XX_CMD_DELAY,                  // 8: Normal display on, no args, w/delay
    10,                                              //    10 ms delay
    ST77XX_DISPON, ST77XX_CMD_DELAY,                 // 9: Main screen turn on, no args, w/delay
    10
};

void ST7789_Init(void)
{
    ST77XX_Reset();
    ST77XX_ExecuteCommandList(init_cmds);
}
