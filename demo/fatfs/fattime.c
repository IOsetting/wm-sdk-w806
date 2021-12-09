#include "fattime.h"

PMU_HandleTypeDef hpmu;

void RTC_Init(void)
{
    RTC_TimeTypeDef time;
    
	hpmu.Instance = PMU;
    hpmu.ClkSource = PMU_CLKSOURCE_32RC;
    HAL_PMU_Init(&hpmu);
    
    time.Year = 121;
    time.Month = 6;
    time.Date = 10;
    time.Hours = 14;
    time.Minutes = 28;
    time.Seconds = 10;
    HAL_PMU_RTC_Start(&hpmu, &time);
}

/**
 * @brief FatFs获取时间函数
 * @note @ref http://elm-chan.org/fsw/ff/doc/fattime.html
 * @retval FatFs DWORD value
 */
DWORD get_fattime (void)
{
    RTC_TimeTypeDef time;
    HAL_PMU_RTC_GetTime(&hpmu, &time);
    return (DWORD)(time.Year - 80) << 25 |
           (DWORD)time.Month << 21 |
           (DWORD)time.Date << 16 |
           (DWORD)time.Hours << 11 |
           (DWORD)time.Minutes << 5 |
           (DWORD)time.Seconds >> 1;
}