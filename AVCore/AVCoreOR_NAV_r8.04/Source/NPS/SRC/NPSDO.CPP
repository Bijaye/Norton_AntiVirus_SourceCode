/* Copyright 1991-1993 Symantec, Peter Norton Computing Group           */
/************************************************************************/
/*                                                                      */
/* $Header:   S:/NPS/VCS/npsdo.cpv   1.40   28 Aug 1998 16:55:44   JBRENNA  $ */
/*                                                                      */
/* Description:                                                         */
/*      This file contains those routines which perform events or       */
/*      announce their removal.                                         */
/*                                                                      */
/* Contains:                                                            */
/*                                                                      */
/************************************************************************/
/* $Log:   S:/NPS/VCS/npsdo.cpv  $ */
// 
//    Rev 1.40   28 Aug 1998 16:55:44   JBRENNA
// Port changes from QAKN branch to trunk.
// 
//    Rev 1.39   11 May 1998 10:57:20   mdunn
// Tweaks to the list control - the columns are now auto-resized when items
// are added/deleted and made wide enough to show all data.
// 
//    Rev 1.38   27 Apr 1998 15:00:28   mdunn
// Now calling MessageBeep() before displaying missed event dialogs so that
// the user gets an audible alert as well.
// 
//    Rev 1.37   15 Apr 1998 17:26:24   mdunn
// Revamped handling of missed recurring events.
// 
//    Rev 1.36   13 Apr 1998 17:02:58   JALLEE
// Fixed the last fix so we copy the contents of static buffer returned by
// localtime() before calling localtime() again.
//
//    Rev 1.35   09 Apr 1998 16:51:30   JALLEE
// Check localtime() return for NULL ptr.
//
//    Rev 1.34   23 Mar 1998 22:10:20   mdunn
// Added support for running missed recurring events on Win 9x.
//
//    Rev 1.33   17 Mar 1998 15:54:40   mdunn
// Made NpsEventDo() non-static.  Added support for running missed events to
// NpsEventExpiredDisplay() and NpsEventExpiredDlgProc().
//
//    Rev 1.32   15 Jul 1997 15:13:08   DBUCHES
// Added Ctl3d Support for DEC Alpha
//
//    Rev 1.31   03 Jul 1997 11:58:20   KSACKIN
// Changed a check to get the count of our list of scheduled items to do a
// ListView_GetItemCount() vs. a ListBox_GetCount().
//
//    Rev 1.30   07 May 1997 18:52:12   BILL
// Removed ctl3d for Alpha platform
//
//    Rev 1.29   02 Oct 1996 18:33:16   GWESTER
// Fixed so that DBCS programs and directories are recognized by
// Scheduler when it validates command lines.
//
//    Rev 1.28   18 Sep 1996 10:14:32   JBRENNA
// Add NG_SetFontsInDialog() call when processing WM_INITDIALOG messages.
// This ensures that a DBCS font is used when necessary.
//
//    Rev 1.27   22 Feb 1996 13:15:02   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' and 'StdHelpOnContextHelp()' to
// 'StdHelpExOnContextHelp()' to use Quake's StdHelp merged with the NAV
// version.
//
//    Rev 1.26   11 Jan 1996 09:49:04   JWORDEN
// Port changes from Quake 8
//
//    Rev 1.25.1.1   30 Nov 1995 21:03:32   BARRY
// Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
//
//    Rev 1.25   01 Jul 1995 16:36:38   MARKL
// I believe I found where all of the LFN processing was broken.  It turns out
// that the code had been ifdef 0-ed in previous areas to allow it to simply
// run instead of fixing all of the LFN issues.  Items that I am aware of
// fixing (although quickly for the build):
//   1)  When browsing, you cannot pass quoted file specs to the browse
//       dialogs.
//   2)  When returning from a browse, if the spec is a LFN with a space
//       in it, the spec needs to be surrounded by quotes.
//   3)  When parsing parameters, go after the first space on SFN but
//       after the quoted spec on an LFN with a space.
//
//    Rev 1.24   29 Jun 1995 22:21:18   BARRY
// Fixed help link. (Gary W.)
//
//    Rev 1.23   03 May 1995 13:19:32   MFALLEN
// Moved stdhelp.h higher up in the include order.
//
//    Rev 1.22   25 Apr 1995 19:52:22   BRAD
// Enabled Ctl3d and got working for NT
//
//    Rev 1.21   18 Apr 1995 14:11:02   MFALLEN
// LFN program scheduling.
//
//    Rev 1.20   14 Mar 1995 13:02:06   MFALLEN
// Added How To help.
//
//    Rev 1.19   13 Mar 1995 17:12:24   MFALLEN
// Added more right click help.
//
//    Rev 1.18   13 Mar 1995 15:34:56   MFALLEN
// Added what's this help. No help file to test with at present time.
//
//    Rev 1.17   06 Mar 1995 10:11:38   MFALLEN
// Made it work for the Win95 platform and added required enhancements to
// the product to give it a native Win95 look and feel.
//
//    Rev 1.16   26 Mar 1994 21:00:10   BRAD
// Cleaned up Windows messages
//
//    Rev 1.15   15 Mar 1994 11:58:28   BRUCE
// Changed SYM_EXPORT to SYM_EXPORT
//
//    Rev 1.14   03 Mar 1994 12:10:10   BRAD
// AWK script applied
//
//    Rev 1.13   07 Oct 1993 18:28:02   BASIL
// [FIX] save and restore cursor during program execution (#10621)
//
//    Rev 1.12   01 Oct 1993 13:00:18   MARKL
// Now fully supports named events.
//
//    Rev 1.11   29 Sep 1993 14:53:50   MARKL
// No change.
//
//    Rev 1.10   23 Jul 1993 10:27:38   MARKL
// Help menu now looks like nirvana's.  Removed help/exit buttons.  All
// child dialogs are centered.
//
//    Rev 1.9   22 Jul 1993 12:01:54   MARKL
// Hooked up quake help system.
//
//    Rev 1.8   02 Jul 1993 06:58:54   KYECK
// Fixed STRICT warnings/errors.
//
//    Rev 1.7   10 Jun 1993 13:23:28   KYECK
// Modified NpsAnnounceEvent() by adding bSound parameter and only
// calling MessageBeep if bSound == TRUE.
//
//    Rev 1.6   10 Jun 1993 09:32:06   KYECK
// Added support for 'yearly' events.
//
//    Rev 1.5   05 May 1993 09:13:58   MARKL
// Now splits out the parameters from the command line such that they may
// be correctly passed to ShellExecute().
//
//    Rev 1.4   22 Apr 1993 15:51:32   MARKL
// Finally found/fixed bug where an event could be launched more than once
// in the same minute (even though we are only supposed to scan events once
// per minute!!!).  The problem was in the check to see if the event was
// scheduled between the last time events were checked and the current time.
//
// The code:
//     if (wLastMin < wCurMin)
//         checkstuff
//     else
//         check hour boundaries stuff
//
//
// should have been:
//     if (wLastMin < wCurMin)
//         checkstuff
//     elseif (wLastMin != wCurMin)
//         check hour boundaries stuff
//
//
// Originally written 4/22/91, function finally fixed on 4/22/93
//
//    Rev 1.3   22 Apr 1993 11:29:32   MARKL
// Fixed bugs where parameters to NMsgBox() (a variable argument function)
// were not being typecasted correctly.
//
//    Rev 1.2   21 Apr 1993 22:05:04   MARKL
// Nirvana gives the schedule a chance to take an a radically new look/feel
// in the Add/Edit dialog!!!
//
//    Rev 1.1   18 Mar 1993 20:32:04   MARKL
// Cleaned up unnecessary typecasting of LPSTR.
//
//    Rev 1.0   15 Mar 1993 17:51:18   MARKL
// Initial revision.
//
//    Rev 1.6   26 Jun 1992 17:55:16   MEIJEN
// Get rid of strids.h because NWRES will eventually disappear
//
//    Rev 1.5   19 Jun 1992 10:34:02   MEIJEN
// Decentralize resources from NWRES to NPS(scheduler) for REVOLVER
//
//    Rev 1.4   05 Feb 1992 20:50:04   MICHAELR
// Adjusted values NPS_RUN_..
//
//    Rev 1.3   05 Feb 1992 19:52:30   MICHAELR
// Support for new NWinExec().
//
//    Rev 1.2   19 Jan 1992 12:28:12   MARKL
// Now handles common properties.  Still need to add support for the
// new NWinExec function and the Standard Directory Browse.
//
//    Rev 1.1   02 Oct 1991 12:06:46   MARKL
// Made WinStric.H pass.
//
//    Rev 1.0   28 Jul 1991 11:37:36   MARKL
// Initial revision.
//
//    Rev 1.2   07 Jun 1991 10:09:22   MARKL
// No longer uses sound routines.  These routines should not be used
// according to Microsoft.  We'll have to wait for the multimedia
// stuff to use sound since the SDK routines suck.
//
//    Rev 1.1   03 Jun 1991 12:19:56   MARKL
// Disables timer if doing an NWinExec.
//
//    Rev 1.0   01 Jun 1991 20:35:16   MARKL
// Initial revision.
/************************************************************************/
#include    "platform.h"
#include    "stdhelp.h"
#include    "xapi.h"
#include    <ctl3d.h>
#include    <shellapi.h>
#include    <sys\timeb.h>
#include    <time.h>
#include    "undoc.h"                  /* for extra typedefs           */
#include    "msgdisp.h"                    /* error message stuff          */
#include    "msgdisp.h"
#include    "symintl.h"
#include    "ngui.h"
#include    "nps.h"                     /* scheduler includes           */
#include    "npsres.h"
#include    "npsstb.h"
#include    "nps_hlp.h"
#include    "dbcs.h"

#undef TRACE                            // We don't want the MFC TRACE.
#define TRACE   ERR_TRACE_ID (_NPSDO_C)

void NpsResizeListColumns(HWND);        // from nps.cpp


/************************************************************************/
/* Local component prototypes                                           */
/************************************************************************/

BOOL LOCAL PASCAL NpsCheckHourMin (
    HWND        hDlg,
    WORD        wEventHour,
    WORD        wEventMin,
    WORD        wLastHour,
    WORD        wLastMin,
    WORD        wCurHour,
    WORD        wCurMin,
    BOOL        bCheckHour);

WORD LOCAL PASCAL NpsAnnounceEvent (    /* Anounces event with melody   */
    BOOL    bSound                      /* Sound?                       */
    );

VOID LOCAL OnExpiredContextHelp (       // Do help stuff
    HWND    hDlg,                       // [in] from this dialog
    UINT    uMessage,                   // [in] help message to deal with
    WPARAM  wParam,                     // [in] ?
    LPARAM  lParam                      // [in] ??
    );

WORD LOCAL NpsExecute (                 // Comvert to short params and exec.
    HWND    hParent,                    // [in] parent window handle
    LPSTR   lpszCmd,                    // [in] command to execute
    LPSTR   lpszParams,                 // [in] parameters
    LPSTR   lpszStartupDir,             // [in] startup dir
    int     nRunStyle                   // [in] run style
    );

/************************************************************************/
/* NPSDO routines                                                       */
/************************************************************************/




/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine checks all the events to see if it its time to     */
/*      execute one of them.  This routine checks all the events in the */
/*      event list just in case some of the same trigger time.          */
/*      Note that LastTime is brought into the check since we may not   */
/*      receive timer messages exactly when we want (i.e. we may get a  */
/*      timer message minutes after the event was supposed to be        */
/*      executed).  Checking against LastTime makes sure that events    */
/*      are not lost.                                                   */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                                                           */
/************************************************************************/
/* 1/5/91 CJR Function created.                                         */
/* 03/04/91 MSL Made clean up pass.  Added bRemoved parameter to        */
/*              NpsEventDo() call such that all events could be checked.*/
/*              Routine use to exit after first hit.                    */
/* 03/11/91 MSL Fixed logic bug.  If there was a weekly event, loop     */
/*              would go on infinately.                                 */
/* 04/22/91 MSL Added check with LastTime since an event may not be     */
/*              triggered at the EXACT time desired.                    */
/* 06/08/93 KDY Added support for yearly events.                        */
/************************************************************************/
WORD PASCAL NpsEventsCheck (            /* Do we need to do an event?   */
    HWND        hDlg)                   /* Handle to main dialog box    */
{
    struct      tm      *curtm;
    struct      tm      *Lasttm;
    struct      tm      *tmCurrentLocal;
    struct      tm      *tmLastLocal;
    struct      _timeb  curtimeb;
    struct      tm      curtmbuf;
    struct      tm      LastTimebuf;
    WORD                wEvent;
    BOOL                bRemoved;
    extern struct _timeb LastTime;      /* Last time processed.         */
    extern      NPSEVNT Events[];       /* The actual events            */

    /* Get the current date & time. */
    /* We need to store curtmbuf    */
    /* since calls to localtime()   */
    /* use an internal static that  */
    /* is reused on each call to    */
    /* localtime().                 */

    _ftime(&curtimeb);
    tmCurrentLocal = localtime (&curtimeb.time);
    // This most likely occurs when
    // not 1970 < year < 2038.
    // Just fail silently.
    if (NULL == tmCurrentLocal)
        return (NOERR);

    MEMCPY (&curtmbuf, tmCurrentLocal, sizeof (struct tm));
    curtm  = &curtmbuf;

    tmLastLocal = localtime (&LastTime.time);
    // If the localtime fails here,
    // get a new last time and
    // return.
    if (NULL != tmLastLocal)
    {

        MEMCPY (&LastTimebuf, tmLastLocal, sizeof (struct tm));
        Lasttm = &LastTimebuf;

        /* Loop through the events      */
        /* looking to see if we have    */
        /* a match.                     */
        wEvent = 0;
        while (wEvent < wNumEvents)
        {
            bRemoved = FALSE;
            /* See if we have a match.      */
            if (Events[wEvent].bEnabled == FALSE)
            {
                goto NpsEventsCheckContinue;
            }
            switch (Events[wEvent].wFreq)
            {
            case NPS_FREQ_HOURLY:
                if (NpsCheckHourMin (hDlg,
                    0,
                    Events[wEvent].wMin,
                    0,
                    (WORD)Lasttm->tm_min,
                    0,
                    (WORD)curtm->tm_min,
                    FALSE))
                {
                    NpsEventDo (hDlg, wEvent, &bRemoved);
                }
                break;

            case NPS_FREQ_DAILY:
                if (NpsCheckHourMin (hDlg,
                    Events[wEvent].wHour,
                    Events[wEvent].wMin,
                    (WORD)Lasttm->tm_hour,
                    (WORD)Lasttm->tm_min,
                    (WORD)curtm->tm_hour,
                    (WORD)curtm->tm_min,
                    TRUE))
                {
                    NpsEventDo (hDlg, wEvent, &bRemoved);
                }
                break;

            case NPS_FREQ_WEEKDAY:
                if (((WORD)curtm->tm_wday == SUN) ||
                    ((WORD)curtm->tm_wday == SAT))
                {
                    goto NpsEventsCheckContinue;
                }
                if (NpsCheckHourMin (hDlg,
                    Events[wEvent].wHour,
                    Events[wEvent].wMin,
                    (WORD)Lasttm->tm_hour,
                    (WORD)Lasttm->tm_min,
                    (WORD)curtm->tm_hour,
                    (WORD)curtm->tm_min,
                    TRUE))
                {
                    NpsEventDo (hDlg, wEvent, &bRemoved);
                }
                break;

            case NPS_FREQ_WEEKLY:
                if ((WORD)curtm->tm_wday != Events[wEvent].wDay)
                {
                    goto NpsEventsCheckContinue;
                }
                if (NpsCheckHourMin (hDlg,
                    Events[wEvent].wHour,
                    Events[wEvent].wMin,
                    (WORD)Lasttm->tm_hour,
                    (WORD)Lasttm->tm_min,
                    (WORD)curtm->tm_hour,
                    (WORD)curtm->tm_min,
                    TRUE))
                {
                    NpsEventDo (hDlg, wEvent, &bRemoved);
                }
                break;

            case NPS_FREQ_MONTHLY:
                if ((WORD)curtm->tm_mday != Events[wEvent].wDate)
                {
                    goto NpsEventsCheckContinue;
                }
                if (NpsCheckHourMin (hDlg,
                    Events[wEvent].wHour,
                    Events[wEvent].wMin,
                    (WORD)Lasttm->tm_hour,
                    (WORD)Lasttm->tm_min,
                    (WORD)curtm->tm_hour,
                    (WORD)curtm->tm_min,
                    TRUE))
                {
                    NpsEventDo (hDlg, wEvent, &bRemoved);
                }
                break;

            case NPS_FREQ_YEARLY:
                if ((WORD)curtm->tm_mon != Events[wEvent].wMonth ||
                    (WORD)curtm->tm_mday != Events[wEvent].wDate)
                {
                    goto NpsEventsCheckContinue;
                }
                if (NpsCheckHourMin (hDlg,
                    Events[wEvent].wHour,
                    Events[wEvent].wMin,
                    (WORD)Lasttm->tm_hour,
                    (WORD)Lasttm->tm_min,
                    (WORD)curtm->tm_hour,
                    (WORD)curtm->tm_min,
                    TRUE))
                {
                    NpsEventDo (hDlg, wEvent, &bRemoved);
                }
                break;

            case NPS_FREQ_ONETIME:
                if ((WORD)curtm->tm_year+1900 != Events[wEvent].wYear)
                {
                    goto NpsEventsCheckContinue;
                }
                if ((WORD)curtm->tm_mon != Events[wEvent].wMonth)
                {
                    goto NpsEventsCheckContinue;
                }
                if ((WORD)curtm->tm_mday != Events[wEvent].wDate)
                {
                    goto NpsEventsCheckContinue;
                }
                if (NpsCheckHourMin (hDlg,
                    Events[wEvent].wHour,
                    Events[wEvent].wMin,
                    (WORD)Lasttm->tm_hour,
                    (WORD)Lasttm->tm_min,
                    (WORD)curtm->tm_hour,
                    (WORD)curtm->tm_min,
                    TRUE))
                {
                    NpsEventDo (hDlg, wEvent, &bRemoved);
                }
                break;
            }
            /* If event was not removed,    */
            /* increment our index.         */
NpsEventsCheckContinue:
            if (!bRemoved)
            {
                wEvent++;
            }
        }
    }
    /* Store this time and return   */
    /* to our caller.               */
    LastTime = curtimeb;

    return (NOERR);
} /* NpsEventsCheck() */




/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine checks to see if the hour and minute of the event  */
/*      is greater than that of the last time an event was checked and  */
/*      less than or equal to the current time.  This gets a little     */
/*      hairy since we may not wish to check the hour in some cases     */
/*      (i.e. hourly events) and since the last check could have been   */
/*      done before the hour and the current time is after the hour.    */
/*      In short "Abandon Hope, All ye who enter here..."               */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           -- Function completed okay.     */
/*                                                                      */
/************************************************************************/
/* 04/22/91 MSL Function created.                                       */
/* 04/22/93 MSL Finally fixed bug where a event could be launched more  */
/*              than once in the same minute...  UGGG                   */
/************************************************************************/
BOOL LOCAL PASCAL NpsCheckHourMin (
    HWND        hDlg,
    WORD        wEventHour,
    WORD        wEventMin,
    WORD        wLastHour,
    WORD        wLastMin,
    WORD        wCurHour,
    WORD        wCurMin,
    BOOL        bCheckHour)
{
    BOOL        bOk;

    bOk = FALSE;

    if (wLastMin < wCurMin)
        {
                                        /* In this case both the last   */
                                        /* and current time are within  */
                                        /* the same hour.  This makes   */
                                        /* the validation simple.       */
        if ((wEventMin >  wLastMin) &&
            (wEventMin <= wCurMin))
            {
            if (bCheckHour)
                {
                if (wEventHour == wCurHour)
                    {
                    bOk = TRUE;
                    }
                }
            else
                {
                bOk = TRUE;
                }
            }
        }
    else if (wLastMin != wCurMin)
        {
                                        /* In this case the last time   */
                                        /* was from an hour previous to */
                                        /* the current time.            */
        if ((wEventMin > wLastMin) && (wEventMin < 60))
            {
            if (bCheckHour)
                {
                                        /* Since the event was from the */
                                        /* previous hour, we must       */
                                        /* compare to the previous hour.*/
                if (wEventHour == wLastHour)
                    {
                    bOk = TRUE;
                    }
                }
            else
                {
                bOk = TRUE;
                }
            }
        else if (wEventMin <= wCurMin)
            {
            if (bCheckHour)
                {
                                        /* Since the event was from     */
                                        /* this hour we must compare    */
                                        /* to the current hour.         */
                if (wEventHour == wCurHour)
                    {
                    bOk = TRUE;
                    }
                }
            else
                {
                bOk = TRUE;
                }
            }

        }

    return bOk;
} /* NpsCheckHourMin() */




/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      An event has been triggered.  Either display the message or     */
/*      launch the specified program for this event.                    */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           -- Function completed okay.     */
/*                                                                      */
/* See Also:                                                            */
/*      NpsAnnounceEvent()                                              */
/************************************************************************/
/* 1/5/91 CJR Function created.                                         */
/* 02/26/91 MSL Outputs a meaningful error message if event application */
/*              could not be launched by WinExec().                     */
/*              If a message is displayed, control is returned back to  */
/*              the window that was previously active.  Also added      */
/*              "Melody Tone of 3 Notes".                               */
/* 03/04/91 MSL Now tells parent if event was removed from the list     */
/*              via the pbRemoved parameter.                            */
/* 03/30/91 MSL Now calls NWinExec() such that data files can launch    */
/*              their respective app via association.                   */
/* 04/22/91 MSL Now looks at bRunMinimized flag.                        */
/* 05/06/91 MSL NpsEventRemove() now calls NpsEventsPut().              */
/* 06/03/91 MSL If this is a EVENT, the timer is killed and started     */
/*              incase a modal dialog box pops up.                      */
/* 01/19/92 MSL Needs to use new NWinExec function....                  */
/* 03/15/93 MSL Uses ShellExecute() to launch applications.             */
/* 07/02/93 KDY Added WORD typecast to return of ShellExecute.          */
/************************************************************************/
WORD PASCAL NpsEventDo (                /* Executes event               */
    HWND        hWnd,                   /* Window handle                */
    WORD        wEvent,                 /* Index to event to execute    */
    PBOOL       pbRemoved)              /* TRUE if removed from list    */

{
    WORD                wRet;
    char                szBuf[ESIZE];
    char                szCmd[SYM_MAX_PATH + 1];
    char                szParams[SYM_MAX_PATH + 1];
    WORD                wError;
    HCURSOR             hcurSav;
    extern      NPSEVNT Events[];       /* The actual events            */


                                        /* Initialization               */
    *pbRemoved = FALSE;

                                        /* Announce that an event has   */
                                        /* occurred.                    */
    NpsAnnounceEvent(Events[wEvent].bSound);

                                        /* If the event was a program   */
                                        /* execute it otherwise display */
                                        /* the message to the user.     */
    if ((Events[wEvent].wType == EVENT_TYPE_PROGRAM) ||
        (Events[wEvent].wType == EVENT_TYPE_NAMED))
        {
                                        // We need to kill our timer and
                                        // split out our params from our
                                        // command.
        NpsTimerKill (hWnd);
        NpsReturnCmdParams (Events[wEvent].szCmdLine, szCmd, szParams);

                                        // If this is a named event, we have
                                        // even more params!
        STRCAT (szParams, " ");
        STRNCAT (szParams,
                 Events[wEvent].szPromptParams,
                 sizeof (szParams) - STRLEN(szParams) - 1);
        szParams[sizeof (szParams) - 1] = EOS;

                                        //-------------------------------
                                        // Turn cursor off, and remember it
                                        // this prevents the hourglass cursor
                                        // caused by subsequent scheduler paint
                                        // message from 'sticking' into other
                                        // programs e.g. nddw (#10620)
                                        //-------------------------------
        hcurSav = SetCursor(NULL);
                                        // Execute the command

        wRet = NpsExecute (
                        hWnd,
                        szCmd,
                        szParams,
                        Events[wEvent].szStartupDir,
                        Events[wEvent].wRunStyle+1
                        );

                                        //-------------------------------
                                        // Restore cursor
                                        //-------------------------------
        SetCursor(hcurSav);

        if ((wRet < 32) || (wRet == ERR))
            {
            switch (wRet)
                {
                case 0:
                    wError = IDS_ERR_MEM_CANT_ALLOC;
                    NMsgBox(ghInst,hWnd,
                             ERR_SEV_NUM (ERR_SEV_WARNING, wError),
                             TRACE,
                             NULL,
                             -1);
                    break;
                case 2:
                    wError = IDS_NPS_FILE_NOT_FOUND;
                    break;
                case 3:
                    wError = IDS_NPS_PATH_NOT_FOUND;
                    break;
                case 10:
                    wError = IDS_NPS_WRONG_VERSION;
                    break;
                default:
                    wError = IDS_NPS_BAD_EXE;
                    break;
                }
            if (wRet != 0)
                {
                NMsgBox (ghInst,
                         hWnd,
                         ERR_SEV_NUM (ERR_SEV_WARNING, wError),
                         TRACE,
                         NULL,
                         -1,
                         (LPSTR)Events[wEvent].szDescr,
                         (LPSTR)Events[wEvent].szCmdLine);
                }
            }
        NpsTimerStart (hWnd);
        }
    else
        {
                                        /* Event type is message so     */
                                        /* display the message.         */
        LoadString (ghInst, IDS_NPS_TITLE, szBuf, ESIZE);
        NpsEventNotifyDisplay (hWnd, wEvent);
        }

                                        /* Delete a one-time event from */
                                        /* the list.                    */
    if (Events[wEvent].wFreq == NPS_FREQ_ONETIME)
        {
        *pbRemoved = TRUE;
        NpsEventRemove (hWnd, wEvent);
        ListView_DeleteItem (GetDlgItem (hWnd, IDC_NPS_LIST), wEvent);
        NpsEventControlsDisable (hWnd);
        NpsResizeListColumns (hWnd);
        }
    else
        {
                                        // Set the event's next run time.
        CalculateNextRunTime ( &Events[wEvent] );

                                        // Save SCHEDULE.DAT to store the
                                        // last-run time of the event.
        NpsEventsPut ( hWnd );

        if ( !g_bDontSaveInNpsDo )
            {
            NpsEventsLoad ( hWnd, FALSE );
            }
        }

    return (NOERR);
}




/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine announces that an event has occured by playing     */
/*      a three note melody.  If we cannot play the tune we simply      */
/*      beep (sic).                                                     */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           -- Everything went okay.        */
/*                                                                      */
/************************************************************************/
/* 02/26/91 MSL Function created.                                       */
/* 06/07/91 MSL Got rid of announcement melody since Mr. Gates' shit    */
/*              don't work.  One must never call the Sound routines     */
/*              in the SDK according to their tech support.             */
/* 06/09/93 KDY Changed messagebox param to MB_ICONASTERISK so it will  */
/*              pickup default sound if driver is installed.            */
/* 06/10/93 KDY Added support for sound flag.                           */
/************************************************************************/
WORD LOCAL PASCAL NpsAnnounceEvent (    /* Anounces event with melody   */
    BOOL    bSound)                     /* Sound?                       */
{
    if (bSound)
        {
        MessageBeep(MB_ICONASTERISK);
        MessageBeep(MB_ICONASTERISK);
        MessageBeep(MB_ICONASTERISK);
        }

    return (NOERR);
} /* NpsAnnounceEvent() */



//***************************************************************************
// NpsReturnCmdParams()
//
// Description:
//      Look at the full command line that is passed in and copy the command
// line and parameter parts to their respective buffers.
//
// Parameters:
//      lpszFullCmdLine                 [in] Full command line
//      lpszCmd                         [out] Buffer to receive cmd
//      lpszParams                      [out] Buffer to receive params
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 05/05/1993 MARKL Function Created.
// 07/01/1995 MARKL Function was destroyed by our buddy now at McAfee.  Fixed.
//***************************************************************************

VOID WINAPI NpsReturnCmdParams (        // Return command and its parameters
    LPCSTR      lpszFullCmdLine,        // [in] Full command line to parse
    LPSTR       lpszCmd,                // [out] Buffer to receive command
    LPSTR       lpszParams)             // [out] Buffer to receive parameters
{
    LPCSTR      lpszFrom;
    LPSTR       lpszTo;
    char        cCheckFor;

    SYM_ASSERT ( lpszFullCmdLine );
    SYM_ASSERT ( lpszCmd );

                                        // First we parse out the command
                                        // line.  This may be a simple SFN
                                        // or a quoted LFN.  See if we are
                                        // looking for the first space or
                                        // a matching quote.
    if ( *lpszFullCmdLine == '\"' )
        cCheckFor = '\"';
    else
        cCheckFor = ' ';

                                        // Copy Command to lpszCmd, making
                                        // sure that if it is quoted, that
                                        // the matching quote makes it into
                                        // the command as well.
    lpszFrom = lpszFullCmdLine;
    lpszTo   = lpszCmd;
    do
        {
        *lpszTo  = *lpszFrom;
        if (DBCSIsLeadByte(*lpszFrom))
            {*(lpszTo+1)  = *(lpszFrom+1);}

        lpszTo   = CharNext ( lpszTo );
        lpszFrom = CharNext ( lpszFrom );
        } while ( (*lpszFrom != cCheckFor) && (*lpszFrom != EOS) );
    if ( *lpszFrom == '\"' )
        {
        *lpszTo  = *lpszFrom;
        if (DBCSIsLeadByte(*lpszFrom))
            {*(lpszTo+1)  = *(lpszFrom+1);}

        lpszTo   = CharNext ( lpszTo );
        lpszFrom = CharNext ( lpszFrom );
        }
    *lpszTo = EOS;

                                        // Nuke any white space between the
                                        // command and its parameters.
    while ( *lpszFrom == ' ' )
        lpszFrom = CharNext ( lpszFrom );

                                        // If there is anything left, its
                                        // parameters.
    if ( lpszParams != NULL)
        STRCPY (lpszParams, lpszFrom);
}



/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine displays our Expired Event dialog box to our user. */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           -- Everything went okay.        */
/*      others                          -- Errors from called routines. */
/*                                                                      */
/************************************************************************/
/* 03/29/91 MSL Function Created.                                       */
/************************************************************************/
WORD PASCAL NpsEventExpiredDisplay (    /* Do we remove expired event?  */
    HWND        hDlg,                   /* Application window handle    */
    WORD        wEvent,                 /* Event to possibly remove     */
    int*        pnAction)               /* What to do with the event    */
{
    WORD                        wRet;
    FARPROC                     lpfnExpired;
    extern      HINSTANCE       ghInst;

                                        /* Initialization               */
    wRet        = NOERR;
    lpfnExpired = NULL;

                                        /* Create the dialog box and    */
                                        /* find out if we nuke the      */
                                        /* event.                       */
    lpfnExpired = MakeProcInstance ((FARPROC)NpsEventExpiredDlgProc, ghInst);

    MessageBeep ( MB_ICONEXCLAMATION );

    *pnAction = DialogBoxParam (ghInst,
                                MAKEINTRESOURCE (IDD_NPS_EXPIRED),
                                hDlg,
                                (DLGPROC)lpfnExpired,
                                MAKELPARAM (wEvent, 0));

                                        /* Return to our caller         */
    if (lpfnExpired)
        {
        FreeProcInstance (lpfnExpired);
        }
    return (NOERR);
} /* NpsEventExpiredDisplay() */


/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This dialog box asks the user whether or not he wishes to       */
/*      remove an expired event from the event list.  This dialog box   */
/*      is only for one-time events that never were triggered.          */
/*                                                                      */
/* Return values:                                                       */
/*      EndDialog (hDlg, TRUE)  -- NUKE the event.                      */
/*      EndDialog (hDlg, FALSE) -- Keep it in the list.                 */
/*                                                                      */
/************************************************************************/
/* 03/29/91 MSL Function created.                                       */
/* 04/27/91 MSL Added support for standard help                         */
/* 07/02/93 KDY Added BOOL typecast to return of Ctl3dColorEx.          */
/************************************************************************/
BOOL SYM_EXPORT CALLBACK NpsEventExpiredDlgProc(   /* Do we nuke expired event?    */
    HWND        hDlg,                   /* Handle to dialog box         */
    UINT        wMessage,               /* Message to process           */
    WPARAM      wParam,                 /* WPARAM parameter             */
    LPARAM      lParam)                 /* LPARAM parameter             */
{
    char                        szFmtBuf[ESIZE];
    char                        szDateBuf[ESIZE];
    char                        szTimeBuf[ESIZE];
    char                        szSchedTimeBuf[MAX_EVENT_SIZE];
	char                        szEventCmdLine[MAX_CMDLINE];
    LPSTR                       lpszBuf;
    WORD                        wEvent;
    extern      NPSEVNT         Events[];
    HICON                       hicon;
    HMENU                       hSysMenu;

                                        /* Process the message we have  */
                                        /* been passed.                 */
    switch (wMessage)
       {
                                        /* We are being initialized so  */
                                        /* Load up our display statics  */
                                        /* with the name of the event,  */
                                        /* the time it was to have been */
                                        /* executed, and the command/   */
                                        /* message.  Also set focus to  */
                                        /* the correct control.         */
        case WM_INITDIALOG:
            NG_CenterDialog (hDlg, GetParent (hDlg));
            wEvent = LOWORD (lParam);

            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);

                                            // Setup the Fonts. Finds the correct
                                            // DBCS font if DBCSInstalled().
            NG_SetFontsInDialog (hDlg, 0);

            LoadString (ghInst, IDS_NPS_SCHED_TIME, szFmtBuf, ESIZE);

            NSetIntlTime (szTimeBuf,
                          Events[wEvent].wHour,
                          Events[wEvent].wMin,
                          0,
                          FALSE,
                          !b24Hour);

            NSetIntlLongDate (szDateBuf,
                              Events[wEvent].wMonth + 1,
                              Events[wEvent].wDate,
                              Events[wEvent].wYear,
                              FALSE);

            SPRINTF ( szSchedTimeBuf,
                      szFmtBuf,
                     (LPSTR)szDateBuf,
                     (LPSTR)szTimeBuf);

            SetDlgItemText (hDlg, IDC_NPS_EXPIRED_SCHEDTIME, szSchedTimeBuf);
            SetDlgItemText (hDlg, IDC_NPS_EXPIRED_DESCR_TEXT, Events[wEvent].szDescr);

            if (Events[wEvent].wType == EVENT_TYPE_PROGRAM )
                {
				STRCPY(szEventCmdLine, Events[wEvent].szCmdLine);

                lpszBuf = strrchr(szEventCmdLine, '\\');
				*lpszBuf = '\"';
                }
            else
                {
                lpszBuf = Events[wEvent].szMessage;
                }

            SetDlgItemText (hDlg, IDC_NPS_EXPIRED_TEXT, lpszBuf);

                                        // Load up the system default
                                        // exclamation icon.
            hicon = LoadIcon ( NULL, IDI_EXCLAMATION );

            SendDlgItemMessage ( hDlg, IDC_WARNING_ICON, STM_SETICON,
                                 (WPARAM) hicon, 0 );

                                        // Default to running the event now.
            CheckDlgButton ( hDlg, IDC_RUN_EVENT_NOW, BST_CHECKED );

                                        // Disable the [X] caption bar
                                        // button since there really isn't
                                        // a "cancel" operation here.  We
                                        // want the user to pick one of the
                                        // choices and click OK.
            hSysMenu = GetSystemMenu ( hDlg, FALSE );

            EnableMenuItem ( hSysMenu, SC_CLOSE, MF_DISABLED );


            SetFocus (GetDlgItem (hDlg, IDC_RUN_EVENT_NOW));
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

#ifdef SYM_WIN16
                                        // WM_ACTIVATE:  Another help
                                        // kluge for F1.
        case WM_ACTIVATE:
            if (GET_WM_ACTIVATE_STATE(wParam, lParam))
                {
                StdHelpSetContext (NPS_HELP_DLG_EXPIRED,
                                   NPS_HELP_DLG_EXPIRED);
                }
            break;
#else
        case WM_HELP:
        case WM_CONTEXTMENU:
                                        // ----------------------------------
                                        // Bring up right click help menu.
                     					// or context sensitive help.
                                        // ----------------------------------
            OnExpiredContextHelp ( hDlg, wMessage, wParam, lParam );
            break;
#endif
                                        /* We are being closed so       */
                                        /* return to our caller.        */
        case WM_CLOSE:
            EndDialog (hDlg, NPS_EXPIRED_LEAVE_AS_IS );
        break;

                                        /* Process a control message.   */
                                        /* If it is the OK button then  */
                                        /* get out of here!             */
        case WM_COMMAND:
            switch (GET_WM_COMMAND_ID(wParam, lParam))
               {
                case IDOK:
                    if ( IsDlgButtonChecked ( hDlg, IDC_RUN_EVENT_NOW ) )
                        EndDialog ( hDlg, NPS_EXPIRED_RUN_NOW );
                    else if ( IsDlgButtonChecked ( hDlg, IDC_DELETE_EVENT ) )
                        EndDialog ( hDlg, NPS_EXPIRED_DELETE );
                    else
                        EndDialog ( hDlg, NPS_EXPIRED_LEAVE_AS_IS );
                break;

                case IDC_NPS_HELP:
                   StdHelpFromDialog (
                                hDlg,
                                IDH_NAVW_SCHEDULER_WHATHIS_EXPIRED_HLP_BTN
                                );
                break;

                default:
                    return (FALSE);
               } /* switch */
           break;

                                        /* Let Windows handle the       */
                                        /* message since we have no     */
                                        /* clue what to do with it.     */
        default:
            return (FALSE);
       } /* switch */

   return (TRUE);
} /* NpsEventExpiredDlgProc() */


#ifdef SYM_WIN32

/* **************************************************************************
@Name: VOID LOCAL OnExpiredContextHelp (
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

VOID LOCAL OnExpiredContextHelp (       // Do help stuff
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
        IDC_NPS_EXPIRED_INFO_TEXT,
        IDH_NAVW_SCHEDULER_WHATHIS_EXPIRED_INFO_TEXT,

        IDC_NPS_EXPIRED_INFO_TEXT2,
        IDH_NAVW_SCHEDULER_WHATHIS_EXPIRED_INFO_TEXT2,

        IDC_NPS_EXPIRED_EVENT_TEXT,
        IDH_NAVW_SCHEDULER_WHATHIS_EXPIRED_DESCRIPTION,

        IDC_NPS_EXPIRED_DESCR_TEXT,
        IDH_NAVW_SCHEDULER_WHATHIS_EXPIRED_DESCRIPTION,

        IDC_NPS_EXPIRED_SCHEDTIME_TEXT,
        IDH_NAVW_SCHEDULER_WHATHIS_SCHEDTIME_MESSAGE,

        IDC_NPS_EXPIRED_SCHEDTIME,
        IDH_NAVW_SCHEDULER_WHATHIS_SCHEDTIME_MESSAGE,

        IDC_NPS_EXPIRED_TEXT_TXT,
        IDH_NAVW_SCHEDULER_WHATHIS_EXPIRED_MESSAGE,

        IDC_NPS_EXPIRED_TEXT,
        IDH_NAVW_SCHEDULER_WHATHIS_EXPIRED_MESSAGE,

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


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

WORD LOCAL NpsExecute (                 // Comvert to short params and exec.
    HWND    hParent,                    // [in] parent window handle
    LPSTR   lpszCmd,                    // [in] command to execute
    LPSTR   lpszParams,                 // [in] parameters
    LPSTR   lpszStartupDir,             // [in] startup dir
    int     nRunStyle                   // [in] run style
    )
{
    auto    WORD    wReturn = 0;
    auto    char    szShortCmd      [ SYM_MAX_PATH + 1 ];
    auto    char    szShortStartDir [ SYM_MAX_PATH + 1 ];
    auto    STATUS  Status;

                                        // If the command line is quoted,
                                        // we need to remove the quotes or
                                        // name return short name will choke.
    if ( *lpszCmd == '\"' )
        {
        LPSTR lpszFrom = CharNext ( lpszCmd );
        LPSTR lpszTo   = szShortCmd;
        while ( (*lpszFrom != '\"') && (*lpszFrom != EOS) )
            {
            *lpszTo  = *lpszFrom;
            if (DBCSIsLeadByte(*lpszFrom))
                {*(lpszTo+1)  = *(lpszFrom+1);}

            lpszTo   = CharNext ( lpszTo );
            lpszFrom = CharNext ( lpszFrom );
            }
        *lpszTo = EOS;
        }
    else
        STRCPY ( szShortCmd, lpszCmd );

                                        // Get a valid short name for the
                                        // command if we can.
    Status = NameReturnShortName ( szShortCmd, szShortCmd );
    if ( Status != NOERR )
        STRCPY ( szShortCmd, lpszCmd );

                                        // If there is a startup directory
                                        // we need to work with it as well.
    if ( *lpszStartupDir )
        {
                                        // If the startup dir is quoted,
                                        // we need to remove the quotes or
                                        // name return short name will choke.
        if ( *lpszStartupDir == '\"' )
            {
            LPSTR lpszFrom = CharNext ( lpszStartupDir );
            LPSTR lpszTo   = szShortStartDir;
            while ( (*lpszFrom != '\"') && (*lpszFrom != EOS) )
                {
                *lpszTo  = *lpszFrom;
                if (DBCSIsLeadByte(*lpszFrom))
                    {*(lpszTo+1)  = *(lpszFrom+1);}

                lpszTo   = CharNext ( lpszTo );
                lpszFrom = CharNext ( lpszFrom );
                }
            *lpszTo = EOS;
            }
        else
            STRCPY ( szShortStartDir, lpszStartupDir );

                                        // See if the startup dir has a
                                        // short equivalent.
        Status = NameReturnShortName ( szShortStartDir, szShortStartDir );
        if ( Status != NOERR )
            {
            STRCPY ( szShortStartDir, lpszStartupDir );
            }
        }
    else
        {
        NameReturnPath ( szShortCmd, szShortStartDir );
        }


    wReturn = (WORD) ShellExecute (
                            hParent,
                            "open",
                            szShortCmd,
                            lpszParams,
                            szShortStartDir,
                            nRunStyle
                            );

    return ( wReturn );
}

