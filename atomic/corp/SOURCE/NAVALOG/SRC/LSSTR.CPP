//===================================================================
//                                                                
// $Header:   S:/navalog/VCS/lsstr.cpv   1.0   06 Feb 1997 20:55:00   RFULLER  $
//                                                                
// Copyright (c) 1992 Symantec Corporation
//
// All Rights Reserved Worldwide.                                       
//
// DESCRIPTION: String file for Special Banker's Trust utility
//              LASTSCAN.EXE
// 
// AUTHOR:      Keith Mund
//                                                                       
//===================================================================
// $Log:   S:/navalog/VCS/lsstr.cpv  $
// 
//    Rev 1.0   06 Feb 1997 20:55:00   RFULLER
// Initial revision
// 
//    Rev 1.1   10 Apr 1996 14:12:52   jworden
// Port DBCS Changes from Kirin
// 
//    Rev 1.0   30 Jan 1996 15:52:14   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:14:28   BARRY
// Initial revision.
// 
//    Rev 1.0   07 Dec 1994 11:40:58   MFALLEN
// Initial revision.
// 
//    Rev 1.1   16 Jun 1994 17:08:22   DALLEE
// Bumped copyright to 1994.
// 
//    Rev 1.0   16 Mar 1994 14:13:24   KEITH
// Initial revision.
// 
//===================================================================

#include "platform.h"
#include "stddos.h"

TCHAR SZ_EMPTY[] = "";

//===================================================================
// Program name/version and "/?" Help text
//===================================================================

TCHAR SZ_PROGRAM_NAME[]  = "LASTSCAN";
TCHAR SZ_VERSION[]       = "3.0";

VersionRec PROGRAM_NAME =
    {
    PROG_VERSION_FLAG,
    SZ_PROGRAM_NAME,
    SZ_VERSION
    };

TCHAR PRODUCT_COPYRIGHT[] = "Copyright 1994 by Symantec Corporation.";

VersionRec PRODUCT_NAME = 
    { 
    PROD_VERSION_FLAG,
    NULL,
    SZ_VERSION
    };

LPTSTR pszHelpScreenTTY [] =
    {
    "Utility to report days since last full scan by The Norton AntiVirus (TM)",
    "\nUse: LASTSCAN dr:",
    SZ_EMPTY
    };

//===================================================================
// Errors
//===================================================================

TCHAR szCtrlBreakPressedTTY[]    = "\nCtrl+Break pressed";
TCHAR szMoreMemoryRequiredTTY[]  = "\nFree %ld bytes and try again.";
TCHAR szAbortTTY[]               = "\nAborting due to: ";
TCHAR szInsufficientMemoryTTY[]  = "\nNot enough memory to run ";
TCHAR szErrorNavOptsReadTTY[]    = "\nCould not read NAVOPTS.DAT";
TCHAR szErrorActivityReadTTY[]   = "\nA problem was encountered attempting to read %s1.";

//===================================================================
// Success
//===================================================================
TCHAR szDaysElapsedPlurTTY[]     = "%d1 days have elapsed since the last scan of drive %c2:.";
TCHAR szDaysElapsedSingTTY[]     = "%d1 day has elapsed since the last scan of drive %c2:.";
TCHAR szNoHistoryTTY[]           = "There is no history stored for drive %c1:.";

