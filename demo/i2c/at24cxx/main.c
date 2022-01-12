
#include <stdio.h>
#include "wm_hal.h"

I2C_HandleTypeDef hi2c;
void Error_Handler(void);
static void I2C_Init(void);
static void GPIO_Init(void);

#define DEVICE_ADDR    0xA0

int main(void)
{
    char *w_buf = "AT24CXX I2C WRITE THEN READ TEST FOR LONG TIME";
    uint8_t len = strlen(w_buf);
    uint8_t r_buf[50] = {0};
    int i = 0;
    
    SystemClock_Config(CPU_CLK_160M);
    printf("enter main\r\n");
    printf("len = %d\n", len);
    GPIO_Init();
    I2C_Init();
    for (i = 0; i < len; i++)
    {
        HAL_I2C_Write(&hi2c, DEVICE_ADDR, 0x00 + i, (uint8_t *)(w_buf + i), 1);
        HAL_Delay(10);
    }
    
    while (1)
    {
        memset(r_buf, 0, len);
        if (HAL_I2C_Read(&hi2c, DEVICE_ADDR, 0x00, r_buf, len) != HAL_OK)
        {
            printf("read failed\r\n");
        }
        if (memcmp(w_buf, r_buf, len))
        {
            printf("err: %s\r\n", r_buf);
        }
        i++;
        if ((i % 10000) == 0)
        {
            printf("i = %d\r\n", i);
        }
    }

    while (1)
    {
        HAL_Delay(1000);
    }
}

static void I2C_Init(void)
{
    hi2c.Instance = I2C;
    hi2c.Frequency = 400000;
    HAL_I2C_Init(&hi2c);
}

static void GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
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