/******************************************************************************
** 
 * \file        main.c
 * \author      IOsetting | iosetting@outlook.com
 * \date        
 * \brief       Demo code of PWM in independent mode
 * \note        This will drive 3 on-board LEDs to show fade effect
 * \version     v0.1
 * \ingroup     demo
 * \remarks     test-board: HLK-W806-KIT-V1.0
 *              PWM Frequency = 40MHz / Prescaler / (Period + 1)；
                Duty Cycle(Edge Aligned)   = (Pulse + 1) / (Period + 1)
                Duty Cycle(Center Aligned) = (2 * Pulse + 1) / (2 * (Period + 1))
 *
******************************************************************************/

#include <stdio.h>
#include "wm_hal.h"

#define DUTY_MAX 100
#define DUTY_MIN 50
PWM_HandleTypeDef pwm[3];
int i, j, m[3] = {0}, d[3] = {DUTY_MIN, (DUTY_MIN + DUTY_MAX) / 2, DUTY_MAX - 1};

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
                if (d[i] == DUTY_MAX)
                {
                    m[i] = 1;
                }
            }
            else // Decreasing
            {
                HAL_PWM_Duty_Set(&pwm[i], d[i]--);
                if (d[i] == DUTY_MIN)
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
    hpwm->Init.Pulse = 19;     // Duty Cycle = (19 + 1) / (99 + 1) = 20%
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