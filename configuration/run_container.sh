#!/bin/bash

docker run \
	-it \
	--volume `pwd`/..:/repo \
	--rm \
	--name=rpi_container \
	--net=bridge \
	--user=root \
	adrian9254/rpi_image