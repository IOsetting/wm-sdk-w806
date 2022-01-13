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

#ifndef __FATFS_MMC_H
#define __FATFS_MMC_H

#include "wm_hal.h"
#include "ff.h"
#include "diskio.h"

extern SPI_HandleTypeDef    hspi;
extern RTC_TimeTypeDef      rtc_time;

// CS: B4, B14
#define MMC_CS_PORT      GPIOB
#define MMC_CS_PIN       GPIO_PIN_14
// SCK: B1, B2, B15, B24
#define MMC_SCK_PORT     GPIOB
#define MMC_SCK_PIN      GPIO_PIN_15
// MOSI: B5, B17, B26, PA7
#define MMC_MOSI_PORT    GPIOB
#define MMC_MOSI_PIN     GPIO_PIN_17
// MISO: B0, B3, B16, B25
#define MMC_MISO_PORT    GPIOB
#define MMC_MISO_PIN     GPIO_PIN_16

#define MMC_CS_LOW       __HAL_SPI_SET_CS_LOW(&hspi)
#define MMC_CS_HIGH      __HAL_SPI_SET_CS_HIGH(&hspi)

/* Definitions for MMC/SDC command */
#define CMD0     (0x40+0)         /* GO_IDLE_STATE */
#define CMD1     (0x40+1)         /* SEND_OP_COND */
#define CMD8     (0x40+8)         /* SEND_IF_COND */
#define CMD9     (0x40+9)         /* SEND_CSD */
#define CMD10    (0x40+10)        /* SEND_CID */
#define CMD12    (0x40+12)        /* STOP_TRANSMISSION */
#define CMD16    (0x40+16)        /* SET_BLOCKLEN */
#define CMD17    (0x40+17)        /* READ_SINGLE_BLOCK */
#define CMD18    (0x40+18)        /* READ_MULTIPLE_BLOCK */
#define CMD23    (0x40+23)        /* SET_BLOCK_COUNT */
#define CMD24    (0x40+24)        /* WRITE_BLOCK */
#define CMD25    (0x40+25)        /* WRITE_MULTIPLE_BLOCK */
#define CMD41    (0x40+41)        /* SEND_OP_COND (ACMD) */
#define CMD55    (0x40+55)        /* APP_CMD */
#define CMD58    (0x40+58)        /* READ_OCR */

/* MMC card type flags (MMC_GET_TYPE) */
#define CT_MMC      0x01        /* MMC ver 3 */
#define CT_SD1      0x02        /* SD ver 1 */
#define CT_SD2      0x04        /* SD ver 2 */
#define CT_SDC      0x06        /* SD */
#define CT_BLOCK    0x08        /* Block addressing */

#define ACMD41_HCS           0x40000000
#define ACMD41_SDXC_POWER    0x10000000
#define ACMD41_S18R          0x04000000
#define ACMD41_VOLTAGE       0x00ff8000
#define ACMD41_ARG_HC        (ACMD41_HCS|ACMD41_SDXC_POWER|ACMD41_VOLTAGE)
#define ACMD41_ARG_SC        (ACMD41_VOLTAGE)  

#define SPI_TIMEOUT 100

#endif
