# Demo: ST77xx

Here are some notes on interfacing an ST7735/ST7789 TFT LCD display with W806 board.

# Wiring

## ST7735, ST7735S, ST7735R

Most ST7735 LCD display boards come with an 8 pin male header, 4-line SPI mode, The pins are: 
* GND, wire it to GND
* VCC, wire it to 3.3V
* SCL, the SPI Clock pin, wire it to SPI SCK/CLK
* SDA, the data pin, wire it to SPI MOSI
* RES, reset pin, wire it to GPIO
* DC,  (or CD), the command/data switch pin, wire it to GPIO
* CS,  chip select pin, wire it to SPI CS
* BL,  backlight, wire it to GPIO

The wiring for this demo
* GND  -> GND
* VCC  -> 3.3V (never, never use 5.0V)
* SCL  -> PB15
* SDA  -> PB17
* RES  -> PB10
* DC   -> PB11
* CS   -> PB14
* BL   -> PB16

## ST7789, ST7789V

ST7789 LCD display boards may come with more pins, the additional pins are for backlight LED control and mode options so just wire them to normal GPIO. The wiring for the main part (GND, VCC, SCL/SCK/CLK, SDA, RES/RESET, DC, CS, BL/LEDA) is the same as ST7735.

Make sure your ST7789 LCD runs in 4-line SPI mode.

# Code

The file structure of this demo
```
├── ascii_fonts.c # ASCII fonts
├── ascii_fonts.h # ASCII fonts header
├── main.c        # 
├── st7735.c      # ST7735 init method
├── st7735.h      # ST7735 header file
├── st7789.c      # ST7789 init method
├── st7789.h      # ST7789 header file
├── st77xx.c      # ST77xx common methods
├── st77xx.h      # ST77xx common header
├── testimg.h     # Image for test, dimension 128x128
├── wm_hal_msp.c  # W806 peripheral init/deinit methods
└── wm_it.c       # Interrupt handlers
```

In order to run the demo, 
* Remove all files in app/src excepth Makefile
* Copy all files in this folder to app/src

## Configuration

### st77xx.h

Basic settings
```c
// Buffer size, unit is byte
#define ST77XX_BUF_SIZE         1024
// Enable or disable hardware SPI, 0:Disable, 1:Enable
#define ST77XX_HARDWARE_SPI     1

// Define the function pins, if you change the pins,
// you will need to adjust the wiring accordingly

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
LCD settings

There are some pre-defined LCD settings, uncomment the settings according to your LCD board, and leave others commented. If none matches, you can create new settings for your LCD board.
```c
#define ST77XX_WIDTH  128
#define ST77XX_HEIGHT 160
#define ST77XX_XSTART 2
#define ST77XX_YSTART 1
#define ST77XX_ROTATION (ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB)
```
for most ST7789 LCD boards, the width would be 240 and height would be 320, and xstart, ystart are always 0.

# Use

### wm_hal_msp.c

Add the peripheral init/deinit code. Skip this step if you don't use hardware SPI.
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

Include the headers. Replace `st7735.h` with `st7789.h` for ST7789 LCD board
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
In main(), replace `ST7735_Init()` with `ST7789_Init()` for ST7789 LCD board
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
Then you can display what you want with your LCD board.

