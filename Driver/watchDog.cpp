/* Linux WatchDog Function */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <linux/watchdog.h>
#include "watchDog.h"

 
#define WATCHDOGDEV "/dev/watchdog"

int LinuxWatchDog::Start()
{
	wtd_FD = open(WATCHDOGDEV, O_WRONLY);
	if (-1 == wtd_FD) {
		fprintf(stderr, "Error: %s\n", strerror(errno));
		return -1;
	}
 
	/* If user wants to change the watchdog interval */
	if (WatchDogInterval > 0) 
	{
		fprintf(stdout, "Set watchdog interval to %d\n", WatchDogInterval);
		if (ioctl(wtd_FD, WDIOC_SETTIMEOUT, &WatchDogInterval) != 0) 
		{
			fprintf(stderr,"Error: Set watchdog interval failed\n");
			return -2;
		}
	}
}

int LinuxWatchDog::KeepAlive()
{
	if (-1 == wtd_FD) {
		fprintf(stderr, "Error: %s\n", strerror(errno));
		return -1;
	}
	//write(wtd_FD, "a", 1);
	
	//ioctl API
	ioctl(wtd_FD, WDIOC_KEEPALIVE, 0);
	//sleep(10);
}

int LinuxWatchDog::Stop()
{
	if (-1 == wtd_FD) {
		fprintf(stderr, "Error: %s\n", strerror(errno));
		return -1;
	}
	write(wtd_FD, "V", 1);
	close(wtd_FD);
}
