# 示例: ST77xx

W806开发板 ST7735/ST7789 TFT LCD 显示屏示例的使用说明.

# 接线

## ST7735, ST7735S, ST7735R

大多数 ST7735 LCD 显示屏使用8线的排针, 预设为4线SPI模式, pin脚为: 
* GND, 接地
* VCC, 接3.3V
* SCL, SPI时钟, 连接到 SPI SCK/CLK
* SDA, SPI数据, 连接到 SPI MOSI
* RES, reset脚, 连接到GPIO
* DC,  (或者 CD), 命令/数据切换脚, 连接到GPIO
* CS,  片选脚, 连接到SPI CS
* BL,  背光, 连接到GPIO

本示例使用的接线
* GND  -> GND
* VCC  -> 3.3V (切记, 不能接5V)
* SCL  -> PB15
* SDA  -> PB17
* RES  -> PB10
* DC   -> PB11
* CS   -> PB14
* BL   -> PB16

## ST7789, ST7789V

ST7789 LCD 显示屏的种类比较多, 带的排针数量也更多, 多出来的接线, 主要是用于背光LED和模式选择, 这些多出来的接线, 接到对应的GPIO上就可以. 主体的接线(GND, VCC, SCL/SCK/CLK, SDA, RES/RESET, DC, CS, BL/LEDA)和ST7735是一样的.

需要确认ST7789 LCD 工作在4线SPI模式.

# 代码

演示代码的文件结构为
```
├── ascii_fonts.c # 英文字体文件
├── ascii_fonts.h # 英文字体头文件
├── main.c        # 程序入口
├── st7735.c      # ST7735的初始化方法
├── st7735.h      # ST7735的头文件
├── st7789.c      # ST7789的初始化方法
├── st7789.h      # ST7789的头文件
├── st77xx.c      # ST7735和ST7789的公共方法
├── st77xx.h      # ST7735和ST7789的公共方法头文件
├── testimg.h     # 测试图片, 尺寸为128x128
├── wm_hal_msp.c  # 外设初始化方法
└── wm_it.c       # 中断处理方法
```

为运行演示代码, 
* 将 app/src 目录下除了 Makefile 以外的其他文件删除
* 将本目录下的所有文件复制到 app/src

## 配置

### st77xx.h

基础配置
* ST77XX\_BUF\_SIZE 缓冲的大小, 单位为字节
* ST77XX\_HARDWARE\_SPI 是否使用硬件SPI, 0:否, 1:是
* ST77XX\_**\_PORT 和 ST77XX\_**\_PIN 各pin脚的选择
```c
#define ST77XX_BUF_SIZE         1024
#define ST77XX_HARDWARE_SPI     1

// 如果修改了pin脚, 需要对应调整接线

// CS: B4, B14
#define ST77XX_CS_PORT      GPIOB
#define ST77XX_CS_PIN       GPIO_PIN_14
// SCK: B1, B2, B15, B24
#define ST77XX_SCK_PORT     GPIOB
#define ST77XX_SCK_PIN      GPIO_PIN_15
// MOSI: B5, B17, B26, PA7
#define ST77XX_MOSI_PORT    GPIOB
#define ST77XX_MOSI_PIN     GPIO_PIN_17
// MISO: B0, B3, B16, B25

#define ST77XX_RES_PORT     GPIOB
#define ST77XX_RES_PIN      GPIO_PIN_10
#define ST77XX_DC_PORT      GPIOB
#define ST77XX_DC_PIN       GPIO_PIN_11
#define ST77XX_BL_PORT      GPIOB
#define ST77XX_BL_PIN       GPIO_PIN_16
```
LCD 设置

在下面的液晶屏尺寸和显示方向色彩格式中选择适合自己屏幕的配置, 并取消注释(注意不要重复定义), 如果没有合适的就需要自己定义
```c
#define ST77XX_WIDTH  128
#define ST77XX_HEIGHT 160
#define ST77XX_XSTART 2
#define ST77XX_YSTART 1
#define ST77XX_ROTATION (ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB)
```
对于大多数 ST7789 LCD 显示屏, 使用宽度 240, 高度 320, xstart 和 ystart 都是 0.

# 使用

### wm_hal_msp.c

添加外设初始化/反初始化代码, 如果使用软件SPI可以跳过这步.
```c
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    __HAL_RCC_SPI_CLK_ENABLE();
    __HAL_AFIO_REMAP_SPI_CS(ST77XX_CS_PORT, ST77XX_CS_PIN);
    __HAL_AFIO_REMAP_SPI_CLK(ST77XX_SCK_PORT, ST77XX_SCK_PIN);
    __HAL_AFIO_REMAP_SPI_MOSI(ST77XX_MOSI_PORT, ST77XX_MOSI_PIN);
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
    __HAL_RCC_SPI_CLK_DISABLE();
    HAL_GPIO_DeInit(ST77XX_CS_PORT, ST77XX_CS_PIN);
    HAL_GPIO_DeInit(ST77XX_SCK_PORT, ST77XX_SCK_PIN);
    HAL_GPIO_DeInit(ST77XX_MOSI_PORT, ST77XX_MOSI_PIN);
}
```

### main.c

引入头文件, 如果使用的是ST7789 LCD, 将 `st7735.h` 替换为 `st7789.h`.
```c
#include "st7735.h"
#include "testimg.h"
```
Initialize SPI
```c
SPI_HandleTypeDef hspi;

static void SPI_Init(void)
{
    hspi.Instance = SPI;
    hspi.Init.Mode = SPI_MODE_MASTER;
    hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi.Init.NSS = SPI_NSS_SOFT;
    hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi.Init.FirstByte = SPI_LITTLEENDIAN;

    if (HAL_SPI_Init(&hspi) != HAL_OK)
    {
        Error_Handler();
    }
}
```
在 main() 方法中初始化, 如果使用的是ST7789 LCD, 将 `ST7735_Init()` 替换为 `ST7789_Init()`
```c
int main(void)
{
    //...
    ST77XX_GPIO_Init();
    SPI_Init();
    ST7735_Init();
    //...
}
```
然后就可以调用ST77XX_开头的方法输出显示了.

