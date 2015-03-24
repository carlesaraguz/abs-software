#include <fcntl.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define THE_DEVICE "/dev/usb_accessory"

int main(int argc, char const *argv[])
{

	int fd = open(THE_DEVICE, O_RDWR);

	/*
     * Basic I/O
	 *
	 *	0	0	1	0	1	1	0	1 <- read digital pin (57)
	 *	0	0	0	0	0	1	0	1 <- digital pin nº 2  (5)
	 *	0	0	0	0	0	0	1	0 <- end & packet = 1  (2)
	 *
	 */

	char packet1[] = "\x2D\x5\x2";

	char packet2[] = "\x29\x5\x2";

	while(1) {

	write(fd,&packet1,sizeof(packet1));	

	printf("Packet 1 sended\n");

	sleep(1);

	write(fd,&packet2,sizeof(packet2));	

	printf("Packet 2 sended\n");

	sleep(1);

	}

	return 0;
}

