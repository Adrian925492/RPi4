#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

docker run \
	-it \
	--volume $SCRIPT_DIR/..:/repo \
	--volume /media/ap/rootfs:/repo/sd_usb/root \
	--volume /media/ap/boot:/repo/sd_usb/boot \
	--rm \
	--name=rpi_container \
	--net=bridge \
	--user=root \
	adrian9254/rpi_image