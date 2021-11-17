#include "wm_hal.h"

void HAL_MspInit(void)
{

}

void HAL_PWM_MspInit(PWM_HandleTypeDef *hpwm)
{
    __HAL_RCC_PWM_CLK_ENABLE();
    __HAL_AFIO_REMAP_PWM0(GPIOB, GPIO_PIN_0);
    __HAL_AFIO_REMAP_PWM1(GPIOB, GPIO_PIN_1);
    __HAL_AFIO_REMAP_PWM2(GPIOB, GPIO_PIN_2);
}

void HAL_PWM_MspDeInit(PWM_HandleTypeDef *hpwm)
{
    __HAL_RCC_PWM_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0);
}
