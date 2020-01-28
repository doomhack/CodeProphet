#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <tlhelp32.h> 

#define DBGHELP_TRANSLATE_TCHAR
#define _NO_CVCONST_H

#include "CodeProphetSampleHost.h"
#include "SampleHost.h"
#include "SaveFile.h"

#ifndef _ARM_
	#include <Dbghelp.h>
#else
	#include <Psapi.h>
	extern "C" ULONG GetThreadCallStack(HANDLE hThrd, ULONG dwMaxFrames, CallSnapshot lpFrames[], DWORD dwFlags, DWORD dwSkip);
#endif

CSampleCallGraph::CSampleCallGraph(HANDLE process, DWORD process_id)
{
	this->hprocess = process;
	this->process_id = process_id;

#ifndef _ARM_
	//Start the symbol server.
	SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES | SYMOPT_DEBUG | SYMOPT_UNDNAME);

	//If syminit fails first time, retry a few times.
	int trys = 0;
	while(trys < 32)
	{
		if(SymInitialize(process, NULL, TRUE))
			break;

		trys++;
		Sleep(100);
	}

	if(trys >= 32) //Failed to init debughelp.
		printf("Failed to initialise debughlp.\n");
#else
	PROCESSENTRY32 pse32 = {0};
	HANDLE snap;
	
	snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | 0x40000000, this->process_id);

	pse32.dwSize = sizeof(PROCESSENTRY32);

	if(Process32First(snap, &pse32))
	{
		do
		{
			if(pse32.th32ProcessID == process_id)
			{
				Module* exe = new Module;
				MODULEINFO mi = {0};

				GetModuleInformation(hprocess, (HMODULE)pse32.th32ModuleID, &mi, sizeof(MODULEINFO));

				exe->baseaddr = pse32.th32MemoryBase;
				exe->len = mi.SizeOfImage;
				exe->name = new TCHAR[wcslen(pse32.szExeFile)+1];
				wcscpy(exe->name, pse32.szExeFile);

				modules[NULL] = exe;

				break;
			}
		} while (Process32Next(snap, &pse32));
	}

	CloseToolhelp32Snapshot(snap);

#endif

}

CSampleCallGraph::~CSampleCallGraph()
{
#ifndef _ARM_
	SymCleanup(this->hprocess);
#endif
	//Find module.
	std::unordered_map<HANDLE, Module*>::iterator i_m;

	for(i_m = modules.begin();  i_m != modules.end(); i_m++)
	{
		Module* this_mod = i_m->second;
		
		delete this_mod->name;
		delete this_mod;
	}

	this->modules.clear();


	//Find module.
	std::unordered_map<DWORD, SampleHit*>::iterator i_s;

	for(i_s = samples.begin();  i_s != samples.end(); i_s++)
	{
		SampleHit* this_sample = i_s->second;
		
		if(this_sample->function)
		{
			delete this_sample->function;
			this_sample->function = NULL;
		}
		
		if(this_sample->source_file)
		{
			delete this_sample->source_file;
			this_sample->source_file = NULL;
		}

		delete this_sample;
	}

	this->samples.clear();

	std::unordered_map<DWORD, CCallTree*>::iterator i_c;

	for(i_c = callgraphs.begin();  i_c != callgraphs.end(); i_c++)
		delete i_c->second;

	this->callgraphs.clear();


	std::unordered_map<DWORD, Thread*>::iterator i_t;

	for(i_t = threads.begin();  i_t != threads.end(); i_t++)
	{
		Thread* this_thread = i_t->second;

		CloseHandle(this_thread->hthread);
		delete this_thread;
	}

	this->threads.clear();

	CloseHandle(this->hprocess);
}

u32 CSampleCallGraph::StartSampling()
{
	/*
	We're live now!

	1) Enumerate Modules.
	2) Enumerate Threads.
	3) For each thread:
		-Sample callstack
	*/

	HANDLE snap = NULL;
	u32 quit = FALSE;
	u32 thopt = TH32CS_SNAPMODULE | TH32CS_SNAPTHREAD;

#ifdef _ARM_
	thopt |= 0x40000000;
#endif

	while( WaitForSingleObject(this->hprocess, 0) == WAIT_TIMEOUT )
	{
		snap = CreateToolhelp32Snapshot(thopt, this->process_id);
		
		if(snap == INVALID_HANDLE_VALUE)
			continue;

		EnumerateModules(snap);

		EnumerateThreads(snap);

#ifndef _ARM_
		CloseHandle( snap );     // Must clean up the snapshot object!
#else
		CloseToolhelp32Snapshot(snap);
#endif
		snap = NULL;

		Sleep(1);
	}
	
	CSaveFile* save_file = new CSaveFile();

	if(!save_file)
		return FALSE;

	std::unordered_map<DWORD, CCallTree*>::iterator i;

	for(i = callgraphs.begin();  i != callgraphs.end(); i++)
		save_file->AddCallTree(i->second);
	
	save_file->SaveData(&modules, &samples);

	delete save_file;

	return TRUE;
}

u32 CSampleCallGraph::EnumerateModules(HANDLE snap)
{
	MODULEENTRY32 me32;
	u32 mods = 0;

	me32.dwSize = sizeof( MODULEENTRY32 );
	
	if( !Module32First( snap, &me32 ) ) 
		return 0;
	else
	{
		do
		{	
			mods++;

			if(!this->modules[me32.hModule])
			{
				Module* curr_mod = new Module;

				curr_mod->baseaddr = (DWORD_PTR)me32.modBaseAddr;
				curr_mod->len = me32.modBaseSize;

				curr_mod->name = new TCHAR[wcslen(me32.szModule)+1];
				wcscpy(curr_mod->name, me32.szModule);

				this->modules[me32.hModule] = curr_mod;
			}
		} while ( Module32Next(snap, &me32));
	}

	return mods;
}

u32 CSampleCallGraph::EnumerateThreads(HANDLE snap)
{
	THREADENTRY32 th32;
	u32 threads = 0;

	th32.dwSize = sizeof(THREADENTRY32);

	if( !Thread32First( snap, &th32 ) )
		return 0; //No threads?
	else
	{
		do
		{
			if(th32.th32OwnerProcessID == this->process_id) //One of the threads in our process.
			{	
				threads++;

				Thread* curr_thread = this->threads[th32.th32ThreadID];

				if(!curr_thread)
				{
					curr_thread = new Thread;
#ifndef _ARM_					
					curr_thread->hthread = OpenThread(THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME, FALSE, th32.th32ThreadID);
#else
					curr_thread->hthread = (HANDLE)th32.th32ThreadID;
#endif
					curr_thread->thread_id = th32.th32ThreadID;

					this->threads[th32.th32ThreadID] = curr_thread;
				}

				this->DoStackWalk(curr_thread);
			}
		} while ( Thread32Next(snap, &th32));
	}

	return threads;
}

u32 CSampleCallGraph::DoStackWalk(Thread* thread)
{
	CallSnapshot cs[MAX_STACK_DEPTH + 1] = {0};
	CONTEXT ctx = {0};
#ifndef _ARM_
	STACKFRAME64 stack = {0};
#endif
	u32 level = 0;

	ctx.ContextFlags = CONTEXT_CONTROL;

	if(SuspendThread(thread->hthread) == 0xFFFFFFFF)
	{
		//printf("Suspend Thread Failed: %d\n", GetLastError());
		return FALSE;
	}

	if(!GetThreadContext(thread->hthread, &ctx))
		return FALSE;

#ifdef _M_IX86
	stack.AddrStack.Offset	= ctx.Esp;
	stack.AddrPC.Offset		= ctx.Eip;
	stack.AddrFrame.Offset	= ctx.Ebp;
	stack.AddrStack.Mode	= stack.AddrPC.Mode = stack.AddrFrame.Mode = AddrModeFlat;
	DWORD machine_type		= IMAGE_FILE_MACHINE_I386;
#elif defined _M_AMD64
	stack.AddrStack.Offset	= ctx.Rsp;
	stack.AddrPC.Offset		= ctx.Rip;
	stack.AddrFrame.Offset	= ctx.Rbp;
	stack.AddrStack.Mode	= stack.AddrPC.Mode = stack.AddrFrame.Mode = AddrModeFlat;
	DWORD machine_type		= IMAGE_FILE_MACHINE_AMD64;
#endif

#ifndef _ARM_
	while(1)
	{
		if(!::StackWalk64(machine_type, this->hprocess, thread->hthread, &stack, &ctx, NULL, &SymFunctionTableAccess64, &SymGetModuleBase64, NULL))
			break;

		if(!stack.AddrPC.Offset || level > MAX_STACK_DEPTH)
			break;

		cs[level++].dwReturnAddr = (DWORD_PTR)stack.AddrPC.Offset;
	}
#else
	level = GetThreadCallStack(thread->hthread, MAX_STACK_DEPTH-1, &cs[1], 0, 0);

	cs[0].dwReturnAddr = ctx.Pc;
#endif

	ResumeThread(thread->hthread);

	CCallTree* curr_tree = this->callgraphs[thread->thread_id];

	if (!curr_tree)
	{
		curr_tree = new CCallTree(this->hprocess, thread, &modules, &samples);
		this->callgraphs[thread->thread_id] = curr_tree;
	}

	curr_tree->AddSample(&cs[0], level);

	return TRUE;
}