// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/report.h_v   1.2   12 Jun 1996 12:17:10   RAY  $
//
// Description:
//  Reporting definitions and function prototypes.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/report.h_v  $
// 
//    Rev 1.2   12 Jun 1996 12:17:10   RAY
// Reorganized boot scanning code
// 
//    Rev 1.1   05 Jun 1996 21:01:46   RAY
// Added boot scanning and repair support
// 
//    Rev 1.0   21 May 1996 12:27:42   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _REPORT_H

#define _REPORT_H

#define REPSTATUS_OK                    0
#define REPSTATUS_ERROR                 1

typedef unsigned int REPSTATUS;

REPSTATUS ReportVirusInMemory
(
    LPSCANGLOBAL    lpScanGlobal,
    HVIRUS          hVirus
);

#define ACTION_REPAIRED                 0x00000001UL
#define ACTION_REPAIR_FAILED            0x00000002UL
#define ACTION_DELETED                  0x00000004UL
#define ACTION_DELETE_FAILED            0x00000008UL
#define ACTION_RENAMED                  0x00000010UL
#define ACTION_RENAME_FAILED            0x00000020UL
#define ACTION_MULTIPLE_INFECTIONS      0x00000040UL
#define ACTION_NO_ACTION                0x00000080UL
#define ACTION_REPAIR_BACKUP_FAILED     0x00000100UL
#define ACTION_FILE_ACCESS_ERROR        0x00000200UL

#define ACTION_MBR                      0x00001000UL
#define ACTION_BOOT                     0x00002000UL
#define ACTION_FLOPPY                   0x00004000UL

REPSTATUS ReportFileVirusAndAction
(
    LPSTR           lpszPathName,
    LPSTR           lpszFileName,
    LPSCANGLOBAL    lpScanGlobal,
    HVIRUS          hVirus,
    DWORD           dwAction,
    LPSTR           lpszNewFileName
);

REPSTATUS ReportBootVirusAndAction
(
    BYTE            byDriveNum,
    BYTE            byPartNum,
    LPSCANGLOBAL    lpScanGlobal,
    HVIRUS          hVirus,
    DWORD           dwAction
);

#define REPORT_ERROR_MEMALLOC           0x00000001UL
#define REPORT_ERROR_MEMFREE            0x00000002UL
#define REPORT_ERROR_FILEACCESS         0x00000003UL
#define REPORT_ERROR_INIT               0x00000004UL
#define REPORT_ERROR_CLOSE              0x00000005UL
#define REPORT_ERROR_SCAN               0x00000006UL

#define REPORT_MBR                      0x00000100UL
#define REPORT_BOOT                     0x00000200UL
#define REPORT_FLOPPY                   0x00000400UL

REPSTATUS ReportError
(
    LPSCANGLOBAL    lpScanGlobal,
    LPSTR           lpszPreMessage,
    DWORD           dwMessage
);

REPSTATUS ReportBootError
(
    LPSCANGLOBAL    lpScanGlobal,
    BYTE            byDriveNum,
    BYTE            byPartNum,
    DWORD           dwMessage
);

#endif
