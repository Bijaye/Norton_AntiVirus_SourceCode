/* Copyright 1990-1993 Symantec, Peter Norton Computing Group           */
/************************************************************************/
/*                                                                      */
/* $Header:   S:/NPSUI/VCS/NPSNOTFY.CPv   1.3   28 Aug 1998 17:19:48   JBRENNA  $ */
/*                                                                      */
/* Description:                                                         */
/*      This module contains the event notification code for the        */
/*      Norton Program Scheduler                                        */
/*                                                                      */
/* Contains:                                                            */
/*                                                                      */
/************************************************************************/
/* $Log:   S:/NPSUI/VCS/NPSNOTFY.CPv  $ */
// 
//    Rev 1.3   28 Aug 1998 17:19:48   JBRENNA
// Port QAKN changes to trunk.
// 
//    Rev 1.2   17 Apr 1998 12:17:06   mdunn
// #undef'ed TRACE since it now collides with an MFC function.
// 
//    Rev 1.1   15 Jul 1997 15:50:46   DBUCHES
// Added Ctl3d for DEC Alpha
// 
//    Rev 1.0   19 May 1997 00:25:10   KSACKIN
// Initial revision.
// 
//    Rev 1.20   07 May 1997 18:52:36   BILL
// Removed ctl3d for Alpha platform
// 
//    Rev 1.19   18 Sep 1996 10:16:34   JBRENNA
// Add NG_SetFontsInDialog() call when processing WM_INITDIALOG messages.
// This ensures that a DBCS font is used when necessary.
// 
//    Rev 1.18   22 Feb 1996 13:12:42   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' and 'StdHelpOnContextHelp()' to
// 'StdHelpExOnContextHelp()' to use Quake's StdHelp merged with the NAV
// version.
// 
//    Rev 1.17   11 Jan 1996 09:49:14   JWORDEN
// Port changes from Quake 8
// 
//    Rev 1.16.1.1   30 Nov 1995 21:03:26   BARRY
// Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
// 
//    Rev 1.16   03 May 1995 13:19:38   MFALLEN
// Moved stdhelp.h higher up in the include order.
// 
//    Rev 1.15   25 Apr 1995 19:52:26   BRAD
// Enabled Ctl3d and got working for NT
// 
//    Rev 1.14   14 Mar 1995 13:01:42   MFALLEN
// Added How To help.
// 
//    Rev 1.13   13 Mar 1995 17:12:06   MFALLEN
// Added more right click help.
// 
//    Rev 1.12   06 Mar 1995 10:11:50   MFALLEN
// Made it work for the Win95 platform and added required enhancements to
// the product to give it a native Win95 look and feel.
// 
//    Rev 1.11   26 Mar 1994 21:00:12   BRAD
// Cleaned up Windows messages
// 
//    Rev 1.10   15 Mar 1994 11:58:30   BRUCE
// Changed SYM_EXPORT to SYM_EXPORT
// 
//    Rev 1.9   03 Mar 1994 12:10:06   BRAD
// AWK script applied
// 
//    Rev 1.8   01 Oct 1993 13:00:22   MARKL
// Now fully supports named events.
// 
//    Rev 1.7   29 Sep 1993 14:53:48   MARKL
// No change.
// 
//    Rev 1.6   23 Jul 1993 10:28:16   MARKL
// Help menu now looks like nirvana's.  Removed help/exit buttons.  All
// child dialogs are centered.
// 
//    Rev 1.5   22 Jul 1993 12:01:50   MARKL
// Hooked up quake help system.
// 
//    Rev 1.4   02 Jul 1993 06:59:18   KYECK
// Fixed STRICT warnings/errors.
//
//    Rev 1.3   10 Jun 1993 09:32:12   KYECK
// Added support for 'yearly' events.
//
//    Rev 1.2   21 Apr 1993 22:05:02   MARKL
// Nirvana gives the schedule a chance to take an a radically new look/feel
// in the Add/Edit dialog!!!
//
//    Rev 1.1   18 Mar 1993 20:32:24   MARKL
// Cleaned up unnecessary typecasting of LPSTR.
//
//    Rev 1.0   15 Mar 1993 17:51:36   MARKL
// Initial revision.
//
//    Rev 1.3   26 Jun 1992 17:55:18   MEIJEN
// Get rid of strids.h because NWRES will eventually disappear
//
//    Rev 1.2   19 Jun 1992 10:34:06   MEIJEN
// Decentralize resources from NWRES to NPS(scheduler) for REVOLVER
//
//    Rev 1.1   02 Oct 1991 12:06:58   MARKL
// Made WinStric.H pass.
//
//    Rev 1.0   28 Jul 1991 11:37:40   MARKL
// Initial revision.
//
//    Rev 1.5   03 Jun 1991 12:20:30   MARKL
// Timer is still enabled during notifications.
//
//    Rev 1.4   01 May 1991 17:43:32   MARKL
// We be Segmented!
//
//    Rev 1.3   27 Apr 1991 15:03:14   MARKL
// No change.
//
//    Rev 1.2   05 Mar 1991 12:09:12   MARKL
// All resources were moved to NWRES.
//
//    Rev 1.1   04 Mar 1991 18:08:30   MARKL
// Moved GetActiveWindow() call.  Was checking for the currently active
// app too late (i.e. after I made myself active).  Once the notification
// dialog box was closed the program that was running before the notice
// popped up was not being restored properly.
//
//    Rev 1.0   04 Mar 1991 17:40:24   MARKL
// Initial revision.
/************************************************************************/
#include    "platform.h"
#include    "stdhelp.h"
#include    <ctl3d.h>
#include    "undoc.h"                  /* for extra typedefs           */
#include    "msgdisp.h"                    /* error message stuff          */
#include    "ngui.h"
#include    "nps.h"                     /* scheduler includes           */
#include    "npsres.h"
#include    "npsstb.h"
#include    "nps_hlp.h"

#undef TRACE
#define TRACE   ERR_TRACE_ID (_NPSNOTFY_C)




/************************************************************************/
/* Local Component Prototypes                                           */
/************************************************************************/

WORD LOCAL PASCAL NpsEventNotifyDlgDeInit( // Exits notify dlg box
    HWND    hDlg                        // Notify Dlg Box dialog handle
    );

VOID LOCAL OnContextHelp (              // Do help stuff
    HWND    hDlg,                       // [in] from this dialog
    UINT    uMessage,                   // [in] help message to deal with
    WPARAM  wParam,                     // [in] ?
    LPARAM  lParam                      // [in] ??
    );

/************************************************************************/
/* Component Code                                                       */
/************************************************************************/




/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine notifies the user that his event has occured by    */
/*      creating an IDD_NPS_NOTIFY modeless dialog box.                 */
/*      We disable our parent and increment our wNumNotify counter      */
/*      such that our parent is never enabled until all notifications   */
/*      have been cleared from the screen.                              */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           -- Everything went okay.        */
/*      others                          -- Errors from called routines. */
/*                                                                      */
/************************************************************************/
/* 03/01/91 MSL Function created.                                       */
/* 03/04/91 MSL Added wIndex to parameter list.                         */
/*              Wasn't finding out the previous active window soon      */
/*              enough.                                                 */
/* 03/05/91 MSL Resources now in NWRES.                                 */
/* 06/03/91 MSL Made notification parent NULL.                          */
/* 04/19/93 MSL The type of event tells us where to get our event text  */
/*              from.                                                   */
/************************************************************************/
WORD PASCAL NpsEventNotifyDisplay(      /* Display event notify box     */
    HWND        hDlg,                   /* Application window handle    */
    WORD        wIndex)                 /* Index in Event list          */
{
    HWND                        hDlgNotify;
    HWND                        hWndPrevActive;
    ATOM                       gaDesc;
    ATOM                       gaMessage;
    extern      WORD            wNumNotify;
    extern      NPSEVNT         Events[];

                                        /* Find out the previous active */
                                        /* window for later activation. */
    hWndPrevActive = GetActiveWindow ();

                                        /* Create our global atoms      */
    gaDesc    = GlobalAddAtom (Events[wIndex].szDescr);

    if (Events[wIndex].wType == EVENT_TYPE_PROGRAM)
        {
        gaMessage = GlobalAddAtom (Events[wIndex].szCmdLine);
        }
    else
        {
        gaMessage = GlobalAddAtom (Events[wIndex].szMessage);
        }

                                        /* Create the notify box        */
    hDlgNotify = CreateDialogParam (ghInst,
                                    MAKEINTRESOURCE (IDD_NPS_NOTIFY),
                                    NULL,
                                    (DLGPROC)NpsEventNotifyDlgProc,
                                    MAKELPARAM (gaDesc, gaMessage));

                                        /* Keep track of this new       */
                                        /* notification dialog box.     */
    if (hDlgNotify != (HWND)(-1))
        {
        SetProp ( hDlgNotify, NPS_PROP_PREV, hWndPrevActive );

#ifdef SYM_WIN32
        SetForegroundWindow ( hDlgNotify );
#endif

        wNumNotify++;
        }
    else
        {
        GlobalDeleteAtom (gaDesc);
        GlobalDeleteAtom (gaMessage);
        }

                                        /* Return to our caller         */
    return (NOERR);
} /* NpsEventNotifyBoxDisplay() */




/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This modeless dialog box tells the user that his event has      */
/*      occured and displays the message.                               */
/*                                                                      */
/* Return values:                                                       */
/*                                                                      */
/************************************************************************/
/* 03/01/91 MSL Function created.                                       */
/* 06/03/91 MSL We do not disable our parent.                           */
/* 07/02/93 KDY Added BOOL typecast to return of Ctl3dColorEx.          */
/************************************************************************/
BOOL SYM_EXPORT CALLBACK NpsEventNotifyDlgProc ( /* Notification dialog box */
    HWND        hDlg,                   /* Handle to dialog box         */
    UINT        wMessage,               /* Message to process           */
    WPARAM      wParam,                 /* WPARAM parameter             */
    LPARAM      lParam)                 /* LPARAM parameter             */
{
    char                szBuf[MAX_CMDLINE];
    extern      HWND    hDlgModelessActive;

                                        /* Process the message we have  */
                                        /* been passed.                 */
    switch (wMessage)
       {
                                        /* We are being initialized so  */
                                        /* set focus to the correct     */
                                        /* control.                     */
        case WM_INITDIALOG:
            NG_CenterDialog (hDlg, GetParent (hDlg));

                                        // Setup the Fonts. Finds the correct
                                        // DBCS font if DBCSInstalled().
            NG_SetFontsInDialog (hDlg, 0);

            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);

            if (LOWORD (lParam) != NULL)
                {
                GlobalGetAtomName (LOWORD (lParam), szBuf, MAX_CMDLINE);
                SetDlgItemText (hDlg, IDC_NPS_NOTIFY_DESCR_TEXT, szBuf);
                GlobalDeleteAtom (LOWORD (lParam));
                }
            if (HIWORD (lParam) != NULL)
                {
                GlobalGetAtomName (HIWORD (lParam), szBuf, MAX_CMDLINE);
                SetDlgItemText (hDlg, IDC_NPS_NOTIFY_TEXT, szBuf);
                GlobalDeleteAtom (HIWORD (lParam));
                }
            SetFocus (GetDlgItem (hDlg, IDOK));
            return (FALSE);
            break;

                                        /* WM_ACTIVATE:  When our dialog*/
                                        /* box is being activated or    */
                                        /* deactivated.  By setting     */
                                        /* hDlgModelessActive, all the  */
                                        /* user's interactions with the */
                                        /* controls will be handled.  We*/
                                        /* must call DefDlgProc() for   */
                                        /* our caption to paint         */
                                        /* properly.                    */
        case WM_ACTIVATE:
            if (GET_WM_ACTIVATE_STATE(wParam, lParam))
                {
                hDlgModelessActive = hDlg;
                }
            return (FALSE);
            break;

                                        // WM_SYSCOLORCHANGE:  For 3D
                                        // effect.
        case WM_SYSCOLORCHANGE:
            Ctl3dColorChange();
            break;

                                        // WM_CTLCOLOR:  For 3D effect.
        case WM_CTLCOLOR:
            return ((BOOL)Ctl3dCtlColorEx (wMessage, wParam, lParam));

#ifdef SYM_WIN32
        case WM_HELP:
        case WM_CONTEXTMENU:
                                        // ----------------------------------
                                        // Bring up right click help menu.
                     					// or context sensitive help.
                                        // ----------------------------------
            OnContextHelp ( hDlg, wMessage, wParam, lParam );
            break;
#endif


                                        /* We are being closed so       */
                                        /* return to our caller.        */
        case WM_CLOSE:
            NpsEventNotifyDlgDeInit (hDlg);
           break;

                                        /* Process a control message.   */
                                        /* If it is the OK button then  */
                                        /* get out of here!             */
        case WM_COMMAND:
            if (GET_WM_COMMAND_ID(wParam, lParam) == IDOK)
                {
                NpsEventNotifyDlgDeInit (hDlg);
                }
            else
                {
                return (FALSE);
                }
           break;

                                        /* Let Windows handle the       */
                                        /* message since we have no     */
                                        /* clue what to do with it.     */
        default:
            return (FALSE);
       } /* switch */

   return (TRUE);
} /* NpsEventNotifyDlgProc() */




/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine cleans up after the NpsEventNotifyDlgProc.         */
/*      It finds out what application window was previously active,     */
/*      removes the NPS_PROP_PREV property, and destroys our dialog box */
/*      window.                                                         */
/*      We decrement our wNumNotify counter such that our parent can be */
/*      enabled once the count reaches zero.                            */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           -- Everything went okay.        */
/*                                                                      */
/************************************************************************/
/* 03/04/91 MSL Function created.                                       */
/* 07/02/93 KDY Added HWND typecast to return of GetProp.               */
/************************************************************************/
WORD LOCAL PASCAL NpsEventNotifyDlgDeInit( /* Exits notify dlg box      */
    HWND        hDlg)                   /* Notify Dlg Box dialog handle */
{
    extern      WORD    wNumNotify;     /* Number of notify boxes up    */
    extern      HWND    hWndScheduler;  /* Scheduler                    */
    HWND                hWndPrevActive; /* Handle to previous active wnd*/

                                        /* Take care of our previous    */
                                        /* active window property.      */
    hWndPrevActive = (HWND) GetProp (hDlg, NPS_PROP_PREV);
    RemoveProp (hDlg, NPS_PROP_PREV);

    DestroyWindow (hDlg);

                                        /* Make the previous app window */
                                        /* active if applicable.        */
    if (IsWindow (hWndPrevActive))
        {
        if (hWndPrevActive != hWndScheduler)
            {
            SetActiveWindow (hWndPrevActive);
            }
        }

                                        /* Return to caller             */
    return (NOERR);
} /* NpsEventNotifyDlgDeInit() */





#ifdef SYM_WIN32

/* **************************************************************************
@Name: VOID LOCAL OnContextHelp (
            HWND    hDlg,
            WPARAM  wParam,
            LPARAM  lParam
            )

@Description:
This function is called when the dialog box receives a WM_CONTEXTMENU and WM_HELP
windows message. The purpose of this function is to call the standard symantec help
routines that display an appropiate: floating help menu or context sensitive help

@Parameters:
$hDlg$  handle of the dialog box where the message was intercepted.

$wParam$    the wParam that was passed to WM_CONTEXTMENU message.

$lParam$    the lParam that was passed to WM_CONTEXTMENU message.

@Include: stdhelp.h
************************************************************************** */

VOID LOCAL OnContextHelp (              // Do help stuff
    HWND    hDlg,                       // [in] from this dialog
    UINT    uMessage,                   // [in] help message to deal with
    WPARAM  wParam,                     // [in] ?
    LPARAM  lParam                      // [in] ??
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
        IDC_NPS_NOTIFY_DESCR_TEXT,
        IDH_NAVW_SCHEDULER_WHATHIS_NOTIFY_DESCRIPTION,

        IDC_NPS_NOTIFY_TEXT,
        IDH_NAVW_SCHEDULER_WHATHIS_NOTIFY_MESSAGE,

        0,
        0
        };


                                        // -----------------------------------
                                        // Invoke cmn Nav context sensitive help
                                        // -----------------------------------
    StdHelpExOnContextHelp (
                        hDlg,
                        uMessage,
                        wParam,
                        lParam,
                        dwWhatsThisMap,
                        NULL,
                        NULL,           //  This dlg. doesn't have any  ...
                        NULL            //  ... List Box controls - so NULL here
                        );
}

#endif



