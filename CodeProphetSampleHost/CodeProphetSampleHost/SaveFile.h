#pragma once

//Versions etc
#define MAJOR_VERSION 0
#define MINOR_VERSION 9
#define BUILD_VERSION 3

#define VERSION (MAJOR_VERSION << 24) | (MINOR_VERSION << 16) | (BUILD_VERSION)

#define FILE_ID "CPG"

#define FILE_EXT ".cpg"

#define THREAD_MARKER "TMRK"

#define FILE_TYPE_INSTRUMENT	1
#define FILE_TYPE_SAMPLE		2


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

class CSaveFile
{
	public:
	CSaveFile::CSaveFile();
	CSaveFile::~CSaveFile();

	void AddCallTree(CCallTree* tree);
	void SaveData(std::unordered_map<HANDLE, Module*> *modules, std::unordered_map<DWORD, SampleHit*> *samples);

	private:
	FILE* savefile;
	FileHeaderV2* fh;
	std::vector<CCallTree*> tree;

	std::vector<SampleHit*> sample_table;
	std::unordered_map<SampleHit*, u32> sample_lookup;

	std::vector<Module*> module_table;
	std::unordered_map<Module*, u32> module_lookup;

	void SaveCallTree(TreeNode* node);
	u32 CreateNewFile();
	u32 CreateFileHeader();

	u32 BuildModuleTable(std::unordered_map<HANDLE, Module*> *modules);
	u32 BuildSampleTable(std::unordered_map<DWORD, SampleHit*> *samples);

	u32 SaveModuleTable();
	u32 SaveSampleTable();
	u32 SaveCallGraphs();
};