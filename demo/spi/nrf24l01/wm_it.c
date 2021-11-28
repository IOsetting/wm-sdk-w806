#include "wm_hal.h"
#include "nrf24l01.h"

#define readl(addr) ({unsigned int __v = (*(volatile unsigned int *) (addr)); __v;})
__attribute__((isr)) void CORET_IRQHandler(void)
{
    readl(0xE000E010);
    HAL_IncTick();
}

__attribute__((isr)) void GPIOB_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(NRF24L01_IRQ_PORT, NRF24L01_IRQ_PIN);
}