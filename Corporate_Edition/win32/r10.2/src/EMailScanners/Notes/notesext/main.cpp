// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#include "notesext.h"

/*---------------------------------------------------------------------------*/

//	DllMain standard Win32 dll entry point function.

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	switch(dwReason)
	{
		case DLL_PROCESS_ATTACH:
			if(!g_hShutdown)
				g_hShutdown = CreateEvent(NULL, TRUE, FALSE, NULL); 

			g_hResource = hInstance;

			DisableThreadLibraryCalls(hInstance);
			break;

		case DLL_PROCESS_DETACH:
			if(g_hShutdown)
				CloseHandle(g_hShutdown);
			
			break;
	}

	return TRUE;
}

/*--- end of source ---*/
