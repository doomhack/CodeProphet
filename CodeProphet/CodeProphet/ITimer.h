#pragma once

class ITimer
{
	public:
	virtual u32 Init(void) = 0;
	virtual u32 Shutdown(void) = 0;
	virtual u64 GetTime(void) = 0;
};