#pragma once

#include <vector>
#include <deque>
#include "IFunctionResolve.h"

typedef enum ESymbolResolution
{
	symres_pdb,
	symres_map,
	symres_none
} ESymbolResolution;

class CNewProjectSettings
{
	public:
	ESymbolResolution symbol_res;
	CString symres_file;

	CString cpgfile;
	BOOL time_norm;
	BOOL tfold;
	BOOL undec;
};

typedef struct FileHeaderV2
{
	char id[4];
	unsigned int version;
	char arch[8];
	SYSTEMTIME time;
	DWORD64 timer_freq;
	DWORD64 base_addr;
	DWORD ptrlen;
	unsigned int filetype;
} FileHeaderV2;

typedef enum EPlatformArch
{
	x86_pc,
	x64_pc,
	wm5_arm,
	undefined
} EPlatformArch;

typedef struct TreeNode
{
	TreeNode* prev;
	CString func_addr;
	CString func_name;
	CString src_file;
	unsigned int src_line;

	DWORD64 func_addr_ptr;

	double tot_msec;
	double tot_pc;

	double exc_msec;
	double exc_pc;

	double min_msec;
	double max_msec;

	double avg_msec;

	unsigned int num_child;
	unsigned int call_count;

	DWORD64 time;
	DWORD64 min_time;
	DWORD64 max_time;

	std::vector<TreeNode*> children;

} TreeNode;

typedef struct Class
{
	CString class_name;
	std::vector<Class*> subclasses;
	std::vector<TreeNode*> members;
} Class;

class CCallTree
{
	public:
	CCallTree();
	~CCallTree();

	void BuildCallGraph(TreeNode* node, TreeNode* parent, CFile& file, DWORD64 timer_freq);
	DWORD64 GetTotalTime();
	TreeNode* GetNode0();
	void SetTotalTime(DWORD64 tot_time);
	void DeleteTrees(TreeNode* node);
	std::vector<TreeNode*> function_list;
	std::vector<TreeNode*> recursive_functions;
	std::vector<Class*> classes;
	DWORD64 start_time;
	DWORD64 end_time;

	private:
	DWORD64 tot_time;
	TreeNode node0;
	TCHAR* thread_name;
};


class CCallGraph
{
	public:
	CCallGraph(void);
	~CCallGraph(void);

	int LoadFromFile(CFile& file, CNewProjectSettings& proj_set, IFunctionResolve* resolver);

	unsigned int version;
	EPlatformArch arch;
	CTime prof_time;
	DWORD64 timer_freq;
	unsigned int num_threads;
	DWORD ptrlen;

	//std::vector<CCallTree*> g_treelist;
	std::deque<CCallTree*> g_treelist;

	private:
	int BuildCallTrees(unsigned int num_trees, CFile& file);
};