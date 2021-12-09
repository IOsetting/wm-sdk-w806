#include "wm_hal.h"

extern SPI_HandleTypeDef hspi;

uint8_t SdSpiReadWriteByte(uint8_t write_byte)
{
    uint8_t read_byte;
    HAL_SPI_TransmitReceive(&hspi, &write_byte, &read_byte, 1, 1000);
    return read_byte;
}

void SdSpiReadData(uint8_t* data, uint32_t len)
{
    HAL_SPI_Receive(&hspi, data, len, 1000);
}

void SdSpiWriteData(uint8_t* data, uint32_t len)
{
    HAL_SPI_Transmit(&hspi, data, len, 1000);
}

void SdIOInit(void)
{
    //SPI和CS引脚会在SPI_Init中初始化,这里就不用了
    //仍然保留这个函数, spisd.c里要调用
}

void Write_CS_Pin(int x)
{
    if(x)
    {
        __HAL_SPI_SET_CS_HIGH(&hspi);
    }
    else
    {
        __HAL_SPI_SET_CS_LOW(&hspi);
    }
}

void SdSpiSpeedLow(void)
{
    hspi.Init.BaudRatePrescaler = 60 - 1;
    if (HAL_SPI_Init(&hspi) != HAL_OK)
    {
        Error_Handler();
    }
}

void SdSpiSpeedHigh(void)
{
    hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    if (HAL_SPI_Init(&hspi) != HAL_OK)
    {
        Error_Handler();
    }
}
