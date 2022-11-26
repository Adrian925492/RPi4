#!/bin/bash

# FOR USE IN DOCKER CONTAINER ONLY !

cd /repo/linux
KERNEL=kernel8
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- dtbs