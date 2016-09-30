// Copyright 1992-1993 Symantec, Peter Norton Product Group
//***********************************************************************
//
// $Header:   S:/cpld/VCS/navdadmi.c_v   1.0   06 Feb 1997 20:50:18   RFULLER  $
//
// Description:
//  This contains routines for manipulating the NAVNET CPL.
//
// See Also:
//***********************************************************************
// $Log:   S:/cpld/VCS/navdadmi.c_v  $
// 
//    Rev 1.0   06 Feb 1997 20:50:18   RFULLER
// Initial revision
// 
//    Rev 1.1   07 Aug 1996 16:48:10   JBRENNA
// Port changes from DOSNAV environment
// 
// 
//    Rev 1.41   13 Jun 1996 11:54:44   JALLEE
// #if USE_PASSWORD -> #ifdef USE_PASSWORD
// 
//    Rev 1.40   28 Dec 1994 11:44:04   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.2   12 Dec 1994 15:48:48   DALLEE
    // Commented out password protection.
    //
    //    Rev 1.1   06 Dec 1994 20:48:30   DALLEE
    // CVT1 script.
    //
//    Rev 1.39   28 Dec 1994 11:34:38   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   29 Nov 1994 19:13:58   DALLEE
    // From Nirvana
//***********************************************************************

#ifdef NAVNET

#include "platform.h"
#include "xapi.h"
#include "stddos.h"
#include "stddlg.h"
#include "symnet.h"
#include "file.h"

#include "navutil.h"
#include "navdprot.h"

#include "options.h"

MODULE_NAME;


//************************************************************************
// DEFINES USED LOCALLY
//************************************************************************

#define INSTALL_OK_BUTTON       (0)
#define INSTALL_CANCEL_BUTTON   (1)
#define INSTALL_BEFORE_BUTTON   (2)
#define INSTALL_USER_BUTTON     (3)
#define INSTALL_WINDOWS_BUTTON  (4)
#define INSTALL_HELP_BUTTON     (5)

#define WINDOWS_OK_BUTTON       (0)
#define WINDOWS_CANCEL_BUTTON   (1)
#define WINDOWS_HELP_BUTTON     (2)

#define USERREG_OK_BUTTON       (0)
#define USERREG_CANCEL_BUTTON   (1)
#define USERREG_HELP_BUTTON     (2)

#define BEFORE_ELLIPSES2_BUTTON (0)
#define BEFORE_ELLIPSES3_BUTTON (1)
#define BEFORE_OK_BUTTON        (2)
#define BEFORE_CANCEL_BUTTON    (3)
#define BEFORE_HELP_BUTTON      (4)

//************************************************************************
// STATIC VARIABLES
//************************************************************************
static HGLOBAL          hBuffers;

//************************************************************************
// EXTERNAL VARIABLES
//************************************************************************
extern  LPNAVOPTIONS    lpNavOptions;

//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************
VOID LOCAL PASCAL InstallBeforeDlg(VOID);
VOID LOCAL PASCAL InstallUserRegDlg(VOID);
VOID LOCAL PASCAL InstallWindowsDlg(VOID);

UINT AdminCP_ENTERING (VOID);
VOID LOCAL PASCAL AdminAccept (VOID);
UINT LOCAL PASCAL AdminMemory (VOID);

VOID LOCAL PASCAL InstallBeforeInit     (VOID);
VOID LOCAL PASCAL InstallBeforeAccept   (VOID);
VOID LOCAL PASCAL InstallWindowsInit    (VOID);
VOID LOCAL PASCAL InstallWindowsAccept  (VOID);
VOID LOCAL PASCAL InstallUserRegInit    (VOID);
VOID LOCAL PASCAL InstallUserRegAccept  (VOID);


//************************************************************************
// CPDialogProcInstall()
//
// This is the callback routine for the Options Control Panel Install/Update
// Dialog.
//
// Parameters:
//  LPCPL         lpCPLDialog,          // Pointer to related CPL structure
//  UINT          uMessage,             // Message
//  UINT          uParameter,           // Additional message parameter
//  DIALOG_RECORD *lpDialog)            // Dialog record w/ internal info.
//
// Returns:
//  EVENT_AVAIL                         Further processing of message
//  ABORT_DIALOG                        Cancel button hit
//  ACCEPT_DIALOG                       OK button hit
//************************************************************************
// 3/14/93 BRAD Function created.
//************************************************************************

UINT CPDialogProcInstall    ( LPCPL         lpCPLDialog,
                              UINT          uMessage,
                              UINT          uParameter,
                              DIALOG_RECORD *lpDialog )
{
    extern      BYTE            byAcceptCPDialog;
    extern      char            FAR SZ_SELECT_NET_DIR[];
    extern      TERec           teNetworkDir;
    extern      ButtonsRec      buEllipses;
    extern      ButtonsRec      buInstallButtons;

    auto        WORD            wResult;
#ifdef USE_PASSWORD //&?
    auto        BOOL            bNormalProcess;
#endif


    switch (uMessage)
        {
        case CP_PRE_EVENT:              // Event received, not yet processed
                                        // Unless the event is the ESC key,
                                        // set the save flag.
            byAcceptCPDialog = (uParameter != ESC);
            return (EVENT_AVAIL);       // Return event avail for processing

        case CP_POST_EVENT:
            return (CONTINUE_DIALOG);

        case CP_BUTTON_PRESSED:         // Button pressed
                                        // lpDialog->item is DIALOG_ITEM
                                        // DIALOG_ITEM->item is ptr to actual
                                        // item, buttons in this case

                                        // If '...', then bring up Select
                                        // directory dialog
            if ((ButtonsRec *) lpDialog->item->item == &buEllipses)
                {
                auto    char    szDir[SYM_MAX_PATH];
                auto    WORD    wTopic;


                wTopic = HyperHelpTopicGet();
                HyperHelpTopicSet(HELP_DLG_NETDIR_BROWSE);
                DirGet(DiskGet(), szDir);
                if ( StdDlgSelectDir(szDir, SZ_SELECT_NET_DIR,
                                     NULL, NULL, NULL) )
                    {
                                        // Expand network directory to
                                        // server:path
                    NameToFull(teNetworkDir.string, szDir);
                    NetPathFromDrivePath(NULL,teNetworkDir.string, szDir,
                                                              sizeof(szDir)-1);
                    STRCPY(teNetworkDir.string, szDir);
                    teNetworkDir.offset = teNetworkDir.insert_pt = 0;
                    _TextEditWrite(&teNetworkDir, FALSE, FALSE);
                    }
                HyperHelpTopicSet(wTopic);
                return (EVENT_AVAIL);
                }

            switch (buInstallButtons.value)   // Current button selected
                {
                case INSTALL_OK_BUTTON:
                                        // Options saved on CP_LEAVING message
                    return (ACCEPT_DIALOG);

                case INSTALL_CANCEL_BUTTON:
                                        // Set save flag FALSE
                    byAcceptCPDialog = FALSE;
                    return (ABORT_DIALOG);

                case INSTALL_BEFORE_BUTTON:
                    InstallBeforeDlg();
                    break;

                case INSTALL_USER_BUTTON:
                    InstallUserRegDlg();
                    break;

                case INSTALL_WINDOWS_BUTTON:
                    InstallWindowsDlg();
                    break;

                case INSTALL_HELP_BUTTON:
                                        // Call scanner settings help here
		    HyperHelpDisplay();
                    break;
                }

            HyperHelpTopicSet(modify_install_settings);
            return (EVENT_AVAIL);       // Default for CP_BUTTON_PRESSED

        case CP_PRE_ENTERING:           // Just before entering
            return ( AdminMemory() );

        case CP_ENTERING:               // Dialog box now active box
                                        // Load OptAdmin info from database
                                        //  and initialize dialog data
            HyperHelpTopicSet(modify_install_settings);
#ifndef USE_PASSWORD //&?
            wResult = AdminCP_ENTERING();
#else
            wResult = ProcessPassword(lpDialog,
                                      PASSWORD_ALWAYS, 
                                      AdminCP_ENTERING,
                                      &bNormalProcess);
#endif
            return (wResult);

        case CP_LEAVING:                // Dialog box is losing focus
                                        // uParameter contains ACCEPT/ABORT
                                        // Return CONTINUE_DIALOG to reject
                                        // the CP_LEAVING request.

            if ( uParameter != ACCEPT_DIALOG )
                byAcceptCPDialog = FALSE;

            DialogSetFocus(lpDialog, &buInstallButtons);
                                        // If the dialog was not canceled,
                                        // save options if necessary.
            if ( byAcceptCPDialog
#if USE_PASSWORD    //&?
                 && !NeedPassword(PASSWORD_ALWAYS)
#endif
                 )
                {
                AdminAccept();
                }
                                        // Free temp ADMINOPT struct
            MemRelease(hBuffers);
            MemFree(hBuffers);

            return (byAcceptCPDialog ? ACCEPT_DIALOG : ABORT_DIALOG);
        }                               // End switch (uMessage)

    return (EVENT_AVAIL);               // Default return value
}


//************************************************************************
// LOCAL FUNCTIONS
//************************************************************************

UINT LOCAL PASCAL AdminMemory (VOID)
{
    extern      TERec           teNetworkDir;
    extern      TERec           teLocalDir;
    auto        LPBYTE          lpBuffers;


    hBuffers  = MemAlloc(GHND, teNetworkDir.max_len + 1 +
                               teLocalDir.max_len + 1);
    lpBuffers = MemLock(hBuffers);
    if ((hBuffers == NULL) || (lpBuffers == NULL))
        return (ABORT_DIALOG);

    teNetworkDir.string = lpBuffers;
    teLocalDir.string   = teNetworkDir.string + teNetworkDir.max_len + 1;

    return (EVENT_AVAIL);
}



//************************************************************************
// AdminCP_ENTERING()
//
// This routine handles the CP_ENTERING message for the Admin options
// Control Panel entry.
//
// Parameters:
//      None
//
// Returns:
//      EVENT_AVAIL                     Continue w/ dialog
//      ABORT_DIALOG                    Cancel dialog - couldn't allocate
//                                       memory for the temp ADMINOPT struct
//************************************************************************
// 3/15/93 DALLEE Function created.
//************************************************************************

UINT AdminCP_ENTERING (VOID)
{
    extern      ComboBoxRec     cxSystemMonitor;
    extern      CheckBoxRec     cbRunNAV;
    extern      CheckBoxRec     cbAddToPath;
    extern      CheckBoxRec     cbRetainSettings;
    extern      CheckBoxRec     cbLogoffIfNoMonitor;
    extern      CheckBoxRec     cbConfirmAllPrompts;
    extern      CheckBoxRec     cbUsePreviousLocalDir;
    extern      RadioButtonsRec rbLocalDir;
    extern      TERec           teNetworkDir;
    extern      TERec           teLocalDir;


    UpdateCheckBox(&cbRunNAV,           lpNavOptions->admin.bRunNAV);
    UpdateCheckBox(&cbAddToPath,        lpNavOptions->admin.bAddToPath);
    UpdateCheckBox(&cbRetainSettings,   lpNavOptions->admin.bRetainSettings);
    UpdateCheckBox(&cbLogoffIfNoMonitor,lpNavOptions->admin.bLogoffIfNoMonitor);
    UpdateCheckBox(&cbConfirmAllPrompts,lpNavOptions->admin.bConfirmAllPrompts);
    UpdateCheckBox(&cbUsePreviousLocalDir,lpNavOptions->admin.bUsePreviousLocalDir);

    RadioButtonsSetValue(&rbLocalDir,
                     (BYTE) ((lpNavOptions->admin.bPromptForLocalDir) ? 0 : 1));
    ComboBoxSetSelection(&cxSystemMonitor, lpNavOptions->admin.uSystemMonitor);

    NAVDLoadTextEdit(&teNetworkDir,     lpNavOptions->admin.szNetworkDir);
    _TextEditWrite(&teNetworkDir, TRUE, FALSE);

    NAVDLoadTextEdit(&teLocalDir,       lpNavOptions->admin.szLocalDir);
    _TextEditWrite(&teLocalDir, TRUE, FALSE);

    return (EVENT_AVAIL);
} // End AdminCP_ENTERING()


//************************************************************************
// AdminAccept()
//
// This routine updates the temporary ADMINOPT struct *lpOptAdmin from the
// dialog controls, and if changes were made, saves them to the .INI file
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/15/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL AdminAccept (VOID)
{
    extern      ComboBoxRec     cxSystemMonitor;
    extern      CheckBoxRec     cbRunNAV;
    extern      CheckBoxRec     cbAddToPath;
    extern      CheckBoxRec     cbRetainSettings;
    extern      CheckBoxRec     cbLogoffIfNoMonitor;
    extern      CheckBoxRec     cbConfirmAllPrompts;
    extern      CheckBoxRec     cbUsePreviousLocalDir;
    extern      RadioButtonsRec rbLocalDir;
    extern      TERec           teNetworkDir;
    extern      TERec           teLocalDir;


    lpNavOptions->admin.uSystemMonitor     = (BYTE) cxSystemMonitor.value;
    lpNavOptions->admin.bRunNAV            = cbRunNAV.value;
    lpNavOptions->admin.bAddToPath         = cbAddToPath.value;
    lpNavOptions->admin.bRetainSettings    = cbRetainSettings.value;
    lpNavOptions->admin.bPromptForLocalDir = (rbLocalDir.value) ? FALSE : TRUE;
    lpNavOptions->admin.bLogoffIfNoMonitor = cbLogoffIfNoMonitor.value;
    lpNavOptions->admin.bConfirmAllPrompts = cbConfirmAllPrompts.value;
    lpNavOptions->admin.bUsePreviousLocalDir= cbUsePreviousLocalDir.value;

    STRCPY(lpNavOptions->admin.szNetworkDir,  teNetworkDir.string);
    STRCPY(lpNavOptions->admin.szLocalDir,    teLocalDir.string);

} // End AdminAccept()


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 00/00/91 GREG Function Created.                                      *
 ************************************************************************/

UINT PathValidate(TERec *te ,UINT *c)
{
    static      char            szIllegalChars[] = "[]<>+=;\",";


    if ( STRCHR(szIllegalChars, *c) != NULL )
        {
        Blip();
        return (EVENT_USED);
        }

    return (EVENT_AVAIL);
}



//************************************************************************
// InstallBeforeDlg()
//
// This is the callback routine for the Options Control Panel Install/Update
// Dialog.
//
// Parameters:
//  LPCPL         lpCPLDialog,          // Pointer to related CPL structure
//  UINT          uMessage,             // Message
//  UINT          uParameter,           // Additional message parameter
//  DIALOG_RECORD *lpDialog)            // Dialog record w/ internal info.
//
// Returns:
//  EVENT_AVAIL                         Further processing of message
//  ABORT_DIALOG                        Cancel button hit
//  ACCEPT_DIALOG                       OK button hit
//************************************************************************
// 3/14/93 BRAD Function created.
//************************************************************************

VOID LOCAL PASCAL InstallBeforeDlg(VOID)
{
    extern      char            SZ_PROGRAM_EXTS[];
    extern      char            SZ_SELECT_BEFORE[];
    extern      char            SZ_SELECT_AFTER[];
    extern      DIALOG          dlgBefore;
    extern      ButtonsRec      buEllipses2;
    extern      ButtonsRec      buEllipses3;
    extern      CheckBoxRec     cbScanMemory;
    extern      CheckBoxRec     cbScanHardDisk;
    extern      CheckBoxRec     cbScanProgramFiles;
    extern      CheckBoxRec     cbScanAlways;
    extern      CheckBoxRec     cbBeforeCommandAlways;
    extern      CheckBoxRec     cbAfterCommandAlways;
    extern      TERec           teBeforeCommand;
    extern      TERec           teAfterCommand;
    extern      ComboBoxRec     cxActionAfterInstalling;
    auto        DIALOG_RECORD   *dialog;
    auto        BOOL            bFinished = FALSE;
    auto        WORD            wResult;
    auto        WORD            wButtons;
    auto        char            szFilename[SYM_MAX_PATH];
    auto        char            *pszTitle;
    auto        TERec           *pte;
    auto        char            szBeforeCommand[SYM_MAX_PATH];
    auto        char            szAfterCommand[SYM_MAX_PATH];



    teBeforeCommand.string = szBeforeCommand;
    teAfterCommand.string  = szAfterCommand;
    InstallBeforeInit();

    dialog = DialogOpen2( &dlgBefore,
                        &cbScanMemory,
                        &cbScanHardDisk,
                        &cbScanProgramFiles,
                        &cbScanAlways,
                        &teBeforeCommand,
                        &buEllipses2,
                        &cbBeforeCommandAlways,
                        &cxActionAfterInstalling,
                        &teAfterCommand,
                        &buEllipses3,
                        &cbAfterCommandAlways);

    while (!bFinished)
        {
        HyperHelpTopicSet(before_and_after_install);

                                        // Enable/disable checkboxes based
                                        // on presence of text in TE's
        if ( (EOS == *teBeforeCommand.string)
             != (CB_DISABLED == (cbBeforeCommandAlways.flags & CB_DISABLED)) )
            {
            CheckBoxEnable(dialog,
                           &cbBeforeCommandAlways,
                           (BYTE)(EOS != teBeforeCommand.string[0]) );
            }
        if ( (EOS == *teAfterCommand.string)
             != (CB_DISABLED == (cbAfterCommandAlways.flags & CB_DISABLED)) )
            {
            CheckBoxEnable(dialog,
                           &cbAfterCommandAlways,
                           (BYTE)(EOS != teAfterCommand.string[0]) );
            }

        wResult = DialogProcessEvent(dialog, DialogGetEvent());

        if ( wResult == ABORT_DIALOG )
            break;                      // User decided not to print

        if ( wResult != ACCEPT_DIALOG )
            continue;

        wButtons = DialogGetUniqueButtonNumber(dialog);

        switch (wButtons)
            {
            default:                    // Not a button at all.  Default to OK.
            case BEFORE_OK_BUTTON:
                InstallBeforeAccept();
                bFinished = TRUE;
                break;

            case BEFORE_CANCEL_BUTTON:
                bFinished = TRUE;
                break;

            case BEFORE_HELP_BUTTON:
	        HyperHelpDisplay();
                break;

            case BEFORE_ELLIPSES2_BUTTON:
                pte = &teBeforeCommand;
                pszTitle = SZ_SELECT_BEFORE;
                goto askUser;

            case BEFORE_ELLIPSES3_BUTTON:
                pte = &teAfterCommand;
                pszTitle = SZ_SELECT_AFTER;

askUser:
                HyperHelpTopicSet(HELP_DLG_BEFORE_BROWSE);
                DirGet(DiskGet(), szFilename);
                if ( StdDlgBrowse(szFilename,   SZ_PROGRAM_EXTS,
                                                0,
                                                pszTitle,
                                                MAX_SELECT_FILES,
                                                NULL, NULL) )
                    {
                    NameToFull(pte->string, szFilename);
                    STRUPR(pte->string);
                    pte->offset = pte->insert_pt = 0;
                    _TextEditWrite(pte, FALSE, FALSE);
                    }
                break;
            }
        }

    DialogClose(dialog, ACCEPT_DIALOG);

}

//************************************************************************
// InstallUserRegDlg()
//
// This is the callback routine for the Options Control Panel Install/Update
// Dialog.
//
// Parameters:
//  LPCPL         lpCPLDialog,          // Pointer to related CPL structure
//  UINT          uMessage,             // Message
//  UINT          uParameter,           // Additional message parameter
//  DIALOG_RECORD *lpDialog)            // Dialog record w/ internal info.
//
// Returns:
//  EVENT_AVAIL                         Further processing of message
//  ABORT_DIALOG                        Cancel button hit
//  ACCEPT_DIALOG                       OK button hit
//************************************************************************
// 3/14/93 BRAD Function created.
//************************************************************************

VOID LOCAL PASCAL InstallUserRegDlg(VOID)
{
    extern      DIALOG          dlgUserReg;
    extern      RadioButtonsRec rbCompany;
    extern      TERec           teCompany;
    extern      CheckBoxRec     cbPromptForName;
    auto        DIALOG_RECORD   *dialog;
    auto        BOOL            bFinished = FALSE;
    auto        WORD            wResult;
    auto        char            szCompany[MAX_USER_INFO+1];


    teCompany.string = szCompany;
    InstallUserRegInit();
    HyperHelpTopicSet(help_dlg_user);

    dialog = DialogOpen2(&dlgUserReg,
                        &rbCompany,
                        &teCompany,
                        &cbPromptForName);

    while (!bFinished)
        {
        wResult = DialogLoop(dialog, NULL);

        if ( wResult == ABORT_DIALOG )
            break;                      // User decided not to print

        if ( wResult != ACCEPT_DIALOG )
            continue;

        switch (dialog->d.buttons->value)
            {
            case USERREG_OK_BUTTON:
                InstallUserRegAccept();
                bFinished = TRUE;
                break;

            case USERREG_CANCEL_BUTTON:
                bFinished = TRUE;
                break;

            case USERREG_HELP_BUTTON:
	        HyperHelpDisplay();
                break;

            default:                    // Not a button at all
                break;
            }
        }

    DialogClose(dialog, ACCEPT_DIALOG);

}

//************************************************************************
// InstallWindowsDlg()
//
// This is the callback routine for the Options Control Panel Install/Update
// Dialog.
//
// Parameters:
//  LPCPL         lpCPLDialog,          // Pointer to related CPL structure
//  UINT          uMessage,             // Message
//  UINT          uParameter,           // Additional message parameter
//  DIALOG_RECORD *lpDialog)            // Dialog record w/ internal info.
//
// Returns:
//  EVENT_AVAIL                         Further processing of message
//  ABORT_DIALOG                        Cancel button hit
//  ACCEPT_DIALOG                       OK button hit
//************************************************************************
// 3/14/93 BRAD Function created.
//************************************************************************

VOID LOCAL PASCAL InstallWindowsDlg(VOID)
{
    extern      DIALOG          dlgWindows;
    extern      CheckBoxRec     cbInstallWindowsNAV;
    extern      CheckBoxRec     cbLoadWindowsDrivers;
    extern      CheckBoxRec     cbCreateWindowsGroup;
    auto        DIALOG_RECORD   *dialog;
    auto        BOOL            bFinished = FALSE;
    auto        WORD            wResult;


    InstallWindowsInit();
    HyperHelpTopicSet(help_dlg_windows);

    dialog = DialogOpen2(&dlgWindows,
                        &cbInstallWindowsNAV,
                        &cbLoadWindowsDrivers,
                        &cbCreateWindowsGroup);

    while (!bFinished)
        {
        wResult = DialogLoop(dialog, NULL);

        if ( wResult == ABORT_DIALOG )
            break;                      // User decided not to print

        if ( wResult != ACCEPT_DIALOG )
            continue;

        switch (dialog->d.buttons->value)
            {
            case WINDOWS_OK_BUTTON:
                InstallWindowsAccept();
                bFinished = TRUE;
                break;

            case WINDOWS_CANCEL_BUTTON:
                bFinished = TRUE;
                break;

            case WINDOWS_HELP_BUTTON:
	        HyperHelpDisplay();
                break;

            default:                    // Not a button at all
                break;
            }
        }

    DialogClose(dialog, ACCEPT_DIALOG);
}


//************************************************************************
// InstallBeforeInit()
//
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/10/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL InstallBeforeInit (VOID)
{
    extern      CheckBoxRec     cbScanMemory;
    extern      CheckBoxRec     cbScanHardDisk;
    extern      CheckBoxRec     cbScanProgramFiles;
    extern      CheckBoxRec     cbScanAlways;
    extern      CheckBoxRec     cbBeforeCommandAlways;
    extern      CheckBoxRec     cbAfterCommandAlways;
    extern      ComboBoxRec     cxActionAfterInstalling;
    extern      TERec           teBeforeCommand;
    extern      TERec           teAfterCommand;


    cbScanMemory.value             = lpNavOptions->admin.bScanMemory;
    cbScanHardDisk.value           = lpNavOptions->admin.bScanHardDisk;
    cbScanProgramFiles.value       = lpNavOptions->admin.bScanProgramFiles;
    cbScanAlways.value             = lpNavOptions->admin.bScanAlways;
    cbBeforeCommandAlways.value    = lpNavOptions->admin.bBeforeCommandAlways;
    cbAfterCommandAlways.value     = lpNavOptions->admin.bAfterCommandAlways;
    cxActionAfterInstalling.value  = lpNavOptions->admin.uActionAfterInstalling;
    STRCPY(teBeforeCommand.string, lpNavOptions->admin.szBeforeCommand);
    STRCPY(teAfterCommand.string,  lpNavOptions->admin.szAfterCommand);

}


//************************************************************************
// InstallBeforeAccept()
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/10/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL InstallBeforeAccept (VOID)
{
    extern      CheckBoxRec     cbScanMemory;
    extern      CheckBoxRec     cbScanHardDisk;
    extern      CheckBoxRec     cbScanProgramFiles;
    extern      CheckBoxRec     cbScanAlways;
    extern      CheckBoxRec     cbBeforeCommandAlways;
    extern      CheckBoxRec     cbAfterCommandAlways;
    extern      ComboBoxRec     cxActionAfterInstalling;
    extern      TERec           teBeforeCommand;
    extern      TERec           teAfterCommand;


    lpNavOptions->admin.bScanMemory             = cbScanMemory.value;
    lpNavOptions->admin.bScanHardDisk           = cbScanHardDisk.value;
    lpNavOptions->admin.bScanProgramFiles       = cbScanProgramFiles.value;
    lpNavOptions->admin.bScanAlways             = cbScanAlways.value;
    lpNavOptions->admin.bBeforeCommandAlways    = cbBeforeCommandAlways.value;
    lpNavOptions->admin.bAfterCommandAlways     = cbAfterCommandAlways.value;
    lpNavOptions->admin.uActionAfterInstalling  = (BYTE) cxActionAfterInstalling.value;
    STRCPY(lpNavOptions->admin.szBeforeCommand, teBeforeCommand.string);
    STRCPY(lpNavOptions->admin.szAfterCommand,  teAfterCommand.string);
}


//************************************************************************
// InstallWindowsInit()
//
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/10/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL InstallWindowsInit (VOID)
{
    extern      CheckBoxRec     cbInstallWindowsNAV;
    extern      CheckBoxRec     cbLoadWindowsDrivers;
    extern      CheckBoxRec     cbCreateWindowsGroup;


    cbInstallWindowsNAV.value   = lpNavOptions->admin.bInstallWindowsNAV;
    cbLoadWindowsDrivers.value  = lpNavOptions->admin.bLoadWindowsDrivers;
    cbCreateWindowsGroup.value  = lpNavOptions->admin.bCreateWindowsGroup;

}


//************************************************************************
// InstallWindowsAccept()
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/10/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL InstallWindowsAccept (VOID)
{
    extern      CheckBoxRec     cbInstallWindowsNAV;
    extern      CheckBoxRec     cbLoadWindowsDrivers;
    extern      CheckBoxRec     cbCreateWindowsGroup;


    lpNavOptions->admin.bInstallWindowsNAV  = cbInstallWindowsNAV.value;
    lpNavOptions->admin.bLoadWindowsDrivers = cbLoadWindowsDrivers.value;
    lpNavOptions->admin.bCreateWindowsGroup = cbCreateWindowsGroup.value;
}

//************************************************************************
// InstallUserRegInit()
//
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/10/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL InstallUserRegInit (VOID)
{
    extern      CheckBoxRec             cbPromptForName;
    extern      RadioButtonsRec         rbCompany;
    extern      TERec                   teCompany;


    cbPromptForName.value       = lpNavOptions->admin.bPromptForName;
    rbCompany.value             = (lpNavOptions->admin.bPromptForCompany) ?
                                                        0 : 1;
    STRCPY(teCompany.string,    lpNavOptions->admin.szCompany);
}


//************************************************************************
// InstallUserRegAccept()
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/10/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL InstallUserRegAccept (VOID)
{
    extern      CheckBoxRec             cbPromptForName;
    extern      RadioButtonsRec         rbCompany;
    extern      TERec                   teCompany;


    lpNavOptions->admin.bPromptForName          = cbPromptForName.value;
    lpNavOptions->admin.bPromptForCompany       = (rbCompany.value) ? FALSE : TRUE;
    STRCPY(lpNavOptions->admin.szCompany,       teCompany.string);
}

#endif

