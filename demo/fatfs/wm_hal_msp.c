#include "wm_hal.h"
#include "fatfs_mmc.h"

void HAL_MspInit(void)
{

}

void HAL_PMU_MspInit(PMU_HandleTypeDef *hpmu)
{
    HAL_NVIC_EnableIRQ(PMU_IRQn);
}

void HAL_PMU_MspDeInit(PMU_HandleTypeDef *hpmu)
{
    HAL_NVIC_DisableIRQ(PMU_IRQn);
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    __HAL_RCC_SPI_CLK_ENABLE();
    __HAL_AFIO_REMAP_SPI_CS(MMC_CS_PORT, MMC_CS_PIN);
    __HAL_AFIO_REMAP_SPI_CLK(MMC_SCK_PORT, MMC_SCK_PIN);
    __HAL_AFIO_REMAP_SPI_MOSI(MMC_MOSI_PORT, MMC_MOSI_PIN);
    __HAL_AFIO_REMAP_SPI_MISO(MMC_MISO_PORT, MMC_MISO_PIN);
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
    __HAL_RCC_SPI_CLK_DISABLE();
    HAL_GPIO_DeInit(MMC_CS_PORT, MMC_CS_PIN);
    HAL_GPIO_DeInit(MMC_SCK_PORT, MMC_SCK_PIN);
    HAL_GPIO_DeInit(MMC_MOSI_PORT, MMC_MOSI_PIN);
    HAL_GPIO_DeInit(MMC_MISO_PORT, MMC_MISO_PIN);
}