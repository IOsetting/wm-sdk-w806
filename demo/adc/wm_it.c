
#include "wm_hal.h"

extern ADC_HandleTypeDef hadc;

__attribute__((isr)) void CORET_IRQHandler(void)
{
	uint32_t temp;
	
	temp = (*(volatile unsigned int *) (0xE000E010));
	HAL_IncTick();
}

__attribute__((isr)) void ADC_IRQHandler(void)
{
 	HAL_ADC_IRQHandler(&hadc);
}