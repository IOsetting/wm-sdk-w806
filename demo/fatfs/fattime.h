#ifndef __FATTIME_H
#define __FATTIME_H

#include "wm_hal.h"
#include "ff.h"

void RTC_Init(void);
DWORD get_fattime (void);

#endif