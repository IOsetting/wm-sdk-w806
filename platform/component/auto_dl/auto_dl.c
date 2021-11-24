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
    CLEAR_REG(RCC->RST);  // reset all peripheral
    uint32_t rv = *(uint32_t*)(0x00000000U); // get reset vector
    ((void (*)())(rv))(); // go to ROM
}

__attribute__((weak)) void USER_UART0_RX(uint8_t ch)
{
    UNUSED(UART0);
    UNUSED(ch);
}

#define RX_COUNT ((UART0->FIFOS & UART_FIFOS_RFC_Msk) >> UART_FIFOS_RFC_Pos)

void Auto_DL_Handler()
{
    uint8_t ch;
    if(RX_COUNT){
        if(last > HAL_GetTick() + timeout){
            p = atz; // timeout
        }
        last = HAL_GetTick();
        do{
            SET_BIT(UART0->INTS, UART_INTS_RL); // clear interrupt flag
            ch = (uint8_t)READ_REG(UART0->RDW);
            if(*p++ == ch){
                if(p >= atz + strlen(atz)){
                    Reset_to_ROM();
                    p = atz;  // reset faild
                }
            }else{
                p = atz;  // not match
            }
            USER_UART0_RX(ch);
        }while(RX_COUNT);
    }
}

__attribute__((isr)) void UART0_IRQHandler(void)
{
    Auto_DL_Handler();
}

#endif
