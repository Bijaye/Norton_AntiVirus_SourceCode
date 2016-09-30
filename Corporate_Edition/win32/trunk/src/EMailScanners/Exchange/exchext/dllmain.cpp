// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/
//
//	There are two distinct personalities for this .DLL--as a MAPI Hook Provider or as Storage Extension for virus protect.
//	Essentially these could be two different .DLLs as they are logically separate.  However, the two personalities
//	work with each other.  There is much advantage in having the complementry parts in the same module.
//
//	The "main" routines the storage extension (top level APIs) are in Storage.cpp
//
//	The "main" routines for the MAPI Hook provider are in HookProvider.cpp
//
//	Most functions in this .DLL are called only in one of the personalites or work the same way in both personalities.
//	There are a few exceptions to this rule.  The code can determine which personality is
//	executing by looking at the global variable g_personality.
//
//	This is the module where global variables are declared.  Stating the obvious, global variables are NOT SHARED!  Each
//	personality has its own copy of them.  If the variables need to be shared, they need to be put into the shared memory
//	file (with proper care and synchronization).

#include "DarwinResCommon.h"
#include "ldvpmec.h"
#include "resource.h"
#include "advapi.h"
#include "SavrtModuleInterface.h"

/*---------------------------------------------------------------------------*/

// .DLL Wide Globals

TCHAR
	g_szComputerName[MAX_COMPUTERNAME_LENGTH + 1];	// Computer name

HINSTANCE
	g_hResInstance;						// Resource Instance

PERSONALITES
	g_personality = Unknown_Personality;


/*---------------------------------------------------------------------------*/
DARWIN_DECLARE_RESOURCE_LOADER(_T("VpmSece4Res.dll"), _T("VpmSece4"))

BOOL WINAPI DllMain(HINSTANCE hInstance, ULONG ul_reason_for_call, LPVOID /* lpReserved */)
{
	switch(ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH :
			{
			if (SUCCEEDED(g_ResLoader.Initialize()))
				g_hResInstance = g_ResLoader.GetResourceInstance();
			else
				return FALSE;

			DWORD dwComputerNameLength = MAX_COMPUTERNAME_LENGTH+1;
			GetComputerName(g_szComputerName, &dwComputerNameLength);

			DisableThreadLibraryCalls(hInstance);
			}
			break;

		case DLL_PROCESS_DETACH:
			break;

		default:
			break;
	}
	
	return TRUE;
}

/* end source file */