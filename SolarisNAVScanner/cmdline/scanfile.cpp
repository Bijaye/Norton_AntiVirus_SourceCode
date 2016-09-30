// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/scanfile.cpv   1.5   27 Aug 1996 15:22:32   DCHI  $
//
// Description:
//  Functions used for scanning files recursively through directories.
//
// Contains:
//  CMDLineScanFiles()
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/scanfile.cpv  $
// 
//    Rev 1.5   27 Aug 1996 15:22:32   DCHI
// Added NAVEX calls for SYM_UNIX.
// 
//    Rev 1.4   12 Aug 1996 18:46:54   DCHI
// Modifications for UNIX.
// 
//    Rev 1.3   12 Aug 1996 10:54:46   DCHI
// Corrected error check that compared NULL instead of FALSE return value.
// 
//    Rev 1.2   04 Jun 1996 18:14:52   DCHI
// Modifications to conform to new header file structure.
// 
//    Rev 1.1   31 May 1996 14:30:22   DCHI
// Changed lpvFileInfo to lpvInfo for file objects.
// 
//    Rev 1.0   21 May 1996 12:25:38   DCHI
// Initial revision.
// 
//************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanfile.h"

#include "filefind.h"
#include "pathname.h"
#include "ext.h"

#include "clcb.h"

#include "report.h"

#include "msgstr.h"

//*************************************************************************
//
// Function:
//  ScanFile()
//
// Parameters:
//  lpScanGlobal    Pointer to global data
//  lpFileObject    File object to scan
//  lpFileInfo      File information for file to scan
//  lpbVirusFound   Pointer to BOOL to store virus found status
//  lphVirus        Pointer to HVIRUS to store virus handle if virus found
//
// Description:
//  Scans a file for file viruses by calling EngScanFile().
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL ScanFile
(
    LPSCANGLOBAL    lpScanGlobal,
    LPFILEOBJECT    lpFileObject,
    LPFILEINFO      lpFileInfo,
    LPBOOL          lpbVirusFound,
    LPHVIRUS        lphVirus
)
{
    if (CLCBFileOpen(lpFileInfo,FILE_OPEN_READ_ONLY) == CBSTATUS_ERROR)
        return(FALSE);

    EngScanFile(lpScanGlobal->hLEng,
        lpFileObject,
        (lpScanGlobal->lpCmdOptions->dwFlags & OPTION_FLAG_SCANWILD) ?
            LENG_SCAN_FILE_FLAG_SCAN_ONLY_WILD :
            LENG_SCAN_FILE_FLAG_SCAN_ALL,
        0,
        lpbVirusFound,
        lphVirus);

#ifdef SYM_UNIX

    if (*lpbVirusFound == 0)
    {
	PAMSTATUS pamResult;
	WORD wVirusID;

	pamResult = PAMScanFile(lpScanGlobal->hLPAM, lpFileInfo->hFile, 0, &wVirusID);
	if (pamResult == PAMSTATUS_VIRUS_FOUND)
	{
            BOOL bFound;

            *lpbVirusFound = TRUE;

            // Get the hVirus

            if (EngVIDToHVIRUS(lpScanGlobal->hGEng,
                               wVirusID,
                               lphVirus,
                               &bFound) != ENGSTATUS_OK ||
                bFound == FALSE)
            {
                // Failed to get the HVIRUS
                return(FALSE);
	    }

	}
    }

#endif

#if 0  // was: #ifdef SYM_UNIX

    if (*lpbVirusFound == 0)
    {
        EXTSTATUS           extResult;
        WORD                wVirusID;

        // Try NAVEX

        extResult = lpScanGlobal->stNAVEXFunctionTable.
            EXTScanFile(lpScanGlobal->lpNAVEXCallBacks,
                        lpFileInfo->lpstrPathName,
                        lpFileInfo->hFile,
                        (LPBYTE)&(lpScanGlobal->hLEng->stAlgScanData),
                        lpScanGlobal->hLEng->byTempBuffer,
                        3,
                        &wVirusID);

        if (CLCBFileClose(lpFileInfo) == CBSTATUS_ERROR)
            return(FALSE);

        if (EXTSTATUS_VIRUS_FOUND == extResult)
        {
            BOOL bFound;

            *lpbVirusFound = TRUE;

            // Get the hVirus

            if (EngVIDToHVIRUS(lpScanGlobal->hGEng,
                               wVirusID,
                               lphVirus,
                               &bFound) != ENGSTATUS_OK ||
                bFound == FALSE)
            {
                // Failed to get the HVIRUS

                return(FALSE);
            }
        }
    }
    else
    {
        if (CLCBFileClose(lpFileInfo) == CBSTATUS_ERROR)
            return(FALSE);
    }

#else // SYM_UNIX

    if (CLCBFileClose(lpFileInfo) == CBSTATUS_ERROR)
        return(FALSE);

#endif // !SYM_UNIX

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  GenerateVirusExtension()
//
// Parameters:
//  lpFileInfo      File information of file to generate extension for
//
// Description:
//  Looks for the first available extension for the file in the following
//  order:
//
//      .VIR
//      .VI#    (where # ranges from 0 through 9)
//      .V##    (where ## ranges from 00 through 99)
//
//  If all extensions are already taken for the given filename,
//  then the function returns error.
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL GenerateVirusExtension
(
    LPFILEINFO      lpFileInfo
)
{
    int             i;
    BOOL            bExists;
    char            szExt[4];

    // Try .VIR

    PathNameChangeExtension(lpFileInfo->lpstrPathName,"VIR");
    if (CLCBFileExists(lpFileInfo,&bExists) == CBSTATUS_ERROR)
        return(FALSE);

    if (bExists == FALSE)
        return(TRUE);

    // Try .VI#

    for (i=0;i<10;i++)
    {
        sprintf(szExt,"VI%d",i);
        PathNameChangeExtension(lpFileInfo->lpstrPathName,szExt);
        if (CLCBFileExists(lpFileInfo,&bExists) == CBSTATUS_ERROR)
            return(FALSE);

        if (bExists == FALSE)
            return(TRUE);
    }

    // Try .V##

    for (i=0;i<100;i++)
    {
        sprintf(szExt,"V%02d",i);
        PathNameChangeExtension(lpFileInfo->lpstrPathName,szExt);
        if (CLCBFileExists(lpFileInfo,&bExists) == CBSTATUS_ERROR)
            return(FALSE);

        if (bExists == FALSE)
            return(TRUE);
    }

    // Failed to find an unused extension

    return(FALSE);
}


//*************************************************************************
//
// Function:
//  RepairFile()
//
// Parameters:
//  lpScanGlobal    Pointer to global data
//  lpFileObject    File object to scan
//  lpFileInfo      File information for file to scan
//  lpstrName       Base name and extension of the file
//  hVirus          The virus handle of the detected virus
//
// Description:
//  Repairs up to MAX_SCAN_REPAIR_TRIES multiple infections on an
//  infected file.
//
//  The following operations are performed:
//  1. Back up the infected file using an extension beginning with .V??.
//  2. Set the infected file's attributes to FILE_ATTR_NORMAL.
//  3. Repair the file.
//  4. If the repair failed, then delete the backup, reset the
//      original attributes of the file and return error.
//  5. Rescan the file.  If the file is no longer infected, then reset
//      the original attributes of the file and return success.
//  6. If the file is multiply infected more than MAX_SCAN_REPAIR_TRIES,
//      then delete the backup, reset the original attributes of the
//      file and return error.
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

#define MAX_SCAN_REPAIR_TRIES       100

BOOL RepairFile
(
    LPSCANGLOBAL    lpScanGlobal,
    LPFILEOBJECT    lpFileObject,
    LPFILEINFO      lpFileInfo,
    LPSTR           lpstrName,
    HVIRUS          hVirus
)
{
	int				i;
    BOOL            bVirusFound;
    BOOL            bFileRepaired;
    FILEINFO_T      stBackUpFileInfo;
    DWORD           dwAction = 0;
    DWORD           dwAttr;
    FILEOBJECT_T    stBackUpFileObject;

    // Create backup file object

    stBackUpFileObject.lpParent = NULL;
    stBackUpFileObject.lpvInfo = &stBackUpFileInfo;
    stBackUpFileObject.lpCallBacks = &gstCLCBFileCallBacks;
    stBackUpFileObject.lpszName = NULL;

    // Get the attributes of the infected file

    if (CLCBFileGetAttr(lpFileInfo,0,&dwAttr) == CBSTATUS_ERROR)
    {
        ReportFileVirusAndAction(lpFileInfo->lpstrPathName,
                                 lpstrName,
                                 lpScanGlobal,
                                 hVirus,
                                 ACTION_REPAIR_BACKUP_FAILED,
                                 NULL);

        return(FALSE);
    }

    // Back up the file first

    strcpy(stBackUpFileInfo.lpstrPathName,lpFileInfo->lpstrPathName);

    if (GenerateVirusExtension(&stBackUpFileInfo) == FALSE ||
        EngCopyFile(lpFileObject,
                    &stBackUpFileObject,
                    lpScanGlobal->byWorkBuffer,
                    SCANGLOB_WORK_BUF_SIZE) == ENGSTATUS_ERROR)
    {
        ReportFileVirusAndAction(lpFileInfo->lpstrPathName,
                                 lpstrName,
                                 lpScanGlobal,
                                 hVirus,
                                 ACTION_REPAIR_BACKUP_FAILED,
                                 NULL);

        return(FALSE);
    }

    // Set the file to be repaired attributes to NORMAL

    if (CLCBFileSetAttr(lpFileInfo,0,FF_ATTRIB_NORMAL) ==
        CBSTATUS_ERROR)
    {
        CLCBFileDelete(&stBackUpFileInfo);

        ReportFileVirusAndAction(lpFileInfo->lpstrPathName,
                                 lpstrName,
                                 lpScanGlobal,
                                 hVirus,
                                 ACTION_REPAIR_BACKUP_FAILED,
                                 NULL);

        return(FALSE);
    }

    // Now attempt repair/rescan iterations

    for (i=0;i<MAX_SCAN_REPAIR_TRIES;i++)
    {
#if 0 // was: #ifdef SYM_UNIX

        EXTSTATUS   extResult;
        N30         stN30;

        if (EngGetN30HVIRUS(lpScanGlobal->hLEng,
                            hVirus,
                            &stN30) == ENGSTATUS_ERROR)
        {
            // Failure getting N30 record

            break;
        }

        extResult = lpScanGlobal->stNAVEXFunctionTable.
            EXTRepairFile(lpScanGlobal->lpNAVEXCallBacks,
                          EXTERNAL_DETECT_VERSION,
                          &stN30,
                          lpFileInfo->lpstrPathName,
                          lpScanGlobal->hLEng->byTempBuffer);

        if (EXTSTATUS_OK == extResult)
        {
            bFileRepaired = TRUE;
        }
        else 
        if (EXTSTATUS_FILE_ERROR == extResult)
        {
            break;
        }
        else

#endif // SYM_UNIX

#if 0 // #ifdef SYM_UNIX
        {
            // Try PAM repair

	    char szPAMFile[SYM_MAX_PATH + 1];
	    PAMSTATUS pamResult;

	    strcpy(szPAMFile, lpScanGlobal->szHomeDir);
	    strcat(szPAMFile, "virscan2.dat");

	    pamResult = PAMRepairFile(lpScanGlobal->hLPAM,
		szPAMFile,
		lpFileInfo->lpstrPathName,
		stBackUpFileInfo.lpstrPathName,
		wVirusID,  // No way to get this from HVIRUS???
		FALSE);

	    if (pamResult != PAMSTATUS_OK)
		break;
	}
#endif

        {
            // Try AVENGE repair

            if (EngRepairFile(lpScanGlobal->hLEng,
                              hVirus,
                              lpFileObject,
                              &bFileRepaired) == ENGSTATUS_ERROR ||
                bFileRepaired == FALSE)
            {
                // Failure during repair

                break;
            }
        }

        // Repair successful

        dwAction |= ACTION_REPAIRED;

        ReportFileVirusAndAction(lpFileInfo->lpstrPathName,
                                 lpstrName,
                                 lpScanGlobal,
                                 hVirus,
                                 dwAction,
                                 NULL);

        // Rescan file

        if (ScanFile(lpScanGlobal,
                     lpFileObject,
                     lpFileInfo,
                     &bVirusFound,
                     &hVirus) == FALSE)
        {
            ReportFileVirusAndAction(lpFileInfo->lpstrPathName,
                                     lpstrName,
                                     lpScanGlobal,
                                     hVirus,
                                     ACTION_FILE_ACCESS_ERROR,
                                     NULL);

            return(TRUE);
        }

        if (bVirusFound == TRUE)
        {
            dwAction |= ACTION_MULTIPLE_INFECTIONS;
        }
        else
        {
            // Set attributes back

            CLCBFileSetAttr(lpFileInfo,0,dwAttr);
            return(TRUE);
        }
    }

    // Outright failed or went through MAX_SCAN_REPAIR_TRIES and still failed

    dwAction &= ~ACTION_REPAIRED;
    dwAction |= ACTION_REPAIR_FAILED;

    ReportFileVirusAndAction(lpFileInfo->lpstrPathName,
                             lpstrName,
                             lpScanGlobal,
                             hVirus,
                             dwAction,
                             NULL);

    // Copy backup over guinea pig

    CLCBFileCopy(&stBackUpFileInfo,lpFileInfo);

    // Delete backup

    CLCBFileDelete(&stBackUpFileInfo);

    // Set the attributes of the copied backup to the original attributes

    CLCBFileSetAttr(lpFileInfo,0,dwAttr);

    return(FALSE);
}


//*************************************************************************
//
// Function:
//  ScanRepairFile()
//
// Parameters:
//  lpScanGlobal    Pointer to global data
//  lpFileObject    File object to scan
//  lpFileInfo      File information for file to scan
//  lpstrName       Base name and extension of the file
//
// Description:
//  Scans the file.  If the file is infected, one of the following
//  actions is taken on the file in this order:
//
//  1. If /REPAIR was specified, repair is attempted.
//  2. If /RENAME was specified, the file is renamed.
//  3. If /DELETE was specified, the file is deleted.
//  4. Do nothing.
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL ScanRepairFile
(
    LPSCANGLOBAL    lpScanGlobal,
    LPFILEOBJECT    lpFileObject,
    LPFILEINFO      lpFileInfo,
    LPSTR           lpstrName
)
{
    HVIRUS          hVirus;
    BOOL            bVirusFound;

    printf("%-12s\r",lpstrName);
    ++lpScanGlobal->dwFilesScanned;

    if (ScanFile(lpScanGlobal,
                 lpFileObject,
                 lpFileInfo,
                 &bVirusFound,
                 &hVirus) == FALSE)
    {
        ReportFileVirusAndAction(lpFileInfo->lpstrPathName,
                                 lpstrName,
                                 lpScanGlobal,
                                 INVALID_VIRUS_HANDLE,
                                 ACTION_FILE_ACCESS_ERROR,
                                 NULL);

        return(TRUE);
    }

    if (bVirusFound == TRUE)
    {
        ++lpScanGlobal->dwFilesInfected;

        // Determine action to take on infected file

        if (lpScanGlobal->lpCmdOptions->dwFlags & OPTION_FLAG_REPAIR)
        {
            // Attempt repair

            return RepairFile(lpScanGlobal,
                              lpFileObject,
                              lpFileInfo,
                              lpstrName,
                              hVirus);
        }
        else
        if (lpScanGlobal->lpCmdOptions->dwFlags & OPTION_FLAG_RENAME)
        {
            FILEINFO_T      stNewFileInfo;

            strcpy(stNewFileInfo.lpstrPathName,lpFileInfo->lpstrPathName);
            if (GenerateVirusExtension(&stNewFileInfo) == FALSE ||
                CLCBFileRename(lpFileInfo,
                               (LPWSTR)stNewFileInfo.lpstrPathName) ==
                    CBSTATUS_ERROR)
            {
                ReportFileVirusAndAction(lpFileInfo->lpstrPathName,
                                         lpstrName,
                                         lpScanGlobal,
                                         hVirus,
                                         ACTION_RENAME_FAILED,
                                         stNewFileInfo.lpstrPathName);

                return(FALSE);
            }

            ReportFileVirusAndAction(lpFileInfo->lpstrPathName,
                                     lpstrName,
                                     lpScanGlobal,
                                     hVirus,
                                     ACTION_RENAMED,
                                     stNewFileInfo.lpstrPathName);
        }
        else
        if (lpScanGlobal->lpCmdOptions->dwFlags & OPTION_FLAG_DELETE)
        {
            // Delete file

            if (CLCBFileSetAttr(lpFileInfo,
                                0,
                                FF_ATTRIB_NORMAL) == CBSTATUS_ERROR ||
                CLCBFileDelete(lpFileInfo) == CBSTATUS_ERROR)
            {
                ReportFileVirusAndAction(lpFileInfo->lpstrPathName,
                                         lpstrName,
                                         lpScanGlobal,
                                         hVirus,
                                         ACTION_DELETE_FAILED,
                                         NULL);

                return(FALSE);
            }

            ReportFileVirusAndAction(lpFileInfo->lpstrPathName,
                                     lpstrName,
                                     lpScanGlobal,
                                     hVirus,
                                     ACTION_DELETED,
                                     NULL);
        }
        else
        {
            ReportFileVirusAndAction(lpFileInfo->lpstrPathName,
                                     lpstrName,
                                     lpScanGlobal,
                                     hVirus,
                                     ACTION_NO_ACTION,
                                     NULL);
        }
    }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  RecursiveScan()
//
// Parameters:
//  lpScanGlobal    Pointer to global data
//  lpstrPath       The volume:directory to scan
//  lpstrFileSpec   Specification of files to scan
//  bRecurse        Whether to recurse down subdirectories
//
// Description:
//  Scans/repairs the files matching the file specification in the
//  given directory.
//
//  The function scans all files in the current directory being
//  scanned then iterates through each subdirectory calling itself
//  recursively if bRecurse is TRUE.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void RecursiveScan
(
    LPSCANGLOBAL    lpScanGlobal,
    LPSTR           lpstrPath,
    LPSTR           lpstrFileSpec,
    BOOL            bRecurse
)
{
    LPFILEFINDDATA  lpFindData;
    BOOL            bMore;
    LPSTR           lpstrEndPath, lpstrName;
    int             nPathLen;
    char            szDotDot[] = "..";

    lpFindData = (LPFILEFINDDATA)malloc(sizeof(FILEFINDDATA_T));
    if (lpFindData == NULL)
    {
        ReportError(lpScanGlobal,NULL,REPORT_ERROR_MEMALLOC);
        return;
    }

    // Point to end

    nPathLen = strlen(lpstrPath);
    lpstrEndPath = lpstrName = lpstrPath + nPathLen;

    printf(gszMsgStrScanningDirectory,lpstrPath);

    //////////////////////////////////////////////////////////////////
    //  Scan files in the directory first
    //////////////////////////////////////////////////////////////////

    if (FindFirstFile(lpstrFileSpec,lpFindData) == FALSE)
        bMore = FALSE;
    else
    {
        // Found a match

        bMore = TRUE;
    }

    while (bMore)
    {
        if (!(lpFindData->dwFileAttributes &
            (FF_ATTRIB_LABEL | FF_ATTRIB_DIREC)) &&
            ((lpScanGlobal->lpCmdOptions->dwFlags & OPTION_FLAG_DOALL) ||
             ValidScanExtension(lpFindData->szFileName) == TRUE))
        {
            // File

            strcpy(lpstrName,lpFindData->szFileName);
            strcpy(lpScanGlobal->stFileInfo.lpstrPathName,
                lpstrPath);
            ScanRepairFile(lpScanGlobal,
                           &(lpScanGlobal->stFileObject),
                           &(lpScanGlobal->stFileInfo),
                           lpFindData->szFileName);
        }

        bMore = FindNextFile(lpFindData);
    }

    CloseFileFind(lpFindData);


    //////////////////////////////////////////////////////////////////
    //  Recurse if possible
    //////////////////////////////////////////////////////////////////

    if (!bRecurse)
    {
        *lpstrEndPath = '\0';
        free(lpFindData);
		return;
    }

    if (FindFirstFile("*.*",lpFindData) == FALSE)
    {
        CloseFileFind(lpFindData);
        free(lpFindData);
        return;
    }

    // Found a match

    bMore = TRUE;
    while (bMore)
    {
        if ((lpFindData->dwFileAttributes & FF_ATTRIB_DIREC) &&
            strcmp(lpFindData->szFileName,".") &&
            strcmp(lpFindData->szFileName,szDotDot))
        {
            // Directory

            if (ChangeDirectory(lpFindData->szFileName) == TRUE)
            {
                sprintf(lpstrName,"%s%c",
                    lpFindData->szFileName,DIR_SEPARATOR);
                RecursiveScan(lpScanGlobal,lpstrPath,
                    lpstrFileSpec,bRecurse);
                ChangeDirectory(szDotDot);
            }
        }

        bMore = FindNextFile(lpFindData);
    }

    CloseFileFind(lpFindData);

    *lpstrEndPath = '\0';
    free(lpFindData);
}


//*************************************************************************
//
// Function:
//  CMDLineScanObject()
//
// Parameters:
//  lpScanGlobal    Pointer to global data
//  lpstrPathName   Object to scan
//  bRecurse        Whether to recurse down subdirectories
//
// Description:
//  Scans/repairs the files matching the file specification in the
//  object from the command line.
//
//  The function decomposes the object into a volume:directory and
//  file specification using the logic described next.
//
//  If the object ends with either a forward slash or a backward
//  slash, then the object is a directory and the file specification
//  is *.*.
//
//  If the object has wildcard (* or ?) characters in the name or
//  extension part, then that forms the file specification and the
//  part of the object before the name and extension is the directory.
//
//  If the object is just a volume, then the current directory on
//  that volume is scanned.
//
//  If the object is a directory, then that directory is scanned.
//
//  If the object is a file, then that file is scanned.  If bRecurse
//  is true, then all files matching that file name in all
//  subdirectories of the directory to scan are scanned.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void CMDLineScanObject
(
    LPSCANGLOBAL    lpScanGlobal,
    LPSTR           lpstrPathName,
    BOOL            bRecurse
)
{
    int             nLen, nPathContents;
    char            lpszPath[SYM_MAX_PATH+1];
    char            lpszName[SYM_MAX_PATH+1];
    char            szVol[PATHNAME_MAX_VOLUME+1];
    char            szDir[PATHNAME_MAX_DIR+1];

    if (lpstrPathName == NULL || lpstrPathName[0] == '\0')
        return;

    // Change to the initial directory first

    if (ChangeFullVolDir(gstScanGlobal.szInitialDir) == FALSE)
        return;

    nLen = strlen(lpstrPathName);

    strcpy(lpszPath,lpstrPathName);

    // Get the name part

    PathNameGetName(lpszPath,lpszName);

    // See if the name has wildcards

    if ((lpstrPathName[nLen-1] == '\\' || lpstrPathName[nLen-1] == '/') ||
        PathNameHasWildCard(lpszName) == FALSE &&
        (PathNameIsVolume(lpstrPathName) == TRUE ||
		 PathNameIsDirectory(lpstrPathName) == TRUE))
    {
        // Directory or volume name, go for it

        if (lpstrPathName[nLen-1] == '\\' || lpstrPathName[nLen-1] == '/' ||
            PathNameIsVolume(lpstrPathName))
            strcpy(lpszPath,lpstrPathName);
        else
            sprintf(lpszPath,"%s%c",lpstrPathName,DIR_SEPARATOR);

        strcpy(lpszName,"*.*");
    }

    // Change to the target directory

    nPathContents = PathNameSplit(lpszPath,szVol,szDir,NULL,NULL);

    if (nPathContents & PATHNAME_HAS_VOLUME)
    {
        if (ChangeVolume(szVol) == FALSE)
            return;
    }

    // Get the full pathname of the current directory on this volume
    //  in order to restore it after scanning

    if (GetCurrentDirectory(lpScanGlobal->szLastCWDOnScannedVol) == FALSE)
    {
        return;
    }

    if (nPathContents & PATHNAME_HAS_DIR)
    {
        if (ChangeDirectory(szDir) == FALSE)
            return;
    }

    // Now get the full pathname of this directory

    if (GetCurrentDirectory(lpszPath) == FALSE)
    {
        return;
    }

    // Go for it

    RecursiveScan(lpScanGlobal,lpszPath,lpszName,bRecurse);

    // Change back to the original current directory on this volume

    ChangeFullVolDir(lpScanGlobal->szLastCWDOnScannedVol);
}


//*************************************************************************
//
// Function:
//  CMDLineScanFiles()
//
// Parameters:
//  lpScanGlobal    Pointer to global data
//
// Description:
//  Iterates through linked list of objects to scan and calls
//  CMDLineScanOjbect() for each.
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL CMDLineScanFiles
(
    LPSCANGLOBAL    lpScanGlobal
)
{
    LPSCAN_NODE     lpScanNode;

    // Set up the file object

    lpScanGlobal->stFileObject.lpParent = NULL;
    lpScanGlobal->stFileObject.lpvInfo =
		&lpScanGlobal->stFileInfo;
    lpScanGlobal->stFileObject.lpCallBacks = &gstCLCBFileCallBacks;
    lpScanGlobal->stFileObject.lpszName = NULL;

    // First initialize scanning data

    if (EngGlobalFileInit(lpScanGlobal->hGEng,
        (lpScanGlobal->lpCmdOptions->dwFlags & OPTION_FLAG_SCANWILD) ?
            GENG_INIT_FILE_FLAG_LOAD_WILD :
            GENG_INIT_FILE_FLAG_LOAD_ALL) == ENGSTATUS_ERROR)
    {
        ReportError(lpScanGlobal,
                    gszMsgStrScanFileInit,
                    REPORT_ERROR_INIT);

        return(FALSE);
    }

    lpScanNode = lpScanGlobal->lpCmdOptions->lpScanList;
    while (lpScanNode != NULL)
    {
        CMDLineScanObject(lpScanGlobal,
            lpScanNode->lpszScanThis,
            (lpScanGlobal->lpCmdOptions->dwFlags & OPTION_FLAG_RECURSE) ?
				TRUE : FALSE);

        lpScanNode = lpScanNode->lpNext;
    }

    // Now close scanning data

    if (EngGlobalFileClose(lpScanGlobal->hGEng) == ENGSTATUS_ERROR)
    {
        ReportError(lpScanGlobal,
                    gszMsgStrScanFileClose,
                    REPORT_ERROR_CLOSE);

        return(FALSE);
    }

    return(TRUE);
}


