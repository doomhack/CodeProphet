#pragma once
#include "ifunctionresolve.h"

class CPDBFile : public IFunctionResolve
{
public:
	CPDBFile(CString& path);
	~CPDBFile(void);

	virtual BOOL ParseFile(BOOL undecorate_names);
	virtual BOOL ResolveAddress(DWORD64 address, CString& name);
	virtual BOOL GetSourceFile(DWORD64 address, CString& file, unsigned int& line);
	virtual BOOL UnloadSymbols(void);
	virtual BOOL SetBaseAddress(DWORD64 address);

private:
	BOOL GetFileParams( const TCHAR* pFileName, DWORD64& BaseAddr, INT64& Offset, DWORD& FileSize );
	BOOL GetFileSize(const TCHAR* pFileName, DWORD& FileSize );

	CString& file;
	HANDLE hProcess;
	INT64 Offset;
};

#define MAX_DISPOSITION 0