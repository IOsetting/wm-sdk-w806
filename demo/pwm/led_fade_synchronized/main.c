/**
    PWM Frequency： f = 40MHz / Prescaler / (Period + 1)；
                        `````40MHz is fixed, change CPU clock won't affect this
    Duty Cycle：
        Edge Aligned: (Pulse + 1) / (Period + 1)
                Pulse >= Period：PWM alway output high
                Pulse < Period ：PWM High = (Pulse + 1), Low = (Period - Pulse)
                Pulse = 0      ：PWM High = 1, Low = Period

        Center Aligned: (2 * Pulse + 1) / (2 * (Period + 1))
                Pulse > Period ：PWM alway output high
                Pulse <= Period：PWM High = (2 * Pulse + 1), Low = (2 * (Period - Pulse) + 1)
                Pulse = 0      ：PWM High = 1, Low = (2 * Period + 1)
  */
#include <stdio.h>
#include "wm_hal.h"

PWM_HandleTypeDef hpwm;

static void PWM_Init(uint32_t channel);
void Error_Handler(void);

uint32_t ch = PWM_CHANNEL_0;

int main(void)
{
    int i = 0;
    
    SystemClock_Config(CPU_CLK_160M);
    printf("enter main\r\n");
    
    PWM_Init(ch);
    HAL_PWM_Start(&hpwm);
    
    while (1)
    {
        for (i = 0; i < 100; i++)
        {
            HAL_PWM_Duty_Set(&hpwm, i);
            HAL_Delay(20);
        }
        for (i = 99; i >= 0; i--)
        {
            HAL_PWM_Duty_Set(&hpwm, i);
            HAL_Delay(20);
        }
    }
}

static void PWM_Init(uint32_t channel)
{
    hpwm.Instance = PWM;
    hpwm.Init.AutoReloadPreload = PWM_AUTORELOAD_PRELOAD_ENABLE;
    hpwm.Init.CounterMode = PWM_COUNTERMODE_EDGEALIGNED_DOWN;
    hpwm.Init.Prescaler = 4;
    hpwm.Init.Period = 99;    // Frequency = 40,000,000 / 4 / (99 + 1) = 100,000 = 100KHz
    hpwm.Init.Pulse = 19;    // Duty Cycle = (19 + 1) / (99 + 1) = 20%
    hpwm.Init.OutMode = PWM_OUT_MODE_5SYNC; // All 5 channels output the same
    hpwm.Channel = channel;
    
    HAL_PWM_Init(&hpwm);
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