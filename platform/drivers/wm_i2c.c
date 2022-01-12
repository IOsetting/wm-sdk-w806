// Copyright 2021 IOsetting <iosetting(at)outlook.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "wm_i2c.h"

#define I2C_FREQ_MAX            (1000000)
#define I2C_FREQ_MIN            (100000)


HAL_StatusTypeDef HAL_I2C_Init(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == NULL)
    {
        return HAL_ERROR;
    }

    hi2c->Lock = HAL_UNLOCKED;
    HAL_I2C_MspInit(hi2c);

    wm_sys_clk sysclk;
    uint32_t div = 0;
    if (hi2c->Frequency < I2C_FREQ_MIN)
    {
        hi2c->Frequency = I2C_FREQ_MIN;
    }
    else if (hi2c->Frequency > I2C_FREQ_MAX)
    {
        hi2c->Frequency = I2C_FREQ_MAX;
    }
    SystemClock_Get(&sysclk);
    div = (sysclk.apbclk * 1000000) / (5 * hi2c->Frequency) - 1;
    WRITE_REG(hi2c->Instance->PRESCALE_L, div & 0xff);
    WRITE_REG(hi2c->Instance->PRESCALE_H, (div >> 8) & 0xff);
    WRITE_REG(hi2c->Instance->EN, I2C_EN_ENABLE | I2C_EN_IEMASK);
    HAL_NVIC_SetPriority(I2C_IRQn, 0);
    HAL_NVIC_EnableIRQ(I2C_IRQn);
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == NULL)
    {
        return HAL_ERROR;
    }

    HAL_I2C_MspDeInit(hi2c);
    
    hi2c->ErrorCode = HAL_SPI_ERROR_NONE;
    __HAL_UNLOCK(hi2c);
    
    return HAL_OK;
}

/**
  * @brief  Initialize the I2C MSP.
  * @param  hi2c pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for I2C module.
  * @retval None
  */
__attribute__((weak)) void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
    UNUSED(hi2c);
}

/**
  * @brief  De-Initialize the I2C MSP.
  * @param  hspi pointer to a I2C_HandleTypeDef structure that contains
  *               the configuration information for I2C module.
  * @retval None
  */
__attribute__((weak)) void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
    UNUSED(hi2c);
}

/**
 * @brief    Send stop signal
 */
void HAL_I2C_Stop(I2C_HandleTypeDef *hi2c)
{
    WRITE_REG(hi2c->Instance->CR_SR, I2C_CR_STOP);
    while(READ_BIT(hi2c->Instance->CR_SR, I2C_SR_TIP));
}

/**
 * @brief    Waiting for ack signal
 */
int HAL_I2C_Wait_Ack(I2C_HandleTypeDef *hi2c)
{
    uint16_t errtime=0;

    while(READ_BIT(hi2c->Instance->CR_SR, I2C_SR_TIP));
    while(READ_BIT(hi2c->Instance->CR_SR, I2C_SR_RXACK))
    {
        errtime ++;
        if(errtime > 512)
        {
            printf("i2c ack error");
            HAL_I2C_Stop(hi2c);
            return HAL_ERROR;
        }
    }
    return HAL_OK;
}

/**
 * @brief    Send one byte
 * @param[in] dat    the byte to be sent
 * @param[in] ifstart  0: data only, 1: with start signal
 */
void HAL_I2C_Write_Byte(I2C_HandleTypeDef *hi2c, uint8_t dat, uint8_t ifstart)
{
    WRITE_REG(hi2c->Instance->DATA, dat);
    if(ifstart)
        WRITE_REG(hi2c->Instance->CR_SR, I2C_CR_START | I2C_CR_WR);
    else
        WRITE_REG(hi2c->Instance->CR_SR, I2C_CR_WR);
    while(READ_BIT(hi2c->Instance->CR_SR, I2C_SR_TIP));
}

/**
 * @brief    Read one byte
 * @param[in] ifack    0:no ack, 1:send ack
 * @param[in] ifstop 0:no stop signal, 1:send stop signal
 * @retval    the received byte
 */
uint8_t HAL_I2C_Read_Byte(I2C_HandleTypeDef *hi2c, uint8_t ifack, uint8_t ifstop)
{
    uint32_t value = I2C_CR_RD;

    if(!ifack)
        value |= I2C_CR_ACK;
    if(ifstop)
        value |= I2C_CR_STOP;
    
    WRITE_REG(hi2c->Instance->CR_SR, value);
    /** Waiting finish */
    while(READ_BIT(hi2c->Instance->CR_SR, I2C_SR_TIP));
    return READ_REG(hi2c->Instance->DATA);
}

HAL_StatusTypeDef HAL_I2C_Write(I2C_HandleTypeDef *hi2c, uint8_t DevAddress, uint8_t MemAddress, uint8_t *pData, uint16_t Size)
{
    uint32_t i, ret = HAL_ERROR;
    HAL_I2C_Write_Byte(hi2c, (DevAddress & 0xFE), 1);
    if (HAL_I2C_Wait_Ack(hi2c) != HAL_OK)
    {
        goto OUT;
    }
    HAL_I2C_Write_Byte(hi2c, MemAddress, 0);
    if (HAL_I2C_Wait_Ack(hi2c) != HAL_OK)
    {
        goto OUT;
    }
    for (i = 0; i < Size; i++)
    {
        HAL_I2C_Write_Byte(hi2c, pData[i], 0);
        if (HAL_I2C_Wait_Ack(hi2c) != HAL_OK)
        {
            goto OUT;
        }
    }
    ret = HAL_OK;
OUT:
    HAL_I2C_Stop(hi2c);
    return ret;
}

HAL_StatusTypeDef HAL_I2C_Read(I2C_HandleTypeDef *hi2c, uint8_t DevAddress, uint8_t MemAddress, uint8_t *pData, uint16_t Size)
{
    uint32_t i, ret = HAL_ERROR;

    HAL_I2C_Write_Byte(hi2c, (DevAddress & 0xFE), 1);
    if (HAL_I2C_Wait_Ack(hi2c) != HAL_OK)
    {
        goto OUT;
    }
    HAL_I2C_Write_Byte(hi2c, MemAddress, 0);
    if (HAL_I2C_Wait_Ack(hi2c) != HAL_OK)
    {
        goto OUT;
    }
    HAL_I2C_Write_Byte(hi2c, (DevAddress | 0x01), 0);
    if (HAL_I2C_Wait_Ack(hi2c) != HAL_OK)
    {
        goto OUT;
    }
    for (i = 0; i < Size; i++)
    {
        if (i == (Size - 1))
        {
            pData[i] = HAL_I2C_Read_Byte(hi2c, 0, 0);
        }
        else
        {
            pData[i] = HAL_I2C_Read_Byte(hi2c, 1, 0);
        }
    }
    ret = HAL_OK;
OUT:
    HAL_I2C_Stop(hi2c);
    return ret;
}
