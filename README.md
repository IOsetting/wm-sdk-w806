**English Readme** | **[中文说明](https://github.com/IOsetting/wm-sdk-w806/blob/main/README.cn.md)**

# About 

A SDK for WinnerMicro MCU W806.

# SDK File Structure

```
WM_SDK 
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

# Quick Start In Linux

## Install Toolchain

### Download

* https://occ.t-head.cn/community/download
* 导航->工具->工具链-800系列->(For now, it is V3.10.29)
* Download according to your OS version, e.g. for Ubuntu20.04, download csky-elfabiv2-tools-x86_64-minilibc-yyyymmdd.tar

### Install

Be careful that the tar ball file use `./` as top level path, move it to a seperate folder or specify a target folder for the uncompressing.
```bash
mkdir csky-elfabiv2-tools-x86_64-minilibc-20210423
tar xvf csky-elfabiv2-tools-x86_64-minilibc-20210423.tar.gz  -C csky-elfabiv2-tools-x86_64-minilibc-20210423/
```
Then move it to somewhere you place your development toolchains, e.g. /opt/toolchains, set it read-only to normal users
```bash
cd /opt/toolchains/
sudo mv ~/Download/csky-elfabiv2-tools-x86_64-minilibc-20210423/ .
sudo chown -R root:root csky-elfabiv2-tools-x86_64-minilibc-20210423/
```
You don't need to add it to system PATH variable.

## Compile

Checkout this project
```bash
git clone https://github.com/IOsetting/wm-sdk-w806.git
```

Run menuconfig, set the toolchains path
```bash
cd wm-sdk-w806
make menuconfig
```

In menuconfig, navigate to Toolchain Configuration, In the second line "the toolchain path", input the absolute path of the toolchains, e.g.
```
/opt/toolchains/csky-elfabiv2-tools-x86_64-minilibc-20210423/bin/
```
Leave other settings unchanged, save and exit menuconfig

Now, compile the SDK
```bash
make
```

## Flash

After connecting the development board to your PC, check the USB port name using commands `dmesg`, `lsusb` and `ls /dev/tty*`.

Then run menuconfig to set the download port
```bash
cd wm-sdk-w806
make menuconfig
```
In menuconfig, navigate to Download Configuration -> download port, input the USB port name you just got, e.g. `ttyUSB0`, then save and exit menuconfig

Then download the hex to the development board by
```bash
make flash
```
Press the `Reset` key according to the prompt
```
build finished!
connecting serial...
serial connected.
wait serial sync.........
please manually reset the device. <--- Press the Reset key
.....
serial sync sucess.
mac CC-CC-CC-CC-CC-CC.
start download.
0% [###] 100%
download completed.
please manually reset the device.
```
You need to press `Reset` key again to make it run, or it will stay in download mode (keep bumping CCCCCC).

<br>

# Quick Start In Windows

## Download 

* Download MSYS2 from [https://www.msys2.org/](https://www.msys2.org/)
* Download toolchains: [https://occ.t-head.cn/community/download](https://occ.t-head.cn/community/download)
   * 导航 -> 工具 -> 工具链-800系列 -> (Currently it is V3.10.29)
   * Download csky-elfabiv2-tools-mingw-minilibc-yyyymmdd.tar.gz for Windows

## Install

* Run msys2-x86_64-yyyymmdd.exe to install MSYS2
* Install necessary tools in MSYS2 console

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
Write down the full path to toolchain executables, e.g. `/d/w806/csky-elfabiv2-tools-mingw-minilibc-20210423/bin/`.

## Compilation

Check out this SDK
```bash
git clone https://github.com/IOsetting/wm-sdk-w806.git
```

Configurate toolchains path in menuconfig
```bash
cd wm-sdk-w806
make menuconfig
```

In menuconfig, navigate to Toolchain Configuration -> toolchain path, input the toolchain path you just got(including the tail slash '/'), e.g.
```
/d/w806/csky-elfabiv2-tools-mingw-minilibc-20210423/bin/
```
Leave other settings unchanged, save and exit menuconfig.

Then compile the project
```bash
make
```
The result bin is under bin/W806.

## Flash

* Connect the development board to your PC
* Run flash tool Upgrade_Tools_V1.4.8.exe
* Select the correct COM port, and use the default baud rate 115200, Click `打开串口`
* Select bin file 'W806.fls' from bin/W806
* Click `下载` to start download
* Press `Reset` key to reset board, then the tool will start downloading.
* When download is finished, press `Reset` key again to make it run.

# Problems

If you change the code, but nothing is changed after compilation, you need to clean the workspace with
```bash
# Clean previouse files
make distclean
# Recompile
make
```