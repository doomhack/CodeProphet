#pragma once

#include <vector>
#include <unordered_map>

#ifdef _M_AMD64
	#define ARCH "PC_X64"
#elif defined (_M_IX86)
	#define ARCH "PC_X86"
#elif defined (_ARM_)
	#define ARCH "WM_ARM"
#else
	#error No CPU selected.
#endif

#define MAX_STACK_DEPTH 256

typedef struct CallSnapshot
{
  DWORD_PTR dwReturnAddr;
} CallSnapshot;

typedef struct Module
{
	TCHAR* name;
	DWORD_PTR baseaddr;
	u32 len;
} Module;

typedef struct Thread
{
	DWORD	thread_id;
	HANDLE	hthread;
} Thread;

typedef struct SampleHit
{
	Module* module;
	DWORD_PTR offset;
	TCHAR* function;
	TCHAR* source_file;
	u32 line;
} SampleHit;

typedef struct TreeNode
{
	TreeNode* prev;
	SampleHit* sample;

	u32 inc_hits;
	u32 exc_hits;
	
	std::vector<TreeNode*> children;
} TreeNode;

class CCallTree
{
	private:
	TreeNode node0;
	Thread* thread;
	HANDLE hprocess; //Needed for debughelp.
	std::unordered_map<HANDLE, Module*> *modules;
	std::unordered_map<DWORD, SampleHit*> *samples; //Key = addr

	public:
	CCallTree::CCallTree(HANDLE hprocess, Thread* thread, std::unordered_map<HANDLE, Module*>* modules, std::unordered_map<DWORD, SampleHit*>* samples);
	CCallTree::~CCallTree();

	void AddSample(CallSnapshot* cs, u32 depth);
	void ResolveAddress(DWORD_PTR address, SampleHit& sample);
	void GetLine(DWORD_PTR address, SampleHit& sample);

	void DeleteNodes(TreeNode* node);
	TreeNode* GetNode0();
};

class CSampleCallGraph
{
	public:
	CSampleCallGraph::CSampleCallGraph(HANDLE process, DWORD process_id);
	CSampleCallGraph::~CSampleCallGraph();

	u32 StartSampling();

	private:

	u32 EnumerateModules(HANDLE snap);
	u32 EnumerateThreads(HANDLE snap);
	u32 CSampleCallGraph::DoStackWalk(Thread* thread);

	std::unordered_map<DWORD, Thread*> threads; //Key = thread_id;
	std::unordered_map<HANDLE, Module*> modules; //Key = hModule;
	std::unordered_map<DWORD, CCallTree*> callgraphs; //Key = thread_id;
	std::unordered_map<DWORD, SampleHit*> samples; //Key = addr

	HANDLE hprocess;
	DWORD process_id;
};

class CSampleHost
{
	private:
	TCHAR program[MAX_PATH];
	TCHAR cmdline[MAX_PATH];
	CSampleCallGraph* callgraph;
	u32 RunProgram();
	u32 SampleData();


	public:
	CSampleHost::CSampleHost(TCHAR* prog, TCHAR* cmd);
	CSampleHost::~CSampleHost();
	u32 Run();
};