#pragma once
#include "ifunctionresolve.h"

class CNoResolve : public IFunctionResolve
{
public:
	virtual BOOL ParseFile(BOOL undecorate_names);
	virtual BOOL ResolveAddress(DWORD64 address, CString& name);
	virtual BOOL GetSourceFile(DWORD64 address, CString& file, unsigned int& line);
	virtual BOOL UnloadSymbols(void);
	virtual BOOL SetBaseAddress(DWORD64 address);
};
