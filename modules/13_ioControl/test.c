#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "ioctrl_test.h"

//How to test:
/*
	1. Build test in user space of target machine
	2. Insmod of the tested kernel module
	3. Create device file with given in the kernel module major id (here it is 64)
			sudo mknod /dev/dummy c 64 0
	4. Run test
			sudo ./test
*/

int main() {
	int dev = open("/dev/dummy", O_WRONLY);

	int variable;
	struct mystruct test = {4, "SomeName"};

	if (dev == -1) {
		printf("Opening was not possible! \n");
		return -1;
	}


	printf("Opening was successful!\n");
	ioctl(dev, READ_VALUE, &variable);
	printf(" TEST: variable is: %d", variable);

	variable  = 121;
	ioctl(dev, WRITE_VALUE, &variable);
	printf(" TEST: variable is: %d", variable);

	ioctl(dev, GREETER, &test);

	close(dev);
	return 0;
}
