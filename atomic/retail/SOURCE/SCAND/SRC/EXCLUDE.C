// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/SCAND/VCS/exclude.c_v   1.1   19 Jun 1997 11:20:12   BGERHAR  $
//
// Description:
//      These are the functions for handling exclusions during scanning and
//      from the Problems Found dialog after scanning.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/SCAND/VCS/exclude.c_v  $
// 
//    Rev 1.1   19 Jun 1997 11:20:12   BGERHAR
// Don't increment cleaned count for exclusions (match NAVW)
// 
//    Rev 1.0   06 Feb 1997 21:08:50   RFULLER
// Initial revision
// 
//    Rev 1.2   08 Aug 1996 13:28:08   JBRENNA
// Ported from DOSNAV environment
// 
// 
//    Rev 1.19   04 Mar 1996 18:38:42   MKEATIN
// Ported DX fixes from KIRIN
// 
//    Rev 1.1   31 Jan 1996 16:15:34   MKEATIN
// Some code still sent an array of three strings instead of five to cutstring()
// 
//    Rev 1.0   30 Jan 1996 16:00:08   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:53:22   BARRY
// Initial revision.
// 
//    Rev 1.18.1.0   11 Dec 1995 16:33:36   JREARDON
// Branch base for version LUG1
// 
//    Rev 1.18   29 Dec 1994 16:46:34   DALLEE
// Include syminteg.h before nonav.h
// 
//    Rev 1.17   28 Dec 1994 14:15:50   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.2   27 Dec 1994 19:36:20   DALLEE
    // Convert to use CERTLIBN.
    //
    //    Rev 1.1   05 Dec 1994 18:10:12   DALLEE
    // CVT1 script.
    //
//    Rev 1.16   28 Dec 1994 13:53:22   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   23 Nov 1994 15:38:04   DALLEE
    // Initial revision.
    //
    //    Rev 1.12   09 Sep 1993 07:08:26   DALLEE
    // Set bUpdateExclusions in order to save exclusions on exit.
//************************************************************************

#include "platform.h"
#include "stddos.h"
#include "ctsn.h"

#include "options.h"
#include "navutil.h"
#include "syminteg.h"
#include "nonav.h"
#include "navdprot.h"
#include "exclude.h"

#include "scand.h"


//************************************************************************
// DEFINES
//************************************************************************

#define EXCLUDE_ONE_BUTTON          0
#define EXCLUDE_CANCEL_BUTTON       1
#define EXCLUDE_HELP_BUTTON         2


//************************************************************************
// ScanFoundExlcudeDialog()
//
// This routine displays the Problems Found Exclude dialog.
//
// Parameters:
//      None
//
// Returns:
//      ACCEPT_DIALOG                   File(s) excluded.
//      ABORT_DIALOG                    Cancel or ESC hit.
//************************************************************************
// 3/5/93 DALLEE Function created.
//************************************************************************

WORD PASCAL ScanFoundExcludeDialog (VOID)
{
    extern  LPNONAV         glpNoNav;
    extern  ListRec         lrScanFoundItems;

    extern  char            *LPSZ_SCAN_EXCLUDE_BUTTONS_LABELS   [];
    extern  char            *LPSZ_SCAN_EXCLUDE_WHAT_FILES       [];
    extern  char            SZ_SCAN_EXCLUDE_TITLE               [];
    extern  DIALOG          dlScanAction;
    extern  ButtonsRec      buScanActionButtons;

    auto    DIALOG_RECORD   *lpdrDialog;
    auto    DWORD           dwEntry;
    auto    WORD            wResult;
    auto    INFECTEDFILE    rInfected;
    auto    WORD            wOldHelp;

    auto    char            lpMessageBuffer [SCAN_PROMPT_MESSAGE_LINES]
                                            [SCAN_PROMPT_MESSAGE_WIDTH + 1];
    auto    LPSTR           lpszMessage [SCAN_PROMPT_MESSAGE_LINES] =
                                {
                                lpMessageBuffer[0],
                                lpMessageBuffer[1],
                                lpMessageBuffer[2],
                                lpMessageBuffer[3],
                                lpMessageBuffer[4]
                                };

    dwEntry = lrScanFoundItems.active_entry;

    if (GetInfectedFile(&rInfected, glpNoNav, dwEntry))
        {
        ScanFoundCreateMessage(lpszMessage, &rInfected);

        buScanActionButtons.strings = LPSZ_SCAN_EXCLUDE_BUTTONS_LABELS;
        dlScanAction.title          = SZ_SCAN_EXCLUDE_TITLE;

        lpdrDialog = DialogOpen2( &dlScanAction,
                                  lpszMessage[0],
                                  lpszMessage[1],
                                  lpszMessage[2],
                                  lpszMessage[3],
                                  lpszMessage[4],
                                  LPSZ_SCAN_EXCLUDE_WHAT_FILES[0],
                                  LPSZ_SCAN_EXCLUDE_WHAT_FILES[1]);

        wOldHelp = HyperHelpTopicSet(HELP_DLG_ASK_ACTION);

        do
            {

            wResult = DialogProcessEvent(lpdrDialog, DialogGetEvent());

                                        // Check for a button hit.
            if (wResult == ACCEPT_DIALOG)
                {
                switch (buScanActionButtons.value)
                    {
                    case EXCLUDE_ONE_BUTTON:
                        ScanExcludeInfectedFile(glpNoNav, &rInfected);
                        ScanActionMessage(&rInfected);
                        PutInfectedFile(&rInfected, glpNoNav, dwEntry);
                        break;

                    case EXCLUDE_CANCEL_BUTTON:
                        wResult = ABORT_DIALOG;
                        break;

                    case EXCLUDE_HELP_BUTTON:
                        HyperHelpDisplay();
                        wResult = CONTINUE_DIALOG;
                        break;
                    }
                }
            } while ((wResult != ACCEPT_DIALOG) && (wResult != ABORT_DIALOG));

        DialogClose(lpdrDialog, wResult);
        HyperHelpTopicSet(wOldHelp);
        }

    return (wResult);
} // End ScanFoundExcludeDialog()


//************************************************************************
// ScanExcludeInfectedFile()
//
// This routine excludes the specified file from the type of activity
// that the status flag indicates.
//
// Parameters:
//      LPINFECTEDFILE  lpInfected      Info on file to exclude
//
// Returns:
//      TRUE                            File excluded
//      FALSE                           Not excluded.
//************************************************************************
// 5/12/93 DALLEE, Function created.
//************************************************************************

BOOL PASCAL ScanExcludeInfectedFile (LPNONAV lpNoNav, LPINFECTEDFILE lpInfected)
{
    extern  NAVOPTIONS  navOptions;
    extern  BOOL        bUpdateExclusions;

    auto    UINT        uResult;
    auto    EXCLUDEITEM rExclude;

                                        // Clear the exclude record first.
    MEMSET(&rExclude, '\0', sizeof(EXCLUDEITEM));

    STRCPY(rExclude.szText, lpInfected->szFullPath);

    switch (lpInfected->wStatus)
        {
        case FILESTATUS_NOTINOCULATED:
        case FILESTATUS_INOCULATEDCHANGED:
        case FILESTATUS_INOCULATIONFAILED:
        case FILESTATUS_REINOCULATIONFAILED:
            rExclude.wBits = excINOC_CHANGE;
            break;

        case FILESTATUS_INFECTED:
        case FILESTATUS_NOTREPAIRED:
        case FILESTATUS_NOTDELETED:
            rExclude.wBits = excVIRUS_FOUND;
            break;

                                        // Default means I do not know from
        default:                        // what type of activity to exclude
            return (FALSE);             // the file.
        }

    uResult = ExcludeAdd(&navOptions.exclude, &rExclude);

    lpInfected->wStatus = UpdateFileStatus(lpInfected->wStatus,
                                           PROCESS_EXCLUDE,
                                           uResult);

    bUpdateExclusions = TRUE;           // Remember to save exclusions on exit
    return (uResult);

} // End ScanExcludeInfectedFile()

