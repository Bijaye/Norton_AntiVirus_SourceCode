// Copyright 1995-1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVINOC/VCS/inocdll.cpv   1.3   27 May 1997 14:50:36   KSACKIN  $
//
// Description:
//
// Contains:
//      DllMain()
//      InoculateInit()
//      InoculateEnd()
//
// See Also:
//************************************************************************
// $Log:   S:/NAVINOC/VCS/inocdll.cpv  $
// 
//    Rev 1.3   27 May 1997 14:50:36   KSACKIN
// Added XAPI.H for SystemGetWindowsType() and system type defines.
//
//    Rev 1.2   20 May 1997 17:38:24   BILL
// Remove intel only calls (certlibn) and fail all calls from NT clients.
//
//    Rev 1.1   24 Mar 1997 16:19:16   DALLEE
// Added file header.
// Removed do-nothing code in InocEnd().
//************************************************************************

#include "platform.h"
#include "ctsn.h"
#include "ctl3d.h"
#include "ngui.h"
#include "options.h"
#include "xapi.h"
#include "inoc.h"

CALLBACKREV1    Init;                   // Required to init certlibn.lib
HINSTANCE       hInstance;

static UINT uNumUsers = 0;


// ---------------------------------------------------------------------------
//                  LOCAL FUNCTION PROTOTYPES
// ---------------------------------------------------------------------------

#if defined(SYM_WIN32)

extern "C" int WINAPI LibMain (
    HINSTANCE   hInst,
    WORD        wDataSeg,
    WORD        wHeapSize,
    LPSTR       lpszCmdLine
    );

extern "C" int SYM_EXPORT WINAPI WEP (
    int         nParam
    );

#endif



#if defined(SYM_WIN32)

/****************************************************************************
   FUNCTION: DllMain

   PURPOSE:  Called when a new process is either using or not using
             this DLL anymore.

*******************************************************************************/
BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD dwReason, LPVOID lpvReserved)
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

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

extern "C" int WINAPI LibMain (
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

    return (1);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

extern "C" int SYM_EXPORT WINAPI WEP (
    int     nParam
    )
{
    return(1);  // Return success to windows //
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID SYM_EXPORT WINAPI InoculateInit (
    VOID
    )
{
#ifdef _ALPHA_
	return;
#else
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return;

    if ( uNumUsers == 0 )
        {
        InitCertlib ( &Init, INIT_CERTLIB_USE_DEFAULT );
        }

    uNumUsers ++;
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID SYM_EXPORT WINAPI InoculateEnd (
    VOID
    )
{
#ifdef _ALPHA_
	return;
#else
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return;

    if ( uNumUsers )
        {
        uNumUsers --;
        }
#endif
}

