/******************************************************************************
** 
 * \file        auto_dl.c
 * \author      Xu Ruijun | 1687701765@qq.com
 * \date        
 * \brief       Reset device with UART0 AT+Z command
 * \note        Set USE_UART0_AUTO_DL = 1 to enable this feature
 * \version     
 * \ingroup     
 * \remarks     
 *
******************************************************************************/

#include "wm_hal.h"

#if USE_UART0_AUTO_DL

const uint32_t timeout = 5;
const uint8_t atz[] = "AT+Z\r\n";
const uint8_t* p = atz;
uint32_t last = 0;

void Reset_to_ROM()
{
    RCC->RST = 0;  // reset all peripheral
    uint32_t rv = *(uint32_t*)(0x00000000U); // get reset vector
    ((void (*)())(rv))(); // go to ROM
}

__attribute__((weak)) void USER_UART0_RX(uint8_t ch)
{
    UNUSED(UART0);
    UNUSED(ch);
}

void Auto_DL_Handler()
{
    uint8_t ch;
    if(UART0->FIFOS & 0xFC0){
        if(last > HAL_GetTick() + timeout){
            p = atz; // timeout
        }
        last = HAL_GetTick();
        do{
            ch = (uint8_t)UART0->RDW;
            if(*p++ == ch){
                if(p == &atz[6]){
                    Reset_to_ROM();
                    p = atz;  // reset faild
                }
            }else{
                p = atz;  // not match
            }
            USER_UART0_RX(ch);
        }while(UART0->FIFOS & 0xFC0);
        UART0->INTS = UART_INTS_RL;
    }
}

__attribute__((isr)) void UART0_IRQHandler(void)
{
    Auto_DL_Handler();
}

#endif
