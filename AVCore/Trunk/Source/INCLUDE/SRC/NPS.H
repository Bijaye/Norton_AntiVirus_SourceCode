/************************************************************************/
/*                                                                      */
/* $Header:   S:/INCLUDE/VCS/nps.h_v   1.20   28 Aug 1998 17:50:12   JBRENNA  $ */
/*                                                                      */
/* Description:                                                         */
/*      This is the main include file for the Norton Program Scheduler. */
/*      It contains all of the definitions, typedefs, and prototypes    */
/*      required by the application.                                    */
/************************************************************************/
/* $Log:   S:/INCLUDE/VCS/nps.h_v  $ */
// 
//    Rev 1.20   28 Aug 1998 17:50:12   JBRENNA
// Port changes from QAKN branch to trunk:
// 1. Rev 1.19.1.1   27 Jun 1998 17:14:30   mdunn
//    Removed NpsRepeatingEventExpiredDlgProc() and
//    NpsRepeatingEventExpiredDisplay().
// 
//    Rev 1.19   18 Apr 1998 16:47:22   mdunn
// Changed declaration of Roswell event struct to not use MAX_DESCR...
// it's been changed for Atomic and the Roswell struct has to remain the
// same size.
// 
//    Rev 1.18   17 Apr 1998 11:38:12   mdunn
// Added NPS_LOGIN_EVENT_NAME.
// 
//    Rev 1.17   15 Apr 1998 17:17:08   mdunn
// Added CalcluateNextRunTime().
// 
//    Rev 1.16   23 Mar 1998 22:38:22   mdunn
// Reinstated bShowTitleTime temporarily so that NT builds won't break for
// the build in a few hours... once I get the NT service figured out it'll
// be removed again.
// 
//    Rev 1.15   23 Mar 1998 22:12:20   mdunn
// Added support for running missed recurring events on Win 9x
// 
//    Rev 1.14   19 Mar 1998 12:57:20   mdunn
// bShowTitleTime is now bAutorunMissedEvents in the options struct
// 
//    Rev 1.13   17 Mar 1998 15:51:18   mdunn
// Made NpsEventDo() non-static.  Changed last param of NpsExpiredEventDisplay()
// from a BOOL* to an int*
// 
//    Rev 1.12   10 Sep 1997 14:47:44   KSACKIN
// Ported changes from QAKG
//
//    Rev 1.11.1.3   13 Aug 1997 22:20:38   KSACKIN
// Moved some size defines from NPS to NPSAPI.
//
//    Rev 1.11.1.2   13 Aug 1997 22:15:44   KSACKIN
// Moved some defines from NPS to NPSAPI so that they will be available for the
// NPSUI which uses the NPSAPI.
//
//    Rev 1.11.1.1   13 Aug 1997 21:42:36   KSACKIN
// Added two new extern characters for the default description and default
// action text.
//
//    Rev 1.11.1.0   12 Aug 1997 21:52:30   DALVIDR
// Branch base for version QAKG
//
//    Rev 1.11   05 Aug 1997 16:25:00   ksackin
// Added a pragma pack statement around the structures in this header file.  We
// want our packing to be 1 byte aligned for compatability.
//
//    Rev 1.10   01 Jun 1997 16:04:28   KSACKIN
// Added prototype for NPSEventsGet( hDlg ).  This function does not take a
// parameter in the NPSAPI, so this prototype resides in NPS.h.
//
//    Rev 1.9   31 May 1997 18:47:38   KSACKIN
// Moved Version information defines into NPSAPI.H.
//
//    Rev 1.8   30 May 1997 16:23:54   KSACKIN
// Moved all items having to do with loading and saving events to NPSAPI.H and
// now include that from this header file.  This is so that if you are only
// using the NPSAPI lib file, you can simply include the NPSAPI.h header file
// and get everything that you need.  Applications such as NPSUI will still
// include nps.h for all UI related items as well.
//
//    Rev 1.7   30 May 1997 13:07:46   KSACKIN
// Added a version of NpsEventsGet() that only takes a BOOL for the bInitLoad
// member.  This version is for the API only.
//
//    Rev 1.6   29 May 1997 00:42:30   ksackin
// Added some defines for the NPSAPI library.
//
//    Rev 1.5   03 Dec 1996 15:20:04   JBRENNA
// Kirin Port: BGERHAR: Fix copyright string; Fixed szCopyright and szVersion
// sizes.
//
//    Rev 1.4   25 Sep 1996 13:54:24   JBRENNA
// Tune the NPS_BUTTON_*_DBCS values for Win95 DOS/V. Update the
// NpsResetButtonBar() prototype.
//
//    Rev 1.3   27 Jun 1996 21:52:20   DALLEE
// LuigiPlus port:
// Upped copyright date to 1996.
//
//    Rev 1.2   11 Jan 1996 08:57:28   JWORDEN
// Port Quake 8 to trunk
//
//    Rev 1.1.1.1   21 Nov 1995 20:22:36   MKEATIN
// added EXTERN variable szVersion
//
//    Rev 1.1   10 Jul 1995 23:20:38   KEITH
// Allow this to be shared.
//
//    Rev 1.0   10 Jul 1995 16:39:48   KEITH
// Initial revision.
//
//    Rev 1.29   24 Jun 1995 16:38:26   MARKL
// Prevented program from running under Windows NT (DUMB!!!) for defect #36309
//
//    Rev 1.28   02 May 1995 13:38:00   MFALLEN
// Using quake shell api
//
//    Rev 1.27   23 Apr 1995 22:16:22   MFALLEN
// On the Win32 platform use the Win95 IShellLink interface to add application
// to startup group and not to the WIN.INI load line.
//
//    Rev 1.26   18 Apr 1995 14:11:26   MFALLEN
// Command line parsing
//
//    Rev 1.25   14 Mar 1995 16:50:56   MFALLEN
// Return value from AskConfirmation
//
//    Rev 1.24   14 Mar 1995 12:32:54   MFALLEN
// How to help
//
//    Rev 1.23   13 Mar 1995 15:35:32   MFALLEN
// Added What's this help.
//
//    Rev 1.22   06 Mar 1995 17:26:28   MFALLEN
// Added revolver structure.
//
//    Rev 1.21   06 Mar 1995 10:12:26   MFALLEN
// Made it work for the Win95 platform.
//
//    Rev 1.20   27 Oct 1994 14:51:10   BRAD
// Added DBCS button bar sizes
//
//    Rev 1.19   27 Apr 1994 10:59:10   BRAD
// Use SYM_MAX_PATH only
//
//    Rev 1.18   29 Mar 1994 01:52:06   BRAD
// WIN32 changes
//
//    Rev 1.17   28 Mar 1994 22:46:26   BRAD
// SYM_EXPORT replaced Copyright string.
//
//    Rev 1.16   26 Mar 1994 21:01:16   BRAD
// Pass the portable param, not lParam
//
//    Rev 1.15   15 Mar 1994 11:58:54   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.14   03 Mar 1994 12:15:04   BRAD
// AWK script applied
//
//    Rev 1.13   10 Feb 1994 12:24:00   PGRAVES
// Add QAK2 changes to trunk.
//
//    Rev 1.11.1.1   13 Dec 1993 18:43:20   MARKL
// Removed ifdefs for NDW
//
//    Rev 1.11.1.0   19 Oct 1993 08:15:02   SCOTTP
// Branch base for version QAK2
//
//    Rev 1.11   15 Oct 1993 00:23:00   BASIL
// added proto of checkoldevents()
//
//    Rev 1.10   12 Oct 1993 16:05:50   BASIL
// Changed buttonbar button sizes for Revolver
//
//    Rev 1.9   01 Oct 1993 13:00:50   MARKL
// Now fully supports named events.
//
//    Rev 1.8   29 Sep 1993 14:58:06   MARKL
// Added named event types.
//
//    Rev 1.7   22 Jul 1993 11:59:10   MARKL
// Added globals for help system.
//
//    Rev 1.6   14 Jun 1993 08:21:52   KYECK
// Added define for MSG_NPS_HIDE.
//
//    Rev 1.5   10 Jun 1993 17:05:16   KYECK
// Changed size of buttonbar defines.
//
//    Rev 1.4   10 Jun 1993 13:22:58   KYECK
// Added bSound flag to Event structure.
//
//    Rev 1.3   10 Jun 1993 09:35:36   KYECK
// Added #defines, macros and prototypes used to support the buttonbar,
// configure dialog box and 'yearly' events.
//
//    Rev 1.2   21 Apr 1993 22:05:28   MARKL
// Nirvana gives the schedule a chance to take an a radically new look/feel
// in the Add/Edit dialog!!!
//
//    Rev 1.1   16 Mar 1993 20:02:36   MARKL
// Added common dialog file browse hook proc.
//
//    Rev 1.0   15 Mar 1993 17:46:48   MARKL
// Initial revision.
//
//    Rev 1.5   22 Feb 1992 13:58:56   MARKL
// Added drag/drop.
//
//    Rev 1.4   05 Feb 1992 20:48:04   MICHAELR
// Restored values of NPS_RUN_..
//
//    Rev 1.3   05 Feb 1992 20:31:20   MICHAELR
// Equated the NPS_RUN_ defines to their Windows conterparts,
// SW_SHOWNORMAL, etc.
//
//    Rev 1.2   19 Jan 1992 12:28:04   MARKL
// Now handles common properties.  Still need to add support for the
// new NWinExec function and the Standard Directory Browse.
//
//    Rev 1.1   02 Oct 1991 12:06:32   MARKL
// Made WinStric.H pass.
//
//    Rev 1.0   28 Jul 1991 11:37:38   MARKL
// Initial revision.
//
//    Rev 1.26   08 Jul 1991 13:29:36   MARKL
// SCHEDULE.BIN now has checksum
//
//    Rev 1.25   04 Jun 1991 20:33:12   MARKL
// Added NpsPaintClock()
//
//    Rev 1.24   03 Jun 1991 12:19:20   MARKL
// Timer still runs if notifications are up.
//
//    Rev 1.23   01 Jun 1991 20:37:08   MARKL
// Segmenting forced many LOCAL routines to become global.
//
//    Rev 1.22   23 May 1991 11:45:56   MARKL
// User can enter bogus command now.
//
//    Rev 1.21   02 May 1991 11:53:18   MARKL
// No change.
//
//    Rev 1.20   22 Apr 1991 17:44:08   MARKL
// Now attempts to launch events that may have been skipped
// over if a timer message was missed.
//
//    Rev 1.19   22 Apr 1991 13:30:54   MARKL
// Has version 1.0 file and entry format.  Can run application minimized.
//
//    Rev 1.18   15 Apr 1991 14:37:26   MARKL
// Now uses StdAboutBox().
//
//    Rev 1.17   01 Apr 1991 19:16:02   MARKL
// No change.
//
//    Rev 1.16   01 Apr 1991  9:21:08   MARKL
// Added error codes.
//
//    Rev 1.15   30 Mar 1991 16:30:34   MARKL
// Scheduler now makes a backup of NPS.BIN to NPSBIN.BAK for the user upon
// startup.
//
//    Rev 1.14   29 Mar 1991 16:15:00   MARKL
// Cleaned up NpsEventsGet() and started to add expanded file format for
// NPS.BIN.
//
//    Rev 1.13   18 Mar 1991 18:51:34   MARKL
// Added hide & autoload checkboxes.
//
//    Rev 1.12   05 Mar 1991 12:07:06   MARKL
// Added the global hResInst.  All resources (with the exception of
// npsWClass) was moved to NWRES.
//
//    Rev 1.11   04 Mar 1991 17:34:30   MARKL
// Major modifications.  Added SYNC timer to set seconds up to 0.
// Local prototypes were moved to their respective source routines.
//
//    Rev 1.10   01 Mar 1991 18:53:10   MARKL
// Added internationalization.  There is ONE function that converts
// 24 hour time to 12 hour time and visaversa.
//
//    Rev 1.9   26 Feb 1991 16:27:08   MARKL
// Restores previous application once event message is responed
// to.  Plays tune when event occurs.
//
//    Rev 1.8   26 Feb 1991 13:24:52   MARKL
// NpsAddModifyDlgDeInit() was added.
//
//    Rev 1.7   25 Feb 1991 18:30:58   MARKL
// Broke NPS out into modules.
//
//    Rev 1.6   25 Feb 1991 10:06:32   MARKL
// Add NpsSetDateRange() routine to verify that the number of days
// in each month are correct.
//
//    Rev 1.5   25 Feb 1991  8:42:10   MARKL
// Added format string specifications for the hours and minutes
// spin buttons.
//
//    Rev 1.4   18 Feb 1991 15:05:16   MARKL
// Still renaming routines and adding more.  Can now pop up standard
// browse and validate file spec for program specifiction when adding,
// modifying, or copying an event.
//
//    Rev 1.3   18 Feb 1991  8:07:26   MARKL
// Added calls to give scheduler a 3d-calculator type look and feel.
//
//    Rev 1.2   12 Feb 1991 23:08:56   MARKL
// Updated for alpha 7.
//
//    Rev 1.0   12 Feb 1991 23:06:00   MARKL
// Initial revision.
/************************************************************************/



#ifndef _NPS_H
#define _NPS_H

#ifndef _INC_TIMEB
#include <sys\timeb.h>
#endif

#include "npsapi.h"                     // NPS API funcitons and struct defines.

#ifndef _NPS_STRUCTS_ONLY_

#ifndef BBS_NOTIFYSELECTONLY
#include "ngui.h"
#endif

/************************************************************************/
/* Help callback function used together with NpsDlgOnContextHelp()      */
/* I do not know what it is for. Alex Matic is the help expert that     */
/* should be consulted about it.                                        */
/************************************************************************/
                                        // ------------------------------------
                                        // ListBox Item Slection Proc:
                                        // Returns 0 based index of curr. sel.
                                        // ------------------------------------

typedef int (FAR *LB_ITEMSELECTIONPROC)(HWND hwndListBoxKindOfCtrl);


//***************************************************************************
// Application messages
//***************************************************************************

#define  APP_MESSAGE_TRAY_NOTIFY         (WM_APP + 100)


#endif // #ifndef _NPS_STRUCTS_ONLY_

//***************************************************************************
// Confirmation related defines
//***************************************************************************

typedef enum tagCONFIRM
{
    CONFIRM_EXIT = 1,                   // Confirm on exit
    CONFIRM_DELETE,                     // Confirm on delete
    CONFIRM_SAVE,                       // Confirm on save
    CONFIRM_EDIT                        // Confirm on edit

} CONFIRM;

typedef WORD    CONFSTATUS;

#define CONFSTATUS_ALLOW    1
#define CONFSTATUS_DISALLOW 2
#define CONFSTATUS_MINIMIZE 3


//***************************************************************************
// File Header IDS
//***************************************************************************

#define _NPS_C          1000
#define _NPSADEDT_C     1001
#define _NPSCLOCK_C     1002
#define _NPSCTLS_C      1003
#define _NPSDATES_C     1004
#define _NPSDO_C        1005
#define _NPSEVNTS_C     1006
#define _NPSINIT_C      1007
#define _NPSNOTFY_C     1008
#define _NPSCFG_C       1009


/************************************************************************/
/* other constants                                                      */
/************************************************************************/
#define NPS_MSG_INIT            (WM_USER + 200)
#define NPS_MSG_HIDE            (WM_USER + 201)
#define NPS_EVENT_MENU_POS      0       /* Event menu position       */
#define NPS_OPTIONS_MENU_POS    1       /* Options menu position     */
#define OLD_SYM_MAX_PATH        130

#define NDAY                    7
#define NMONTH                  12
#define NFREQ                   7
#define NAM                     2
#define NORD                    4
#define NTABSTOPS               4
#define NRUNSTYLE               3

#define NPS_TIMER_ID_NONE       0
#define NPS_TIMER_ID_SYNC       1
#define NPS_TIMER_ID_MIN        2
#define NPS_TIMER_INTRVL_SYNC   200
#define NPS_TIMER_INTRVL_MIN    ((WORD)(unsigned int)60000)

#define NPS_HOURS_AM     0
#define NPS_HOURS_PM     1

#define NPS_24TO12       0
#define NPS_12TO24       1

#define NPS_FORMAT_MIN   "%02d"
#define NPS_FORMAT_HR    "%2d"
#define NPS_FORMAT_HR0   "%02d"
#define NPS_FORMAT_DATE  "%2d"
#define NPS_FORMAT_DAY   "%d"

#define NPS_PROP_PREV    "NPSPropPrevWindow"

#define NPS_BUTTON_WIDTH_BITMAPNTEXT    24
#define NPS_BUTTON_WIDTH_BITMAPNTEXT_DBCS 24
#define NPS_BUTTON_HEIGHT_BITMAPNTEXT   24
#define NPS_BUTTON_HEIGHT_BITMAPNTEXT_DBCS 21
#define NPS_BUTTON_WIDTH_BITMAP         24
#define NPS_BUTTON_HEIGHT_BITMAP        24
#define NPS_BUTTON_HEIGHT_BITMAP_DBCS   21
#define NPS_BUTTON_WIDTH_TEXT           30
#define NPS_BUTTON_WIDTH_TEXT_DBCS      30
#define NPS_BUTTON_HEIGHT_TEXT          12
#define NPS_FIELD_OFFSET                2

#define NPS_BBARSTYLE_DEFAULT           BBS_NOTIFYSELECTONLY | \
                                        BBS_CENTERBUTTONS | \
                                        BBS_NORAISEDBKGROUND | \
                                        BBS_DRAWBITMAPS | BBS_DRAWTEXTENABLED
#define NPS_BBARSTYLE_BITMAPS           0x0001
#define NPS_BBARSTYLE_TEXT              0x0002

#define NPS_DISPLAY_BBAR(lStyle) \
                (BOOL)((lStyle & (BBS_DRAWTEXTENABLED|BBS_DRAWBITMAPS)) != 0)
#define NPS_DISPLAY_BBAR_TEXT(lStyle) \
                (BOOL)((lStyle & BBS_DRAWTEXTENABLED) != 0)
#define NPS_DISPLAY_BBAR_BITMAPS(lStyle) \
                (BOOL)((lStyle & BBS_DRAWBITMAPS) != 0)

#define MAX_TRAY_MESSAGE    50

#define NPS_LOGIN_EVENT_NAME            _T("Nps_Login_Event")

// ---------------------------------------------------------------------------
// Scheduler options. A block of memory of this size is allocated during init
// and freed up when exiting. Pointer to this structure is stored with main
// dialog box. Attempting to eliminate some global variables before they
// multiply out of control.
// ---------------------------------------------------------------------------

#pragma pack( push, 1 )

typedef struct tagNPSOPTIONS
{
    struct tagCONFIRM
        {
        BOOL    bOnExit;                // Confirm before exiting
        BOOL    bOnDelete;              // Confirm before deleting
        BOOL    bOnSave;                // Confirm before save
        BOOL    bOnEdit;                // Confirm before edit save
        } Confirm;

    struct tagBUTTONBAR
        {
        UINT    uStyle;                 // any BBS_ define
        BOOL    bShow;                  // show button bar
        } ButtonBar;

    BOOL    bStatusBar;                 // Show follow help
    BOOL    bHide;                      // Hide when active
    BOOL    bLoad;                      // Load during startup

    BOOL    bShowTitleTime;             //&^ Just here temporarily so the 9x
                                        // stuff doesn't break NT.

    BOOL    bAutorunMissedEvents;       // TRUE=Autorun missed events,
                                        // FALSE=Prompt user for action.
                                        

} NPSOPTIONS, FAR * LPNPSOPTIONS;

#define SaveNpsOptions(h) SaveLoadNpsOptions(h,FALSE)
#define LoadNpsOptions(h) SaveLoadNpsOptions(h,TRUE)

#define ShowTrayIcon(h)   ShowHideTrayIcon(h,TRUE)
#define HideTrayIcon(h)   ShowHideTrayIcon(h,FALSE)

// ---------------------------------------------------------------------------

/************************************************************************/
/* File format typedefs                                                 */
/************************************************************************/
typedef struct tagNPSFILEHEADER
{
    char        szHeader[NPS_FILE_HDR_SIZE];
    WORD        wVerMajor;
    WORD        wVerMinor;
    DWORD       dwChecksum;
    WORD        wNumEntries;
    LONG        lNextEntryOffset;
} NPSFILEHEADER;
typedef NPSFILEHEADER           *PNPSFILEHEADER;
typedef NPSFILEHEADER   FAR     *LPNPSFILEHEADER;



/************************************************************************/
/* Event structure and typedef of old OTHELLO Events.  Must convert to  */
/* new NPSEVNT structure.                                               */
/************************************************************************/
typedef struct tagNPSOTHELLOEVNT
{
    NPSENTRYHEADER      EntryHeader;

    WORD  wType;                        /* type of event                */
    WORD  wFreq;                        /* frequeny of event            */
    WORD  wMin;                         /* minutes                      */
    WORD  wHour;                        /* hour (24 hour fmt            */
    WORD  wDay;                         /* day of the week              */
    WORD  wDate;                        /* date of the month            */
    WORD  wMonth;                       /* month of the year            */
    WORD  wYear;                        /* year                         */
    char  szDescr[MAX_DESCR];           /* description of the event     */
    char  szMessage[MAX_CMDLINE];       /* message or command line      */
    SHORTBOOL  bRunMinimized;                /* TRUE to run minimized        */
} NPSOTHELLOEVNT;



/************************************************************************/
/* Event structure and typedef of old VIPER Events.  The only difference*/
/* is that we now use common properties for startup directory and for   */
/* what run style (normal, minimized, maximized) to use.                */
/************************************************************************/
typedef struct tagNPSVIPEREVNT
{
    NPSENTRYHEADER      EntryHeader;

    WORD  wType;                        /* type of event                */
    WORD  wFreq;                        /* frequeny of event            */
    WORD  wMin;                         /* minutes                      */
    WORD  wHour;                        /* hour (24 hour fmt            */
    WORD  wDay;                         /* day of the week              */
    WORD  wDate;                        /* date of the month            */
    WORD  wMonth;                       /* month of the year            */
    WORD  wYear;                        /* year                         */
    char  szDescr[MAX_DESCR];           /* description of the event     */
    char  szMessage[MAX_CMDLINE];       /* message or command line      */
    char  szStartupDir[OLD_SYM_MAX_PATH];   /* Startup directory            */
    WORD  wRunStyle;                    /* Run Style                    */
} NPSVIPEREVNT;
typedef NPSVIPEREVNT FAR        *LPNPSVIPEREVNT;

/************************************************************************/
/* Event structure and typedef of NIRVANA Events.  The message and      */
/* command line are now stored in different fields and we have added    */
/* a Enable this event and sound flags.                                 */
/************************************************************************/
typedef struct tagNPSNIRVANAEVNT
{
    NPSENTRYHEADER      EntryHeader;

    WORD  wType;                        /* type of event                */
    SHORTBOOL  bEnabled;                     /* TRUE if event enabled        */
    WORD  wFreq;                        /* frequeny of event            */
    WORD  wMin;                         /* minutes                      */
    WORD  wHour;                        /* hour (24 hour fmt            */
    WORD  wDay;                         /* day of the week              */
    WORD  wDate;                        /* date of the month            */
    WORD  wMonth;                       /* month of the year            */
    WORD  wYear;                        /* year                         */
    char  szDescr[MAX_DESCR];           /* description of the event     */
    char  szMessage[MAX_CMDLINE];       /* message to display           */
    char  szCmdLine[MAX_CMDLINE];       /* message to display           */
    char  szStartupDir[OLD_SYM_MAX_PATH];   /* Startup directory            */
    WORD  wRunStyle;                    /* Run Style                    */
    SHORTBOOL  bSound;                       /* TRUE if sound enabled        */
} NPSNIRVANAEVNT;
typedef NPSNIRVANAEVNT FAR *LPNPSNIRVANAEVNT;

/************************************************************************/
/* Event structure and typedef of new REVOLVER Events.  This version    */
/* supports named events or action types (as the code refers to them).  */
/************************************************************************/

typedef struct tagNPSREVOLVEREVNT
{
    NPSENTRYHEADER      EntryHeader;

    WORD  wType;                        /* type of event                */
    SHORTBOOL  bEnabled;                     /* TRUE if event enabled        */
    WORD  wFreq;                        /* frequeny of event            */
    WORD  wMin;                         /* minutes                      */
    WORD  wHour;                        /* hour (24 hour fmt            */
    WORD  wDay;                         /* day of the week              */
    WORD  wDate;                        /* date of the month            */
    WORD  wMonth;                       /* month of the year            */
    WORD  wYear;                        /* year                         */
    char  szDescr[MAX_DESCR];           /* description of the event     */
    char  szMessage[MAX_CMDLINE];       /* message to display           */
    char  szCmdLine[MAX_CMDLINE];       /* message to display           */
    char  szStartupDir[OLD_SYM_MAX_PATH]; /* Startup directory            */
    WORD  wRunStyle;                    /* Run Style                    */
    SHORTBOOL  bSound;                       /* TRUE if sound enabled        */
    char  szNamed[MAX_NAMED_SIZE];      /* named event title            */
    char  szPromptParams[MAX_CMDLINE];  /* Params to pass to named event*/
} NPSREVOLVEREVNT;
typedef NPSREVOLVEREVNT FAR *LPNPSREVOLVEREVNT;


/************************************************************************/
/* Event structure and typedef of old Luigi/Roswell/Tesla Events.       */
/* Only change is the addition of a last-run member.                    */
/************************************************************************/

// Note that szDescr is now hard-coded to 40 because I changed the
// value of MAX_DESCR for Atomic, and the size of this struct has to
// remain the same.  4/18/98 Mike Dunn.

typedef struct tagNPSROSWELLEVNT
{
    NPSENTRYHEADER      EntryHeader;

    WORD  wType;                        /* type of event                */
    SHORTBOOL  bEnabled;                     /* TRUE if event enabled        */
    WORD  wFreq;                        /* frequeny of event            */
    WORD  wMin;                         /* minutes                      */
    WORD  wHour;                        /* hour (24 hour fmt            */
    WORD  wDay;                         /* day of the week              */
    WORD  wDate;                        /* date of the month            */
    WORD  wMonth;                       /* month of the year            */
    WORD  wYear;                        /* year                         */
    char  szDescr[40];                  /* description of the event     */
    char  szMessage[MAX_CMDLINE];       /* message to display           */
    char  szCmdLine[MAX_CMDLINE];       /* message to display           */
    char  szStartupDir[SYM_MAX_PATH];   /* Startup directory            */
    WORD  wRunStyle;                    /* Run Style                    */
    SHORTBOOL  bSound;                       /* TRUE if sound enabled        */
    char  szNamed[MAX_NAMED_SIZE];      /* named event title            */
    char  szPromptParams[MAX_CMDLINE];  /* Params to pass to named event*/
} NPSROSWELLEVNT;
typedef NPSROSWELLEVNT FAR *LPNPSROSWELLEVNT;


#pragma pack( pop )

#ifndef _NPS_STRUCTS_ONLY_

/************************************************************************/
/* Globals                                                              */
/************************************************************************/
#ifdef MAIN
   #define EXTERN
#else
   #define EXTERN extern
#endif

EXTERN HINSTANCE ghInst;                /* Application Instance handle  */
EXTERN HINSTANCE hResInst;              /* NWRES instance handle        */
EXTERN HWND      hWndScheduler;         /* Handle to application.       */
EXTERN struct    _timeb  LastTime;      /* Last time processed.         */
EXTERN int       nCurEvent;             /* Index to current event       */
EXTERN WORD      wTimerID;              /* Current Timer ID             */
EXTERN BOOL      b24Hour;               /* TRUE if 24 Hour format       */
EXTERN BOOL      b24HourOld;            /* Maintains old value b24Hour  */
EXTERN BOOL      g_bDontSaveInNpsDo;
EXTERN UINT      uHelpBrowseFileMsg;    /* Common dialog message        */
EXTERN char      szTimeSep[SSIZE];      /* time separator               */
EXTERN char      szHourFmt[SSIZE];      /* format string for hours      */
EXTERN char      months[NMONTH][SSIZE];
EXTERN char      days[NDAY][SSIZE];
EXTERN char      freqs[NFREQ][SSIZE];
EXTERN char      ampm[NAM][SSIZE];
EXTERN char      th[NORD][SSIZE];
EXTERN char      runstyle[NRUNSTYLE][SSIZE];
EXTERN char      szTitle[ESIZE];
EXTERN char      szAutoLoad[SSIZE];
EXTERN char      szHide[SSIZE];
EXTERN char      szTitleTime[SSIZE];
EXTERN char      szButtonStyle[SSIZE];
EXTERN char      szButtonShow[SSIZE];
EXTERN char      szStatusBar[SSIZE];
EXTERN char      szConfirmExit[SSIZE];
EXTERN char      szConfirmDelete[SSIZE];
EXTERN char      szConfirmSave[SSIZE];
EXTERN char      szConfirmEdit[SSIZE];
EXTERN char      szIniNamedEvents[SSIZE];
EXTERN char      szIniName[SSIZE];
EXTERN char      szIniPrompt[SSIZE];
EXTERN char      szIniCmdLine[SSIZE];
EXTERN char      szIniStartupDir[SSIZE];
EXTERN char      szIniRunStyle[SSIZE];
EXTERN char      szEventMessage[SSIZE];
EXTERN char      szEventProgram[SSIZE];
EXTERN char      szNortonIni[SYM_MAX_PATH];
EXTERN char      szBinFile[SYM_MAX_PATH];
EXTERN char      szBinBakFile[SYM_MAX_PATH];
EXTERN char      szHeaderFile[NPS_FILE_HDR_SIZE];
EXTERN char      szHeaderEntry[NPS_ENTRY_HDR_SIZE];
EXTERN char      szAppName[]
                  #ifdef MAIN
                     = "npsWClass"
                  #endif
                     ;
EXTERN char    szCopyright[ESIZE]
                  #ifdef MAIN
                     = "Copyright (C) 1991-1996 Symantec Corporation"
                  #endif
                     ;
EXTERN char    szVersion[ESIZE]
                  #ifdef MAIN
                     = "Version 2.0"
                  #endif
                     ;
EXTERN HWND    hDlgModelessActive       /* Handle active modeless dlg   */
                  #ifdef MAIN
                     = NULL
                  #endif
                     ;
EXTERN WORD    wNumNotify               /* Number of notify boxes up    */
                  #ifdef MAIN
                     = 0
                  #endif
                     ;
EXTERN WORD    wDaysInMonth[]
                  #ifdef MAIN
                     = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
                  #endif
                     ;
#define NPS_MONTH_FEB 1
EXTERN HELPSTRUCT HelpInfo
                  #ifdef MAIN
                     = {0}
                  #endif
                     ;



/************************************************************************/
/* Prototypes                                                           */
/************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif
LRESULT SYM_EXPORT CALLBACK MainWndProc (   /* Main Window Proc for NPS     */
    HWND        hDlg,                   /* Handle to dialog box         */
    UINT        wMessage,               /* Message to process           */
    WPARAM      wParam,                 /* wParam of message            */
    LPARAM      lParam);                /* lParam of message            */

BOOL SYM_EXPORT CALLBACK NpsMainWnd3DEnumProc (// Makes control 3D
    HWND        hWndChild,              // [in] Window handle of child window
    LPARAM      lParam);                // [in] not used

BOOL SYM_EXPORT CALLBACK NpsAddEditDlgProc( /* Add / Edit event settings    */
    HWND        hDlg,                   /* Handle to dialog box         */
    UINT        wMessage,               /* Message to process           */
    WPARAM      wParam,                 /* WPARAM data for message      */
    LPARAM      lParam);                /* LPARAM data for message      */

BOOL SYM_EXPORT CALLBACK NpsConfigDlgProc(  // App configuration settings
    HWND        hDlg,                   // Handle to dialog box
    UINT        wMessage,               // Message to process
    WPARAM      wParam,                 // WPARAM data for message
    LPARAM      lParam);                // LPARAM data for message

WORD SYM_EXPORT CALLBACK NpsDropEnumProc (  /* For Dropping files into NPS  */
    HWND        hWndFrameNotUsed,
    WORD        wType,                  /* Type of thing being dropped  */
    LPVOID      lpRec,
    LPSTR       lpszName,               /* File name to view            */
    POINT       ptNotUsed,
    WORD        wStateNotUsed,
    DWORD       dwUser);                /* Not used by this callback    */

BOOL SYM_EXPORT CALLBACK NpsEventExpiredDlgProc( /* Do we nuke expired event?    */
    HWND        hDlg,                   /* Handle to dialog box         */
    UINT        wMessage,               /* Message to process           */
    WPARAM      wParam,                 /* WPARAM parameter             */
    LPARAM      lParam);                /* LPARAM parameter             */

BOOL SYM_EXPORT CALLBACK NpsCommonOptionsDlgProc(  /* Common Options settings      */
    HWND        hDlg,                   /* Handle to dialog box         */
    UINT        wMessage,               /* Message to process           */
    WPARAM      wParam,                 /* WPARAM data for message      */
    LPARAM      lParam);                /* LPARAM data for message      */

BOOL SYM_EXPORT CALLBACK NpsEventNotifyDlgProc ( /* Notification dialog box */
    HWND        hDlg,                   /* Handle to dialog box         */
    UINT        wMessage,               /* Message to process           */
    WPARAM      wParam,                 /* WPARAM parameter             */
    LPARAM      lParam);                /* LPARAM parameter             */

UINT SYM_EXPORT CALLBACK NpsFileBrowseHookProc (    // For handling File Browse
    HWND        hWnd,                           // [in] Window handle
    UINT        wMessage,                       // [in] Message to process
    WPARAM      wParam,                         // [in] wParam of message
    LPARAM      lParam);                        // [in] lParam of message

#ifdef __cplusplus
}
#endif

WORD PASCAL NpsRegisterClass (          /* Registers the NPS wnd class  */
    VOID);                              /* No parameters.               */

WORD PASCAL NpsPrevInstActivate (       /* Registers the NPS wnd class  */
    VOID);                              /* No parameters.               */

WORD PASCAL NpsDialogCreate (           /* Creates Nps Dialog Box       */
    PHWND        phDlg,                 /* For returning handle to box  */
    int          nCmdShow);             /* How to initially display     */

WORD PASCAL NpsTimerKill (              /* Kills current timer          */
    HWND        hDlg);                  /* Handle to dialog box         */

WORD PASCAL NpsTimerStart (             /* Starts our NPS timer         */
    HWND        hDlg);                  /* Handle to dialog box         */

WORD PASCAL NpsEventNotifyDisplay(      /* Display event notify box     */
    HWND        hDlg,                   /* Application window handle    */
    WORD        wIndex);                /* Index in Event list          */

WORD PASCAL NpsInitTime (               /* Inits international time info*/
    VOID);                              /* Takes no parameters          */

WORD PASCAL NpsEventsGet(               /* Gets events from NPS.BIN file*/
    HWND        hDlg,                   /* Handle to calling dialog box */
    BOOL        bInitLoad);             /* TRUE if initial load         */

WORD PASCAL NpsEventGet(                /* Gets events from NPS.BIN file*/
    HFILE       fd,                     /* Handle to file               */
    LPWORD      lpwEvent                /* Buffer to current event cntr */
    );

WORD PASCAL NpsEventRemove(             /* Removes event                */
    HWND        hDlg,                   /* Handle of calling dialog     */
    WORD        wEvent);                /* Event to remove              */

WORD LOCAL PASCAL NpsEventRemoveExpired(/* Removes expired event        */
    HWND        hDlg,                   /* Handle of calling dialog     */
    PWORD       pwEvent);               /* Event to possibly remove     */

WORD PASCAL NpsEventControlsDisable(    /* Disables event ctls          */
    HWND        hDlg);                  /* Handle to dialog box         */

WORD PASCAL NpsEventDo (                /* Executes event               */
    HWND        hWnd,                   /* Window handle                */
    WORD        wEvent,                 /* Index to event to execute    */
    PBOOL       pbRemoved);             /* TRUE if removed from list    */

WORD PASCAL NpsEventExpiredDisplay (    /* Do we remove expired event?*/
    HWND        hDlg,                   /* Application window handle    */
    WORD        wEvent,                 /* Event to possibly remove     */
    int*        pnAction);              /* What to do with the event    */

WORD PASCAL NpsEventListMsg (           /* Handles Event List Box       */
    HWND        hDlg,                   /* Handle to dialog box         */
    UINT        uCmd);                  /* lParam of WM_COMMAND message */

WORD PASCAL NpsEventAddMsg (            /* Handles Add Button           */
    HWND        hDlg);                  /* Handle to dialog box         */

WORD PASCAL NpsEventEditMsg (           /* Handles Edit Button          */
    HWND        hDlg);                  /* Handle to dialog box         */

WORD PASCAL NpsEventCopyMsg (           /* Handles Copy Button          */
    HWND        hDlg);                  /* Handle to dialog box         */

WORD PASCAL NpsEventRemoveMsg (         /* Handles Remove Button        */
    HWND        hDlg);                  /* Handle to dialog box         */

WORD PASCAL NpsAddEditDlgInit(          /* Initializes Add / Edit Dlg   */
    HWND        hDlg,                   /* Handle to dailog box         */
    WORD        wCaptionStrID);         /* What caption to display      */

WORD PASCAL NpsSetDateRange (           /* Sets correct date range      */
    HWND        hDlg);                  /* Handle to dialog box         */

WORD PASCAL NpsAddEditCtlsInit (        /* Init combo and spin buttons  */
    HWND        hDlg,                   /* Handle to dialog box         */
    BOOL        bInitDialog);           /* TRUE if initing dialog box   */

WORD PASCAL NpsHoursConvert (           /* Converts hours 12->24/24->12 */
    HWND        hDlg,                   /* Handle to dialog box         */
    PWORD       pwHours,                /* Hours to convert             */
    PWORD       pwAmPmFlag,             /* Am / Pm indicator            */
    WORD        wConversion);           /* Specifies conversion         */

WORD PASCAL NpsPaintClock (             /* Paints clock in client area  */
    HWND        hDlg);                  /* Handle to main dialog window */

VOID WINAPI NpsSmallFontSet (           // Specified control to small font
    HWND        hWnd,                   // Window that owns child
    WORD        wChildID);              // Child control id

WORD WINAPI NpsGetValidTime (           // Gets valid time
    HWND        hWnd,                   // [in] Handle to time field
    LPWORD      lpwHour,                // [in/out] Hour
    LPWORD      lpwMin,                 // [in/out] Minutes
    BOOL        bDispAMPM);             // [in] TRUE if AM/PM displayed

WORD WINAPI NpsGetValidDate (           // Gets valid date
    HWND        hWnd,                   // [in] Handle to date field
    LPWORD      lpwMonth,               // [in/out] Month
    LPWORD      lpwDate,                // [in/out] Date
    LPWORD      lpwYear);               // [in/out] Year

WORD WINAPI NpsInitButtonBar (          // Initializes the button bar
    HWND        hDlg,                   // [in] Handle to dialog box
    HWND        hWndButtonBar,          // [in] handle to button bar
    UINT        uButtonStyle,           // [in] button bar style to use
    BOOL        bMoveTop                // [in] move button bar to top
    );

WORD WINAPI NpsResetButtonBar (         // Resets the button bar
    HWND        hDlg,                   // [in] Handle to dialog box
    HWND        hWndButtonBar,          // [in] handle to button bar
    UINT        uButtonStyle,           // [in] button bar style to use
    UINT        uButtonMaxWidth,        // [in] Calculated Max Button Width
    BOOL        bInitDialog,            // [in] TRUE if initing dlg box
    BOOL        bMoveTop                // [in[ TRUE to move to top of dialog
    );

WORD WINAPI NpsScreenToClient(          // Convert rect to client coords
    HWND        hWnd,                   // [in] Window handle
    LPRECT      lpcrWnd                 // [in/out] Window rect
    );

WORD PASCAL NpsEventsCheck (            /* Do we need to do an event?   */
    HWND        hDlg);                  /* Handle to main dialog box    */

WORD PASCAL MkEventStr (                /* Makes an event list item str */
    WORD        wEvent,                 /* Event to make string for     */
    PSTR        pszBuf);                /* Ptr to buffer to load        */

WORD  PASCAL NpsEventsCheckOld(         /* Removes old events if okay   */
    HWND        hDlg);                  /* Handle to dialog box         */

VOID WINAPI SaveLoadNpsOptions (        // Either saves or loads our options
    HWND        hDlg,                   // [in] required for Quack functions
    BOOL        bLoad                   // [in] TRUE to load options
    );

BOOL SYM_EXPORT WINAPI NpsOptsGeneralDlgProc (
    HWND        hDlg,
    UINT        uMessage,
    WPARAM      wParam,
    LPARAM      lParam
    );

BOOL SYM_EXPORT WINAPI NpsOptsGlobalDlgProc (
    HWND        hDlg,
    UINT        uMessage,
    WPARAM      wParam,
    LPARAM      lParam
    );

BOOL SYM_EXPORT WINAPI NpsOptsConfirmDlgProc (
    HWND        hDlg,
    UINT        uMessage,
    WPARAM      wParam,
    LPARAM      lParam
    );

BOOL SYM_EXPORT WINAPI NpsOptsLookDlgProc (
    HWND        hDlg,
    UINT        uMessage,
    WPARAM      wParam,
    LPARAM      lParam
    );

VOID WINAPI NpsOptions (                // Show scheduler options
    HWND        hParent                 // [in] moma
    );

VOID WINAPI ShowHideTrayIcon (          // Initialize tray with out icon
    HWND        hDlg,                   // [in] parent window
    BOOL        bShow                   // [in] true to show icon
    );

BOOL WINAPI MainDlgTrayNotify (         // Handles tray notifications
    HWND        hDlg,                   // [IN] Dialog box handle
    WPARAM      wParam,                 // [IN] wParam of message
    LPARAM      lParam                  // [IN] lParam of message
    );

CONFSTATUS WINAPI AskConfirmation (     // Ask for confirmation
    HWND            hDlg,               // [in] moma window
    CONFIRM         Event,              // [in] CONFIRM_xxx
    LPNPSOPTIONS    lpOptions           // [in] pointer to options
    );

VOID WINAPI ShowHideButtonBar (         // Visualize button bar as required
    HWND    hWndButtonBar,              // [in] button bar to adjust
    BOOL    bShow,                      // [in] TRUE to show buttonbar
    UINT    uStyle,                     // [in] style to show it with
    BOOL    bInit
    );

VOID WINAPI ShowHideStatusBar (         // Visualize status bar as required
    HWND    hWndStatusBar,              // [in] status bar to adjust
    BOOL    bShowStatusBar              // [in] TRUE to show status bar
    );

WORD WINAPI NpsUpdateCaption (          // Updates the clock in caption
    HWND        hDlg                    // Handle to main dialog window
    );

VOID WINAPI NpsReturnCmdParams (        // Return command and its parameters
    LPCSTR  lpszFullCmdLine,            // [in] Full command line to parse
    LPSTR   lpszCmd,                    // [out] Buffer to receive command
    LPSTR   lpszParams                  // [out] Buffer to receive parameters
    );

WORD NpsMakeNpsBak (                    // Creates NpsBin.Bak file
    LPSTR       lpszBinFile,            // Name of Bin file
    LPSTR       lpszBinBakFile          // Name of Backup file
    );

WORD WINAPI NpsEventsPut(               /* Write events to NPS.BIN      */
    HWND  hDlg                          /* Handle to calling dlg box    */
    );

void CalculateNextRunTime ( NPSEVNT* pEvent );

#endif // #ifndef _NPS_STRUCTS_ONLY_

#endif
