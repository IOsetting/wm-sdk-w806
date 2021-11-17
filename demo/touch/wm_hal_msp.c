
#include "wm_hal.h"

void HAL_TOUCH_MspInit(TOUCH_HandleTypeDef* htouch)
{
    __HAL_RCC_TOUCH_CLK_ENABLE();
    // 如果用到TOUCH功能，PA7必须复用为TOUCH功能，不可他用
    __HAL_AFIO_REMAP_TOUCH(GPIOA, GPIO_PIN_7);
    HAL_NVIC_SetPriority(TOUCH_IRQn, 0);
    HAL_NVIC_EnableIRQ(TOUCH_IRQn);
}

void HAL_TOUCH_MspDeInit(TOUCH_HandleTypeDef* htouch)
{
    __HAL_RCC_TOUCH_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_7);
    HAL_NVIC_DisableIRQ(TOUCH_IRQn);
}