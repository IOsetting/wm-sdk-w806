
#include "wm_hal.h"

extern PMU_HandleTypeDef hpmu;

__attribute__((isr)) void CORET_IRQHandler(void)
{
	uint32_t temp;
	
	temp = (*(volatile unsigned int *) (0xE000E010));
	HAL_IncTick();
}

__attribute__((isr)) void PMU_IRQHandler(void)
{
	HAL_PMU_IRQHandler(&hpmu);
}