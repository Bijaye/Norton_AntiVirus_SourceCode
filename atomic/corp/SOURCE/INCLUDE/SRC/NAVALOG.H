#ifndef _NAV_ACTIVITY_LOG_
#define _NAV_ACTIVITY_LOG_

#if 0

#include "navopts.h"                    // Required to LPACTOPT

// **************************************************************** //
// ****            Possible events to log within NAV           **** //
// ****  DO NOT CHANGE THE NUMBERING OF THESE DEFINES !!!!     **** //
// **************************************************************** //

#define LOG_CATEGORY_VIRUSACTIVITY              ((WORD)1)
#define LOG_CATEGORY_VIRLISTCHANGE              ((WORD)2)
#define LOG_CATEGORY_INOCHANGE                  ((WORD)3)
#define LOG_CATEGORY_KNOWNVIRUS                 ((WORD)4)
#define LOG_CATEGORY_UNKNOWNVIRUS               ((WORD)5)
#define LOG_CATEGORY_SCANEND                    ((WORD)6)

#define LOGEVENT_ATTRCHANGE                     ((WORD)0)
#define LOGEVENT_WRITEMBR                       ((WORD)1)
#define LOGEVENT_WRITEBRFLOPPY                  ((WORD)2)
#define LOGEVENT_WRITEPROTFILE                  ((WORD)3)

#define LOGEVENT_FILEINOC_NEW                   ((WORD)0)
#define LOGEVENT_FILEINOC_CHANGE                ((WORD)1)
#define LOGEVENT_UNINOCULATE                    ((WORD)2)
#define LOGEVENT_INTEG_CREATE                   ((WORD)3)
#define LOGEVENT_INTEG_MBCHANGED                ((WORD)4)
#define LOGEVENT_INTEG_BOOTCHANGED              ((WORD)5)
#define LOGEVENT_INTEG_SYSFILECHANGED           ((WORD)6)

#define LOGEVENT_FILEINFECTED                   ((WORD)0)
#define LOGEVENT_BOOTINFECTED                   ((WORD)1)
#define LOGEVENT_MEMORYINFECTED                 ((WORD)2)
#define LOGEVENT_MASTERBOOTINFECTED             ((WORD)3)

#define LOGEVENT_SCANEND                        ((WORD)1)
#define LOGEVENT_SCANINTERRUPTED                ((WORD)2)
#define LOGEVENT_VIRLISTCHANGED                 ((WORD)3)

#define LOGEVENT_LASTID                         LOGEVENT_VIRLISTCHANGED


#define LOGACTION_UNKNOWNVIRUS_REPAIRED         ((WORD)0)
#define LOGACTION_UNKNOWNVIRUS_REPAIR_ERROR     ((WORD)1)
#define LOGACTION_UNKNOWNVIRUS_DELETED          ((WORD)2)
#define LOGACTION_UNKNOWNVIRUS_DELETE_ERROR     ((WORD)3)
#define LOGACTION_UNKNOWNVIRUS_EXCLUDED         ((WORD)4)
#define LOGACTION_UNKNOWNVIRUS_NOTEXCLUDED      ((WORD)5)
#define LOGACTION_UNKNOWNVIRUS_ACCESSDENIED     ((WORD)6)
#define LOGACTION_UNKNOWNVIRUS_CPUHALTED        ((WORD)7)
#define LOGACTION_UNKNOWNVIRUS_NOACTION         ((WORD)8)

#define LOGACTION_KNOWNVIRUS_REPAIRED           ((WORD)0)
#define LOGACTION_KNOWNVIRUS_REPAIR_ERROR       ((WORD)1)
#define LOGACTION_KNOWNVIRUS_DELETED            ((WORD)2)
#define LOGACTION_KNOWNVIRUS_DELETE_ERROR       ((WORD)3)
#define LOGACTION_KNOWNVIRUS_EXCLUDED           ((WORD)4)
#define LOGACTION_KNOWNVIRUS_NOTEXCLUDED        ((WORD)5)
#define LOGACTION_KNOWNVIRUS_CPUHALTED          ((WORD)6)
#define LOGACTION_KNOWNVIRUS_NOACTION           ((WORD)7)

#define LOGACTION_INOC_OK                       ((WORD)0)
#define LOGACTION_INOC_ERROR                    ((WORD)1)
#define LOGACTION_INOC_REPAIRED                 ((WORD)2)
#define LOGACTION_INOC_REPAIR_ERROR             ((WORD)3)
#define LOGACTION_INOC_DELETED                  ((WORD)4)
#define LOGACTION_INOC_DELETE_ERROR             ((WORD)5)
#define LOGACTION_INOC_EXCLUDED                 ((WORD)6)
#define LOGACTION_INOC_NOTEXCLUDED              ((WORD)7)
#define LOGACTION_INOC_NOACTION                 ((WORD)8)

#define LOGACTION_INOC_INTEG_CREATE_OK          ((WORD)0)
#define LOGACTION_INOC_INTEG_CREATE_ERROR       ((WORD)1)

#define LOGACTION_VIRACT_NOTALLOWED             ((WORD)0)
#define LOGACTION_VIRACT_ALLOWED                ((WORD)1)
#define LOGACTION_VIRACT_EXCLUDED               ((WORD)2)
#define LOGACTION_VIRACT_NOTEXCLUDED            ((WORD)3)

                                        // ------------------------------
                                        // This is the format of the
                                        // callback function that is
                                        // being notified about the
                                        // progress of the log file build
                                        // ------------------------------
typedef BOOL (WINAPI *LOGCALLBACK)
   (UINT uNumProcessed,
    UINT uNumToProcess,
    DWORD dwUserData);

// **************************************************************** //
// ****            Possible events to log within NAV           **** //
// **************************************************************** //

enum {
     LOGEV_HALTCPU = 1,
     LOGEV_SCANEND,
     LOGEV_SCANINTERRUPTED,
     LOGEV_VIRLISTCHANGE,
     LOGEV_INOCULATED_NEW,
     LOGEV_INOCULATEFAILED_NEW,
     LOGEV_INOCULATED_CHANGED,
     LOGEV_INOCULATEFAILED_CHANGED,
     LOGEV_UNINOCULATED,
     LOGEV_UNINOCULATEFAILED,
     LOGEV_INTEG_CREATEFAILED,
     LOGEV_INTEG_REINOCULATED,
     LOGEV_INTEG_REINOCULATEFAILED,
     LOGEV_INTEG_MBREPAIRED,
     LOGEV_INTEG_MBREPAIRFAILED,
     LOGEV_INTEG_BOOTREPAIRED,
     LOGEV_INTEG_BOOTREPAIRFAILED,
     LOGEV_INTEG_SYSFILEREPAIRED,
     LOGEV_INTEG_SYSFILEREPAIRFAILED,
     LOGEV_INTEG_MBCHANGED,
     LOGEV_INTEG_BOOTCHANGED,
     LOGEV_INTEG_SYSFILECHANGED,
     LOGEV_INTEG_NOTEXIST,
     LOGEV_QUARANTINED
#ifdef BANKERS
     ,LOGEV_CONFIGCHANGE
#endif  // BANKERS
	 ,LOGEV_SKIPDRIVE
     };

// ********************************************************* //
// structure required to log events to the activity log file //
// ********************************************************* //


typedef struct tagLOGSTRUCT
{
   UINT        uEvent;        // Type of event
   LPSTR       lpFileName;    // the name of the file in question
   LPSTR       lpVirusName;   // and the virus name, if applicable
   LPACTOPT    lpAct;         // Activity log options
} LOGSTRUCT, FAR * LPLOGSTRUCT;

                                        // -----------------------------------
                                        // Date Logic Defines. Do not change
                                        // numbering.
                                        // -----------------------------------
typedef enum tagDATELOGIC
{
    ON = 0,
    NOT_ON,
    BEFORE,
    BEFORE_OR_ON,
    AFTER,
    AFTER_OR_ON,
    BETWEEN,
    NOT_BETWEEN,
    NO_LOGIC
} DATELOGIC;

typedef struct tagCHECKDATE
{
    UINT        uDateToCheck;
    DATELOGIC   DateLogic;
    UINT        uDateOne;
    UINT        uDateTwo;
} CHECKDATE, FAR * LPCHECKDATE;

                                        // -----------------------------------
                                        // These defines are used to
                                        // specify record types to show
                                        // -----------------------------------

#define LOGFILTER_KNOWNVIRUS    0x0001
#define LOGFILTER_UNKNOWNVIRUS  0x0002
#define LOGFILTER_INOCACTIVITY  0x0004
#define LOGFILTER_VIRUSACTIVITY 0x0008
#define LOGFILTER_SCANEND       0x0010
#define LOGFILTER_VIRLISTCHANGE 0x0020

typedef LPVOID HLOGFILE;                // handle to log file
typedef HLOGFILE FAR * LPHLOGFILE;      // pointer to handle to log file

                                        // -----------------------------------
                                        // structure required to log events to
                                        // the activity log file
                                        // -----------------------------------
typedef struct tagADDLOG
{
    WORD        wCategory;              // Type of category
    WORD        wEvent;                 // Type of event
    WORD        wStatus;                // Type of action taken
    LPSTR       lpFileName;             // the name of the file in question
    LPSTR       lpVirusName;            // and the virus name, if applicable
} ADDLOG, FAR * LPADDLOG;


typedef BOOL (WINAPI FAR * LPLOGFUNC )  // Activity log callback function
    ( DWORD     dwIndex,                // [in] entry that is being processed
      DWORD     dwCount,                // [in] out of this many entries
      DWORD     dwUser );               // [in] user defined DWORD

typedef BOOL (WINAPI FAR * LPLOGCUSTOMREC) // Custom log callback function
    ( LPDWORD   lpdwMaxNumRec,          // [out] maximum number of record to add
      LPSTR     lpMem,                  // [in] record should be added here
      WORD      wMemSize,               // [in] size of lpMem
      DWORD     dwUser );               // [in] user defined DWORD

                                        // -----------------------------------
                                        // Used to translate binary log file
                                        // to ASCII text file.
                                        // -----------------------------------
typedef struct tagLOGCREATETEXT
{
    DWORD       dwNumRecs;              // [out] Number of records processed
    LPSTR       lpszTempFile;           // [out] Temporary file name of log
    LPSTR       lpszLogFile;            // [in] Binary log file name
    WORD        wFilterFlags;           // [in] Flags to filter log file
    DATELOGIC   DateLogic;              // [in] Date logic to filter log file
    UINT        uStartDate;             // [in] Encoded date
    UINT        uEndDate;               // [in] Encoded date
    LPLOGFUNC   lpfn;                   // [in] Callback function address
    DWORD       dwUser;                 // [in] passed to callback function.

} LOGCREATETEXT, FAR * LPLOGCREATETEXT;

#ifdef SYM_WIN
                                        // -----------------------------------
                                        // Application should initialize this
                                        // structure and pass it along to
                                        // LogShowFile().
                                        // -----------------------------------
typedef struct tagLOGSHOWSTRUCT
{
    HWND            hParent;            // Parent window handle
    BOOL            bAllowClear;        // Allow user to clear the log file
    BOOL            bAllowFilter;       // Allow user to filter the log file
    BOOL            bShowPencil;        // Show a pencil YES/NO.
    UINT            uHelpID;            // Help ID
    UINT            uFilterHelpID;      // Filter Help ID
    WORD            wFilterFlags;       // See LOGFILTER_xxx falgs
    char            szTitle   [ SYM_MAX_PATH ];
    LPLOGCUSTOMREC  lpfnCustomRec;      // Pointer to custom rec func if any
    DWORD           dwCustomCookie;     // cookie to custom rec func.
    char            szLogFile [ SYM_MAX_PATH ];

    UINT            uHelpRightClickClear;
    UINT            uHelpRightClickFilter;
    UINT            uHelpRightClickPrint;
    UINT            uHelpRightClickClose;
    UINT            uHelpRightClickHelp;
    UINT            uHelpRightClickDetails;

    LPDWORD         lpdwHelpHowToArray; // How to right click help array ptr.
    LPDWORD         lpdwHelpHowToPrintArray;

} LOGSHOWSTRUCT, FAR * LPLOGSHOWSTRUCT;

#endif // #ifdef SYM_WIN

// --------------------------------------------------------------------------
// POSSIBLE RETURN VALUES FROM ACTIVITY LOG FUNCTIONS
// --------------------------------------------------------------------------

typedef WORD LOGERR;

#define LOGERR_OK           ((WORD)0)   // Okay
#define LOGERR_MEMALLOC     ((WORD)-1)  // Memory allocation error
#define LOGERR_CLOSE        ((WORD)-2)  // file close error
#define LOGERR_OPEN         ((WORD)-3)  // file open error
#define LOGERR_CREATE       ((WORD)-4)  // file create error
#define LOGERR_READ         ((WORD)-5)  // file read error
#define LOGERR_WRITE        ((WORD)-6)  // file write error
#define LOGERR_SIZE         ((WORD)-7)  // file resizing error
#define LOGERR_HANDLE       ((WORD)-8)  // invalid handle
#define LOGERR_LESSRECORDS  ((WORD)-9)  // less records written
#define LOGERR_PARTIALDISP  ((WORD)-10) // the entire file couldn't be shown
#define LOGERR_INTERNAL     ((WORD)-99) // internal error.

#ifdef SYM_DOS
    #ifdef __cplusplus
        extern "C" {
    #endif
#endif

// --------------------------------------------------------------------------
// Non exported function that must be in this for the DOS platform to pick
// it up.
// --------------------------------------------------------------------------

LOGERR WINAPI LogCreateTextFile (       // Create a text file outa binary
    LPLOGCREATETEXT lpLogCreate );      // [in] create structure pointer.

LOGERR WINAPI LogCreateCustomTextFile ( // Create a custom text file.
    LPLOGCREATETEXT lpLogCreate,        // [in] create structure pointer.
    LPLOGCUSTOMREC  lpfnCustomRec,      // [in] custom rec build pointer.
    DWORD           dwCustomCookie );   // [in] cookie to custom fucntion.

// --------------------------------------------------------------------------
//                          EXPORTED FUNCTIONS
// --------------------------------------------------------------------------

LOGERR SYM_EXPORT WINAPI LogCreate (    // Create a log file
    LPSTR lpszLogFile,                  // [in] log file name to open/create
    DWORD dwMaxNumRecs,                 // [in] maximum number of records
    LPHLOGFILE lphLogFile );            // [out] handle to this instance

LOGERR SYM_EXPORT WINAPI LogOpen (      // Open a log file
    LPSTR lpszLogFile,                  // [in] log file name to open/create
    LPHLOGFILE );                       // [out] handle to this instance

LOGERR SYM_EXPORT WINAPI LogClose (     // Close
    HLOGFILE );                         // [in] this file

LOGERR SYM_EXPORT WINAPI LogGetMaxRecords (  // Get max number of records
    HLOGFILE,                           // [in] in this file
    LPDWORD lpdwCount );                // [out] place it here

LOGERR SYM_EXPORT WINAPI LogSetMaxRecords (  // Set max number of records
    HLOGFILE,                           // [in] in this file
    DWORD dwCount );                    // [in] to this number of recs

LOGERR SYM_EXPORT WINAPI LogGetMaxKilobytes (  // Get max number of kilobytes
    HLOGFILE,                           // [in] in this file
    LPDWORD lpdwCount );                // [out] place it here

LOGERR SYM_EXPORT WINAPI LogSetMaxKilobytes (  // Set max number of kilobytes
    HLOGFILE,                           // [in] in this file
    DWORD dwCount );                    // [in] to this number of recs

LOGERR SYM_EXPORT WINAPI LogAddRecord ( // Add a new activity log record
    HLOGFILE,                           // [in] to this file
    LPADDLOG );                         // [in] information for the entry.

LOGERR SYM_EXPORT WINAPI LogBlockWriteStart( // Start a block write operation
    HLOGFILE,                           // [in] for this log file
    DWORD dwEntries );                  // [in] number of entries to write

LOGERR SYM_EXPORT WINAPI LogBlockWriteEnd( // End block write to log file
    HLOGFILE );                         // [in] log file handle

BOOL SYM_EXPORT WINAPI LogCheckDate (   // Check date according to
    LPCHECKDATE );                      // [in] this structure


VOID SYM_EXPORT WINAPI BuildLogFilename (
    LPSTR   lpLogFilename,
    LPCSTR  lpPartialFilename );

BOOL SYM_EXPORT WINAPI WriteAllFileLogRecords (
    LPACTOPT    lpAct,                  // [in] Activity log options
    LPVOID      lpvProblems,            // [in] infected array pointer
    LOGCALLBACK lpfnCallback,           // [in] callback function
    DWORD       dwUserData );           // [in] user data to callback

UINT SYM_EXPORT WINAPI LogEvent (
    LPLOGSTRUCT lpLog );

#ifdef SYM_DOS
    #ifdef __cplusplus
        }
    #endif
#endif

#ifdef SYM_WIN

LOGERR SYM_EXPORT WINAPI LogShowFile (  // Visualize the log file
    LPLOGSHOWSTRUCT );                  // [in] show structure

#endif
#endif

#endif _NAV_ACTIVITY_LOG_

