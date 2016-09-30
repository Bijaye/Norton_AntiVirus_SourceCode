//************************************************************************
//
// $Header:   S:/ENGINE15/VCS/NAVEX15.CPv   1.3   13 Mar 1998 11:12:32   CNACHEN  $
//
// Description:
//      Contains NAVENG code necessary for non-DOS platforms.
//
// Notes:
//      This file should not be part of the compilation on DOS platforms.
//      This file should NOT contain any code except for what's
//      already here.
//
//************************************************************************
// $Log:   S:/ENGINE15/VCS/NAVEX15.CPv  $
// 
//    Rev 1.3   13 Mar 1998 11:12:32   CNACHEN
// 
//    Rev 1.2   02 Sep 1997 14:44:16   CNACHEN
// Fixed for NAVENG.NLM
// 
//    Rev 1.1   15 May 1997 18:26:32   jsulton
// 
//    Rev 1.0   15 May 1997 15:02:18   jsulton
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

//---------------------------------------------------------------------------
//
// The value of SYMEVENT_VERSION_REQUIRED_BY_NAVENGVXD is determined by
// Core functionality.  Currently this is version 0x20A.
//
//---------------------------------------------------------------------------
#define SYMEVENT_VERSION_REQUIRED_BY_NAVENGVXD 0x20A

extern DWORD   gdwMVPMagicNum;

extern "C" BOOL WINAPI NAVENGVXD_Dynamic_Init
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

extern "C" BOOL WINAPI NAVENGVXD_Dynamic_Exit
(
    VOID
)
{
    PVOID pvCoreInit;

    // Do a local initialization of Core.

    if ( pvCoreInit = CoreLocalInit() )
    {
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



