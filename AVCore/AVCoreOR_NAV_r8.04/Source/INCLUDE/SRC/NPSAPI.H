/////////////////////////////////////////////////////////////////////
// NPSAPI.H - Created - 05-30-97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/npsapi.h_v   1.16   11 Jun 1998 13:36:28   mdunn  $
//
// Description:
//    This header file contains the API's, structs, and defines that
//    are neccessary to manipulate the Norton Program Scheduler's
//    scheduled entries.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/npsapi.h_v  $
// 
//    Rev 1.16   11 Jun 1998 13:36:28   mdunn
// Added bMissedAlertPending member to NPSEVNT struct.
// 
//    Rev 1.15   18 Apr 1998 16:32:24   mdunn
// In NPSEVNT, made the description field larger and removed the last run
// time.  Now only the next run time is kept.
// 
// 
//    Rev 1.14   15 Apr 1998 17:16:48   mdunn
// Changed NPSEVNT struct to hold just the next-run time.
// 
//    Rev 1.13   23 Mar 1998 22:12:20   mdunn
// Added support for running missed recurring events on Win 9x
// 
//    Rev 1.12   17 Mar 1998 15:52:12   mdunn
// Added NPS_EXPIRED_* constants.
// 
//    Rev 1.11   10 Sep 1997 14:45:06   KSACKIN
// Ported changes from QAKG.
//
//    Rev 1.10.1.3   14 Aug 1997 22:03:32   KSACKIN
// Added a string value for the ini value string for Allowing a blank Action
// text value.
//
//    Rev 1.10.1.2   13 Aug 1997 22:20:46   KSACKIN
// Moved some size defines from NPS to NPSAPI.
//
//    Rev 1.10.1.1   13 Aug 1997 22:16:20   KSACKIN
// Moved some defines from NPS to NPSAPI so that they will be available for the
// NPSUI which uses the NPSAPI.
//
//    Rev 1.10.1.0   12 Aug 1997 21:52:56   DALVIDR
// Branch base for version QAKG
//
//    Rev 1.10   06 Aug 1997 08:58:08   JBRENNA
// Add a parameter to StartScheduler. Add StartSchedulerA and StartSchedulerW.
//
//    Rev 1.9   04 Aug 1997 20:51:32   KSACKIN
// Removed ';' after pragma statement.
//
//    Rev 1.8   04 Aug 1997 20:45:24   KSACKIN
// Fixed packing problem with the NPS entry and header structures.  We now set
// packing to 1 for these structures.
//
//    Rev 1.7   02 Jul 1997 17:51:48   KSACKIN
// Added an error string for being unable to display an event.
//
//    Rev 1.6   05 Jun 1997 18:07:18   KSACKIN
// Added functions to determine if the scheduler is running, and start it if it
// is not.
//
//    Rev 1.5   02 Jun 1997 01:29:26   KSACKIN
// Added defines for NPSEventRemove and NPSGetMyPath functions.
//
//    Rev 1.4   02 Jun 1997 00:46:20   KSACKIN
// Added new error defines.
//
//    Rev 1.3   02 Jun 1997 00:11:50   KSACKIN
// Added new API function to update all clients of the schedule.dat file to
// reload their events.
//
//    Rev 1.2   31 May 1997 18:48:00   KSACKIN
// Added version information defines from NPS.h.
//
//    Rev 1.1   30 May 1997 18:59:22   KSACKIN
// Exposed the NPSEventsPut(void) function.
//
//    Rev 1.0   30 May 1997 16:26:20   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__NPSAPI_H)
#define __NPSAPI_H


/************************************************************************/
/* Component return codes                                               */
/************************************************************************/
#define NPS_REGISTER_CANT               ERR_SEV_NUM (ERR_SEV_FATAL,   \
                                                     IDS_ERR_WND_CANT_REGISTER)
#define NPS_CREATE_DIALOG_CANT          ERR_SEV_NUM (ERR_SEV_FATAL,   \
                                                     IDS_ERR_DIALOG_CANT_CREATE)
#define NPS_EVENTS_TOO_MANY             ERR_SEV_NUM(ERR_SEV_ERROR,    \
                                                     IDS_NPS_MAX_EVENTS)
#define NPS_FILE_SPEC_INVAL             ERR_SEV_NUM (ERR_SEV_WARNING, \
                                                     IDS_ERR_FILE_NOT_FOUND)
#define NPS_STRING_CANT_LOAD            ERR_SEV_NUM (ERR_SEV_FATAL,   \
                                                     IDS_ERR_STRING_CANT_LOAD)
#define NPS_OUT_OF_TIMERS               ERR_SEV_NUM (ERR_SEV_ERROR,   \
                                                     IDS_NPS_NO_TIMERS)
#define NPS_NO_WIN_DIR                  ERR_SEV_NUM (ERR_SEV_FATAL,   \
                                                     IDS_ERR_WIN_CANT_GET_DIR)
#define NPS_FILE_CANT_OPEN              ERR_SEV_NUM (ERR_SEV_FATAL,   \
                                                     IDS_ERR_FILE_NOT_FOUND)
#define NPS_FILE_IO_ERR                 ERR_SEV_NUM (ERR_SEV_FATAL,   \
                                                     IDS_ERR_FILE_IO)
#define NPS_MEM_CANT_ALLOC              ERR_SEV_NUM (ERR_SEV_FATAL,   \
                                                     IDS_ERR_MEM_CANT_ALLOC)
#define NPS_MEM_CANT_LOCK               ERR_SEV_NUM (ERR_SEV_FATAL,   \
                                                     IDS_ERR_MEM_CANT_LOCK)
#define NPS_WILDCARD                    ERR_SEV_NUM (ERR_SEV_WARNING,   \
                                                     IDS_NPS_NO_WILDCARDS)
#define NPS_FILE_VER_INVALID            ERR_SEV_NUM (ERR_SEV_WARNING, \
                                                     IDS_NPS_INVAL_FILE)
#define NPS_FILE_CHKSUM_INVALID         ERR_SEV_NUM (ERR_SEV_WARNING, \
                                                     IDS_NPS_INVAL_FILE)
#define NPS_ENTRY_VER_INVALID           ERR_SEV_NUM (ERR_SEV_WARNING, \
                                                     IDS_NPS_INVAL_ENTRY)
#define NPS_PROMPT_COMMAND_LINE         ERR_SEV_NUM (ERR_SEV_WARNING, \
                                                     IDS_NPS_PROMPT_INVAL_FILE)
#define NPS_INVALID_DATETIME            ERR_SEV_NUM (ERR_SEV_WARNING, \
                                                     IDS_NPS_BAD_DATETIME)
#define NPS_STRING_SHELL_ERROR          ERR_SEV_NUM (ERR_SEV_ERROR, \
                                                     IDS_NPS_SHELL_ERROR)
#define NPS_NO_WIN_NT                   ERR_SEV_NUM (ERR_SEV_WARNING, \
                                                     IDS_NPS_NOWINNT)
#define NPS_ERROR_WATCH_EVENT           ERR_SEV_NUM (ERR_SEV_ERROR, \
                                                     IDS_ERR_CREATING_RELOAD_EVENT)
#define NPS_ERROR_DISPLAYING_EVENT      ERR_SEV_NUM (ERR_SEV_ERROR, \
                                                     IDS_ERR_DISPLAYING_EVENT)


#define NPS_FILE_HDR_LEN        15
#define NPS_FILE_HDR_SIZE       (NPS_FILE_HDR_LEN + 1)
#define NPS_ENTRY_HDR_LEN       15
#define NPS_ENTRY_HDR_SIZE      (NPS_ENTRY_HDR_LEN + 1)
#define NPS_INDEX_MAX_ENTRIES   80

#define MAX_DESCR               80
#define MAX_CMDLINE             256
#define MAX_EVENTS              30
#define MAX_NAMED_SIZE          80      /* max size of named event name */
#define MAX_EVENT_SIZE          BSIZE

#define BSIZE                   200
#define MAX_EVENT_SIZE          BSIZE
#define SSIZE                   80      /* max size of constant strings
                                           months, days, and freqs */
#define ESIZE                   100     /* max size of error msgs */
#define DSIZE                   100     /* max size long date string */


#define NPS_FILE_VER_MAJOR      1
#define NPS_FILE_VER_MINOR      1

#define NPS_ENTRY_VER_OTHELLO_MAJOR     1
#define NPS_ENTRY_VER_OTHELLO_MINOR     0
#define NPS_ENTRY_VER_VIPER_MAJOR       2
#define NPS_ENTRY_VER_VIPER_MINOR       0
#define NPS_ENTRY_VER_NIRVANA_MAJOR     3
#define NPS_ENTRY_VER_NIRVANA_MINOR     0
#define NPS_ENTRY_VER_REVOLVER_MAJOR    3
#define NPS_ENTRY_VER_REVOLVER_MINOR    1
#define NPS_ENTRY_VER_ROSWELL_MAJOR     4
#define NPS_ENTRY_VER_ROSWELL_MINOR     0
#define NPS_ENTRY_VER_MAJOR             5
#define NPS_ENTRY_VER_MINOR             3

#define NPS_EXPIRED_RUN_NOW             1
#define NPS_EXPIRED_DELETE              2
#define NPS_EXPIRED_LEAVE_AS_IS         3

/////////////////////////////////////////////////////////////////////
// NPS Structure Definitions.

#pragma pack( push, 1 )

typedef struct tagNPSENTRYHEADER
{
    char        szHeader[NPS_ENTRY_HDR_SIZE];
    WORD        wVerMajor;
    WORD        wVerMinor;
    WORD        wEntryID;
#define EVENT_ID_SCHEDULE       0x01
    WORD        wSizeEntry;
} NPSENTRYHEADER;
typedef NPSENTRYHEADER          *PNPSENTRYHEADER;
typedef NPSENTRYHEADER  FAR     *LPNPSENTRYHEADER;


typedef struct tagNPSEVNT
{
    NPSENTRYHEADER      EntryHeader;

    WORD  wType;                        /* type of event                */

#define EVENT_TYPE_MESSAGE  1
#define EVENT_TYPE_PROGRAM  2
#define EVENT_TYPE_NAMED    3

    SHORTBOOL  bEnabled;                     /* TRUE if event enabled        */

    WORD  wFreq;                        /* frequeny of event            */
#define   NPS_FREQ_ONETIME  0
#define   NPS_FREQ_HOURLY   1
#define   NPS_FREQ_DAILY    2
#define   NPS_FREQ_WEEKDAY  3
#define   NPS_FREQ_WEEKLY   4
#define   NPS_FREQ_MONTHLY  5
#define   NPS_FREQ_YEARLY   6

    WORD  wMin;                         /* minutes                      */
    WORD  wHour;                        /* hour (24 hour fmt            */
    WORD  wDay;                         /* day of the week              */
#define SUN        0
#define MON        1
#define TUE        2
#define WED        3
#define THU        4
#define FRI        5
#define SAT        6

    WORD  wDate;                        /* date of the month            */
    WORD  wMonth;                       /* month of the year            */
    WORD  wYear;                        /* year                         */
    char  szDescr[MAX_DESCR];           /* description of the event     */
    char  szMessage[MAX_CMDLINE];       /* message to display           */
    char  szCmdLine[MAX_CMDLINE];       /* message to display           */
    char  szStartupDir[SYM_MAX_PATH];   /* Startup directory            */
    WORD  wRunStyle;                    /* Run Style                    */
#define NPS_RUN_NORMAL          0
#define NPS_RUN_MINIMIZED       1
#define NPS_RUN_MAXIMIZED       2
    SHORTBOOL  bSound;                       /* TRUE if sound enabled        */
    char  szNamed[MAX_NAMED_SIZE];      /* named event title            */
    char  szPromptParams[MAX_CMDLINE];  /* Params to pass to named event*/

    SYSTEMTIME rNextRunTime;            /* The time this event should've run */

    BOOL       bMissedAlertPending;     // TRUE=this event was missed but
                                        // the user hasn't been notified yet.
                                        // Used on NT only.
} NPSEVNT;
typedef NPSEVNT FAR     *LPNPSEVNT;

#pragma pack( pop )

/************************************************************************/
/* Globals                                                              */
/************************************************************************/

#ifdef MAIN
   #define EXTERN
#else
   #define EXTERN extern
#endif


EXTERN NPSEVNT   Events[MAX_EVENTS];    /* The actual events            */
EXTERN WORD      wNumEvents;            /* Number of actual events      */

EXTERN char      szIniDefDesc[SSIZE];
EXTERN char      szIniDefActionText[SSIZE];
EXTERN char      szIniAllowBlankActionText[SSIZE];
EXTERN char      szIniBlankActionTextMessage[SSIZE];

/************************************************************************/
/* Prototypes                                                           */
/************************************************************************/

BOOL IsSchedulerRunning( void );        /* Determine scheduler status   */

BOOL StartSchedulerA (LPCSTR);          /* Start the scheudler          */
BOOL StartSchedulerW (LPCWSTR);         /* Start the scheudler          */
#ifdef UNICODE
  #define StartScheduler  StartSchedulerW
#else
  #define StartScheduler  StartSchedulerA
#endif

BOOL CreateSchedulerService( void );    /* Create the scheduler service */

BOOL StartSchedulerService( void );     /* Start the scheudler Service  */

WORD PASCAL NpsEventsLoad (             /* Loads any stored events      */
    HWND        hDlg,                   /* Handle to dialog box         */
    BOOL        bInitLoad);             /* TRUE if initial load         */

VOID PASCAL MkEvent (                   /* Makes an event               */
    int         iEvent);                /* Event to make                */

WORD PASCAL NpsEventsGet(               /* Gets events from NPS.BIN file*/
    BOOL        bInitLoad);             /* TRUE if initial load         */

WORD PASCAL NpsEventsPut( void );       /* Write events to NPS.BIN      */

WORD NpsEventRemove(                    /* Removes event                */
    WORD        wEvent);                /* Event to remove              */

WORD NpsEventRemoveExpired(             /* Removes expired event       */
    PWORD       pwEvent);               /* Event to possibly remove     */

VOID NpsGetMyPath (                     /* Makes an event               */
    LPSTR       lpszPath);              /* Event to make                */

void NpsSignalReload( void );           /* Signal all clients to reload */

#endif // if !defined(__NPSAPI_H)
