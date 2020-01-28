#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

#include "CodeProphetSampleHost.h"
#include "SampleHost.h"


CSampleHost::CSampleHost(TCHAR* prog, TCHAR* cmd)
{
	if(!prog)
		wcscpy_s(this->program, MAX_PATH, _T(""));
	else
		wcscpy_s(this->program, MAX_PATH, prog);

	if(!cmd)
		wcscpy_s(this->cmdline, MAX_PATH, _T(""));
	else
		wcscpy_s(this->cmdline, MAX_PATH, cmd);

	this->callgraph = NULL;
}

CSampleHost::~CSampleHost()
{
	delete this->callgraph;
}

u32 CSampleHost::Run()
{	
	if (!RunProgram())
		return FALSE;

	if (!SampleData())
		return FALSE;

	return TRUE;
}

u32 CSampleHost::RunProgram()
{
	PROCESS_INFORMATION psi = {NULL, NULL, 0 ,0};
	STARTUPINFO sti = {0};
	sti.cb = sizeof(STARTUPINFO);
	TCHAR startup_dir[MAX_PATH];
	TCHAR *c;

	wcscpy(startup_dir, this->program);
	c = wcsrchr(startup_dir, '\\');
	*(c+1) = '\0';

#ifndef _ARM_
	if( !CreateProcess(this->program, this->cmdline, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, startup_dir, &sti, &psi) )
#else
	if( !CreateProcess(this->program, this->cmdline, NULL, NULL, FALSE, 0, NULL, NULL, NULL, &psi) )
#endif
		return FALSE;

	//Spin until program has actually started.
	DWORD isrunning = 0;
	do
	{
		GetExitCodeProcess(psi.hProcess ,&isrunning);
	} while (isrunning != STILL_ACTIVE);

	this->callgraph = new CSampleCallGraph(psi.hProcess, psi.dwProcessId);

	return TRUE;
}

u32 CSampleHost::SampleData()
{
	if(!this->callgraph->StartSampling())
		return FALSE;

	return TRUE;
}