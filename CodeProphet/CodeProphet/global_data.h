#pragma once

//global typedefs.
typedef DWORD64 u64;
typedef INT64 s64;

typedef DWORD32 u32;
typedef INT32 s32;

typedef UINT16 u16;
typedef INT16 s16;

typedef UINT8 u8;
typedef INT8 s8;


//Stuff to include
#include "stack.h"
#include "callhooks.h"
#include "callstack.h"
#include "save_callgraph.h"
#include "timers.h"


//Versions etc
#define MAJOR_VERSION 0
#define MINOR_VERSION 9
#define BUILD_VERSION 3

#define VERSION (MAJOR_VERSION << 24) | (MINOR_VERSION << 16) | (BUILD_VERSION)


#ifdef _M_AMD64
	#define ARCH "PC_X64"
#elif defined (_M_IX86)
	#define ARCH "PC_X86"
#elif defined (_ARM_)
	#define ARCH "WM_ARM"
#else
	#error No CPU selected.
#endif


#define FILE_ID "CPG"

#define FILE_EXT ".cpg"

#define THREAD_MARKER "TMRK"

#ifndef _ARM_
	extern "C" IMAGE_DOS_HEADER __ImageBase;
	#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#else
	#define HINST_THISCOMPONENT ((HINSTANCE)NULL)
#endif

#define MARKER_ADDRESS (&cpg_marker);

#define SEMAPHORE_NAME _T("CODEPROPHET_TIMER")