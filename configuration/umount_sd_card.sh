#!/bin/bash

# FOR USE OUTSIDE DOCKER !

ROOT_DIR=/home/ap/Projects/RPi4/sd/boot
BOOT_DIR=/home/ap/Projects/RPi4/sd/root

sudo umount $BOOT_DIR
sudo umount $ROOT_DIR