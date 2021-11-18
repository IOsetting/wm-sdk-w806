#include "wm_hal.h"

extern DMA_HandleTypeDef hdma_i2s_tx;
extern DMA_HandleTypeDef hdma_i2s_rx;

static DMA_LinkDescriptor tx_desc[2];
static DMA_LinkDescriptor rx_desc[2];

extern void Error_Handler(void);

void HAL_MspInit(void)
{

}

void HAL_I2S_MspInit(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == I2S)
    {
        __HAL_RCC_I2S_CLK_ENABLE();
        __HAL_RCC_GPIO_CLK_ENABLE();
        
        __HAL_AFIO_REMAP_I2S_MCK(GPIOA, GPIO_PIN_7);
        __HAL_AFIO_REMAP_I2S_WS(GPIOB, GPIO_PIN_9);
        __HAL_AFIO_REMAP_I2S_CK(GPIOB, GPIO_PIN_8);
        __HAL_AFIO_REMAP_I2S_MOSI(GPIOB, GPIO_PIN_11);
        __HAL_AFIO_REMAP_I2S_MISO(GPIOB, GPIO_PIN_10);
        
        hdma_i2s_tx.Instance = DMA_Channel0;
        hdma_i2s_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_i2s_tx.Init.DestInc = DMA_DINC_DISABLE;
        hdma_i2s_tx.Init.SrcInc = DMA_SINC_ENABLE;
        hdma_i2s_tx.Init.DataAlignment = DMA_DATAALIGN_WORD;
        hdma_i2s_tx.Init.Mode = DMA_MODE_LINK_SINGLE;
        hdma_i2s_tx.Init.RequestSourceSel = DMA_REQUEST_SOURCE_I2S_TX;
        
        hdma_i2s_tx.LinkDesc = tx_desc;
        
        if (HAL_DMA_Init(&hdma_i2s_tx) != HAL_OK)
        {
            Error_Handler();
        }
        __HAL_LINKDMA(hi2s, hdmatx, hdma_i2s_tx);
        
        hdma_i2s_rx.Instance = DMA_Channel1;
        hdma_i2s_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_i2s_rx.Init.DestInc = DMA_DINC_ENABLE;
        hdma_i2s_rx.Init.SrcInc = DMA_SINC_DISABLE;
        hdma_i2s_rx.Init.DataAlignment = DMA_DATAALIGN_WORD;
        hdma_i2s_rx.Init.Mode = DMA_MODE_LINK_SINGLE;
        hdma_i2s_rx.Init.RequestSourceSel = DMA_REQUEST_SOURCE_I2S_RX;
        
        hdma_i2s_rx.LinkDesc = rx_desc;
        
        if (HAL_DMA_Init(&hdma_i2s_rx) != HAL_OK)
        {
            Error_Handler();
        }
        __HAL_LINKDMA(hi2s, hdmarx, hdma_i2s_rx);
        
        HAL_NVIC_SetPriority(I2S_IRQn, 1);
        HAL_NVIC_EnableIRQ(I2S_IRQn);
    }
}

void HAL_I2S_MspDeInit(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == I2S)
    {
        __HAL_RCC_I2S_CLK_DISABLE();
        
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_7);
        HAL_GPIO_DeInit(GPIOB, (GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11));
    }
}