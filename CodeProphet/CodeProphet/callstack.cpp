#include <stdio.h>

#ifndef _ARM_
	#include <process.h>
#endif

#include <assert.h>
#include <Windows.h>
#include "global_data.h"

CCallStack::CCallStack()
{
	node0.func_addr = NULL;
	node0.prev = NULL;
	node0.call_count = 0;
	node0.time = 0;
	node0.maxtime = 0;
	node0.mintime = 0;

	this->begintime = 0;
	this->endtime = 0;

	current_node = &node0;

	this->isprofiling = TRUE;
	thread_name = NULL;
}

CCallStack::~CCallStack()
{
	this->DeleteNodes(&this->node0);

	if(thread_name)
		delete[] thread_name;
}

void CCallStack::AddCall(void* address, void* frame_ptr, u64& timestamp)
{
	if(!this->isprofiling )
		return;

	FunctionCall fc = {address, frame_ptr, timestamp};

	this->CallStack.Push(fc);

	/*
	1) Search the current node for a child matching address.
	2) If there is one, set the current_node pointer to this node then return.
	3) If not, then create a new node and set the current_node pointer to this.
	*/
	u32 num_children = current_node->children.Size();

	while(num_children--)
	{
		TreeNode* thisnode = current_node->children.At(num_children);

		if(thisnode->func_addr == address)
		{
			current_node = thisnode;
			return;
		}
	}

	//Have not found. Need to create a new node.
	TreeNode * newnode = new TreeNode;
	newnode->func_addr = address;
	newnode->prev = current_node;
	newnode->call_count = 0;
	newnode->time = 0;
	newnode->mintime = 0;
	newnode->maxtime = 0;

	current_node->children.Push(newnode);
	current_node = newnode;

	//Set start time if not already done. 
	if(!this->begintime)
		this->begintime = timestamp;
}

void CCallStack::ReturnCall(void* frame_ptr, u64& timestamp)
{
	/*
	When a function returns, we need to:
	1) Unwind the stack and walk up the tree until either:
		-We get to Node0.
		-Or we find a match.
	2) If we find a match, incriment call counts etc.
	3) If we find node0, update endtime and return.
	*/

	if(!this->isprofiling )
		return;

	FunctionCall fc;

	//Set end timestamp.
	this->endtime = timestamp;

	while(1)
	{
		//Function has returned. We pop off the last call.
		fc = this->CallStack.Pop();

		//We are at the right level. (Most common case!)
		if(fc.frame_ptr == frame_ptr)
			break;
		else //Step back up the call tree.
			current_node = current_node->prev;

		//If we're back up to node0, don't update call counts etc.
		//Things are screwed up enough already!!!
		if( current_node == this->GetNode0())
			return;
	}

	//Function time = call time - return time.
	u64 functiontime;
	
	if(timestamp > fc.timestamp)
		functiontime = (timestamp - fc.timestamp);
	else //This shouldn't happen but buggy HAL's, unsynced TSC's etc mean it might!
		functiontime = 0;

	current_node->time += functiontime;

	current_node->call_count++;

	if( (current_node->mintime > functiontime) || (!current_node->mintime) )
		current_node->mintime = functiontime;

	if( (current_node->maxtime < functiontime) || (!current_node->maxtime) )
		current_node->maxtime = functiontime;

	//Step back up the call tree.
	current_node = current_node->prev;
}

void CCallStack::DeleteNodes(TreeNode* node)
{
	u32 num_child = node->children.Size();

	while(num_child--)
		this->DeleteNodes( node->children.At(num_child) );

	//We don't delete node0 because it is statically allocated.
	if(node != &this->node0)
		delete node;
}

TreeNode* CCallStack::GetNode0()
{
	return &this->node0;
}

u64 CCallStack::GetTotalTime()
{
	return (this->endtime - this->begintime);
}

void CCallStack::CloseCallGraph()
{
	FunctionCall fc;

	CTimer::GetTimerTime(this->endtime);

	while(current_node != this->GetNode0())
	{
		fc = this->CallStack.Pop();
	
		//Function time = call time - return time.
		u64 functiontime = (this->endtime - fc.timestamp);

		current_node->time += functiontime;

		current_node->call_count++;

		if( (current_node->mintime > functiontime) || (!current_node->mintime) )
			current_node->mintime = functiontime;

		if( (current_node->maxtime < functiontime) || (!current_node->maxtime) )
			current_node->maxtime = functiontime;

		//Step back up the call tree.
		current_node = current_node->prev;
	}
}

void CCallStack::SuspendProfiling()
{
	this->isprofiling = FALSE;
}

void CCallStack::ResumeProfiling()
{
	this->isprofiling = TRUE;
}

void CCallStack::SetThreadName(TCHAR* name)
{
	if(!name)
		return;

	u32 len = wcslen(name);

	this->thread_name = new TCHAR[len+1];

	wcscpy(thread_name, name);
}

TCHAR* CCallStack::GetThreadName()
{
	return thread_name;
}