
#include "wm_hal.h"

extern WDG_HandleTypeDef hwdg;

__attribute__((isr)) void CORET_IRQHandler(void)
{
    uint32_t temp;
    
    temp = (*(volatile unsigned int *) (0xE000E010));
    HAL_IncTick();
}

__attribute__((isr)) void WDG_IRQHandler(void)
{
    HAL_WDG_IRQHandler(&hwdg);
}