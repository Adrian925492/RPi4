#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>

#define SIGTX 44
#define REGISTER_UAPP _IO('R', 'g')     //IOCTL api

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
    In that example we have a button with an interrupt handled in kernel side. When the button is pressed, an interrupt occurs. Then, the irq handler
    sends linux kernel signal to the user app.

    To do that, the user app process is registered into lk module, so the module code know the detail of the app process and can send a singal to it.

    The process, in that case test app, receives the signal which indicated button pressing.
*/

void signalhandler(int sig) {
	printf("Button was pressed!\n");
}

int main() {
	int fd;
	signal(SIGTX, signalhandler);

	printf("PID: %d\n", getpid());

	/* Open the device file */
	fd = open("/dev/irq_signal", O_RDONLY);
	if(fd < 0) {
		perror("Could not open device file");
		return -1;
	}

	/* Register app to KM */
	if(ioctl(fd, REGISTER_UAPP, NULL)) {
		perror("Error registering app");
		close(fd);
		return -1;
	}


	/* Wait for Signal */
	printf("Wait for signal...\n");
	while(1)
		sleep(1);

	return 0;
}