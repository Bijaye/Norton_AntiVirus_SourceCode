// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/msgstr.h_v   1.2   12 Jun 1996 12:17:06   RAY  $
//
// Description:
//  Message strings header.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/msgstr.h_v  $
// 
//    Rev 1.2   12 Jun 1996 12:17:06   RAY
// Reorganized boot scanning code
// 
//    Rev 1.1   05 Jun 1996 21:01:42   RAY
// Added boot scanning and repair support
// 
//    Rev 1.0   21 May 1996 12:27:22   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _MSGSTR_H

#define _MSGSTR_H

// CMDLINE.CPP

extern char gszMsgStrTerminating[];
extern char gszMsgStrParse[];
extern char gszMsgStrInvalidOption[];
extern char gszMsgStrUnknownError[];
extern char gszMsgStrErrorGettingHomeDir[];
extern char gszMsgStrScanningMemory[];
extern char gszMsgStrScanningMBR[];
extern char gszMsgStrScanningBoot[];
extern char gszMsgStrScanningFiles[];

// SCANFILE.CPP

extern char gszMsgStrScanFileInit[];
extern char gszMsgStrScanFileClose[];

extern char gszMsgStrScanningDirectory[];

// SCANMEM.CPP

extern char gszMsgStrScanMemoryInit[];
extern char gszMsgStrScanMemoryClose[];

// SCANBOOT.CPP

extern char gszMsgStrScanBootInit[];
extern char gszMsgStrScanBootClose[];

extern char gszReportBootInfection[];
extern char gszReportMBRInfection[];
extern char gszReportBootScanError[];
extern char gszReportFloppyScanError[];
extern char gszReportFloppyInfection[];
extern char gszReportMBRScanError[];

// SCANINIT.CPP

extern char gszMsgStrGlobalInit[];
extern char gszMsgStrGlobalClose[];

extern char gszMsgStrLocalInit[];
extern char gszMsgStrLocalClose[];

// VIRLIST.CPP

extern char gszMsgStrVirusName[];
extern char gszMsgStrVID[];
extern char gszMsgStrSize[];
extern char gszMsgStrInfects[];

extern char gszMsgStrErrorAccessingVirusList[];

extern char gszMsgStrFailedVirusDelete[];

extern char gszMsgStrSuccessfulVirusDelete[];

// REPORT.CPP

// Virus found action strings

extern char gszReportInfection[];
extern char gszReportDelete[];
extern char gszReportRepair[];
extern char gszReportFailedRepair[];
extern char gszReportFailedDelete[];
extern char gszReportFailedRename[];
extern char gszReportRename[];
extern char gszReportNoAction[];
extern char gszReportMultipleInfection[];
extern char gszReportRepairBackUpFailed[];
extern char gszReportFileAccessError[];

// Error strings

extern char gszReportErrorMemAlloc[];
extern char gszReportErrorMemFree[];
extern char gszReportErrorFileAccess[];
extern char gszReportErrorInit[];
extern char gszReportErrorClose[];
extern char gszReportErrorDefaultError[];

#endif  // _MSGSTR_H

