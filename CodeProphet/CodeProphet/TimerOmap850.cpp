#include <stdlib.h>
#include <Windows.h>
#include "global_data.h"
#include "ITimer.h"
#include "TimerOmap850.h"

TimerOmap850::TimerOmap850(void)
{
	last_value = 0xFFFFFFFF;
	wrap_count = 0;
	timer3 = NULL;
	semaphore = NULL;
}

TimerOmap850::~TimerOmap850(void)
{
}

u32 TimerOmap850::Init(void)
{
	//We need to map the Timer register into our address space.
	HMODULE hDdkDll = (HMODULE) LoadLibrary(_T("CEDDK.DLL")); 
	tMmMapIoSpace _MmMapIoSpace = (tMmMapIoSpace) GetProcAddress(hDdkDll,_T("MmMapIoSpace"));

	if(!_MmMapIoSpace)
		return FALSE;

	PHYSICAL_ADDRESS pa;

	pa.QuadPart = TIMER3;
	timer3 = (u32*)_MmMapIoSpace(pa, 12, 0);

	FreeLibrary(hDdkDll);
	
	if(!timer3)
		return FALSE;

	semaphore = CreateSemaphore(NULL, 31, 32, SEMAPHORE_NAME);

	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		WaitForSingleObject(semaphore, 0);
		return TRUE;
	}

	//Now we set up TIMER3.
	timer3[LOAD_TIM]	=	0xFFFFFFFF; //Set timer to max count. (It counts down to 0!)
	timer3[READ_TIM]	=	0xFFFFFFFF; //Set the reset value for when it hits 0.
	timer3[CNTL_TIMER]	=	0xFFFFFFA3; //Set the control reg. Div = 0 (6.5Mhz). Set running = 1.

	return TRUE;
}

u32 TimerOmap850::Shutdown(void)
{
	LONG prev_count = 0;

	ReleaseSemaphore(semaphore, 1, &prev_count);

	if(prev_count >= 31)
	{
		//Shutdown TIMER3.
		timer3[LOAD_TIM]	=	0xFFFFFFFF;
		timer3[READ_TIM]	=	0xFFFFFFFF;
		timer3[CNTL_TIMER]	=	0xFFFFFF80; //Turn timer back off.
		
		CloseHandle(semaphore);
		semaphore = NULL;
	}

	HMODULE hDdkDll = (HMODULE) LoadLibrary(_T("CEDDK.DLL")); 
	tMmUnmapIoSpace _MmUnmapIoSpace = (tMmUnmapIoSpace) GetProcAddress(hDdkDll,_T("MmUnmapIoSpace"));

	if(!_MmUnmapIoSpace)
		return FALSE;

	_MmUnmapIoSpace(timer3, 12);

	FreeLibrary(hDdkDll);

	return TRUE;
}

u64 TimerOmap850::GetTime(void)
{
	u32 t = timer3[READ_TIM];

	if(t > last_value)
		wrap_count++;

	last_value = t;

	return (u64)(0xFFFFFFFF - t | ((u64)wrap_count << 32));
}