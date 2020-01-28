#pragma once

class TimerQPC : public ITimer
{
	public:

	virtual u32 Init(void)		{ return TRUE; }
	virtual u32 Shutdown(void)	{ return TRUE; }
	virtual u64 GetTime(void)
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return li.QuadPart;
	}
};