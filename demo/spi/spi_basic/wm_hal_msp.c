#include "wm_hal.h"

extern DMA_HandleTypeDef hdma_spi_tx;
extern DMA_HandleTypeDef hdma_spi_rx;

extern void Error_Handler(void);

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
    
    hdma_spi_tx.Instance = DMA_Channel0;
    hdma_spi_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_spi_tx.Init.DestInc = DMA_DINC_DISABLE;
    hdma_spi_tx.Init.SrcInc = DMA_SINC_ENABLE;
    hdma_spi_tx.Init.DataAlignment = DMA_DATAALIGN_WORD;
    hdma_spi_tx.Init.Mode = DMA_MODE_NORMAL_SINGLE;
    hdma_spi_tx.Init.RequestSourceSel = DMA_REQUEST_SOURCE_SPI_TX;
    
    __HAL_LINKDMA(hspi, hdmatx, hdma_spi_tx);
    if (HAL_DMA_Init(&hdma_spi_tx) != HAL_OK)
    {
        Error_Handler();
    }
    
    hdma_spi_rx.Instance = DMA_Channel1;
    hdma_spi_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_spi_rx.Init.DestInc = DMA_DINC_ENABLE;
    hdma_spi_rx.Init.SrcInc = DMA_SINC_DISABLE;
    hdma_spi_rx.Init.DataAlignment = DMA_DATAALIGN_WORD;
    hdma_spi_rx.Init.Mode = DMA_MODE_NORMAL_SINGLE;
    hdma_spi_rx.Init.RequestSourceSel = DMA_REQUEST_SOURCE_SPI_RX;
    
    __HAL_LINKDMA(hspi, hdmarx, hdma_spi_rx);
    if (HAL_DMA_Init(&hdma_spi_rx) != HAL_OK)
    {
        Error_Handler();
    }
    
    HAL_NVIC_SetPriority(SPI_LS_IRQn, 1);
    HAL_NVIC_EnableIRQ(SPI_LS_IRQn);
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
    __HAL_RCC_SPI_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
}

