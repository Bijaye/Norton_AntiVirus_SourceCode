// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APKERNEL/VCS/APKERNEL.H_v   1.8   08 Jun 1998 21:05:48   RStanev  $
//
// Description:
//      This file prototypes NAVAP's Kernel functions.  Some of these
//      functions are exported to other NAVAP modules.
//
// Contains:
//      KernelNotify()
//
//      TaskScanFile()
//      TaskScanBootRecord()
//      TaskScanDevices()                   (SYM_VXD only)
//
//      TaskNotifyFileAttrChange()          (SYM_VXD only)
//      TaskNotifyFileWrite()               (SYM_VXD only)
//      TaskNotifyHDFormat()                (SYM_VXD only)
//      TaskNotifyHDMBRWrite()              (SYM_VXD only)
//      TaskNotifyHDBootWrite()             (SYM_VXD only)
//      TaskNotifyFDBootWrite()             (SYM_VXD only)
//
//      NAVGetMainAction()
//      NAVGetPromptActions()
//
//      BackupFileBeforeRepair()
//      ProcessBackupFileAfterRepair()
//      RepairInfectedFile()
//      QuarantineInfectedFile()
//      MoveInfectedFile()
//      RenameInfectedFile()
//
// See Also:
//      INCLUDE\actions.h
//
// Note:
//      Kernel module
//
//***************************************************************************
// $Log:   S:/APKERNEL/VCS/APKERNEL.H_v  $
// 
//    Rev 1.8   08 Jun 1998 21:05:48   RStanev
// Backup files are now created by quarantine.
//
//    Rev 1.7   16 Mar 1998 14:25:24   DALLEE
// NAVGetMainAction() now takes pointer to sequential list of actions
// to take in response to events.
//
//    Rev 1.6   19 Feb 1998 15:56:48   RStanev
// Added quarantine support.
//
//    Rev 1.5   18 Feb 1998 21:28:56   RStanev
// Removed unknown virus detection.
//
//    Rev 1.4   17 Feb 1998 20:24:48   RStanev
// Updated copyright.
//
//    Rev 1.3   28 May 1997 12:51:40   RStanev
// Prototyped MoveChangedFile().
//
//    Rev 1.2   15 May 1997 01:57:54   RStanev
// File cleaned up and organized.
//
//    Rev 1.1   29 Apr 1997 12:02:46   RStanev
// Fixed compilation errors introduced by AVENGE changes.
//
//    Rev 1.0   26 Feb 1997 16:39:40   RSTANEV
// Initial revision.
//
//***************************************************************************

#ifndef _APKERNEL_H_
#define _APKERNEL_H_

//---------------------------------------------------------------------------
//
// Exported Kernel function prototypes.
//
//---------------------------------------------------------------------------
VOID KernelNotify (
    HEVENTPACKET hEventPacket
    );

//---------------------------------------------------------------------------
//
// Kernel task function prototypes.
//
//---------------------------------------------------------------------------
BOOL TaskScanFile (
    HEVENTPACKET hEventPacket
    );

BOOL TaskScanBootRecord (
    HEVENTPACKET hEventPacket
    );

#if defined(SYM_VXD)

BOOL TaskScanDevices (
    HEVENTPACKET hEventPacket
    );

BOOL TaskNotifyFileAttrChange (
    HEVENTPACKET hEventPacket
    );

BOOL TaskNotifyFileWrite (
    HEVENTPACKET hEventPacket
    );

BOOL TaskNotifyHDFormat (
    HEVENTPACKET hEventPacket
    );

BOOL TaskNotifyHDMBRWrite (
    HEVENTPACKET hEventPacket
    );

BOOL TaskNotifyHDBootWrite (
    HEVENTPACKET hEventPacket
    );

BOOL TaskNotifyFDBootWrite (
    HEVENTPACKET hEventPacket
    );

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Action function prototypes.
//
//---------------------------------------------------------------------------
MAINACTION NAVGetMainAction (
    AVCONTEXT     uAVContext,
    PMAINACTION   puMainActions,
    AVACTION      uLastAVAction,
    PAVACTIONDATA prAVActionData
    );

UINT NAVGetPromptActions (
    AVCONTEXT     uAVContext,
    UINT          uActionsAllowed,
    AVACTION      uLastAVAction,
    PAVACTIONDATA prAVActionData
    );

//---------------------------------------------------------------------------
//
// Kernel helper function prototypes.
//
//---------------------------------------------------------------------------
VOID BackupFileBeforeRepair (
    HEVENTPACKET hEventPacket
    );

VOID ProcessBackupFileAfterRepair (
    HEVENTPACKET hEventPacket,
    BOOL         bSuccess
    );

BOOL RepairInfectedFile (
    HEVENTPACKET hEventPacket,
    PHVLVIRUS    phVLVirus
    );

BOOL QuarantineInfectedFile (
    HEVENTPACKET hEventPacket,
    PHVLVIRUS    phVLVirus
    );

BOOL MoveInfectedFile (
    HEVENTPACKET hEventPacket,
    PHVLVIRUS    phVLVirus
    );

BOOL RenameInfectedFile (
    HEVENTPACKET hEventPacket,
    PHVLVIRUS    phVLVirus
    );

#endif // #ifndef _APKERNEL_H_
