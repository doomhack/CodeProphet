#include "StdAfx.h"
#include "NoResolve.h"

BOOL CNoResolve::ParseFile(BOOL undecorate_names)
{
	return TRUE;
}

BOOL CNoResolve::ResolveAddress(DWORD64 address, CString& name)
{
	name = "";
	return TRUE;
}

BOOL CNoResolve::GetSourceFile(DWORD64 address, CString& file, unsigned int& line)
{
	line = 0;
	file = "";
	return TRUE;
}

BOOL CNoResolve::UnloadSymbols(void)
{
	return TRUE;
}

BOOL CNoResolve::SetBaseAddress(DWORD64 address)
{
	return TRUE;
}