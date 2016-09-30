// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

// dllmain.cpp

// INITIIDS defines GUIDs for SYM interfaces and objects.
// It's used here to provide a central place for defining
// GUIDs for objects defined in this module.
#define INITIIDS

#include "stdafx.h"

#pragma warning( disable : 4530 ) // try catch used when exceptions disabled
#pragma warning( disable : 4786 ) // debug identifier truncated to less than 255 chars


#include "SymInterface.h"
#include "RiskGEDataStore.h"
#include "RiskGEItem.h"
#include "FileHashGEDataStore.h"
#include "FileHashGEItem.h"
#include "FileNameGEDataStore.h"
#include "FileNameGEItem.h"
#include "DirectoryGEDataStore.h"
#include "DirectoryGEItem.h"
#include <time.h>
#include "FineTimeImplementation.c"
#include "dprintf.h"
#include "GERegWatchWrapper.h"
#include "pscan.h"

// These macros provide implementations for GetFactory and GetObjectCount.
SYM_OBJECT_MAP_BEGIN()
    SYM_OBJECT_ENTRY( GlobalExceptionDataStore::SYMOBJECT_RiskGEDataStore,	    CRiskGEDataStore )
    SYM_OBJECT_ENTRY( GlobalExceptionDataStore::SYMOBJECT_RiskGEItem,		    CRiskGEItem )
	SYM_OBJECT_ENTRY( GlobalExceptionDataStore::SYMOBJECT_FileHashGEDataStore,	CFileHashGEDataStore )
    SYM_OBJECT_ENTRY( GlobalExceptionDataStore::SYMOBJECT_FileHashGEItem,		CFileHashGEItem )
	SYM_OBJECT_ENTRY( GlobalExceptionDataStore::SYMOBJECT_FileNameGEDataStore,	CFileNameGEDataStore )
    SYM_OBJECT_ENTRY( GlobalExceptionDataStore::SYMOBJECT_FileNameGEItem,		CFileNameGEItem )
	SYM_OBJECT_ENTRY( GlobalExceptionDataStore::SYMOBJECT_DirectoryGEDataStore,	CDirectoryGEDataStore )
	SYM_OBJECT_ENTRY( GlobalExceptionDataStore::SYMOBJECT_DirectoryGEItem,		CDirectoryGEItem )
SYM_OBJECT_MAP_END()

// DllMain -- module entry point
BOOL APIENTRY DllMain( HANDLE module, DWORD reason, LPVOID reserved )
{
    UNREFERENCED_PARAMETER(reserved);
    UNREFERENCED_PARAMETER(module);

    switch( reason )
    {
		case DLL_PROCESS_ATTACH :
			CGEHRegWatchWrapper::InitGEHRegWatcher();
			MyBeginThread((THREAD)CGEHRegWatchWrapper::StartGEHRegWatcher, NULL, "StartGEHRegWatcher");
			break;

		case DLL_PROCESS_DETACH :
			CGEHRegWatchWrapper::StopGEHRegWatcher();
			break;
    }

    return( TRUE );
}

VOID Real_dprintfTagged(DEBUGFLAGTYPE dwTag, const char *format,...)
{

    va_list marker;

    va_start(marker, format);
    Real_vdprintfCommon(dwTag, format, marker);
    va_end(marker);

}

void Real_dprintf(const char *format,...)
{
    // Calls to this function used to not have the call time prefixed in the logged line.
    va_list marker;

    va_start(marker, format);
    Real_vdprintfCommon(0, format, marker);
    va_end(marker);
}
