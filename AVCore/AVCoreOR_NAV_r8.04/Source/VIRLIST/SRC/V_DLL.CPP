// Copyright 1995 Symantec Corporation
//***************************************************************************
// 
// $Header:   S:\virlist\vcs\v_dll.cpv   1.5   01 Mar 1996 12:02:02   JWORDEN  $ 
// 
// Description:                                                         
// 
// Contains:                                                            
// 
// See Also:                                                            
// 
//***************************************************************************
// $Log:   S:\virlist\vcs\v_dll.cpv  $ 
// 
//    Rev 1.5   01 Mar 1996 12:02:02   JWORDEN
// DBCS enable
// 
//    Rev 1.3.1.0   12 Jul 1995 18:59:24   JREARDON
// Branch base for version QAK8
// 
//    Rev 1.3   23 Jun 1995 15:01:08   KEITH
// Tape on a header
//***************************************************************************

#if defined(SYM_WIN)

#include "platform.h"
#include "virusdbf.h"
#include "navbtmap.h"

HINSTANCE hInstance;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

int WINAPI LibMain (
    HINSTANCE hInst,
    WORD wDataSeg,
    WORD wHeapSize,
    LPSTR lpszCmdLine );

int SYM_EXPORT WINAPI WEP (
    int nParam );

// ---------------------------------------------------------------------------

#if defined(SYM_WIN32)

/****************************************************************************
   FUNCTION: DllMain

   PURPOSE:  Called when a new process is either using or not using
             this DLL anymore.

*******************************************************************************/

BOOL SYM_EXPORT WINAPI DllMain (
    HINSTANCE   hInstDLL,
    DWORD       dwReason,
    LPVOID      lpvReserved
    )
{
    BOOL        bResult = TRUE;

    switch (dwReason)
        {
        case DLL_PROCESS_ATTACH:
            bResult = LibMain(hInstDLL, 0, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            WEP(0);
            break;
        }   // switch

    return (bResult);
}   //  DllMain

#endif

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****     Very hard complicated routine.                             **** //
// ************************************************************************ //

int WINAPI LibMain (
    HINSTANCE   hInst,
    WORD        wDataSeg,
    WORD        wHeapSize,
    LPSTR       lpszCmdLine
    )
{
   hInstance = hInst;

#ifdef SYM_WIN16

   if (wHeapSize > 0)
      UnlockData (0);

#endif
 
    NG_VListRegister();

                                        // ----------------------------------
                                        // Register our bitmap class.
                                        // ----------------------------------
    NAVBITMAP_CtlRegister ( hInstance );
    
   return (1);
}

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****    Extremelly difficult procedure to write.                    **** //
// ************************************************************************ //

int SYM_EXPORT WINAPI WEP (
    int     nParam
    )
{
    return ( TRUE );                    // Return success to windows
}

#endif

