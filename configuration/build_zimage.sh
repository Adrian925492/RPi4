#!/bin/bash

# FOR USE IN DOCKER CONTAINER ONLY !

cd /repo/linux
KERNEL=kernel8
make -j 8 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- Image