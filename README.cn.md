[English Readme](https://github.com/IOsetting/wm-sdk-w806/blob/main/README.md) | 中文说明

# 关于 

联盛德MCU W806 开发套件

## 文件结构

```
wm-sdk-w806
├─app              # 用户应用代码
├─bin              # 编译中间及结果产物
├─demo             # 功能演示代码
├─doc              # 文档及发布说明
├─include          # API头文件
├─ld               # 链接脚本
├─lib              # 库文件
├─Makefile
├─platform         # 芯片和平台相关公共代码
├─src              # 操作系统,网络协议及应用和第三方项目源代码
└─tools            # 编译脚本和工具
```

# Linux环境说明

## 下载

* https://occ.t-head.cn/community/download 下载编译工具
* 导航->工具->工具链-800系列->(当前是V3.10.29)
* 根据自己的操作系统, 下载对应版本, 对于Ubuntu20.04, 下载 csky-elfabiv2-tools-x86_64-minilibc-yyyymmdd.tar
* 如果上面的链接有问题或者无法注册, 可以通过百度盘下载 https://pan.baidu.com/s/1Mp-oHNM3k4Hb8vEybv8pZg code:vw42

## 安装

上面下载的tar.gz文件外层路径用的是`./`目录, 建议放到一个单独的目录中解压, 或者指定解压目录解压目录, 参考命令如下
```bash
mkdir csky-elfabiv2-tools-x86_64-minilibc-20210423
tar xvf csky-elfabiv2-tools-x86_64-minilibc-20210423.tar.gz  -C csky-elfabiv2-tools-x86_64-minilibc-20210423/
```
移动到/opt下, 目录可以自己定, 设置权限禁止普通用户修改
```bash
cd /opt/toolchains/
sudo mv ~/Download/csky-elfabiv2-tools-x86_64-minilibc-20210423/ .
sudo chown -R root:root csky-elfabiv2-tools-x86_64-minilibc-20210423/
```
不需要添加系统路径

## 编译

导出此项目
```bash
git clone https://github.com/IOsetting/wm-sdk-w806.git
```
运行menuconfig, 配置工具路径
```bash
cd wm-sdk-w806
make menuconfig
```
在menuconfig界面中, Toolchain Configuration -> 第二个toolchain path, 将刚才的路径填进去, 需要完整路径, 带最后的斜杆, 例如
```
/opt/toolchains/csky-elfabiv2-tools-x86_64-minilibc-20210423/bin/
```
其他不用动, Save后退出menuconfig

执行编译
```bash
make
```

### 更多编译选项

* 串口0打印`printf()`输出  
  在 /include/arch/xt804/csi_config.h 中通过`USE_UART0_PRINT`控制是否使用串口0打印`printf()`输出, 默认开启. 注意: 此功能会占用串口0, 如果需要使用串口0与其他设备通信, 请关闭此选项

* 免按键自动下载  
  在 /include/arch/xt804/csi_config.h 中通过`USE_UART0_AUTO_DL`控制是否开启免按键自动下载, 默认关闭. 需要先开启`USE_UART0_PRINT`才能开启此选项, 如果关闭`USE_UART0_PRINT`则此选项关闭.

## 写入开发板

首先通过`dmesg`,`lsusb`, `ls /dev/tty*`等命令确定自己开发板在系统中对应的USB端口, 例如`ttyUSB0`.  

运行menuconfig, 配置端口名称
```bash
cd wm-sdk-w806
make menuconfig
```
在menuconfig界面中, Download Configuration -> download port, 填入开发板在你的系统中对应的USB端口, 例如`ttyUSB0`, 注意这里只需要填纯端口名, 不需要用完整的路径. 可以调高波特率加快下载，只支持`115200`, `460800`, `921600`, `1000000`, `2000000`, Save后退出menuconfig

执行烧录
```bash
make flash
```
根据输出的提示, 按一下reset键就会开始下载. 如果前一次写入的固件已经开启了`USE_UART0_AUTO_DL`则不需要按键, 会自动开始下载
```
enerate compressed image completed.
build finished!
connecting serial...
serial connected.
wait serial sync.........         <----- 这里按下reset
please manually reset the device. <----- 或者这里
....
serial sync sucess.
mac CC-CC-CC-CC-CC-CC.
start download.
0% [###] 100%
download completed.
reset command has been sent.
```
下载完成后, 下载工具会发送复位指令, 复位成功后程序会自动开始执行.

### 更多下载选项

烧录并打开串口监视器 
```bash
make run
```
只打开串口监视器 
```bash
make monitor
```

<br>

# Windows环境说明

## 下载相关软件

* 下载MSYS2 [https://www.msys2.org/](https://www.msys2.org/), 下载MSYS2安装文件, 当前是 msys2-x86_64-20210725.exe
* 下载工具链 [https://occ.t-head.cn/community/download](https://occ.t-head.cn/community/download)
   * 导航 -> 工具 -> 工具链-800系列 -> (当前是V3.10.29)
   * 下载对应版本, 对于Win10, 下载 csky-elfabiv2-tools-mingw-minilibc-20210423.tar.gz
   * 如果无法下载或无法注册, 请通过百度盘下载 https://pan.baidu.com/s/1Mp-oHNM3k4Hb8vEybv8pZg code:vw42
* 下载烧录工具 [https://h.hlktech.com/Mobile/download/fdetail/143.html](https://h.hlktech.com/Mobile/download/fdetail/143.html)
   * 点击 'W800串口烧录工具_V1.4.8(.rar)' 右侧的下载链接下载

## 安装和配置

* 在本机运行MSYS2的安装文件安装MSYS2
* 在MSYS2命令行下, 参考下面的命令安装必要的软件

```bash
# 更新软件包
pacman -Syu
# 安装 make
pacman -S msys/make
# 安装 automake
pacman -S msys/automake
# 安装 autoconf
pacman -S msys/autoconf
# 安装 gcc
pacman -S msys/gcc
# 安装 git
pacman -S msys/git
# 安装编译时需要的依赖库
pacman -S msys/ncurses-devel
pacman -S msys/gettext-devel
```

解压工具链到指定目录, 注意这个tar包没有顶层目录, 建议指定目录解压
```
mkdir csky-elfabiv2-tools-mingw-minilibc-20210423
tar xvf csky-elfabiv2-tools-mingw-minilibc-20210423.tar.gz -C csky-elfabiv2-tools-mingw-minilibc-20210423/
```
记下这个目录的路径, 例如 `/d/w806/csky-elfabiv2-tools-mingw-minilibc-20210423/bin/` , 下面配置menuconfig需要用到

## 编译

导出SDK
```bash
git clone https://github.com/IOsetting/wm-sdk-w806.git
```
用menuconfig配置工具链路径
```bash
cd wm-sdk-w806
make menuconfig
```
在menuconfig界面中, Toolchain Configuration -> 第二个toolchain path, 将刚才的路径填入, 需要包含最后的`/`, 例如
```
/d/w806/csky-elfabiv2-tools-mingw-minilibc-20210423/bin/
```
其他使用默认设置, Save后退出menuconfig. 如果下面一排菜单高亮不显示, 可以使用快捷键`Alt+E`=退出, `Alt+S`=保存

然后执行编译
```bash
make
```
生成的固件在 bin/W806 目录下

## 写入开发板

### 选项一: 使用 Upgrade_Tools

* 连接开发板
* 运行官方烧录工具 Upgrade_Tools_V1.4.8.exe, 
* 选择正确的端口, 波特率使用默认的115200, 点击`打开串口`
* 选择刚才编译好的固件, 文件路径为 bin/W806/W806.fls
* 点击`下载`
* 短按开发板的`Reset`键, 等待烧录工具完成烧录
* 再次短按开发板的`Reset`键, 烧录好的程序会开始执行

### 选项二: 使用 wm_tool

首先通过Windows设备管理器确定自己开发板在系统中对应的USB端口, 例如`COM5`.  

运行menuconfig, 配置端口名称
```bash
cd wm-sdk-w806
make menuconfig
```
在menuconfig界面中, Download Configuration -> download port, 填入开发板在你的系统中对应的USB端口, 例如`COM5`, 注意这里只需要填纯端口名, 不需要用完整的路径. 
可以调高波特率加快下载，只支持`115200`, `460800`, `921600`, `1000000`, `2000000`, Save后退出menuconfig

其余烧录和选项与Linux环境相同, 请参考Linux相应说明.

## 问题

1. 下载失败
如果出现`can not open serial make: *** [tools/w806/rules.mk:158: flash] Error 255`错误, 检查一下是否有其他串口软件占用了这个端口, 如果有需要先关闭
2. 使用FreeRTOS时, 延时无效
请检查: 在/include/arch/xt804/csi_config.h中, 是否未将`#define CONFIG_KERNEL_NONE 1`宏定义注释掉?
3. 编译结果未更新
如果修改代码后编译, 发现固件未更新, 可以执行下面的命令清空旧的编译输出, 然后再次编译
```bash
# 清理旧的编译结果和中间结果
make distclean
# 重新编译
make
```
4. 自动复位失败
在个别情况下, 自动复位会失败, 这时候需要按开发板的 Reset 键进行手工复位