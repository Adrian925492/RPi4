#Raspberry Pi4 linux kernel toolchain build and driver development

## Part 1. Kernel build

Based on instruction from: https://www.raspberrypi.com/documentation/computers/linux_kernel.html

Base course dedicated for Raspberry Pi 4 Type B: https://forbot.pl/blog/kurs-raspberry-pi-4-instalacja-systemu-rpi-jako-pc-id21047


Parts of kernel:
-> Kernel image - base image containing source of the kernel
-> Device tree blob - kernel devices configuration description
-> Kernel modules - parts of kernel that can be dynamically loaded/unloaded