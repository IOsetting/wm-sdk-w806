/**
 * @file spisd.c
 * @brief SPI SD card driver for FATFS
 * @note copy from https://www.cnblogs.com/dengxiaojun/p/4279439.html
 * @note 为了方便移植,本文件作为跨平台驱动,不写具体平台SPI驱动
 */

#include "spisd.h"

//预定义SD卡类型
u8  SD_Type=0;//SD卡的类型

//这部分应根据具体的连线来修改!
extern void Write_CS_Pin(int x);

//data:要写入的数据
//返回值:读到的数据
extern u8 SdSpiReadWriteByte(u8 data);

//SD卡初始化的时候,需要低速
extern void SdSpiSpeedLow(void);

//SD卡正常工作的时候,可以高速了
extern void SdSpiSpeedHigh(void);


extern void SdIOInit(void);


//覆盖掉这两个函数,提高读写速度
__attribute__((weak)) void SdSpiReadData(u8* data, u32 len)
{
    while(len--){
        *data++ = SdSpiReadWriteByte(0xff);
    }
}

__attribute__((weak)) void SdSpiWriteData(u8* data, u32 len)
{
    while(len--){
        SdSpiReadWriteByte(*data);
    }
}

/**
 * @brief 等待卡准备好
 * @retval 0,准备好了;其他,错误代码
 */
static u8 SdWaitReady(void)
{
    u32 t=0;
    do
    {
        if(SdSpiReadWriteByte(0XFF)==0XFF)return 0;  //OK
        t++;
    }while(t<0XFFFFFF);                              //等待
    return 1;
}

/**
 * @brief 取消选择,释放SPI总线
 */
void SD_DisSelect(void)
{
    Write_CS_Pin(1);
    SdSpiReadWriteByte(0xff);  //提供额外的8个时钟
}

/**
 * @brief  选择sd卡,并且等待卡准备OK
 * @retval 0,成功;1,失败;
 */
u8 SdSelect(void)
{
    Write_CS_Pin(0);
    if(SdWaitReady()==0)return 0;  //等待成功
    SD_DisSelect();
    return 1;                      //等待失败
}

/**
 * @brief  等待SD卡回应
 * @param  Response:要得到的回应值
 * @retval 0,成功得到了该回应值;其他,得到回应值失败    期待得到的回应值
 */
u8 SdGetResponse(u8 Response)
{
    u16 Count=0xFFF;                           //等待次数
    while ((SdSpiReadWriteByte(0XFF)!=Response)&&Count)Count--;//等待得到准确的回应
    if (Count==0)return MSD_RESPONSE_FAILURE;  //得到回应失败
    else return MSD_RESPONSE_NO_ERROR;         //正确回应
}

/**
 * @brief  从sd卡读取一个数据包的内容
 * @param  buf : 数据缓存区
 * @param  len : 要读取的数据长度.
 * @retval 0,成功;0XFE数据起始令牌;其他,失败
 */
u8 SdRecvData(u8*buf,u16 len)
{
    if(SdGetResponse(0xFE))return 1;  //等待SD卡发回数据起始令牌0xFE
    SdSpiReadData(buf, len);
    //下面是2个伪CRC（dummy CRC）
    SdSpiReadWriteByte(0xFF);
    SdSpiReadWriteByte(0xFF);
    return 0;//读取成功
}

/**
 * @brief  向sd卡写入一个数据包的内容 512字节
 * @param  buf:数据缓存区
 * @param  cmd:指令
 * @retval 0,成功;其他,失败;
*/
u8 SdSendBlock(u8*buf,u8 cmd)
{
    u16 t;
    if(SdWaitReady())return 1;       //等待准备失效
    SdSpiReadWriteByte(cmd);
    if(cmd!=0XFD)                    //不是结束指令
    {
        SdSpiWriteData(buf, 512);//提高速度,减少函数传参时间
        SdSpiReadWriteByte(0xFF);    //忽略crc
        SdSpiReadWriteByte(0xFF);
        t=SdSpiReadWriteByte(0xFF);  //接收响应
        if((t&0x1F)!=0x05)return 2;  //响应错误
    }
    return 0;//写入成功
}


/**
 * @brief  向SD卡发送一个命令
 * @param  cmd : 命令
 * @param  arg : 命令参数
 * @param  crc : crc校验值
 * @retval SD卡返回的响应
 */
u8 SdSendCmd(u8 cmd, u32 arg, u8 crc)
{
    u8 r1;
    u8 Retry=0;
    SD_DisSelect();                  //取消上次片选
    if(SdSelect())return 0XFF;       //片选失效
    //发送
    SdSpiReadWriteByte(cmd | 0x40);  //分别写入命令
    SdSpiReadWriteByte(arg >> 24);
    SdSpiReadWriteByte(arg >> 16);
    SdSpiReadWriteByte(arg >> 8);
    SdSpiReadWriteByte(arg);
    SdSpiReadWriteByte(crc);
    if(cmd==CMD12)SdSpiReadWriteByte(0xff);//Skip a stuff byte when stop reading
    //等待响应，或超时退出
    Retry=0X1F;
    do
    {
        r1=SdSpiReadWriteByte(0xFF);
    }while((r1&0X80) && Retry--);
    //返回状态值
    return r1;
}

/**
 * @brief  获取SD卡的CID信息，包括制造商信息
 * @param  cid_data : 存放CID的内存，至少16Byte
 * @retval 0：NO_ERR, 1：错误
 */
u8 SdGetCID(u8 *cid_data)
{
    u8 r1;
    //发CMD10命令，读CID
    r1=SdSendCmd(CMD10,0,0x01);
    if(r1==0x00)
    {
        r1=SdRecvData(cid_data,16);  //接收16个字节的数据
    }
    SD_DisSelect();                  //取消片选
    if(r1)return 1;
    else return 0;
}

/**
 * @brief  获取SD卡的CSD信息，包括容量和速度信息
 * @param  csd_data : 存放CID的内存，至少16Byte
 * @retval 0：NO_ERR, 1：错误
 */
u8 SdGetCSD(u8 *csd_data)
{
    u8 r1;
    r1=SdSendCmd(CMD9,0,0x01);        //发CMD9命令，读CSD
    if(r1==0)
    {
        r1=SdRecvData(csd_data, 16);  //接收16个字节的数据
        r1=SdRecvData(csd_data, 16);  //接收16个字节的数据
        r1=SdRecvData(csd_data, 16);  //接收16个字节的数据
    }
    SD_DisSelect();//取消片选
    if(r1)return 1;
    else return 0;
}

/**
 * @brief  获取SD卡的总扇区数（扇区数）
 * @retval 0,取容量出错, 其他:SD卡的容量(扇区数/512字节)
 * @note   每扇区的字节数必为512，因为如果不是512，则初始化不能通过.
 */
u32 SdGetSectorCount(void)
{
    u8 csd[16];
    u32 Capacity;
    u8 n;
    u16 csize;
    //取CSD信息，如果期间出错，返回0
    if(SdGetCSD(csd)!=0) return 0;
    //如果为SDHC卡，按照下面方式计算
    if((csd[0]&0xC0)==0x40)               //V2.00的卡
    {
        csize = csd[9] + ((u16)csd[8] << 8) + 1;
        Capacity = (u32)csize << 10;      //得到扇区数
    }else//V1.XX的卡
    {
        n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
        csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
        Capacity= (u32)csize << (n - 9);  //得到扇区数
    }
    return Capacity;
}

/**
 * @brief  初始化SD卡
 * @retval 0,正常;其他,不正常.
 */
u8 SdInitialize(void)
{
    u8 r1;       // 存放SD卡的返回值
    u16 retry;   // 用来进行超时计数
    u8 buf[4];
    u16 i;
    
    SdIOInit();  //初始化IO
    for(i=0;i<10;i++)SdSpiReadWriteByte(0XFF);//发送最少74个脉冲  ，这里发送了80个脉冲
    retry=20;
    do
    {
        r1=SdSendCmd(CMD0,0,0x95); //进入IDLE状态
    }while((r1!=0X01) && retry--);
    SD_Type=0;                     //默认无卡
    if(r1==0X01)
    {
        if(SdSendCmd(CMD8,0x1AA,0x87)==1)               //SD V2.0
        {
            for(i=0;i<4;i++)buf[i]=SdSpiReadWriteByte(0XFF);    //Get trailing return value of R7 resp
            if(buf[2]==0X01&&buf[3]==0XAA)              //卡是否支持2.7~3.6V
            {
                retry=0XFFFE;
                do
                {
                    SdSendCmd(CMD55,0,0X01);            //发送CMD55
                    r1=SdSendCmd(CMD41,0x40000000,0X01);//发送CMD41
                }while(r1&&retry--);
                if(retry&&SdSendCmd(CMD58,0,0X01)==0)   //鉴别SD2.0卡版本开始
                {
                    for(i=0;i<4;i++)buf[i]=SdSpiReadWriteByte(0XFF);//得到OCR值
                    if(buf[0]&0x40)SD_Type=SD_TYPE_V2HC;            //检查CCS
                    else SD_Type=SD_TYPE_V2;
                }
            }
        }else//SD V1.x/ MMC V3
        {
            SdSendCmd(CMD55,0,0X01);            //发送CMD55
            r1=SdSendCmd(CMD41,0,0X01);         //发送CMD41
            if(r1<=1)
            {
                SD_Type=SD_TYPE_V1;
                retry=0XFFFE;
                do                              //等待退出IDLE模式
                {
                    SdSendCmd(CMD55,0,0X01);    //发送CMD55
                    r1=SdSendCmd(CMD41,0,0X01); //发送CMD41
                }while(r1&&retry--);
            }else
            {
                SD_Type=SD_TYPE_MMC;            //MMC V3
                retry=0XFFFE;
                do                              //等待退出IDLE模式
                {
                    r1=SdSendCmd(CMD1,0,0X01);  //发送CMD1
                }while(r1&&retry--);
            }
            if(retry==0||SdSendCmd(CMD16,512,0X01)!=0)SD_Type=SD_TYPE_ERR;//错误的卡
        }
    }
    SD_DisSelect();//取消片选
    SdSpiSpeedHigh();//高速
    if(SD_Type)return 0;
    else if(r1)return r1;
    return 0xaa;//其他错误
}

/**
 * @brief 读SD卡
 * @param  buf    : 数据缓存区
 * @param  sector : 起始扇区
 * @param  cnt    : 扇区数
 * @retval 0,ok;其他,失败.
 */
u8 SdReadDisk(u8*buf,u32 sector,u8 cnt)
{
    u8 r1;
    if(SD_Type!=SD_TYPE_V2HC)sector <<= 9; //转换为字节地址
    if(cnt==1)
    {
        r1=SdSendCmd(CMD17,sector,0X01);   //读命令
        if(r1==0)                          //指令发送成功
        {
            r1=SdRecvData(buf,512);        //接收512个字节
        }
    }else
    {
        r1=SdSendCmd(CMD18,sector,0X01);   //连续读命令
        do
        {
            r1=SdRecvData(buf,512);        //接收512个字节
            buf+=512;
        }while(--cnt && r1==0);
        SdSendCmd(CMD12,0,0X01);           //发送停止命令
    }
    SD_DisSelect();                        //取消片选
    return r1;                             //
}

/**
 * @brief 写SD卡
 * @param  buf    : 数据缓存区
 * @param  sector : 起始扇区
 * @param  cnt    : 扇区数
 * @retval 0,ok;其他,失败.
 */
u8 SdWriteDisk(u8*buf,u32 sector,u8 cnt)
{
    u8 r1;
    if(SD_Type!=SD_TYPE_V2HC)sector *= 512; //转换为字节地址
    if(cnt==1)
    {
        r1=SdSendCmd(CMD24,sector,0X01);    //读命令
        if(r1==0)//指令发送成功
        {
            r1=SdSendBlock(buf,0xFE);       //写512个字节
        }
    }else
    {
        if(SD_Type!=SD_TYPE_MMC)
        {
            SdSendCmd(CMD55,0,0X01);
            SdSendCmd(CMD23,cnt,0X01);      //发送指令
        }
        r1=SdSendCmd(CMD25,sector,0X01);    //连续读命令
        if(r1==0)
        {
            do
            {
                r1=SdSendBlock(buf,0xFC);   //发送512个字节
                buf+=512;
            }while(--cnt && r1==0);
            r1=SdSendBlock(0,0xFD);         //发送512个字节
        }
    }
    SD_DisSelect();                         //取消片选
    return r1;                              //
}

int MMC_disk_status()
{
    return 0;
}

int MMC_disk_initialize()
{
    printf("SdInitialize %d\r\n", SdInitialize());
    return 0;
}

int MMC_disk_read(uint8_t *buff, uint32_t sector, uint8_t cnt)
{
    return SdReadDisk(buff, sector, cnt);
}

int MMC_disk_write(buff, sector, count)
{
    return SdWriteDisk(buff, sector, count);
}