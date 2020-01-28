#include "StdAfx.h"

#define DBGHELP_TRANSLATE_TCHAR
#define _NO_CVCONST_H

#include <Dbghelp.h>
#include "PDBFile.h"

BOOL CALLBACK MyEnumSymbolsCallback( SYMBOL_INFO* pSymInfo, ULONG SymbolSize, PVOID UserContext ) ;

CPDBFile::CPDBFile(CString& path)
:file(path)
{
}

CPDBFile::~CPDBFile(void)
{

}

BOOL CPDBFile::ParseFile(BOOL undecorate_names)
{
	DWORD64   base_addr  = 0; 
	DWORD     file_size  = 0;
	DWORD	  options;
	IMAGEHLP_MODULE64 mod = {0};

#ifdef DEBUG
	options = (/*SYMOPT_DEFERRED_LOADS | */SYMOPT_LOAD_LINES | SYMOPT_LOAD_ANYTHING | SYMOPT_DEBUG);
#else
	options = (/*SYMOPT_DEFERRED_LOADS | */SYMOPT_LOAD_LINES | SYMOPT_LOAD_ANYTHING);
#endif

	if(undecorate_names)
		options |= SYMOPT_UNDNAME;

	SymSetOptions(options);

	hProcess = GetCurrentProcess();

	if (!SymInitialize(hProcess, NULL, FALSE))
		return FALSE;

	if (!GetFileParams(file, base_addr, Offset, file_size)) 
		return FALSE;

	base_addr = SymLoadModuleEx(hProcess, NULL, file, NULL, base_addr, file_size, NULL, 0);

	if (!base_addr)
		return FALSE;

	mod.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);

	if(!SymGetModuleInfo64(hProcess, base_addr, &mod))
		return FALSE;




	return TRUE;
}

BOOL CPDBFile::ResolveAddress(DWORD64 address, CString& name)
{
	ULONG64 buffer[(sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR) + sizeof(ULONG64) - 1) / sizeof(ULONG64)];
	PSYMBOL_INFO pSymbol = (PSYMBOL_INFO) buffer;
	DWORD64 displacement;

	pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	pSymbol->MaxNameLen = MAX_SYM_NAME;

	if (SymFromAddr(hProcess, address + Offset, &displacement, pSymbol))
	{
		if(displacement <= MAX_DISPOSITION)
		{
			name = pSymbol->Name;

			return TRUE;
		}
	}

	name = "";
	return FALSE;
}

BOOL CPDBFile::GetSourceFile(DWORD64 address, CString& file, unsigned int& line)
{
	IMAGEHLP_LINE64 lineinfo;
	DWORD displacement;

	lineinfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

	if (SymGetLineFromAddr64(hProcess, address + Offset, &displacement, &lineinfo))
	{
		line = lineinfo.LineNumber;
		file = lineinfo.FileName;
		return TRUE;
	}

	line = 0;
	file = "";
	return FALSE;
}

BOOL CPDBFile::UnloadSymbols()
{
	return SymCleanup(hProcess);
}

BOOL CPDBFile::GetFileParams( const TCHAR* pFileName, DWORD64& BaseAddr, INT64& Offset, DWORD& FileSize ) 
{
	TCHAR szFileExt[_MAX_EXT] = {0}; 

	if(!pFileName) 
		return FALSE; 

	_tsplitpath_s( pFileName, NULL, 0, NULL, 0, NULL, 0, szFileExt, _MAX_EXT ); 

	// Is it .PDB file ? 
	if(!_tcsicmp(szFileExt, _T(".pdb"))) 
	{
		BaseAddr = 0x00400000;
		
		if( !GetFileSize( pFileName, FileSize ) ) 
			return FALSE; 
	}
	else 
	{
		BaseAddr	= 0;
		FileSize	= 0; 
	}

	return TRUE;
}

BOOL CPDBFile::GetFileSize( const TCHAR* pFileName, DWORD& FileSize )
{
	if(!pFileName) 
		return FALSE; 

	// Open the file 
	HANDLE hFile = ::CreateFile( pFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL ); 

	if( hFile == INVALID_HANDLE_VALUE ) 
		return FALSE; 

	// Obtain the size of the file 
	FileSize = ::GetFileSize( hFile, NULL ); 

	::CloseHandle( hFile );

	return ( FileSize != INVALID_FILE_SIZE );
}

BOOL CPDBFile::SetBaseAddress(DWORD64 address)
{
	/*
	Try to get address of cpg_marker.
	Offset = address
	*/
	
	ULONG64 buffer[(sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR) + sizeof(ULONG64) - 1) / sizeof(ULONG64)];
	PSYMBOL_INFO pSymbol = (PSYMBOL_INFO) buffer;	

	memset((void*)buffer,0,sizeof(buffer));

	DWORD old_opt = SymGetOptions();
	DWORD new_opt = (old_opt | SYMOPT_UNDNAME);

	SymSetOptions(new_opt);

	if( !SymFromName(hProcess, _T("cpg_marker"), pSymbol))
	{
		Offset = 0;
		SymSetOptions(old_opt);
		return FALSE;
	}

	SymSetOptions(old_opt);

	Offset = (pSymbol->Address - address);

	return TRUE;
}