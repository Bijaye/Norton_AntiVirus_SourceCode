//************************************************************************
//
// $Header:   S:/NAVEX/VCS/navex15.cpv   1.1   10 Jul 1997 13:34:18   DDREW  $
//
// Description:
//      Contains NAVEX code necessary for non-DOS platforms.
//
// Notes:
//      This file should not be part of the compilation on DOS platforms.
//      This file should NOT contain any code except for what's
//      already here.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/navex15.cpv  $
// 
//    Rev 1.1   10 Jul 1997 13:34:18   DDREW
// If def'ed out several functions not implemented in NLM
// 
//    Rev 1.0   08 May 1997 12:57:54   MKEATIN
// Initial revision.
// 
//    Rev 1.11   29 Apr 1997 17:20:38   DCHI
// Removed #include of symevent.h and removed calls to symevent_get_version.
// 
//    Rev 1.10   29 Apr 1997 11:53:36   DCHI
// Readded MVP initialization.
// 
//    Rev 1.9   24 Apr 1997 17:54:02   DCHI
// Removed initialization for MVP support.
// 
//    Rev 1.8   18 Apr 1997 17:58:04   AOONWAL
// Modified during DEV1 malfunction
// 
//    Rev ABID  16 Apr 1997 19:45:34   DCHI
// Changes so that mvpdef.dat is only loaded from NAV directory.
// 
//    Rev 1.7   07 Apr 1997 18:11:04   DCHI
// Added MVP support.
// 
//    Rev 1.6   26 Dec 1996 15:22:08   AOONWAL
// No change.
// 
//    Rev 1.5   02 Dec 1996 14:02:16   AOONWAL
// No change.
// 
//    Rev 1.4   29 Oct 1996 12:59:04   AOONWAL
// No change.
// 
//    Rev 1.3   30 Jan 1996 15:42:52   DCHI
// Added #ifndef SYM_NLM and #endif pairs to ifdef out functions unused
// on NLM platform.
// 
//    Rev 1.2   01 Nov 1995 10:48:30   DCHI
// Removed include of navexhdr.h due to change in format of navexshr.h.
// 
//    Rev 1.1   16 Oct 1995 13:00:28   DCHI
// Added include of navexhdr.h.
// 
//    Rev 1.0   13 Oct 1995 13:04:50   DCHI
// Initial revision.
// 
//************************************************************************

#include "platform.h"
#include "avtypes.h"
#include "n30type.h"
#include "callbk15.h"
#include "navex15.h"
#include "navexshr.h"                   // Declare shared routines

EXPORT15_TABLE_TYPE export_table =

{
    sizeof(export_table),
    EXTScanFile,
#ifndef SYM_NLM
    EXTScanBoot,
    EXTScanPart,
#else
    NULL,
    NULL,
#endif
    EXTScanMemory,
    EXTRepairFile,
#ifndef SYM_NLM
    EXTRepairBoot,
    EXTRepairPart,
#else
    NULL,
    NULL,
#endif
    EXTStartUp,
    EXTShutDown
};

#if defined(SYM_VXD)

#include "mvp.h"

//---------------------------------------------------------------------------
//
// The value of SYMEVENT_VERSION_REQUIRED_BY_NAVEXVXD is determined by
// Core functionality.  Currently this is version 0x20A.
//
//---------------------------------------------------------------------------
#define SYMEVENT_VERSION_REQUIRED_BY_NAVEXVXD 0x20A

extern DWORD   gdwMVPMagicNum;

extern "C" BOOL WINAPI NAVEXVXD_Dynamic_Init
(
    VOID
)
{
    // Initialize Core

    if ( CoreGlobalInit() )
    {
        PVOID pvCoreInit;

        // Now that this is done, do a local
        //  initialization of Core.

        if ( pvCoreInit = CoreLocalInit() )
        {
            // Initialize MVP.

            MVPLoadData(NULL);

            CoreLocalDeInit( pvCoreInit );

            return ( TRUE );
        }

        if ( !CoreGlobalDeInit() )
        {
            SYM_ASSERT ( FALSE );
        }
    }
    else
    {
        SYM_ASSERT ( FALSE );
    }

    return ( FALSE );
}

extern "C" BOOL WINAPI NAVEXVXD_Dynamic_Exit
(
    VOID
)
{
    PVOID pvCoreInit;

    // Do a local initialization of Core.

    if ( pvCoreInit = CoreLocalInit() )
    {
        // Uninitialize MVP.

        MVPUnloadData();

        CoreLocalDeInit( pvCoreInit );
    }
                                        // Stop using Core.

    if ( !CoreGlobalDeInit() )
    {
        SYM_ASSERT ( FALSE );
    }

    return ( TRUE );
}

#endif



