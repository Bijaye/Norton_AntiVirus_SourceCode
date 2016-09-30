/************************************************************************/
/*                                                                      */
/* $Header:   S:/INCLUDE/VCS/NAVNTSCH.H_v   1.0   30 Jan 1996 15:47:14   BARRY  
/*                                                                      */
/* Description:                                                         */
/*      This is the main include file for NAVNTSCH.DLL .                */
/*                                                                      */
/************************************************************************/
/* $Log:   S:/INCLUDE/VCS/navntsch.h_v  $                               */
// 
//    Rev 1.1   24 Apr 1997 18:03:36   JBRENNA
// Tesla port: TSmith r1.1:
// Changed LEN_COMMANDBUFFER from 300 to 1024 to fix #80420.
// 
//    Rev 1.0   06 Feb 1997 20:51:08   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:47:14   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:11:36   BARRY
// Initial revision.
// 
//    Rev 1.7   14 Dec 1995 16:13:44   GWESTER
// Corrected some comments.
// 
//    Rev 1.6   14 Dec 1995 00:13:06   GWESTER
// Modified some comments about function parameters.
// 
//    Rev 1.5   13 Dec 1995 19:48:10   GWESTER
// Added new exported functions GetScheduledScanForOutsideWorld() and
// ScheduleScanForOutsideWorld(), which Norton apps (i.e., Navigator)
// can use to check and set scheduled scans.
// 
//    Rev 1.4   21 Nov 1995 17:02:30   GWESTER
// Added NetApiAccessTest to exports for Setup program.
// 
//    Rev 1.3   13 Nov 1995 16:01:28   GWESTER
// Added APIs for the Setup program to call while installing.
// 
// 
//    Rev 1.2   09 Nov 1995 19:49:06   GWESTER
// Added flags which the outside world can use.
// 
//    Rev 1.1   31 Oct 1995 21:12:34   GWESTER
// Added a function internal to the DLL. It will be moved to a header file
// NOT in the Luigi include area.
// 
//    Rev 1.0   25 Oct 1995 02:36:22   GWESTER
// Initial revision.
// 
/************************************************************************/

// --> SPLIT THIS INTO TWO HEADER FILES :
// One goes in the environment include area, and one goes in the project
// area. Why? Because outside modules shouldn't be #including this
// file, and gaining this much visibility inside NAVNTSCH.DLL .

#define LEN_COMMANDBUFFER      1024     // Max chars in the command passed
                                        // in from the base program.

#define DSIZE                   100     /* max size long date string */

#define ESIZE                   100     /* max size of error msgs */
#define BSIZE                   200
#define NDAY                    7
#define NAM                     2
#define SSIZE                   80      // Max size of constant strings
                                        //    months, days, and freqs.
EXTERN char      days[NDAY][SSIZE];     // Necessary ?
EXTERN char      ampm[NAM][SSIZE];
EXTERN BOOL      b24Hour;               /* TRUE if 24 Hour format       */
EXTERN char      szHourFmt[SSIZE];      /* format string for hours      */
EXTERN char      szTimeSep[SSIZE];      /* time separator               */

#define NPS_FORMAT_MIN   "%02d"
#define NPS_FORMAT_HR    "%2d"
#define NPS_FORMAT_HR0   "%02d"

EXTERN BOOL UseScheduler;
EXTERN WORD Scheduled_wMin;             // minutes                      
EXTERN WORD Scheduled_wHour;            // hour (24 hour fmt)            
EXTERN WORD Scheduled_wDay;             // day of the week              
#define SUN        0
#define MON        1
#define TUE        2
#define WED        3
#define THU        4
#define FRI        5
#define SAT        6


#define NTS_DEFAULT_SCANDAY     FRI     
#define NTS_DEFAULT_SCANHOUR    20     
#define NTS_DEFAULT_SCANMIN     0     
#define NTS_DEFAULT_SCANSEC     0     

#define NT_DATEMASK_MON 0x01
#define NT_DATEMASK_TUE 0x02
#define NT_DATEMASK_WED 0x04
#define NT_DATEMASK_THU 0x08
#define NT_DATEMASK_FRI 0x10
#define NT_DATEMASK_SAT 0x20
#define NT_DATEMASK_SUN 0x40

extern HINSTANCE hInternalInst;
extern HINSTANCE hAppInst;
extern HWND hParentWindow;
extern char CommandText [LEN_COMMANDBUFFER];
extern DWORD FlagsIn;

BOOL SYM_EXPORT CALLBACK NavNTSchedDlgProc
    (
    HWND        hDlg,                   // Handle to dialog box         
    UINT        wMessage,               // Message to process           
    WPARAM      wParam,                 // WPARAM data for message      
    LPARAM      lParam                  // LPARAM data for message      
    );

int SYM_EXPORT WINAPI NAVWNTSchedulerInterface
    (
    HINSTANCE hAppInstIn,               // Application Instance
    HWND hParentWindowIn,               // Parent Window
    LPCTSTR CommandTextIn,              // Command to be Scheduled
    DWORD FlagsIn                       // Flags 
    );


                                        // Flags passed to 
                                        // NAVWNTSchedulerInterface():

#define NTSCHED_CONVERTFROMANSI 0x01    // Convert the CommandTextIn string
                                        // from ANSI to Unicode 
#define NTSCHED_MAKETOPLEVEL    0x02    // Make the Scheduling dialog a
                                        // top-level window 


//NET_API_STATUS
DWORD
Encapsulated_NetScheduleJobEnum
    (
    LPWSTR Servername,	
    LPBYTE * PointerToBuffer,	
    DWORD PreferredMaximumLength,	
    LPDWORD EntriesRead,	
    LPDWORD TotalEntries,	
    LPDWORD ResumeHandle 	
    );



//----------------------------------------------------------------
// Interface for Setup's Scheduling of Scans:
//----------------------------------------------------------------

//Call this function during Install to check NT Schedule access:
//NET_API_STATUS is DWORD
DWORD SYM_EXPORT WINAPI NetAPIAccessTest (VOID);

// Return Value: 
//      NERR_Success ... NT Schedule is running, and the current user is an administrator.
//      ERROR_ACCESS_DENIED ... Current User is not an administrator
//      NERR_ServiceNotInstalled ... NT's Schedule service is not currently running
// Note that other return values are possible. (These mean failure for other reasons.)

//-------------------------------------

//Call this function while installing:

WORD SYM_EXPORT WINAPI ScheduleScanFromInstall
    (
    BOOL ScheduleScan,                 
    WORD DayOfWeek,
    WORD HourOfDay,
    WORD MinuteOfHour,
    LPCTSTR CommandText,
    DWORD Flags
    );

//Return Value: 
//ERR or NOERR. NOERR means everything worked.

//ScheduleScan: 
//TRUE if you want to schedule a scan.
//FALSE if you don't want any scans scheduled. 
//(If FALSE, you don't need to specify DayOfWeek, HourOfDay, and MinuteOfHour.)

//DayOfWeek:
//SUN, MON, TUE, WED, THU, FRI, or SAT. 
//(Don't need this if ScheduleScan is FALSE.)

//HourOfDay:
//0 through 23. 
//(Don't need this if ScheduleScan is FALSE.)

//MinuteOfHour:
//0 through 59.
//(Don't need this if ScheduleScan is FALSE.)

//CommandText:
//A pointer to a buffer of chars holding the command line string. 
//(Ex. ""c:\win32app\norton antivirus\n32Scanw" /L" -- The outer set
//of quotes denotes the string itself; inside that string, a set of quotes 
//should be wrapped around the full path to the EXE file name.)

//Flags:
//NTSCHED_CONVERTFROMANSI if the string you are passing in CommandText is an ANSI string.

//-------------------------------------

//Call this function while uninstalling:

WORD SYM_EXPORT WINAPI PurgeScheduledScans
    (
    LPCTSTR CommandText,
    DWORD Flags
    );

//Return Value: 
//ERR or NOERR. NOERR means everything worked.

//CommandText and Flags:
//Exactly as ScheduleScanFromInstall().

//-------------------------------------

BOOL SYM_EXPORT WINAPI SchedulerIsRunning ( VOID );

//Return Value: 
//TRUE or FALSE. FALSE means the Scheduler is not running right now.

//----------------------------------------------------------------



//----------------------------------------------------------------
// Interface for Scheduling Scans from the Outside World:
//----------------------------------------------------------------

WORD SYM_EXPORT WINAPI GetScheduledScanForOutsideWorld
    (
    LPBOOL ScanIsScheduled,
    LPWORD DayOfWeek,
    LPWORD HourOfDay,
    LPWORD MinuteOfHour,
    LPCTSTR CommandTextIn,
    DWORD Flags
    );

//Return Value: 
//ERR or NOERR. NOERR means that valid data was returned in the parameters.

//ScanIsScheduled: 
//Points to a BOOL that will be set to TRUE if a scan is currently scheduled.
//FALSE means that no scan is currently scheduled. (If FALSE, DayOfWeek, 
//HourOfDay, and MinuteOfHour are not set.)

//DayOfWeek:
//Points to a WORD that is set to the day of week the scan is scheduled on;
//SUN, MON, TUE, WED, THU, FRI, or SAT (defined above). 

//HourOfDay:
//Points to a WORD that is set to the hour of the scheduled scan, 0 through 23. 

//MinuteOfHour:
//Points to a WORD that is set to the minute of the scheduled scan, 0 through 59.

//CommandText:
//A pointer to a buffer of chars holding the command line string that 
//launches the scan.
//(Ex. ""c:\win32app\norton antivirus\n32Scanw" /L" -- The outer set
//of quotes denotes the string itself; inside that string, a set of quotes 
//should be wrapped around the full path to the EXE file name.)

//Flags:
//NTSCHED_CONVERTFROMANSI if the string you are passing in CommandText is an ANSI string.


//-------------------------------------

WORD SYM_EXPORT WINAPI ScheduleScanForOutsideWorld
    (
    BOOL ScheduleScan,
    WORD DayOfWeek,
    WORD HourOfDay,
    WORD MinuteOfHour,
    LPCTSTR CommandTextIn,
    DWORD Flags
    );

// Return Value and Parameters are identical to ScheduleScanFromInstall(),
// described above.

//----------------------------------------------------------------

