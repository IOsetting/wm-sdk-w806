/**
 * @file spisd.h
 * @brief spi SD card driver
 * @note copy from https://www.cnblogs.com/dengxiaojun/p/4279439.html
 */

#ifndef __SPISD_H_
#define __SPISD_H_

#include <stdint.h>

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t

// SD卡类型定义
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06

// SD卡指令表
#define CMD0    0       //卡复位
#define CMD1    1
#define CMD8    8       //命令8 ，SEND_IF_COND
#define CMD9    9       //命令9 ，读CSD数据
#define CMD10   10      //命令10，读CID数据
#define CMD12   12      //命令12，停止数据传输
#define CMD16   16      //命令16，设置SectorSize 应返回0x00
#define CMD17   17      //命令17，读sector
#define CMD18   18      //命令18，读Multi sector
#define CMD23   23      //命令23，设置多sector写入前预先擦除N个block
#define CMD24   24      //命令24，写sector
#define CMD25   25      //命令25，写Multi sector
#define CMD41   41      //命令41，应返回0x00
#define CMD55   55      //命令55，应返回0x01
#define CMD58   58      //命令58，读OCR信息
#define CMD59   59      //命令59，使能/禁止CRC，应返回0x00

// SD卡中的响应有许多种，R1为标准响应，最为常用。与R1响应相似的还有R1b、R2和R3。
// R1响应在除SEND_STATUS外其它命令后发送，也是最高位先发送，共1个字节。最高位为0。响应说明如下：
// 0x01：空闲状态
// 0x02：擦除错误
// 0x04：命令错误
// 0x08：CRC通信错误
// 0x10：擦除次序错误
// 0x20：地址错误
// 0x40：参数错误

#define MSD_RESPONSE_NO_ERROR      0x00    //无错误
#define MSD_IN_IDLE_STATE          0x01    //空闲状态
#define MSD_ERASE_RESET            0x02    //擦除错误
#define MSD_ILLEGAL_COMMAND        0x04    //命令错误
#define MSD_COM_CRC_ERROR          0x08    //CRC通信错误
#define MSD_ERASE_SEQUENCE_ERROR   0x10    //擦除次序错误
#define MSD_ADDRESS_ERROR          0x20    //地址错误
#define MSD_PARAMETER_ERROR        0x40    //参数错误
#define MSD_RESPONSE_FAILURE       0xFF    //这次命令根本是失败的，没有任何回应

u8 SdInitialize(void);

u8 SdGetCID(u8 *cid_data);

u8 SdGetCSD(u8 *csd_data);

u32 SdGetSectorCount(void);

u8 SdReadDisk(u8*buf,u32 sector,u8 cnt);

u8 SdWriteDisk(u8*buf,u32 sector,u8 cnt);


#endif