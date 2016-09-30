// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPI/VCS/MEMORY.CPv   1.4   30 Jul 1998 21:51:54   DHERTEL  $
//
// Description: Contains NAVAPI's NAVScanMemory
//
//************************************************************************
// $Log:   S:/NAVAPI/VCS/MEMORY.CPv  $
// 
//    Rev 1.4   30 Jul 1998 21:51:54   DHERTEL
// Changes for NLM packaging of NAVAPI.
// 
//    Rev 1.3   29 Jul 1998 18:33:52   DHERTEL
// DX and NLM changes
// 
//    Rev 1.2   22 Jun 1998 14:47:02   MKEATIN
// We now make sure NULL is not passed as our lphVirus in memory scanning.
//
//    Rev 1.1   17 Jun 1998 17:48:42   MKEATIN
// Now, returning NAV_xxx error codes and check for valid arguments -
// basically valid hEngine's that is.
//
//    Rev 1.0   29 May 1998 13:29:48   MKEATIN
// Initial revision.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "avapi_l.h"
#include "nlm_nav.h"
#include "navapi.h"

//************************************************************************
// HNAVENGINE NAVCALLAPI NAVScanMemory
// (
//     HNAVENGINE       hNAVEngine,   // [in] a valid NAV engine handle
//     HNAVVIRUS*       lphVirus      // [in/out] HNAVVIRUS* to allocate or NULL
// )
//
// This routine scans for viruses in memory. If a virus is found lphVirus will
// reference a virus handle.  If no virus is found lphVirus will reference a
// NULL pointer.
//
// Returns:
//      NAVSTATUS error code
//************************************************************************
// 05/28/98 Created by MKEATIN
//************************************************************************

NAVSTATUS NAVCALLAPI NAVScanMemory
(
    HNAVENGINE       hNAVEngine,   // [in] a valid NAV engine handle
    HNAVVIRUS FAR *  lphVirus      // [in/out] HNAVVIRUS* to allocate or NULL
)
{
    VSTATUS          nStatus;

    // Make sure lphVirus is not NULL

    if (!lphVirus) return NAV_INVALID_ARG;

    // lphVirus will reference NULL unless a virus is found

    *lphVirus = NULL;

#ifdef SYM_WIN32

    //  Don't scan memory on NT

    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    {
        return 1;
    }

#endif

    // We only scan low memory here (0-640k). Some machines lock up when we
    // scan high memory, so high memory scanning was removed from our recent
    // releases.

    nStatus = VLScanMemory((HVCONTEXT)hNAVEngine,
                                   0,
                                   lphVirus);


    switch (nStatus)
    {
        case VS_OK:

            return NAV_OK;

        case VS_MEMALLOC:

            return NAV_MEMORY_ERROR;

        case VS_INVALID_ARG:

            return NAV_INVALID_ARG;

        default:

            return NAV_ERROR;
    }

}

