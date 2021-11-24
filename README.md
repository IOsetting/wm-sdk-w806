**English Readme** | **[中文说明](https://github.com/IOsetting/wm-sdk-w806/blob/main/README.cn.md)**

# About 

A SDK for WinnerMicro MCU W806.

# File Structure

```
wm-sdk-w806
├─app              # User application source code
├─bin              # Compilaion results
├─demo             # Demos
├─doc              # Release notes, API Docs 
├─include          # API header files 
├─ld               # Link scripts
├─lib              # Libraries
├─Makefile
├─platform         # Chip&Platform related common code
├─src              # Applications, network protocols, OS and 3rd party source code
└─tools            # Compilation scripts, utilities for CDS IDE project、CDK project and IMAGE generation
```

# For Linux Users

## Download Toolchains

* https://occ.t-head.cn/community/download
* You need to register an account for download
* 导航->工具->工具链-800系列->(For now, it is V3.10.29)
* Download according to your OS version, e.g. for Ubuntu20.04, download csky-elfabiv2-tools-x86_64-minilibc-yyyymmdd.tar.gz
* If you cannot download it from above link, try this https://pan.baidu.com/s/1Mp-oHNM3k4Hb8vEybv8pZg code:vw42

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

In menuconfig, navigate to Toolchain Configuration, In the second line "the toolchain path", input the absolute path of the toolchains executables, e.g.
```
/opt/toolchains/csky-elfabiv2-tools-x86_64-minilibc-20210423/bin/
```
Leave other settings unchanged, save and exit menuconfig.

Then build the project
```bash
make
```


## Download To Development Board

Connect the development board to PC, get the USB port name by commands `dmesg`, `lsusb` and `ls /dev/tty*`.

Run menuconfig to set the download port
```bash
cd wm-sdk-w806
make menuconfig
```
In menuconfig, navigate to Download Configuration -> download port, input the USB port name, e.g. `ttyUSB0`, then save and exit menuconfig

Then download the hex file to development board
```bash
make flash
```
Auto download is enable by default in the program. If it is first download, 
you need to manually press the `Reset` key to start downloading.
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
send reset command OK.
```
after downloading, auto send reset command.

<br>

# For Windows Users

## Download Toolchains

* Download MSYS2 from [https://www.msys2.org/](https://www.msys2.org/)
* Download toolchains: [https://occ.t-head.cn/community/download](https://occ.t-head.cn/community/download)
   * 导航 -> 工具 -> 工具链-800系列 -> (Currently it is V3.10.29)
   * Download csky-elfabiv2-tools-mingw-minilibc-yyyymmdd.tar.gz for Windows
   * If you cannot download it from above link, try this link https://pan.baidu.com/s/1Mp-oHNM3k4Hb8vEybv8pZg code:vw42
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

* Connect the development board to your PC
* Run flash tool Upgrade_Tools_V1.4.8.exe
* Select the correct COM port, and use the default baud rate 115200, Click `打开串口`
* Select bin file 'W806.fls' from bin/W806
* Click `下载` to start download
* Press `Reset` key to reset board, then it will start downloading.
* When downloading finishes, press `Reset` key again to make it run.

# Problems

If the compilation doesn't reflect your code changes, please clean the workspace with
```bash
# Clean previouse files
make distclean
# Recompile
make
```
