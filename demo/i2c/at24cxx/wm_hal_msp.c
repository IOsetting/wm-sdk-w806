#include "wm_hal.h"

void HAL_MspInit(void)
{

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
    __HAL_RCC_I2C_CLK_ENABLE();
    __HAL_AFIO_REMAP_I2C_SCL(GPIOA, GPIO_PIN_1);
    __HAL_AFIO_REMAP_I2C_SDA(GPIOA, GPIO_PIN_4);
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
    __HAL_RCC_I2C_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);
}
