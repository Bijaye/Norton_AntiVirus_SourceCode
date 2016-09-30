// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APEVENT/VCS/MONITOR.H_v   1.4   18 Feb 1998 22:13:08   RStanev  $
//
// Description:
//      This file prototypes the file monitor functions.  Some of these
//      functions are exported to other NAVAP modules.
//
// Contains:
//      FileMonitorInit()
//      FileMonitorDeInit()
//      FileMonitorEnable()
//      FileMonitorDisable()
//
//      FileMonitorGetAttributes()
//      FileMonitorSetAttributes()
//      FileMonitorClearAttributes()
//
//      FileMonitorSaveCurrentUserSID()     (SYM_NTK only)
//      FileMonitorQuerySavedUserSID()      (SYM_NTK only)
//
//      FileMonitorGetFileID()              (SYM_NTK only)
//      FileMonitorSetFileID()              (SYM_NTK only)
//
//      FileMonitorTransferData()
//
// See Also:
//      monitor.cpp
//
// Note:
//      Event Manager module.
//
//***************************************************************************
// $Log:   S:/APEVENT/VCS/MONITOR.H_v  $
// 
//    Rev 1.4   18 Feb 1998 22:13:08   RStanev
// Removed unknown virus detection.
//
//    Rev 1.3   17 Feb 1998 16:55:52   RStanev
// Updated copyright.
//
//    Rev 1.2   16 May 1997 01:49:14   RStanev
// Prototyped FileMonitorExchangeInocInfoPtr().
//
//    Rev 1.1   15 May 1997 01:41:26   RStanev
// Cleaned up.
//
//    Rev 1.0   26 Feb 1997 04:34:22   RSTANEV
// Initial revision.
//
//***************************************************************************

#ifndef _MONITOR_H_
#define _MONITOR_H_

//---------------------------------------------------------------------------
//
// File monitor function prototypes.
//
//---------------------------------------------------------------------------
BOOL FileMonitorInit (
    VOID
    );

BOOL FileMonitorDeInit (
    VOID
    );

VOID FileMonitorEnable (
    VOID
    );

VOID FileMonitorDisable (
    VOID
    );

BOOL FileMonitorGetAttributes (
    LPED   lpEventData,
    LPUINT lpuAttributes
    );

inline BOOL FileMonitorGetAttributes (
    HEVENTPACKET hEventPacket,
    LPUINT       lpuAttributes
    )
    {
    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );

    return ( FileMonitorGetAttributes ( hEventPacket->prEDExtra->prSEEventData,
                                        lpuAttributes ) );
    }

BOOL FileMonitorSetAttributes (
    LPED lpEventData,
    UINT uAttributes
    );

inline BOOL FileMonitorSetAttributes (
    HEVENTPACKET hEventPacket,
    UINT         uAttributes
    )
    {
    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );

    return ( FileMonitorSetAttributes ( hEventPacket->prEDExtra->prSEEventData,
                                        uAttributes ) );
    }

BOOL FileMonitorClearAttributes (
    LPED lpEventData,
    UINT uAttributes
    );

#if defined(SYM_NTK)

BOOL FileMonitorSaveCurrentUserSID (
    LPED lpEventData
    );

DWORD FileMonitorQuerySavedUserSID (
    LPED lpEventData,
    PSID pSID
    );

BOOL FileMonitorGetFileID (
    LPED    lpEventData,
    __int64 *pi64FileID
    );

BOOL FileMonitorSetFileID (
    LPED    lpEventData,
    __int64 i64FileID
    );

#endif // #if defined(SYM_NTK)

BOOL FileMonitorTransferData (
    LPED  lpEventData,
    PVOID pvDestination
    );

#endif // #ifndef _MONITOR_H_
