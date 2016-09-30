// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/report.cpv   1.5   19 Aug 1996 10:11:00   DCHI  $
//
// Description:
//  Reporting of viruses and errors to standard out and log file.
//
// Contains:
//  ReportVirusInMemory()
//  ReportFileVirusAndAction()
//  ReportError()
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/report.cpv  $
// 
//    Rev 1.5   19 Aug 1996 10:11:00   DCHI
// For Win32, use WriteFile() in ReportOutputString() instead of fprintf().
// 
//    Rev 1.4   12 Jun 1996 12:17:12   RAY
// Reorganized boot scanning code
// 
//    Rev 1.3   05 Jun 1996 21:01:36   RAY
// Added boot scanning and repair support
// 
//    Rev 1.2   04 Jun 1996 18:14:46   DCHI
// Modifications to conform to new header file structure.
// 
//    Rev 1.1   22 May 1996 10:58:02   DCHI
// Modified reporting for FILE_ACCESS_ERROR.
// 
//    Rev 1.0   21 May 1996 12:25:46   DCHI
// Initial revision.
// 
//************************************************************************

#include "avtypes.h"

#include "clcb.h"

#include "fileinfo.h"
#include "scanglob.h"
#include "report.h"
#include "avenge.h"

#include "msgstr.h"

//*************************************************************************
//
// Function:
//  ReportOutputString()
//
// Parameters:
//  lpScanGlobal        Pointer to global data
//  lpStr               Pointer to output string
//
// Description:
//  Writes lpStr to standard output and to the log file
//  if OPTION_FLAG_REPORT is set in the command options.
//
// Returns:
//  REPSTATUS_ERROR     On error writing to log file
//  REPSTATUS_OK        On success
//
//*************************************************************************

REPSTATUS ReportOutputString
(
    LPSCANGLOBAL    lpScanGlobal,
    LPSTR           lpStr
)
{
    printf("%s",lpStr);
    if (lpScanGlobal->lpCmdOptions->dwFlags & OPTION_FLAG_REPORT &&
		lpScanGlobal->stReportFileInfo.hFile != NULL)
    {
#if defined(VC20) || defined(SYM_WIN32)

        DWORD   dwNumBytes, dwNumActualBytes;

        dwNumBytes = strlen(lpStr);
        if (WriteFile(lpScanGlobal->stReportFileInfo.hFile,
                      lpStr,
                      dwNumBytes,
                      &dwNumActualBytes,
                      NULL) == FALSE ||
            dwNumActualBytes != dwNumBytes)
            return(REPSTATUS_ERROR);

#else
        int             iResult;

        iResult = fprintf((FILE *)lpScanGlobal->stReportFileInfo.hFile, lpStr);

        if (iResult == EOF)
            return(REPSTATUS_ERROR);

#endif
    }

    return(REPSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  ReportVirusInMemory()
//
// Parameters:
//  lpScanGlobal        Pointer to global data
//  hVirus              Handle to virus detected in memory
//
// Description:
//  Reports a virus detected in memory.
//
// Returns:
//  REPSTATUS_ERROR     On error writing to log file
//  REPSTATUS_OK        On success
//
//*************************************************************************

REPSTATUS ReportVirusInMemory
(
    LPSCANGLOBAL    lpScanGlobal,
    HVIRUS          hVirus
)
{
    char                szOutput[512];
    VIRUS_USER_INFO_T   stVirusUserInfo;

    // Get the name

    if (EngGetUserInfoHVIRUS(lpScanGlobal->hGEng,
                             hVirus,
                             &stVirusUserInfo) == ENGSTATUS_ERROR)
    {
        return(REPSTATUS_ERROR);
    }

    sprintf(szOutput,"Found virus [%s][%04X] in memory.\n",
        stVirusUserInfo.sVirusName,
        stVirusUserInfo.wVirusID);

    return ReportOutputString(lpScanGlobal,szOutput);
}


//*************************************************************************
//
// Function:
//  ReportFileVirusAndAction()
//
// Parameters:
//  lpszPathName        Full path name of infected file
//  lpszFileName        Just the name and extension of the infected file
//  lpScanGlobal        Pointer to global data
//  hVirus              Handle to virus detected in the file
//  dwAction            Bit fields describing action taken on file
//  lpszNewFileName     New full path name of infected file if renamed
//
// Description:
//  Reports a virus detected in a file and the action taken on it.
//
// Returns:
//  REPSTATUS_ERROR     On error
//  REPSTATUS_OK        On success
//
//*************************************************************************

REPSTATUS ReportFileVirusAndAction
(
    LPSTR           lpszPathName,
    LPSTR           lpszFileName,
    LPSCANGLOBAL    lpScanGlobal,
    HVIRUS          hVirus,
    DWORD           dwAction,
    LPSTR           lpszNewFileName
)
{
    char                    szOutput[2*SYM_MAX_PATH];
    char                    szFullFileName[SYM_MAX_PATH];
    VIRUS_USER_INFO_T       stVirusUserInfo;

    (void)lpszFileName;

    sprintf(szFullFileName,"%s",lpszPathName);

    if (hVirus != INVALID_VIRUS_HANDLE)
    {
        if (EngGetUserInfoHVIRUS(lpScanGlobal->hGEng,
                                 hVirus,
                                 &stVirusUserInfo) != ENGSTATUS_OK)
        {
            return(REPSTATUS_ERROR);
        }
    }

    if (dwAction & ACTION_FILE_ACCESS_ERROR)
    {
        sprintf(szOutput,
                gszReportFileAccessError,
                lpszPathName);

        if (ReportOutputString(lpScanGlobal,szOutput) != REPSTATUS_OK)
            return(REPSTATUS_ERROR);
    }
    else
    if (!(dwAction & ACTION_MULTIPLE_INFECTIONS))
    {
        // we're dealing with the first infection. print out the filename

        sprintf(szOutput,
                gszReportInfection,
                szFullFileName,
                stVirusUserInfo.sVirusName,
                stVirusUserInfo.wVirusID);

        if (ReportOutputString(lpScanGlobal,szOutput) != REPSTATUS_OK)
            return(REPSTATUS_ERROR);
    }
    else
    {
        // dealing with a multiple infection

        sprintf(szOutput,
                gszReportMultipleInfection,
                stVirusUserInfo.sVirusName,
                stVirusUserInfo.wVirusID);

        if (ReportOutputString(lpScanGlobal,szOutput) != REPSTATUS_OK)
            return(REPSTATUS_ERROR);
    }

    if (dwAction & ACTION_REPAIRED)
    {
        sprintf(szOutput,
                gszReportRepair,
                stVirusUserInfo.sVirusName);

        if (ReportOutputString(lpScanGlobal,szOutput) != REPSTATUS_OK)
            return(REPSTATUS_ERROR);
    }

    if (dwAction & ACTION_REPAIR_FAILED)
    {
        sprintf(szOutput,
                gszReportFailedRepair,
                stVirusUserInfo.sVirusName);

        if (ReportOutputString(lpScanGlobal,szOutput) != REPSTATUS_OK)
            return(REPSTATUS_ERROR);
    }

    if (dwAction & ACTION_DELETED)
    {
        sprintf(szOutput,
                gszReportDelete);

        if (ReportOutputString(lpScanGlobal,szOutput) != REPSTATUS_OK)
            return(REPSTATUS_ERROR);
    }

    if (dwAction & ACTION_DELETE_FAILED)
    {
        sprintf(szOutput,
                gszReportFailedDelete);

        if (ReportOutputString(lpScanGlobal,szOutput) != REPSTATUS_OK)
            return(REPSTATUS_ERROR);
    }

    if (dwAction & ACTION_RENAMED)
    {
        sprintf(szOutput,
                gszReportRename,
                lpszNewFileName);

        if (ReportOutputString(lpScanGlobal,szOutput) != REPSTATUS_OK)
            return(REPSTATUS_ERROR);
    }

    if (dwAction & ACTION_RENAME_FAILED)
    {
        sprintf(szOutput,
                gszReportFailedRename);

        if (ReportOutputString(lpScanGlobal,szOutput) != REPSTATUS_OK)
            return(REPSTATUS_ERROR);
    }

    if (dwAction & ACTION_NO_ACTION)
    {
        sprintf(szOutput,
                gszReportNoAction);

        if (ReportOutputString(lpScanGlobal,szOutput) != REPSTATUS_OK)
            return(REPSTATUS_ERROR);
    }

    if (dwAction & ACTION_REPAIR_BACKUP_FAILED)
    {
        sprintf(szOutput,
                gszReportRepairBackUpFailed,
                lpszPathName);

        if (ReportOutputString(lpScanGlobal,szOutput) != REPSTATUS_OK)
            return(REPSTATUS_ERROR);
    }

    return(REPSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  ReportBootVirusAndAction()
//
// Parameters:
//  byDriveNum          Infected drive number (A: 0x00, C: 0x80, etc)
//  byPartNum           Infected Boot partition number (0 - 3)
//  lpScanGlobal        Pointer to global data
//  hVirus              Handle to virus detected in the file
//  dwAction            Bit fields describing action taken on file
//
// Description:
//  Reports a virus detected in the boot sector or MBR of a drive and the
//  action taken on it.
//
// Returns:
//  REPSTATUS_ERROR     On error
//  REPSTATUS_OK        On success
//
//*************************************************************************

REPSTATUS ReportBootVirusAndAction
(
    BYTE            byDriveNum,
    BYTE            byPartNum,
    LPSCANGLOBAL    lpScanGlobal,
    HVIRUS          hVirus,
    DWORD           dwAction
)
{
    char                    szOutput[2*SYM_MAX_PATH];
    VIRUS_USER_INFO_T       stVirusUserInfo;

    if (hVirus != INVALID_VIRUS_HANDLE)
        {
        if (EngGetUserInfoHVIRUS(lpScanGlobal->hGEng,
                                 hVirus,
                                 &stVirusUserInfo) != ENGSTATUS_OK)
            {
            return(REPSTATUS_ERROR);
            }
        }

    if ( dwAction & ACTION_MBR )
    {
        sprintf(szOutput,
                gszReportMBRInfection,
                byDriveNum,
                stVirusUserInfo.sVirusName,
                stVirusUserInfo.wVirusID);

        if (ReportOutputString(lpScanGlobal,szOutput) != REPSTATUS_OK)
            return(REPSTATUS_ERROR);
    }
    else if ( dwAction & ACTION_FLOPPY )
    {
        sprintf(szOutput,
                gszReportFloppyInfection,
                (char) ( byDriveNum + 'A' ),
                stVirusUserInfo.sVirusName,
                stVirusUserInfo.wVirusID);

        if (ReportOutputString(lpScanGlobal,szOutput) != REPSTATUS_OK)
            return(REPSTATUS_ERROR);
    }

    else
    {
        sprintf(szOutput,
                gszReportBootInfection,
                byDriveNum,
                byPartNum,
                stVirusUserInfo.sVirusName,
                stVirusUserInfo.wVirusID);

        if (ReportOutputString(lpScanGlobal,szOutput) != REPSTATUS_OK)
            return(REPSTATUS_ERROR);
    }

    if (dwAction & ACTION_REPAIRED)
    {
        sprintf(szOutput,
                gszReportRepair,
                stVirusUserInfo.sVirusName);

        if (ReportOutputString(lpScanGlobal,szOutput) != REPSTATUS_OK)
            return(REPSTATUS_ERROR);
    }

    if (dwAction & ACTION_REPAIR_FAILED)
    {
        sprintf(szOutput,
                gszReportFailedRepair,
                stVirusUserInfo.sVirusName);

        if (ReportOutputString(lpScanGlobal,szOutput) != REPSTATUS_OK)
            return(REPSTATUS_ERROR);
    }

    if (dwAction & ACTION_NO_ACTION)
    {
        sprintf(szOutput,
                gszReportNoAction);

        if (ReportOutputString(lpScanGlobal,szOutput) != REPSTATUS_OK)
            return(REPSTATUS_ERROR);
    }

    return(REPSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  ReportError()
//
// Parameters:
//  lpszPathName        Full path name of infected file
//  lpszPreMessage      Message to go before predefined error messages
//  dwMessage           Constant specifying which predefined error
//                       message to use
//
// Description:
//  Prints lpszPreMessage:dwMessage.  This function is called to report
//  general errors.
//
// Returns:
//  REPSTATUS_ERROR     On error
//  REPSTATUS_OK        On success
//
//*************************************************************************

REPSTATUS ReportError
(
    LPSCANGLOBAL    lpScanGlobal,
    LPSTR           lpszPreMessage,
    DWORD           dwMessage
)
{
	LPSTR			lpstrErrorMsg;
    char            szOutput[512];

    switch (dwMessage)
    {
        case REPORT_ERROR_MEMALLOC:
            lpstrErrorMsg = gszReportErrorMemAlloc;
            break;

        case REPORT_ERROR_MEMFREE:
            lpstrErrorMsg = gszReportErrorMemFree;
            break;

        case REPORT_ERROR_FILEACCESS:
            lpstrErrorMsg = gszReportErrorFileAccess;
            break;

        case REPORT_ERROR_INIT:
            lpstrErrorMsg = gszReportErrorInit;
            break;

        case REPORT_ERROR_CLOSE:
            lpstrErrorMsg = gszReportErrorClose;
            break;

        default:
            lpstrErrorMsg = gszReportErrorDefaultError;
            break;
    }

    if (lpszPreMessage != NULL)
    {
        sprintf(szOutput,"%s:%s",lpszPreMessage,lpstrErrorMsg);
        return ReportOutputString(lpScanGlobal,szOutput);
    }

    // No premessage

    return ReportOutputString(lpScanGlobal,lpstrErrorMsg);
}

//*************************************************************************
//
// Function:
//  ReportBootError()
//
// Parameters:
//  lpszPathName        Full path name of infected file
//  byDriveNum          Drive Number
//  byPartNum           Partition Number
//  dwMessage           Constant specifying which predefined error
//                       message to use
//
// Description:
//  Prints byDriveNum, byPartNum, and dwMessage.  This function is called to
//  report boot/MBR scanning errors.
//
// Returns:
//  REPSTATUS_ERROR     On error
//  REPSTATUS_OK        On success
//
//*************************************************************************

REPSTATUS ReportBootError
(
    LPSCANGLOBAL    lpScanGlobal,
    BYTE            byDriveNum,
    BYTE            byPartNum,
    DWORD           dwMessage
)
{
    LPSTR           lpstrErrorMsg;
    char            szOutput[512];

    switch (dwMessage)
        {
        case ( REPORT_BOOT | REPORT_ERROR_SCAN ):
            lpstrErrorMsg = gszReportBootScanError;
            break;

        case ( REPORT_FLOPPY | REPORT_ERROR_SCAN ):
            lpstrErrorMsg = gszReportFloppyScanError;
            break;

        case ( REPORT_MBR | REPORT_ERROR_SCAN ):
            lpstrErrorMsg = gszReportMBRScanError;
            break;

        default:
            lpstrErrorMsg = gszReportErrorDefaultError;
            break;
        }

    if ( dwMessage & REPORT_BOOT )
        {
        sprintf ( szOutput,
                  "%s: Drive %x, Partition %d!\n",
                  lpstrErrorMsg,
                  byDriveNum,
                  byPartNum );
        }
    else if ( dwMessage & REPORT_FLOPPY )
        {
        sprintf ( szOutput,
                  "%s: Drive %c:!\n",
                  lpstrErrorMsg,
                  (char) ( byDriveNum + 'A' ));
        }
    else
        {
        sprintf ( szOutput,
                  "%s: Drive %x!\n",
                  lpstrErrorMsg,
                  byDriveNum );
        }
    return ReportOutputString(lpScanGlobal,szOutput);
}

