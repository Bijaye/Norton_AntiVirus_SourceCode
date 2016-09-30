// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/cmdline.cpv   1.10   27 Aug 1996 15:16:18   DCHI  $
//
// Description:
//  Command line top level.
//
// Contains:
//  main()
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/cmdline.cpv  $
// 
//    Rev 1.10   27 Aug 1996 15:16:18   DCHI
// #ifdef'd out stklen assignment for SYM_UNIX.
// 
//    Rev 1.9   12 Aug 1996 18:46:50   DCHI
// Modifications for UNIX.
// 
//    Rev 1.8   12 Aug 1996 10:54:32   DCHI
// Corrected error check that compared NULL instead of FALSE return value.
// 
//    Rev 1.7   17 Jun 1996 18:30:36   RAY
// Moved Boot init and close into CMDLineScanBootArea() in scanboot.cpp
// 
//    Rev 1.6   12 Jun 1996 12:25:22   RAY
// Reorganized boot scanning
// 
//    Rev 1.5   05 Jun 1996 21:01:28   RAY
// Added boot scanning and repair support
// 
//    Rev 1.4   03 Jun 1996 17:45:46   DCHI
// Changed total processor time to total time.
// 
//    Rev 1.3   03 Jun 1996 17:20:58   DCHI
// Changes to support building on BORLAND, VC20, SYM_W32, SYM_DOS, and SYM_DOSX.
// 
//    Rev 1.2   22 May 1996 11:04:44   DCHI
// Added call to DisplayOptionsHelp() if /help was specified.
// 
//    Rev 1.1   21 May 1996 12:29:54   DCHI
// Changed #include "virlist.h" to #include "virlista.h".
// 
//    Rev 1.0   21 May 1996 12:25:24   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef SYM_UNIX
#include <dos.h>
#endif // !SYM_UNIX

#include <stdio.h>
#include <string.h>

#ifdef BORLAND
#include <alloc.h>
#endif // BORLAND

#include "scaninit.h"
#include "scanmem.h"
#include "scanfile.h"
#include "scanboot.h"
#include "virlista.h"

#include "cmdparse.h"

#include "pathname.h"
#include "filefind.h"

#include "clcb.h"

#include "report.h"
#include "msgstr.h"

#include "cmdhelp.h"

// Globals

SCANGLOBAL_T    gstScanGlobal;

HGENG           gDebughGEng;

WORD            gwDebugVID;

#ifndef SYM_UNIX
extern unsigned _stklen = 65535u;
#endif // !SYM_UNIX

//*************************************************************************
//
// Function:
//  GetInitialAndHomeDir()
//
// Parameters:
//  lpstrArg0       Argument 0 from the command line
//
// Description:
//  Stores into gstScanGlobal.szInitialDir the directory from which
//  the scanner was invoked.
//
//  Stores into gstScanGlobal.szHomeDir the home directory of the
//  scanner.  This directory is where the VIRSCAN?.DAT files are
//  found.
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL GetInitialAndHomeDir
(
    LPSTR           lpstrArg0
)
{
    int             nPathContents;
    char            szVol[PATHNAME_MAX_VOLUME];
    char            szDir[PATHNAME_MAX_DIR];

    // Initial directory

    if (GetCurrentDirectory(gstScanGlobal.szInitialDir) == FALSE)
        return(FALSE);

    // Home directory

    nPathContents = PathNameSplit(lpstrArg0,szVol,szDir,NULL,NULL);

    if (nPathContents & PATHNAME_HAS_VOLUME)
    {
        if (ChangeVolume(szVol) == FALSE)
        {
            return(FALSE);
        }
    }

    if (nPathContents & PATHNAME_HAS_DIR)
    {
        if (ChangeDirectory(szDir) == FALSE)
        {
            return(FALSE);
        }
    }

    // Now get the full pathname of this directory

    if (GetCurrentDirectory(gstScanGlobal.szHomeDir) == FALSE)
    {
        return(FALSE);
    }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  ReturnToInitialDirs()
//
// Parameters:
//  None
//
// Description:
//  Restores the current working directory on the last scanned volume.
//  The last scanned volume and directory are taken from
//  gstScanGlobal.szLastCWDOnScannedVol.
//
//  Restores the current volume and working directory from which the
//  scanner was invoked.  The volume and working directory are taken
//  from gstScanGlobal.szInitialDir.
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL ReturnToInitialDirs(void)
{
    ChangeFullVolDir(gstScanGlobal.szLastCWDOnScannedVol);

    return ChangeFullVolDir(gstScanGlobal.szInitialDir);
}


//*************************************************************************
//
// Function:
//  CheckAvailableMemory()
//
// Parameters:
//  None
//
// Description:
//  Prints out the amount of memory currently available.
//
// Returns:
//  Nothing
//
//*************************************************************************

void CheckAvailableMemory(void)
{
#ifdef BORLAND
    printf("Memory available: %ld bytes\n",coreleft());
#endif // BORLAND

#if defined(VC20) || defined(SYM_WIN32)
    printf("Memory available function not supported!\n");
#endif // VC20 || SYM_WIN32
}


//*************************************************************************
//
// Function:
//  PrintStats()
//
// Parameters:
//  None
//
// Description:
//  Prints out the numbers of files scanned, number of files infected,
//  and the total time.
//
// Returns:
//  Nothing
//
//*************************************************************************

void PrintStats(void)
{
    printf("\nNumber of files scanned: %lu    Infected: %lu\n",
        gstScanGlobal.dwFilesScanned,gstScanGlobal.dwFilesInfected);

    printf("Total time: %lu seconds\n",
        (unsigned long)difftime(gstScanGlobal.tEndTime,
                                gstScanGlobal.tStartTime));
}


//*************************************************************************
//
// Function:
//  OpenReportFile()
//
// Parameters:
//  None
//
// Description:
//  Checks to see whether the /REPORT command line option was specified.
//  If so, then the function opens a file with the given name and sets
//  gstScanGlobal.stReportFileInfo to the file information for the
//  report file.  If /REPORT was not specified or the file open failed,
//  then gstScanGlobal.stReportFileInfo.hFile is set to NULL.
//
// Returns:
//  Nothing
//
//*************************************************************************

void OpenReportFile(void)
{
    if (gstScanGlobal.lpCmdOptions->dwFlags & OPTION_FLAG_REPORT)
    {
        strcpy(gstScanGlobal.stReportFileInfo.lpstrPathName,
            gstScanGlobal.lpCmdOptions->szReportFile);

        if (CLCBFileOpen(&(gstScanGlobal.stReportFileInfo),
                         FILE_OPEN_WRITE_ONLY) == CBSTATUS_ERROR)
            gstScanGlobal.stReportFileInfo.hFile = NULL;
    }
    else
        gstScanGlobal.stReportFileInfo.hFile = NULL;
}

//*************************************************************************
//
// Function:
//  CloseReportFile()
//
// Parameters:
//  None
//
// Description:
//  Closes the report file and sets gstScanGlobal.stReportFileInfo.hFile
//  to NULL if successful.
//
// Returns:
//  Nothing
//
//*************************************************************************

void CloseReportFile(void)
{
    if (gstScanGlobal.stReportFileInfo.hFile != NULL)
    {
        if (CLCBFileClose(&(gstScanGlobal.stReportFileInfo)) ==
            CBSTATUS_OK)
            gstScanGlobal.stReportFileInfo.hFile = NULL;
    }
}


//*************************************************************************
//
// Function:
//  TerminateHandler()
//
// Parameters:
//  None
//
// Description:
//  Prints "terminating...", closes the report file, and restores
//  the current working directories.
//
// Returns:
//  0
//
//*************************************************************************

int TerminateHandler(void)
{
    printf(gszMsgStrTerminating);
    CloseReportFile();
    ReturnToInitialDirs();
    return 0;
}


//*************************************************************************
//
// Function:
//  main()
//
// Parameters:
//  argc            Number of arguments on command line
//  argv[]          Array of pointers to arguments
//
// Description:
//
// Returns:
//
//*************************************************************************

int main(int argc, char *argv[])
{
    CMDSTATUS                   cmdStatus;
	CMD_OPTIONS_T               stCommandOptions;
    WORD                        wInvalid;
    BOOL                        bContinue = TRUE;

    // Initialize global data

    stCommandOptions.wDebugVID = 0;

    gstScanGlobal.tStartTime = time(NULL);

    gstScanGlobal.stReportFileInfo.hFile = NULL;

    gstScanGlobal.lpCmdOptions = &stCommandOptions;
	gstScanGlobal.dwFilesInfected = 0;
    gstScanGlobal.dwFilesScanned = 0;
    gstScanGlobal.dwEventFlags = NO_EVENT;
    gstScanGlobal.szLastCWDOnScannedVol[0] = '\0';

	cmdStatus = CommandLineParse(argc,
								 argv,
								 &stCommandOptions,
                                 &wInvalid);

	if (cmdStatus != CMDSTATUS_OK)
	{
		switch (cmdStatus)
		{
			case CMDSTATUS_MEM_ERROR:
                ReportError(&gstScanGlobal,gszMsgStrParse,
                    REPORT_ERROR_MEMALLOC);
				break;
			case CMDSTATUS_INVALID_OPTION:
                printf(gszMsgStrInvalidOption,argv[wInvalid]);
				break;
			default:
                printf(gszMsgStrUnknownError);
				break;
		}

        bContinue = FALSE;
	}

    PrintOptions(&stCommandOptions);

    if (stCommandOptions.dwFlags & OPTION_FLAG_HELP)
    {
        DisplayOptionsHelp();
        bContinue = FALSE;
    }

    if (bContinue == TRUE &&
        GetInitialAndHomeDir(argv[0]) == FALSE)
    {
        printf(gszMsgStrErrorGettingHomeDir);
        return(FALSE);
    }

#ifdef BORLAND
    ctrlbrk(TerminateHandler);
#endif // BORLAND

    if (bContinue)
        OpenReportFile();

    if (bContinue && CMDLineScanInit(&gstScanGlobal) == FALSE)
        bContinue = FALSE;

    // AVENGE_DEBUG

    gDebughGEng = gstScanGlobal.hGEng;
    gwDebugVID = stCommandOptions.wDebugVID;

    // Check available memory

    if (stCommandOptions.dwFlags & OPTION_FLAG_MEMAVAIL)
        CheckAvailableMemory();

    // Display virus list

    if (bContinue &&
        (stCommandOptions.dwFlags & OPTION_FLAG_VIRLIST))
    {
        VirusListDisplay(&gstScanGlobal);
        bContinue = FALSE;
    }

    // Remove virus

    if (bContinue &&
        (stCommandOptions.dwFlags & OPTION_FLAG_REMVIR))
    {
        VirusListDeleteVirusVID(&gstScanGlobal);
        bContinue = FALSE;
    }

#ifndef SYM_UNIX

    // Memory scan

    if (bContinue == TRUE &&
        (stCommandOptions.dwFlags & OPTION_FLAG_NOMEM) == 0)
    {
        printf(gszMsgStrScanningMemory);
        if (CMDLineScanMemory(&gstScanGlobal) == FALSE)
        {
            bContinue = FALSE;
        }
    }

    // MBR and Boot scans if necessary

    if ( bContinue == TRUE )
        {
        bContinue = CMDLineScanBootArea ( &gstScanGlobal );
        }

#endif // !SYM_UNIX

    // File scan

    if ((bContinue == TRUE) &&
        (stCommandOptions.dwFlags & OPTION_FLAG_NOFILE) == 0)
    {
        printf(gszMsgStrScanningFiles);
        bContinue = CMDLineScanFiles(&gstScanGlobal);
    }

    if (bContinue && CMDLineScanClose(&gstScanGlobal) == FALSE)
        return 0;

    // Note ending time

    gstScanGlobal.tEndTime = time(NULL);

    FreeCommandLineData(&stCommandOptions);

    CloseReportFile();

    PrintStats();

    ReturnToInitialDirs();

    return 0;
}



