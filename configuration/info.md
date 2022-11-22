#Raspberry Pi4 linux kernel toolchain build and driver development


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
    make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- Image
```
6. Build kernel modules (as followed by kernel config)
```
    make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules
```
7. Build device tree blob
```
    make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- dtbs
```
======================================================================================================================================

## Part 3. Install

The base way to install the kernel is to burn it directly on the SD card.

### Part 3.3 Direct burn

According to that way it wil be burnt directly on the SD card, which will be used for boot the system.

After that, on the SD we shall have 2 partitions:
/boot (/sdb1) - That partition contains all about boot process (fat filesystem)
/root (/sdb2) - That partition contains all abut filesystem (ext4 filesystem)

Install instruction for Linux:
1. Put your sd card to computer, mount it
2. Use `lsblk` program to play with SD card

    -> HowTo access SD card from Virtual Machine: https://scribles.net/accessing-sd-card-from-linux-virtualbox-guest-on-windows-host/
    
