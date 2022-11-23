#!/bin/bash

# FOR USE IN DOCKER CONTAINER ONLY !

cd /repo/RPi4/linux
KERNEL=kernel7
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2709_defconfig