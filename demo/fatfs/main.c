
#include <stdio.h>
#include "wm_hal.h"
#include "ff.h"
#include "fattime.h"

/*
从W800 SDK中复制的Demo，稍作修改，添加了速度测试和一些打印信息。
FAT功能测试：没有文件系统会自动格式化，读写字符串测试并打印，读写速度测试。
目前还没有实现RTC功能，写入时间是一个固定的值。
有点小问题，不清楚原因。有时候会显示没有文件系统，格式化不成功，需要将SD卡断电后再上电才能识别。
*/

#define TEST_DEBUG printf

SPI_HandleTypeDef hspi;
FATFS fs;               //FatFs文件系统对象
FIL fnew;               //文件对象
FRESULT res_sd;         //文件操作结果
UINT fnum;              //文件成功读写数量
BYTE ReadBuffer[256] = {0};
BYTE work[FF_MAX_SS];
BYTE WriteBuffer[] = "成功移植了FatFs文件系统！\r\n"; //写缓存区

void Error_Handler(void);

static void SPI_Init(void)
{
    hspi.Instance = SPI;
    hspi.Init.Mode = SPI_MODE_MASTER;
    hspi.Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi.Init.CLKPhase = SPI_PHASE_2EDGE;
    hspi.Init.NSS = SPI_NSS_SOFT;
    hspi.Init.BaudRatePrescaler = 60 - 1;
    hspi.Init.FirstByte = SPI_LITTLEENDIAN;
    
    if (HAL_SPI_Init(&hspi) != HAL_OK)
    {
        Error_Handler();
    }
}

static FRESULT FatFs_Init(void)
{
    //挂载SD卡
    res_sd = f_mount(&fs, "1:/", 1);
    
    //***********************格式化测试****************************
    if(res_sd == FR_NO_FILESYSTEM)
    {
        TEST_DEBUG("SD卡没有文件系统，即将进行格式化...\r\n");
        //格式化
        res_sd = f_mkfs("1:", 0, work, sizeof(work));
        //已移除while循环，只尝一次格式化，防止大量打印
        if(res_sd == FR_OK)
        {
            TEST_DEBUG("SD卡成功格式化！\r\n");
            //格式化后先取消挂载
            res_sd = f_mount(NULL, "1:", 1);
            //再重新挂载
            res_sd = f_mount(&fs, "1:", 1);
        }
        else
        {
            TEST_DEBUG("SD卡格式化失败！\r\n");
        }
    }
    else if(res_sd != FR_OK)
    {
        TEST_DEBUG("挂载文件系统失败！可能是因为文件初始化失败！错误代码：%d\r\n", res_sd);
    }
    else
    {
        TEST_DEBUG("文件系统挂载成功， 可进行读写测试！\r\n");
    }
    return res_sd;
}

static void FatFs_DeInit(void)
{
    //取消挂载文件系统
    TEST_DEBUG("取消挂载文件系统\r\n");
    res_sd = f_mount(NULL, "1:", 1);
}

static void FatFs_RWstr(void)
{
    //***********************写测试****************************
    //打开文件，如果文件不存在则创建它
    TEST_DEBUG("即将进行文件写入测试....\r\n");
    //打开文件，若不存在就创建
    res_sd = f_open(&fnew, "1:/fatfs.txt", FA_CREATE_ALWAYS | FA_WRITE);
    //文件打开成功
    if(res_sd == FR_OK)
    {
        TEST_DEBUG("打开文件成功！开始写入数据！\r\n");
        res_sd= f_write(&fnew, WriteBuffer, strlen(WriteBuffer), &fnum);
        
        if(res_sd == FR_OK)
        {
            TEST_DEBUG("数据写入成功，共写入%d个字符！\r\n", fnum);
            TEST_DEBUG("数据：%s", WriteBuffer);
        }
		else
		{
            TEST_DEBUG("数据写入失败！\r\n");
        }

        //关闭文件
        f_close(&fnew);
    }
    else
    {
        TEST_DEBUG("文件打开失败！错误码=%d\r\n", res_sd);
    }
    
    //***********************读测试****************************
    //请在电脑上创建read.txt文件，写点内容供测试
    //打开文件，如果文件不存在则打开刚才写入的文件
    TEST_DEBUG("即将进行文件读取测试....\r\n");
    res_sd = f_open(&fnew, "1:/read.txt", FA_OPEN_EXISTING | FA_READ);
    if(res_sd != FR_OK)
    {
        TEST_DEBUG("无法打开read.txt！错误码=%d, 打开刚才写入的文件.\r\n", res_sd);
        res_sd = f_open(&fnew, "1:/fatfs.txt", FA_OPEN_EXISTING | FA_READ);
    }
    //文件打开成功
    if(res_sd == FR_OK)
    {
        TEST_DEBUG("打开文件成功！开始读取数据！\r\n");
        res_sd= f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
        
        if(res_sd == FR_OK)
        {
            TEST_DEBUG("数据读取成功！\r\n");
            TEST_DEBUG("数据：%s\r\n", ReadBuffer);
        }
        else
        {
            TEST_DEBUG("数据读取失败！\r\n");
        }
        
        //关闭文件
        f_close(&fnew);
    }else{
        TEST_DEBUG("文件打开失败！错误码=%d\r\n", res_sd);
    }
}

static void FatFs_SpeedTest(void)
{
    uint32_t Tick0, cost_ms; //用于读写速度测试记时
    TEST_DEBUG("即将进行写入速度测试....\r\n");
    //打开文件，若不存在就创建
    res_sd = f_open(&fnew, "1:/speed_test.txt", FA_CREATE_ALWAYS | FA_WRITE);
    //文件打开成功
    if(res_sd == FR_OK)
    {
        uint32_t bytes = 0;
        Tick0 = HAL_GetTick();
        for(uint32_t i=0;i<1024;i++)
        {
            f_write(&fnew, WriteBuffer, strlen(WriteBuffer), &fnum);
            bytes += fnum;
        }
        f_close(&fnew);
        cost_ms = HAL_GetTick() - Tick0;
        printf("写入%dkiB, 耗时%dms, 平均%dkiB/s\r\n", \
               bytes/1024, cost_ms, (bytes*1000/1024)/cost_ms);
    }
    else
    {
        TEST_DEBUG("文件打开失败！错误码=%d\r\n", res_sd);
    }

    TEST_DEBUG("即将进行读速度测试....\r\n");
    //打开刚写入的测试文件
    res_sd = f_open(&fnew, "1:/speed_test.txt", FA_OPEN_EXISTING | FA_READ);
    //文件打开成功
    if(res_sd == FR_OK)
    {
        uint32_t bytes = 0;
        Tick0 = HAL_GetTick();
        fnum = 1;
        while(fnum != 0)
        {
            f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
            bytes += fnum;
        }
        f_close(&fnew);
        cost_ms = HAL_GetTick() - Tick0;
        printf("读出%dkiB, 耗时%dms, 平均%dkB/s\r\n", \
               bytes/1024, cost_ms, bytes/cost_ms);
    }
    else
    {
        TEST_DEBUG("文件打开失败！错误码=%d\r\n", res_sd);
    }
    return 0;
}

int main(void)
{
    SystemClock_Config(CPU_CLK_160M);
    printf("enter main\r\n");
    RTC_Init();
    printf("RTC初始化完成\r\n");
    SPI_Init();
    printf("SPI初始化完成\r\n");
    res_sd = FatFs_Init();
    if(res_sd == FR_OK)
    {
        FatFs_RWstr();
        FatFs_SpeedTest();
        FatFs_DeInit();
        printf("全部测试已完成\r\n");
    }
    else
    {
        printf("初始化失败！结束测试\r\n");
    }
    while (1);
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