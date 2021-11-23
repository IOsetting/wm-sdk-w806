#include "auto_dl.h"
#include "wm_hal.h"

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

__attribute__((weak)) void USER_UART0_RX()
{
    //add your code
}

void Auto_DL_Handler()
{
    uint8_t rx_ch;
    if(UART0->FIFOS & 0xFC0){
        if(last > HAL_GetTick() + timeout){
            p = atz; // timeout
        }
        last = HAL_GetTick();
        do{
            if(*p++ == (uint8_t)UART0->RDW){
                if(p == &atz[6]){
                    Reset_to_ROM();
                    p = atz;  // reset faild
                }
            }else{
                p = atz;  // not match
            }
            USER_UART0_RX(rx_ch);
        }while(UART0->FIFOS & 0xFC0);
    }
}