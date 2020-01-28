// CodeProphetSampleHost.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

#include <vector>
#include <unordered_map>

#include "CodeProphetSampleHost.h"
#include "SampleHost.h"


int _tmain(int argc, _TCHAR* argv[])
{
	CSampleHost* host = NULL;

	if (argc <= 1)
		printf("Usage: cpsh32 <Program to profile> <Commandline params>\n");
	else
	{
		host = new CSampleHost(argv[1],argv[2]);
		
		if(!host->Run())
			printf("Failed to launch program.\n");

		delete host;
		host = NULL;
	}

	return 0;
}