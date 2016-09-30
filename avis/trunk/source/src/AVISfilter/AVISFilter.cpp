// AVISFilter.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "AVISFilter.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


// This is an example of an exported variable
AVISFILTER_API int nAVISFilter=0;

// This is an example of an exported function.
AVISFILTER_API int fnAVISFilter(void)
{
	return 42;
}


