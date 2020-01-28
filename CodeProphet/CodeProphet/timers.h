#pragma once

//Timer types
#define TIMER_QPC	1 //QueryPerformance counter. Hi-res, slow, innaccurate.
#define TIMER_RDTSC	2 //CPU Specific RDTSC instruction. Very Hi-res, fast, sometimes innaccurate.
#define TIMER_GTC	3 //GetTickCount. Low res (~55ms), very fast, very accurate.
#define TIMER_NONE	4 //No timer. To be used for performance comparison.

#define MIN_RDTSC_FREQ_TICS 1000

//#define TIMER_TYPE TIMER_QPC
#define TIMER_TYPE TIMER_RDTSC
//#define TIMER_TYPE TIMER_GTC
//#define TIMER_TYPE TIMER_NONE

#if TIMER_TYPE==TIMER_RDTSC

	#ifdef _ARM_

		//Not defined in any of the Windows Mobile Headers.
		extern "C" BOOL SetKMode(BOOL fMode);

		#include "access_arm_cop.h"
		#include "ITimer.h"
		#include "TimerOmap850.h"
		#include "TimerQPC.h"
		#include "TimerCCNT.h"

		//TI OMAP
		#define OMAP850	0x41069263
		
		//VS2008 Emu
		#define ARM_EMU	0x69059201

		typedef enum ETimer
		{
			omap850,	//Use OMAP850 timer code
			xscale,		//Use XScale timer code
			arm_emu,	//Use Device Emulator code
			use_qpc		//Use QueryPerformanceCounter.
		} ETimer;

	#endif //_ARM_

#endif //TIMER_TYPE==TIMER_RDTSC


class CTimer
{
	private:

#if TIMER_TYPE==TIMER_RDTSC

	#ifdef _ARM_
		static ITimer* timer;
	#endif

	static u64 begin_tsc;
	static u32 begin_gtc;

#endif //TIMER_TYPE==TIMER_RDTSC

#ifdef _ARM_

	#if TIMER_TYPE==TIMER_RDTSC

		static ETimer GetCPUType()
		{
			//Intel PMU is our best bet. Use it if available.
			if( IsProcessorFeaturePresent(PF_ARM_INTEL_PMU) )
				return xscale;

			//Must be in kernel mode to get cpuid.
			BOOL oldkmode = SetKMode(TRUE);

			//Get the CPUID.
			const u32 cpuid = ReadCOPReg(REG_CPUID);

			//Return to whatever mode we were in before.
			SetKMode(oldkmode);

			switch(cpuid)
			{
				case OMAP850:
					return omap850;

				case ARM_EMU:
					return arm_emu;

				default:
					return use_qpc;
			}
		}

	#endif //TIMER_TYPE==TIMER_RDTSC

#endif //_ARM_

	public:

	static void InitTimer()
	{
#ifdef _ARM_

	#if TIMER_TYPE==TIMER_RDTSC

		ETimer codepath = CTimer::GetCPUType();

		switch(codepath)
		{
			case omap850:
				timer = new TimerOmap850();
				break;

			case arm_emu:
			case use_qpc:
				timer = new TimerQPC();
				break;

			case xscale:
				timer = new TimerCCNT();
				break;

			default:
				timer = new TimerQPC();
		}

		if(!timer->Init())
		{
			delete timer;
			timer = new TimerQPC();
			timer->Init();
		}

	#endif //TIMER_TYPE==TIMER_RDTSC

#endif //_ARM_
	}


	static void ShutdownTimer()
	{
#ifdef _ARM_

	#if TIMER_TYPE==TIMER_RDTSC

		timer->Shutdown();
		delete timer;
		timer = NULL;

	#endif //TIMER_TYPE==TIMER_RDTSC

#endif //_ARM_
	}

	static void GetTimerTime(u64& time)
	{
#if TIMER_TYPE==TIMER_QPC
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		time = li.QuadPart;
#elif TIMER_TYPE==TIMER_GTC
		time = GetTickCount();
#elif TIMER_TYPE==TIMER_RDTSC
	#ifdef _ARM_
		time = timer->GetTime();
	#else
		time = __rdtsc();
	#endif

		if(begin_gtc)
			return;

		begin_gtc = GetTickCount();
		begin_tsc = time;

#elif TIMER_TYPE==TIMER_NONE
		time = 0;
#else
		#error No timer specified.
#endif
	}

	static void GetTimerFreq(u64& freq)
	{
#if TIMER_TYPE==TIMER_QPC
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		freq = li.QuadPart;
#elif TIMER_TYPE==TIMER_GTC
		freq = 1000;
#elif TIMER_TYPE==TIMER_RDTSC
	
		u32 end_gtc = GetTickCount();

		if((end_gtc - begin_gtc) < MIN_RDTSC_FREQ_TICS)
			Sleep( MIN_RDTSC_FREQ_TICS - (end_gtc - begin_gtc) );
	
	#ifdef _ARM_
		u64 end_tsc = timer->GetTime();
	#else
		u64 end_tsc = __rdtsc();
	#endif

		end_gtc = GetTickCount();
		freq = ((end_tsc - begin_tsc) / (end_gtc - begin_gtc)) * 1000;

#elif TIMER_TYPE==TIMER_NONE
		freq = 0;
#else
		#error No timer specified.
#endif
	}
};