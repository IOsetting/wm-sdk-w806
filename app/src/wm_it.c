
#include "wm_hal.h"
#include "auto_dl.h"

#define readl(addr) ({unsigned int __v = (*(volatile unsigned int *) (addr)); __v;})
__attribute__((isr)) void CORET_IRQHandler(void)
{
    readl(0xE000E010);
    HAL_IncTick();
}

#if USE_UART0_AUTO_DL
__attribute__((isr)) void UART0_IRQHandler(void)
{
    Auto_DL_Handler();
}
#endif