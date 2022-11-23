#!/bin/bash

# FOR USE IN DOCKER CONTAINER ONLY !

cd /repo/RPi4/linux
KERNEL=kernel7l
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules