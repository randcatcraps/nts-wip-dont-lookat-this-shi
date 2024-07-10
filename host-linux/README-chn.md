simple host program implementation for Linux, TUI based

| [English](/host-linux/README.md) |

### 依赖

| 发行版        | 安装指令                                                          |
| :------------| :--------------------------------------------------------------- |
| Debian 系    | apt install build-essential cmake libusb-1.0-0-dev libdialog-dev |
| archlinux 系 | pacman -S --needed base-devel cmake libusb dialog                |

### 编译

> [!NOTE]
>
> 注意：排错时，请将 `Release` 修改为 `Debug`

```shell
cmake -S . -DCMAKE_BUILD_TYPE=Release \
           -DCMAKE_INSTALL_PREFIX=$HOME/NtsHostLinux
make
make install
```

### 安装

将 `99-nts-uinput.rules` 复制到 `/etc/udev/rules.d` 或 `/usr/lib/udev/rules.d` (取决于您的发行版), 同时将 `uinput.conf` 复制到 `/etc/modules-load.d`

```shell
groupadd -r uinput
usermod -aG uinput $USER
```

> [!NOTE]
>
> 完成后需要重启

### 用法

```shell
~/NtsHostLinux/bin/nts_host_linux
```
