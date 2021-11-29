
#include <stdio.h>
#include "wm_hal.h"

void Error_Handler(void);

SPI_HandleTypeDef hspi;
DMA_HandleTypeDef hdma_spi_tx;
DMA_HandleTypeDef hdma_spi_rx;

static void SPI_Init(void);
static void DMA_Init(void);
#define data_len (10000)
uint8_t tx_data[data_len] = {0};
uint8_t rx_data[data_len] = {0};

int main(void)
{
    int i = 0;
    uint16_t *p = (uint16_t *)tx_data;
    
    SystemClock_Config(CPU_CLK_160M);
    printf("enter main\r\n");
    DMA_Init();
    SPI_Init();

    for(i = 0; i < (data_len / 2); i++)
    {
        p[i] = i;
    }
    __HAL_SPI_SET_CS_LOW(&hspi);
    HAL_SPI_Transmit_DMA(&hspi, (uint8_t *)tx_data, data_len);

//    __HAL_SPI_SET_CS_LOW(&hspi);
//    HAL_SPI_Receive_DMA(&hspi, (uint8_t *)rx_data, data_len);

    while (1)
    {
        HAL_Delay(1000);
    }
}

static void SPI_Init(void)
{
    hspi.Instance = SPI;
    hspi.Init.Mode = SPI_MODE_MASTER;
    hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi.Init.NSS = SPI_NSS_SOFT;
    hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_20;
    hspi.Init.FirstByte = SPI_LITTLEENDIAN;
    
    if (HAL_SPI_Init(&hspi) != HAL_OK)
    {
        Error_Handler();
    }
}

static void DMA_Init(void)
{
    __HAL_RCC_DMA_CLK_ENABLE();
    
    HAL_NVIC_SetPriority(DMA_Channel0_IRQn, 0);
    HAL_NVIC_EnableIRQ(DMA_Channel0_IRQn);
    
    HAL_NVIC_SetPriority(DMA_Channel1_IRQn, 0);
    HAL_NVIC_EnableIRQ(DMA_Channel1_IRQn);
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    __HAL_SPI_SET_CS_HIGH(hspi);
    printf("tx cplt\r\n");
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    __HAL_SPI_SET_CS_HIGH(hspi);
    printf("rx cplt\r\n");
}

void Error_Handler(void)
{
    while (1)
    {
    }
}

void assert_failed(uint8_t *file, uint32_t line)
{
    printf("Wrong parameters value: file %s on line %d\r\n", file, line);
}