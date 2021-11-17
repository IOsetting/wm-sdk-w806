English Readme | [中文说明](https://github.com/IOsetting/wm-sdk-w806/blob/main/README.cn.md)

# About 

A SDK for WinnerMicro MCU W806, it's migrated from W800 SDK.

## SDK File Structure

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

# How To

## Install Toolchain

### Download

* https://occ.t-head.cn/community/download
* 导航->工具->工具链-800系列->(For now, it is V3.10.29)
* Download according to your OS version, e.g. for Ubuntu20.04, download csky-elfabiv2-tools-x86_64-minilibc-yyyymmdd.tar

### Install

Be careful that the tar ball file use `./` as top level path, move it to a seperate folder or specify a target folder for the uncompression
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
cd w806_makefile
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
cd w806_makefile
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


