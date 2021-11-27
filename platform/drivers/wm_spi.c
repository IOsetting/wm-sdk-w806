#include "wm_spi.h"

HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi)
{
    if (hspi == NULL)
    {
        return HAL_ERROR;
    }

    assert_param(IS_SPI_ALL_INSTANCE(hspi->Instance));
    assert_param(IS_SPI_MODE(hspi->Init.Mode));
    assert_param(IS_SPI_DIRECTION(hspi->Init.Direction));
    assert_param(IS_SPI_DATASIZE(hspi->Init.DataSize));
    assert_param(IS_SPI_NSS(hspi->Init.NSS));
    assert_param(IS_SPI_BAUDRATE_PRESCALER(hspi->Init.BaudRatePrescaler));
    assert_param(IS_SPI_CPOL(hspi->Init.CLKPolarity));
    assert_param(IS_SPI_CPHA(hspi->Init.CLKPhase));
    assert_param(IS_SPI_BIG_OR_LITTLE(hspi->Init.FirstByte));

    if (hspi->State == HAL_SPI_STATE_RESET)
    {
        hspi->Lock = HAL_UNLOCKED;
        HAL_SPI_MspInit(hspi);
    }
    hspi->State = HAL_SPI_STATE_BUSY;

    __HAL_SPI_DISABLE(hspi);

    WRITE_REG(hspi->Instance->CH_CFG, (hspi->Init.NSS | SPI_CH_CFG_CLEARFIFOS));
    WRITE_REG(hspi->Instance->SPI_CFG, (hspi->Init.Mode | hspi->Init.CLKPolarity | hspi->Init.CLKPhase | hspi->Init.FirstByte));
    WRITE_REG(hspi->Instance->CLK_CFG, hspi->Init.BaudRatePrescaler);

    __HAL_SPI_SET_CS_HIGH(hspi);

    hspi->ErrorCode = HAL_SPI_ERROR_NONE;
    hspi->State     = HAL_SPI_STATE_READY;

    return HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_DeInit(SPI_HandleTypeDef *hspi)
{
    if (hspi == NULL)
    {
        return HAL_ERROR;
    }
    
    assert_param(IS_SPI_ALL_INSTANCE(hspi->Instance));
    
    hspi->State = HAL_SPI_STATE_BUSY;
    __HAL_SPI_DISABLE(hspi);
    HAL_SPI_MspDeInit(hspi);
    
    hspi->ErrorCode = HAL_SPI_ERROR_NONE;
    hspi->State = HAL_SPI_STATE_RESET;
    __HAL_UNLOCK(hspi);
    
    return HAL_OK;
}

__attribute__((weak)) void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    UNUSED(hspi);
}

__attribute__((weak)) void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
    UNUSED(hspi);
}

HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, const uint8_t *pData, uint32_t Size, uint32_t Timeout)
{
    uint32_t tickstart, data = 0, i = 0;
    HAL_StatusTypeDef errorcode = HAL_OK;
    uint32_t fifo_count = 0, block_cnt = 0, tx_block_cnt = 0, tx_size = 0;
    
    __HAL_LOCK(hspi);
    
    tickstart = HAL_GetTick();
    
    if (hspi->State != HAL_SPI_STATE_READY)
    {
        errorcode = HAL_BUSY;
        goto error;
    }

    if ((pData == NULL) || (Size == 0U))
    {
        errorcode = HAL_ERROR;
        goto error;
    }
    
    __HAL_SPI_CLEAR_FIFO(hspi);
    SET_BIT(hspi->Instance->CH_CFG, SPI_CH_CFG_TXON);
    hspi->State       = HAL_SPI_STATE_BUSY_TX;
    hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
    
    hspi->pRxBuffPtr  = (uint8_t *)NULL;
    hspi->RxXferSize  = 0U;
    hspi->RxXferCount = 0U;
    
    block_cnt = Size / BLOCK_SIZE;
    while (tx_block_cnt <= block_cnt)
    {
        if (tx_block_cnt < block_cnt)
        {
            tx_size = BLOCK_SIZE;
        }
        else
        {
            tx_size = Size % BLOCK_SIZE;
        }
        hspi->pTxBuffPtr  = (uint8_t *)(pData + (tx_block_cnt * BLOCK_SIZE));
        hspi->TxXferSize  = tx_size;
        hspi->TxXferCount = tx_size;
        
        __HAL_SPI_SET_CLK_NUM(hspi, tx_size * 8);
        if (hspi->Init.NSS == SPI_NSS_SOFT)
        {
            // __HAL_SPI_SET_CS_LOW(hspi);
        }
        __HAL_SPI_SET_START(hspi);
        
        while (hspi->TxXferCount > 0U)
        {
            fifo_count = (32 - __HAL_SPI_GET_TXFIFO(hspi)) / 4;
            while((fifo_count > 0) && (hspi->TxXferCount > 0))
            {
                data = 0;
                for (i = 0; i < hspi->TxXferCount; i++)
                {
                    if (i == 4)
                    {
                        break;
                    }
                    data |= (hspi->pTxBuffPtr[i] << (i * 8));
                }
                hspi->pTxBuffPtr += sizeof(uint8_t) * i;
                hspi->TxXferCount -= i;
                WRITE_REG(hspi->Instance->TXDATA, data);
                // READ_REG(hspi->Instance->RXDATA);
                if (fifo_count > 0)
                {
                    fifo_count--;
                }
            }
            if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
            {
                errorcode = HAL_TIMEOUT;
                goto error;
            }
        }
        while (__HAL_SPI_GET_FLAG(hspi, SPI_INT_SRC_DONE) != SPI_INT_SRC_DONE)
        {
            if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
            {
                errorcode = HAL_TIMEOUT;
                goto error;
            }
        }
        __HAL_SPI_CELAR_FLAG(hspi, SPI_INT_SRC_DONE);
        
        tx_block_cnt++;
    }
    if (hspi->Init.NSS == SPI_NSS_SOFT)
    {
    //    __HAL_SPI_SET_CS_HIGH(hspi);
    }
error:
    CLEAR_BIT(hspi->Instance->CH_CFG, SPI_CH_CFG_TXON);
    hspi->State = HAL_SPI_STATE_READY;
    
    __HAL_UNLOCK(hspi);
    return errorcode;
}

HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint32_t Size, uint32_t Timeout)
{
    uint32_t tickstart, data = 0, i = 0;
    HAL_StatusTypeDef errorcode = HAL_OK;
    uint32_t fifo_count = 0, block_cnt = 0, tx_block_cnt = 0, tx_size = 0;
    
    __HAL_LOCK(hspi);
    
    tickstart = HAL_GetTick();
    if (hspi->State != HAL_SPI_STATE_READY)
    {
        errorcode = HAL_BUSY;
        goto error;
    }
    
    if ((pData == NULL) || (Size == 0U))
    {
        errorcode = HAL_ERROR;
        goto error;
    }
    __HAL_SPI_CLEAR_FIFO(hspi);
    SET_BIT(hspi->Instance->CH_CFG, SPI_CH_CFG_TXON | SPI_CH_CFG_RXON);
    
    hspi->State       = HAL_SPI_STATE_BUSY_RX;
    hspi->ErrorCode   = HAL_SPI_ERROR_NONE;

    hspi->pTxBuffPtr  = (uint8_t *)NULL;
    hspi->TxXferSize  = 0U;
    hspi->TxXferCount = 0U;

    block_cnt = Size / BLOCK_SIZE;
    while (tx_block_cnt <=  block_cnt)
    {
        if (tx_block_cnt < block_cnt)
        {
            tx_size = BLOCK_SIZE;
        }
        else
        {
            tx_size = Size % BLOCK_SIZE;
        }
        hspi->pRxBuffPtr  = (uint8_t *)(pData + tx_block_cnt * BLOCK_SIZE);
        hspi->RxXferSize  = tx_size;
        hspi->RxXferCount = tx_size;
        
        __HAL_SPI_SET_CLK_NUM(hspi, tx_size * 8);
        if (hspi->Init.NSS == SPI_NSS_SOFT)
        {
        //    __HAL_SPI_SET_CS_LOW(hspi);
        }
        __HAL_SPI_SET_START(hspi);
        
        while (hspi->RxXferCount > 0U)
        {
            fifo_count = (32 - __HAL_SPI_GET_TXFIFO(hspi)) / 4;
            while((fifo_count > 0) && (tx_size > 0))
            {
                WRITE_REG(hspi->Instance->TXDATA, data);
                tx_size -= 4;
                if (fifo_count > 0)
                {
                    fifo_count--;
                }
            }
            
            fifo_count = __HAL_SPI_GET_RXFIFO(hspi) / 4;
            while ((fifo_count > 0) || (__HAL_SPI_GET_FLAG(hspi, SPI_INT_SRC_DONE) == SPI_INT_SRC_DONE))
            {
                data = READ_REG(hspi->Instance->RXDATA);
                for (i = 0; i < 4; i++)
                {
                    if (i == hspi->RxXferCount)
                    {
                        break;
                    }
                    hspi->pRxBuffPtr[i] = (data >> (i * 8)) & 0xFF;
                }
                hspi->pRxBuffPtr += sizeof(uint8_t) * i;
                hspi->RxXferCount -= i;
                if (fifo_count > 0)
                {
                    fifo_count--;
                }
                if((hspi->RxXferCount == 0))
                {
                    break;
                }
            }
            
            if ((((HAL_GetTick() - tickstart) >=  Timeout) && ((Timeout != HAL_MAX_DELAY))) || (Timeout == 0U))
            {
                errorcode = HAL_TIMEOUT;
                goto error;
            }
        }
        while (__HAL_SPI_GET_FLAG(hspi, SPI_INT_SRC_DONE) != SPI_INT_SRC_DONE)
        {
            if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
            {
                errorcode = HAL_TIMEOUT;
                goto error;
            }
        }
        __HAL_SPI_CELAR_FLAG(hspi, SPI_INT_SRC_DONE);
        
        tx_block_cnt++;
    }
    if (hspi->Init.NSS == SPI_NSS_SOFT)
    {
    //    __HAL_SPI_SET_CS_HIGH(hspi);
    }
error :
    CLEAR_BIT(hspi->Instance->CH_CFG, SPI_CH_CFG_TXON | SPI_CH_CFG_RXON);
    hspi->State = HAL_SPI_STATE_READY;
    __HAL_UNLOCK(hspi);
    return errorcode;
}

HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint32_t Size,
                                          uint32_t Timeout)
{
    HAL_StatusTypeDef    errorcode = HAL_OK;
    uint32_t tickstart, data = 0, i = 0;
    uint32_t fifo_count = 0, block_cnt = 0, tx_block_cnt = 0, tx_size = 0;

    __HAL_LOCK(hspi);
    
    tickstart = HAL_GetTick();
    
    if (!((hspi->State == HAL_SPI_STATE_READY) || (hspi->State == HAL_SPI_STATE_BUSY_RX)))
    {
        errorcode = HAL_BUSY;
        goto error;
    }

    if ((pTxData == NULL) || (pRxData == NULL) || (Size == 0U))
    {
        errorcode = HAL_ERROR;
        goto error;
    }
    
    if (hspi->State != HAL_SPI_STATE_BUSY_RX)
    {
        hspi->State = HAL_SPI_STATE_BUSY_TX_RX;
    }
    hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
    __HAL_SPI_CLEAR_FIFO(hspi);
    SET_BIT(hspi->Instance->CH_CFG, SPI_CH_CFG_TXON | SPI_CH_CFG_RXON);
    
    block_cnt = Size / BLOCK_SIZE;
    while (tx_block_cnt <= block_cnt)
    {
        if (tx_block_cnt < block_cnt)
        {
            tx_size = BLOCK_SIZE;
        }
        else
        {
            tx_size = Size % BLOCK_SIZE;
        }
        
        hspi->pRxBuffPtr  = (uint8_t *)(pRxData + (tx_block_cnt * BLOCK_SIZE));
        hspi->RxXferCount = tx_size;
        hspi->RxXferSize  = tx_size;
        hspi->pTxBuffPtr  = (uint8_t *)(pTxData + (tx_block_cnt * BLOCK_SIZE));
        hspi->TxXferCount = tx_size;
        hspi->TxXferSize  = tx_size;
        
        __HAL_SPI_SET_CLK_NUM(hspi, tx_size * 8);
        if (hspi->Init.NSS == SPI_NSS_SOFT)
        {
        //    __HAL_SPI_SET_CS_LOW(hspi);
        }
        __HAL_SPI_SET_START(hspi);
        
        while ((hspi->TxXferCount > 0U) || (hspi->RxXferCount > 0U))
        {
            fifo_count = (32 - __HAL_SPI_GET_TXFIFO(hspi)) / 4;
            while ((fifo_count > 0) && (hspi->TxXferCount > 0U))
            {
                data = 0;
                for (i = 0; i < hspi->TxXferCount; i++)
                {
                    if (i == 4)
                    {
                        break;
                    }
                    data |= (hspi->pTxBuffPtr[i] << (i * 8));
                }
                hspi->pTxBuffPtr += sizeof(uint8_t) * i;
                hspi->TxXferCount -= i;
                WRITE_REG(hspi->Instance->TXDATA, data);
                if (fifo_count > 0)
                {
                    fifo_count--;
                }
            }
            
            fifo_count = __HAL_SPI_GET_RXFIFO(hspi) / 4 ;
            while ((fifo_count > 0) || (__HAL_SPI_GET_FLAG(hspi, SPI_INT_SRC_DONE) == SPI_INT_SRC_DONE))
            {
                data = READ_REG(hspi->Instance->RXDATA);
                for (i = 0; i < 4; i++)
                {
                    if (i == hspi->RxXferCount)
                    {
                        break;
                    }
                    hspi->pRxBuffPtr[i] = (data >> (i * 8)) & 0xFF;
                }
                hspi->pRxBuffPtr += sizeof(uint8_t) * i;
                hspi->RxXferCount -= i;
                if (fifo_count > 0)
                {
                    fifo_count--;
                }
                if (hspi->RxXferCount == 0)
                {
                    break;
                }
            }
            
            if ((((HAL_GetTick() - tickstart) >=  Timeout) && ((Timeout != HAL_MAX_DELAY))) || (Timeout == 0U))
            {
                errorcode = HAL_TIMEOUT;
                goto error;
            }
        }
        while (__HAL_SPI_GET_FLAG(hspi, SPI_INT_SRC_DONE) != SPI_INT_SRC_DONE)
        {
            if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
            {
                errorcode = HAL_TIMEOUT;
                goto error;
            }
        }
        __HAL_SPI_CELAR_FLAG(hspi, SPI_INT_SRC_DONE);
        
        tx_block_cnt++;
    
    }
    if (hspi->Init.NSS == SPI_NSS_SOFT)
    {
    //    __HAL_SPI_SET_CS_HIGH(hspi);
    }
error :
    CLEAR_BIT(hspi->Instance->CH_CFG, SPI_CH_CFG_TXON | SPI_CH_CFG_RXON);
    hspi->State = HAL_SPI_STATE_READY;
    __HAL_UNLOCK(hspi);
    return errorcode;    
}


