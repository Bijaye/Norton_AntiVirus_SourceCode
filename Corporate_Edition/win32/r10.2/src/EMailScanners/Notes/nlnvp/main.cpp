// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#include <windows.h>
#include "SavrtModuleInterface.h"
#include "nlnvp.h"

/*---------------------------------------------------------------------------*/

//	DllMain standard Win32 dll entry point function.

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	switch(dwReason)
	{
		case DLL_PROCESS_ATTACH:
			SAVRTModuleInit();
			DisableThreadLibraryCalls(hInstance);
			break;

		case DLL_PROCESS_DETACH:
			SAVRTModuleUnInit();
			break;

		default:
			break;
	}

	return TRUE;
}

/*--- end of source ---*/
