#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <time.h>

#include "CodeProphetSampleHost.h"
#include "SampleHost.h"
#include "SaveFile.h"

CSaveFile::CSaveFile()
{
	this->fh = NULL;
	this->savefile = NULL;
}

CSaveFile::~CSaveFile()
{
	if(this->fh)
		delete this->fh;
}

void CSaveFile::SaveData(std::unordered_map<HANDLE, Module*> *modules, std::unordered_map<DWORD, SampleHit*> *samples)
{
	if(!CreateNewFile())
		return;

	if(!CreateFileHeader())
		return;

	if(!BuildModuleTable(modules))
		return;

	if(!BuildSampleTable(samples))
		return;

	if(!SaveModuleTable())
		return;

	if(!SaveSampleTable())
		return;

	if(!SaveCallGraphs())
		return;

	fwrite("END", 1, 3, savefile);

	fclose(savefile);
}

void CSaveFile::AddCallTree(CCallTree* tree)
{
	this->tree.push_back(tree);
}

u32 CSaveFile::SaveCallGraphs()
{
	u32 num_threads = (u32)this->tree.size();
	
	fwrite("TT", 1, 2, savefile);

	fwrite(&num_threads, sizeof(u32), 1, savefile);

	while (num_threads--)
	{
		CCallTree* this_tree = this->tree.at(num_threads);

		fwrite(THREAD_MARKER, 1, 4, savefile);

		SaveCallTree(this_tree->GetNode0());
	}

	return TRUE;
}

void CSaveFile::SaveCallTree(TreeNode* node)
{
	u32 num_child = (u32)node->children.size();

	u32 sample_num = this->sample_lookup[node->sample];
	fwrite(&sample_num, sizeof(u32), 1, savefile);

	fwrite(&node->exc_hits, sizeof(u32), 1, savefile);
	fwrite(&node->inc_hits, sizeof(u32), 1, savefile);

	fwrite(&num_child, sizeof(u32), 1, savefile);

	while(num_child--)
		SaveCallTree( node->children.at(num_child) );
}

u32 CSaveFile::CreateNewFile()
{
	savefile = _wfopen(_T("cp_sampledata.cpg"),_T("wb"));

	return (savefile != NULL);
}

u32 CSaveFile::BuildModuleTable(std::unordered_map<HANDLE, Module*> *modules)
{
	//Find module.
	std::unordered_map<HANDLE, Module*>::iterator i;

	u32 c = 0;

	for(i = modules->begin();  i != modules->end(); i++, c++)
	{
		Module* this_mod = i->second;
		
		this->module_table.push_back(this_mod);
		this->module_lookup[this_mod] = c;
	}

	return TRUE;
}

u32 CSaveFile::BuildSampleTable(std::unordered_map<DWORD, SampleHit*> *samples)
{
	//Find module.
	std::unordered_map<DWORD, SampleHit*>::iterator i;

	u32 c = 0;

	for(i = samples->begin();  i != samples->end(); i++, c++)
	{
		SampleHit* this_sample = i->second;
		
		this->sample_table.push_back(this_sample);
		this->sample_lookup[this_sample] = c;
	}

	return TRUE;
}

u32 CSaveFile::CreateFileHeader()
{
	this->fh = new FileHeaderV2;
	
	if(!fh)
		return FALSE;

	strcpy(this->fh->id, FILE_ID);
	this->fh->version = VERSION;
	strcpy(this->fh->arch, ARCH);
	GetLocalTime(&this->fh->time);
	
	this->fh->timer_freq = 0; //Not used;
	this->fh->base_addr = 0; 
	this->fh->ptrlen = sizeof(DWORD_PTR);

	this->fh->filetype = FILE_TYPE_SAMPLE;

	fwrite(fh, sizeof(FileHeaderV2), 1, savefile);
	
	delete fh;
	fh = NULL;

	return TRUE;
}

u32 CSaveFile::SaveModuleTable()
{
	fwrite("MT", 2, 1, savefile);

	u32 num_mods = (u32)this->module_table.size();
	
	while(num_mods--)
	{
		Module* this_mod = this->module_table.at(num_mods);

		fwrite(&this_mod->baseaddr, sizeof(DWORD_PTR), 1, savefile); //Baseaddr
		fwrite(&this_mod->len, sizeof(u32), 1, savefile); //Len

		u32 name_len = (u32)wcslen(this_mod->name);

		fwrite(&name_len, sizeof(u32), 1, savefile);

		fwrite(this_mod->name, sizeof(TCHAR), name_len + 1, savefile); //Include the /0 string terminator.
	}

	return TRUE;
}

u32 CSaveFile::SaveSampleTable()
{
	fwrite("ST", 2, 1, savefile); //Write a marker.

	u32 num_sam = (u32)this->sample_table.size();
	
	while(num_sam--)
	{
		SampleHit* this_sam = this->sample_table.at(num_sam);

		u32 mod = this->module_lookup[this_sam->module]; //get sample number
		fwrite(&mod, sizeof(u32), 1, savefile); //write it.
	
		fwrite(&this_sam->offset, sizeof(DWORD_PTR), 1, savefile); //Write offset.

		if(this_sam->function)
		{
			u32 func_name_len = (u32)wcslen(this_sam->function);
			fwrite(&func_name_len, sizeof(u32), 1, savefile);
			fwrite(this_sam->function, sizeof(TCHAR), func_name_len + 1, savefile); //Include the /0 string terminator.
		}
		else
		{
			u32 zero = 0;
			fwrite(&zero, sizeof(u32), 1, savefile);
		}

		if(this_sam->source_file)
		{
			u32 src_name_len = (u32)wcslen(this_sam->source_file);
			fwrite(&src_name_len, sizeof(u32), 1, savefile);
			fwrite(this_sam->source_file, sizeof(TCHAR), src_name_len + 1, savefile); //Include the /0 string terminator.
		}
		else
		{
			u32 zero = 0;
			fwrite(&zero, sizeof(u32), 1, savefile);
		}

		fwrite(&this_sam->line, sizeof(u32), 1, savefile);
	}

	return TRUE;
}

#if 0
#ifdef _ARM_
time_t time( time_t *inTT ) 
{
	SYSTEMTIME sysTimeStruct;
	FILETIME fTime;
	ULARGE_INTEGER int64time;
	
	GetSystemTime( &sysTimeStruct );
	if ( SystemTimeToFileTime( &sysTimeStruct, &fTime ) )
	{
		memcpy( &int64time, &fTime, sizeof( FILETIME ) );
		/* Subtract the value for 1970-01-01 00:00 (UTC) */
		int64time.QuadPart -= 0x19db1ded53e8000;
		/* Convert to seconds. */
		int64time.QuadPart /= 10000000;
		*inTT = (time_t)int64time.QuadPart;
	}

	return *inTT;
}
#endif
#endif