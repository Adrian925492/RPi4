#!/bin/bash

# FOR USE IN DOCKER CONTAINER ONLY !

ROOT_DIR=/repo/RPi4/sd/root
BOOT_DIR=/repo/RPi4/sd/boot

cd /repo/RPi4/linux
KERNEL=kernel7l

# Install modules onto root fs
env PATH=$PATH make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=$ROOT_DIR modules_install

# Store old kernel image and copy a new one
cp $BOOT_DIR/$KERNEL.img $BOOT_DIR/$KERNEL-backup.img
cp arch/arm/boot/zImage $BOOT_DIR/$KERNEL.img

# Copy device tree blobs
cp arch/arm/boot/dts/*.dtb $BOOT_DIR
cp arch/arm/boot/dts/overlays/*.dtb* $BOOT_DIR/overlays/
cp arch/arm/boot/dts/overlays/README $BOOT_DIR/overlays/