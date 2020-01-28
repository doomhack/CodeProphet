#pragma once

#ifdef _M_IX86
	#define CALLHOOK_FUNC extern "C" void __declspec(naked) __cdecl
#endif

typedef volatile int Mutex;

void __fastcall OnFunctionEnter(void* func_addr, void* frame_ptr);
void __fastcall OnFunctionExit(void* frame_ptr);

void CSEnter(Mutex& m);
void CSExit(Mutex& m);

void OnExit(void);
