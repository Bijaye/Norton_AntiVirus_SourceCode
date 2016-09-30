// AVISdb.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "AVISdbDLL.h"
#include "Internal.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	static	bool	firstThread	= true;

    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			Internal::OnProcessInit();
			if (!firstThread)
				break;
			firstThread = false;	// drop through to the thread attach for the
									// primary thread.

		case DLL_THREAD_ATTACH:
			Internal::OnThreadInit();
			break;

		case DLL_THREAD_DETACH:
			Internal::OnThreadExit();
			break;

		case DLL_PROCESS_DETACH:
			Internal::OnProcessExit();
			break;
    }
    return TRUE;
}

AVISDB_API void AVISDatabaseName(std::string& avisDBName)
{
	Internal::DatabaseName(avisDBName);
}


