#!/bin/bash

# FOR USE IN DOCKER CONTAINER ONLY !

if [ -d "/repo/sd_usb/boot" ]; then
    BOOT_DIR=/repo/sd_usb/boot
else
    BOOT_DIR=/repo/sd/boot
fi

echo "Boot files will be stored in $BOOT_DIR"

cd /repo/linux
KERNEL=kernel8

# Copy device tree blobs
cp arch/arm64/boot/dts/broadcom/*.dtb $BOOT_DIR
echo "######## [!] DTB files stored in $BOOT_DIR"

mkdir -p $BOOT_DIR/overlays
cp -r arch/arm64/boot/dts/overlays/*.dtb* $BOOT_DIR/overlays/
cp -r arch/arm64/boot/dts/overlays/README $BOOT_DIR/overlays/
echo "######## [!] Overlays stored in $BOOT_DIR/overlays"

echo "######## [!] Kernel update done."