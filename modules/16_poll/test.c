#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <poll.h>
#include <string.h>

//How to test:
/*
	1. Build test in user space of target machine
	2. Insmod of the tested kernel module
	3. Create device file with given in the kernel module major id (here it is 64)
			sudo mknod /dev/dummy c 64 0
	4. Run test
			sudo ./test
*/

//What it does
/*
    In that example we have a button with an interrupt handled in kernel side. When the button is pressed, an interrupt occurs. The information about pressing
    the button is propagated to the user space app by polling - a special LK mechanism that allows to hang a user space app on some event (in that case named POLLIN).

    The polling can hang forever or untill some timeout will be finished.
*/

int main() {
	int fd;
    int test;

    struct pollfd my_poll;

	/* Open the device file */
	fd = open("/dev/irq_poll", O_RDONLY);
	if(fd < 0) {
		perror("Could not open device file");
		return -1;
	}

    memset(&my_poll, 0, sizeof(my_poll));
    my_poll.fd = fd;
    my_poll.events = POLLIN;

    /* Wait for the signal */
    printf("Wait for the signal...\n");
    poll(&my_poll, 1, -1);      //poll function, on my_poll struct, only one event (1), -1  wait forever).
    printf("Button pressed!\n");

	return 0;
}