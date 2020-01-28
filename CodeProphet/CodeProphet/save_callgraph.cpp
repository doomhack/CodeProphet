#include <stdio.h>
#include <time.h>

#ifndef _ARM_
	#include <io.h>
#else
	#include <stdlib.h>
#endif

#include <Windows.h>

#ifdef _ARM_
	#include <psapi.h>
	#include <tlhelp32.h>
#endif

#include "global_data.h"

/*
We export a variable which we can get the address of both at runtime and 'post-mortum' in dbghelp.
This allows us to get the base address for relocated dll's and exe's. The symbol (and its address)
Gets rebased with the rest of the code/data!
*/
extern "C" __declspec(dllexport) u32 cpg_marker = 0;

/*
*/
extern CStack<CCallStack*> *g_stacklist;

/*
*/
extern CSaveCallgraph* savecallgraph;

/*
*/
Mutex treelocked = 0;


void SaveAllCallTrees(void* tree)
{
	CSEnter(treelocked);

	if(!savecallgraph)
		savecallgraph = new CSaveCallgraph;

	savecallgraph->SaveAllCallTrees();

	CSExit(treelocked);
}

void CleanUpSave()
{
	CSEnter(treelocked);

	if(savecallgraph)
		delete savecallgraph;

	savecallgraph = NULL;

	CSExit(treelocked);
}

CSaveCallgraph::CSaveCallgraph()
{
	this->fh = NULL;
	this->savefile = NULL;

	filename = new wchar_t[MAX_PATH];
	GetFileName();

	savefile = _wfopen(filename,TEXT("wb") );

	delete[] filename;
	filename = NULL;
}

CSaveCallgraph::~CSaveCallgraph()
{
	fclose(savefile);
	savefile = NULL;

	delete fh;
	fh = NULL;
}


void CSaveCallgraph::SaveAllCallTrees()
{
	CreateFileHeader();
	SaveFileHeader();

	u32 num_threads = g_stacklist->Size();
	savecallgraph->WriteNumThreads(num_threads);

	while(num_threads--)
	{
		CCallStack* thistree = g_stacklist->At(num_threads);

		thistree->CloseCallGraph();

		fwrite(THREAD_MARKER,sizeof(char),4,savefile);

		u64 tottime = thistree->GetTotalTime();
		fwrite( &tottime, sizeof(u64), 1, savefile);

		SaveCallTree( thistree->GetNode0() );
	}
}

void CSaveCallgraph::CreateFileHeader()
{
	if(!fh)
		fh = new FileHeaderV2;

	strcpy(fh->id,FILE_ID);
	fh->version = VERSION;
	strcpy(fh->arch, ARCH);
	
	GetLocalTime(&fh->time);

	CTimer::GetTimerFreq(fh->timer_freq);

	fh->ptrlen = sizeof(void*);

	fh->base_addr = (u64)MARKER_ADDRESS;

	fh->filetype = FILE_TYPE_INSTRUMENT;
}

//Recursively save the tree.
void CSaveCallgraph::SaveCallTree(TreeNode* node)
{
	u32 num_child = node->children.Size();
	fwrite(&num_child, sizeof(u32), 1, savefile);
	fwrite(&node->func_addr, sizeof(void*), 1, savefile);
	fwrite(&node->time, sizeof(u64), 1, savefile);
	fwrite(&node->mintime, sizeof(u64), 1, savefile);
	fwrite(&node->maxtime, sizeof(u64), 1, savefile);
	fwrite(&node->call_count, sizeof(u32), 1, savefile);

	while(num_child--)
		SaveCallTree( node->children.At(num_child) );
}

void CSaveCallgraph::SaveFileHeader()
{
	fseek(savefile, 0, SEEK_SET);
	fwrite(fh,sizeof(FileHeaderV2),1,savefile);
}

void CSaveCallgraph::GetFileName()
{
	wchar_t* dot;

#ifndef _ARM_
	GetModuleFileName( HINST_THISCOMPONENT, filename, MAX_PATH);
#else
	GetModuleFileName( GetModuleHandleByAddress(&cpg_marker), filename, MAX_PATH);
#endif
	dot = wcsrchr(filename, '.');

	wcscpy(dot,TEXT(FILE_EXT));
}

void CSaveCallgraph::WriteNumThreads(u32 threads)
{
	fseek(savefile,sizeof(FileHeaderV2),SEEK_SET);
	fwrite(&threads,sizeof(u32),1,savefile);
}


#ifdef _ARM_

HMODULE GetModuleHandleByAddress(LPVOID lpAddress)
{
	//Is this module the main .exe?
	HMODULE mod = NULL;
	MODULEINFO modinfo;
	
	if ( GetModuleInformation(GetCurrentProcess(), NULL, &modinfo, sizeof(MODULEINFO)) )
	{
		if(lpAddress >= modinfo.lpBaseOfDll && lpAddress <= (BYTE*)modinfo.lpBaseOfDll + modinfo.SizeOfImage)
			return mod;
	}
	else //GetModuleInformation Failed.
		return NULL;

	//Ok, we're a loaded dll then...
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);

	if (snap == INVALID_HANDLE_VALUE)
		return NULL;

	MODULEENTRY32 modinfo32;
	modinfo32.dwSize = sizeof(MODULEENTRY32);

	if (! Module32First(snap, &modinfo32))
	{
		CloseToolhelp32Snapshot(snap);
		return NULL;
	}

	do
	{
		if(lpAddress >= modinfo32.modBaseAddr && lpAddress <= modinfo32.modBaseAddr + modinfo32.modBaseSize)
		{
			CloseToolhelp32Snapshot(snap);
			return modinfo32.hModule;
		}

	} while( Module32Next(snap, &modinfo32) );
	
	//We didn't find a match.
	CloseToolhelp32Snapshot(snap);
	return NULL;
}

#endif //_ARM_