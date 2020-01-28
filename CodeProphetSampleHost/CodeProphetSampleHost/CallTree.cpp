#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

#define DBGHELP_TRANSLATE_TCHAR
#define _NO_CVCONST_H

#ifndef _ARM_
	#include <Dbghelp.h>
#endif

#include "CodeProphetSampleHost.h"
#include "SampleHost.h"

CCallTree::CCallTree(HANDLE hprocess, Thread* thread, std::unordered_map<HANDLE, Module*> *modules, std::unordered_map<DWORD, SampleHit*> *samples)
{
	this->hprocess = hprocess;
	this->thread = thread;
	this->node0.inc_hits = 0;
	this->node0.exc_hits = 0;
	this->node0.prev = NULL;
	this->node0.sample = NULL;

	this->modules = modules;
	this->samples = samples;
}

CCallTree::~CCallTree()
{
	this->DeleteNodes(GetNode0());
}

void CCallTree::AddSample(CallSnapshot* cs, u32 depth)
{
	/*
	Starting at the bottom of the callstack...

	1) Does a sample record exist?
		-If not, create one.
	2) Starting at Node0, Look for sample at this level.
		-If found, incriment counter.
		-If not found, create a new child.
	3) Move up 1 level. Repeat.
	*/

	//static u32 num_samples;
	//printf("Num Samples: %d\n", ++num_samples);

	TreeNode* curr_node = GetNode0();

	while(depth--)
	{
		SampleHit* curr_sample = NULL;

		if( this->samples->find(cs[depth].dwReturnAddr) != this->samples->end() )
			curr_sample = (*this->samples)[cs[depth].dwReturnAddr];
		else
		{
			curr_sample = new SampleHit;

			curr_sample->module = NULL;

			//Find module.
			std::unordered_map<HANDLE, Module*>::iterator i;

			for(i = modules->begin();  i != modules->end(); i++)
			{
				Module* this_mod = i->second;
				
				if(cs[depth].dwReturnAddr >= (DWORD_PTR)this_mod->baseaddr
					&& cs[depth].dwReturnAddr <= (DWORD_PTR)this_mod->baseaddr + this_mod->len)
				{
					curr_sample->module = this_mod;
					break;
				}
			}
			
			//We didn't find the module. (Why not?)
			if(!curr_sample->module)
			{
				delete curr_sample; //We're not really interested in it.
				curr_sample = NULL;
				continue;
			}

			curr_sample->offset = cs[depth].dwReturnAddr - (DWORD_PTR)curr_sample->module->baseaddr;
			
			ResolveAddress(cs[depth].dwReturnAddr, *curr_sample);
			GetLine(cs[depth].dwReturnAddr, *curr_sample);
			
			(*this->samples)[cs[depth].dwReturnAddr] = curr_sample;
		}

		int num_child = (u32)curr_node->children.size();
		u32 found = FALSE;

		while(num_child--)
		{
			TreeNode* this_node = curr_node->children.at(num_child);
			if(this_node->sample == curr_sample)
			{
				curr_node = this_node;
				found = TRUE;
				break;
			}
		}

		if(!found)
		{
			TreeNode* this_node = new TreeNode;
			this_node->inc_hits = 0;
			this_node->exc_hits = 0;
			this_node->prev = curr_node;
			this_node->sample = curr_sample;

			curr_node->children.push_back(this_node);

			curr_node = this_node;
		}

		if(!depth)
			curr_node->exc_hits++;

		curr_node->inc_hits++;
	}
}

void CCallTree::ResolveAddress(DWORD_PTR address, SampleHit& sample)
{
#ifndef _ARM_
	ULONG64 buffer[(sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR) + sizeof(ULONG64) - 1) / sizeof(ULONG64)];
	PSYMBOL_INFO pSymbol = (PSYMBOL_INFO) buffer;
	DWORD64 displacement = 0;
	DWORD64 addr64 = 0;

	addr64 = address;

	pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	pSymbol->MaxNameLen = MAX_SYM_NAME;

	if (SymFromAddr(this->hprocess, addr64, &displacement, pSymbol))
	{
		sample.function = new TCHAR[wcslen(pSymbol->Name)+1];
		wcscpy(sample.function, pSymbol->Name);
	}
	else
		sample.function = NULL;
#else
	sample.function = NULL;
#endif
}

void CCallTree::GetLine(DWORD_PTR address, SampleHit& sample)
{
#ifndef _ARM_
	DWORD disp = 0;
	IMAGEHLP_LINE64 lineinfo;		
	lineinfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

	DWORD64 addr64 = 0;

	addr64 = address;

	if (SymGetLineFromAddr64(this->hprocess, addr64, &disp, &lineinfo))
	{
		sample.line = lineinfo.LineNumber;
		sample.source_file = new TCHAR[wcslen(lineinfo.FileName)+1];
		wcscpy(sample.source_file, lineinfo.FileName);
	}
	else
	{
		sample.line = 0;
		sample.source_file = NULL;
	}
#else
	sample.line = 0;
	sample.source_file = NULL;
#endif
}

void CCallTree::DeleteNodes(TreeNode* node)
{
	u32 num_child = (u32)node->children.size();

	while(num_child--)
		DeleteNodes( node->children.at(num_child) );

	if(node != GetNode0())
		delete node;
}

TreeNode* CCallTree::GetNode0()
{
	return &this->node0;
}