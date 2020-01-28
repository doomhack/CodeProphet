#include <stdio.h>
#include <assert.h>
#include <Windows.h>

#include "global_data.h"

/*
This is the call stack index for the current thread.
It is stored thread-locally so that each thread gets its own.
*/
DWORD dwTlsIndex = 0;

/*
The list of all of the thread stacks.
*/
CStack<CCallStack*> *g_stacklist = NULL;

/*
Mutex so that g_stacklist and dwTlsIndex cannot be accessed by two threads at a time.
*/
Mutex stacklocked = 0;
Mutex tlslocked = 0;

/*
*/
CSaveCallgraph* savecallgraph = NULL;


/*
*/
u32 profiling_enabled = TRUE;

#ifdef _M_IX86 //For x86 we use /Gh /GH compiler options...

#pragma optimize("", off)

CALLHOOK_FUNC _penter( void )
{
	__asm
	{
		push	ebp			//1 tick
		mov		ebp, esp	//1 tick
		pushad				//8 ticks
	}
	
	__asm
	{
		mov edx, [ebp]
		mov ecx, [ebp+4]
		sub ecx, 5
		call   OnFunctionEnter //__fastcall so no push/pop params
	}

	__asm
	{
		popad
		mov   esp, ebp
		pop   ebp
		ret
	}
}

CALLHOOK_FUNC _pexit( void )
{
	__asm
	{
		push   ebp
		mov   ebp, esp
		pushad
	}

	__asm
	{
		mov ecx, [ebp]
		call   OnFunctionExit //__fastcall so no push/pop params
	}

	__asm
	{
		popad
		mov   esp, ebp
		pop   ebp
		ret
	}
}

#pragma optimize("", on)

#else //for ARM we use /callcap or /fastcap.
#ifndef _M_AMD64

extern "C" void _CAP_Enter_Function(void *p) 
{
	OnFunctionEnter(p, p);
}

extern "C" void _CAP_Exit_Function(void *p) 
{
	OnFunctionExit(p);
}

//Or /fastcap...
extern "C"  void _CAP_Start_Profiling(void* this_func, void *p)
{
   OnFunctionEnter(p, this_func);
}

extern "C" void _CAP_End_Profiling(void *p)
{
   OnFunctionExit(p);
}

#endif //_M_AMD64
#endif //_M_IX86

void __fastcall OnFunctionEnter(void* func_addr, void* frame_ptr)
{
	if(!profiling_enabled)
		return;

	CSEnter(tlslocked);
	
	if(!dwTlsIndex)
		dwTlsIndex = TlsAlloc();

	CSExit(tlslocked);

	CCallStack *g_callstack = static_cast<CCallStack*> (TlsGetValue(dwTlsIndex));

	// There is no callstack for this thread yet.
	if(!g_callstack)
	{
		CSEnter(stacklocked);

		// There is no stacklist. This is our first run.
		if(!g_stacklist)
		{
			//Clean up when process ends.
			atexit(OnExit);

			//Add a new list of stacks.
			g_stacklist = new CStack<CCallStack*>;
			CTimer::InitTimer();

			savecallgraph = new CSaveCallgraph;
		}

		g_callstack = new CCallStack();

		TlsSetValue(dwTlsIndex, g_callstack);

		//Add this threads callstack to the global stacklist.
		g_stacklist->Push(g_callstack);

		CSExit(stacklocked);
	}

	u64 timestamp;

	//Get the time.
	CTimer::GetTimerTime(timestamp);

	//Dispatch the call onto the callstack.
	g_callstack->AddCall(func_addr, frame_ptr, timestamp);
}

void __fastcall OnFunctionExit(void* frame_ptr)
{
	if(!profiling_enabled)
		return;

	u64 timestamp;
	
	//Get the time.
	CTimer::GetTimerTime(timestamp);

	CCallStack *g_callstack = static_cast<CCallStack*> (TlsGetValue(dwTlsIndex));

#ifdef DEBUG
	//Cannot continue if callstack is not allocd.
	assert(g_callstack != NULL);
#endif

	//Dispatch call onto the callstack.
	g_callstack->ReturnCall(frame_ptr, timestamp);
}

//Primative spinlock

void CSEnter(Mutex& m)
{
	while(m > 0){}

	m++;
}

void CSExit(Mutex& m)
{
	m--;
}

void OnExit(void)
{
	/*
	When we exit the program, we need to:
	1) Save the current data.
	2) Free the save stuff.
	3) For each thread:
		- Free each callstack.
		- Free the stack list.
	*/

	TlsFree(dwTlsIndex);
	SaveAllCallTrees(NULL);
	CleanUpSave();
	CTimer::ShutdownTimer();

	u32 num_threads = g_stacklist->Size();

	while(num_threads--)
		delete g_stacklist->At(num_threads);

	delete g_stacklist;
}

/*
Profiler control functions.
*/

DWORD StartProfiling(void)
{
	profiling_enabled = TRUE;

	return TRUE;
}


DWORD StopProfiling(void)
{
	profiling_enabled = FALSE;

	return TRUE;
}


DWORD IsProfilerRunning(void)
{
	return profiling_enabled;
}

DWORD SuspendProfiling(void)
{
	CCallStack *g_callstack = static_cast<CCallStack*> (TlsGetValue(dwTlsIndex));

	if(!g_callstack)
		return FALSE;

	g_callstack->SuspendProfiling();

	return TRUE;
}

DWORD ResumeProfiling(void)
{
	CCallStack *g_callstack = static_cast<CCallStack*> (TlsGetValue(dwTlsIndex));

	if(!g_callstack)
		return FALSE;

	g_callstack->ResumeProfiling();

	return TRUE;
}

DWORD SaveCallGraphData(void)
{
	StopProfiling();
	OnExit();

	return TRUE;
}

DWORD SetThreadName(TCHAR* thread_name)
{
	CCallStack *g_callstack = static_cast<CCallStack*> (TlsGetValue(dwTlsIndex));

	if(!g_callstack)
		return FALSE;

	g_callstack->SetThreadName(thread_name);

	return TRUE;
}