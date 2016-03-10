#ifndef _____WATCHDOG_____
#define _____WATCHDOG_____




class LinuxWatchDog
{
public:
	LinuxWatchDog(void);
	~LinuxWatchDog(void){Stop();};
	LinuxWatchDog(int interval){ WatchDogInterval = interval;};
	int Start(void);
	int KeepAlive(void);
	int Stop(void);
private:
	int wtd_FD;
	int WatchDogInterval;
};

#endif