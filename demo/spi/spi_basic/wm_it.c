
#include "wm_hal.h"

extern SPI_HandleTypeDef hspi;
extern DMA_HandleTypeDef hdma_spi_tx;
extern DMA_HandleTypeDef hdma_spi_rx;

#define readl(addr) ({unsigned int __v = (*(volatile unsigned int *) (addr)); __v;})
__attribute__((isr)) void CORET_IRQHandler(void)
{
    readl(0xE000E010);
    HAL_IncTick();
}

__attribute__((isr)) void SPI_LS_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&hspi);
}

__attribute__((isr)) void DMA_Channel0_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_spi_tx);
}

__attribute__((isr)) void DMA_Channel1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_spi_rx);
}