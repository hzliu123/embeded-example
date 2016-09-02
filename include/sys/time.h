#ifndef _LINUX_TIME_H
#define _LINUX_TIME_H

struct timeval {
	long	tv_sec;		/* seconds */
	long	tv_usec;	/* microseconds */
};


/*
 * Names of the interval timers, and structure
 * defining a timer setting:
 */
#define	ITIMER_REAL		0
#define	ITIMER_VIRTUAL		1
#define	ITIMER_PROF		2

struct itimerval {
	struct timeval it_interval;	/* timer interval */
	struct timeval it_value;	/* current value */
};

#endif
