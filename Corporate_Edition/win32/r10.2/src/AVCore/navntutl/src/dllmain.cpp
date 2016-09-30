// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998 - 2003, 2005 Symantec Corporation. All rights reserved. 
//*************************************************************************
// dllmain.cpp - created 6/9/98 5:53:30 PM
//
// $Header:   S:/NAVNTUTL/VCS/dllmain.cpv   1.0   10 Jun 1998 15:47:42   DBuches  $
//
// Description:  Contains main entry point for NAVNTUTL DLL.
//
// Contains:
//
//*************************************************************************
// $Log:   S:/NAVNTUTL/VCS/dllmain.cpv  $
// 
//    Rev 1.0   10 Jun 1998 15:47:42   DBuches
// Initial revision.
//*************************************************************************

#include "windows.h"

//*************************************************************************
// DllMain()
//
// DllMain(
//       HANDLE hInst
//      ULONG ulReason
//      LPVOID pReserved )
//
// Description: Performs DLL initialization and termination.  Does nothing 
//              at the moment.
//
// Returns: BOOL WINAPI 
//
//*************************************************************************
// 6/9/98 DBUCHES, created - header added.
//*************************************************************************

BOOL WINAPI DllMain( HANDLE hInst, ULONG ulReason, LPVOID pReserved )
{
    UNREFERENCED_PARAMETER( hInst );
    UNREFERENCED_PARAMETER( ulReason );
    UNREFERENCED_PARAMETER( pReserved );

    return TRUE;
}


