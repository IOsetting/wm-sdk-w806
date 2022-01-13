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

#include "fatfs_mmc.h"

static volatile DSTATUS mmc_state = STA_NOINIT;     /* Disk Status */
static uint8_t CardType;                            /* Type 0:MMC, 1:SDC, 2:Block addressing */
static uint8_t PowerFlag = 0;                       /* Power flag */

/***************************************
 * SPI functions
 **************************************/

/* SPI transmit a byte */
static void MMC_SPI_TxByte(uint8_t data)
{
    HAL_SPI_Transmit(&hspi, &data, 1, SPI_TIMEOUT);
}

/* SPI transmit buffer */
static void MMC_SPI_TxBuffer(uint8_t *buffer, uint16_t len)
{
    HAL_SPI_Transmit(&hspi, buffer, len, SPI_TIMEOUT);
}

/* SPI receive a byte */
static uint8_t MMC_SPI_RxByte(void)
{
    uint8_t dummy, data;
    dummy = 0xFF;
    HAL_SPI_TransmitReceive(&hspi, &dummy, &data, 1, SPI_TIMEOUT);
    return data;
}

/***************************************
 * MMC/SD functions
 **************************************/

/* wait SD ready */
static uint8_t MMC_ReadyWait(void)
{
    uint8_t res;
    uint32_t tickstart = HAL_GetTick();
    /* if SD goes ready, receives 0xFF, timeout 500 */
    do 
    {
        res = MMC_SPI_RxByte();
    } while ((res != 0xFF) && (HAL_GetTick() - tickstart < 500));
    return res;
}

/* power on */
static void MMC_PowerOn(void) 
{
    uint8_t args[6];
    uint32_t cnt = 0x1FFF;

    /* transmit bytes to wake up */
    MMC_CS_HIGH;
    for(int i = 0; i < 10; i++)
    {
        MMC_SPI_TxByte(0xFF);
    }

    /* slave select */
    MMC_CS_LOW;

    /* make idle state */
    args[0] = CMD0;        /* CMD0:GO_IDLE_STATE */
    args[1] = 0;
    args[2] = 0;
    args[3] = 0;
    args[4] = 0;
    args[5] = 0x95;        /* CRC */

    MMC_SPI_TxBuffer(args, sizeof(args));

    /* wait response */
    while ((MMC_SPI_RxByte() != 0x01) && cnt)
    {
        cnt--;
    }

    MMC_CS_HIGH;
    MMC_SPI_TxByte(0XFF);

    PowerFlag = 1;
}

/* power off */
static void MMC_PowerOff(void) 
{
    PowerFlag = 0;
}

/* check power flag */
static uint8_t MMC_CheckPower(void) 
{
    return PowerFlag;
}

/* receive data block */
static bool MMC_RxDataBlock(BYTE *buff, UINT len)
{
    uint8_t token;
    uint32_t tickstart = HAL_GetTick();

    /* loop until receive a response or timeout, timeout 200ms */
    do 
    {
        token = MMC_SPI_RxByte();
    } while((token == 0xFF) && (HAL_GetTick() - tickstart < 200));

    /* invalid response */
    if(token != 0xFE) return false;

    /* receive data */
    do {
        *buff++ = MMC_SPI_RxByte();
    } while(len--);

    /* discard CRC */
    MMC_SPI_RxByte();
    MMC_SPI_RxByte();

    return true;
}

/* transmit data block */

static bool MMC_TxDataBlock(const uint8_t *buff, BYTE token)
{
    uint8_t resp = 0;
    uint8_t i = 0;

    /* wait SD ready */
    if (MMC_ReadyWait() != 0xFF) return false;

    /* transmit token */
    MMC_SPI_TxByte(token);

    /* if it's not STOP token, transmit data */
    if (token != 0xFD)
    {
        MMC_SPI_TxBuffer((uint8_t*)buff, 512);

        /* discard CRC */
        MMC_SPI_RxByte();
        MMC_SPI_RxByte();

        /* receive response */
        while (i <= 64)
        {
            resp = MMC_SPI_RxByte();

            /* transmit 0x05 accepted */
            if ((resp & 0x1F) == 0x05) break;
            i++;
        }

        /* recv buffer clear */
        while (MMC_SPI_RxByte() == 0);
    }

    /* transmit 0x05 accepted */
    if ((resp & 0x1F) == 0x05) return true;

    return false;
}

/* transmit command */
static BYTE MMC_SendCmd(BYTE cmd, uint32_t arg)
{
    uint8_t crc, res;

    /* wait SD ready */
    if (MMC_ReadyWait() != 0xFF) return 0xFF;

    /* transmit command */
    MMC_SPI_TxByte(cmd);                     /* Command */
    MMC_SPI_TxByte((uint8_t)(arg >> 24));     /* Argument[31..24] */
    MMC_SPI_TxByte((uint8_t)(arg >> 16));     /* Argument[23..16] */
    MMC_SPI_TxByte((uint8_t)(arg >> 8));     /* Argument[15..8] */
    MMC_SPI_TxByte((uint8_t)arg);             /* Argument[7..0] */

    /* prepare CRC */
    if(cmd == CMD0) crc = 0x95;    /* CRC for CMD0(0) */
    else if(cmd == CMD8) crc = 0x87;    /* CRC for CMD8(0x1AA) */
    else crc = 1;

    /* transmit CRC */
    MMC_SPI_TxByte(crc);

    /* Skip a stuff byte when STOP_TRANSMISSION */
    if (cmd == CMD12) MMC_SPI_RxByte();

    /* receive response */
    uint8_t n = 10;
    do {
        res = MMC_SPI_RxByte();
    } while ((res & 0x80) && --n);

    return res;
}

/***************************************
 * diskio.c functions implementation
 **************************************/

/* return disk status */
DSTATUS MMC_disk_status(void) 
{
    return mmc_state;
}

/* initialize SD */
DSTATUS MMC_disk_initialize(void)
{
    uint8_t n, type, ocr[4], t1, t2, f1 = 0x00;
    uint32_t tickstart;
    printf("MMC_disk_initialize\r\n");

    /* single drive, drv should be 0 */
    if(mmc_state & STA_NODISK) return STA_NOINIT;

    /* no disk */
    if(mmc_state & STA_NODISK) return STA_NODISK;

    /* power on */
    MMC_PowerOn();

    /* slave select */
    MMC_CS_LOW;

    /* check disk type */
    type = 0;

    /* send GO_IDLE_STATE command */
    printf("\r\nCMD0\r\n");
    if (MMC_SendCmd(CMD0, 0) == 1)
    {
        /* SDC V2+ accept CMD8 command, http://elm-chan.org/docs/mmc/mmc_e.html */
        printf("CMD8... ");
        if (MMC_SendCmd(CMD8, 0x1AA) == 1)
        {
            printf("succeeded, SDC V2+\r\n");
            /* operation condition register */
            for (n = 0; n < 4; n++)
            {
                ocr[n] = MMC_SPI_RxByte();
            }

            /* voltage range 2.7-3.6V */
            if (ocr[2] == 0x01 && ocr[3] == 0xAA)
            {
                printf("ACMD41 ACMD41_HCS.. ");
                /* timeout 1 sec */
                tickstart = HAL_GetTick();
                /* ACMD41 with HCS bit */
                do 
                {
                    t1 = MMC_SendCmd(CMD55, 0);
                    t2 = MMC_SendCmd(CMD41, ACMD41_HCS);
                    if (t1 <= 1 && t2 == 0)
                    {
                        f1 = 0x01;
                        break;
                    }
                } while (HAL_GetTick() - tickstart < 1000);
        
                if (f1 == 0x00)
                {
                    printf("failed\r\ntry ACMD41_SDXC_POWER... ");
                    tickstart = HAL_GetTick();
                    do {
                        t1 = MMC_SendCmd(CMD55, 0);
                        t2 = MMC_SendCmd(CMD41, ACMD41_SDXC_POWER);
                        if (t1 <= 1 && t2 == 0)
                        {
                            f1 = 0x01;
                            break;
                        }
                    } while (HAL_GetTick() - tickstart < 1000);
                }

                if (f1 == 0x00)
                {
                    printf("failed\r\ntry ACMD41_S18R... ");
                    tickstart = HAL_GetTick();
                    do
                    {
                        t1 = MMC_SendCmd(CMD55, 0);
                        t2 = MMC_SendCmd(CMD41, ACMD41_S18R);
                        if (t1 <= 1 && t2 == 0)
                        {
                            f1 = 0x01;
                            break;
                        }
                    } while (HAL_GetTick() - tickstart < 1000);
                }

                if (f1 == 0x00)
                {
                    printf("failed\r\ntry ACMD41_VOLTAGE... ");
                    tickstart = HAL_GetTick();
                    do
                    {
                        t1 = MMC_SendCmd(CMD55, 0);
                        t2 = MMC_SendCmd(CMD41, ACMD41_VOLTAGE);
                        if (t1 <= 1 && t2 == 0)
                        {
                            f1 = 0x01;
                            break;
                        }
                    } while (HAL_GetTick() - tickstart < 1000);
                }

                if (f1 == 0x00)
                {
                    printf("failed, stop trying\r\n");
                }
                else
                {
                    printf("succeeded\r\nCMD58 ");
                    /* READ_OCR */
                    if (MMC_SendCmd(CMD58, 0) == 0)
                    {
                        /* Check CCS bit */
                        for (n = 0; n < 4; n++)
                        {
                            ocr[n] = MMC_SPI_RxByte();
                            printf("%02X ", ocr[n]);
                        }

                        /* SDv2 (HC or SC) */
                        type = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
                        printf("type:%02X\r\n", type);
                    }
                }
            }
        }
        else
        {
            printf("failed, SDC V1 or MMC\r\n");
            /* timeout 1 sec */
            tickstart = HAL_GetTick();
            /* SDC V1 or MMC */
            type = (MMC_SendCmd(CMD55, 0) <= 1 && MMC_SendCmd(CMD41, 0) <= 1) ? CT_SD1 : CT_MMC;
            do
            {
                if (type == CT_SD1)
                {
                    if (MMC_SendCmd(CMD55, 0) <= 1 && MMC_SendCmd(CMD41, 0) == 0) break; /* ACMD41 */
                }
                else
                {
                    if (MMC_SendCmd(CMD1, 0) == 0) break; /* CMD1 */
                }

            } while (HAL_GetTick() - tickstart < 1000);

            /* SET_BLOCKLEN */
            if ((HAL_GetTick() - tickstart) >= 1000 || MMC_SendCmd(CMD16, 512) != 0) type = 0;
        }
    }

    CardType = type;

    /* Idle */
    MMC_CS_HIGH;
    MMC_SPI_RxByte();

    /* Clear STA_NOINIT */
    if (type)
    {
        mmc_state &= ~STA_NOINIT;
    }
    else
    {
        /* Initialization failed */
        MMC_PowerOff();
    }
    //printf("Stat:%02X\r\n", Stat);
    return mmc_state;
}

/* read sector */
DRESULT MMC_disk_read(BYTE *buff, LBA_t sector, UINT count) 
{
    if (!count) return RES_PARERR;

    /* no disk */
    if (mmc_state & STA_NOINIT) return RES_NOTRDY;

    /* if not block-addressing, convert to byte address */
    if (!(CardType & CT_BLOCK)) sector *= 512;

    MMC_CS_LOW;

    if (count == 1)
    {
        /* READ_SINGLE_BLOCK */
        if (MMC_SendCmd(CMD17, sector) == 0)
        {
            if (MMC_RxDataBlock(buff, 512))
            {
                count = 0;
            }
        }
    }
    else
    {
        /* READ_MULTIPLE_BLOCK */
        if (MMC_SendCmd(CMD18, sector) == 0)
        {
            do
            {
                if (!MMC_RxDataBlock(buff, 512))
                    break;
                buff += 512;
            } while (--count);

            /* STOP_TRANSMISSION */
            MMC_SendCmd(CMD12, 0);
        }
    }

    /* Idle */
    MMC_CS_HIGH;
    MMC_SPI_RxByte();

    return count ? RES_ERROR : RES_OK;
}

/* write sector */
DRESULT MMC_disk_write(const BYTE *buff, LBA_t sector, UINT count) 
{
    if (!count) return RES_PARERR;

    /* no disk */
    if (mmc_state & STA_NOINIT) return RES_NOTRDY;

    /* write protection */
    if (mmc_state & STA_PROTECT) return RES_WRPRT;

    /* convert to byte address */
    if (!(CardType & CT_BLOCK)) sector *= 512;

    MMC_CS_LOW;

    if (count == 1)
    {
        /* WRITE_BLOCK */
        if ((MMC_SendCmd(CMD24, sector) == 0) && MMC_TxDataBlock(buff, 0xFE))
        {
            count = 0;
        }
    }
    else
    {
        /* WRITE_MULTIPLE_BLOCK */
        if (CardType & CT_SD1)
        {
            MMC_SendCmd(CMD55, 0);
            MMC_SendCmd(CMD23, count); /* ACMD23 */
        }

        if (MMC_SendCmd(CMD25, sector) == 0)
        {
            do 
            {
                if(!MMC_TxDataBlock(buff, 0xFC)) break;
                buff += 512;
            } while (--count);

            /* STOP_TRAN token */
            if(!MMC_TxDataBlock(0, 0xFD))
            {
                count = 1;
            }
        }
    }

    /* Idle */
    MMC_CS_HIGH;
    MMC_SPI_RxByte();

    return count ? RES_ERROR : RES_OK;
}

/* ioctl */
DRESULT MMC_disk_ioctl(BYTE cmd, void* buff) 
{
    DRESULT res;
    uint8_t n, csd[16], *ptr = buff;
    WORD csize;

    res = RES_ERROR;

    if (cmd == CTRL_POWER)
    {
        switch (*ptr)
        {
        case 0:
            MMC_PowerOff();        /* Power Off */
            res = RES_OK;
            break;
        case 1:
            MMC_PowerOn();        /* Power On */
            res = RES_OK;
            break;
        case 2:
            *(ptr + 1) = MMC_CheckPower();
            res = RES_OK;        /* Power Check */
            break;
        default:
            res = RES_PARERR;
        }
    }
    else
    {
        /* no disk */
        if (mmc_state & STA_NOINIT) return RES_NOTRDY;

        MMC_CS_LOW;

        switch (cmd)
        {
        case GET_SECTOR_COUNT:
            /* SEND_CSD */
            if ((MMC_SendCmd(CMD9, 0) == 0) && MMC_RxDataBlock(csd, 16))
            {
                if ((csd[0] >> 6) == 1)
                {
                    /* SDC V2 */
                    csize = csd[9] + ((WORD) csd[8] << 8) + 1;
                    *(DWORD*) buff = (DWORD) csize << 10;
                }
                else
                {
                    /* MMC or SDC V1 */
                    n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
                    csize = (csd[8] >> 6) + ((WORD) csd[7] << 2) + ((WORD) (csd[6] & 3) << 10) + 1;
                    *(DWORD*) buff = (DWORD) csize << (n - 9);
                }
                res = RES_OK;
            }
            break;
        case GET_SECTOR_SIZE:
            *(WORD*) buff = 512;
            res = RES_OK;
            break;
        case CTRL_SYNC:
            if (MMC_ReadyWait() == 0xFF) res = RES_OK;
            break;
        case MMC_GET_CSD:
            /* SEND_CSD */
            if (MMC_SendCmd(CMD9, 0) == 0 && MMC_RxDataBlock(ptr, 16)) res = RES_OK;
            break;
        case MMC_GET_CID:
            /* SEND_CID */
            if (MMC_SendCmd(CMD10, 0) == 0 && MMC_RxDataBlock(ptr, 16)) res = RES_OK;
            break;
        case MMC_GET_OCR:
            /* READ_OCR */
            if (MMC_SendCmd(CMD58, 0) == 0)
            {
                for (n = 0; n < 4; n++)
                {
                    *ptr++ = MMC_SPI_RxByte();
                }
                res = RES_OK;
            }
        default:
            res = RES_PARERR;
        }

        MMC_CS_HIGH;
        MMC_SPI_RxByte();
    }

    return res;
}

/**
 * DWORD, 4-bytes, format:
 * 
 * [25,31], year, [0,127] from 1980
 * [21,24], month, [1,12]
 * [16,20], day, [1,31]
 * [11,15], hour, [0,23]
 * [5,10], minute, [0,59]
 * [0,4], second, [0,59]
*/
DWORD MMC_get_fattime(void)
{
    DWORD val;
    val = (rtc_time.Year - 80) << 25;
    val += rtc_time.Month << 21;
    val += rtc_time.Date << 16;
    val += rtc_time.Hours << 11;
    val += rtc_time.Minutes << 5;
    val += rtc_time.Seconds;
    return val;
}
