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

/******************************************************************************
 * \brief       Demo code of FatFs on SD Card with RTC
 * \remarks     test-board: HLK-W806-KIT-V1.0
 * 
 *    This test will perform the following tests:
 *      1. Start RTC with time 2022-1-12 12:28:10
 *      2. Mount SD Card
 *      3. Scan SD Card and list files
 *      4. Check if w806test_long_name.txt exists
 *      5. Delete w806test_long_name.txt if it exists
 *      6. Open w806test_long_name.txt, write and close
 *      7. Open w806test_long_name.txt, read and close
 *      8. Display time in main loop
 * 
 *    Pin Wiring:
 *        B14   -> CS/DAT3
 *        B15   -> SCK, SCL, CLK
 *        B16   -> MISO/DAT0/DO
 *        B17   -> MOSI/CMD/DI
 *        GND   -> VSS
 *        3.3V  -> VDD
 * 
******************************************************************************/

#include <stdio.h>
#include "wm_hal.h"
#include "fatfs_mmc.h"

PMU_HandleTypeDef hpmu;
RTC_TimeTypeDef rtc_time;
SPI_HandleTypeDef hspi;

FATFS fs;
FATFS *pfs;
FIL fil;
FRESULT fres;
DWORD fre_clust;
uint32_t totalSpace, freeSpace;
char buff[256];

#define TEST_FILENAME   "w806test_long_name.txt"

void Error_Handler(void);

static void MPU_Init(void);
static void RTC_Init(void);
static void SPI_Init(void);

static void FatFs_Init(void);
static FRESULT FatFs_ScanFiles(char* path);
static FRESULT FatFs_CheckFileStates(void);
static FRESULT FatFs_DeleteFile(void);
static void FatFs_WriteFile(void);
static void FatFs_Unmount(void);

int main(void)
{
    SystemClock_Config(CPU_CLK_160M);
    printf("enter main\r\n");

    MPU_Init();
    printf("MPU_Init\r\n");
    RTC_Init();
    printf("RTC_Init\r\n");
    SPI_Init();
    printf("SPI_Init\r\n");
    // Wait SD card
    HAL_Delay(500);
    FatFs_Init();
    strcpy(buff, "");
    FatFs_ScanFiles(buff);
    fres = FatFs_CheckFileStates();
    if (fres == FR_OK)
    {
        fres = FatFs_DeleteFile();
        if (fres != FR_OK)
        {
            printf("failed to delete file\r\n");
        }
        printf("file deleted\r\n");
    }
    FatFs_WriteFile();
    FatFs_Unmount();

    while (1)
    {
        HAL_PMU_RTC_GetTime(&hpmu, &rtc_time);
        printf("%d-%d-%d %d:%d:%d\r\n", 
            (rtc_time.Year + 1900), rtc_time.Month, rtc_time.Date, rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds);

        HAL_Delay(5000);
    }
}

static void FatFs_Init(void)
{
    /* Mount SD Card */
    fres = f_mount(&fs, "", 0x01);
    if (fres != FR_OK)
    {
        printf("f_mount failed: %02X\r\n", fres);
        Error_Handler();
    }
    else
    {
        printf("f_mount succeeded\r\n");
    }

    /* Check freeSpace space */
    fres = f_getfree("", &fre_clust, &pfs);
    if (fres != FR_OK)
    {
        printf("f_getfree failed: %02X\r\n", fres);
        Error_Handler();
    }

    totalSpace = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
    freeSpace = (uint32_t)(fre_clust * pfs->csize * 0.5);
    printf("Total:%dKB, Free:%dKB\r\n", totalSpace, freeSpace);

    /* free space is less than 1kb */
    if (freeSpace < 1)
    {
        printf("free space is not enough\r\n");
        Error_Handler();
    }
}

static FRESULT FatFs_ScanFiles(char* path)
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;

    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                res = FatFs_ScanFiles(path);                    /* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
                printf("%s/%s %llu\r\n", path, fno.fname, fno.fsize);
            }
        }
        f_closedir(&dir);
    }
    return res;
}

static FRESULT FatFs_CheckFileStates(void)
{
    static FILINFO fno;

    printf("Test for %s...\n", TEST_FILENAME);

    fres = f_stat(TEST_FILENAME, &fno);
    switch (fres) {

    case FR_OK:
        printf("Size: %llu\n", fno.fsize);
        printf("Timestamp: %u/%02u/%02u, %02u:%02u\n",
               (fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31,
               fno.ftime >> 11, fno.ftime >> 5 & 63);
        printf("Attributes: %c%c%c%c%c\n",
               (fno.fattrib & AM_DIR) ? 'D' : '-',
               (fno.fattrib & AM_RDO) ? 'R' : '-',
               (fno.fattrib & AM_HID) ? 'H' : '-',
               (fno.fattrib & AM_SYS) ? 'S' : '-',
               (fno.fattrib & AM_ARC) ? 'A' : '-');
        break;

    case FR_NO_FILE:
        printf("File does not exist.\n");
        break;

    default:
        printf("An error occured. (%d)\n", fres);
    }
    return fres;
}

static FRESULT FatFs_DeleteFile(void)
{
    return f_unlink (TEST_FILENAME);
}

static void FatFs_WriteFile(void)
{
    /* Open file to write */
    printf("open to write: %s\r\n", TEST_FILENAME);
    if (f_open(&fil, TEST_FILENAME, FA_OPEN_ALWAYS | FA_READ | FA_WRITE) != FR_OK)
    {
        printf("f_open failed\r\n");
        Error_Handler();
    }

    /* Writing text */
    f_puts("W806 SD Card I/O Example via SPI\n", &fil);
    f_puts("-- Planet 4032-877", &fil);

    /* Close file */
    printf("close: %s\r\n", TEST_FILENAME);
    if (f_close(&fil) != FR_OK)
    {
        printf("f_close failed\r\n");
        Error_Handler();
    }

    /* Open file to read */
    printf("open to read: %s\r\n", TEST_FILENAME);
    if (f_open(&fil, TEST_FILENAME, FA_READ) != FR_OK)
    {
        printf("f_open failed\r\n");
        Error_Handler();
    }

    printf("read: %s\r\n", TEST_FILENAME);
    while (f_gets(buff, sizeof(buff), &fil))
    {
        /* SWV output */
        printf("%s", buff);
    }
    printf("\r\ndone\r\n");

    /* Close file */
    printf("close: %s\r\n", TEST_FILENAME);
    if (f_close(&fil) != FR_OK)
    {
        printf("f_close failed\r\n");
        Error_Handler();
    }
}

static void FatFs_Unmount(void)
{
    /* Unmount */
    printf("Unmount sd card\r\n");
    fres = f_unmount("");
    if (fres!= FR_OK)
    {
        printf("f_unmount failed: %02X\r\n", fres);
        Error_Handler();
    }
    else
    {
        printf("f_unmount succeeded\r\n");
    }
}

static void RTC_Init(void)
{
    rtc_time.Year = 122;
    rtc_time.Month = 1;
    rtc_time.Date = 12;
    rtc_time.Hours = 12;
    rtc_time.Minutes = 28;
    rtc_time.Seconds = 10;
    HAL_PMU_RTC_Start(&hpmu, &rtc_time);
}

static void MPU_Init(void)
{
	hpmu.Instance = PMU;
    hpmu.ClkSource = PMU_CLKSOURCE_32RC;
    HAL_PMU_Init(&hpmu);
}

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

void HAL_PMU_RTC_Callback(PMU_HandleTypeDef *hpmu)
{
    printf("rtc irq callback\r\n");
}

void Error_Handler(void)
{
    while (1)
    {
    }
}

void assert_failed(uint8_t *file, uint32_t line)
{
    printf("Wrong parameters value: file %s on line %d\r\n", file, line);
}
