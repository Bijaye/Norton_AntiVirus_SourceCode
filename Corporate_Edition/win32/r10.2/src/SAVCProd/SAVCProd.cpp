// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2003, 2005 Symantec Corporation. All rights reserved.
//***************************************************************************
// SavcProd.cpp
//
// Source for SAVCProd.dll -- SAV Corporate Edition ccApp Product plug-in
// responsible for loading functionality plug-ins (in our case, ccEmailProxy).
//
// Dll and class factory routines, things of a global nature.
//***************************************************************************

#include "Stdafx.h"

#define INITIIDS
#include "SymInterface.h"
#include "ccAppPlugin.h"

#include "SavProduct.h"

//***************************************************************************
// Globals
//***************************************************************************

HINSTANCE g_hInstance;

// See syminterface.h for description of the following macros:

// Declaration of our ccApp Plug-in's GUID
// {6803CD1A-EF60-4bce-A4C9-C4120EE7326C}

SYM_DEFINE_OBJECT_ID( SAV_PRODUCT_OBJECT_ID,
    0x6803cd1a, 0xef60, 0x4bce, 0xa4, 0xc9, 0xc4, 0x12, 0xe, 0xe7, 0x32, 0x6c );

DECLARE_CCAPP_PLUGIN( SAV_PRODUCT_OBJECT_ID );

// SYM_OBJECT_MAP_xxx implements class factory and global object counts.
// Need to export: GetFactory, GetObjectCount

SYM_OBJECT_MAP_BEGIN()
    SYM_OBJECT_ENTRY( SAV_PRODUCT_OBJECT_ID, CSavProductPlugin )
SYM_OBJECT_MAP_END()


//***************************************************************************

//*************************************************************************
// DllMain()
//
// BOOL WINAPI DllMain ( HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved )
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

BOOL WINAPI DllMain ( HINSTANCE hInstance, DWORD /*dwReason*/, LPVOID /*lpReserved*/ )
{
    g_hInstance = hInstance;

    return TRUE;
} // DllMain()


