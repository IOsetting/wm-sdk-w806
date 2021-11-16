
#include "wm_hal.h"

__attribute__((isr)) void CORET_IRQHandler(void)
{
	uint32_t temp;
	
	temp = (*(volatile unsigned int *) (0xE000E010));
	HAL_IncTick();
}

extern UART_HandleTypeDef huart1;

__attribute__((isr)) void UART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart1);
}
