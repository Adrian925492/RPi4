# That makefile allows to build kernel module on the rpi itself


obj-m += rtc_i2c.o

all: module dt
	echo ### Build device tree overlay and kernel module

module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

dt: testoverlay.dts
	dtc -@ -I dts -O dtb -o testoverlay.dtbo testoverlay.dts

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
