/******************************************************************************
** 
 * \file        main.c
 * \author      IOsetting | iosetting@outlook.com
 * \date        
 * \brief       Demo code of NRF24L01
 * \note        
 * \version     v0.1
 * \ingroup     demo
 * \remarks     test-board: HLK-W806-KIT-V1.0
 *              
******************************************************************************/

#include "wm_hal.h"
#include "nrf24l01.h"

extern NRF24L01_bufStruct NRF24L01_rxbuff;

void Error_Handler(void);

SPI_HandleTypeDef hspi;

static void SPI_Init(void)
{
    hspi.Instance = SPI;
    hspi.Init.Mode = SPI_MODE_MASTER;
    hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi.Init.NSS = SPI_NSS_SOFT;
    hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    hspi.Init.FirstByte = SPI_LITTLEENDIAN;

    if (HAL_SPI_Init(&hspi) != HAL_OK)
    {
        Error_Handler();
    }
}

int main(void)
{
    SystemClock_Config(CPU_CLK_240M);
    NRF24L01_GPIO_Init();
    SPI_Init();

    if (NRF24L01_initCheck(&hspi) != HAL_OK) {
        printf("Check failed\r\n");
        Error_Handler();
    } else {
        printf("Check succeeded\r\n");
    }

    uint8_t tmp[] = {
                0x1F, 0x80, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                0x21, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x28,
                0x31, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x38,
                0x41, 0x12, 0x13, 0x14, 0x15, 0x16, 0x37, 0x48};
    uint8_t txaddr[5] = {0x32, 0x4E, 0x6F, 0x64, 0x65};
    uint8_t rxaddr[5] = {0x32, 0x4E, 0x6F, 0x64, 0x22};
    if (NRF24L01_config(&hspi, txaddr, rxaddr) != HAL_OK) {
        printf("Config failed\r\n");
        Error_Handler();
    } else {
        printf("Config succeeded\r\n");
    }

    NRF24L01_dumpConfig(&hspi);

    if (NRF24L01_MODE == NRF24L01_modeRX)
    {
        while (1);
    }
    else
    {
        uint8_t succ = 0, err = 0;
        while (1)
        {
            if (NRF24L01_writeFast(&hspi, tmp) != HAL_OK)
            {
                NRF24L01_resetTX(&hspi);
                err++;
            }
            else
            {
                succ++;
            }
            if (err == 255 || succ == 255)
            {
                printf("Fail/Succ: %d/%d\r\n", err, succ);
                err = 0;
                succ = 0;
            }
        }
    }
}

void HAL_GPIO_EXTI_Callback(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin)
{
    if ((GPIOx == NRF24L01_IRQ_PORT) && (GPIO_Pin == NRF24L01_IRQ_PIN))
    {
        NRF24L01_handelIrqFlag(&hspi);
        for (uint8_t i = 0; i < NRF24L01_PLOAD_WIDTH; i++)
        {
            printf("%02X ", NRF24L01_rxbuff.buf[i]);
        }
        printf("\r\n");
    }
}

void Error_Handler(void)
{
    while (1);
}

void assert_failed(uint8_t *file, uint32_t line)
{
    printf("Wrong parameters value: file %s on line %d\r\n", file, line);
}
