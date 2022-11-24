#!/bin/bash

# FOR USE IN DOCKER CONTAINER ONLY !

cd /repo/RPi4/linux
KERNEL=kernel7l
make -j 8 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- Image