// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2003, 2005 Symantec Corporation. All rights reserved.
//***************************************************************************
// SavEmail.cpp
//
// Source for SAVEmail.dll -- SAV Corporate Edition ccEmailProxy filter plug-in.
//
// Dll and class factory routines, things of a global nature.
//***************************************************************************

#include "Stdafx.h"

#include "vpcommon.h"
#include "clientreg.h"

#define INITIIDS
#include "SymInterface.h"
#include "EmailProxyInterface.h"
#include "ccSettingsInterface.h"
#include "SavrtModuleInterface.h"
#undef INITIIDS

#include "SavEmailFilter.h"

//***************************************************************************
// Globals
//***************************************************************************

HINSTANCE g_hInstance;

// See syminterface.h for description of the following macros:

// Declaration of our IEmailFilter object's GUID
// {B58CE337-3CA1-47e4-B900-D7894861696F}

SYM_DEFINE_OBJECT_ID(SAV_FILTER_OBJECT_ID, 
    0xb58ce337, 0x3ca1, 0x47e4, 0xb9, 0x0, 0xd7, 0x89, 0x48, 0x61, 0x69, 0x6f);

// SYM_OBJECT_MAP_xxx implements class factory and global object counts.
// Need to export: GetFactory, GetObjectCount

SYM_OBJECT_MAP_BEGIN()
    SYM_OBJECT_ENTRY(SAV_FILTER_OBJECT_ID, CSavEmailFilter)
SYM_OBJECT_MAP_END()


//***************************************************************************

//*************************************************************************
// DllMain()
//
// BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
//
// Parameters:
//      hInstance
//      dwReason
//      lpReserved
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.07.09 DALLEE - comments.
//*************************************************************************

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD /*dwReason*/, LPVOID /*lpReserved*/)
{
    g_hInstance = hInstance;

    return TRUE;
} // DllMain()


//*************************************************************************
// GetFilterObjectID()
//
// extern "C" SYMRESULT WINAPI GetFilterObjectID(SYMOBJECT_ID* pID)
//
// Required export for all ccEmailProxy filter plug-ins.
// Returns GUID of plug-in's IEmailFilter object
//
// Parameters:
//      pID
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.07.09 DALLEE - comments.
//*************************************************************************

extern "C" SYMRESULT WINAPI GetFilterObjectID(SYMOBJECT_ID* pID)
{
    if (pID == NULL)
        return SYMERR_INVALIDARG;

    *pID = SAV_FILTER_OBJECT_ID;

    return SYM_OK;
} // GetFilterObjectID()

