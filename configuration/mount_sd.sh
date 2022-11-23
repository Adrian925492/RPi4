#!/bin/bash

# FOR USE OUTSIDE DOCKER !

ROOT_DIR=/home/ap/Projects/RPi4/sd/boot
BOOT_DIR=/home/ap/Projects/RPi4/sd/root

mkdir -p $ROOT_DIR
mkdir -p $BOOT_DIR
sudo mount -t vfat /dev/sdb1 $BOOT_DIR
sudo mount -t ext4 /dev/sdb2 $ROOT_DIR