#pragma once

#define FILE_TYPE_INSTRUMENT	1
#define FILE_TYPE_SAMPLE		2

#ifdef _ARM_
	HMODULE GetModuleHandleByAddress(LPVOID lpAddress);
#endif

void SaveAllCallTrees(void* tree);
void CleanUpSave();

//This struct needs ti be kept in sync with
typedef struct FileHeaderV2
{
	char id[4];
	u32 version;
	char arch[8];
	SYSTEMTIME time;
	u64 timer_freq;
	u64 base_addr;
	u32 ptrlen;
	u32 filetype;
} FileHeaderV2;

class CSaveCallgraph
{
	public:
	CSaveCallgraph::CSaveCallgraph();
	CSaveCallgraph::~CSaveCallgraph();
	void SaveAllCallTrees();

	private:
	FileHeaderV2* fh;
	FILE* savefile;
	wchar_t* filename;

	void CreateFileHeader();
	void SaveCallTree(TreeNode* node);
	void SaveFileHeader();
	void GetFileName();
	void WriteNumThreads(u32 threads);
};