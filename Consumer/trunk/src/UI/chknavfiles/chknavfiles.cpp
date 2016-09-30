////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// chknavfiles.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#define INITIIDS
#include "ccAppPlugin.h"
#include "ChkNAVFiles.h"
#include "UninstallEvents.h"
#include "resource.h"


CPlugin::CPlugin()
{	
	m_NAVUninstallEvent.Create(SYM_NAV_UNINSTALL_EVENT);
	m_hKillEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

}

CPlugin::~CPlugin()
{
	if(m_hKillEvent)
	{
		CloseHandle(m_hKillEvent);
	}
}

void CPlugin::Run( ICCApp* pSymApp )
{	

	HANDLE hSingleInstanceMutex = CreateMutex( NULL, FALSE, "CheckNAVFiles" );
	if( hSingleInstanceMutex == NULL )
	{
		// Can't create this: Need to report.
		return;
	}
	else
	{
		// Q: Is there another instance running?
		if( GetLastError() == ERROR_ALREADY_EXISTS )
		{
			// There is.  Bail out now.
			CloseHandle( hSingleInstanceMutex );
			return;
		}
	}

	try
	{
		// We look for the missing files.
		if(IsNAVFileRemoved())
		{
			ReportFatalError(IDS_START_FAILURE);			
		}
	}
	catch(...)
	{
	}

	// Clean up.
	CloseHandle( hSingleInstanceMutex );
	
	return;
}

// Declare plugin.
SYM_DEFINE_GUID(CHECKNAV_UUID, 
	0xb47bcc31, 0xb34e, 0x4ac1, 0xa8, 0x38, 0xac, 0x51, 0x68, 0xd0, 0x90, 0xaa);

DECLARE_CCAPP_PLUGIN( CHECKNAV_UUID );


//////////////////////////////////////////////////////////////////////
// Object map.

SYM_OBJECT_MAP_BEGIN()
	SYM_OBJECT_ENTRY( CHECKNAV_UUID, CPlugin )
SYM_OBJECT_MAP_END()


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    if(DLL_PROCESS_ATTACH == ul_reason_for_call)
    {
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls(hModule);
        
        g_hInstance = (HINSTANCE)hModule;
    }

	return TRUE;
}

