#ifndef __WM_PMU_H__
#define __WM_PMU_H__

#include "wm_hal.h"

typedef struct
{
    uint8_t Year;       // range [0, 99]
    uint8_t Month;      // range [1, 12]
    uint8_t Date;       // range [1, 31]
    uint8_t Hours;      // range [0, 23]
    uint8_t Minutes;    // range [0, 59]
    uint8_t Seconds;    // range [0, 59]
} RTC_TimeTypeDef;

typedef struct
{
    PMU_TypeDef        *Instance;
    uint32_t           ClkSource;    /* This parameter can be a value of @ref PMU_ClkSource_Definitions */
} PMU_HandleTypeDef;

#define PMU        ((PMU_TypeDef *)PMU_BASE)

// PMU_ClkSource_Definitions
#define PMU_CLKSOURCE_32RC            PMU_CR_32KRC_CAL_EN
#define PMU_CLKSOURCE_32RCBYPASS      PMU_CR_32KRCBYPASS

#define IS_PMU_ALL_INSTANCE(INSTANCE) ((INSTANCE) == PMU)                                        
                                        
#define IS_PMU_TIMPERIOD(__PERIOD__)  (((__PERIOD__) >= 0x0000) && ((__PERIOD__) <= 0x0FFFF))

#define IS_PMU_CLKSOURCE(SOURCE)      (((SOURCE) == PMU_CR_32KRC_CAL_EN) || ((SOURCE) == PMU_CR_32KRCBYPASS))
#define IS_RTC_HOUR24(HOUR)           ((HOUR) <= 23U)
#define IS_RTC_MINUTES(MINUTES)       ((MINUTES) <= 59U)
#define IS_RTC_SECONDS(SECONDS)       ((SECONDS) <= 59U)
#define IS_RTC_YEAR(YEAR)             ((YEAR) <= 99U)
#define IS_RTC_MONTH(MONTH)           (((MONTH) >= 1U) && ((MONTH) <= 12U))
#define IS_RTC_DATE(DATE)             (((DATE) >= 1U) && ((DATE) <= 31U))


#ifdef __cplusplus
extern "C"{
#endif

HAL_StatusTypeDef HAL_PMU_Init(PMU_HandleTypeDef *hpmu);
HAL_StatusTypeDef HAL_PMU_DeInit(PMU_HandleTypeDef *hpmu);

void HAL_PMU_MspInit(PMU_HandleTypeDef *hpmu);
void HAL_PMU_MspDeInit(PMU_HandleTypeDef *hpmu);
void HAL_PMU_Enter_Sleep(PMU_HandleTypeDef *hpmu);
void HAL_PMU_Enter_Standby(PMU_HandleTypeDef *hpmu);

/**
 * Set and start timer 0
*/
HAL_StatusTypeDef HAL_PMU_TIMER0_Start(PMU_HandleTypeDef *hpmu, uint32_t Period);
HAL_StatusTypeDef HAL_PMU_TIMER0_Stop(PMU_HandleTypeDef *hpmu);

/**
 * Set and start clock
*/
HAL_StatusTypeDef HAL_PMU_RTC_Start(PMU_HandleTypeDef *hpmu, RTC_TimeTypeDef *Time);
HAL_StatusTypeDef HAL_PMU_RTC_Stop(PMU_HandleTypeDef *hpmu);
HAL_StatusTypeDef HAL_PMU_RTC_GetTime(PMU_HandleTypeDef *hpmu, RTC_TimeTypeDef *Time);
HAL_StatusTypeDef HAL_PMU_RTC_Alarm_Enable(PMU_HandleTypeDef *hpmu, RTC_TimeTypeDef *Time);
HAL_StatusTypeDef HAL_PMU_RTC_Alarm_Disable(PMU_HandleTypeDef *hpmu);

void HAL_PMU_Tim0_Callback(PMU_HandleTypeDef *hpmu);

void HAL_PMU_IO_Callback(PMU_HandleTypeDef *hpmu);

void HAL_PMU_RTC_Callback(PMU_HandleTypeDef *hpmu);

void HAL_PMU_IRQHandler(PMU_HandleTypeDef *hpmu);
#ifdef __cplusplus
}
#endif

#endif
