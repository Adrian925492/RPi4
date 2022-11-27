#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
	int dev = open("/dev/my_device_nr", O_RDONLY);
	if (dev == -1) {
		printf("Opening was nit possible! \n");
		return -1;
	}
	printf("Opening was successful!\n");
	close(dev);
	return 0;
}
