**English Readme** | **[中文说明](https://github.com/IOsetting/wm-sdk-w806/blob/main/README.cn.md)**

# About 

A SDK for WinnerMicro MCU W806.

# File Structure

```
wm-sdk-w806
├─app              # User application code
├─bin              # Compilation results
├─demo             # Demos
├─include          # SDK header files 
├─ld               # Link scripts
├─lib              # Libraries
├─Makefile
├─platform         # SDK source code
└─tools            # Utilities
```

# For Linux Users

## Download Toolchains

* https://occ.t-head.cn/community/download
* You need to register an account for download
* 导航->工具->工具链-800系列->(For now, it is V3.10.29)
* Download according to your OS version, e.g. for Ubuntu20.04, download csky-elfabiv2-tools-x86_64-minilibc-yyyymmdd.tar.gz
* If you can not download from above link, try
   * https://pan.baidu.com/s/1Mp-oHNM3k4Hb8vEybv8pZg code:vw42
   * https://ipfs.io/ipfs/QmNmtn1gsqWq2641vJJrLbBfENCKHT4xpiH5cdaPnkxzqd
   * https://gateway.ipfs.io/ipfs/QmNmtn1gsqWq2641vJJrLbBfENCKHT4xpiH5cdaPnkxzqd

## Installation

Extract the toolchains to proper folder -- be careful that the tar ball use `./` as top level path, move it to a seperate folder or specify a target folder for the uncompressing.
```bash
mkdir csky-elfabiv2-tools-x86_64-minilibc-20210423
tar xvf csky-elfabiv2-tools-x86_64-minilibc-20210423.tar.gz  -C csky-elfabiv2-tools-x86_64-minilibc-20210423/
```
Then move it to somewhere, e.g. /opt/toolchains, set it read-only to normal users
```bash
cd /opt/toolchains/
sudo mv ~/Download/csky-elfabiv2-tools-x86_64-minilibc-20210423/ .
sudo chown -R root:root csky-elfabiv2-tools-x86_64-minilibc-20210423/
```
You don't need to add it to the system PATH variable.

## Building

Checkout this project
```bash
git clone https://github.com/IOsetting/wm-sdk-w806.git
```

Run menuconfig, configurate the toolchains path
```bash
cd wm-sdk-w806
make menuconfig
```

In menuconfig, navigate to `Toolchain Configuration`, In the second line "the toolchain path", input the absolute path of the toolchains executables, e.g.
```
/opt/toolchains/csky-elfabiv2-tools-x86_64-minilibc-20210423/bin/
```
Leave other settings unchanged, save and exit menuconfig.

Then build the project
```bash
make
```

### More Building Options

* UART0 `printf()` output  
  In /include/arch/xt804/csi_config.h, option `USE_UART0_PRINT` controls whether use UART0 to send `printf()` output, it is turned on by default. Note: This feature will occupy UART0, if any other devices are going to communicate with UART0, please turn it off.
* Hands-free download  
  In /include/arch/xt804/csi_config.h, option `USE_UART0_AUTO_DL` controls enable/disable automatic download, it is turned off by default. When it is enabled, download tool will reset the board automatically before the downloading. Option `USE_UART0_PRINT` should be enabled to make this work.

## Download To Development Board

Connect the development board to PC, get the USB port name by commands `dmesg`, `lsusb` and `ls /dev/tty*`.

Run menuconfig to set the download port
```bash
cd wm-sdk-w806
make menuconfig
```
In menuconfig, navigate to `Download Configuration`
* Download port: Input the USB port name, e.g. `ttyUSB0`;
* Down rate: Input the UART baud rate, the higher rate the higher download speed. The availabe options are `115200`, `460800`, `921600`, `1000000` and `2000000`.

Then save and exit menuconfig, download the hex file to development board
```bash
make flash
```

Press the `Reset` key to start the downloading. If previously downloaded hex was built with `USE_UART0_AUTO_DL` enabled, the board will start downloading automatically.
```
build finished!
connecting serial...
serial connected.
wait serial sync.........         <--- Press the Reset key here
please manually reset the device. <--- (Or here)
.....
serial sync sucess.
mac CC-CC-CC-CC-CC-CC.
start download.
0% [###] 100%
download completed.
reset command has been sent.
```
When downloding finishes, the board will be reset automatically to run the new program. In case the auto-reset fails, you need to press the reset key manually to make it run.

### More Download Options

Show serial ports
```bash
make list
```
Build, download and start serial monitor
```bash
make run
```
Start serial monitor only
```bash
make monitor
```

<br>

# For Windows Users

## Download Toolchains

* Download MSYS2 from [https://www.msys2.org/](https://www.msys2.org/)
* Download toolchains: [https://occ.t-head.cn/community/download](https://occ.t-head.cn/community/download)
   * 导航 -> 工具 -> 工具链-800系列 -> (Currently it is V3.10.29)
   * Download csky-elfabiv2-tools-mingw-minilibc-yyyymmdd.tar.gz for Windows
* If you can not download from above link, try
   * https://pan.baidu.com/s/1Mp-oHNM3k4Hb8vEybv8pZg code:vw42
   * https://ipfs.io/ipfs/QmNmtn1gsqWq2641vJJrLbBfENCKHT4xpiH5cdaPnkxzqd
   * https://gateway.ipfs.io/ipfs/QmNmtn1gsqWq2641vJJrLbBfENCKHT4xpiH5cdaPnkxzqd
* Download Flash Tool: [https://h.hlktech.com/Mobile/download/fdetail/143.html](https://h.hlktech.com/Mobile/download/fdetail/143.html)
   * Click the download link right to 'W800串口烧录工具_V1.4.8(.rar)'

## Installation

* Run msys2-x86_64-yyyymmdd.exe to install MSYS2
* When the installation finishes, install necessary tools in MSYS2 console

```bash
# Update all packages
pacman -Syu
# Install make
pacman -S msys/make
# Install automake
pacman -S msys/automake
# Install autoconf
pacman -S msys/autoconf
# Install gcc
pacman -S msys/gcc
# Install git
pacman -S msys/git
# Install dependencies
pacman -S msys/ncurses-devel
pacman -S msys/gettext-devel
```

Extract toolchains to specified folder
```
mkdir csky-elfabiv2-tools-mingw-minilibc-20210423
tar xvf csky-elfabiv2-tools-mingw-minilibc-20210423.tar.gz -C csky-elfabiv2-tools-mingw-minilibc-20210423/
```
Note down the full path of toolchain executables, e.g. `/d/w806/csky-elfabiv2-tools-mingw-minilibc-20210423/bin/`.

## Building

Check out this SDK
```bash
git clone https://github.com/IOsetting/wm-sdk-w806.git
```

Configurate toolchains path in menuconfig
```bash
cd wm-sdk-w806
make menuconfig
```

In menuconfig, navigate to Toolchain Configuration -> toolchain path, input the path (including the tail slash '/'), e.g.
```
/d/w806/csky-elfabiv2-tools-mingw-minilibc-20210423/bin/
```
Leave other settings unchanged, save and exit menuconfig.

Then build the project
```bash
make
```
The result files are under bin/W806.

## Download To Development Board

### Option 1: Upgrade_Tools

* Connect the development board to your PC
* Run flash tool Upgrade_Tools_V1.4.8.exe
* Select the correct COM port, and use the default baud rate 115200, Click `打开串口`
* Select bin file 'W806.fls' from bin/W806
* Click `下载` to start download
* Press `Reset` key to reset board, then it will start downloading.
* When downloading finishes, press `Reset` key again to make it run.

### Option 2: wm_tool

Find the port name of connected board in Device Manager,  e.g. `COM5`.  

Run menuconfig to set the download port
```bash
cd wm-sdk-w806
make menuconfig
```
In menuconfig, navigate to `Download Configuration`
* Download port: Input the USB port name, e.g. `COM5`;
* Down rate: The higher rate the higher download speed, the availabe options are `115200`, `460800`, `921600`, `1000000` and `2000000`.

The rest are the same as operations in Linux, please refer to `Linux - Download To Development Board`.

# Problems

1. Download Failed  
When it shows `can not open serial make: *** [tools/w806/rules.mk:158: flash] Error 255`, check if any other applications are occupying the USB port, if yes, close it and retry.
2. Delay function failed in FreeRTOS  
In /include/arch/xt804/csi_config.h, please comment out `#define CONFIG_KERNEL_NONE 1`
3. Hex is not updated  
If the compilation doesn't reflect your code changes, please clean the workspace with`make clean` or `make distclean`
4. Auto-reset failed  
In some cases the auto-reset may fail, you need to press the Reset key to reset the board manually.

# License

The code of WM-SDK-W806 is licensed under the Apache-2.0 license.

As WM-SDK-W806 includes code from many upstream projects it includes many copyright owners. WM-SDK-W806 makes NO claim of copyright on any upstream code. Patches to upstream code have the same license as the upstream project, unless specified otherwise. For a complete copyright list please checkout the source code to examine license headers. 

Unless expressly stated otherwise all code submitted to the WM-SDK-W806 project (in any form) will be licensed under Apache-2.0 license. You are absolutely free to retain copyright. To retain copyright simply add a copyright header to each submitted code page. If you submit code that is not your own work it is your responsibility to resolve the conflicts and place a header stating the copyright.
