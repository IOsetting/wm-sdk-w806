#include <stdio.h>
#include "wm_hal.h"

PWM_HandleTypeDef pwm[3];
int i, j, m[3] = {0}, d[3] = {0, 33, 66};

static void PWM_Init(PWM_HandleTypeDef *hpwm, uint32_t channel);
void Error_Handler(void);

int main(void)
{
    SystemClock_Config(CPU_CLK_160M);
    printf("enter main\r\n");

    for (i = 2; i >= 0; i--)
    {
        PWM_Init(&pwm[i], PWM_CHANNEL_0 + i);
        HAL_PWM_Start(&pwm[i]);
    }

    while (1)
    {
        for (i = 0; i < 3; i++)
        {
            if (m[i] == 0) // Increasing
            {
                HAL_PWM_Duty_Set(&pwm[i], d[i]++);
                if (d[i] == 100)
                {
                    m[i] = 1;
                }
            }
            else // Decreasing
            {
                HAL_PWM_Duty_Set(&pwm[i], d[i]--);
                if (d[i] == 0)
                {
                    m[i] = 0;
                }
            }
        }
        HAL_Delay(20);
    }
}

static void PWM_Init(PWM_HandleTypeDef *hpwm, uint32_t channel)
{
    hpwm->Instance = PWM;
    hpwm->Init.AutoReloadPreload = PWM_AUTORELOAD_PRELOAD_ENABLE;
    hpwm->Init.CounterMode = PWM_COUNTERMODE_EDGEALIGNED_DOWN;
    hpwm->Init.Prescaler = 4;
    hpwm->Init.Period = 99;    // Frequency = 40,000,000 / 4 / (99 + 1) = 100,000 = 100KHz
    hpwm->Init.Pulse = 19;    // Duty Cycle = (19 + 1) / (99 + 1) = 20%
    hpwm->Init.OutMode = PWM_OUT_MODE_INDEPENDENT; // Independent mode
    hpwm->Channel = channel;
    HAL_PWM_Init(hpwm);
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