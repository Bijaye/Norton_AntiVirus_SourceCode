/////////////////////////////////////////////////////////////////////
// NPSEvnts.cpp
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/NPSAPI/VCS/npsevnts.cpv   1.18   01 Jul 1998 12:07:30   mdunn  $
//
// Description:
//      This code has been taken from the NPS project and made into
//      member functions of the CNPSService class for use in the
//      Norton Program Scheduler service.
//
/////////////////////////////////////////////////////////////////////
// $Log:   S:/NPSAPI/VCS/npsevnts.cpv  $
// 
//    Rev 1.18   01 Jul 1998 12:07:30   mdunn
// Changed KEY_ALL_ACCESS to KEY_QUERY_VALUE so things will work for non-admin
// users.
// 
//    Rev 1.17   11 Jun 1998 16:32:56   mdunn
// Now setting an event's bMissedAlertPending flag to FALSE when a new
// event is created or a Roswell event is converted.
// 
//    Rev 1.16   05 Jun 1998 15:40:40   mdunn
// Fixed code that converts from a Roswell format SCHEDULE.DAT file.
// 
//    Rev 1.15   18 Apr 1998 16:31:38   mdunn
// Reworked the code that converts SCHEDULE.DAT files from earlier 32-bit
// NAVs, since I changed the format of the Atomic NPSEVNT struct.
// 
//    Rev 1.14   17 Apr 1998 13:48:12   mdunn
// Little tweak to NpsEventsCheckOld() -- removed references to CTime
// since that function isn't used by NPSUI or NPSSVC.  If anything else
// using NPSAPI called that function, it'll break on missed repeating
// events!!
// 
//    Rev 1.13   17 Apr 1998 11:54:46   mdunn
// Revamped missed events handling.  Added CalculateNextRunTime().
// 
//    Rev 1.12   09 Apr 1998 13:49:12   JALLEE
// Check for NULL returned from localtime(). Occurs if time > 1/18/2038.
// 
//    Rev 1.11   01 Apr 1998 12:25:58   mdunn
// Fixed MkEvent() to set the new event's last-run time to the current time,
// instead of all zeros.
// 
//    Rev 1.10   30 Mar 1998 10:01:12   mdunn
// Added code to read SCHEDULE.DAT files from Roswell or earlier.
// 
//    Rev 1.8   18 Jul 1997 15:43:14   KSACKIN
// Removed the MkEventString() function.  This will now live in the application
// that displays these events, and not in the API it uses to read them.
//
//    Rev 1.7   02 Jun 1997 00:12:20   KSACKIN
// Added a function call at the end of NPSEventsPut() to notify all clients of
// this data file to reload their events.
//
//    Rev 1.6   01 Jun 1997 20:39:32   JBRENNA
// Rewrite NpsGetMyPath to return the path based on either the
// InstalledApps\NAV95 or NAVNT path. This should be where the SCHEDULE.DAT
// file is located.
//
//    Rev 1.5   12 May 1997 22:28:00   JBRENNA
// Add more initialization code to the MkEvent function. This seems to NOW
// initialize everything I need initialized for adding a new event.
//
//    Rev 1.4   31 May 1997 18:46:42   KSACKIN
 // Now inserting entry header version information into newly created structs.
//
//    Rev 1.3   31 May 1997 18:04:38   JBRENNA
// Change NpsEventsPut() to be a PASCAL function to match the other functions
// here (like NpsEventsGet) ... and to also match its prototype.
//
//    Rev 1.2   29 May 1997 21:01:42   KSACKIN
// Fixed a bug where if your path had Long File Names, the path returned from
// the NPSGetMyCmdLine() call would have quotes around it.  This would then
// break the NameReturnPath function, breaking the options loading.  Now we
// remove the quotes around the path before we load the options.
//
//    Rev 1.1   29 May 1997 17:10:24   KSACKIN
// #defined MAIN in this module so that we would have the data for the NPS.H
// file defined here.  Because of this, we should not define this in other NPS
// modules that use this library.
//
//    Rev 1.0   29 May 1997 00:43:12   ksackin
// Initial revision.
//
//    Rev 1.0   19 May 1997 00:03:04   KSACKIN
// Initial revision.
//
//    Rev 1.1   09 Apr 1996 11:59:06   DSACKING
// Fixed a couple of the include lines.
//
//    Rev 1.0   09 Apr 1996 11:33:52   DSACKING
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#define MAIN

#include "platform.h"
#include <afx.h>
#include "stdhelp.h"
#include "sys\timeb.h"
#include "time.h"
#include "undoc.h"                      // for extra typedefs
#include "msgdisp.h"                    // error message stuff
#include "xapi.h"
#include "file.h"
#include "ngui.h"
#include "symintl.h"
#include "npsres.h"
#include "npsstb.h"
#include "npserr.h"

//#include "npsapi.h"                     // NPS function prototypes

#include "nps.h"                        // Scheduler includes

#undef TRACE
#define TRACE   ERR_TRACE_ID (_NPSAPI_CPP)


/************************************************************************/
/* Function Prototypes                                                  */
/************************************************************************/

                                        // Only prototype this function for
                                        // local use.
LPTSTR RemoveSurroundingCharsOfType(
    LPTSTR szStringToModify,
    LPCTSTR szSetOfCharsToRemove
    );

BOOL IsLeapYear ( WORD wYear )          // Swiped from Symkrnl
    { 
    return(wYear % 4 == 0  &&  (wYear % 400 == 0  ||  wYear % 100));
    }

/************************************************************************/
/* NPSEVNTS routines                                                    */
/************************************************************************/


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
    extern  char    *szNPSHeaderEntry;

                                        // get the time and date
    _ftime(&curtimeb);
    curtm = localtime (&curtimeb.time);
    if ( NULL != curtm )
    {

        STRCPY (Events[iEvent].EntryHeader.szHeader, szNPSHeaderEntry);

        Events[iEvent].EntryHeader.wVerMajor  = NPS_ENTRY_VER_MAJOR;
        Events[iEvent].EntryHeader.wVerMinor  = NPS_ENTRY_VER_MINOR;
        Events[iEvent].EntryHeader.wEntryID   = EVENT_ID_SCHEDULE;
        Events[iEvent].EntryHeader.wSizeEntry = sizeof (NPSEVNT);

        Events[iEvent].wMin     = curtm->tm_min;
        Events[iEvent].wHour    = curtm->tm_hour;
        Events[iEvent].wDate    = curtm->tm_mday;
        Events[iEvent].wDay     = curtm->tm_wday;
        Events[iEvent].wMonth   = curtm->tm_mon;
        Events[iEvent].wYear    = curtm->tm_year+1900;
        Events[iEvent].wFreq    = NPS_FREQ_ONETIME;
        Events[iEvent].wType    = EVENT_TYPE_PROGRAM;
        Events[iEvent].bEnabled = TRUE;
        Events[iEvent].bSound   = TRUE;
        Events[iEvent].bMissedAlertPending = FALSE;

        ZeroMemory ( &Events[iEvent].rNextRunTime, sizeof(SYSTEMTIME) );
    }
}




/************************************************************************/
/* Description:                                                         */
/* Get the path that we have been executed from.                        */
/*                                                                      */
/* VOID NpsGetMyPath(LPSTR lpszPath)  - Path is returned here           */
/*                                                                      */
/* Return values:                                                       */
/*      nothing                                                         */
/************************************************************************/
/* 4/4/96 DSACKIN Function created.                                     */
/* 6/1/97 JBRENNA Modified to get the path from registry values rather  */
/*                than using the path from the command line.            */
/************************************************************************/
VOID NpsGetMyPath (                     /* Makes an event               */
    LPSTR       lpszPath)               /* Event to make                */
{
    const TCHAR     szKey[]    = _T("Software\\Symantec\\InstalledApps");
    const LPCTSTR   szValues[] = { _T("NAV95"), _T("NAVNT") };
    const int       nNumValues = sizeof (szValues) / sizeof (szValues[0]);

    auto  BOOL      bFound = FALSE;
    auto  int       nCurValue;
    auto  TCHAR     szValueBuf[_MAX_PATH+1] = _T("");
    auto  DWORD     dwValueBufLen = sizeof (szValueBuf);
    auto  HKEY      hBaseKey;
    auto  DWORD     dwValueType;

    //  Get the value from the key.
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, szKey, 0, KEY_QUERY_VALUE, &hBaseKey))
    {
        for (nCurValue = 0; !bFound && nCurValue < nNumValues; ++nCurValue)
        {
            bFound = (ERROR_SUCCESS == RegQueryValueEx (hBaseKey, szValues[nCurValue], 0,
                                                        &dwValueType, (LPBYTE)szValueBuf,
                                                        &dwValueBufLen));
        }

        // Empty szValueBuf if we did not find the right information.
        if (!bFound || dwValueType != REG_SZ || !dwValueBufLen)
        {
            *szValueBuf = '\0';
        }
        RegCloseKey(hBaseKey);
    }

    STRCPY (lpszPath, szValueBuf);      // Copy this to our output parameter.
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
    extern      char    *szNPSBinFile;
    extern      char    *szNPSBinBakFile;
    extern      char    *szNPSHeaderFile;
    extern      char    szHeaderEntry[];
    extern      WORD    wNumEvents;
    extern      NPSEVNT Events[];       /* The actual events            */
                                        /* Initialization               */
    wRet       = NOERR;
    wNumEvents = 0;
    fd         = 0;

    NpsGetMyPath(szPath);

    STRCAT (szPath, "\\");
    STRCPY (szBakPath, szPath);
    STRCAT (szPath, szNPSBinFile);
    STRCAT (szBakPath, szNPSBinBakFile);

                                        /* Does the file exist?  If     */
                                        /* not, just set the events to  */
                                        /* zero and get out of here.    */
    if ((fd = OpenFile (szPath, &of, OF_EXIST)) == HFILE_ERROR)
        {
        fd = 0;
        wNumEvents = 0;
        wRet = ERR;
        goto NpsEventsGetExit;
        }

                                        /* Make a backup copy of the    */
                                        /* file in NpsBin.Bak to keep   */
                                        /* user happy if Nps.Bin gets   */
                                        /* trashed.                     */
    if (bInitLoad == TRUE)
        {
        if (wRet = NpsMakeNpsBak (szPath, szBakPath))
            {
            wRet = ERR;
            goto NpsEventsGetExit;
            }
        }

                                        /* Okay, now we can read in our */
                                        /* events.                      */
    if ((fd = OpenFile (szPath, &of, OF_READ | OF_PROMPT)) == (HFILE)-1)
        {
        wRet = NPS_FILE_CANT_OPEN;
        goto NpsEventsGetExit;
        }

                                        /* Read the file header and     */
                                        /* check its validity.  If not  */
                                        /* valid, just set events to 0  */
                                        /* and get out of here.         */
    if ((FileRead (fd,
                   (LPNPSFILEHEADER)&npsFileHeader,
                   sizeof (NPSFILEHEADER)) != sizeof (NPSFILEHEADER)) ||
        STRCMP (npsFileHeader.szHeader, szNPSHeaderFile) ||
        ((npsFileHeader.wVerMajor != NPS_FILE_VER_MAJOR) &&
         (npsFileHeader.wVerMinor != NPS_FILE_VER_MINOR)))
        {
        wRet = NPS_FILE_VER_INVALID;
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
        NpsEventGet (fd, &wEvent);
        }
                                        /* Return to caller.            */
NpsEventsGetExit:
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
WORD PASCAL NpsEventGet(                /* Gets events from NPS.BIN file*/
    HFILE       fd,                     /* Handle to file               */
    LPWORD      lpwEvent)               /* Buffer to current event cntr */
{
    WORD                  wRet;
    NPSENTRYHEADER        npsEntryHeader;
    NPSOTHELLOEVNT        npsOthelloEvent;
    NPSVIPEREVNT          npsViperEvent;
    NPSNIRVANAEVNT        npsNirvanaEvent;
    NPSROSWELLEVNT        npsRoswellEvent;
    extern      char      *szNPSHeaderEntry;
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
        STRCMP (npsEntryHeader.szHeader, szNPSHeaderEntry))
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
          ((npsEntryHeader.wVerMajor == NPS_ENTRY_VER_ROSWELL_MAJOR) &&
           (npsEntryHeader.wVerMinor == NPS_ENTRY_VER_ROSWELL_MINOR)) ||
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
    else if ((npsEntryHeader.wVerMajor == NPS_ENTRY_VER_ROSWELL_MAJOR) &&
             (npsEntryHeader.wVerMinor == NPS_ENTRY_VER_ROSWELL_MINOR))
        {
                                        // Luigi/Roswell/Tesla event,
                                        // xlate to Atomic/Maxwell format.
        if (FileRead (fd,
                      &npsRoswellEvent,
                      npsEntryHeader.wSizeEntry) < sizeof(NPSROSWELLEVNT))
            {
            wRet = NPS_FILE_IO_ERR;
            goto NpsEventGetExit;
            }

        Events[*lpwEvent].wType    = npsRoswellEvent.wType;
        Events[*lpwEvent].bEnabled = npsRoswellEvent.bEnabled;
        Events[*lpwEvent].wFreq    = npsRoswellEvent.wFreq;
        Events[*lpwEvent].wMin     = npsRoswellEvent.wMin;
        Events[*lpwEvent].wHour    = npsRoswellEvent.wHour;
        Events[*lpwEvent].wDay     = npsRoswellEvent.wDay;
        Events[*lpwEvent].wDate    = npsRoswellEvent.wDate;
        Events[*lpwEvent].wMonth   = npsRoswellEvent.wMonth;
        Events[*lpwEvent].wYear    = npsRoswellEvent.wYear;

        lstrcpy ( Events[*lpwEvent].szDescr, 
                  npsRoswellEvent.szDescr );

        lstrcpy ( Events[*lpwEvent].szMessage,
                  npsRoswellEvent.szMessage );

        lstrcpy ( Events[*lpwEvent].szCmdLine, 
                  npsRoswellEvent.szCmdLine );

        lstrcpy ( Events[*lpwEvent].szStartupDir, 
                  npsRoswellEvent.szStartupDir );

        Events[*lpwEvent].wRunStyle = npsRoswellEvent.wRunStyle;
        Events[*lpwEvent].bSound = npsRoswellEvent.bSound;

        lstrcpy ( Events[*lpwEvent].szNamed,
                  npsRoswellEvent.szNamed );

        lstrcpy ( Events[*lpwEvent].szPromptParams, 
                  npsRoswellEvent.szPromptParams );

        Events[*lpwEvent].EntryHeader = npsRoswellEvent.EntryHeader;
        Events[*lpwEvent].EntryHeader.wVerMajor = NPS_ENTRY_VER_MAJOR;
        Events[*lpwEvent].EntryHeader.wVerMinor = NPS_ENTRY_VER_MINOR;
        Events[*lpwEvent].EntryHeader.wSizeEntry = sizeof (NPSEVNT);

        if ( NPS_FREQ_ONETIME == Events[*lpwEvent].wFreq )
            {
            ZeroMemory ( &Events[*lpwEvent].rNextRunTime,
                         sizeof(SYSTEMTIME) );
            }
        else
            {
            CalculateNextRunTime ( &Events[*lpwEvent] );
            }

        Events[*lpwEvent].bMissedAlertPending = FALSE;
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
WORD PASCAL NpsEventsPut(VOID)          /* Write events to NPS.BIN      */
{
    WORD                wEvent;
    HFILE               fd;
    char                szPath[SYM_MAX_PATH];
    WORD                wRet;
    OFSTRUCT            of;
    NPSFILEHEADER       npsFileHeader;
    extern      NPSEVNT Events[];       /* The actual events            */
    extern      char    *szNPSHeaderEntry;
    extern      char    *szNPSHeaderFile;
    extern      char    *szNPSBinFile;

                                        /* Perform initialization       */
    wRet = NOERR;
    fd   = 0;

                                        /* Were do we find our NPS.BIN  */
                                        /* file?                        */
    NpsGetMyPath(szPath);
    STRCAT(szPath, "\\");
    STRCAT(szPath, szNPSBinFile);

    if ((fd = OpenFile(szPath, &of, OF_WRITE | OF_CREATE)) == (HFILE)-1)
        {
        wRet = NPS_FILE_CANT_OPEN;
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
    STRCPY (npsFileHeader.szHeader, szNPSHeaderFile);
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
        goto NpsEventsPutExit;
        }

                                        // Signal all applications using this
                                        // data file that it needs to be
                                        // reloaded.
    NpsSignalReload();


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
WORD NpsEventRemove(                    /* Removes event                */
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
    NpsEventsPut ();

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
WORD NpsEventRemoveExpired(             /* Removes expired event       */
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
// @?  NpsEventExpiredDisplay (hDlg, *pwEvent, &bRemove);
    if (bRemove)
        {
// @?        wRet = NpsEventRemove (hDlg, *pwEvent);
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
WORD  NpsEventsCheckOld(VOID)           /* Removes old events if okay   */
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
            if (Events[wEvent].wFreq != NPS_FREQ_ONETIME)
                {
//                if ( CTime ( Events[wEvent].rNextRunTime ) <=
//                   CTime::GetCurrentTime() )
//                    {
//                    wEvent++;
//                    // Doesn't look like this is ever getting called!
//                    }
//                else
//                    {
                    wEvent++;
//                    }

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
                NpsEventRemoveExpired (&wEvent);
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
                NpsEventRemoveExpired (&wEvent);
                continue;
                }
            if ((WORD)curtm->tm_mday < Events[wEvent].wDate)
                {
                wEvent++;
                continue;
                }
            if ((WORD)curtm->tm_mday > Events[wEvent].wDate)
                {
                NpsEventRemoveExpired (&wEvent);
                continue;
                }
            if ((WORD)curtm->tm_hour < Events[wEvent].wHour)
                {
                wEvent++;
                continue;
                }
            if ((WORD)curtm->tm_hour > Events[wEvent].wHour)
                {
                NpsEventRemoveExpired (&wEvent);
                continue;
                }
            if ((WORD)curtm->tm_min >= Events[wEvent].wMin)
                {
                NpsEventRemoveExpired (&wEvent);
                continue;
                }
            wEvent++;
            }
        }
    return (wRet);
} /* NpsEventsCheckOld() */


//////////////////////////////////////////////////////////////////////////
//
// Function:    CalculateNextRunTime()
//
// Description:
//  Calculates the time and date that a repeating event will run next.
//
// Input:
//  pEvent: [in/out] Pointer to the NPSEVNT struct for the event in
//          question.  The rNextRunTime member of this struct is set to
//          the event's next run date/time.
//
// Returns:
//  Nothing.
//
//////////////////////////////////////////////////////////////////////////
// 4/14/98  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

void CalculateNextRunTime ( NPSEVNT* pEvent )
{
CTime      currTime     = CTime::GetCurrentTime();
CTime      nextTime;
SYSTEMTIME rNextTime    = {0};
int        nHourAdj     = 0;
int        nDayAdj      = 0;
int        nMonthAdj    = 0;
int        nYearAdj     = 0;
BOOL       bIllegalDate = FALSE;

    if ( pEvent->wFreq == NPS_FREQ_ONETIME )
        return;                         // Nothing to do for one-time events.

    switch ( pEvent->wFreq )
        {
        case NPS_FREQ_HOURLY:
            {
            nextTime = CTime ( currTime.GetYear(), currTime.GetMonth(),
                               currTime.GetDay(), currTime.GetHour(),
                               pEvent->wMin, 0 );

            if ( nextTime <= currTime )
                nHourAdj = 1;
            }
        break;


        case NPS_FREQ_WEEKDAY:
        case NPS_FREQ_DAILY:
            {
            nextTime = CTime ( currTime.GetYear(), currTime.GetMonth(),
                               currTime.GetDay(), pEvent->wHour,
                               pEvent->wMin, 0 );

            if ( nextTime <= currTime )
                nDayAdj = 1;
            }
        break;


        case NPS_FREQ_WEEKLY:
            {
            nextTime = CTime ( currTime.GetYear(), currTime.GetMonth(),
                               currTime.GetDay(), pEvent->wHour,
                               pEvent->wMin, 0 );

            if ( currTime.GetDayOfWeek()-1 < pEvent->wDay )
                {
                                        // The trigger day is later in the week.
                nDayAdj = pEvent->wDay - currTime.GetDayOfWeek() + 1;
                }
            else if ( currTime.GetDayOfWeek()-1 > pEvent->wDay )
                {
                                        // The trigger day is earlier in the week.
                nDayAdj = 7 + pEvent->wDay - currTime.GetDayOfWeek() + 1;
                }
            else
                {
                                        // The trigger day is today.
                if ( nextTime <= currTime )
                    nDayAdj = 7;
                }
            }
        break;


        case NPS_FREQ_MONTHLY:
            {
            nextTime = CTime ( currTime.GetYear(), currTime.GetMonth(),
                               pEvent->wDate, pEvent->wHour,
                               pEvent->wMin, 0 );

            // I need to do a little tweaking of the date here, since CTime
            // fudges the date sometimes.  If you pass it something like
            // April 31, the resulting CTime object will be May 1.  Or,
            // passing Feb 30 will result in March 2 (or 1 in leap years).
            // If this happens, then I reset the day to the 1st of the month.

            if ( nextTime.GetDay() != pEvent->wDate )
                {
                                        // Reset day to 1st.  CTime will
                                        // have already incremented the month.
                nextTime = CTime ( nextTime.GetYear(), nextTime.GetMonth(),
                                   1, pEvent->wHour, pEvent->wMin, 0 );
                }

            if ( nextTime <= currTime )
                nMonthAdj = 1;
            }
        break;


        case NPS_FREQ_YEARLY:
            {
            nextTime = CTime ( currTime.GetYear(), pEvent->wMonth+1,
                               pEvent->wDate, pEvent->wHour,
                               pEvent->wMin, 0 );

            // Same tweaking here as in the MONTHLY case above.

            if ( nextTime.GetDay() != pEvent->wDate )
                {
                                        // Reset day to 1st.  CTime will
                                        // have already incremented the month.
                nextTime = CTime ( nextTime.GetYear(), nextTime.GetMonth(),
                                   1, pEvent->wHour, pEvent->wMin, 0 );
                }

            if ( nextTime <= currTime )
                nYearAdj = 1;
            }
        break;
        }   // end switch (wFreq)


    // Now, if we need to make any adjustments to the next run date,
    // figure out the new date....

                                        // With days and hours, it's easy.
    nextTime += CTimeSpan ( nDayAdj, nHourAdj, 0, 0 );

                                        // If the event is run on weekdays,
                                        // check that the next run time we
                                        // have calculated is a weekday.
    if ( NPS_FREQ_WEEKDAY == pEvent->wFreq )
        {
        if ( nextTime.GetDayOfWeek() == 1 )
            {                           // Sunday, so add 1 day
            nextTime += CTimeSpan ( 1, 0, 0, 0 );
            }
        else if ( nextTime.GetDayOfWeek() == 7 )
            {                           // Saturday, so add 2 days
            nextTime += CTimeSpan ( 2, 0, 0, 0 );
            }
        }


                                        // Copy fields into the SYSTEMTIME
                                        // struct that we'll use for the
                                        // rest of the calculations.
    rNextTime.wYear = nextTime.GetYear();
    rNextTime.wMonth = nextTime.GetMonth();
    rNextTime.wDay = nextTime.GetDay();
    rNextTime.wHour = nextTime.GetHour();
    rNextTime.wMinute = nextTime.GetMinute();


    // For month/year adjustments, I do the calculations manually to account
    // for the varying lengths of months. (try saying that fast)

    if ( 0 != nMonthAdj  ||  0 != nYearAdj )
        {
        rNextTime.wMonth += nMonthAdj;

        while ( rNextTime.wMonth > 12 )
            {
            ++rNextTime.wYear;
            rNextTime.wMonth -= 12;
            }

        rNextTime.wYear += nYearAdj;

                                        // Now see if the date we have is
                                        // illegal, eg April 31. If so,
                                        // move the date to the 1st of the
                                        // following month.

                                        // 1. If the date is 2/29, check
                                        // for a leap year.
        if ( rNextTime.wDay == 29  &&  rNextTime.wMonth == 2 )
            {
            if ( !IsLeapYear ( rNextTime.wYear ) )
                {
                bIllegalDate = TRUE;
                }
            }
                                        // 2. If the date is a 30th, it's
                                        // OK unless the month is Feb.
        else if ( rNextTime.wDay == 30  &&  rNextTime.wMonth == 2 )
            {
            bIllegalDate = TRUE;
            }
                                        // 3. If the date is a 31st, check
                                        // that the resulting month has a
                                        // 31st.
        else if ( rNextTime.wDay == 31 )
            {
            switch ( rNextTime.wMonth )
                {
                case 2:                 // February
                case 4:                 // April
                case 6:                 // June
                case 9:                 // September
                case 11:                // November
                    {
                    bIllegalDate = TRUE;
                    }
                }
            }

        if ( bIllegalDate )
            {
            ++rNextTime.wMonth;         // This will never make wMonth 13,
                                        // since all dates in Dec. are legal.
            rNextTime.wDay = 1;
            }
        }   // end if (nMonthAdj != 0 || nYearAdj != 0)

    MEMCPY ( &pEvent->rNextRunTime, &rNextTime, sizeof(SYSTEMTIME) );
}


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
WORD NpsMakeNpsBak (                    /* Creates NpsBin.Bak file      */
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


/////////////////////////////////////////////////////////////////////
// LPTSTR RemoveSurroundingCharsOfType( LPTSTR szStringToModify, LPCTSTR szSetOfCharsToRemove )
//
// Purpose:
//     Function to remove the surrounding characters from the beginning and
//     the end of a string that are found in a set of characters taht is
//     sent to the function.  For example, you may wish to strip all EOL
//     chars and spaces from a string, therefore, you would call this
//     function with a set of chars like " \n\r" which would remove all
//     spaces, carriage returns, and line feeds.
//
// Parameters:
//    LPTSTR szStringToModify - [IN][OUT] string to remove chars from.
//    LPCTSTR szSetOfCharsToRemove - [IN] A set of chars to search for at
//                                        the start and end of a string that
//                                        will be removed.
//
// Returns:
//    Pointer to the modified string.  Will be the same value as the
//    original string.
//    NULL if passed a NULL string.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/15/97 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
LPTSTR RemoveSurroundingCharsOfType(
    LPTSTR szStringToModify,
    LPCTSTR szSetOfCharsToRemove
    )
{
LPTSTR szMovePtr;

    if ( NULL == szStringToModify )
        return NULL;

                                        // Go to the last character of the
                                        // string.
    if ( _tcslen(szStringToModify) )
        {
        szMovePtr = _tcsninc( szStringToModify, _tcslen(szStringToModify)-1 );
        }

                                        // While we have a character in our set of chars
                                        // as the last character of the string...
    while (( _tcschr( szSetOfCharsToRemove, *szMovePtr ))&&(szMovePtr != szStringToModify))
        {
                                        // Chop them off and decrement our
                                        // string pointer.
        *szMovePtr = 0;
        szMovePtr = _tcsdec( szStringToModify, szMovePtr );
        }

                                        // Take care of last character since
                                        // _tcsdec on the last char is undefined.
    if ( szMovePtr == szStringToModify )
        {
        if ( _tcschr( szSetOfCharsToRemove, *szMovePtr ) )
            {
            *szMovePtr = 0;
            }
        }

    szMovePtr = szStringToModify;

    while ( ( _tcschr( szSetOfCharsToRemove, *szMovePtr ) ) && ( *szMovePtr != NULL ) )
        szMovePtr = _tcsinc( szMovePtr );

                                        // Copy the string back to the original
                                        // location by copying the number of
                                        // bytes in the string back to the
                                        // starting location.
    memmove( szStringToModify, szMovePtr, _tcsnbcnt( szMovePtr, _tcslen(szMovePtr) ) + sizeof( TCHAR ) );

    return szStringToModify;
}

/////////////////////////////////////////////////////////////////////
// void NpsSignalReload( void )
//
// Purpose:
//  This function will signal all applications using this data file
//  that it has changed by pulsing an event created for this purpose
//  if one exists.
//
// Parameters:
//  None.
//
// Returns:
//  None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 06-01-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void NpsSignalReload( void )
{
HANDLE          hDataReloadEvent;
extern char *   szNPSReloadDataEventName;

                                        // If anyone is waiting on this event...
    if ( hDataReloadEvent = OpenEvent( EVENT_ALL_ACCESS,
                                       FALSE,
                                       szNPSReloadDataEventName ) )
        {
                                        // Pulse it so that everyone reload's
                                        // their data.
        PulseEvent( hDataReloadEvent );
        }
}
