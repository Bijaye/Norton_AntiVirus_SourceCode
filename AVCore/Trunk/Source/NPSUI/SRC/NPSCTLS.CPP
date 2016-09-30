/* Copyright 1991-1993 Symantec, Peter Norton Computing Group           */
/************************************************************************/
/*                                                                      */
/* $Header:   S:/NPSUI/VCS/NPSCTLS.CPv   1.7   11 May 1998 12:32:20   mdunn  $ */
/*                                                                      */
/* Description:                                                         */
/*      This routine contains the routines required to manage the       */
/*      controls within the main NPS dialog box.  This includes the     */
/*      Event list, add button, edit button, copy button, and the       */
/*      remove button.                                                  */
/*                                                                      */
/* Contains:                                                            */
/*                                                                      */
/************************************************************************/
/* $Log:   S:/NPSUI/VCS/NPSCTLS.CPv  $ */
// 
//    Rev 1.7   11 May 1998 12:32:20   mdunn
// Now resizing list control columns so the columns are always wide enough
// to display all data.
// 
//    Rev 1.6   17 Apr 1998 12:20:02   mdunn
// Now calling CalculateNextRunTime() after adding/editing an event.
// 
//    Rev 1.5   30 Mar 1998 12:23:34   mdunn
// In NpsEventsLoad(), set the global flag g_bDontSaveInNpsDo before calling
// NpsEventsGet().  This may (read: almost certainly) be removed later when
// I get the alert dialog hooked up and working fully.
// 
//    Rev 1.3   24 Jul 1997 15:45:22   KSACKIN
// Fixed a problem with adding an uninitialized scheduled string if an event
// was disabled.
//
//    Rev 1.2   03 Jul 1997 18:15:50   KSACKIN
// Implemented a ListView control for showing the scheduled items.
//
//    Rev 1.1   10 Jun 1997 13:21:20   KSACKIN
// Updated to use the NPSAPI.
//
//    Rev 1.0   19 May 1997 00:25:06   KSACKIN
// Initial revision.
//
//    Rev 1.24   03 Dec 1996 16:09:14   JBRENNA
// Kirin (QAKF) port: Use CalcTabStops() instead of hard coding the tab stops
// in SetTabStops().
//
//    Rev 1.23   18 Oct 1996 22:57:22   JBRENNA
// Add tab stops for Chinese and Korean to SetTabStops(). Update the European
// and Japanese tab stops because the Frequency and Scheduled controls moved
// left.
//
//    Rev 1.22   30 Sep 1996 16:16:42   JBRENNA
// Update the tab stops in SetTabStops() because the "Scheduled" column
// had to move left for Japanese Win95. The longest "Scheduled" line was
// getting clipped.
//
//    Rev 1.21   27 Sep 1996 18:39:14   JBRENNA
// Add DBCS specific tab stops in SetTabStops().
//
//    Rev 1.20   22 Feb 1996 13:26:08   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
//
//    Rev 1.19   11 Jan 1996 09:48:40   JWORDEN
// Port changes from Quake 8
//
//    Rev 1.18.1.1   30 Nov 1995 21:03:26   BARRY
// Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
//
//    Rev 1.18   28 Jun 1995 18:10:18   KEITH
// Add asserts, and fix an invalidate rect on the desktop!
//
//    Rev 1.17   03 May 1995 13:20:04   MFALLEN
// Moved stdhelp.h higher up in the include order.
//
//    Rev 1.16   14 Mar 1995 17:08:46   MFALLEN
// Special confirmation when exiting.
//
//    Rev 1.15   06 Mar 1995 10:11:28   MFALLEN
// Made it work for the Win95 platform and added required enhancements to
// the product to give it a native Win95 look and feel.
//
//    Rev 1.14   03 Nov 1994 14:32:26   BRAD
// DBCS() enabled
//
//    Rev 1.13   29 Mar 1994 01:51:16   BRAD
// WIN32 changes
//
//    Rev 1.12   26 Mar 1994 21:00:14   BRAD
// Cleaned up Windows messages
//
//    Rev 1.11   15 Mar 1994 11:58:34   BRUCE
// Changed SYM_EXPORT to SYM_EXPORT
//
//    Rev 1.10   03 Mar 1994 12:10:08   BRAD
// AWK script applied
//
//    Rev 1.9   13 Oct 1993 14:10:18   BASIL
// Added setting of current list item after add/edit/remove to prevent
// the graying of buttons even though we have a valid event.
//
//    Rev 1.8   29 Sep 1993 14:53:34   MARKL
// No change.
//
//    Rev 1.7   23 Jul 1993 10:27:58   MARKL
// Help menu now looks like nirvana's.  Removed help/exit buttons.  All
// child dialogs are centered.
//
//    Rev 1.6   22 Jul 1993 12:02:00   MARKL
// Hooked up quake help system.
//
//    Rev 1.5   21 Jul 1993 11:32:24   DLEVITON
// Change BUTTONBAR to TOOLBAR.
//
//    Rev 1.4   10 Jun 1993 17:07:26   KYECK
// Changed IDD_NPS_CONFIGURE to CPLDLG_BUTTONBAR.
//
//    Rev 1.3   10 Jun 1993 09:32:14   KYECK
// Added support for 'yearly' events.
//
//    Rev 1.2   21 Apr 1993 22:05:02   MARKL
// Nirvana gives the schedule a chance to take an a radically new look/feel
// in the Add/Edit dialog!!!
//
//    Rev 1.1   18 Mar 1993 20:32:16   MARKL
// Cleaned up unnecessary typecasting of LPSTR.
//
//    Rev 1.0   15 Mar 1993 17:51:54   MARKL
// Initial revision.
//
//    Rev 1.8   19 Aug 1992 17:51:18   MARKL
// Fixed shadow bug.  If list box contained no entries the shadow could
// not size itself correctly since the list box had yet to adjust its
// integral height.
//
//    Rev 1.7   26 Jun 1992 17:53:14   MEIJEN
// Get rid of strids.h because NWRES will eventually disappear
//
//    Rev 1.6   19 Jun 1992 10:34:14   MEIJEN
// Decentralize resources from NWRES to NPS(scheduler) for REVOLVER
//
//    Rev 1.5   08 May 1992 20:59:38   MANSHAN
// Straightened the macro ListBox_InsertString().
//
//    Rev 1.4   06 Mar 1992 19:43:02   MARKL
// Finally fixed shadow code.
//
//    Rev 1.3   06 Mar 1992 19:37:36   MARKL
// Fixed calls to shadowing code.
//
//    Rev 1.2   28 Feb 1992 10:45:38   MARKL
// Fixed shadowing of events list box.
//
//    Rev 1.1   02 Oct 1991 12:06:38   MARKL
// Made WinStric.H pass.
//
//    Rev 1.0   28 Jul 1991 11:37:30   MARKL
// Initial revision.
//
//    Rev 1.1   02 Jun 1991 12:32:02   MARKL
// Timer start/stop was moved to NPS.C in the WM_ENABLE processing.
//
//    Rev 1.0   01 Jun 1991 20:37:00   MARKL
// Initial revision.
/************************************************************************/
#include "platform.h"
#include "stdhelp.h"
#include <windowsx.h>
#include "ngui.h"
#include "dbcs.h"
#include "msgdisp.h"                    /* error message stuff          */
#include "msgdisp.h"                   /* norton look stuff            */
#include "nps.h"                     /* scheduler includes           */
#include "npsres.h"
#include "npsstb.h"
#include "nps_hlp.h"
#include "symintl.h"


void NpsResizeListColumns ( HWND );     // from nps.cpp

//***************************************************************************
// Local Definitions
//***************************************************************************
#undef TRACE
#define TRACE   ERR_TRACE_ID (_NPSCTLS_C)




/************************************************************************/
/* Local component prototypes                                           */
/************************************************************************/
VOID LOCAL PASCAL SetTabStops(HWND,int *);

WORD DisplayEvent( HWND hDlg, WORD wEventNumber );


/************************************************************************/
/* NPSCTLS routines                                                     */
/************************************************************************/



/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine loads up any events that have been stored by       */
/*      the program schedule.  We try to be smart and set the current   */
/*      selection for the user.                                         */
/*                                                                      */
/*      NOTES:  We ASSUME that all events are written to disk once      */
/*      added or removed from the event list in real time.              */
/*      We also assume that this routine may be called due to a change  */
/*      in the internationalization crap in the win.ini file.           */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           - Everything went fine.         */
/*                                                                      */
/************************************************************************/
/* 02/12/91 MSL Function created.                                       */
/* 02/28/91 MSL Can now load events real time, replacing the current    */
/*          events.                                                     */
/* 02/28/92 MSL Now calls ShadowWindow() so shadow is correctly         */
/*              adjusted.                                               */
/* 08/19/92 MSL Fixed shadow if no entries in list box.                 */
/************************************************************************/
WORD PASCAL NpsEventsLoad (             /* Loads any stored events      */
    HWND        hDlg,                   /* Handle to dialog box         */
    BOOL        bInitLoad)              /* TRUE if initial load         */
{
    WORD                wEvent;
    extern      WORD    wNumEvents;     /* Number of events             */

                                        /* Get our internationalization */
                                        /* strings.                     */
    NpsInitTime();

                                        /* Turn painting in the list    */
                                        /* box off.                     */
    SendDlgItemMessage (hDlg, IDC_NPS_LIST, WM_SETREDRAW, FALSE, 0L);

                                        /* Initialize the list view.    */
    ListView_DeleteAllItems( GetDlgItem (hDlg, IDC_NPS_LIST) );

    g_bDontSaveInNpsDo = TRUE;
                                        /* Get events from the disk     */
    NpsEventsGet(bInitLoad);

    g_bDontSaveInNpsDo = FALSE;

                                        /* load the events into the     */
                                        /* list box.                    */
    for (wEvent = 0; wEvent < wNumEvents; wEvent++)
        {
        DisplayEvent( hDlg, wEvent );
        }

                                        /* Turn painting in the list    */
                                        /* box back on and try to turn  */
                                        /* the current selection back   */
                                        /* on.                          */
    SYM_ASSERT ( GetDlgItem (hDlg, IDC_NPS_LIST) && GetDesktopWindow() != GetDlgItem (hDlg, IDC_NPS_LIST) );
    SYM_ASSERT ( hDlg && GetDesktopWindow() != hDlg );
    SendDlgItemMessage (hDlg, IDC_NPS_LIST, WM_SETREDRAW, (WPARAM)TRUE, 0L);
    InvalidateRect (GetDlgItem (hDlg, IDC_NPS_LIST), NULL, TRUE);

    return (NOERR);
}

/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine is called when ever a WM_COMMAND message is        */
/*      directed to the Event List.  If the user has double clicked an  */
/*      event in the list box, assume he wants to modify it.  If the    */
/*      selection status of an event in the box has changed then see    */
/*      what buttons need to be Grayed Out or Restored.                 */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           - Everything went fine.         */
/*                                                                      */
/* See Also:                                                            */
/*      NpsEventControlsDisable()                                       */
/*                                                                      */
/************************************************************************/
/* 02/14/91 MSL Function created.                                       */
/* 05/02/91 MSL Added IsWindowEnabled() check to clean up display.      */
/************************************************************************/
WORD PASCAL NpsEventListMsg (           /* Handles Event List Box       */
    HWND        hDlg,                   /* Handle to dialog box         */
    UINT        uCmd)                   /* lParam of WM_COMMAND message */
{
    if (uCmd == NM_DBLCLK)
        {
        SendMessage (hDlg, WM_COMMAND,
                           GET_WM_COMMAND_MPS(IDC_NPS_EDIT,0,BBN_SELECT));
        }
    else
        {
        if (uCmd == LBN_SELCHANGE )
            {
            NpsEventControlsDisable (hDlg);
            }
        }

    return (NOERR);
} /* NpsEventListMsg() */




/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine is called when ever a WM_COMMAND message is        */
/*      directed to the Add Button.  The IDD_NPS_EVENT dialog box is    */
/*      presented to the user to collect information about the new      */
/*      event and the event is added.                                   */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           - Everything went fine.         */
/*                                                                      */
/************************************************************************/
/* 02/14/91 MSL Function created.                                       */
/* 02/18/91 MSL Localized bGotEvent.                                    */
/* 03/01/91 MSL Dialog box now takes param to figure out what title     */
/*              to display in caption.                                  */
/* 03/03/91 MSL We turn our timer off then back on.  We cannot have a   */
/*              timer on while in the Add/Edit dialog box.              */
/* 03/05/91 MSL Resources moved to NWRES.                               */
/* 06/02/91 MSL Timer manipulation is now done in NPS.C                 */
/* 02/28/92 MSL Added call to ShadowWindow().                           */
/************************************************************************/
WORD PASCAL NpsEventAddMsg (            /* Handles Add Button           */
    HWND        hDlg)                   /* Handle to dialog box         */
{
    auto    WORD            wRet;
    auto    FARPROC         lpfnAddProc;
    auto    BOOL            bGotEvent;
    extern  HINSTANCE       ghInst;
    extern  NPSEVNT         Events[];
    const   LPNPSOPTIONS    lpOpts = (LPNPSOPTIONS)GetWindowLong (
                                        hDlg,
                                        GWL_USERDATA
                                        );
    SYM_ASSERT ( lpOpts );

                                        /* Initialization               */
    wRet        = NOERR;
    lpfnAddProc = NULL;

                                        /* Check to see if we have too  */
                                        /* many events.                 */
    if (wNumEvents == MAX_EVENTS)
        {
        wRet = NPS_EVENTS_TOO_MANY;
        NMsgBox(ghInst, hDlg, wRet, TRACE, NULL, -1);
        goto NpsEventAddMsgExit;
        }

                                        /* Make the last event in the   */
                                        /* list the current event.      */
    nCurEvent = wNumEvents;
    MEMSET (&Events[nCurEvent], 0, sizeof(NPSEVNT));
    MkEvent (nCurEvent);

                                        /* Let the user enter in the    */
                                        /* data for this new event.     */
    lpfnAddProc = MakeProcInstance ((FARPROC)NpsAddEditDlgProc, ghInst);

    bGotEvent = DialogBoxParam (ghInst,
                                MAKEINTRESOURCE (IDD_NPS_EVENT),
                                hDlg,
                                (DLGPROC)lpfnAddProc,
                                MAKELPARAM (IDS_NPS_EVENT_ADD, 0));

                                        /* Add the event for the user.  */

    if ( bGotEvent && (bGotEvent != -1) && lpOpts )
        {
        CONFSTATUS Status = AskConfirmation (
                                        hDlg,
                                        CONFIRM_SAVE,
                                        lpOpts
                                        );

        if ( Status == CONFSTATUS_ALLOW )
            {
            CalculateNextRunTime ( &Events[wNumEvents] );
            wNumEvents++;
            NpsEventsPut();
            NpsEventsLoad( hDlg, FALSE );
            SetFocus( GetDlgItem (hDlg, IDC_NPS_LIST) );

            ListView_SetItemState (GetDlgItem (hDlg, IDC_NPS_LIST), wNumEvents-1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
            }
        }

                                        /* Clean up and exit            */
NpsEventAddMsgExit:
    if (lpfnAddProc)
        {
        FreeProcInstance (lpfnAddProc);
        }
    NpsEventControlsDisable(hDlg);
    return (wRet);
} /* NpsEventAddMsg() */

/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*    This function handles all notification messages to our main       */
/*    dialog.                                                           */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           - Everything went fine.         */
/*                                                                      */
/* See Also:                                                            */
/*      NpsEventControlsDisable()                                       */
/*                                                                      */
/************************************************************************/
/* 06/28/97 ksackin Function created.                                   */
/************************************************************************/
WORD NpsOnNotify(
    HWND    hDlg,
    int     nControlID,
    LPNMHDR lpNMHeader
    )
{
    DWORD           dwMessagePos;
    int             nIndex;
    LV_HITTESTINFO  lvHitTestInfo;

    switch( lpNMHeader->code )
        {
        case NM_DBLCLK:
            if ( nControlID == IDC_NPS_LIST )
                {
                lvHitTestInfo.flags=LVHT_ONITEM;

                                        // Get the position of this message.
                dwMessagePos = GetMessagePos();
                lvHitTestInfo.pt.x=LOWORD( dwMessagePos );
                lvHitTestInfo.pt.y=HIWORD( dwMessagePos );

                                        // Convert the message position to
                                        // client coordinates.
                if ( ScreenToClient( lpNMHeader->hwndFrom, &lvHitTestInfo.pt ) )
                    {
                                        // Send the hit test message.
                    nIndex = SendMessage( lpNMHeader->hwndFrom, LVM_HITTEST, 0, (LPARAM)&lvHitTestInfo );

                                        // If we got one...
                    if ( -1 != nIndex )
                        {
                                        // Set the index and edit the item.
                        nCurEvent= nIndex;
                        NpsEventEditMsg( hDlg );
                        }
                    }
                else
                    {
                                        // Should never hit this.
                    SYM_ASSERT( FALSE );
                    }
                }

        default:
            break;
        };

    return (NOERR);
}



/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine is called when ever a WM_COMMAND message is        */
/*      directed to the Edit Button.  The IDD_NPS_EVENT dialog box is   */
/*      presented to the user, with the current settings for the event  */
/*      currently selected, such that the user may make modifications   */
/*      to any of the settings.                                         */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           - Everything went fine.         */
/*                                                                      */
/************************************************************************/
/* 02/18/91 MSL Function created.                                       */
/* 02/18/91 MSL Localized bGotEvent.                                    */
/* 03/01/91 MSL Changed from modify to edit.                            */
/* 03/01/91 MSL Dialog box now takes param to figure out what title     */
/*              to display in caption.                                  */
/* 03/03/91 MSL We turn our timer off then back on.  We cannot have a   */
/*              timer on while in the Add/Edit dialog box.              */
/* 03/05/91 MSL Resources moved to NWRES.                               */
/* 06/02/91 MSL Timer manipulation is now done in NPS.C                 */
/************************************************************************/
WORD PASCAL NpsEventEditMsg (           /* Handles Edit Button          */
    HWND        hDlg)                   /* Handle to dialog box         */
{
    auto    WORD            wRet;
    auto    FARPROC         lpfnAddProc;
    auto    BOOL            bGotEvent;
    auto    HWND            hList;
    extern  HINSTANCE       ghInst;
    const   LPNPSOPTIONS    lpOpts = (LPNPSOPTIONS)GetWindowLong (
                                        hDlg,
                                        GWL_USERDATA
                                        );
    SYM_ASSERT ( lpOpts );


                                        /* Initialization               */
    wRet        = NOERR;
    lpfnAddProc = NULL;
    hList = GetDlgItem (hDlg, IDC_NPS_LIST);

                                        /* Get the current event.       */
    nCurEvent = ListView_GetNextItem( hList, -1, LVNI_SELECTED );

    if (nCurEvent == -1)
        {
        goto NpsEventEditMsg;
        }

                                        /* Pop up the Add/Edit dialog   */
                                        /* box so the user can modify   */
                                        /* the event.                   */
    lpfnAddProc = MakeProcInstance ((FARPROC)NpsAddEditDlgProc, ghInst);
    bGotEvent = DialogBoxParam (ghInst,
                                MAKEINTRESOURCE(IDD_NPS_EVENT),
                                hDlg,
                                (DLGPROC)lpfnAddProc,
                                MAKELPARAM (IDS_NPS_EVENT_EDIT, 0));

                                        /* If the event was modified    */
                                        /* then replace the event with  */
                                        /* the new one.                 */

    if ( bGotEvent && (bGotEvent != -1) && lpOpts )
        {
        CONFSTATUS Status = AskConfirmation (
                                        hDlg,
                                        CONFIRM_EDIT,
                                        lpOpts
                                        );

        if ( Status == CONFSTATUS_ALLOW )
            {
            CalculateNextRunTime ( &Events[nCurEvent] );
            NpsEventsPut();
            NpsEventsLoad( hDlg, FALSE );

                                        // Set focus to our item.
            SetFocus( GetDlgItem (hDlg, IDC_NPS_LIST) );
            ListView_SetItemState (GetDlgItem (hDlg, IDC_NPS_LIST), nCurEvent,
                                   LVIS_SELECTED | LVIS_FOCUSED,
                                   LVIS_SELECTED | LVIS_FOCUSED );
            }
        }

                                        /* Clean up and exit.           */
NpsEventEditMsg:
    if (lpfnAddProc != NULL)
        {
        FreeProcInstance (lpfnAddProc);
        }
    NpsEventControlsDisable(hDlg);
    return (wRet);
} /* NpsEventEditMsg() */




/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine is called when ever a WM_COMMAND message is        */
/*      directed to the Copy Button.  The IDD_NPS_EVENT dialog box is   */
/*      presented to the user, with the current settings for the event  */
/*      currently selected, such that the user may create a new event   */
/*      based on this selected event.                                   */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           - Everything went fine.         */
/*                                                                      */
/************************************************************************/
/* 02/18/91 MSL Function created.                                       */
/* 02/22/91 MSL No longer calls MkEvent().  Never should have...        */
/* 03/01/91 MSL Dialog box now takes param to figure out what title     */
/*              to display in caption.                                  */
/* 03/03/91 MSL We turn our timer off then back on.  We cannot have a   */
/*              timer on while in the Add/Edit dialog box.              */
/* 03/05/91 MSL Resources moved to NWRES.                               */
/* 06/02/91 MSL Timer manipulation is now done in NPS.C                 */
/************************************************************************/
WORD PASCAL NpsEventCopyMsg (           /* Handles Copy Button          */
    HWND        hDlg)                   /* Handle to dialog box         */
{
    auto    WORD            wRet;
    auto    FARPROC         lpfnAddProc;
    auto    BOOL            bGotEvent;
    extern  HINSTANCE       ghInst;
    const   LPNPSOPTIONS    lpOpts = (LPNPSOPTIONS)GetWindowLong (
                                        hDlg,
                                        GWL_USERDATA
                                        );
    SYM_ASSERT ( lpOpts );


                                        /* Initialization               */
    wRet        = NOERR;
    lpfnAddProc = NULL;

                                        /* Check to see if we have too  */
                                        /* many events.                 */
    if (wNumEvents == MAX_EVENTS)
        {
        wRet = NPS_EVENTS_TOO_MANY;
        NMsgBox(ghInst, hDlg, wRet, TRACE, NULL, -1);
        goto NpsEventCopyMsgExit;
        }

                                        /* Get the current event.       */
    nCurEvent = ListView_GetNextItem( GetDlgItem (hDlg, IDC_NPS_LIST), -1, LVNI_SELECTED );
    if (nCurEvent == -1)
        {
        goto NpsEventCopyMsgExit;
        }

                                        /* Copy our current event to    */
                                        /* our new event such that the  */
                                        /* user can modify only those   */
                                        /* items he wishes to change    */
                                        /* for this new event.          */
    MEMCPY (&Events[wNumEvents],
              &Events[nCurEvent],
              sizeof(NPSEVNT));
    nCurEvent = wNumEvents;

                                        /* Pop up the Add/Edit dialog   */
                                        /* box so the user can modify   */
                                        /* the event.                   */
    lpfnAddProc = MakeProcInstance ((FARPROC)NpsAddEditDlgProc, ghInst);
    bGotEvent = DialogBoxParam (ghInst,
                                MAKEINTRESOURCE(IDD_NPS_EVENT),
                                hDlg,
                                (DLGPROC)lpfnAddProc,
                                MAKELPARAM (IDS_NPS_EVENT_COPY, 0));

                                        /* Add the event for the user.  */

    if ( bGotEvent && (bGotEvent != -1) && lpOpts )
        {
        CONFSTATUS Status = AskConfirmation (
                                        hDlg,
                                        CONFIRM_SAVE,
                                        lpOpts
                                        );

        if ( Status == CONFSTATUS_ALLOW )
            {
            CalculateNextRunTime ( &Events[wNumEvents] );
            wNumEvents++;
            NpsEventsPut();
            NpsEventsLoad( hDlg, FALSE );
            SetFocus( GetDlgItem (hDlg, IDC_NPS_LIST) );

                                        // Set the focus to the item that we
                                        // added.
            ListView_SetItemState (GetDlgItem (hDlg, IDC_NPS_LIST), wNumEvents-1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
            }
        }

                                        /* Clean up and exit.           */
NpsEventCopyMsgExit:
    if (lpfnAddProc != NULL)
        {
        FreeProcInstance (lpfnAddProc);
        }
    NpsEventControlsDisable(hDlg);
    return (wRet);
} /* NpsEventCopyMsg() */




/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine is called when ever a WM_COMMAND message is        */
/*      directed to the Remove Button.  The selected event is removed   */
/*      from the event list.                                            */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           - Everything went fine.         */
/*                                                                      */
/************************************************************************/
/* 02/18/91 MSL Function created.                                       */
/* 02/26/91 MSL Fixed Alpha 6 bug where NpsEventsPut() was never called */
/*              when events were removed.                               */
/* 05/06/91 MSL NpsEventRemove() now calls NpsEventsPut().              */
/************************************************************************/
WORD PASCAL NpsEventRemoveMsg (         /* Handles Remove Button        */
    HWND        hDlg)                   /* Handle to dialog box         */
{
    auto    WORD        wRet = ERR;     /* Component return code        */
    auto    HWND        hList;
    extern  WORD        wNumEvents;     /* Number of events             */

    const   LPNPSOPTIONS lpOpts = (LPNPSOPTIONS)GetWindowLong (
                                        hDlg,
                                        GWL_USERDATA
                                        );
    SYM_ASSERT ( lpOpts );

    if ( lpOpts &&
        AskConfirmation ( hDlg, CONFIRM_DELETE, lpOpts ) == CONFSTATUS_ALLOW )
        {
        hList = GetDlgItem (hDlg, IDC_NPS_LIST);

                                        /* Find out the selected event. */
        nCurEvent = ListView_GetNextItem( hList, -1, LVNI_SELECTED );

        if ( nCurEvent != -1 )
            {
                                        /* Remove the event from the    */
                                        /* event list and from the      */
                                        /* IDC_NPS_LIST view.           */

            NpsEventRemove (nCurEvent);

                                        // Save and load the events to make
                                        // sure that we are updated.
            NpsEventsPut();
            NpsEventsLoad( hDlg, FALSE );
            SetFocus( GetDlgItem (hDlg, IDC_NPS_LIST) );


                                        // If we have deleted the last event,
                                        // Subtract one...
            if ( nCurEvent >= wNumEvents )
                {
                nCurEvent--;
                }

                                        // If we still have any events in the
                                        // list...
            if ( nCurEvent >= 0 )
                {
                                        // Set the focus to the next item.
                ListView_SetItemState (GetDlgItem (hDlg, IDC_NPS_LIST), nCurEvent, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
                }

                                        /* Clean up and exit.           */
            NpsEventControlsDisable(hDlg);
            wRet  = NOERR;
            }
        }

    return ( wRet );
} /* NpsEventRemoveMsg() */

/////////////////////////////////////////////////////////////////////
// WORD DisplayEvent( HWND hDlg, WORD wEventNumber )
//
// Purpose:
//   This function will display an event into our ListView control.
//
// Parameters:
//    HWND hDlg         - [IN] Handle to the dialog.
//    WORD wEventNumber - [IN] Event to display.
//
// Returns:
//    NOERR - Everything went fine, otherwise, error.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 06-23-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
WORD DisplayEvent(
    HWND hDlg,
    WORD wEventNumber
    )
{
    char                        szFmtBuf[ESIZE];
    char                        szTimeBuf[SSIZE];
    char                        szDateBuf[ESIZE];
    WORD                        ith;
    WORD                        wEnd;
    WORD                        wRet=NOERR;
    WORD                        wPm=0;
    extern      NPSEVNT         Events[];
    extern      char            ampm[NAM][SSIZE];
    extern      BOOL            b24Hour;
    auto        LV_ITEM         lvItemToAdd;
    auto        HWND            hListView;
    auto        char            pszBuf[SYM_MAX_PATH];

    hListView = GetDlgItem( hDlg, IDC_NPS_LIST );

    if ( NULL == hListView )
        {
        wRet = NPS_ERROR_DISPLAYING_EVENT;
        NMsgBox(ghInst, hDlg, wRet, TRACE, NULL, -1);
        goto BailOut;
        }
                                        // Clear structure.
    memset( &lvItemToAdd, 0, sizeof( lvItemToAdd ) );
    lvItemToAdd.mask = LVIF_TEXT;
    lvItemToAdd.iItem = wEventNumber;

/////////////////////////////////////////////////////////////////////
// Add Event Name SubItem.

                                        // Insert our desription field.
    lvItemToAdd.iSubItem = 0;
    lvItemToAdd.pszText = (LPSTR)Events[wEventNumber].szDescr;
    ListView_InsertItem( hListView, &lvItemToAdd );


/////////////////////////////////////////////////////////////////////
// Add Frequency SubItem.

    lvItemToAdd.iSubItem = 1;

                                        // Are we disabled?
    if (Events[wEventNumber].bEnabled == FALSE)
        {
                                        // Load our disabled string.
        LoadString (ghInst, IDS_NPS_DISABLED, szFmtBuf, ESIZE);
        lvItemToAdd.pszText = szFmtBuf;
        }
    else
        {
                                        // We are enabled.  Add proper
                                        // frequency string.
        lvItemToAdd.pszText = (LPSTR)freqs[Events[wEventNumber].wFreq];
        }

                                        // Add Item to proper column.
    ListView_SetItem( hListView, &lvItemToAdd );


/////////////////////////////////////////////////////////////////////
// Add Scheduled Time Fields.

    lvItemToAdd.iSubItem = 2;

    if (Events[wEventNumber].bEnabled == TRUE)
        {
                                        /* Build a string with an       */
                                        /* international time in it.    */

        NSetIntlTime (szTimeBuf,
                      Events[wEventNumber].wHour,
                      Events[wEventNumber].wMin,
                      0,
                      FALSE,
                      !b24Hour);

        switch (Events[wEventNumber].wFreq)
            {
            case NPS_FREQ_HOURLY:
                LoadString (ghInst, IDS_NPS_HOURLY, szFmtBuf, ESIZE);
                wsprintf(pszBuf,
                         szFmtBuf,
                         (LPSTR)Events[wEventNumber].wMin);
                break;

            case NPS_FREQ_DAILY:
            case NPS_FREQ_WEEKDAY:
                LoadString (ghInst, IDS_NPS_DAILY, szFmtBuf, ESIZE);
                wsprintf(pszBuf,
                         szFmtBuf,
                         (LPSTR)szTimeBuf);
                break;

            case NPS_FREQ_WEEKLY:
                LoadString (ghInst, IDS_NPS_WEEKLY, szFmtBuf, ESIZE);
                wsprintf(pszBuf,
                         szFmtBuf,
                         (LPSTR)days[Events[wEventNumber].wDay],
                         (LPSTR)szTimeBuf);
                break;

            case NPS_FREQ_MONTHLY:
                /* This chunk figures out 1st, 2nd, 3rd, 4th...20th, 21st, ... */
                /* crap.  YUK!!!!!!  Screw internationalization.               */
                wEnd = Events[wEventNumber].wDate % 10;
                ith = 3;
                if ((Events[wEventNumber].wDate < 10) ||
                    (Events[wEventNumber].wDate > 20))
                    {
                    if ((wEnd != 0) && (wEnd <= 3))
                        {
                        ith = wEnd - 1;
                        }
                    }
                LoadString (ghInst, IDS_NPS_MONTHLY, szFmtBuf, ESIZE);
                wsprintf(pszBuf,
                         szFmtBuf,
                         Events[wEventNumber].wDate,
                         (LPSTR)th[ith],
                         (LPSTR)szTimeBuf,
                         (LPSTR)ampm[wPm]);
                break;

            case NPS_FREQ_YEARLY:
                /* This chunk figures out 1st, 2nd, 3rd, 4th...20th, 21st, ... */
                /* crap.  YUK!!!!!!  Screw internationalization.               */
                wEnd = Events[wEventNumber].wDate % 10;
                ith = 3;
                if ((Events[wEventNumber].wDate < 10) ||
                    (Events[wEventNumber].wDate > 20))
                    {
                    if ((wEnd != 0) && (wEnd <= 3))
                        {
                        ith = wEnd - 1;
                        }
                    }

                LoadString (ghInst, IDS_NPS_YEARLY, (LPSTR)szFmtBuf, ESIZE);
                wsprintf(pszBuf,
                         szFmtBuf,
                         (LPSTR)months[Events[wEventNumber].wMonth],
                         Events[wEventNumber].wDate,
                         (LPSTR)th[ith],
                         (LPSTR)szTimeBuf);
                break;

            case NPS_FREQ_ONETIME:
                LoadString (ghInst, IDS_NPS_ONETIME, szFmtBuf, ESIZE);
                NSetIntlLongDate (szDateBuf,
                                  Events[wEventNumber].wMonth + 1,
                                  Events[wEventNumber].wDate,
                                  Events[wEventNumber].wYear,
                                  FALSE);
                wsprintf(pszBuf,
                         szFmtBuf,
                         (LPSTR)szDateBuf,
                         (LPSTR)szTimeBuf);
                break;

            default:
                                        // -----------------------------------
                                        // Some frequency that we don't know?
                                        // -----------------------------------
                SYM_ASSERT ( 0 );
                break;
            }
                                        // Set our text field to our custom
                                        // string.
        lvItemToAdd.pszText = pszBuf;

                                        // Add scheduled custom string column.
        ListView_SetItem( hListView, &lvItemToAdd );
        }

    NpsResizeListColumns ( hDlg );

BailOut:

    return (wRet);
}

