simple host program implementation for Linux, TUI based

| [简体中文](/host-linux/README-chn.md) |

### Dependencies

| Distro             | Command                                                          |
| :------------------| :--------------------------------------------------------------- |
| Debian-based       | apt install build-essential cmake libusb-1.0-0-dev libdialog-dev |
| archlinux-based    | pacman -S --needed base-devel cmake libusb dialog                |

### Build

> [!NOTE]
>
> When troubleshooting, please change `Release` into `Debug`

```shell
cmake -S . -DCMAKE_BUILD_TYPE=Release \
           -DCMAKE_INSTALL_PREFIX=$HOME/NtsHostLinux
make
make install
```

### Install

Copy `99-nts-uinput.rules` into `/etc/udev/rules.d` or `/usr/lib/udev/rules.d` (depending on your distro), and `uinput.conf` into `/etc/modules-load.d`

```shell
groupadd -r uinput
usermod -aG uinput $USER
```

> [!NOTE]
>
> Reboot

### Usage

```shell
~/NtsHostLinux/bin/nts_host_linux
```
