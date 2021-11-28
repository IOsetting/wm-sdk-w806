/******************************************************************************
** 
 * \file        nrf24l01.c
 * \author      IOsetting | iosetting@outlook.com
 * \date        
 * \brief       Library of NRF24L01 on W806
 * \note        
 * \version     v0.1
 * \ingroup     demo
 * \remarks     test-board: HLK-W806-KIT-V1.0
 * 
 *              Defined in nrf24l01.h
 *              NRF24L01_MODE:
 *                  RX mode or TX mode
 *              Pin Wiring:
 *                  B10   -> IRQ
 *                  B11   -> CE
 *                  B14   -> CS, Chip Select
 *                  B15   -> SCK, SCL, CLK, Clock
 *                  B16   -> MISO
 *                  B17   -> MOSI
 *                  GND   -> GND
 *                  3.3V  -> VCC
 * 
 *              Baud rate should be under 10M (prescaler = 4, 8 or larger)
 * 
******************************************************************************/

#include "nrf24l01.h"
#include <stdio.h>

extern SPI_HandleTypeDef hspi;

uint8_t nrf24_state;

NRF24L01_bufStruct NRF24L01_txbuff;
NRF24L01_bufStruct NRF24L01_rxbuff;
uint8_t *NRF24L01_txbuffpt = (uint8_t *)(&NRF24L01_txbuff);
uint8_t *NRF24L01_rxbuffpt = (uint8_t *)(&NRF24L01_rxbuff);

/**
 * Initialize nRF24L01 GPIO ports
 */
void NRF24L01_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin  = NRF24L01_CE_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(NRF24L01_CE_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(NRF24L01_CE_PORT, NRF24L01_CE_PIN, GPIO_PIN_RESET);

    if (NRF24L01_MODE == NRF24L01_modeRX)
    {
        GPIO_InitStruct.Pin = NRF24L01_IRQ_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(NRF24L01_IRQ_PORT, &GPIO_InitStruct);
        HAL_NVIC_SetPriority(NRF24L01_IRQn, 0);
        HAL_NVIC_EnableIRQ(NRF24L01_IRQn);
    }
    else
    {
        GPIO_InitStruct.Pin = NRF24L01_IRQ_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(NRF24L01_IRQ_PORT, &GPIO_InitStruct);
    }
}

/**
 * Send a one-byte command or one-byte command with one-byte argument
 */
HAL_StatusTypeDef NRF24L01_cmd(SPI_HandleTypeDef *hspi, uint8_t cmd, uint8_t arg)
{
    NRF24L01_txbuff.opt = cmd;
    NRF24L01_txbuff.buf[0] = arg;
    NRF24L01_CS_LOW;
    if (HAL_SPI_TransmitReceive(hspi, NRF24L01_txbuffpt, NRF24L01_rxbuffpt, 2, NRF24L01_SPI_TIMEOUT) != HAL_OK)
    {
        return HAL_ERROR;
    }
    NRF24L01_CS_HIGH;
    //printf("cmd %02X:%02X, rx %02X\r\n", NRF24L01_txbuff.opt, NRF24L01_txbuff.buf[0], NRF24L01_rxbuff.opt);
    return HAL_OK;
}

/**
 * Send a one-byte command and read the output
 */
HAL_StatusTypeDef NRF24L01_read(SPI_HandleTypeDef *hspi, uint8_t cmd, uint8_t len)
{
    NRF24L01_txbuff.opt = cmd;
    memset(NRF24L01_txbuff.buf, 0, len);
    memset(NRF24L01_rxbuff.buf, 0, len);
    NRF24L01_CS_LOW;
    if (HAL_SPI_TransmitReceive(hspi, NRF24L01_txbuffpt, NRF24L01_rxbuffpt, len + 1, NRF24L01_SPI_TIMEOUT) != HAL_OK)
    {
        return HAL_ERROR;
    }
    NRF24L01_CS_HIGH;
    //printf("read 0x%02X\r\n", NRF24L01_rxbuff.opt);
    return HAL_OK;
}

/**
 * Send a one-byte command and followed with byte array input
 */
HAL_StatusTypeDef NRF24L01_write(SPI_HandleTypeDef *hspi, uint8_t cmd, const uint8_t *tx, uint8_t len)
{
    NRF24L01_txbuff.opt = cmd;
    memset(NRF24L01_txbuff.buf, 0, NRF24L01_PLOAD_WIDTH);
    memcpy(NRF24L01_txbuff.buf, tx, len);
    NRF24L01_CS_LOW;
    if (HAL_SPI_TransmitReceive(hspi, NRF24L01_txbuffpt, NRF24L01_rxbuffpt, len + 1, NRF24L01_SPI_TIMEOUT) != HAL_OK)
    {
        return HAL_ERROR;
    }
    NRF24L01_CS_HIGH;
    //printf("write 0x%02X\r\n", NRF24L01_rxbuff.opt);
    return HAL_OK;
}

/**
 * Check if nRF24L01 wiring correctly
 */
HAL_StatusTypeDef NRF24L01_initCheck(SPI_HandleTypeDef *hspi)
{
    uint8_t i;
    uint8_t *addr = (uint8_t *)NRF24L01_TEST_ADDR;

    // Flush TX FIFO and clear IRQ flags
    NRF24L01_cmd(hspi, NRF24L01_CMD_FLUSH_TX, NRF24L01_CMD_NOP);
    NRF24L01_clearIRQFlags(hspi);

    if (NRF24L01_write(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_TX_ADDR, addr, NRF24L01_ADDR_WIDTH) != HAL_OK)
    {
        return HAL_ERROR;
    }
    if (NRF24L01_read(hspi, NRF24L01_CMD_R_REGISTER | NRF24L01_REG_TX_ADDR, NRF24L01_ADDR_WIDTH) != HAL_OK)
    {
        return HAL_ERROR;
    }
    for (i = 0; i < NRF24L01_ADDR_WIDTH; i++)
    {
        if (NRF24L01_rxbuff.buf[i] != *addr++)
            return HAL_ERROR;
    }
    return HAL_OK;
}

uint8_t NRF24L01_readStatus(SPI_HandleTypeDef *hspi)
{
    NRF24L01_cmd(hspi, NRF24L01_CMD_R_REGISTER | NRF24L01_REG_STATUS, NRF24L01_CMD_NOP);
    return NRF24L01_rxbuff.opt;
}

/**
 * Flush RX FIFO, used in RX mode. Should not be executed during transmission of acknowledge,
 * that is, acknowledge package will not be completed.
 */
HAL_StatusTypeDef NRF24L01_flushRX(SPI_HandleTypeDef *hspi)
{
    return NRF24L01_cmd(hspi, NRF24L01_CMD_FLUSH_RX, NRF24L01_CMD_NOP);
}

/**
 * Flush TX FIFO, used in TX mode
 */
HAL_StatusTypeDef NRF24L01_flushTX(SPI_HandleTypeDef *hspi)
{
    return NRF24L01_cmd(hspi, NRF24L01_CMD_FLUSH_TX, NRF24L01_CMD_NOP);
}

/**
* Clear RX_DR, TX_DS and MAX_RT bits of the STATUS register
*/
HAL_StatusTypeDef NRF24L01_clearIRQFlags(SPI_HandleTypeDef *hspi)
{
  return NRF24L01_cmd(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_STATUS, NRF24L01_STATUS_IT_BITS);
}

/**
* Common configurations
*/
HAL_StatusTypeDef NRF24L01_config(SPI_HandleTypeDef *hspi, uint8_t *tx_addr, uint8_t *rx_addr)
{
  // RX P0 payload width
  NRF24L01_cmd(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_RX_PW_P0, NRF24L01_PLOAD_WIDTH);
  NRF24L01_cmd(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_RX_PW_P1, NRF24L01_PLOAD_WIDTH);
  // Enable auto ACK
  NRF24L01_cmd(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_EN_AA, 0x3f);
  // Enable RX pipe 0
  NRF24L01_cmd(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_EN_RXADDR, 0x3f);
  // RF channel 40 - 2.440GHz = 2.400G  + 0.040G
  NRF24L01_cmd(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_RF_CH, 40);
  // 000+0+[0:1Mbps,1:2Mbps]+[00:-18dbm,01:-12dbm,10:-6dbm,11:0dbm]+[0:LNA_OFF,1:LNA_ON]
  // 01:1Mbps,-18dbm; 03:1Mbps,-12dbm; 05:1Mbps,-6dbm; 07:1Mbps,0dBm
  // 09:2Mbps,-18dbm; 0b:2Mbps,-12dbm; 0d:2Mbps,-6dbm; 0f:2Mbps,0dBm, 
  NRF24L01_cmd(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_RF_SETUP, 0x07);
  // 0A:delay=250us,count=10, 1A:delay=500us,count=10
  NRF24L01_cmd(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_SETUP_RETR, 0x0A);
  NRF24L01_CE_LOW;
  if (NRF24L01_MODE == NRF24L01_modeRX)
  {
    NRF24L01_write(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_TX_ADDR, tx_addr, NRF24L01_ADDR_WIDTH);
    NRF24L01_write(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_RX_ADDR_P0, tx_addr, NRF24L01_ADDR_WIDTH);
    NRF24L01_write(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_RX_ADDR_P1, rx_addr, NRF24L01_ADDR_WIDTH);
    /**
    REG 0x00: 
    0)PRIM_RX     0:TX             1:RX
    1)PWR_UP      0:OFF            1:ON
    2)CRCO        0:8bit CRC       1:16bit CRC
    3)EN_CRC      Enabled if any of EN_AA is high
    4)MASK_MAX_RT 0:IRQ low        1:NO IRQ
    5)MASK_TX_DS  0:IRQ low        1:NO IRQ
    6)MASK_RX_DR  0:IRQ low        1:NO IRQ
    7)Reserved    0
    */
    NRF24L01_cmd(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_CONFIG, 0x0F); //RX,PWR_UP,CRC16,EN_CRC
    NRF24L01_flushRX(hspi);
  }
  else 
  {
    NRF24L01_write(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_TX_ADDR, tx_addr, NRF24L01_ADDR_WIDTH);
    NRF24L01_write(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_RX_ADDR_P0, tx_addr, NRF24L01_ADDR_WIDTH);
    NRF24L01_write(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_RX_ADDR_P1, rx_addr, NRF24L01_ADDR_WIDTH);
    NRF24L01_cmd(hspi, NRF24L01_CMD_W_REGISTER + NRF24L01_REG_CONFIG, 0x0E); //TX,PWR_UP,CRC16,EN_CRC
  }
  NRF24L01_CE_HIGH;
  return HAL_OK;
}

/**
* Start NRF24L01 in TX mode
*/
HAL_StatusTypeDef NRF24L01_txMode(SPI_HandleTypeDef *hspi, uint8_t *tx_addr, uint8_t *rx_addr)
{
  NRF24L01_write(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_TX_ADDR, tx_addr, NRF24L01_ADDR_WIDTH);
  NRF24L01_write(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_RX_ADDR_P0, tx_addr, NRF24L01_ADDR_WIDTH);
  NRF24L01_write(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_RX_ADDR_P1, rx_addr, NRF24L01_ADDR_WIDTH);
  NRF24L01_cmd(hspi, NRF24L01_CMD_W_REGISTER + NRF24L01_REG_CONFIG, 0x0E); //TX,PWR_UP,CRC16,EN_CRC
  return HAL_OK;
}

/**
* Start NRF24L01 in RX mode
*/
HAL_StatusTypeDef NRF24L01_rxMode(SPI_HandleTypeDef *hspi, uint8_t *tx_addr, uint8_t *rx_addr)
{
    NRF24L01_CE_LOW;
    NRF24L01_write(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_TX_ADDR, tx_addr, NRF24L01_ADDR_WIDTH);
    NRF24L01_write(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_RX_ADDR_P0, rx_addr, NRF24L01_ADDR_WIDTH);
    // NRF24L01_write(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_RX_ADDR_P1, rx_addr, NRF24L01_ADDR_WIDTH);
    /**
    REG 0x00:
    0)PRIM_RX     0:TX             1:RX
    1)PWR_UP      0:OFF            1:ON
    2)CRCO        0:8bit CRC       1:16bit CRC
    3)EN_CRC      Enabled if any of EN_AA is high
    4)MASK_MAX_RT 0:IRQ low        1:NO IRQ
    5)MASK_TX_DS  0:IRQ low        1:NO IRQ
    6)MASK_RX_DR  0:IRQ low        1:NO IRQ
    7)Reserved    0
    */
    NRF24L01_cmd(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_CONFIG, 0x0F); // RX,PWR_UP,CRC16,EN_CRC
    NRF24L01_CE_HIGH;
    NRF24L01_flushRX(hspi);
    return HAL_OK;
}

void NRF24L01_startFastWrite(SPI_HandleTypeDef *hspi, const void *txbuf)
{
    NRF24L01_write(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_CMD_W_TX_PAYLOAD, txbuf, NRF24L01_PLOAD_WIDTH);
    NRF24L01_CE_HIGH;
}

HAL_StatusTypeDef NRF24L01_writeFast(SPI_HandleTypeDef *hspi, const void *txbuf)
{
    // Blocking only if FIFO is full. This will loop and block until TX is successful or fail
    while (NRF24L01_readStatus(hspi) & NRF24L01_STATUS_TX_FULL)
    {
        //printf("%02X\r\n", NRF24L01_rxbuff.opt);
        if (NRF24L01_rxbuff.opt & NRF24L01_STATUS_MAX_RT)
        {
            return HAL_ERROR;
        }
    }
    NRF24L01_startFastWrite(hspi, txbuf);
    return HAL_OK;
}

void NRF24L01_resetTX(SPI_HandleTypeDef *hspi)
{
    NRF24L01_cmd(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_STATUS, NRF24L01_STATUS_MAX_RT); // Clear max retry flag
    NRF24L01_CE_LOW;
    NRF24L01_CE_HIGH;
}

void NRF24L01_checkFlag(SPI_HandleTypeDef *hspi, uint8_t *tx_ds, uint8_t *max_rt, uint8_t *rx_dr)
{
    // Read the status & reset the status in one easy call
    NRF24L01_cmd(hspi, NRF24L01_CMD_W_REGISTER | NRF24L01_REG_STATUS, NRF24L01_STATUS_IT_BITS);
    // Report what happened
    *tx_ds = NRF24L01_rxbuff.opt & NRF24L01_STATUS_TX_DS;
    *max_rt = NRF24L01_rxbuff.opt & NRF24L01_STATUS_MAX_RT;
    *rx_dr = NRF24L01_rxbuff.opt & NRF24L01_STATUS_RX_DR;
}

HAL_StatusTypeDef NRF24L01_rxAvailable(SPI_HandleTypeDef *hspi, uint8_t *pipe_num)
{
    NRF24L01_cmd(hspi, NRF24L01_CMD_R_REGISTER | NRF24L01_REG_STATUS, NRF24L01_CMD_NOP);
    uint8_t pipe = (NRF24L01_rxbuff.opt >> 1) & 0x07;
    if (pipe > 5)
        return HAL_ERROR;
    // If the caller wants the pipe number, include that
    if (pipe_num)
        *pipe_num = pipe;

    return HAL_OK;
}

void NRF24L01_handelIrqFlag(SPI_HandleTypeDef *hspi)
{
    uint8_t tx_ds, max_rt, rx_dr, pipe_num;
    NRF24L01_checkFlag(hspi, &tx_ds, &max_rt, &rx_dr);
    if (NRF24L01_rxAvailable(hspi, &pipe_num) == HAL_OK)
    {
        NRF24L01_read(hspi, NRF24L01_CMD_R_RX_PAYLOAD, NRF24L01_PLOAD_WIDTH);
    }
}

/**
 * Dump nRF24L01 configuration for debug
 */
void NRF24L01_dumpConfig(SPI_HandleTypeDef *hspi)
{
    uint8_t i, j;

    // CONFIG
    NRF24L01_cmd(hspi, NRF24L01_REG_CONFIG, NRF24L01_CMD_NOP);
    printf("[0x%02X] 0x%02X MASK:%02X CRC:%02X PWR:%s MODE:%s\r\n",
           NRF24L01_REG_CONFIG,
           NRF24L01_rxbuff.buf[0],
           NRF24L01_rxbuff.buf[0] >> 4,
           (NRF24L01_rxbuff.buf[0] & 0x0c) >> 2,
           (NRF24L01_rxbuff.buf[0] & 0x02) ? "ON" : "OFF",
           (NRF24L01_rxbuff.buf[0] & 0x01) ? "RX" : "TX");

    // EN_AA
    NRF24L01_cmd(hspi, NRF24L01_REG_EN_AA, NRF24L01_CMD_NOP);
    printf("[0x%02X] 0x%02X ENAA: ", NRF24L01_REG_EN_AA, NRF24L01_rxbuff.buf[0]);
    for (j = 0; j < 6; j++)
    {
        printf("[P%1u%s]%s", j,
               (NRF24L01_rxbuff.buf[0] & (1 << j)) ? "+" : "-",
               (j == 5) ? "\r\n" : " ");
    }

    // EN_RXADDR
    NRF24L01_cmd(hspi, NRF24L01_REG_EN_RXADDR, NRF24L01_CMD_NOP);
    printf("[0x%02X] 0x%02X EN_RXADDR: ", NRF24L01_REG_EN_RXADDR, NRF24L01_rxbuff.buf[0]);
    for (j = 0; j < 6; j++)
    {
        printf("[P%1u%s]%s", j,
               (NRF24L01_rxbuff.buf[0] & (1 << j)) ? "+" : "-",
               (j == 5) ? "\r\n" : " ");
    }

    // SETUP_AW
    NRF24L01_cmd(hspi, NRF24L01_REG_SETUP_AW, NRF24L01_CMD_NOP);
    int8_t ww = (NRF24L01_rxbuff.buf[0] & 0x03) + 2;
    printf("[0x%02X] 0x%02X EN_RXADDR=%03X (address width = %u)\r\n",
           NRF24L01_REG_SETUP_AW, NRF24L01_rxbuff.buf[0], NRF24L01_rxbuff.buf[0] & 0x03, ww);

    // SETUP_RETR
    NRF24L01_cmd(hspi, NRF24L01_REG_SETUP_RETR, NRF24L01_CMD_NOP);
    printf("[0x%02X] 0x%02X ARD=%04X ARC=%04X (retr.delay=%uus, count=%u)\r\n",
           NRF24L01_REG_SETUP_RETR,
           NRF24L01_rxbuff.buf[0],
           NRF24L01_rxbuff.buf[0] >> 4,
           NRF24L01_rxbuff.buf[0] & 0x0F,
           ((NRF24L01_rxbuff.buf[0] >> 4) * 250) + 250,
           NRF24L01_rxbuff.buf[0] & 0x0F);

    // RF_CH
    NRF24L01_cmd(hspi, NRF24L01_REG_RF_CH, NRF24L01_CMD_NOP);
    printf("[0x%02X] 0x%02X (%.3uGHz)\r\n", NRF24L01_REG_RF_CH, NRF24L01_rxbuff.buf[0], 2400 + NRF24L01_rxbuff.buf[0]);

    // RF_SETUP
    NRF24L01_cmd(hspi, NRF24L01_REG_RF_SETUP, NRF24L01_CMD_NOP);
    printf("[0x%02X] 0x%02X CONT_WAVE:%s PLL_LOCK:%s DataRate=",
           NRF24L01_REG_RF_SETUP,
           NRF24L01_rxbuff.buf[0],
           (NRF24L01_rxbuff.buf[0] & 0x80) ? "ON" : "OFF",
           (NRF24L01_rxbuff.buf[0] & 0x80) ? "ON" : "OFF");
    switch ((NRF24L01_rxbuff.buf[0] & 0x28) >> 3)
    {
    case 0x00:
        printf("1M");
        break;
    case 0x01:
        printf("2M");
        break;
    case 0x04:
        printf("250k");
        break;
    default:
        printf("???");
        break;
    }
    printf("pbs RF_PWR=");
    switch ((NRF24L01_rxbuff.buf[0] & 0x06) >> 1)
    {
    case 0x00:
        printf("-18");
        break;
    case 0x01:
        printf("-12");
        break;
    case 0x02:
        printf("-6");
        break;
    case 0x03:
        printf("0");
        break;
    default:
        printf("???");
        break;
    }
    printf("dBm\r\n");

    // STATUS
    NRF24L01_cmd(hspi, NRF24L01_REG_STATUS, NRF24L01_CMD_NOP);
    printf("[0x%02X] 0x%02X IRQ:%03X RX_PIPE:%u TX_FULL:%s\r\n",
           NRF24L01_REG_STATUS,
           NRF24L01_rxbuff.opt,
           (NRF24L01_rxbuff.opt & 0x70) >> 4,
           (NRF24L01_rxbuff.opt & 0x0E) >> 1,
           (NRF24L01_rxbuff.opt & 0x01) ? "YES" : "NO");

    // OBSERVE_TX
    NRF24L01_cmd(hspi, NRF24L01_REG_OBSERVE_TX, NRF24L01_CMD_NOP);
    printf("[0x%02X] 0x%02X PLOS_CNT=%u ARC_CNT=%u\r\n",
           NRF24L01_REG_OBSERVE_TX, NRF24L01_rxbuff.buf[0], NRF24L01_rxbuff.buf[0] >> 4, NRF24L01_rxbuff.buf[0] & 0x0F);

    // RPD
    NRF24L01_cmd(hspi, NRF24L01_REG_RPD, NRF24L01_CMD_NOP);
    printf("[0x%02X] 0x%02X RPD=%s\r\n",
           NRF24L01_REG_RPD, NRF24L01_rxbuff.buf[0], (NRF24L01_rxbuff.buf[0] & 0x01) ? "YES" : "NO");

    // RX_ADDR_P0
    NRF24L01_read(hspi, NRF24L01_REG_RX_ADDR_P0, NRF24L01_ADDR_WIDTH);
    printf("[0x%02X] RX_ADDR_P0 \"", NRF24L01_REG_RX_ADDR_P0);
    for (i = 0; i < NRF24L01_ADDR_WIDTH; i++)
        printf("%X ", NRF24L01_rxbuff.buf[i]);
    printf("\"\r\n");

    // RX_ADDR_P1
    NRF24L01_read(hspi, NRF24L01_REG_RX_ADDR_P1, NRF24L01_ADDR_WIDTH);
    printf("[0x%02X] RX_ADDR_P1 \"", NRF24L01_REG_RX_ADDR_P1);
    for (i = 0; i < NRF24L01_ADDR_WIDTH; i++)
        printf("%X ", NRF24L01_rxbuff.buf[i]);
    printf("\"\r\n");

    // RX_ADDR_P2
    printf("[0x%02X] RX_ADDR_P2 \"", NRF24L01_REG_RX_ADDR_P2);
    for (i = 0; i < NRF24L01_ADDR_WIDTH - 1; i++)
        printf("%X ", NRF24L01_rxbuff.buf[i]);
    NRF24L01_cmd(hspi, NRF24L01_REG_RX_ADDR_P2, NRF24L01_CMD_NOP);
    printf("%X\"\r\n", NRF24L01_rxbuff.buf[0]);

    // RX_ADDR_P3
    printf("[0x%02X] RX_ADDR_P3 \"", NRF24L01_REG_RX_ADDR_P3);
    for (i = 0; i < NRF24L01_ADDR_WIDTH - 1; i++)
        printf("%X ", NRF24L01_rxbuff.buf[i]);
    NRF24L01_cmd(hspi, NRF24L01_REG_RX_ADDR_P3, NRF24L01_CMD_NOP);
    printf("%X\"\r\n", NRF24L01_rxbuff.buf[0]);

    // RX_ADDR_P4
    printf("[0x%02X] RX_ADDR_P4 \"", NRF24L01_REG_RX_ADDR_P4);
    for (i = 0; i < NRF24L01_ADDR_WIDTH - 1; i++)
        printf("%X ", NRF24L01_rxbuff.buf[i]);
    NRF24L01_cmd(hspi, NRF24L01_REG_RX_ADDR_P4, NRF24L01_CMD_NOP);
    printf("%X\"\r\n", NRF24L01_rxbuff.buf[0]);

    // RX_ADDR_P5
    printf("[0x%02X] RX_ADDR_P5 \"", NRF24L01_REG_RX_ADDR_P5);
    for (i = 0; i < NRF24L01_ADDR_WIDTH - 1; i++)
        printf("%X ", NRF24L01_rxbuff.buf[i]);
    NRF24L01_cmd(hspi, NRF24L01_REG_RX_ADDR_P5, NRF24L01_CMD_NOP);
    printf("%X\"\r\n", NRF24L01_rxbuff.buf[0]);

    // TX_ADDR
    NRF24L01_read(hspi, NRF24L01_REG_TX_ADDR, NRF24L01_ADDR_WIDTH);
    printf("[0x%02X] TX_ADDR \"", NRF24L01_REG_TX_ADDR);
    for (i = 0; i < NRF24L01_ADDR_WIDTH; i++)
        printf("%02X ", NRF24L01_rxbuff.buf[i]);
    printf("\"\r\n");

    // RX_PW_P0
    NRF24L01_cmd(hspi, NRF24L01_REG_RX_PW_P0, NRF24L01_CMD_NOP);
    printf("[0x%02X] RX_PW_P0=%u\r\n", NRF24L01_REG_RX_PW_P0, NRF24L01_rxbuff.buf[0]);

    // RX_PW_P1
    NRF24L01_cmd(hspi, NRF24L01_REG_RX_PW_P1, NRF24L01_CMD_NOP);
    printf("[0x%02X] RX_PW_P1=%u\r\n", NRF24L01_REG_RX_PW_P1, NRF24L01_rxbuff.buf[0]);

    // RX_PW_P2
    NRF24L01_cmd(hspi, NRF24L01_REG_RX_PW_P2, NRF24L01_CMD_NOP);
    printf("[0x%02X] RX_PW_P2=%u\r\n", NRF24L01_REG_RX_PW_P2, NRF24L01_rxbuff.buf[0]);

    // RX_PW_P3
    NRF24L01_cmd(hspi, NRF24L01_REG_RX_PW_P3, NRF24L01_CMD_NOP);
    printf("[0x%02X] RX_PW_P3=%u\r\n", NRF24L01_REG_RX_PW_P3, NRF24L01_rxbuff.buf[0]);

    // RX_PW_P4
    NRF24L01_cmd(hspi, NRF24L01_REG_RX_PW_P4, NRF24L01_CMD_NOP);
    printf("[0x%02X] RX_PW_P4=%u\r\n", NRF24L01_REG_RX_PW_P4, NRF24L01_rxbuff.buf[0]);

    // RX_PW_P5
    NRF24L01_cmd(hspi, NRF24L01_REG_RX_PW_P5, NRF24L01_CMD_NOP);
    printf("[0x%02X] RX_PW_P5=%u\r\n", NRF24L01_REG_RX_PW_P5, NRF24L01_rxbuff.buf[0]);
}