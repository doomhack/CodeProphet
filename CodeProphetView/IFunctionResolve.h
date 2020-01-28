#pragma once

class IFunctionResolve
{
public:
	virtual BOOL ParseFile(BOOL undecorate_names) = 0;
	virtual BOOL ResolveAddress(DWORD64 address, CString& name) = 0;
	virtual BOOL GetSourceFile(DWORD64 address, CString& file, unsigned int& line) = 0;
	virtual BOOL UnloadSymbols(void) = 0;
	virtual BOOL SetBaseAddress(DWORD64 address) = 0;
};