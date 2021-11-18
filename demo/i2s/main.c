
#include <stdio.h>
#include "wm_hal.h"

I2S_HandleTypeDef hi2s;
DMA_HandleTypeDef hdma_i2s_tx;
DMA_HandleTypeDef hdma_i2s_rx;

static void I2S_Init(void);
static void DMA_Init(void);
void Error_Handler(void);

uint32_t tx_buf[1000] = {0};
uint32_t rx_buf[1000] = {0};

int main(void)
{
    int i = 0;
    SystemClock_Config(CPU_CLK_160M);
    printf("enter main\r\n");
    HAL_Init();
    DMA_Init();
    I2S_Init();
    
    for (i = 0; i < 1000; i++)
    {
        tx_buf[i] = i;
        rx_buf[i] = i;
    }
    
    HAL_I2S_Transmit_DMA(&hi2s, (uint32_t *)tx_buf, 1000);

//    HAL_I2S_Receive_DMA(&hi2s, (uint32_t *)rx_buf, 1000);
    while (1)
    {
        printf(".");
        HAL_Delay(1000);
    }
}

static void I2S_Init(void)
{
    hi2s.Instance = I2S;
    hi2s.Init.Mode = I2S_MODE_MASTER;
    hi2s.Init.Standard = I2S_STANDARD_PHILIPS;
    hi2s.Init.DataFormat = I2S_DATAFORMAT_32B;
    hi2s.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
    hi2s.Init.AudioFreq = I2S_AUDIOFREQ_44K;
    hi2s.Init.Channel = I2S_CHANNEL_MONO;
    hi2s.Init.ChannelSel = I2S_CHANNELSEL_LEFT;
    if (HAL_I2S_Init(&hi2s) != HAL_OK)
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

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    printf("tx halfcplt\r\n");
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    printf("tx cplt\r\n");
}

void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    printf("rx halfcplt\r\n");
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    printf("rx cmplt\r\n");
}

void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s)
{
    printf("err cb\r\n");
}

void Error_Handler(void)
{
    while (1)
    {
        printf("error handler\r\n");
    }
}

void assert_failed(uint8_t *file, uint32_t line)
{
    printf("Wrong parameters value: file %s on line %d\r\n", file, line);
}