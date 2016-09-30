/* Copyright 1991-1993 Symantec, Peter Norton Computing Group           */
/************************************************************************/
/*                                                                      */
/* $Header:   S:/NPSUI/VCS/Npsevnts.cpv   1.1   09 Apr 1998 14:10:54   JALLEE  $ */
/*                                                                      */
/* Description:                                                         */
/*      This file contains those routines used for performing event     */
/*      management for the Norton Program Scheduler.                    */
/*                                                                      */
/* Contains:                                                            */
/*                                                                      */
/************************************************************************/
/* $Log:   S:/NPSUI/VCS/Npsevnts.cpv  $ */
// 
//    Rev 1.1   09 Apr 1998 14:10:54   JALLEE
// Check NULL ptr return from localtime(). Occurs when date/time > 1/18/2038.
// 
//    Rev 1.0   19 May 1997 00:24:58   KSACKIN
// Initial revision.
// 
//    Rev 1.24   22 Feb 1996 13:24:54   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
// 
//    Rev 1.23   11 Jan 1996 09:49:06   JWORDEN
// Port changes from Quake 8
// 
//    Rev 1.22.1.1   30 Nov 1995 21:03:24   BARRY
// Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
// 
//    Rev 1.22   07 Jul 1995 09:52:12   KEITH
// Put in the following comment for a heads up:
// 
//     Note that the data in SCHEDULE.DAT is ANSI. Other files, 
//     such as NAVOPTS.DAT, store only OEM data in the file.
// 
// 
//    Rev 1.21   03 May 1995 13:19:42   MFALLEN
// Moved stdhelp.h higher up in the include order.
// 
//    Rev 1.20   24 Apr 1995 09:34:34   MFALLEN
// Was not checking for valid number of events before decrementing counter.
// 
//    Rev 1.19   06 Mar 1995 17:25:40   MFALLEN
// Added NPSREVOLVEREVENT structure
// 
//    Rev 1.18   06 Mar 1995 10:11:42   MFALLEN
// Made it work for the Win95 platform and added required enhancements to
// the product to give it a native Win95 look and feel.
// 
//    Rev 1.17   03 May 1994 23:26:44   BRAD
// Don't use NFileOpen() anymore
// 
//    Rev 1.16   26 Mar 1994 21:00:44   BRAD
// Cleaned up Windows messages
// 
//    Rev 1.15   15 Mar 1994 11:58:18   BRUCE
// Changed SYM_EXPORT to SYM_EXPORT
// 
//    Rev 1.14   09 Mar 1994 13:02:38   BRAD
// Cleaned up for SYM_WIN32
// 
//    Rev 1.13   03 Mar 1994 12:10:04   BRAD
// AWK script applied
// 
//    Rev 1.12   15 Oct 1993 00:23:48   BASIL
// [FIX] check expired events when size restored [11148]
// 
//    Rev 1.11   01 Oct 1993 13:00:32   MARKL
// Now fully supports named events.
// 
//    Rev 1.10   29 Sep 1993 14:53:58   MARKL
// Now supports messages, events, and named event types.
// 
//    Rev 1.9   28 Aug 1993 14:35:30   MARKL
// Fixed problem with converting old versions.
// 
//    Rev 1.8   23 Jul 1993 10:28:14   MARKL
// Help menu now looks like nirvana's.  Removed help/exit buttons.  All
// child dialogs are centered.
// 
//    Rev 1.7   22 Jul 1993 12:02:04   MARKL
// Hooked up quake help system.
// 
//    Rev 1.6   11 Jun 1993 19:49:56   BRAD
// Changed order of params for NameReturnPath().
// 
//    Rev 1.5   10 Jun 1993 13:25:12   KYECK
// Added support for Event.bSound (it defaults to TRUE).
//
//    Rev 1.4   10 Jun 1993 09:31:34   KYECK
// Added support for 'yearly' events.
//
//    Rev 1.3   22 Apr 1993 11:28:56   MARKL
// Fixed bugs where parameters to NMsgBox() (a variable argument function)
// were not being typecasted correctly.
//
//    Rev 1.2   21 Apr 1993 22:05:06   MARKL
// Nirvana gives the schedule a chance to take an a radically new look/feel
// in the Add/Edit dialog!!!
//
//    Rev 1.1   18 Mar 1993 20:32:20   MARKL
// Cleaned up unnecessary typecasting of LPSTR.
//
//    Rev 1.0   15 Mar 1993 17:50:34   MARKL
// Initial revision.
//
//    Rev 1.4   26 Jun 1992 17:53:20   MEIJEN
// Get rid of strids.h because NWRES will eventually disappear
//
//    Rev 1.3   19 Jun 1992 10:34:10   MEIJEN
// Decentralize resources from NWRES to NPS(scheduler) for REVOLVER
//
//    Rev 1.2   19 Jan 1992 12:28:14   MARKL
// Now handles common properties.  Still need to add support for the
// new NWinExec function and the Standard Directory Browse.
//
//    Rev 1.1   02 Oct 1991 12:06:54   MARKL
// Made WinStric.H pass.
//
//    Rev 1.0   28 Jul 1991 11:37:44   MARKL
// Initial revision.
//
//    Rev 1.4   20 Jul 1991  0:08:28   ENRIQUE
// Fixed CRC bug if there were 0 events being written to or read
// from the SCHEDULE.BIN file.
//
//    Rev 1.3   09 Jul 1991  9:04:44   MARKL
// CpyCopyFile() has changed its return codes.  We'll ignore them.
//
//    Rev 1.2   08 Jul 1991 13:29:48   MARKL
// SCHEDULE.BIN now has checksum
//
//    Rev 1.1   09 Jun 1991 13:20:58   MARKL
// LastTime was not being initialized if Schedule.Bin was not present.  We
// must always call NpsCheckOldEvents() if bInitial is TRUE.
//
//    Rev 1.0   01 Jun 1991 20:36:24   MARKL
// Initial revision.
/************************************************************************/
#include    "platform.h"
#include    "stdhelp.h"
#include    <sys\timeb.h>
#include    <time.h>
#include    "undoc.h"                  /* for extra typedefs           */
#include    "msgdisp.h"                    /* error message stuff          */
#include    "xapi.h"
#include    "file.h"
#include    "ngui.h"
#include    "symintl.h"
#include    "nps.h"                     /* scheduler includes           */
#include    "npsres.h"
#include    "npsstb.h"


#define TRACE   ERR_TRACE_ID (_NPSEVNTS_C)



/************************************************************************/
/* Local component prototypes                                           */
/************************************************************************/

WORD LOCAL PASCAL NpsMakeNpsBak (       /* Creates NpsBin.Bak file      */
    HWND        hDlg,                   /* Handle to dialog window      */
    LPSTR       lpszBinFile,            /* Name of Bin file             */
    LPSTR       lpszBinBakFile);        /* Name of Backup file          */

WORD LOCAL PASCAL NpsEventGet(          /* Gets events from NPS.BIN file*/
    HWND        hDlg,                   /* Handle to calling dialog box */
    HFILE       fd,                     /* Handle to file               */
    LPWORD      lpwEvent);              /* Buffer to current event cntr */




/************************************************************************/
/* NPSEVNTS routines                                                    */
/************************************************************************/




/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*                                                                      */
/*      Given the current event, formats a string that is to be added   */
/*      to the event list box.  We use the internationalization stuff   */
/*      wherever possible.                                              */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           -- Function completed okay.     */
/************************************************************************/
/* 01/05/91 CJR Function created.                                       */
/* 02/26/91 MSL Made clean up pass.  Calls Manshan's internation-       */
/*              alization stuff.                                        */
/* 03/05/91 MSL Resources now in NWRES.                                 */
/* 05/14/91 MSL Wasn't handling "nd", "th", and "nd" correctly on       */
/*              monthly events.                                         */
/* 04/22/93 MSL Handles disabled events.                                */
/* 06/08/93 KDY Added support for yearly events.                        */
/************************************************************************/
WORD PASCAL MkEventStr (                /* Makes an event list item str */
    WORD        wEvent,                 /* Event to make string for     */
    PSTR        pszBuf)                 /* Ptr to buffer to load        */
{
    char                        szFmtBuf[ESIZE];
    char                        szTimeBuf[SSIZE];
    char                        szDateBuf[ESIZE];
    WORD                        ith;
    WORD                        wEnd;
    WORD                        wPm=0;
    extern      NPSEVNT         Events[];
    extern      char            ampm[NAM][SSIZE];
    extern      BOOL            b24Hour;

    if (Events[wEvent].bEnabled == FALSE)
        {
        LoadString (ghInst, IDS_NPS_DISABLED, szFmtBuf, ESIZE);
        wsprintf (pszBuf, szFmtBuf, (LPSTR)Events[wEvent].szDescr);
        }
    else
        {
                                        /* Build a string with an       */
                                        /* international time in it.    */
        NSetIntlTime (szTimeBuf,
                      Events[wEvent].wHour,
                      Events[wEvent].wMin,
                      0,
                      FALSE,
                      !b24Hour);

        switch (Events[wEvent].wFreq)
            {
            case HOURLY:
                LoadString (ghInst, IDS_NPS_HOURLY, szFmtBuf, ESIZE);
                wsprintf(pszBuf,
                         szFmtBuf,
                         (LPSTR)Events[wEvent].szDescr,
                         (LPSTR)freqs[Events[wEvent].wFreq],
                         (LPSTR)Events[wEvent].wMin);
                break;

            case DAILY:
            case WEEKDAY:
                LoadString (ghInst, IDS_NPS_DAILY, szFmtBuf, ESIZE);
                wsprintf(pszBuf,
                         szFmtBuf,
                         (LPSTR)Events[wEvent].szDescr,
                         (LPSTR)freqs[Events[wEvent].wFreq],
                         (LPSTR)szTimeBuf);
                break;

            case WEEKLY:
                LoadString (ghInst, IDS_NPS_WEEKLY, szFmtBuf, ESIZE);
                wsprintf(pszBuf,
                         szFmtBuf,
                         (LPSTR)Events[wEvent].szDescr,
                         (LPSTR)freqs[Events[wEvent].wFreq],
                         (LPSTR)days[Events[wEvent].wDay],
                         (LPSTR)szTimeBuf);
                break;

            case MONTHLY:
                /* This chunk figures out 1st, 2nd, 3rd, 4th...20th, 21st, ... */
                /* crap.  YUK!!!!!!  Screw internationalization.               */
                wEnd = Events[wEvent].wDate % 10;
                ith = 3;
                if ((Events[wEvent].wDate < 10) ||
                    (Events[wEvent].wDate > 20))
                    {
                    if ((wEnd != 0) && (wEnd <= 3))
                        {
                        ith = wEnd - 1;
                        }
                    }
                LoadString (ghInst, IDS_NPS_MONTHLY, szFmtBuf, ESIZE);
                wsprintf(pszBuf,
                         szFmtBuf,
                         (LPSTR)Events[wEvent].szDescr,
                         (LPSTR)freqs[Events[wEvent].wFreq],
                         Events[wEvent].wDate,
                         (LPSTR)th[ith],
                         (LPSTR)szTimeBuf,
                         (LPSTR)ampm[wPm]);
                break;

            case YEARLY:
                /* This chunk figures out 1st, 2nd, 3rd, 4th...20th, 21st, ... */
                /* crap.  YUK!!!!!!  Screw internationalization.               */
                wEnd = Events[wEvent].wDate % 10;
                ith = 3;
                if ((Events[wEvent].wDate < 10) ||
                    (Events[wEvent].wDate > 20))
                    {
                    if ((wEnd != 0) && (wEnd <= 3))
                        {
                        ith = wEnd - 1;
                        }
                    }

                LoadString (ghInst, IDS_NPS_YEARLY, (LPSTR)szFmtBuf, ESIZE);
                wsprintf(pszBuf,
                         szFmtBuf,
                         (LPSTR)Events[wEvent].szDescr,
                         (LPSTR)freqs[Events[wEvent].wFreq],
                         (LPSTR)months[Events[wEvent].wMonth],
                         Events[wEvent].wDate,
                         (LPSTR)th[ith],
                         (LPSTR)szTimeBuf);
                break;

            case ONETIME:
                LoadString (ghInst, IDS_NPS_ONETIME, szFmtBuf, ESIZE);
                NSetIntlLongDate (szDateBuf,
                                  Events[wEvent].wMonth + 1,
                                  Events[wEvent].wDate,
                                  Events[wEvent].wYear,
                                  FALSE);
                wsprintf(pszBuf,
                         szFmtBuf,
                         (LPSTR)Events[wEvent].szDescr,
                         (LPSTR)freqs[Events[wEvent].wFreq],
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
        }

    return (NOERR);
}




/************************************************************************/
/* Description:                                                         */
/* Initialize an event to today and other defaults                      */
/*                                                                      */
/* VOID PASCAL MkEvent(n)  - index of the event to create               */
/*                                                                      */
/* Return values:                                                       */
/*      nothing                                                         */
/************************************************************************/
/* 1/5/91 CJR Function created.                                         */
/* 04/19/93 MSL Initialize such that event is enabled by default.       */
/* 06/10/93 KDY Initialize such that sound is default for all events.   */
/************************************************************************/
VOID PASCAL MkEvent (                   /* Makes an event               */
    int         iEvent)                 /* Event to make                */
{
    struct  tm      *curtm;
    struct  _timeb  curtimeb;
    extern  NPSEVNT Events[];           /* The actual events            */

                                        // get the time and date
    _ftime(&curtimeb);
    curtm = localtime (&curtimeb.time);
    if ( NULL != curtm )
    {
        Events[iEvent].wMin     = curtm->tm_min;
        Events[iEvent].wHour    = curtm->tm_hour;
        Events[iEvent].wDate    = curtm->tm_mday;
        Events[iEvent].wDay     = curtm->tm_wday;
        Events[iEvent].wMonth   = curtm->tm_mon;
        Events[iEvent].wYear    = curtm->tm_year+1900;
        Events[iEvent].wFreq    = ONETIME;
        Events[iEvent].wType    = EVENT_TYPE_PROGRAM;
        Events[iEvent].bEnabled = TRUE;
        Events[iEvent].bSound   = TRUE;
    }
}




/************************************************************************/
/* Description:                                                         */
/*      This routine reads the event table from disk and calls          */
/*      NpsEventsCheckOld to discard any one time events that have      */
/*      expired.                                                        */
/*                                                                      */
/*      Note that the data in this .DAT file is ANSI. Other files,      */
/*      such as NAVOPTS.DAT, store only OEM data in the file.           */
/*                                                                      */
/*                                                                      */
/* Future Enhancements:                                                 */
/*      In future versions, routine will have to be smart enough to     */
/*      translate old file formats to the currently supported format.   */
/*                                                                      */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR - Event table successfully read                           */
/*                                                                      */
/************************************************************************/
/* 01/05/91 CJR Function created.                                       */
/* 03/29/91 MSL Made cleanup pass, standarizing, bin file name no longer*/
/*              hardcoded, _fstrcat() replaced with lstrcat().          */
/*              Added error handling.  I have also introduced the use   */
/*              of a NPSBIN.BAK file to keep tech support happy if      */
/*              something blows.                                        */
/* 04/01/91 MSL Added the bInitLoad parameter.  I forgot that           */
/*              NpsEventsGet() is also called in response to the        */
/*              WM_WININICHANGE message.                                */
/* 04/22/91 MSL Now uses new 1.0 file format.                           */
/* 05/23/91 MSL Now calls NWinPathGet()                                 */
/* 06/09/91 MSL We must always call NpsEventsCheckOld().                */
/* 07/08/91 MSL File now has a checksum.                                */
/* 07/19/91 MSL Checksum only calculated if there are entries.          */
/* 01/17/92 MSL tagNPSEVNT has gained new entries.  We need to also be  */
/*              able to read old tagNPSOTHELLOEVNT entries and          */
/*              translate them to the new tagNPSEVNT entries.  Calls    */
/*              NpsEventGet() to do this.                               */
/* 03/15/93 MSL Finds data file in the directory the module is in.      */
/* 04/19/93 MSL tagNPSEVNT has gained new entries.  We need to also be  */
/*              able to read old tagNPSOTHELLOEVNT and tagNPSVIPER      */
/*              entries and translate them to the new tagNPSEVNT        */
/*              entries.  Calls NpsEventGet() to do this.               */
/************************************************************************/
WORD PASCAL NpsEventsGet(               /* Gets events from NPS.BIN file*/
    HWND        hDlg,                   /* Handle to calling dialog box */
    BOOL        bInitLoad)              /* TRUE if initial load         */
{
    WORD        wRet;
    WORD        wEvent;
    HFILE       fd;
    DWORD       dwChecksum = 0;
    char        szPath[SYM_MAX_PATH];
    char        szBakPath[SYM_MAX_PATH];
    OFSTRUCT    of;
    NPSFILEHEADER       npsFileHeader;
    extern      char    szBinFile[SYM_MAX_PATH];
    extern      char    szBinBakFile[SYM_MAX_PATH];
    extern      char    szHeaderEntry[];
    extern      WORD    wNumEvents;
    extern      NPSEVNT Events[];       /* The actual events            */

                                        /* Initialization               */
    wRet       = NOERR;
    wNumEvents = 0;
    fd         = 0;

    GetModuleFileName (ghInst, szPath, sizeof (szPath));
    NameReturnPath (szPath, szBakPath);

    STRCAT (szBakPath, "\\");
    STRCPY (szPath, szBakPath);
    STRCAT (szPath, szBinFile);
    STRCAT (szBakPath, szBinBakFile);

                                        /* Does the file exist?  If     */
                                        /* not, just set the events to  */
                                        /* zero and get out of here.    */
    if ((fd = OpenFile (szPath, &of, OF_EXIST)) == HFILE_ERROR)
        {
        fd = 0;
        wNumEvents = 0;
        goto NpsEventsGetExit;
        }

                                        /* Make a backup copy of the    */
                                        /* file in NpsBin.Bak to keep   */
                                        /* user happy if Nps.Bin gets   */
                                        /* trashed.                     */
    if (bInitLoad == TRUE)
        {
        if (wRet = NpsMakeNpsBak (hDlg, szPath, szBakPath))
            {
            goto NpsEventsGetExit;
            }
        }

                                        /* Okay, now we can read in our */
                                        /* events.                      */
    if ((fd = OpenFile (szPath, &of, OF_READ | OF_PROMPT)) == (HFILE)-1)
        {
        wRet = NPS_FILE_CANT_OPEN;
        NMsgBox(ghInst,hDlg, wRet,
                 TRACE, NULL, -1,
                 (LPSTR)szPath);
        goto NpsEventsGetExit;
        }

                                        /* Read the file header and     */
                                        /* check its validity.  If not  */
                                        /* valid, just set events to 0  */
                                        /* and get out of here.         */
    if ((FileRead (fd,
                   (LPNPSFILEHEADER)&npsFileHeader,
                   sizeof (NPSFILEHEADER)) != sizeof (NPSFILEHEADER)) ||
        STRCMP (npsFileHeader.szHeader, szHeaderFile) ||
        ((npsFileHeader.wVerMajor != NPS_FILE_VER_MAJOR) &&
         (npsFileHeader.wVerMinor != NPS_FILE_VER_MINOR)))
        {
        wRet = NPS_FILE_VER_INVALID;
        NMsgBox(ghInst,hDlg, wRet, TRACE, NULL, -1);
        wNumEvents = 0;
        goto NpsEventsGetExit;
        }

    wNumEvents = npsFileHeader.wNumEntries;

    if (wNumEvents != 0)
        {
        CRCFile (fd, &dwChecksum);
        if (dwChecksum != npsFileHeader.dwChecksum)
            {
            wRet = NPS_FILE_CHKSUM_INVALID;
            NMsgBox(ghInst,hDlg, wRet, TRACE, NULL, -1);
            wNumEvents = 0;
            goto NpsEventsGetExit;
            }
        }

    FileSeek (fd, sizeof (NPSFILEHEADER), SEEK_FROM_BEGINNING);

                                        /* Read in all of the events,   */
                                        /* Skipping over those we don't */
                                        /* recognize.  NOTE:  The       */
                                        /* routine NpsEventGet may      */
                                        /* alter the value of wNumEvents*/
                                        /* dynamically if it is skipping*/
                                        /* over an event or detects an  */
                                        /* error.                       */
    wEvent = 0;

    while (wEvent < wNumEvents)
        {
        NpsEventGet (hDlg, fd, &wEvent);
        }



                                        /* Return to caller.            */
NpsEventsGetExit:
    if (bInitLoad == TRUE)
        {
        if (wRet == NOERR)
            {
                                        /* Remove any expired events    */
                                        /* from the list and initialize */
                                        /* LastTime.                    */
            NpsEventsCheckOld (hDlg);
            }
        }
    if (fd != (HFILE)0 && fd != (HFILE)-1)
        {
        _lclose (fd);
        }
    return (wRet);
} /* NpsEventsGet() */




/************************************************************************/
/* Description:                                                         */
/*      This routine reads an event from disk.  If the version of the   */
/*      event is not the newest (i.e. an old Othello or Viper event) it */
/*      is converted to our new event entry format.                     */
/*                                                                      */
/*      Note that the data in this .DAT file is ANSI. Other files,      */
/*      such as NAVOPTS.DAT, store only OEM data in the file.           */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR - Event table successfully read                           */
/*                                                                      */
/************************************************************************/
/* 01/19/91 MSL Function created.                                       */
/* 04/19/93 MSL tagNPSEVNT has gained new entries.  We need to also be  */
/*              able to read old tagNPSOTHELLOEVNT and tagNPSVIPER      */
/*              entries and translate them to the new tagNPSEVNT        */
/*              entries.                                                */
/* 06/10/93 KDY Added code to set bSound for the 'old' events.          */
/************************************************************************/
WORD LOCAL PASCAL NpsEventGet(          /* Gets events from NPS.BIN file*/
    HWND        hDlg,                   /* Handle to calling dialog box */
    HFILE       fd,                     /* Handle to file               */
    LPWORD      lpwEvent)               /* Buffer to current event cntr */
{
    WORD                  wRet;
    NPSENTRYHEADER        npsEntryHeader;
    NPSOTHELLOEVNT        npsOthelloEvent;
    NPSVIPEREVNT          npsViperEvent;
    NPSNIRVANAEVNT        npsNirvanaEvent;
    extern      char      szHeaderEntry[];
    extern      char      szBinFile[SYM_MAX_PATH];
    extern      WORD      wNumEvents;
    extern      NPSEVNT   Events[];       /* The actual events            */
    extern      HINSTANCE ghInst;

                                        /* Initialization               */
    wRet       = NOERR;

                                        // Read in the event, translating
                                        // those that we can and skipping
                                        // over those that we can't.

                                        // First read in the header and
                                        // make sure we can handle the
                                        // entry
    if ((FileRead (fd,
                   (LPNPSENTRYHEADER)&npsEntryHeader,
                   sizeof (NPSENTRYHEADER)) != sizeof (NPSENTRYHEADER)) ||
        STRCMP (npsEntryHeader.szHeader, szHeaderEntry))
        {
        wRet = NPS_ENTRY_VER_INVALID;
        goto NpsEventGetExit;
        }
    if (npsEntryHeader.wEntryID != EVENT_ID_SCHEDULE)
        {
        wRet = NPS_ENTRY_VER_INVALID;
        goto NpsEventGetExit;
        }
    if (!(((npsEntryHeader.wVerMajor == NPS_ENTRY_VER_MAJOR) &&
           (npsEntryHeader.wVerMinor == NPS_ENTRY_VER_MINOR)) ||
          ((npsEntryHeader.wVerMajor == NPS_ENTRY_VER_OTHELLO_MAJOR) &&
           (npsEntryHeader.wVerMinor == NPS_ENTRY_VER_OTHELLO_MINOR)) ||
          ((npsEntryHeader.wVerMajor == NPS_ENTRY_VER_VIPER_MAJOR) &&
           (npsEntryHeader.wVerMinor == NPS_ENTRY_VER_VIPER_MINOR)) ||
          ((npsEntryHeader.wVerMajor == NPS_ENTRY_VER_REVOLVER_MAJOR) &&
           (npsEntryHeader.wVerMinor == NPS_ENTRY_VER_REVOLVER_MINOR)) ||
          ((npsEntryHeader.wVerMajor == NPS_ENTRY_VER_NIRVANA_MAJOR) &&
           (npsEntryHeader.wVerMinor == NPS_ENTRY_VER_NIRVANA_MINOR))))
        {
        wRet = NPS_ENTRY_VER_INVALID;
        goto NpsEventGetExit;
        }

                                        // Back off to the beginning of
                                        // the header now that we know
                                        // it is valid.
    FileSeek (fd,
              -1L * (LONG) sizeof(NPSENTRYHEADER),
              SEEK_FROM_CURRENT_LOC);

                                        // Based on the type of the event
                                        // read in the entry, and translate,
                                        // if necesassary to the newer
                                        // entry format.
    if ((npsEntryHeader.wVerMajor == NPS_ENTRY_VER_MAJOR) &&
        (npsEntryHeader.wVerMinor == NPS_ENTRY_VER_MINOR))
        {
                                        // Normal event--no translation.
        if (FileRead (fd,
                      &(Events[*lpwEvent]),
                      npsEntryHeader.wSizeEntry) != sizeof(NPSEVNT) )
            {
            wRet = NPS_FILE_IO_ERR;
            goto NpsEventGetExit;
            }
        }
    else if ((npsEntryHeader.wVerMajor == NPS_ENTRY_VER_OTHELLO_MAJOR) &&
             (npsEntryHeader.wVerMinor == NPS_ENTRY_VER_OTHELLO_MINOR))
        {
                                        // Othello event, translate to new
                                        // Revolver format.
        if (FileRead (fd,
                      &npsOthelloEvent,
                      npsEntryHeader.wSizeEntry) < sizeof(NPSOTHELLOEVNT))
            {
            wRet = NPS_FILE_IO_ERR;
            goto NpsEventGetExit;
            }
        Events[*lpwEvent].EntryHeader   = npsOthelloEvent.EntryHeader;
        Events[*lpwEvent].EntryHeader.wVerMajor = NPS_ENTRY_VER_MAJOR;
        Events[*lpwEvent].EntryHeader.wVerMinor = NPS_ENTRY_VER_MINOR;
        Events[*lpwEvent].EntryHeader.wSizeEntry = sizeof (NPSEVNT);
        Events[*lpwEvent].wType         = npsOthelloEvent.wType;
        Events[*lpwEvent].wFreq         = npsOthelloEvent.wFreq;
        Events[*lpwEvent].wMin          = npsOthelloEvent.wMin;
        Events[*lpwEvent].wHour         = npsOthelloEvent.wHour;
        Events[*lpwEvent].wDay          = npsOthelloEvent.wDay;
        Events[*lpwEvent].wDate         = npsOthelloEvent.wDate;
        Events[*lpwEvent].wMonth        = npsOthelloEvent.wMonth;
        Events[*lpwEvent].wYear         = npsOthelloEvent.wYear;
        STRCPY (Events[*lpwEvent].szDescr, npsOthelloEvent.szDescr);

        if ( npsOthelloEvent.wType == EVENT_TYPE_PROGRAM )
            {
            STRCPY (Events[*lpwEvent].szMessage, "");
            STRCPY (Events[*lpwEvent].szCmdLine, npsOthelloEvent.szMessage);
            STRCPY (Events[*lpwEvent].szNamed, szEventProgram);
            Events[*lpwEvent].wRunStyle     =
                (npsOthelloEvent.bRunMinimized ? NPS_RUN_MINIMIZED :
                                                 NPS_RUN_NORMAL);
            }
        else
            {
            STRCPY (Events[*lpwEvent].szMessage, npsOthelloEvent.szMessage);
            STRCPY (Events[*lpwEvent].szCmdLine, "");
            STRCPY (Events[*lpwEvent].szNamed, szEventMessage);
            Events[*lpwEvent].wRunStyle = NPS_RUN_NORMAL;
            }

        STRCPY (Events[*lpwEvent].szStartupDir, "");
        Events[*lpwEvent].bEnabled      = TRUE;
        Events[*lpwEvent].bSound        = TRUE;
        }
    else if ((npsEntryHeader.wVerMajor == NPS_ENTRY_VER_VIPER_MAJOR) &&
             (npsEntryHeader.wVerMinor == NPS_ENTRY_VER_VIPER_MINOR))
        {
                                        // Viper event, translate to new
                                        // Revolver format.
        if (FileRead (fd,
                      &npsViperEvent,
                      npsEntryHeader.wSizeEntry) < sizeof(NPSVIPEREVNT))
            {
            wRet = NPS_FILE_IO_ERR;
            goto NpsEventGetExit;
            }
        Events[*lpwEvent].EntryHeader   = npsViperEvent.EntryHeader;
        Events[*lpwEvent].EntryHeader.wVerMajor = NPS_ENTRY_VER_MAJOR;
        Events[*lpwEvent].EntryHeader.wVerMinor = NPS_ENTRY_VER_MINOR;
        Events[*lpwEvent].EntryHeader.wSizeEntry = sizeof (NPSEVNT);
        Events[*lpwEvent].wType         = npsViperEvent.wType;
        Events[*lpwEvent].wFreq         = npsViperEvent.wFreq;
        Events[*lpwEvent].wMin          = npsViperEvent.wMin;
        Events[*lpwEvent].wHour         = npsViperEvent.wHour;
        Events[*lpwEvent].wDay          = npsViperEvent.wDay;
        Events[*lpwEvent].wDate         = npsViperEvent.wDate;
        Events[*lpwEvent].wMonth        = npsViperEvent.wMonth;
        Events[*lpwEvent].wYear         = npsViperEvent.wYear;
        STRCPY (Events[*lpwEvent].szDescr, npsViperEvent.szDescr);

        if ( npsViperEvent.wType == EVENT_TYPE_PROGRAM )
            {
            STRCPY (Events[*lpwEvent].szMessage, "");
            STRCPY (Events[*lpwEvent].szCmdLine, npsViperEvent.szMessage);
            STRCPY (Events[*lpwEvent].szStartupDir, npsViperEvent.szStartupDir);
            STRCPY (Events[*lpwEvent].szNamed, szEventProgram);
            Events[*lpwEvent].wRunStyle = npsViperEvent.wRunStyle;
            }
        else
            {
            STRCPY (Events[*lpwEvent].szMessage, npsViperEvent.szMessage);
            STRCPY (Events[*lpwEvent].szCmdLine, "");
            STRCPY (Events[*lpwEvent].szStartupDir, "");
            STRCPY (Events[*lpwEvent].szNamed, szEventMessage);
            Events[*lpwEvent].wRunStyle = NPS_RUN_NORMAL;
            }
        Events[*lpwEvent].bEnabled      = TRUE;
        Events[*lpwEvent].bSound        = TRUE;
        }
    else if ((npsEntryHeader.wVerMajor == NPS_ENTRY_VER_NIRVANA_MAJOR) &&
             (npsEntryHeader.wVerMinor == NPS_ENTRY_VER_NIRVANA_MINOR))
        {
                                        // Nirvana event, translate to new
                                        // Revolver format.
        if (FileRead (fd,
                      &npsNirvanaEvent,
                      npsEntryHeader.wSizeEntry) < sizeof(NPSNIRVANAEVNT))
            {
            wRet = NPS_FILE_IO_ERR;
            goto NpsEventGetExit;
            }
        Events[*lpwEvent].EntryHeader   = npsNirvanaEvent.EntryHeader;
        Events[*lpwEvent].EntryHeader.wVerMajor = NPS_ENTRY_VER_MAJOR;
        Events[*lpwEvent].EntryHeader.wVerMinor = NPS_ENTRY_VER_MINOR;
        Events[*lpwEvent].EntryHeader.wSizeEntry = sizeof (NPSEVNT);
        Events[*lpwEvent].wType         = npsNirvanaEvent.wType;
        Events[*lpwEvent].wFreq         = npsNirvanaEvent.wFreq;
        Events[*lpwEvent].wMin          = npsNirvanaEvent.wMin;
        Events[*lpwEvent].wHour         = npsNirvanaEvent.wHour;
        Events[*lpwEvent].wDay          = npsNirvanaEvent.wDay;
        Events[*lpwEvent].wDate         = npsNirvanaEvent.wDate;
        Events[*lpwEvent].wMonth        = npsNirvanaEvent.wMonth;
        Events[*lpwEvent].wYear         = npsNirvanaEvent.wYear;
        Events[*lpwEvent].bEnabled      = npsNirvanaEvent.bEnabled;
        Events[*lpwEvent].bSound        = npsNirvanaEvent.bSound;
        Events[*lpwEvent].wRunStyle     = npsNirvanaEvent.wRunStyle;
        STRCPY (Events[*lpwEvent].szDescr,      npsNirvanaEvent.szDescr);
        STRCPY (Events[*lpwEvent].szMessage,    npsNirvanaEvent.szMessage);
        STRCPY (Events[*lpwEvent].szCmdLine,    npsNirvanaEvent.szCmdLine);
        STRCPY (Events[*lpwEvent].szStartupDir, npsNirvanaEvent.szStartupDir);

        if (npsNirvanaEvent.wType == EVENT_TYPE_PROGRAM )
            {
            STRCPY (Events[*lpwEvent].szNamed, szEventProgram);
            }
        else
            {
            STRCPY (Events[*lpwEvent].szNamed, szEventMessage);
            }
        }
    else if ((npsEntryHeader.wVerMajor == NPS_ENTRY_VER_REVOLVER_MAJOR) &&
             (npsEntryHeader.wVerMinor == NPS_ENTRY_VER_REVOLVER_MINOR))
        {
        auto    NPSREVOLVEREVNT     npsRevolverEvent = { 0 };

                                        // Revolver event, translate to new
                                        // Revolver format.
        if (FileRead (fd,
                      &npsRevolverEvent,
                      npsEntryHeader.wSizeEntry) < sizeof(NPSREVOLVEREVNT))
            {
            wRet = NPS_FILE_IO_ERR;
            goto NpsEventGetExit;
            }

        Events[*lpwEvent].EntryHeader   = npsRevolverEvent.EntryHeader;
        Events[*lpwEvent].EntryHeader.wVerMajor = NPS_ENTRY_VER_MAJOR;
        Events[*lpwEvent].EntryHeader.wVerMinor = NPS_ENTRY_VER_MINOR;
        Events[*lpwEvent].EntryHeader.wSizeEntry = sizeof (NPSEVNT);
        Events[*lpwEvent].wType         = npsRevolverEvent.wType;
        Events[*lpwEvent].wFreq         = npsRevolverEvent.wFreq;
        Events[*lpwEvent].wMin          = npsRevolverEvent.wMin;
        Events[*lpwEvent].wHour         = npsRevolverEvent.wHour;
        Events[*lpwEvent].wDay          = npsRevolverEvent.wDay;
        Events[*lpwEvent].wDate         = npsRevolverEvent.wDate;
        Events[*lpwEvent].wMonth        = npsRevolverEvent.wMonth;
        Events[*lpwEvent].wYear         = npsRevolverEvent.wYear;
        Events[*lpwEvent].bEnabled      = npsRevolverEvent.bEnabled;
        Events[*lpwEvent].bSound        = npsRevolverEvent.bSound;
        Events[*lpwEvent].wRunStyle     = npsRevolverEvent.wRunStyle;
        STRCPY (Events[*lpwEvent].szDescr,      npsRevolverEvent.szDescr);
        STRCPY (Events[*lpwEvent].szMessage,    npsRevolverEvent.szMessage);
        STRCPY (Events[*lpwEvent].szCmdLine,    npsRevolverEvent.szCmdLine);
        STRCPY (Events[*lpwEvent].szStartupDir, npsRevolverEvent.szStartupDir);
        STRCPY (Events[*lpwEvent].szNamed, npsRevolverEvent.szNamed);
        STRCPY (Events[*lpwEvent].szPromptParams, npsRevolverEvent.szPromptParams );
        }

                                        /* Return to caller.            */
NpsEventGetExit:
    if (wRet == NOERR)
        {
                                        // Okay to go on to next event.
        (*lpwEvent)++;
        }
    else if (wRet == NPS_ENTRY_VER_INVALID)
        {
                                        // Pretend this event doesn't
                                        // exist, and skip to the next event
                                        // entry.
        if ( wNumEvents )
            wNumEvents--;

        wRet = NPS_ENTRY_VER_INVALID;
        NMsgBox(ghInst,hDlg, wRet, TRACE, NULL, -1);
        FileSeek (fd,
                  (LONG)(DWORD)(npsEntryHeader.wSizeEntry -
                                sizeof (NPSENTRYHEADER)),
                  SEEK_FROM_CURRENT_LOC);
        }
    else
        {
                                        // Things are hosed from here on
                                        // out.  Pretend there are no more
                                        // entries.
        wNumEvents = *lpwEvent;
        NMsgBox(ghInst,hDlg, wRet,
                 TRACE, NULL, -1,
                 (LPSTR)szBinFile);
        }
    return (wRet);
} /* NpsEventGet() */




/************************************************************************/
/* Description:                                                         */
/*      Write event to disk                                             */
/*                                                                      */
/*      Note that the data in this .DAT file is ANSI. Other files,      */
/*      such as NAVOPTS.DAT, store only OEM data in the file.           */
/*                                                                      */
/* Return values:                                                       */
/************************************************************************/
/* 01/05/91 CJR Function created.                                       */
/* 03/29/91 MSL Made cleanup pass, standarizing, bin file name no longer*/
/*              hardcoded, _fstrcat() replaced with STRCAT().           */
/*              Added error handling.                                   */
/* 04/22/91 MSL Uses 1.0 file format.                                   */
/* 05/23/91 MSL Now calls NWinPathGet()                                 */
/* 07/08/91 MSL File now has checksum                                   */
/* 07/19/91 MSL Checksum only calculated if there are entries.          */
/************************************************************************/
WORD PASCAL NpsEventsPut(               /* Write events to NPS.BIN      */
    HWND        hDlg)                   /* Handle to calling dlg box    */
{
    WORD                wEvent;
    HFILE               fd;
    char                szPath[SYM_MAX_PATH];
    WORD                wRet;
    OFSTRUCT            of;
    NPSFILEHEADER       npsFileHeader;
    extern      NPSEVNT Events[];       /* The actual events            */
    extern      char    szBinFile[SYM_MAX_PATH];
    extern      char    szHeaderFile[];
    extern      char    szHeaderEntry[];

                                        /* Perform initialization       */
    wRet = NOERR;
    fd   = 0;

                                        /* Were do we find our NPS.BIN  */
                                        /* file?                        */
    GetModuleFileName (ghInst, szPath, sizeof (szPath));
    NameReturnPath (szPath, szPath);
    STRCAT(szPath, "\\");
    STRCAT(szPath, szBinFile);

    if ((fd = OpenFile(szPath, &of, OF_WRITE | OF_CREATE)) == (HFILE)-1)
        {
        wRet = NPS_FILE_CANT_OPEN;
        NMsgBox(ghInst,hDlg, wRet,
                 TRACE, NULL, -1,
                 (LPSTR)szBinFile);
        goto NpsEventsPutExit;
        }

                                        /* Write each event entry.      */
    FileSeek (fd, sizeof (NPSFILEHEADER), SEEK_FROM_BEGINNING);
    for (wEvent = 0; wEvent < wNumEvents; wEvent++)
        {
        if (FileWrite (fd, &(Events[wEvent]), sizeof(NPSEVNT)) !=
            sizeof(NPSEVNT))
            {
            wRet = NPS_FILE_IO_ERR;
            NMsgBox(ghInst,hDlg, wRet,
                     TRACE, NULL, -1,
                     (LPSTR)szBinFile);
            goto NpsEventsPutExit;
            }
        }

                                        /* Now let's calculate a check- */
                                        /* sum for this file!           */
    npsFileHeader.dwChecksum = 0;

    if (wNumEvents != 0)
        {
        FileSeek (fd, sizeof (NPSFILEHEADER), SEEK_FROM_BEGINNING);
        CRCFile (fd, &(npsFileHeader.dwChecksum));
        }

                                        /* Write the file header.       */
    FileSeek (fd, 0, SEEK_FROM_BEGINNING);
    STRCPY (npsFileHeader.szHeader, szHeaderFile);
    npsFileHeader.wVerMajor   = NPS_FILE_VER_MAJOR;
    npsFileHeader.wVerMinor   = NPS_FILE_VER_MINOR;
    npsFileHeader.wNumEntries = wNumEvents;
    npsFileHeader.lNextEntryOffset = sizeof (NPSFILEHEADER) +
                                     (wNumEvents * sizeof (NPSEVNT));
    if (FileWrite (fd,
                   (LPNPSFILEHEADER)&npsFileHeader,
                   sizeof (NPSFILEHEADER)) != sizeof (NPSFILEHEADER))
        {
        wRet = NPS_FILE_IO_ERR;
        NMsgBox(ghInst,hDlg, wRet,
                 TRACE, NULL, -1,
                 (LPSTR)szBinFile);
        goto NpsEventsPutExit;
        }



                                        /* Clean up and return to       */
                                        /* caller.                      */
NpsEventsPutExit:
    if (fd != (HFILE)0 && fd != (HFILE)-1)
        {
        FileClose(fd);
        }
    return (wRet);
}




/************************************************************************/
/* Description:                                                         */
/*      Remove an event from the event struct array.                    */
/*                                                                      */
/* Return values:                                                       */
/************************************************************************/
/* 01/05/91 CJR Function created.                                       */
/* 03/29/91 MSL Performed cleanup.                                      */
/* 05/06/91 MSL Now updates BIN file.                                   */
/************************************************************************/
WORD PASCAL NpsEventRemove(             /* Removes event                */
    HWND        hDlg,                   /* Handle of calling dialog     */
    WORD        wEvent)                 /* Event to remove              */
{
    WORD                wEventCur;
    extern      WORD    wNumEvents;
    extern      NPSEVNT Events[];       /* The actual events            */

                                        /* Remove the event             */

    if ( wNumEvents )
        wNumEvents--;

    for (wEventCur = wEvent; wEventCur < wNumEvents; wEventCur++)
        {
        Events[wEventCur] = Events[wEventCur+1];
        }

                                        /* Update our BIN file.         */
    NpsEventsPut (hDlg);

                                        /* Return to caller.            */
    return (NOERR);
} /* NpsEventRemove() */




/************************************************************************/
/* Description:                                                         */
/*      Remove an event from the event struct array.  We prompt the     */
/*      user whether or not it is okay to remove the expired event and  */
/*      update the *pwEvent if we do not remove the event so            */
/*      NpsEventsCheckOld() wont choke.                                 */
/*                                                                      */
/* Return values:                                                       */
/************************************************************************/
/* 01/05/91 CJR Function created.                                       */
/* 03/29/91 MSL Rewrote to remove only expired events that the user     */
/*              wants removed.                                          */
/************************************************************************/
WORD LOCAL PASCAL NpsEventRemoveExpired(/* Removes expired event       */
    HWND        hDlg,                   /* Handle of calling dialog     */
    PWORD       pwEvent)                /* Event to possibly remove     */
{
    WORD                wRet;
    BOOL                bRemove;
    extern      WORD    wNumEvents;
    extern      NPSEVNT Events[];       /* The actual events            */

                                        /* Initialization               */
    wRet    = NOERR;
    bRemove = FALSE;

                                        /* Remove the event only if     */
                                        /* the user wants us to.        */
    NpsEventExpiredDisplay (hDlg, *pwEvent, &bRemove);
    if (bRemove)
        {
        wRet = NpsEventRemove (hDlg, *pwEvent);
        }
    else
        {
        *pwEvent = *pwEvent + 1;
        }

                                        /* Return to caller.            */
    return (wRet);
} /* NpsEventRemoveExpired() */




/************************************************************************/
/* Description:                                                         */
/*      Check for old events that should be deleted.  This also         */
/*      initializes LastTime.                                           */
/*                                                                      */
/* VOID PASCAL NpsEventsCheckOld(VOID)                                  */
/*                                                                      */
/* Return values:                                                       */
/*              nothing                                                 */
/*                                                                      */
/************************************************************************/
/* 01/05/91 CJR Function created.                                       */
/* 03/29/91 MSL Made cleanup pass and query user to see if he really    */
/*              wants to have the event removed from the list.          */
/* 04/22/91 MSL Initializes LastTime so we know at the earliest time    */
/*              in which an event was last processed.                   */
/* 05/06/91 MSL Deletes event if on this minute.                        */
/* 10/14/93 BG  Made function global                                    */
/************************************************************************/
WORD  PASCAL NpsEventsCheckOld(         /* Removes old events if okay   */
    HWND        hDlg)                   /* Handle to dialog box         */
{
    WORD                wRet;
    WORD                wEvent;
    struct      tm      *curtm;
    struct      _timeb  curtimeb;
    extern struct _timeb LastTime;      /* Last time processed.         */
    extern      NPSEVNT Events[];       /* The actual events            */

                                        /* Get the current date and     */
                                        /* time.                        */
    wRet = NOERR;
    _ftime(&curtimeb);
    LastTime = curtimeb;
    curtm = localtime (&curtimeb.time);
    if ( NULL != curtm )
        {

                                            /* Check to see if we need to   */
                                            /* remove any expired events.   */
        for (wEvent = 0; wEvent < wNumEvents;)
            {
            if (Events[wEvent].wFreq != ONETIME)
                {
                wEvent++;
                continue;
                }

            if ((WORD)curtm->tm_year+1900 < Events[wEvent].wYear)
                {
                wEvent++;                   // future year
                continue;
                }
            if ((WORD)curtm->tm_year+1900 > Events[wEvent].wYear)
                {
                                            /* Previous year                */
                NpsEventRemoveExpired (hDlg, &wEvent);
                continue;
                }
            if ((WORD)curtm->tm_mon < Events[wEvent].wMonth)
                {
                wEvent++;                   // future month
                continue;
                }
            if ((WORD)curtm->tm_mon > Events[wEvent].wMonth)
                {
                                            /* Previous month               */
                NpsEventRemoveExpired (hDlg, &wEvent);
                continue;
                }
            if ((WORD)curtm->tm_mday < Events[wEvent].wDate)
                {
                wEvent++;
                continue;
                }
            if ((WORD)curtm->tm_mday > Events[wEvent].wDate)
                {
                NpsEventRemoveExpired (hDlg, &wEvent);
                continue;
                }
            if ((WORD)curtm->tm_hour < Events[wEvent].wHour)
                {
                wEvent++;
                continue;
                }
            if ((WORD)curtm->tm_hour > Events[wEvent].wHour)
                {
                NpsEventRemoveExpired (hDlg, &wEvent);
                continue;
                }
            if ((WORD)curtm->tm_min >= Events[wEvent].wMin)
                {
                NpsEventRemoveExpired (hDlg, &wEvent);
                continue;
                }
            wEvent++;
            }
        }
    return (wRet);
} /* NpsEventsCheckOld() */




/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           - Everything went fine.         */
/*                                                                      */
/************************************************************************/
/* 03/30/91 MSL Function created.                                       */
/* 03/15/93 MSL Greatly simplified with the new FileCopy() routine.     */
/************************************************************************/
WORD LOCAL PASCAL NpsMakeNpsBak (       /* Creates NpsBin.Bak file      */
    HWND        hDlg,                   /* Handle to dialog window      */
    LPSTR       lpszBinFile,            /* Name of Bin file             */
    LPSTR       lpszBinBakFile)         /* Name of Backup file          */
{
    WORD        wRet = NOERR;

                                        /* Initialization.              */
    wRet      = NOERR;

                                        /* Copy the file.               */
    FileCopy (lpszBinFile,
              lpszBinBakFile,
              NULL);

    return (wRet);
} /* NpsMakeNpsBak() */


