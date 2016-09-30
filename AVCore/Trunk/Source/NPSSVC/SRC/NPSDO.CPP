/////////////////////////////////////////////////////////////////////
// NPSDo.cpp
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/NPSSVC/VCS/NPSDO.CPv   1.14   28 Aug 1998 17:11:44   JBRENNA  $
//
// Description:
//      This code has been taken from the NPS project and made into
//      member functions of the CNPSService class for use in the
//      Norton Program Scheduler service.
//
/////////////////////////////////////////////////////////////////////
// $Log:   S:/NPSSVC/VCS/NPSDO.CPv  $
// 
//    Rev 1.14   28 Aug 1998 17:11:44   JBRENNA
// Port QAKN changes to trunk.
// 
//    Rev 1.13   11 Jun 1998 16:47:00   mdunn
// Fixed #116043 - missed events were being run erroneously by NpsEventsCheck()
//
//    Rev 1.12   27 Apr 1998 16:17:38   mdunn
// Now calling MessageBeep() before displaying missed event dialogs so that
// the user gets an audible alert as well.
//
//    Rev 1.11   24 Apr 1998 14:38:38   mdunn
// Fixed NpsReturnCmdParams() and NpsExecute() to properly handle DBCS
// strings.  Fixes #112209.
//
//    Rev 1.10   17 Apr 1998 14:55:08   mdunn
// Revamped missed event handling
//
//    Rev 1.9   13 Apr 1998 16:52:40   JALLEE
// Fixed the last fix, which fixed the crash, but no longer fired events.
//
//    Rev 1.8   09 Apr 1998 13:18:50   JALLEE
// Check localtime() function return for NULL ptr before use.
// Fixes crash when date > 1/18/2038. Now fail silently.
//
//    Rev 1.7   31 Mar 1998 11:55:48   mdunn
// Removed references to Ctl3d functions - who needs those things for
// Newshell Windows anyway?
//
//    Rev 1.6   30 Mar 1998 10:24:24   mdunn
// Added support for running missed events.
//
//    Rev 1.4   10 Jul 1997 11:44:22   KSACKIN
// Fixed the OS check, and added a MB_ICONINFORMATION flag to our Message
// Display feature.
//
//    Rev 1.3   25 Jun 1997 13:50:52   KSACKIN
// Added a simple, mundane, message box call to do the message display on NT.
// This is done with a flag of MB_SERVICE_NOTIFICATION so that we can show this
// through the login screen when no desktop is present.
//
//    Rev 1.2   25 Jun 1997 13:13:52   KSACKIN
// Added a message box call with a ServiceNotification flag to the Mesage
// Display event.  This was previously commented out.  We need re revisit this
// when we can cue up these on a separate thread.  Now, it will block process.
//
//    Rev 1.1   02 Jun 1997 02:37:44   KSACKIN
// Added a new function for NPSEventNotify().
//
//    Rev 1.0   19 May 1997 00:03:08   KSACKIN
// Initial revision.
//
//    Rev 1.1   09 Apr 1996 11:59:40   DSACKING
// Fixed a couple of the include lines.
//
//    Rev 1.0   09 Apr 1996 11:32:48   DSACKING
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "platform.h"
#include "stdhelp.h"
#include "xapi.h"
#include "ctl3d.h"
#include "shellapi.h"
#include "sys\timeb.h"
#include "time.h"
#include "undoc.h"                      // for extra typedefs
#include "msgdisp.h"                    // error message stuff
#include "msgdisp.h"
#include "symintl.h"
#include "ngui.h"
#include "npsres.h"
#include "npsstb.h"
#include "dbcs.h"

#include "service.h"                    // For CService
#include "npssvstr.h"                   // For string IDs
#include "npssvc.h"                   // Class declaration with prototypes
#include "nps.h"                        // scheduler includes
                                        // for functions without hDlg.
#include "nps_hlp.h"
#include "resource.h"

#undef TRACE
#define TRACE   ERR_TRACE_ID (_NPSDO_C)



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
WORD CNPSService::NpsEventsCheck (VOID) /* Do we need to do an event?   */
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


            // Check if NpsEventsCheckOld() has flagged the event as
            // missed.  If so, we don't want to touch it since
            // NpsEventsCheckOld() will handle it.

            if ( Events[wEvent].bMissedAlertPending )
                {
                goto NpsEventsCheckContinue;
                }


            switch (Events[wEvent].wFreq)
            {
            case NPS_FREQ_HOURLY:
                if (NpsCheckHourMin (0,
                    Events[wEvent].wMin,
                    0,
                    (WORD)Lasttm->tm_min,
                    0,
                    (WORD)curtm->tm_min,
                    FALSE))
                {
                    NpsEventDo (wEvent, &bRemoved);
                }
                break;

            case NPS_FREQ_DAILY:
                if (NpsCheckHourMin (Events[wEvent].wHour,
                    Events[wEvent].wMin,
                    (WORD)Lasttm->tm_hour,
                    (WORD)Lasttm->tm_min,
                    (WORD)curtm->tm_hour,
                    (WORD)curtm->tm_min,
                    TRUE))
                {
                    NpsEventDo (wEvent, &bRemoved);
                }
                break;

            case NPS_FREQ_WEEKDAY:
                if (((WORD)curtm->tm_wday == SUN) ||
                    ((WORD)curtm->tm_wday == SAT))
                {
                    goto NpsEventsCheckContinue;
                }
                if (NpsCheckHourMin (Events[wEvent].wHour,
                    Events[wEvent].wMin,
                    (WORD)Lasttm->tm_hour,
                    (WORD)Lasttm->tm_min,
                    (WORD)curtm->tm_hour,
                    (WORD)curtm->tm_min,
                    TRUE))
                {
                    NpsEventDo (wEvent, &bRemoved);
                }
                break;

            case NPS_FREQ_WEEKLY:
                if ((WORD)curtm->tm_wday != Events[wEvent].wDay)
                {
                    goto NpsEventsCheckContinue;
                }
                if (NpsCheckHourMin (Events[wEvent].wHour,
                    Events[wEvent].wMin,
                    (WORD)Lasttm->tm_hour,
                    (WORD)Lasttm->tm_min,
                    (WORD)curtm->tm_hour,
                    (WORD)curtm->tm_min,
                    TRUE))
                {
                    NpsEventDo (wEvent, &bRemoved);
                }
                break;

            case NPS_FREQ_MONTHLY:
                if ((WORD)curtm->tm_mday != Events[wEvent].wDate)
                {
                    goto NpsEventsCheckContinue;
                }
                if (NpsCheckHourMin (Events[wEvent].wHour,
                    Events[wEvent].wMin,
                    (WORD)Lasttm->tm_hour,
                    (WORD)Lasttm->tm_min,
                    (WORD)curtm->tm_hour,
                    (WORD)curtm->tm_min,
                    TRUE))
                {
                    NpsEventDo (wEvent, &bRemoved);
                }
                break;

            case NPS_FREQ_YEARLY:
                if ((WORD)curtm->tm_mon != Events[wEvent].wMonth ||
                    (WORD)curtm->tm_mday != Events[wEvent].wDate)
                {
                    goto NpsEventsCheckContinue;
                }
                if (NpsCheckHourMin (Events[wEvent].wHour,
                    Events[wEvent].wMin,
                    (WORD)Lasttm->tm_hour,
                    (WORD)Lasttm->tm_min,
                    (WORD)curtm->tm_hour,
                    (WORD)curtm->tm_min,
                    TRUE))
                {
                    NpsEventDo (wEvent, &bRemoved);
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
                if (NpsCheckHourMin (Events[wEvent].wHour,
                    Events[wEvent].wMin,
                    (WORD)Lasttm->tm_hour,
                    (WORD)Lasttm->tm_min,
                    (WORD)curtm->tm_hour,
                    (WORD)curtm->tm_min,
                    TRUE))
                {
                    NpsEventDo (wEvent, &bRemoved);
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
BOOL CNPSService::NpsCheckHourMin (
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



/////////////////////////////////////////////////////////////////////
// void NpsEventNotifyDisplay( WORD wEvent )
//
// Purpose:
//  This function will display a notification message for the
//  Norton Program Scheduler service.  This will be a simple message
//  box implementation for now, since our service may not be able to
//  put up a complicated UI.
//
// Parameters:
//    WORD wEvent - [IN] Event to display.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 06-02-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void NpsEventNotifyDisplay(
    WORD wEvent
    )
{
    auto    TCHAR       szNPSCustomMessageTitle[SYM_MAX_PATH];
    auto    DWORD       dwMessageBoxFlags;
    extern  HINSTANCE   ghInst;

    dwMessageBoxFlags = MB_OK | MB_ICONINFORMATION;

                                        // Use the correct
                                        // MB_SERVICE_NOTIFICATION flag for the
                                        // OS that we are running on.
    if ( LOBYTE(GetVersion()) >= 4 )
        {
        dwMessageBoxFlags |= 0x00200000L;
        }
    else
        {
        dwMessageBoxFlags |= 0x00040000L;
        }

                                        // Load our custom message box title.
    LoadString(
        ghInst,
        IDS_CUSTOM_MESSAGE_TITLE,
        szNPSCustomMessageTitle,
        SYM_MAX_PATH
        );

                                        // ********** MUST REVISIT *************
                                        // Need Modless solution for Windows NT!
                                        // ********** MUST REVISIT *************
    MessageBox(
        NULL,
        Events[wEvent].szMessage,
        szNPSCustomMessageTitle,
        dwMessageBoxFlags
        );
}


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
WORD CNPSService::NpsEventDo (          /* Executes event               */
    WORD        wEvent,                 /* Index to event to execute    */
    PBOOL       pbRemoved)              /* TRUE if removed from list    */

{
    WORD                wRet;
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
//        NpsTimerKill (hWnd);
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
                    ErrorHandler(
                       ID_ERR_EXECUTION_FAILURE,
                       szCmd);
// @?               NMsgBox (ghInst,
//                         hWnd,
//                         ERR_SEV_NUM (ERR_SEV_WARNING, wError),
//                         TRACE,
//                         NULL,
//                         -1,
//                         (LPSTR)Events[wEvent].szDescr,
//                         (LPSTR)Events[wEvent].szCmdLine);
                }
            }
//        NpsTimerStart (hWnd);
        }
    else                              // Don't allow type Message!
        {
        NpsEventNotifyDisplay (wEvent);
        }

                                        /* Delete a one-time event from */
                                        /* the list.                    */
    if (Events[wEvent].wFreq == NPS_FREQ_ONETIME)
        {
        *pbRemoved = TRUE;
        NpsEventRemove (wEvent);
        }
    else
        {
                                        // Calculate the event's next-run time
        CalculateNextRunTime ( &Events[wEvent] );

                                        // Save SCHEDULE.DAT to store the
                                        // last run time of the event.
        NpsEventsPut();
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
WORD CNPSService::NpsAnnounceEvent (    /* Anounces event with melody   */
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

VOID CNPSService::NpsReturnCmdParams (  // Return command and its parameters
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
WORD CNPSService::NpsEventExpiredDisplay ( /* Do we remove expired event?  */
    WORD        wEvent,                 /* Event to possibly remove     */
    int*        pnAction)               /* TRUE if we remove event      */
{
    WORD                        wRet;
    extern      HINSTANCE       ghInst;

                                        /* Initialization               */
    wRet        = NOERR;

                                        /* Create the dialog box and    */
                                        /* find out if we nuke the      */
                                        /* event.                       */

    MessageBeep ( MB_ICONEXCLAMATION );

    *pnAction = (BOOL)DialogBoxParam (ghInst,
                                      MAKEINTRESOURCE (IDD_NPS_EXPIRED),
                                      NULL,
                                      (DLGPROC)NpsEventExpiredDlgProc,
                                      MAKELPARAM (wEvent, 0));

                                        /* Return to our caller         */

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
    LPSTR                       lpszBuf;
    WORD                        wEvent;
    extern      NPSEVNT         Events[];

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
            {
            HICON hicon;
            HMENU hSysMenu;

            NG_CenterDialog (hDlg, GetParent (hDlg));
            wEvent = LOWORD (lParam);

            //Ctl3dSubclassDlg(hDlg, CTL3D_ALL);

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
            // +1 there because NPS stores the month as 0-11, but 
            // NSetIntlLongDate() calls thru to GetDateFormat() which
            // expects the month in the SYSTEMTIME format, which is 1-12.

            SPRINTF ( szSchedTimeBuf,
                      szFmtBuf,
                     (LPSTR)szDateBuf,
                     (LPSTR)szTimeBuf);

            SetDlgItemText (hDlg, IDC_NPS_EXPIRED_SCHEDTIME, szSchedTimeBuf);
            SetDlgItemText (hDlg, IDC_NPS_EXPIRED_DESCR_TEXT, Events[wEvent].szDescr);

            if (Events[wEvent].wType == EVENT_TYPE_PROGRAM )
                {
                lpszBuf = Events[wEvent].szCmdLine;
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
            }
        break;

                                        // WM_SYSCOLORCHANGE:  For 3D
                                        // effect.
        case WM_SYSCOLORCHANGE:
            //Ctl3dColorChange();
            break;

                                        // WM_CTLCOLOR:  For 3D effect.
        case WM_CTLCOLOR:
            //return ((BOOL)Ctl3dCtlColorEx (wMessage, wParam, lParam));

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
            //OnExpiredContextHelp ( hDlg, wMessage, wParam, lParam );
            break;
#endif
                                        /* We are being closed so       */
                                        /* return to our caller.        */
        case WM_CLOSE:
            EndDialog ( hDlg, NPS_EXPIRED_LEAVE_AS_IS );
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

// VOID LOCAL OnExpiredContextHelp (       // Do help stuff
//     HWND    hDlg,                       // [in] from this dialog
//     UINT    uMessage,                   // [in] help message to deal with
//     WPARAM  wParam,                     // [in] ?
//     LPARAM  lParam                      // [in] ??
//     )
// {
//                                         // -----------------------------------
//                                         // This array is a cross reference
//                                         // between a control id number and a
//                                         // help id number that should be
//                                         // displayed when the user selects the
//                                         // 'What's This' menu entry.
//                                         // -----------------------------------
//
//     static  DWORD       dwWhatsThisMap[] =
//         {
//         IDC_NPS_EXPIRED_INFO_TEXT,
//         IDH_NAVW_SCHEDULER_WHATHIS_EXPIRED_INFO_TEXT,
//
//         IDC_NPS_EXPIRED_INFO_TEXT2,
//         IDH_NAVW_SCHEDULER_WHATHIS_EXPIRED_INFO_TEXT2,
//
//         IDC_NPS_EXPIRED_EVENT_TEXT,
//         IDH_NAVW_SCHEDULER_WHATHIS_EXPIRED_DESCRIPTION,
//
//         IDC_NPS_EXPIRED_DESCR_TEXT,
//         IDH_NAVW_SCHEDULER_WHATHIS_EXPIRED_DESCRIPTION,
//
//         IDC_NPS_EXPIRED_SCHEDTIME_TEXT,
//         IDH_NAVW_SCHEDULER_WHATHIS_SCHEDTIME_MESSAGE,
//
//         IDC_NPS_EXPIRED_SCHEDTIME,
//         IDH_NAVW_SCHEDULER_WHATHIS_SCHEDTIME_MESSAGE,
//
//         IDC_NPS_EXPIRED_TEXT_TXT,
//         IDH_NAVW_SCHEDULER_WHATHIS_EXPIRED_MESSAGE,
//
//         IDC_NPS_EXPIRED_TEXT,
//         IDH_NAVW_SCHEDULER_WHATHIS_EXPIRED_MESSAGE,
//
//         0,
//         0
//         };
//
//                                         // -----------------------------------
//                                         // This array is a set of string id's
//                                         // in our resources that are added to
//                                         // the 'How To..." menu entry. When one
//                                         // of the entries is selected by the
//                                         // user the corresponding help is
//                                         // displayed.
//                                         // -----------------------------------
//     static  DWORD       dwHowToMap[] =
//         {
//         IDH_NAVW_HOWTO_SCHEDULE_SCANS,  IDH_NAVW_HOWTO_SCHEDULE_SCANS,
//         0,          0
//         };
//
//                                         // -----------------------------------
//                                         // Invoke cmn Nav context sensitive help
//                                         // -----------------------------------
//     StdHelpOnContextHelp (
//                         hDlg,
//                         uMessage,
//                         wParam,
//                         lParam,
//                         dwWhatsThisMap,
//                         dwHowToMap,
//                         NULL,           //  This dlg. doesn't have any  ...
//                         NULL            //  ... List Box controls - so NULL here
//                         );
// }
//
#endif


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

WORD CNPSService::NpsExecute (          // Comvert to short params and exec.
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
                            NULL,
                            "open",
                            szShortCmd,
                            lpszParams,
                            szShortStartDir,
                            nRunStyle
                            );

    return ( wReturn );
}
