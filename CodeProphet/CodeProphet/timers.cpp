#include <stdlib.h>
#include <Windows.h>
#include "global_data.h"
#include "timers.h"

#if TIMER_TYPE==TIMER_RDTSC

	#ifdef _ARM_
		ITimer* CTimer::timer=NULL;
	#endif

	u64 CTimer::begin_tsc = 0;
	u32 CTimer::begin_gtc = 0;
#endif