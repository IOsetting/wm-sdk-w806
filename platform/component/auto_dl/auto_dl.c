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

#define __AUTO_DL_UART_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->INTS |= __FLAG__)
#define __AUTO_DL_TIMEOUT 5
#define __AUTO_DL_BUF_SIZE 32

const uint8_t auto_dl_cmd[] = "AT+Z\r\n";
uint8_t auto_dl_buf[__AUTO_DL_BUF_SIZE] = {0}, auto_dl_buf_pt = 0, auto_dl_cmd_pt = 0;
uint32_t auto_dl_act_ts = 0;

void AUTO_DL_Reset(void)
{
    CLEAR_REG(RCC->RST);                     // reset all peripherals
    uint32_t rv = *(uint32_t*)(0x00000000U); // get reset vector
    ((void (*)())(rv))();                    // go to ROM
}

__attribute__((weak)) void USER_UART0_RX(uint8_t ch)
{
    UNUSED(ch);
}

void AUTO_DL_UART_IRQHandler(USART_TypeDef* huart)
{
    uint8_t ch, count;
    uint32_t ts, isrflags = READ_REG(huart->INTS), isrmasks = READ_REG(huart->INTM);
    // Clear interrupts
    __AUTO_DL_UART_CLEAR_FLAG(huart, isrflags);

    if (((isrflags & UART_RX_INT_FLAG) != RESET) && ((isrmasks & UART_RX_INT_FLAG) == RESET))
    {
        /**
         *   1) Data always comes in as single bytes, so the count is always 1(or 0);
         *   2) Each byte will comes in twice, the second time with count=0 will be ignored;
         */
        count = ((READ_REG(huart->FIFOS) & UART_FIFOS_RFC) >> UART_FIFOS_RFC_Pos);
        while (count-- > 0)
        {
            ts = HAL_GetTick();
            if ((ts - auto_dl_act_ts) > __AUTO_DL_TIMEOUT)
            {
                // Restart the comparison if timeout
                auto_dl_cmd_pt = 0;
            }
            // auto_dl_act_ts: Record last active timestamp, restart the comparison if timeout.
            auto_dl_act_ts = ts;
            ch = (uint8_t)(huart->RDW);
            auto_dl_buf[auto_dl_buf_pt++] = ch;
            if (auto_dl_buf_pt == __AUTO_DL_BUF_SIZE) auto_dl_buf_pt = 0;
            // Compare
            if (auto_dl_cmd[auto_dl_cmd_pt] == ch)
            {
                auto_dl_cmd_pt++;
                if (auto_dl_cmd_pt == 6)
                {
                    AUTO_DL_Reset();
                }
            }
            else
            {
                // Restart the comparison
                auto_dl_cmd_pt = 0;
            }
            USER_UART0_RX(ch);
        }
    }

    if (((isrflags & UART_INTS_TL) != RESET) && ((isrmasks & UART_INTM_RL) == RESET))
    {
        //UART_Transmit_IT(huart);
    }

    if (((isrflags & UART_INTS_TEMPT) != RESET) && ((isrmasks & UART_INTM_TEMPT) == RESET))
    {
        //UART_EndTransmit_IT(huart);
    }
}

__attribute__((isr)) void UART0_IRQHandler(void)
{
    AUTO_DL_UART_IRQHandler(UART0);
}

#endif
