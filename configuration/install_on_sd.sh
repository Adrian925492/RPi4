#!/bin/bash

# FOR USE IN DOCKER CONTAINER ONLY !

if [ -d "/repo/sd_usb/boot" ]; then
    BOOT_DIR=/repo/sd_usb/boot
else
    BOOT_DIR=/repo/sd/boot
fi

if [ -d "/repo/sd_usb/root" ]; then
    ROOT_DIR=/repo/sd_usb/root
else
    ROOT_DIR=/repo/sd/root
fi

echo "Root FS files will be stored in $ROOT_DIR"
echo "Boot files will be stored in $BOOT_DIR"

cd /repo/linux
KERNEL=kernel8

# Install modules onto root fs
env PATH=$PATH make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- INSTALL_MOD_PATH=$ROOT_DIR modules_install
echo "######## [!] Kernel modules stored in $ROOT_DIR"

# Store old kernel image and copy a new one
cp $BOOT_DIR/$KERNEL.img $BOOT_DIR/$KERNEL-backup.img
echo "######## [!] Old kernel image stored in $BOOT_DIR as $KERNEL-backup.img"

cp arch/arm64/boot/Image $BOOT_DIR/$KERNEL.img
echo "######## [!] New kernel image stored in $BOOT_DIR as $KERNEL.img"

# Copy device tree blobs
cp arch/arm64/boot/dts/broadcom/*.dtb $BOOT_DIR
echo "######## [!] DTB files stored in $BOOT_DIR"

mkdir -p $BOOT_DIR/overlays
cp -r arch/arm64/boot/dts/overlays/*.dtb* $BOOT_DIR/overlays/
cp -r arch/arm64/boot/dts/overlays/README $BOOT_DIR/overlays/
echo "######## [!] Overlays stored in $BOOT_DIR/overlays"

echo "######## [!] Kernel update done."