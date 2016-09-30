// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/scand/VCS/scaninoc.c_v   1.0   06 Feb 1997 21:08:52   RFULLER  $
//
// Description:
//      These are the functions to scan files and take inoculation
//      information when the Use Inoculation Technology switch is set.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/scand/VCS/scaninoc.c_v  $
// 
//    Rev 1.0   06 Feb 1997 21:08:52   RFULLER
// Initial revision
// 
//    Rev 1.2   08 Aug 1996 13:28:06   JBRENNA
// Ported from DOSNAV environment
// 
// 
//    Rev 1.45   13 Jun 1996 14:04:44   JALLEE
// #if USE_NETWORKALERTS changed to #ifdef USE_NETWORKALERTS.
// 
//    Rev 1.44   17 May 1996 12:02:14   MZAREMB
// Validate infection path (drive letter) before taking callback action.
// 
//    Rev 1.43   04 Mar 1996 18:38:52   MKEATIN
// Ported DX fixes from KIRIN
// 
//    Rev 1.1   31 Jan 1996 16:15:38   MKEATIN
// Some code still sent an array of three strings instead of five to cutstring()
// 
//    Rev 1.0   30 Jan 1996 16:00:08   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:53:22   BARRY
// Initial revision.
// 
//    Rev 1.42.1.0   11 Dec 1995 16:33:28   JREARDON
// Branch base for version LUG1
// 
//    Rev 1.42   06 Feb 1995 13:37:24   DALLEE
// SmartInocOpen() parameter change: Takes full path of file for which we're 
// opening the database -- necessary to distinguish NAVINOC from NAVINOCL?
// 
//    Rev 1.41   29 Dec 1994 16:46:30   DALLEE
// Include syminteg.h before nonav.h
// 
//    Rev 1.40   28 Dec 1994 14:15:50   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.3   27 Dec 1994 19:36:26   DALLEE
    // Convert to use CERTLIBN.
    //
    //    Rev 1.2   12 Dec 1994 18:11:02   DALLEE
    // Commented out network alerts and password protection.
    //
    //    Rev 1.1   05 Dec 1994 18:10:20   DALLEE
    // CVT1 script.
    //
//    Rev 1.39   28 Dec 1994 13:53:24   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   23 Nov 1994 15:38:32   DALLEE
    // Initial revision.
    //
    //    Rev 1.35   19 Sep 1993 07:07:14   barry
    // Changed == to = in assignment
//************************************************************************

#include "platform.h"
#include "stddos.h"
#include "stddlg.h"
# include "disk.h"
#include "ctsn.h"
#include "virscan.h"

#include "navdprot.h"
#include "options.h"
#include "navutil.h"
#include "syminteg.h"
#include "nonav.h"
#include "inoc.h"

#include "scand.h"


//************************************************************************
// DEFINES
//************************************************************************

#define INOC_ONE_BUTTON         0
#define INOC_ALL_BUTTON         1
#define INOC_CANCEL_BUTTON      2
#define INOC_HELP_BUTTON        3


//************************************************************************
// LOCAL VARIABLES
//************************************************************************

STATIC 	DIALOG_RECORD   *InocDialog;
STATIC 	char            LastDrive;

//************************************************************************
// EXTERNAL VARIABLES
//************************************************************************

extern LPNONAV  glpNoNav;


//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

VOID LOCAL PASCAL InocAllFiles (LPNONAV lpNoNav, BOOL bReInoc);

BOOL STATIC PASCAL InocAllFilesCallback( LPINFECTEDFILE lpInfected,
                                         LPNONAV        lpNoNav,
                                         WORD           wEntry,
                                         LPARAM         dwReInoc  );


//************************************************************************
// ScanFoundInocDialog()
//
// This routine displays the Problems Found inoculate dialog.
//
// Parameters:
//      None.
//
// Returns:
//      ACCEPT_DIALOG                   File(s) inoculated.
//      ABORT_DIALOG                    Cancel or ESC hit.
//      FALSE                           Couldn't read INFECTEDFILE info.
//************************************************************************
// 4/13/93 DALLEE, Function created.
//************************************************************************

WORD PASCAL ScanFoundInocDialog (VOID)
{
    extern  ListRec         lrScanFoundItems;

    extern  char            *LPSZ_SCAN_INOCULATE_BUTTONS_LABELS [];
    extern  char            *LPSZ_SCAN_INOCULATE_WHAT_FILES     [];
    extern  char            *LPSZ_SCAN_REINOCULATE_WHAT_FILES   [];
    extern  char            SZ_FILEINOCULATED                   [];
    extern  char            SZ_FILENOTINOCULATED                [];
    extern  char            SZ_SCAN_INOCULATE_TITLE             [];
    extern  DIALOG          dlScanAction;
    extern  ButtonsRec      buScanActionButtons;

    auto    DIALOG_RECORD   *lpdrDialog;
    auto    WORD            wResult = FALSE;
    auto    INFECTEDFILE    rInfected;
    auto    BOOL            bDone = FALSE;
    auto    BOOL            bReInoc;
    auto    DWORD           dwEntry;
    auto    WORD            wOldHelp;
    auto    LPSTR           *lpszQuestion;

    auto    char            lpMessageBuffer [SCAN_PROMPT_MESSAGE_LINES]
                                            [SCAN_PROMPT_MESSAGE_WIDTH + 1];
    auto    char            *lpszMessage [SCAN_PROMPT_MESSAGE_LINES] =
                                {
                                lpMessageBuffer[0],
                                lpMessageBuffer[1],
                                lpMessageBuffer[2],
                                lpMessageBuffer[3],
                                lpMessageBuffer[4]
                                };

    // OK ask for password every time since user is driving here...

#ifdef USE_PASSWORD    //&?
    if (!AskForPassword(PASSWORD_INOCFILES))
        {
        return (ABORT_DIALOG);
        }
#endif

    dwEntry = lrScanFoundItems.active_entry;

    if (GetInfectedFile(&rInfected, glpNoNav, dwEntry))
        {
        bReInoc = (rInfected.wStatus == FILESTATUS_INOCULATEDCHANGED);

        lpszQuestion = (bReInoc ? LPSZ_SCAN_REINOCULATE_WHAT_FILES :
                                  LPSZ_SCAN_INOCULATE_WHAT_FILES);

        ScanFoundCreateMessage(lpszMessage, &rInfected);

        buScanActionButtons.strings = LPSZ_SCAN_INOCULATE_BUTTONS_LABELS;
        dlScanAction.title          = SZ_SCAN_INOCULATE_TITLE;

        lpdrDialog = DialogOpen2( &dlScanAction,
                                  lpszMessage[0],
                                  lpszMessage[1],
                                  lpszMessage[2],
                                  lpszMessage[3],
                                  lpszMessage[4],
                                  lpszQuestion[0],
                                  lpszQuestion[1] );

        wOldHelp = HyperHelpTopicSet(HELP_DLG_ASK_ACTION);

        do
            {

            wResult = DialogProcessEvent(lpdrDialog, DialogGetEvent());

            if (wResult == ABORT_DIALOG)
                {
                bDone = TRUE;
                }
                                        // Check for a button hit.
            if (wResult == ACCEPT_DIALOG)
                {
                LastDrive = rInfected.szFullPath[0];

                SmartInocOpen(rInfected.szFullPath, TRUE);

                switch (buScanActionButtons.value)
                    {
                    case INOC_ONE_BUTTON:
                        wResult = InocFile(rInfected.szFullPath);

                        rInfected.wStatus = UpdateFileStatus(rInfected.wStatus,
                                                             PROCESS_INOCULATE,
                                                             wResult);
                        ScanActionMessage(&rInfected);

                        PutInfectedFile(&rInfected, glpNoNav, dwEntry);
                        wResult = ACCEPT_DIALOG;
                        bDone = TRUE;
                        break;

                    case INOC_ALL_BUTTON:
                        InocAllFiles(glpNoNav, bReInoc);
                        bDone = TRUE;
                        break;

                    case INOC_CANCEL_BUTTON:
                        wResult = ABORT_DIALOG;
                        bDone = TRUE;
                        break;

                    case INOC_HELP_BUTTON:
                        HyperHelpDisplay();
                        break;
                    }
                SmartInocClose();
                }
            } while (!bDone);
        DialogClose(lpdrDialog, wResult);
        HyperHelpTopicSet(wOldHelp);
        }

    return (wResult);
} // End ScanFoundInocDialog()


//************************************************************************
// InocAllFiles()
//
// This routine goes through the list of infected files in the NONAV
// struct and attempts to inoculate any with status FILESTATUS_NOTINOCULATED.
//
// Parameters:
//      LPNONAV lpNoNav                 Has the linked list of files.
//
// Returns:
//      Nothing
//************************************************************************
// 4/13/93 DALLEE, Function created.
//************************************************************************

VOID LOCAL PASCAL InocAllFiles (LPNONAV lpNoNav, BOOL bReInoc)
{
    extern   	DIALOG		dlFileProgress;
    extern      char    	FAR szFileInoculInProgress[];
    extern      char    	FAR szInocAllTitle[];
    extern      OutputFieldRec 	ofScanDirectory;    // for file name.
    extern      OutputFieldRec 	ofScanPercentBar;   // % bar
    extern      ButtonsRec 	buScanStop;
    extern      OutputFieldRec 	ofProgressStatus;
   
    auto    DWORD                   dwDelayStart;

    dlFileProgress.title  = szInocAllTitle;
    ofProgressStatus.data = szFileInoculInProgress;

    InocDialog = DialogOpen2(&dlFileProgress,
			 &ofScanPercentBar,             // % percent bar
			 &ofProgressStatus,
			 &ofScanDirectory,		// file name.
			 &buScanStop);			// stop button

    TimerDelayBegin ( & dwDelayStart ); // Avoid "flash" if error or short
    ReadAllInfectedFiles(lpNoNav,
                         InocAllFilesCallback,
                         (LPARAM) bReInoc);

    StdDlgUpdateFillBar(&ofScanPercentBar,1,1);
    TimerDelayEnd ( 1, dwDelayStart );
    DialogClose(InocDialog, ACCEPT_DIALOG);
} // End InocAllFiles()

//************************************************************************
// InocAllFilesCallback()
//
// This routine is called for each uninoculated file found in a scan
// when Inoculate All is chosen from the Problems Found dialog.
// NOTE: There is no need to make a call to PutInfectedFile to update
// the information, since lpInfected passed by ReadAllInfectedFiles
// points to the actual INFECTEDFILE structure which is locked in memory.
//
// Parameters:
//      LPINFECTEDFILE  lpInfected      Infected file information.
//      LPNONAV         lpNoNav         Has scan information.
//      WORD            wEntry          Index of INFECTEDFILE in list.
//      LPARAM          lParam          ListRec * for list to which we append.
//
// Returns:
//      TRUE                            Tells ReadAllInfectedFiles to keep
//                                          on going.
//************************************************************************
// 4/13/93 DALLEE, Function created.
//************************************************************************

BOOL STATIC PASCAL InocAllFilesCallback(LPINFECTEDFILE  lpInfected,
                                        LPNONAV         lpNoNav,
                                        WORD            wEntry,
                                        LPARAM          dwReInoc )
{
    extern      OutputFieldRec      ofScanDirectory;    // for file name.
    extern      OutputFieldRec      ofScanPercentBar;   // % bar
    extern      ListRec             lrScanFoundItems;
    extern      FILESTATUSSTRUCT    FileStatus [];

    auto	WORD		wEvent;
    auto    BYTE                    byDrive;

    wEvent = DialogProcessEvent(InocDialog,DialogGetEvent());

    if (wEvent == ACCEPT_DIALOG || wEvent == ABORT_DIALOG)
        return(FALSE);

    StdDlgUpdateFillBar(&ofScanPercentBar,wEntry,lrScanFoundItems.num);

    byDrive = lpInfected->szFullPath[0];

    if (( byDrive < 'A') ||             // Invalid path or MBR infected 
        ( byDrive > 'Z'))
        { 
        return (TRUE);
        }
    
    if ( (FileStatus[lpInfected->wStatus].byCanInoc) &&
         (dwReInoc == (lpInfected->wStatus == FILESTATUS_INOCULATEDCHANGED)) )
        {
        DialogOutput(&ofScanDirectory,lpInfected->szFullPath);

        if (LastDrive != byDrive)
            {
            SmartInocClose();
            LastDrive = byDrive;
            SmartInocOpen(lpInfected->szFullPath, TRUE);
            }

        wEvent = InocFile(lpInfected->szFullPath);

        lpInfected->wStatus = UpdateFileStatus(lpInfected->wStatus,
                                               PROCESS_INOCULATE,
                                               wEvent);
        if (FALSE == wEvent)
            {
            ScanActionMessage(lpInfected);
            }
        }

    return (TRUE);
} // End InocAllFilesCallback()


//************************************************************************
// FileInoculated()
//
// This routine checks whether or not a file has been inoculated,
// and also scans it for viruses.
//
// Parameters:
//      LPNONAV lpNav                   File and scan info.
//
// Returns:
//      TRUE                            File was/has now been inoculated or
//                                          is excluded from inoc checking.
//      FALSE                           File not inoculated.
//************************************************************************
// 4/9/93 DALLEE, Taken from Martin's code.
//************************************************************************

BOOL PASCAL FileInoculated (LPNONAV lpNav)
{
    extern  NAVOPTIONS  navOptions;
    UINT    uInocRet;
    UINT    uDiskType;
    UINT    uMinorType;

    lpNav->Infected.Notes.wVirID = (WORD)-1;

                                        // If it's excluded from inoculation
                                        // checks, just return TRUE.
    if ( !FIL4_IS_AT(lpNav->Infected.szFullPath, lpNav->Gen.szExt) ||
         ExcludeIs(&navOptions.exclude,
                   lpNav->Infected.szFullPath,
                   excINOC_CHANGE) )
        {
        return (TRUE);
        }

    DiskGetType(lpNav->Infected.szFullPath[0], &uDiskType, &uMinorType);

    if ((uDiskType == TYPE_REMOVEABLE) && (!lpNav->Inoc.bInocFloppies))
        {
        uInocRet = TRUE;
        }
    else
        {
        if ( lpNav -> uInocOpenOK == SMARTOPEN_RW ||
             lpNav -> uInocOpenOK == SMARTOPEN_RO )
            {
            uInocRet = IsInoculated(lpNav->Infected.szFullPath);
            }
        else
            {
            uInocRet = FALSE;
            }
        }

    switch (uInocRet)
        {
        case FALSE:
            lpNav->Infected.wStatus = FILESTATUS_NOTINOCULATED;
            MEMSET(&lpNav->Infected.Notes, '\0', sizeof(N30NOTES));

            RespondToNotInoc(lpNav);
                                        // Update the status of this file
                                        // and add to the list.
            AddVirusInfo(lpNav);
            uInocRet = (lpNav->Infected.wStatus == FILESTATUS_INOCULATED);
            break;

                                        // The file is already inoculated
                                        // so we're done.
        case TRUE:
            break;

                                        // The file was previously inoculated
                                        // but the Inoculation Information
                                        // has changed.
        case 2:
            lpNav->Infected.wStatus = FILESTATUS_INOCULATEDCHANGED;
            MEMSET(&lpNav->Infected.Notes, '\0', sizeof(N30NOTES));

            RespondToInocChange(lpNav);
                                        // Update the status of this file
                                        // and add to the list.
            AddVirusInfo(lpNav);
            uInocRet = (lpNav->Infected.wStatus == FILESTATUS_INOCULATED);
            break;
        } // End switch (IsInoculated(NULL, lpNav...

   return (uInocRet);
} // End FileInoculated()

