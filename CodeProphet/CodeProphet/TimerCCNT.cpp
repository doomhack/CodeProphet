#include <stdlib.h>
#include <Windows.h>
#include "global_data.h"
#include "ITimer.h"
#include "TimerCCNT.h"


TimerCCNT::TimerCCNT(void)
{
	wrap_count = 0;
	semaphore = NULL;
}

TimerCCNT::~TimerCCNT(void)
{
}

u32 TimerCCNT::Init(void)
{
	semaphore = CreateSemaphore(NULL, 32, 32, SEMAPHORE_NAME);

	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		WaitForSingleObject(semaphore, 0);
		return TRUE;
	}

	//Unfortunatly, Entering Kernel mode is quite expensive.
	//so we do it once on enter and again on exit.
	oldkmode = SetKMode(TRUE);

	u32 ctrl = ReadCOPReg(REG_INTEL_PMU_CTRL);

	//Clear reserved bits to 0;
	PMNC_BIT_RESERVED_CLEAR(ctrl);

	//Reset the overflow flag. Write 1 to reset!
	PMNC_BIT_CCNT_OVERFLOW_SET(ctrl);

	//Set divider to 64
	PMNC_BIT_D_SET(ctrl);

	//Timer should reset to 0 on overflow.
	PMNC_BIT_C_SET(ctrl);

	//Enbale counters
	PMNC_BIT_E_SET(ctrl);

	//Write the control register.
	WriteCOPReg(REG_INTEL_PMU_CTRL, ctrl);

	//Finally, reset the counter to 0.
	WriteCOPReg(REG_INTEL_PMU_CCNT, 0);

	return TRUE;
}

u32 TimerCCNT::Shutdown(void)
{
	LONG prev_count;

	ReleaseSemaphore(semaphore, 1, &prev_count);

	if(prev_count < 31)
		return TRUE;

	CloseHandle(semaphore);
	semaphore = NULL;

	u32 ctrl = ReadCOPReg(REG_INTEL_PMU_CTRL);

	//Clear reserved bits to 0;
	PMNC_BIT_RESERVED_CLEAR(ctrl);

	//Reset the overflow flag. Write 1 to reset!
	PMNC_BIT_CCNT_OVERFLOW_SET(ctrl);

	//Set divider to 1;
	PMNC_BIT_D_SET(ctrl);

	//Timer should reset to 0 on overflow.
	PMNC_BIT_C_SET(ctrl);

	//Disable counters
	PMNC_BIT_E_DIS(ctrl);

	//Write the control register.
	WriteCOPReg(REG_INTEL_PMU_CTRL, ctrl);

	//Finally, reset the counter to 0.
	WriteCOPReg(REG_INTEL_PMU_CCNT, 0);

	SetKMode(oldkmode);

	return TRUE;
}

u64 TimerCCNT::GetTime(void)
{
	u32 t = ReadCOPReg(REG_INTEL_PMU_CCNT);
	u32 ctrl = ReadCOPReg(REG_INTEL_PMU_CTRL);

	if(ctrl & PMNC_BIT_CCNT_OVERFLOW)
	{
		wrap_count++;

		//Clear reserved bits to 0;
		PMNC_BIT_RESERVED_CLEAR(ctrl);

		//Reset the overflow flag. Write 1 to reset!
		PMNC_BIT_CCNT_OVERFLOW_SET(ctrl);

		//Write the control register.
		WriteCOPReg(REG_INTEL_PMU_CTRL, ctrl);
	}

	return (u64)(t | ((u64)wrap_count << 32));
}