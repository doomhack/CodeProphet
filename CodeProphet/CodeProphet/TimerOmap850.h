#pragma once

typedef LARGE_INTEGER PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS; 

typedef PVOID (WINAPI *tMmMapIoSpace) ( PHYSICAL_ADDRESS PhysicalAddress, ULONG NumberOfBytes, BOOLEAN CacheEnable ); 
typedef VOID (WINAPI *tMmUnmapIoSpace) (PVOID BaseAddress,ULONG NumberOfBytes);

#define TIMER1 0xFFFEC500
#define TIMER2 0xFFFEC600
#define TIMER3 0xFFFEC700

#define CNTL_TIMER	0
#define LOAD_TIM	1
#define READ_TIM	2


class TimerOmap850 : public ITimer
{
	public:
	TimerOmap850(void);
	~TimerOmap850(void);

	virtual u32 Init(void);
	virtual u32 Shutdown(void);
	virtual u64 GetTime(void);

	private:
	volatile u32 last_value;
	volatile u32 wrap_count;

	HANDLE semaphore;
	u32 *timer3;

};