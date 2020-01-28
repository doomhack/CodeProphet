#pragma once

typedef struct FunctionCall
{
	void* address;
	void* frame_ptr;
	u64 timestamp;
} FunctionCall;

typedef struct TreeNode
{
	TreeNode* prev;
	void* func_addr;

	u64 time;
	u64 mintime;
	u64 maxtime;
	
	u32 call_count;
	CStack<TreeNode*> children;
} TreeNode;

class CCallStack
{
	private:
	CStack<FunctionCall> CallStack;
	TreeNode* current_node;
	u64 begintime;
	u64 endtime;
	TreeNode node0;
	u32 isprofiling;
	TCHAR* thread_name;

	public:
	CCallStack::CCallStack();
	CCallStack::~CCallStack();

	void AddCall(void* address, void* frame_ptr, u64& timestamp);
	void ReturnCall(void* frame_ptr, u64& timestamp);

	void DeleteNodes(TreeNode* node);
	TreeNode* GetNode0();
	u64 GetTotalTime();

	void CloseCallGraph();

	void SuspendProfiling();
	void ResumeProfiling();

	void SetThreadName(TCHAR* name);
	TCHAR* GetThreadName();
};