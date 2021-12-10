#include "wm_hal.h"

void HAL_MspInit(void)
{

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    __HAL_RCC_SPI_CLK_ENABLE();
    __HAL_AFIO_REMAP_SPI_CS(GPIOB, GPIO_PIN_4);
    __HAL_AFIO_REMAP_SPI_CLK(GPIOB, GPIO_PIN_2);
    __HAL_AFIO_REMAP_SPI_MISO(GPIOB, GPIO_PIN_3);
    __HAL_AFIO_REMAP_SPI_MOSI(GPIOB, GPIO_PIN_5);
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
    __HAL_RCC_SPI_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
}

void HAL_PMU_MspInit(PMU_HandleTypeDef *hpmu)
{
    HAL_NVIC_EnableIRQ(PMU_IRQn);
}

void HAL_PMU_MspDeInit(PMU_HandleTypeDef *hpmu)
{
    HAL_NVIC_DisableIRQ(PMU_IRQn);
}