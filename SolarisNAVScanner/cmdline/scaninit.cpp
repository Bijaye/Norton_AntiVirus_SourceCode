// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/SCANINIT.CPv   1.3   23 Oct 1996 11:55:54   DCHI  $
//
// Description:
//  Initialization and closedown for scanning.
//
// Contains:
//  CMDLineScanInit()
//  CMDLineScanClose()
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/SCANINIT.CPv  $
// 
//    Rev 1.3   23 Oct 1996 11:55:54   DCHI
// Added support for IPC callbacks.
// 
//    Rev 1.2   27 Aug 1996 15:18:46   DCHI
// Added NAVEX initialization and closedown calls.
// 
//    Rev 1.1   04 Jun 1996 18:14:44   DCHI
// Modifications to conform to new header file structure.
// 
//    Rev 1.0   21 May 1996 12:25:34   DCHI
// Initial revision.
// 
//************************************************************************

#include "scaninit.h"
#include "clcb.h"

#include "report.h"
#include "msgstr.h"

#include "avenge.h"
#ifdef SYM_UNIX
#include "pamapi.h"
#include <strings.h>
#endif

#include "clnavex.h"

//*************************************************************************
//
// Function:
//  CMDLineScanInit()
//
// Parameters:
//  lpScanGlobal    Pointer to global data
//
// Description:
//  Calls EngGlobalInit() and EngLocalInit().
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL CMDLineScanInit
(
	LPSCANGLOBAL	lpScanGlobal
)
{
    char szPAMFile[SYM_MAX_PATH + 1];
    strcpy(szPAMFile, lpScanGlobal->szHomeDir);
    strcat(szPAMFile, "virscan2.dat");

    // AVENGE global init

    if (EngGlobalInit(&(lpScanGlobal->hGEng),
                      &gstCLCBGeneralCallBacks,
                      &gstCLCBProgressCallBacks,
                      &gstCLCBDataFileCallBacks,
                      &gstCLCBIPCCallBacks,
                      0xDEADBEEFUL) == ENGSTATUS_ERROR)
    {
        ReportError(lpScanGlobal,
                    gszMsgStrGlobalInit,
                    REPORT_ERROR_INIT);

        return(FALSE);
    }

    // AVENGE local init

    if (EngLocalInit(lpScanGlobal->hGEng,
                     &(lpScanGlobal->hLEng),
                     0xDEADBEE0UL) == ENGSTATUS_ERROR)
    {
        ReportError(lpScanGlobal,
                    gszMsgStrLocalInit,
                    REPORT_ERROR_INIT);

        return(FALSE);
    }

#if SYM_UNIX

    // PAM (EMULATOR) global init

    if (PAMGlobalInit(szPAMFile, &lpScanGlobal->hGPAM) != PAMSTATUS_OK)
    {
	fprintf(stderr, "PAMGlobalInit failed\n");
        return(FALSE);
    }

    // PAM (EMULATOR) local init

    if (PAMLocalInit(szPAMFile, NULL, lpScanGlobal->hGPAM, &(lpScanGlobal->hLPAM)) != PAMSTATUS_OK)
     {
	fprintf(stderr, "PAMLocalInit failed\n");
	 return(FALSE);
     }

#endif

    // NAVEX init

    if (CMDLineNAVEXInit(lpScanGlobal) == FALSE)
    {
        // Failed to initialize NAVEX system

        return(FALSE);
    }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  CMDLineScanClose()
//
// Parameters:
//  lpScanGlobal    Pointer to global data
//
// Description:
//  Calls EngLocalClose() and EngGlobalClose().
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL CMDLineScanClose
(
	LPSCANGLOBAL	lpScanGlobal
)
{
    if (EngLocalClose(lpScanGlobal->hLEng) == ENGSTATUS_ERROR)
    {
        ReportError(lpScanGlobal,
                    gszMsgStrLocalClose,
                    REPORT_ERROR_CLOSE);

        return(FALSE);
    }

    if (EngGlobalClose(lpScanGlobal->hGEng) == ENGSTATUS_ERROR)
    {
        ReportError(lpScanGlobal,
                    gszMsgStrGlobalClose,
                    REPORT_ERROR_CLOSE);

        return(FALSE);
    }

#if SYM_UNIX

    if (PAMLocalClose(lpScanGlobal->hLPAM) != 0)
	return(FALSE);

    if (PAMGlobalClose(lpScanGlobal->hGPAM) != PAMSTATUS_OK)
	return(FALSE);

#endif

    if (CMDLineNAVEXClose(lpScanGlobal) == FALSE)
    {
        // Failed to close down NAVEX system

        return(FALSE);
    }

    return(TRUE);
}

