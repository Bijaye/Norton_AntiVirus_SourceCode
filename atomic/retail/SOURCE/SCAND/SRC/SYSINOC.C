// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/scand/VCS/sysinoc.c_v   1.0   06 Feb 1997 21:08:56   RFULLER  $
//
// Description:
//      Routines for creating/verifying/repairing the integ entries for
//      boot records and system files.
//
// Contains:
//
// See Also:
//      SYMINTEG project in Quake
//************************************************************************
// $Log:   S:/scand/VCS/sysinoc.c_v  $
// 
//    Rev 1.0   06 Feb 1997 21:08:56   RFULLER
// Initial revision
// 
//    Rev 1.5   29 Oct 1996 16:02:32   BGERHAR
// Removed remaining alert message code that sets up the buffer
// 
//    Rev 1.4   29 Oct 1996 15:30:38   MKEATIN
// Removed alert messages from the lpszSysemInocMessage.
// 
//    Rev 1.3   24 Sep 1996 13:46:24   JALLEE
// Removed alert message from the Sysinteg creation message box.
// 
//    Rev 1.2   08 Aug 1996 15:58:54   JBRENNA
// LuigiPlus Port:
//   YVANG: Modified to use the hard drive number, 0x00 when calling
//     'SystemInocVirusDlg()' in 'SystemInocCheck()', instead of reading
//     *lpNoNav->Infected.szFullPath[0]. (STS#58200, STS#58213)
// 
//   JWORDEN: Pass file name in Infected.szFullOemPath to match shared
//     functions in NAVXUTIL
// 
//   YVANG: Modified to specify the hard drive number when calling
//     'SystemInocVirusDlg()' if the MBR is found infected so that the
//     error message will be consistent with that of NAVW, (i.e. "The
//     master boot record of hard drive #0 is ...") (Bug fix of STS 52513)
// 
//    Rev 1.1   08 Aug 1996 13:26:04   JBRENNA
// Ported from DOSNAV environment
// 
// 
//    Rev 1.32   01 Aug 1996 18:53:08   MZAREMB
// Changed SystemInocCreate to take an extra parameter to be able to display 
// the custom message when creating the inoculation database. 
// 
//    Rev 1.31   30 Jul 1996 12:24:24   JALLEE
// Added custom alert message to MBR / Boot , SYSINTEG, Inoculation dialogs.
// 
//    Rev 1.30   13 Jun 1996 14:04:46   JALLEE
// #if USE_NETWORKALERTS changed to #ifdef USE_NETWORKALERTS.
// 
//    Rev 1.29   29 Nov 1995 22:20:28   MKEATIN
// Fixed dialog messages
// 
//    Rev 1.28   24 Oct 1995 20:42:26   JWORDEN
// Add physical drive parameter to VirusScanMasterBoot ()
// 
//    Rev 1.27   14 Mar 1995 17:46:06   DALLEE
// Hide display during startup util problems are found.
// 
//    Rev 1.26   21 Feb 1995 12:39:06   DALLEE
// Oops!  Cleaned up an #endif.
// 
//    Rev 1.25   21 Feb 1995 11:12:02   DALLEE
// Re-enabled sysinteg.
// See note in SystemInocCheck() for what caused it to break in DOS boxes --
//  Int 21, fn 62 followed by write to 0040:0041.
// 
//    Rev 1.24   14 Feb 1995 21:49:04   DALLEE
// Ack!  Had to comment out sysinteg for one build... still bombing on some
// machines...
// 
//    Rev 1.23   13 Feb 1995 22:59:38   DALLEE
// Expanded size of static rIntegArray buffer.
//     &? Should change so this is allocated dynamically since no more RT-LINK.
// Re-enabled system integrity.    
// 
//    Rev 1.22   28 Dec 1994 14:15:56   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.3   12 Dec 1994 18:10:58   DALLEE
    // Commented out network alerts and password protection.
    //
    //    Rev 1.2   06 Dec 1994 15:44:52   DALLEE
    // Comment out sysinteg.
    //
    //    Rev 1.1   05 Dec 1994 18:10:22   DALLEE
    // CVT1 script.
    //
//    Rev 1.21   28 Dec 1994 13:53:32   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   23 Nov 1994 15:38:44   DALLEE
    //
    //    Rev 1.17   10 Dec 1993 03:08:56   DALLEE
    // Sysinoc error dialogs.
//************************************************************************

#include "platform.h"
#include "file.h"
#include "stddos.h"
#include "stddlg.h"
#include "syminteg.h"
#include "virscan.h"

#include "nonav.h"
#include "scand.h"
#include "navdprot.h"
#ifdef USE_NETWORKALERTS   //&?
 #include "netalert.h"
#endif
#include "tsr.h"
#include "tsrcomm.h"
#include <stdio.h>


//************************************************************************
// DEFINES
//************************************************************************

#define SYSTEM_INOC_MBR     0
#define SYSTEM_INOC_BOOT(x) ((x)+1)

#define SYSTEM_NEW_INOC_YES_BUTTON      0
#define SYSTEM_NEW_INOC_NO_BUTTON       1

#define SYSTEM_INOC_CHANGE_REPAIR       0
#define SYSTEM_INOC_CHANGE_INOCULATE    1
#define SYSTEM_INOC_CHANGE_CONTINUE     2
#define SYSTEM_INOC_CHANGE_STOP         3

#define FIRST_PHYSICAL_DRIVE    0x80


//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

#ifdef USE_NETWORKALERTS   //&?
 VOID LOCAL PASCAL AlertSystemInocChange   (LPNONAV  lpNoNav, LPSTR lpFile);
#endif
VOID LOCAL PASCAL RespondSystemInocNew    (LPNONAV  lpNoNav, LPSTR lpIntegFile);
BOOL LOCAL PASCAL SystemInocCreate        (LPNONAV  lpNoNav, LPSTR lpIntegFile);
BOOL LOCAL PASCAL RespondSystemInocChange (LPNONAV  lpNoNav, LPSTR lpIntegFile);
BOOL LOCAL PASCAL PromptSystemInocChange  (LPNONAV  lpNoNav,
                                           UINT     uIndex,
                                           LPSTR    lpIntegFile,
                                           LPBOOL   lpbIsRebuilt);
VOID LOCAL PASCAL SystemInocVirusDlg      (LPSTR    *lpszDialogStrings,
                                           DWORD    dwTicks,
                                           LPSTR    lpszItem,
                                           LPNONAV  lpNoNav);

//************************************************************************
// STATIC VARIABLES
//************************************************************************

STATIC  INTEG   rIntegArray [24] =
    {
        {0,"MBR80"},
        {0,"BOOT800"},
        {0,"BOOT801"},
        {0,"BOOT802"},
        {0,"BOOT803"},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""},
        {0,""}
    };


//************************************************************************
// SystemInocCheck()
//
// Checks boot records and system files vs. the integrity database.
//
// Parameters:
//      LPNONAV     lpNoNav             Info and options for this scan.
//
// Returns:
//      FALSE                           Stop the current scan.
//      TRUE                            Continue with scan.
//************************************************************************
// 7/19/93 DALLEE, Function created.
//************************************************************************

BOOL PASCAL SystemInocCheck (LPNONAV lpNoNav)
{
    extern  LPSTR       LPSZ_SYSTEM_INOC_MBR_VIRUS  [];
    extern  LPSTR       LPSZ_SYSTEM_INOC_BOOT_VIRUS [];
    extern  LPSTR       LPSZ_SYSTEM_INOC_FILE_VIRUS [];
    extern  char        SZ_HARD_DRIVE_LC [];

    auto    char        szIntegFile [SYM_MAX_PATH];
    auto    BOOL        bContinueScan = TRUE;
    auto    N30NOTES    rNotes;
    auto    WORD        wScanOk;
    auto    LPINTEG     lpArray;
    auto    char        cDrive;
    auto    BOOL        bDoInoc = TRUE;
    auto    DWORD       dwTimedDlgTicks;
    auto    char        szHardDrive [SYM_MAX_PATH];

    IntegGetSystemNames(&rIntegArray[5]);

    // If any system names are on floppy, don't inoculate
    for ( lpArray = &rIntegArray[5] ; *lpArray->byFilename ; lpArray ++ )
        {
        cDrive = *lpArray->byFilename;

        if ( cDrive == 'A' || cDrive == 'B' ||
             cDrive == 'a' || cDrive == 'b' )
            {
            bDoInoc = FALSE;
            break;
            }
        }

    if ( bDoInoc )
        {
        dwTimedDlgTicks = lpNoNav->Alert.bRemoveAfter
                          ? lpNoNav->Alert.uSeconds * TICKS_PER_SECOND
                            : 0;
                                        // Make sure we're not inoculating
                                        // infected stuff...
        TSR_OFF;

        //&? ARG!  Int 21, Fn 62, Get PSP in IntegGetSystemNames
        // can't be followed too closely by a tweak of diskette status byte
        // @ 0040:0041 as done in DiskAbsOperation() called by VSMasterBoot
        // when in a Windows '95 DOS box w/ protected memory. Hence, this delay.
        // Check w/ MS for the reason for this.

        TimerDelay(7);

        if (VirusScanMasterBoot(FIRST_PHYSICAL_DRIVE, &rNotes, &wScanOk)
        &&  !wScanOk)
            {
            SPRINTF (szHardDrive, SZ_HARD_DRIVE_LC, 
                     FIRST_PHYSICAL_DRIVE-0x80);
            SystemInocVirusDlg(LPSZ_SYSTEM_INOC_MBR_VIRUS,
                               dwTimedDlgTicks,
                               szHardDrive,
                               lpNoNav);
            TSR_ON;
            return (bContinueScan);
            }

        if (VirusScanBoot('C', &rNotes, &wScanOk) && !wScanOk)
            {
            SystemInocVirusDlg(LPSZ_SYSTEM_INOC_BOOT_VIRUS,
                               dwTimedDlgTicks,
                               NULL,
                               lpNoNav);
            TSR_ON;
            return (bContinueScan);
            }

        for ( lpArray = &rIntegArray[5];
              *lpArray->byFilename;
              lpArray ++ )
            {
            if (VirusScanFile(lpArray->byFilename, &rNotes, TRUE))
                {
                SystemInocVirusDlg(LPSZ_SYSTEM_INOC_FILE_VIRUS,
                                   dwTimedDlgTicks,
                                   lpArray->byFilename,
                                   lpNoNav);
                TSR_ON;
                return (bContinueScan);
                }
            }
        TSR_ON;

        GetSysIntegFile(szIntegFile, SYM_MAX_PATH, &lpNoNav->Inoc);

        switch (IntegVerify(szIntegFile, rIntegArray))
            {
            case 2:
                RespondSystemInocNew(lpNoNav, szIntegFile);
                break;

            case 3:
                // Mem alloc error happened.
                break;

            case TRUE:
                // All system inoculation entries matched.
                break;

            case FALSE:
                bContinueScan = RespondSystemInocChange(lpNoNav, szIntegFile);

                if ( IntegVerify(szIntegFile, rIntegArray) != 4 )
                    break;

                // Fall through if after responding the worst thing
                // that is left is some missing files.  In all other
                // cases (particularly verify errors), live with it.

            case 4:
                // Some inoc entries missing (but database exists)
                SystemInocCreate(lpNoNav, szIntegFile);
                break;
            }
        }
    return (bContinueScan);
} // End ScanCheckSystemIntegrity()


//************************************************************************
// RespondSystemInocNew()
//
// Responds to case where system inoculation database has not been created.
//
// Parameters:
//      LPNONAV lpNoNav                 Info and options for this scan.
//      LPSTR   lpIntegFile             Full path where integ file should be.
//
// Returns:
//      Nothing.
//************************************************************************
// 7/19/93 DALLEE, Function created.
//************************************************************************

VOID LOCAL PASCAL RespondSystemInocNew (LPNONAV lpNoNav, LPSTR lpIntegFile)
{
    extern  BOOL            bScreenStarted;
    extern  DIALOG_RECORD   *glpScanFilesDialog;

    extern  DIALOG          dlSystemInocPrompt;
    extern  ButtonsRec      buSystemInocNew;
    extern  ButtonsRec      okButton;
    extern  LPSTR           lpszSystemInocNewStrings [];
    extern  LPSTR           lpszSystemInocMessage [];
    extern  LPSTR           LPSZ_NO_SYSTEM_INOC_DATABASE [];

    auto    LPSTR           lpszAlertMessageNext;
    auto    UINT            uResult;
    auto    DIALOG_RECORD   *lpdrDialog;

                                        // Make sure screen IO is started.
    if (!bScreenStarted)
        {
        InitDisplay(TRUE);
        glpScanFilesDialog = OpenScanFilesDialog(lpNoNav);
        }

#ifdef USE_PASSWORD    //&?
    if ( (lpNoNav->Inoc.uActionNewInoc == INOC_PROMPT)  ||
         (lpNoNav->Inoc.uActionNewInoc == INOC_AUTO)    ||
         (lpNoNav->Inoc.uActionNewInoc == INOC_AUTO_FILEONLY) )
        {
        if (FALSE == AskForPassword(PASSWORD_INOCFILES))
            {
            lpNoNav->Inoc.uActionNewInoc = INOC_REPORT;
            }
        }
#endif

    switch (lpNoNav->Inoc.uActionNewInoc)
        {
        case INOC_DENY:                 // For Deny, prompt instead in scanner.
        case INOC_PROMPT:
            dlSystemInocPrompt.strings = lpszSystemInocNewStrings;
            dlSystemInocPrompt.buttons = &buSystemInocNew;

            if ( NULL == (lpdrDialog = DialogOpen2(&dlSystemInocPrompt,
                                                   LPSZ_NO_SYSTEM_INOC_DATABASE[0],
                                                   LPSZ_NO_SYSTEM_INOC_DATABASE[1],
                                                   LPSZ_NO_SYSTEM_INOC_DATABASE[2],
                                                   LPSZ_NO_SYSTEM_INOC_DATABASE[3],
                                                   LPSZ_NO_SYSTEM_INOC_DATABASE[4])))
                {
                break;
                }

            ButtonEnable(&buSystemInocNew, SYSTEM_NEW_INOC_YES_BUTTON,
                         lpNoNav->Inoc.bDispInoc);

            if (lpNoNav->Inoc.bDispInoc)
                {
                ButtonEnable(&buSystemInocNew, SYSTEM_NEW_INOC_NO_BUTTON,
                             lpNoNav->Inoc.bDispCont);
                }
            else
                {
                ButtonEnable(&buSystemInocNew, SYSTEM_NEW_INOC_NO_BUTTON,
                             TRUE);
                }
                             
            do
                {
                uResult = DialogProcessEvent(lpdrDialog, DialogGetEvent());
                if (uResult == ACCEPT_DIALOG)
                    {
                    if (buSystemInocNew.value == SYSTEM_NEW_INOC_YES_BUTTON)
                        SystemInocCreate(lpNoNav, lpIntegFile);
                    }
                }
                while ((uResult != ABORT_DIALOG) && (uResult != ACCEPT_DIALOG));
            
            DialogClose(lpdrDialog, uResult);
            
            break;

        case INOC_AUTO:
            SystemInocCreate(lpNoNav, lpIntegFile);
            break;

        case INOC_REPORT:
            dlSystemInocPrompt.strings = lpszSystemInocMessage;
            dlSystemInocPrompt.buttons = &okButton;
//&? DALLEE, Make call to timed alert box...
            DialogBox2(&dlSystemInocPrompt,
                       LPSZ_NO_SYSTEM_INOC_DATABASE[0],
                       LPSZ_NO_SYSTEM_INOC_DATABASE[1],
                       LPSZ_NO_SYSTEM_INOC_DATABASE[2],
                       LPSZ_NO_SYSTEM_INOC_DATABASE[3],
                       LPSZ_NO_SYSTEM_INOC_DATABASE[4]);

            break;
        }

} // End RespondSystemInocNew()


//************************************************************************
// SystemInocCreate()
//
// Creates the system inoculation database.
//
// Parameters:
//      LPSTR   lpIntegFile             Full path to file to create.
//
// Returns:
//      TRUE                            Made the integ dat file.
//      FALSE                           Failed creating integ dat file.
//************************************************************************
// 7/19/93 DALLEE, Function created.
//************************************************************************

BOOL LOCAL PASCAL SystemInocCreate (LPNONAV lpNoNav, LPSTR lpIntegFile)
{
    extern  DIALOG          dlSystemInocPrompt;
    extern  LPSTR           lpszSystemInocMessage   [];
    extern  LPSTR           LPSZ_SYSTEM_INOC_CREATE [];

    auto    DIALOG_RECORD   *lpdrDialog;
    auto    BOOL            bResult;

    dlSystemInocPrompt.strings = lpszSystemInocMessage;
    dlSystemInocPrompt.buttons = NULL;

    lpdrDialog = DialogOpen2(&dlSystemInocPrompt,
                             LPSZ_SYSTEM_INOC_CREATE[0],
                             LPSZ_SYSTEM_INOC_CREATE[1],
                             LPSZ_SYSTEM_INOC_CREATE[2],
                             LPSZ_SYSTEM_INOC_CREATE[3],
                             LPSZ_SYSTEM_INOC_CREATE[4]);

    StdDlgStartWait();
    bResult = IntegCreate(lpIntegFile, rIntegArray);
    StdDlgEndWait();

    if (NULL != lpdrDialog)
        DialogClose(lpdrDialog, ACCEPT_DIALOG);

    return (bResult);
} // End SystemInocCreate()


//************************************************************************
// RespondSystemInocChange()
//
// Responds to a change in the system inoculation entries.
//
// Parameters:
//      LPNONAV lpNoNav                 Info and options for this scan.
//      UINT    uIndex                  Index of integ item which changed.
//      LPSTR   lpIntegFile             Full path of integ file.
//
// Returns:
//      TRUE                            Continue with scan.
//      FALSE                           End scan now.
//************************************************************************
// 7/19/93 DALLEE, Function created.
//************************************************************************

BOOL LOCAL PASCAL RespondSystemInocChange (LPNONAV lpNoNav, LPSTR lpIntegFile)
{
    extern  DIALOG_RECORD   *glpScanFilesDialog;
    extern  BOOL            bScreenStarted;

    extern  DIALOG          dlSystemInocPrompt;
    extern  ButtonsRec      okButton;
    extern  LPSTR           lpszSystemInocMessage [];
    extern  LPSTR           LPSZ_SYSTEM_INOC_CHANGE [];

    auto    LPSTR           lpszAlertMessageNext;
    auto    char            lpAlertMessageBuffer [2][50 + 1];
    auto    BOOL            bContinueScan = TRUE;
    auto    BOOL            bDoneWithInteg = FALSE;
    auto    UINT            uCount;

                                        // Make sure screen IO is started.
    if (!bScreenStarted)
        {
        InitDisplay(TRUE);
        glpScanFilesDialog = OpenScanFilesDialog(lpNoNav);
        }

    if (IntegVerifyDatabaseMatch(lpIntegFile) == TRUE)
        {
        for (uCount = 0; *rIntegArray[uCount].byFilename; uCount++)
            {
            if (rIntegArray[uCount].wStatus == INTEG_FAIL_VERIFY)
                {
#ifdef USE_NETWORKALERTS   //&?
                AlertSystemInocChange(lpNoNav, rIntegArray[uCount].byFilename);
#endif

                switch (lpNoNav->Inoc.uActionInocChg)
                    {
                    case INOC_CHANGE_REPORT:
                        dlSystemInocPrompt.strings = lpszSystemInocMessage;
                        dlSystemInocPrompt.buttons = &okButton;
        //&? DALLEE, Make call to timed alert box...
                        DialogBox2(&dlSystemInocPrompt,
                                   LPSZ_SYSTEM_INOC_CHANGE[0],
                                   LPSZ_SYSTEM_INOC_CHANGE[1],
                                   LPSZ_SYSTEM_INOC_CHANGE[2],
                                   LPSZ_SYSTEM_INOC_CHANGE[3],
                                   LPSZ_SYSTEM_INOC_CHANGE[4]);
                        bDoneWithInteg = TRUE;
                        break;

                    default:                // For Deny, Prompt instead.
                    case INOC_CHANGE_PROMPT:
                        bContinueScan = PromptSystemInocChange(lpNoNav,
                                                               uCount,
                                                               lpIntegFile,
                                                               &bDoneWithInteg);
                        break;
                    }
                }
            if (!bContinueScan || bDoneWithInteg)
                {
                break;
                }
            }
        }
    else
        {
        SystemInocCreate(lpNoNav, lpIntegFile);
        }

    return (bContinueScan);
} // End RespondSystemInocChange()


//************************************************************************
// AlertSystemInocChange()
//
// Sends an inoc change alert for masterboot, boot, or system file change.
//
// Parameters:
//      LPNONAV lpNoNav                 Scan info
//      LPSTR   lpFile                  File name
//
// Returns:
//      Nothing.
//************************************************************************
// 12/2/93 DALLEE
//************************************************************************

#ifdef USE_NETWORKALERTS   //&?
VOID LOCAL PASCAL AlertSystemInocChange (LPNONAV lpNoNav, LPSTR lpFile)
{
    auto    NETALERT        Alert;
    auto    INFECTEDFILE    Infected;
    auto    UINT            n;

    MEMSET (&Alert,    EOS, sizeof(Alert));
    MEMSET (&Infected, EOS, sizeof(Infected));

                                        // Check if it's a boot record.
    for (n = 0; n <= 4; n++)
        {
        if (!STRCMP(lpFile, rIntegArray[n].byFilename))
            {
            Alert.uTypeOfEvent = (0 == n ? EV_ALERT_SYSTEMINOC_MBOOT_CHANGE
                                         : EV_ALERT_SYSTEMINOC_BOOT_CHANGE);
            break;
            }
        }
                                        // If it wasn't a boot record change
                                        // treat it as a regular file inoc.
                                        // change.
    if (!Alert.uTypeOfEvent)
        {
        Alert.uTypeOfEvent = EV_ALERT_INOC_CHANGE;
        STRCPY (Infected.szFullPath, lpFile);
        STRCPY (Infected.szFullOemPath, lpFile);
        }

    Alert.lpNet          = lpNoNav->lpNetUserItem;
    Alert.lpInfected     = &Infected;
    Alert.lpAlertOpt     = &lpNoNav->Alert;
    NetAlert (&Alert);
} // AlertSystemInocChange()
#endif


//************************************************************************
// PromptSystemInocChange()
//
// Prompts for the action to take when a System Inoculation item has changed.
//
// Parameters:
//      LPNONAV lpNoNav                 Info and options for this scan.
//      UINT    uIndex                  Index of the changed item.
//      LPSTR   lpIntegFile             Full path to system inoc database.
//      LPBOOL  lpbIsRebuilt            TRUE if Integ database rebuilt
//
// Returns:
//      TRUE                            Continue with the scan.
//      FALSE                           Abort the scan.
//************************************************************************
// 7/20/93 DALLEE, Function created.
//************************************************************************

BOOL LOCAL PASCAL PromptSystemInocChange (LPNONAV   lpNoNav,
                                          UINT      uIndex,
                                          LPSTR     lpIntegFile,
                                          LPBOOL    lpbIsRebuilt)
{
    extern  DIALOG          dlSystemInocPrompt;
    extern  LPSTR           lpszSystemInocPrompt [];
    extern  ButtonsRec      buSystemInocChange;

    auto    BOOL            bContinueScan = TRUE;
    auto    UINT            uResult;
    auto    DIALOG_RECORD   *lpdrDialog;
    auto    LPSTR           lpszAlertMessageNext;
    auto    char            lpAlertMessageBuffer [2][50 + 1];
    auto    char            lpMessageBuffer [5][46 + 1];
    auto    char            *lpszMessage [5] =
                                {
                                lpMessageBuffer[0],
                                lpMessageBuffer[1],
                                lpMessageBuffer[2],
                                lpMessageBuffer[3],
                                lpMessageBuffer[4]
                                };

    *lpbIsRebuilt=FALSE;

//&? DALLEE, merge this code with alert message code in Prompt Response()...
    if ( lpNoNav->Alert.bDispAlertMsg )
        {
        lpszAlertMessageNext = CutString(lpAlertMessageBuffer[0],
                                     lpNoNav->Alert.szAlertMsg,
                                     50);
        CutString(lpAlertMessageBuffer[1],
              lpszAlertMessageNext,
              50);
        }
    else
        {
        lpAlertMessageBuffer[0][0] = lpAlertMessageBuffer[1][0] = EOS;
        }

    STRCPY(lpNoNav->Infected.szFullPath, rIntegArray[uIndex].byFilename);
    STRCPY(lpNoNav->Infected.szFullOemPath, rIntegArray[uIndex].byFilename);
    switch (uIndex)
        {
        case 0:
            lpNoNav->Infected.wStatus = FILESTATUS_INTEG_MBCHANGED;
            break;

        case 1:
        case 2:
        case 3:
        case 4:
            lpNoNav->Infected.wStatus = FILESTATUS_INTEG_BOOTCHANGED;
            break;

        default:
            lpNoNav->Infected.wStatus = FILESTATUS_INTEG_SYSFILECHANGED;
            break;
        }
    ScanFoundCreateMessage(lpszMessage, &lpNoNav->Infected);

    dlSystemInocPrompt.strings = lpszSystemInocPrompt;
    dlSystemInocPrompt.buttons = &buSystemInocChange;

    if (NULL == (lpdrDialog = DialogOpen2(&dlSystemInocPrompt,
                                          lpszMessage[0],
                                          lpszMessage[1],
                                          lpszMessage[2],
                                          lpszMessage[3],
                                          lpszMessage[4],
                                          lpAlertMessageBuffer[0],
                                          lpAlertMessageBuffer[1])) )
        {
        return (TRUE);
        }

    ButtonEnable(&buSystemInocChange, SYSTEM_INOC_CHANGE_REPAIR, 
                 lpNoNav->Inoc.bDispRepair);
    ButtonEnable(&buSystemInocChange, SYSTEM_INOC_CHANGE_INOCULATE,
                 lpNoNav->Inoc.bDispInoc);
    ButtonEnable(&buSystemInocChange, SYSTEM_INOC_CHANGE_CONTINUE, 
                 lpNoNav->Inoc.bDispCont);
    ButtonEnable(&buSystemInocChange, SYSTEM_INOC_CHANGE_STOP,
                 lpNoNav->Opt.bAllowScanStop);

    if ( !lpNoNav->Inoc.bDispRepair &&
         !lpNoNav->Inoc.bDispInoc   &&
         !lpNoNav->Inoc.bDispCont   &&
         !lpNoNav->Opt.bAllowScanStop)
        {
        ButtonEnable(&buSystemInocChange, SYSTEM_INOC_CHANGE_CONTINUE, TRUE);
        }
        
    do
        {
        uResult = DialogProcessEvent(lpdrDialog, DialogGetEvent());

        if (uResult == ACCEPT_DIALOG)
            {
            switch (buSystemInocChange.value)
                {
                case SYSTEM_INOC_CHANGE_REPAIR:
                    TSR_OFF;
                    IntegRepair (lpIntegFile, &rIntegArray[uIndex]);
                    TSR_ON;
                    break;

                case SYSTEM_INOC_CHANGE_INOCULATE:
#ifdef USE_PASSWORD    //&?
                    if (FALSE == AskForPassword(PASSWORD_INOCFILES))
                        {
                        uResult = EVENT_AVAIL;
                        }
                    else
#endif
                        {
                        *lpbIsRebuilt = TRUE;
                        FileDelete(lpIntegFile);
                        SystemInocCreate(lpNoNav, lpIntegFile);
                        }
                    break;

                case SYSTEM_INOC_CHANGE_CONTINUE:
                    break;

                case SYSTEM_INOC_CHANGE_STOP:
                    bContinueScan = FALSE;
                    break;
                }
            }
        }
        while ((uResult != ABORT_DIALOG) && (uResult != ACCEPT_DIALOG));

    DialogClose(lpdrDialog, uResult);

    return (bContinueScan);
} // End PromptSystemInocChange()


//************************************************************************
// SystemInocVirusDlg ()
//
// Notifies that system inoc was skipped due to a virus.
//
// Parameters:
//      LPSTR   *lpszDialogStrings      Strings to use in dialog.
//      LPSTR   lpszItem                Filename if file infection.
// Returns:
//      Nothing.
//************************************************************************
// 12/10/93 DALLEE
//************************************************************************

VOID LOCAL PASCAL SystemInocVirusDlg (LPSTR     *lpszDialogStrings,
                                      DWORD     dwTicks,
                                      LPSTR     lpszItem,
                                      LPNONAV   lpNoNav)
{
    extern  BOOL            bScreenStarted;
    extern  DIALOG_RECORD   *glpScanFilesDialog;

    extern  DIALOG          stdErrorDlg;
    extern  COLORS          alert_colors;

    auto    DIALOG_RECORD   *lpdrDialog;
    auto    DWORD           dwStartTime;
    auto    WORD            wResult;

    if (!bScreenStarted)
        {
        InitDisplay(TRUE);
        glpScanFilesDialog = OpenScanFilesDialog(lpNoNav);
        }

    stdErrorDlg.strings = lpszDialogStrings;

    if (NULL != (lpdrDialog = DialogOpenC(&stdErrorDlg,
                                          &alert_colors,
                                          lpszItem)))
        {
        dwStartTime = TimerTicks();

        do
            {
            wResult = DialogProcessEvent(lpdrDialog, DialogGetEvent());

            if ((0 != dwTicks) && (dwTicks < TimerElapsedTicks(dwStartTime)))
                {
                wResult == ACCEPT_DIALOG;
                }
            }
            while ((wResult != ACCEPT_DIALOG) && (wResult != ABORT_DIALOG));
        DialogClose(lpdrDialog, wResult);
        }
} // SystemInocVirusDlg()

