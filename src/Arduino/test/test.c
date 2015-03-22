#include <fcntl.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define THE_DEVICE "/dev/usb_accessory"

int main(int argc, char const *argv[])
{

	int fd = open(THE_DEVICE, O_RDWR);

	/*
     * Basic I/O
	 *
	 *	0	0	1	1	1	0	0	1 <- read digital pin (57)
	 *	0	0	0	0	0	1	0	1 <- digital pin nº 2  (5)
	 *	0	0	0	0	0	0	1	0 <- end & packet = 1  (2)
	 *
	 */

	char packet[] = "\x39\x5\x2";
	write(fd,&packet,sizeof(packet));	

	printf("Packet sended\n");

	return 0;
}


