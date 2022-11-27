#Raspberry Pi4 linux kernel toolchain build and driver development

Basic RPI configuration:
username: pi
password: pi

## Part 1. Starting RPI with default image

Base instruction: https://forbot.pl/blog/kurs-raspberry-pi-4-instalacja-systemu-rpi-jako-pc-id21047

HowTo connect UART: https://emteria.com/kb/connect-uart-rpi

To do so:
1. Download program Rpi Imager (for Windows)
2. Start the program
3. Insert Rpi boot SD card to the computer
4. Select proper Rpi Image (for RPIv4 RPI OS lite), accept
5. Select the boot SD card as target to burn an image
6. Click on settings
7. In settings, type rpi username and password. On demand you can setup the network and ssh connection settings
8. Accept and go back to main menu
9. Click on burn the image

Enable UART and UART config
1. Insert SD card to PC card reader
2. Select /boot partition, get into
3. Open /boot/config.txt file, add the lines:
```
    enable_uart=1
```
At the end of the file
4. Save and close.
5. Open /boot/console.txt. Here you can see/modify console settings of the uart.

Running the RPI
1. Connect UART to RPI
2. Insert SD card with boot image
3. Power on the RPI, the RPI shall start. UART shall be accesible from the console.


=====================================================================================================================================

## Part 2. Kernel build

Based on instruction from:  https://www.raspberrypi.com/documentation/computers/linux_kernel.html
                            https://www.stephenwagner.com/2020/03/17/how-to-compile-linux-kernel-raspberry-pi-4-raspbian/


Base course dedicated for Raspberry Pi 4 Type B: https://forbot.pl/blog/kurs-raspberry-pi-4-instalacja-systemu-rpi-jako-pc-id21047


Parts of kernel:
-> Kernel image - base image containing source of the kernel
-> Device tree blob - kernel devices configuration description
-> Kernel modules - parts of kernel that can be dynamically loaded/unloaded

! Setting own kernel version

You can set-up your own kernel version by setting env variable before build by:
```
CONFIG_LOCALVERSION="-v7l-MY_CUSTOM_KERNEL"
```

### Part 2.1 - Build kernel image - crosscompile

1. Run container (./configuration/run_container.sh)
2. Go into /repo/linux
3. Set the proper kernel for your device. For RPIv4 it will be kernel71.
```
    KERNEL=kernel71
```
That command will export an env variable, which will be later used by build system.
4. Build config
```
    make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2711_defconfig
```
5. Build base kernel iamge (zImage)
```
    make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- Image
```
6. Build kernel modules (as followed by kernel config)
```
    make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules
```
7. Build device tree blob
```
    make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- dtbs
```
======================================================================================================================================

## Part 3. Install

The base way to install the kernel is to burn it directly on the SD card.

### Part 3.3 Direct burn

According to that way it wil be burnt directly on the SD card, which will be used for boot the system.

After that, on the SD we shall have 2 partitions:
/boot (/sdb1) - That partition contains all about boot process (fat filesystem)
/root (/sdb2) - That partition contains all abut filesystem (ext4 filesystem)

In boot partition we have:
-> kernel image for specified architectures (for pure bare metal only suitable for the platfor image can be left)
-> boot binary
-> config.txt file - with basic boot configuration
-> cmdline.txt - which sets serial console output of the boot process
-> device tree blobs

In root partition we have
-> kenrel modules (/root/lib/modules)
-> root filesystem
-> initial process (started after boot) - named /bin/init


Install instruction for Linux:

1. Put your sd card to computer, mount it
1.1. Check if sd card is accesible.
```
sudo fdisk -l /dev/sdb
```
That command will show sd card partitions and its filesystem. Example output:
```
Device     Boot  Start      End  Sectors  Size Id Type
/dev/sdb1         8192   532479   524288  256M  c W95 FAT32 (LBA)
/dev/sdb2       532480 60506111 59973632 28,6G 83 Linux
```
1.2. Now, we can mount the partitions in a selected place, like:

! Full list of supported filesystems can be found in /proc/filesystems and /lib/modules/$(uname -r)/kernel/fs for a complete list of the filesystems !
```
mkdir -p /home/ap/Projects/RPi4/sd/boot
mkdir -p /home/ap/Projects/RPi4/sd/root
sudo mount -t vfat /dev/sdb1 /home/ap/Projects/RPi4/sd/boot
sudo mount -t ext4 /dev/sdb2 /home/ap/Projects/RPi4/sd/root
```
After that you should be able to see content of the sd card in mounted `boot` and `root` directories.

! If you use docker container, that directories can be mounted to the container as a volume !


-> HowTo access SD card from Virtual Machine: https://scribles.net/accessing-sd-card-from-linux-virtualbox-guest-on-windows-host/

! If you need, you can mount the /boot and /root direcotried into the container. According to that setup it is mounted in `/repo/RPi4/sd`.

2. Copy the artifacts into sd card

2.1. Copy kernel image:
```
env PATH=$PATH make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=/repo/RPi4/sd/root modules_install
```

2.2. Copy boot stuffs
```
sudo cp /repo/RPi4/sd/boot/$KERNEL.img /repo/RPi4/sd/boot/$KERNEL-backup.img
sudo cp arch/arm/boot/zImage /repo/RPi4/sd/boot/$KERNEL.img
sudo cp arch/arm/boot/dts/*.dtb /repo/RPi4/sd/boot/
sudo cp arch/arm/boot/dts/overlays/*.dtb* /repo/RPi4/sd/boot/overlays/
sudo cp arch/arm/boot/dts/overlays/README /repo/RPi4/sd/boot/overlays/
```

3. Check serial settings

Serial settings can be checked in `/boot/config.txt` and `/boot/console.txt`. The fistr file allows to enable serial access by adding the following entry at the end of file:
```
enable_uart=1
```

And the second file allows to modify serial settings (baudrates, parity etc.)

4. Umount the sd card
From VMachine do:
```
sudo umount /home/ap/Projects/RPi4/sd/boot
sudo umount /home/ap/Projects/RPi4/sd/root
```

4. Put the sd card into rpi and start. Observe output on srial console.

### Part 3.2 Burn via usb sd card reader

Burning image via UDB card reader is much simpler. 

1. Put the USB card reader into PC

2. Set "Devices" menu of the running VMachine, set USB card reader. It will be automatically mounted in a guest OS in `/media/ap/boot` for boot partiton and `/media/ap/root` for toor partition.

3. You can store the files directly on the partitions. It can be also easily mounted in the container.

## Part 4. Kernel configuration

Linux kernel i a large and complicated piece of software. The kernel can be configured before compilation to allow user to switch between required features and options, and modify some of them.

To do so, type:

```
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
```

That will run menu for configuration.

All configurations will be written in `.config` file in `/linux` directory. It can be also modified by hand, without menuconfig tool usage.

## Part 5. Device tree

The base device tree for Raspberry Pi 4 B (4GB) is stored in `/linux/arch/arm/boot/dts/bcm2711-rpi-4-b.dts`. The device tree files for both 32 ans 64 bit kenel types are the same.

Overlays are dtb files that overlays the base devicetree settings. It is used to add an extra layers describing hardware that can be connected to the board. Overlay source files are stored in `/boot/overlays1 direcotry.

More about device tree: https://blog.stabel.family/raspberry-pi-4-device-tree/

### Overlays

Device tree overlay is a kernel build-in device tree extension mechanism. The overlay is a .dtb file (copiled .dts), which overlays selected piece of hardware. It can be used to add/modify selected piece of device tree configuration.

The overlays are stored in `/boot/overlays` directory and loaded with .dtb file at boot mode.

To enable overlay we have to modyfi `config.txt` file as follows
```
dtoverlay=my-overlay
```
That entry enables "my-overlay.dtb" file.

Using overlays: https://hackaday.com/2022/04/04/a-power-button-for-raspberry-pi-courtesy-of-device-tree-overlays/
                https://www.cnblogs.com/qiengo/p/5915508.html

#### Building overlays

Way 1: build with whole device tree (cross compile)
1. Put overlay file named like `XXXX-overlay.dts` in an catalogue wih overlays for the device `/linux/arch64/boot/dts/overlays`.
2. Add your overlay in Makefile in above catalogue like:
```
...
	wm8960-soundcard.dtbo \
	my.dtbo

targets += dtbs dtbs_install
...
```
3. Compile whole dtb database, burn it to sd card
4. Enable your overlay in `/boot/config.txt` file.


#### The overlay pattern

Overal pattern of how to write overlay file is described in: https://github.com/raspberrypi/firmware/blob/master/boot/overlays/README



## Part 6. Access threw ssh

To have an access to the RPI threw SSH yu need to attach it to the network.

Instruction: https://stevessmarthomeguide.com/configuring-networking-raspberry-pi/

To check status of the network just type `ifconfig` command.

The `eth0` interface corresponds to ethernet port, and `wlanX` interfaces to wifi.

#### Configure over ethernet

1. Put ethernet cable into ethernet port and connect it to router. The router dhcp will add ip for the rpi and it will be pingable in your lan network.

That way is much easier than configuring wifi.

#### Configure over wifi:

Follow instruction on: https://binaryupdates.com/how-to-configure-wifi-on-raspberry-pi-4/

! It can be also configured during flashing base image by raspberryPi flasher !


#### Configuring ssh on the rpi

https://raspberrypi-guide.github.io/networking/connecting-via-ssh

https://phoenixnap.com/kb/enable-ssh-raspberry-pi

After configuration, you can use ssh services over rpi by:

`ssh -p 22 pi@<IP>`



## Part 7. Kernel modules

The kernel modules are special parts of software that can be understoowd as separetly compiled adiditonal software features working in kernel space. It is stored in `/lib/modules` on the device.

The module itself needs to handle specified pattern nd needs to be compiled as a module. For compile the module we will need also kernel headers for our kernel version.

The module can be build directly on the machine or crosscompiled and burned.

### Direct build

1. Make sure you have kernel headers installed (see /usr/src/linux-headers-<version>). If it is not installed, you cna install t on the machine by:
```
sudo apt install raspberrypi-kernel-headers
```
Please notice, that it will install the headers for latest kernel version.
2. Build the module using make
After build, what is important is `<module_name>.ko` file, which is the module to be loaded to the kernel.
3. Load the module by 
```
sudo insmod <modulename>.ko
```
You can examine if module has been loaded by typing `lsmod` command and check if your module is on he list.

! If you used `printk()` you can examine efects by using `dmesg` command.
4. To remove the module use `sudo rmmod <modbane>` command.