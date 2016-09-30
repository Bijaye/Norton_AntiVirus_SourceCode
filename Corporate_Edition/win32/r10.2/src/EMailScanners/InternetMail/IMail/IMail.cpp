// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2003, 2005 Symantec Corporation. All rights reserved.
//***************************************************************************
// IMail.cpp : Defines the entry point for the DLL application.
//
// IMail.cpp
//
// Source for IMail.dll -- SAV Corporate Edition Internet Email storage extension.
//
// Dll and class factory routines, things of a global nature.
//***************************************************************************

#include "stdafx.h"

//***************************************************************************
// Globals
//***************************************************************************

#ifdef __cplusplus
extern "C" 
{
#endif

HINSTANCE g_hInstance;

#ifdef __cplusplus
}
#endif

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


// Required to make use of PSCAN's CDecomposerSDK wrapper around Dec3.

extern "C" BOOL GetNAVAppPath(LPTSTR lpszLibPath);

BOOL GetNAVAppPath(LPTSTR lpszAppPath)
{
    auto    LPTSTR  lpszEnd;
    auto    BOOL    bSuccess;

    bSuccess = FALSE;

    if ( GetModuleFileName( NULL, lpszAppPath, MAX_PATH ) )
    {
        lpszEnd = _tcsrchr( lpszAppPath, _T('\\') );

        if ( lpszEnd )
        {
            *lpszEnd = _T('\0');
            bSuccess = TRUE;
        }
    }

    return ( bSuccess );
}
