[English Readme](https://github.com/IOsetting/wm-sdk-w806/blob/main/README.md) | 中文说明

# 关于 

联盛德MCU W806 开发套件

## SDK 文件结构

```
WM_SDK 
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

# 使用

## 安装编译工具

### 编译工具下载

* https://occ.t-head.cn/community/download
* 导航->工具->工具链-800系列->(当前是V3.10.29)
* 根据自己的操作系统, 下载对应版本, 对于Ubuntu20.04, 下载 csky-elfabiv2-tools-x86_64-minilibc-yyyymmdd.tar

### 编译工具安装

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
cd w806_makefile
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

## 烧录

首先通过`dmesg`,`lsusb`, `ls /dev/tty*`等命令确定自己开发板在系统中对应的USB端口, 例如`ttyUSB0`.  

运行menuconfig, 配置端口名称
```bash
cd w806_makefile
make menuconfig
```
在menuconfig界面中, Download Configuration -> download port, 填入开发板在你的系统中对应的USB端口, 例如`ttyUSB0`, 注意这里只需要填纯端口名, 不需要用完整的路径. 其他不用动, Save后退出menuconfig

执行烧录
```bash
make flash
```
根据输出的提示, 按一下reset键, 就会开始下载了. 
```
enerate compressed image completed.
build finished!
connecting serial...
serial connected.
wait serial sync.........         
please manually reset the device. <----- 这里按下reset
....
serial sync sucess.
mac CC-CC-CC-CC-CC-CC.
start download.
0% [###] 100%
download completed.
please manually reset the device.
```
下载完成后, 需要再按一次`Reset`让程序运行, 否则开发板会一直处在下载状态(不断输出 CCCCCC).

# 问题

如果出现`can not open serial make: *** [tools/w806/rules.mk:158: flash] Error 255`错误, 检查一下是否有其他串口软件占用了这个端口, 如果有需要先关闭
