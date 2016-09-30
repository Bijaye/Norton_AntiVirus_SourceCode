// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header: /Atomic/NAVXUtil/drvprblm.cpp 3     2/09/99 9:38a Cbenson $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log: /Atomic/NAVXUtil/drvprblm.cpp $
// 
// 3     2/09/99 9:38a Cbenson
// 
// 2     2/08/99 1:52p Cbenson
// Added code to supress the 'Drive is locked' message when trying to
// access a drive that cannot be read (in this case due to a HPFS
// partition on a NT server w/o HPFS drivers installed).  This is a
// special 'fix' for Royal Canadian Bank.
// 
// 1     11/05/98 3:05p Pputton
// 
//    Rev 1.2   15 Jul 1997 14:57:22   DBUCHES
// Added Ctl3d support for DEC Alpha
// 
//    Rev 1.1   21 May 1997 13:13:36   BILL
// Removed ctl3d
// 
//    Rev 1.0   06 Feb 1997 21:05:02   RFULLER
// Initial revision
// 
//    Rev 1.2   10 Jul 1996 18:45:46   TIVANOV
// fix for the dialog font initilization
//
//    Rev 1.1   28 Jun 1996 12:02:58   MKEATIN
// Ported Changes from LuigiPlus
//
//    Rev 1.0   30 Jan 1996 15:56:12   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:51:14   BARRY
// Initial revision.
//
//    Rev 1.13   15 Nov 1995 21:44:20   DALLEE
// Changed SYMSTAT references to use the NavStatXXX covers.
// NAVXUTIL now dynamically loads S32STAT to get around path issues.
// NAV dll's call SYMSTAT by going through NAVXUTIL.
//
//    Rev 1.12   24 Oct 1995 19:48:26   JWORDEN
// New options for MBR exceptions
//
//    Rev 1.11   20 Jul 1995 19:30:36   KEITH
// Check for no owner for the drive lock from SymStat and display unknown
// disk utility.
//
//    Rev 1.10   02 Jul 1995 10:56:22   MARKL
// SIZE_WORK was too small for messages.  Made larger and made allocation from
// the heap instead of the stack.  Fixes GPF.
//
//    Rev 1.9   24 May 1995 11:17:48   KEITH
// Add 2 more cases of DRIVEACCESS_MSG
//
//    Rev 1.8   23 May 1995 16:08:34   KEITH
// Add another message case for a SYMSTAT lock for DriveAccessProblem()
//
//    Rev 1.7   18 May 1995 10:21:04   KEITH
// Added a header and VCS log.
//
//************************************************************************


#include "platform.h"

#ifdef SYM_WIN

#include "ctl3d.h"
#include "ngui.h"
#include <time.h>
#include "symcfg.h"
#include "navutil.h"
#include "resexts.h"
#include "stbexts.h"
#include "navw_hlp.h"

#ifdef SYM_WIN32
#include "navstat.h"
#endif

extern HINSTANCE hInstance;

// ************************************************************************ //
// ****                     Local Prototypes                           **** //
// ************************************************************************ //

BOOL SYM_EXPORT WINAPI DriveAccessDlgProc (
    HWND    hDlg,
    UINT    wMessage,
    WPARAM  wParam,
    LPARAM  lParam
    );

VOID LOCAL GetTheMessage (
    LPSTR   lpMsg,
    LPDRIVEPROBLEMSTRUCT lpDP
    );

VOID LOCAL HideContinueButton (
    HWND    hDlg
    );

VOID LOCAL OnContextMenu                // Respond to WM_CONTEXTMENU message
    (
    HWND    hDlg,                       // [in] dialog that sent the message.
    UINT    uMsg,
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
    );

BOOL LOCAL IgnoreDriveLockedMessage();
BOOL LOCAL LogSkippedDrive(LPDRIVEPROBLEMSTRUCT lpDP);

// ************************************************************************ //


UINT SYM_EXPORT WINAPI DriveAccessProblem (
    LPDRIVEPROBLEMSTRUCT    lpDP
    )
{
    extern       HINSTANCE       hInstance;
    auto         DLGPROC         lpfn;
    auto         int             nStatus;

	// Fix for Royal Canadian Bank.  They don't want to see the 'Drive is locked'
	// message when they try to scan all hard drives. They have some machines with
	// HPFS drives that will cause this message to be displayed.  They are aware
	// (I think?) of the possible consequences of ignoring this message, but they'll
	// live with it...
	if((lpDP->uInfoMsgID == DRIVEACCESS_MSG_DISKLOCKED) && (IgnoreDriveLockedMessage()))
	{
		// also log that the drive was skipped
		LogSkippedDrive(lpDP);
		return DRIVEERROR_SKIP;
	}


                                        // ------------------------------------
                                        // This if() statement is just to make
                                        // sure that the suspicious "What would
                                        // you like to do?" message is not
                                        // prompted.
                                        // ------------------------------------
#ifdef SYM_WIN32
    if (lpDP->uInfoMsgID == DRIVEACCESS_MSG_BOOTRECORD  ||
        lpDP->uInfoMsgID == DRIVEACCESS_MSG_DRIVE       ||
        lpDP->uInfoMsgID == DRIVEACCESS_MSG_DISKLOCKED  ||
        lpDP->uInfoMsgID == DRIVEACCESS_MSG_DISKLOCKED_SYMSTAT ||
        lpDP->uInfoMsgID == DRIVEACCESS_MSG_DISKLOCKED_SYMSTAT_MBR ||
        lpDP->uInfoMsgID == DRIVEACCESS_MSG_DISKLOCKED_SYMSTAT_BOOT ||
        lpDP->uInfoMsgID == DRIVEREPAIR_MSG_DISKLOCKED_SYMSTAT ||
        lpDP->uInfoMsgID == DRIVEREPAIR_MSG_DISKLOCKED_SYMSTAT_MBR ||
        lpDP->uInfoMsgID == DRIVEREPAIR_MSG_DISKLOCKED_SYMSTAT_BOOT ||
        lpDP->uInfoMsgID == DRIVEACCESS_MSG_PHYS_BOOTRECORD ||
        lpDP->uInfoMsgID == DRIVEACCESS_MSG_PHYS_DISKLOCKED )
#else
    if (lpDP->uInfoMsgID == DRIVEACCESS_MSG_BOOTRECORD  ||
        lpDP->uInfoMsgID == DRIVEACCESS_MSG_DRIVE       ||
        lpDP->uInfoMsgID == DRIVEACCESS_MSG_DISKLOCKED )
#endif
        {
        lpfn = (DLGPROC) MakeProcInstance (
                                        (FARPROC)DriveAccessDlgProc,
                                        hInstance
                                        );

        nStatus = DialogBoxParam (
                                hInstance,
                                MAKEINTRESOURCE(IDD_DRIVEACCESS),
                                lpDP->hParent,
                                lpfn,
                                (LPARAM)lpDP
                                );

        SYM_ASSERT ( nStatus != -1 );

        FreeProcInstance((FARPROC) lpfn);
		// if user selected skip, log a skipped drive
        if (nStatus == DRIVEERROR_SKIP)
			LogSkippedDrive(lpDP);
        }
    else
		{
		// log that the drive was skipped
		LogSkippedDrive(lpDP);
        nStatus = DRIVEERROR_SKIP;
		}

    return ((nStatus == -1) ? DRIVEERROR_ERROR : nStatus);
}

// ************************************************************************ //
// ****                                                                **** //
// ************************************************************************ //

BOOL SYM_EXPORT WINAPI DriveAccessDlgProc (
    HWND    hDlg,
    UINT    wMessage,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
    auto    LPDRIVEPROBLEMSTRUCT    lpDP;
    auto    LPSTR                   lpszMsg;
    auto    BOOL                    bReturn = FALSE;

    switch(wMessage)
        {
        case WM_CTLCOLOR:
            bReturn = (BOOL)Ctl3dCtlColorEx(wMessage, wParam, lParam);
            break;
        case WM_INITDIALOG:
            lpDP = (LPDRIVEPROBLEMSTRUCT)lParam;

            SetWindowLong(hDlg, DWL_USER, lParam);
            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
            NG_SetFontsInDialog (hDlg, 0);

            if ( IsIconic( lpDP->hParent) )
                NG_CenterDialog ( hDlg, NULL );
            else
                NG_CenterDialog (hDlg, lpDP->hParent);

            NG_SetChildSmallFont (hDlg, ID_DRIVEACCESS_MSG);


                                        // --------------------------------
                                        // Obtain the required message and
                                        // display it.
                                        // --------------------------------

            lpszMsg = (LPSTR)MemAllocPtr (GMEM_ZEROINIT, SIZE_WORK * 2);
            if (lpszMsg != NULL)
                {
                GetTheMessage (lpszMsg, lpDP);
                Static_SetText(GetDlgItem(hDlg, ID_DRIVEACCESS_MSG), lpszMsg);
                MemFreePtr (lpszMsg);
                }

                                        // --------------------------------
                                        // If buttons to be hidden, doit.
                                        // --------------------------------
            if ( lpDP->bHideContinue )
                {
                HideContinueButton (hDlg);
                Button_Enable (GetDlgItem(hDlg, ID_CONTINUE), FALSE);
                ShowWindow    (GetDlgItem(hDlg, ID_CONTINUE), SW_HIDE);
                SetFocus      (GetDlgItem(hDlg, ID_SKIP));
                Button_SetStyle(GetDlgItem(hDlg, ID_SKIP),
                                BS_DEFPUSHBUTTON, FALSE);
                }
            else
                SetFocus (GetDlgItem(hDlg, ID_CONTINUE));

                                        // --------------------------------
                                        // For drives that are being accessed
                                        // physically we don't have enough info
                                        // to support a "skip"
                                        // --------------------------------
            if (lpDP->uInfoMsgID == DRIVEACCESS_MSG_PHYS_BOOTRECORD ||
                lpDP->uInfoMsgID == DRIVEACCESS_MSG_PHYS_DISKLOCKED)
                {
                Button_Enable (GetDlgItem(hDlg, ID_SKIP), FALSE);
                ShowWindow    (GetDlgItem(hDlg, ID_SKIP), SW_HIDE);
                }

            if ( lpDP->uTimeOutSeconds )
                {
                lpDP->uTimeOutSeconds *= 1000;
                SetTimer (hDlg, 1, lpDP->uTimeOutSeconds, NULL);
                }

            break;

        case WM_TIMER:
            KillTimer (hDlg, 1);
            lpDP = (LPDRIVEPROBLEMSTRUCT)GetWindowLong(hDlg,DWL_USER);

            if ( lpDP->bHideContinue )
                {
                FORWARD_WM_COMMAND ( hDlg, ID_SKIP,
                                     GetDlgItem ( hDlg, ID_SKIP ),
                                     BN_CLICKED, PostMessage );
                }
            else
                {
                FORWARD_WM_COMMAND ( hDlg, ID_CONTINUE,
                                     GetDlgItem ( hDlg, ID_CONTINUE ),
                                     BN_CLICKED, PostMessage );
                }
            break;

#ifdef SYM_WIN32
        case WM_HELP:
        case WM_CONTEXTMENU:
                                        // ----------------------------------
                                        // Bring up right click help menu.
                                        // or standard context sensitive help
                                        // ----------------------------------
            OnContextMenu ( hDlg, wMessage, wParam, lParam );
            bReturn = TRUE;
            break;
#endif

        case WM_COMMAND:
            {
            switch ( GET_WM_COMMAND_ID (wParam, lParam) )
                {
                case ID_RETRY:
                    EndDialog(hDlg, DRIVEERROR_RETRY);
                    break;

                case ID_CONTINUE:
                    KillTimer (hDlg, 1);
                    EndDialog(hDlg, DRIVEERROR_CONTINUE);
                    break;

                case ID_SKIP:
                    EndDialog(hDlg, DRIVEERROR_SKIP);
                    break;

                case IDHELP:
                    StdHelpFromDialog(
                                    hDlg,
                                    IDH_NAVW_WHATHIS_NODISK_HELP_BTN
                                    );
                    break;
                }
            }
        }

    return ( bReturn );
}

VOID LOCAL GetTheMessage (
    LPSTR   lpMsg,
    LPDRIVEPROBLEMSTRUCT lpDP
    )
{
    auto    UINT        uMsgID      = 0;
    auto    HINSTANCE   hUseInst    = NULL;
#ifdef SYM_WIN32
    auto    char        szLocalMsg  [ SYM_MAX_PATH*2+1 ];
    auto    char        szWho  [ SYM_MAX_PATH+1 ];
#else
    auto    char        szLocalMsg  [ SYM_MAX_PATH+1 ];
#endif

    *lpMsg = EOS;
    *szWho = EOS;

    switch (lpDP->uInfoMsgID)
        {
        case DRIVEACCESS_MSG_BOOTRECORD:
            uMsgID      = IDS_BOOTACCESS;
            hUseInst    = hInstance;
            break;

        case DRIVEACCESS_MSG_DRIVE:
            uMsgID      = IDS_DRIVEACCESS;
            hUseInst    = hInstance;
            break;

        case DRIVEACCESS_MSG_DISKLOCKED:
            uMsgID      = IDS_DRIVELOCKED;
            hUseInst    = hInstance;
            break;

#ifdef SYM_WIN32
        case DRIVEACCESS_MSG_DISKLOCKED_SYMSTAT:
            uMsgID      = IDS_DRIVELOCKED_SYMSTAT;
            hUseInst    = hInstance;
            NavStatGetDriveState ( lpDP->byDrive,
                                   0,
                                   szWho,
                                   sizeof ( szWho ) );
            break;

        case DRIVEACCESS_MSG_DISKLOCKED_SYMSTAT_BOOT:
            uMsgID      = IDS_DRIVELOCKED_SYMSTAT_BOOT;
            hUseInst    = hInstance;
            NavStatGetDriveState ( lpDP->byDrive,
                                   0,
                                   szWho,
                                   sizeof ( szWho ) );
            break;

        case DRIVEACCESS_MSG_DISKLOCKED_SYMSTAT_MBR:
            uMsgID      = IDS_DRIVELOCKED_SYMSTAT_MBR;
            hUseInst    = hInstance;
            NavStatGetDriveState ( lpDP->byDrive,
                                   0,
                                   szWho,
                                   sizeof ( szWho ) );
            break;

        case DRIVEREPAIR_MSG_DISKLOCKED_SYMSTAT:
            uMsgID      = IDS_DRIVELOCKED_REPAIR_SYMSTAT;
            hUseInst    = hInstance;
            break;

        case DRIVEREPAIR_MSG_DISKLOCKED_SYMSTAT_BOOT:
            uMsgID      = IDS_DRIVELOCKED_REPAIR_SYMSTAT_BOOT;
            hUseInst    = hInstance;
            break;

        case DRIVEREPAIR_MSG_DISKLOCKED_SYMSTAT_MBR:
            uMsgID      = IDS_DRIVELOCKED_REPAIR_SYMSTAT_MBR;
            hUseInst    = hInstance;
            break;

        case DRIVEACCESS_MSG_PHYS_BOOTRECORD:
            uMsgID      = IDS_BOOTACCESS_PHYS;
            hUseInst    = hInstance;
            break;

        case DRIVEACCESS_MSG_PHYS_DISKLOCKED:
            uMsgID      = IDS_DRIVELOCKED_PHYS;
            hUseInst    = hInstance;
            break;

#endif

        default:
            uMsgID      = lpDP->uInfoMsgID;
            hUseInst    = lpDP->hInstance;
            break;
        }

    if ( uMsgID && hUseInst )
        {
        if ( *szWho == EOS )
            LoadString ( hUseInst, IDS_DRIVELOCKED_UNKNOWN_APP,
                         szWho, sizeof(szWho));

        LoadString (hUseInst, uMsgID, szLocalMsg, sizeof(szLocalMsg));

#ifdef SYM_WIN32

        if ( uMsgID == IDS_DRIVELOCKED_SYMSTAT             ||
             uMsgID == IDS_DRIVELOCKED_SYMSTAT_MBR         ||
             uMsgID == IDS_DRIVELOCKED_SYMSTAT_BOOT        ||
             uMsgID == IDS_DRIVELOCKED_REPAIR_SYMSTAT      ||
             uMsgID == IDS_DRIVELOCKED_REPAIR_SYMSTAT_MBR  ||
             uMsgID == IDS_DRIVELOCKED_REPAIR_SYMSTAT_BOOT )
            {
                                        // If we don't know who has the lock
                                        // we need to use a different message
                                        // Note that the new message's Id must
                                        // always be one greater than the
                                        // original. The new message ignores
                                        // the szWho parameter
            if ( *(lpDP->szWho) == EOS )
                uMsgID++;

            LoadString (hUseInst, uMsgID, szLocalMsg, sizeof(szLocalMsg));

            SPRINTF ( lpMsg,
                      szLocalMsg,
                      (BYTE)lpDP->byDrive,
                      (LPSTR)(lpDP->szWho),
                      (LPSTR)(lpDP->szWho));
            }
        else
#endif
            {
            LoadString (hUseInst, uMsgID, szLocalMsg, sizeof(szLocalMsg));
            SPRINTF ( lpMsg, szLocalMsg, lpDP->byDrive );
            }
        }
}

VOID LOCAL HideContinueButton (
    HWND    hDlg
    )
{
    auto    RECT  RectContinue;
    auto    RECT  RectSkip;
    auto    POINT ptUpperLeft;
    auto    POINT ptLowerRight;

    GetWindowRect (GetDlgItem(hDlg, IDCANCEL), &RectContinue);
    GetWindowRect (GetDlgItem(hDlg, ID_SKIP), &RectSkip);

    ptUpperLeft.x  = RectContinue.left;
    ptUpperLeft.y  = RectContinue.top;
    ptLowerRight.x = RectContinue.right;
    ptLowerRight.y = RectContinue.bottom;

    ScreenToClient (hDlg, &ptUpperLeft );
    ScreenToClient (hDlg, &ptLowerRight);

    MoveWindow (GetDlgItem(hDlg, ID_SKIP), ptUpperLeft.x, ptUpperLeft.y,
                ptLowerRight.x - ptUpperLeft.x,
                ptLowerRight.y - ptUpperLeft.y, TRUE);
}

#ifdef SYM_WIN32

/* **************************************************************************
@Name: VOID LOCAL OnContextMenu (
            HWND    hDlg,
            UINT    uMsg
            WPARAM  wParam,
            LPARAM  lParam
            )

@Description:
This function is called when the dialog box receives a WM_CONTEXTMENU windows
message. The purpose of this function is to call the standard symantec help
routines that display an appropiate floating help menu.

@Parameters:
$hDlg$  handle of the dialog box where the message was intercepted.

$wParam$    the wParam that was passed to WM_CONTEXTMENU message.

$lParam$    the lParam that was passed to WM_CONTEXTMENU message.

@Include: stdhelp.h
************************************************************************** */

VOID LOCAL OnContextMenu                // Respond to WM_CONTEXTMENU message
    (
    HWND    hDlg,                       // [in] dialog that sent the message.
    UINT    uMsg,
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
    )
{
                                        // -----------------------------------
                                        // This array is a cross reference
                                        // between a control id number and a
                                        // help id number that should be
                                        // displayed when the user selects the
                                        // 'What's This' menu entry.
                                        // -----------------------------------


    static  DWORD       dwWhatsThisMap[] =
        {
        IDD_DRIVEACCESS,    IDH_NAVW_WHATHIS_NODISK_DLG,
        ID_RETRY,           IDH_NAVW_WHATHIS_NODISK_RETRY,
        ID_CONTINUE,        IDH_NAVW_WHATHIS_NODISK_CONTINUE,
        ID_SKIP,            IDH_NAVW_WHATHIS_NODISK_SKIP,
        0,                  0
        };


    NavDlgOnContextHelp(                /* Invoke Common Context Help Handling*/
                        hDlg,
                        uMsg,
                        wParam,
                        lParam,
                        dwWhatsThisMap,
                        NULL,           // dwHowToMap,
                        NULL,           /* This dlg. does not have any LB Ctrl*/
                        NULL            /* that's why these nulls             */
                       );
}

#endif  // For OnContextMenu()


/* **************************************************************************
@Name: BOOL LOCAL IgnoreDriveLockedMessage()

@Description:
This function determines if the IgnoreLockedDrive registry key is set. This
is a fix for Royal Canadian bank. They get the 'drive is locked' message when
trying to scan NT 4.0 Servers with HPFS drives on them. They didn't want to
see the message and will live with the possible side-effects of ignoring this
error.

@Parameters:
None.

************************************************************************** */
BOOL LOCAL IgnoreDriveLockedMessage() {  
	const TCHAR szNavKey[] = "SOFTWARE\\Symantec\\Norton AntiVirus";
	const TCHAR szNavIgnoreLockedDrive[] = "IgnoreLockedDriveMessage";
	HKEY hKey;
	DWORD dwType, dwSize, dwValue;

	BOOL bRetVal = FALSE;
	dwSize = sizeof(DWORD);

	// Open the main NAV registry key...
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, // handle to open key
									 szNavKey,			 // address of name of subkey to open
								 	 0,					 // reserved
									 KEY_ALL_ACCESS,	 // security access mask
									 &hKey)) {			 // address of handle to open key

		//
		// Does the value exist? Only on builds for Royal Canadian Bank...
		//
		if(ERROR_SUCCESS == RegQueryValueEx(hKey,						// handle to key to query
											szNavIgnoreLockedDrive,		// address of name of value to query
											0,							// reserved
										    &dwType,					// address of buffer for value type
											(LPBYTE)&dwValue,			// address of data buffer
											&dwSize))					// address of data buffer size
			if(dwValue == 1)
				bRetVal = TRUE;

			else
				bRetVal = FALSE;
	}

	return bRetVal;
}

/* **************************************************************************
@Name: BOOL LOCAL LogSkippedDrive(LPDRIVEPROBLEMSTRUCT lpDP)

@Description:
This function logs the fact that a drive was skipped during a scan (manual or
scheduled). This could be because the user was prompted and elected to cancel,
or due to new Reg key for Royal Canadian bank (see above). Drive letter is
formatted into a string for LOGSTRUCT input, passed in the lpFileName member.

@Parameters:
LPDRIVEPROBLEMSTRUCT - byDrive contains the letter of the drive (BYTE).

************************************************************************** */
BOOL LOCAL LogSkippedDrive(LPDRIVEPROBLEMSTRUCT lpDP)
{
    auto    LOGSTRUCT    Log;
    auto    NAVOPTIONS  navOptions = { 0 };
    char	szDriveString[3];

    szDriveString[0] = lpDP->byDrive;
    szDriveString[1] = '\0';

    ReadNavBin( &navOptions );

    Log.uEvent       = LOGEV_SKIPDRIVE;
    Log.lpFileName   = szDriveString;
    Log.lpVirusName  = NULL;
    Log.lpAct        = &navOptions.activity;
    LogEvent (&Log);

    return TRUE;
}

#endif  // SYM_WIN

