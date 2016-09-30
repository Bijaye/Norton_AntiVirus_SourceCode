// Copyright 1993 Symantec, Peter Norton Group
//
// $Header:   S:/INCLUDE/VCS/symalog.h_v   1.44   07 Apr 1998 18:35:36   tcashin  $
//
// $Log:   S:/INCLUDE/VCS/symalog.h_v  $
// 
//    Rev 1.44   07 Apr 1998 18:35:36   tcashin
// Added a new field type for quarantine.
// 
//    Rev 1.43   23 Feb 1998 13:27:12   CEATON
// Exported local functions FilterLog() and KeyWord() for READLOG project.
// 
// 
//    Rev 1.42   27 Aug 1997 12:41:44   DDREW
// Added repair
// 
//    Rev 1.41   02 May 1997 14:23:02   TIVANOV
// new filed type for the quarantine - ALOG_FI_MOVE
//
//    Rev 1.40   09 Nov 1995 13:16:34   GDZIECI
// Roll over Keith's changes from QUAKE 6.
//
//    Rev 1.39   28 Dec 1994 10:40:50   MARKL
// Added ALogAddEx() for adding log entries where the date/time cannot
// be the current date/time and must be passed in as parameters.
//
//    Rev 1.38   20 Dec 1994 17:11:20   MARKL
// Fixed where SYMALOG was writing UINTs to the disk.  This doesn't work where
// 16 and 32 bit components have to share the same log file and be able to
// read/write each other's data.  These UINTs were changed to type WORD.
//
//    Rev 1.37   16 Dec 1994 13:46:44   MARKL
// Added ALOG_DIR_NA for when direction is not applicable.
//
//    Rev 1.36   16 Dec 1994 13:36:48   MARKL
// Added ALOG_PT_WINVXD for alerts from the Windows Auto-Protect VxD.
//
//    Rev 1.35   07 Dec 1994 11:05:20   MFALLEN
// Commented out AlogPrint() function that is useless. NPT project should be used.
//
//    Rev 1.34   07 Dec 1994 09:49:04   MFALLEN
// Moved over the latest and greates super duper activity log from the
// Mario product area.
//
//    Rev 1.4   27 Sep 1994 14:56:38   KEITH
// Removed NLMPRINTDLGSTRUCT and the like - use PRINTDLGSTRUCT only.
//
//    Rev 1.3   26 Sep 1994 17:37:34   KEITH
// Use Martin's new PRINTDLGSTRUCT to fix related problems
//
//    Rev 1.2   18 Aug 1994 11:00:38   KEITH
// Changed PRINTDLGPROC to NLMPRINTDLGPROC to avoid a conflict (too)
//
//    Rev 1.1   18 Aug 1994 10:50:42   KEITH
// Changed PRINTDLGSTRUCT to NLMPRINTDLGSTRUCT to avoid conflict with
// NPT.H PRINTDLGSTRUCT. This was first, but we will most likely be
// moving to the newer routines in the near future.
//
//    Rev 1.0   17 Aug 1994 15:30:10   JMILLARD
// Baseline for mario from quake
//
//    Rev 1.33   26 Apr 1994 16:39:42   BRAD
// Removed PVCS Logs

#if !defined(_SYMALOG_H)
#define _SYMALOG_H

/**********************************************************************
** Temporarily for compatibility
**********************************************************************/
#define symALogInit     ALogStart
#define symALogTerm     ALogDone
#define symALogAdd      ALogAdd
#define symALogAddEx    ALogAddEx
#define LogGetMaxSize   ALogGetMaxSize
#define LogSetMaxSize   ALogSetMaxSize
#define symALogBuildFile ALogBuildFile

/**********************************************************************
** - Error codes
**********************************************************************/
#define ALOGERR_NOERR         0         /* Use internal to SYMALOG.C */
#define ALOGERR_OPENLOG      -1         /* Log file cannot be opened */
#define ALOGERR_DISPFILE     -2         /* Cant open temp disply file*/
#define ALOGERR_MEMORY       -3         /* Cant open temp disply file*/
#define ALOGERR_BADLOGFILE   -4         /* The log file is trash     */
#define ALOGERR_CLOSELOG     -5         /* Error while closing log */

/**********************************************************************
*   Return codes from Bill Lee's OverwriteOrAppendDlg()
**********************************************************************/

#define OVERWRITE_FILE                  0
#define APPEND_FILE                     1
#define OVERWRITE_APPEND_CANCEL         2

/**********************************************************************
** - Typedefs, etc.
**********************************************************************/
typedef short ALOGFIELD;

/** ALogStart() *******************************************************
**
** This routine initializes the component.  A ALOGINIT structure of
** "application global" info is passed.  The number of types is the
** number of record types that will be screened by the filter.
** lpFormats is an array (nTypes long) of structures describing the
** types.  Each element of the structure contains the record type and
** its associated format string ID (this string ID is referenced from
** the instance of the caller).
**
** Each template can contain values starting with '#' along with
** constant text.  When generating a displayable log file, the '#'
** delimited values are replaced by corresponding record field content.
** Any other text is output as is (including \r\n).
**
** See ALOG_FT_??? field types in SYMALOGI.H for, possibly, more
** understanding of these values.
**
**     #A   Action string
**  *  #a   Date of log record
**  *  #e   description/extra
**     #r   File movement direction
**     #P   Full file path
**     #M   Information text
**     #N   Network address
**     #S   Operating system
**     #p   Program type
**     #T   Record type string
**     #s   Server
**     #B   Size of file in bytes
**  *  #t   Time of log entry
**  *  #u   User name
**     #v   Virus name
**
**  * Consistent with TSR. Others used by TSR and not used here: #d-
**    drive letter and colon; #D-directory name (w/trailing backslash);
**    #f-filename; #P-executing program name... yikes I missread its use!
**
**********************************************************************/
typedef LPVOID HALOG;                   /* Private pointer           */

#if defined(SYM_WIN) || defined(WIN31)
    typedef WORD    STRINGREF;          /* For Windows use string    */
#else                                   /*    table IDs              */
    typedef LPSTR   STRINGREF;          /* For DOS, use actual       */
#endif                                  /*    strings                */

typedef struct {                        /* Output templates:         */
   WORD      TypeCode;                  /* Type associated w/template*/
   STRINGREF SID;                       /* String ID of template     */
} ALOGFORMAT, FAR *LPALOGFORMAT;

typedef struct {
   WORD         SSize;                  /* Size of this struct       */
   int          nTypes;                 /* Number of output formats  */
   LPALOGFORMAT lpFormats;              /* Output format templates   */
} ALOGINIT, FAR *LPALOGINIT;

//&? DALLEE - commented out LPDOMAIN Domain.  Is this being used?

typedef struct tagALOG {
   ALOGINIT Init;                       // Initialization parms
   HINSTANCE hInst;                     // Instance of caller
//   LPDOMAIN  Domain;                    //
} ALOG, FAR *LPALOG;

/**********************************************************************
** Log manipulation routines
**********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

EXTERNC HALOG SYM_EXPORT WINAPI ALogStart(  /* Component initialization  */
   HINSTANCE hCaller,                   /* Instance of caller        */
   LPALOGINIT InitStruct);              /* Initialization parameters */

UINT SYM_EXPORT WINAPI ALogDone(LPVOID);    /* Component termination     */

EXTERNC DWORD SYM_EXPORT WINAPI ALogGetMaxSize(
    WORD    wHand);                     /* file handle               */

EXTERNC VOID SYM_EXPORT WINAPI ALogSetMaxSize(
    WORD    wHand,                      /* file handle               */
    DWORD   dwMaxSize);                 /* new max size              */

VOID SYM_EXPORT WINAPI ALogClear(           /* Name of log file          */
    WORD    wHand,                      /* Handle or NULL            */
    LPSTR   szLogFile);                 /* Name of file if wHand == 0*/

typedef struct {
    LPSTR       lpszFile;
    WORD        wHelpID;
} OVERWRITESTRUCT, FAR *LPOVERWRITESTRUCT;

UINT SYM_EXPORT WINAPI OverwriteOrAppendDlg(
    HWND    hDlg,                       /* Parent window handle      */
    LPOVERWRITESTRUCT lpOverwriteStruct);

#ifdef __cplusplus
}
#endif

/** ALogAdd() *********************************************************
** Add a record to the activity log.
**********************************************************************/
                                        /* Direction Field Types     */
#define ALOG_DIR_NA  0                  /* Not Applicable            */
#define ALOG_DIR_IN  1                  /* Occurred during incoming  */
#define ALOG_DIR_OUT 2                  /* Occurred during outgoing  */

                                        /* Information Field Types:  */
#define ALOG_FI_CONFIGCHANGE  'C'
#define ALOG_FI_VIRLISTCHANGE 'D'
#define ALOG_FI_ERROR         'E'
#define ALOG_FI_INOCHANGE     'I'
#define ALOG_FI_KNOWNVIRUS    'K'
#define ALOG_FI_LOADUNLOAD    'L'
#define ALOG_FI_MOVE          'M'
#define ALOG_FI_REPAIR        'P'
#define ALOG_FI_QUARANTINE    'Q'
#define ALOG_FI_REGCHANGE     'R'
#define ALOG_FI_STARTEND      'S'
#define ALOG_FI_STATUS        'T'
#define ALOG_FI_UNKNOWNVIRUS  'U'
#define ALOG_FI_VIRUSLIKE     'V'
#define ALOG_FI_FROMWRK       'W'

                                        /* Program types:            */
#define ALOG_PT_DOS           'D'       /* Dos                       */
#define ALOG_PT_MACINTOSH     'M'       /* Macintosh                 */
#define ALOG_PT_NLM           'N'       /* Netware                   */
#define ALOG_PT_TSR           'T'       /* TSR                       */
#define ALOG_PT_WINDOWS       'W'       /* Windows                   */
#define ALOG_PT_WINTSR        'w'       /* Windows TSR               */
#define ALOG_PT_WINVXD        'v'       /* Windows VxD               */

typedef struct {                        /* symALogAdd structure:     */
   WORD     SSize;                      /* Size of this struct       */
   LPSTR    lpAction;                   /* Action taken on this event*/
   LPSTR    lpMessage;                  /* Message                   */
   LPSTR    lpNetAddr;                  /* Network address           */
   LPSTR    lpPath;                     /* Path to a file            */
   LPSTR    lpServer;                   /* Name of server            */
   LPSTR    lpUser;                     /* Name of user              */
   LPSTR    lpVirus;                    /* Virus name                */
   LPSTR    lpOS;                       /* Operating system used     */
   WORD     wDirection;                 /* File movement direction   */
                                        /*   ALOG_DIR_IN|ALOG_DIR_OUT*/
} ALOGADD, FAR *LPALOGADD;


#ifdef __cplusplus
extern "C" {
#endif

EXTERNC WORD SYM_EXPORT WINAPI ALogAdd(     /* Add Activity log item     */
   LPSTR    lpLogFile,                  /* Path to the log file      */
   BYTE     byRecType,                  /* one of the ALOG_FI_xxx    */
   BYTE     byProgType,                 /* one of ALOG_PT_xxx        */
   DWORD    uFileSize,                  /* Size of file (or 0)       */
   LPALOGADD lpAddLog,                  /* Log Information           */
   DWORD    dwMaxSize);                 /* max size of log file      */

EXTERNC WORD SYM_EXPORT WINAPI ALogAddEx(  /* Add Activity log item     */
   LPSTR    lpLogFile,                  /* Path to the log file      */
   BYTE     byRecType,                  /* one of the ALOG_FI_xxx    */
   BYTE     byProgType,                 /* one of ALOG_PT_xxx        */
   DWORD    uFileSize,                  /* Size of file (or 0)       */
   LPALOGADD lpAddLog,                  /* Log Information           */
   DWORD    dwMaxSize,                  /* max size of log file      */
   DWORD    dwEncodedDate,              /* date to write to log      */
   DWORD    dwEncodedTime);             /* time to write to log      */

#ifdef __cplusplus
}
#endif

/** ALogBuildFile() ***************************************************
**
** This call creates a temporary file that contains the formatted,
** filtered records from the specifed activity logs.  An array of
** pointers to log files are passed.  An array of pointers to
** descriptions for each file are also passed.  These descriptions may
** be incorporated into the output based on the '#e' variable usage in
** the formatting strings passed to ALogStart().
**
** A structure containing filter information is passed.  This structure
** contains the structure passed to the ALogAdd function so that those
** same fields may be used to filter the output records.  Note that an
** array of booleans is passed (why isnt this a bit bit vector?!),
** which specifies which corresponding record types are to be included
** in the output.  The order of the values in this array correspond to
** the order of ALOGFORMAT array passed to the ALogStart function.
**
**********************************************************************/

                                        // ------------------------------
                                        // This is the format of the
                                        // callback function that is
                                        // being notified about the
                                        // progress of the log file build
                                        // ------------------------------

typedef BOOL (WINAPI *LOGCALLBACK)
   (WORD wNumProcessed,
    WORD wNumToProcess,
    DWORD dwUserData);


                                        /* Datelogic bit masks       */
#define ALOG_LT_START   0x0001          /*   Earlier than StartDate  */
#define ALOG_EQ_START   000002          /*   On StartDate            */
#define ALOG_GT_START   0x0004          /*   After StartDate         */
#define ALOG_LT_END     0x0010          /*   Earlier than EndDate    */
#define ALOG_EQ_END     0x0020          /*   On EndDate              */
#define ALOG_GT_END     0x0040          /*   After EndDate           */
#define ALOG_BETWEEN    0x0114          /*   Between Start and End   */
                 /*        ^^^                  (exclusively)        */
                 /* special SYM_EXPORT  SYM_EXPORT  SYM_EXPORT StartDate                              */
                 /*         EndDate                                  */
typedef struct {                        /* Structure for filtering   */
   WORD    SSize;                       /* Size of this struct       */
                                        /*    Corresponds to ALogStar*/
                                        /*    types in format array. */
   ALOGADD FldStruct;                   /* Character field searching */
   BOOL FAR *pTypeIncl;                 /* Types to include in filter*/
   WORD    StartDate;                   /* First record to include   */
   WORD    EndDate;                     /* Last record to include    */
   WORD    DateLogic;                   /* How to apply date values  */
} ALOGFILTER, FAR *LPALOGFILTER;

EXTERNC int SYM_EXPORT WINAPI ALogBuildFile( /* Prepare display log file */
   HALOG hALog,                         /* In: Dummy project handle  */
   WORD nLogs,                          /* In: Number of log files   */
   LPSTR FAR *szLogName,                /* In: Array of file names   */
   LPSTR FAR *szDescriptions,           /* In: Array of file descrip */
   LPALOGFILTER pFilter,                /* In: Filters conditions    */
   LPSTR szFile,                        /* I/O: Temp file name       */
   DWORD dwMaxSize,                     /* max size of file if create*/
   LOGCALLBACK  lpfn,                   /* callback function to notfy*/
   DWORD        dwUserData);

#if !defined(SYM_NLM)

/** ALogPrint() *******************************************************
** This function prints the specified file.  If the pointer to the file
** name is NULL or points to a zero length string, no file is printed;
** the return value indicates the user's choice.  If the pointer is not
** NULL and the user selects "print to file," then the filename is
** returned in the buffer.  This buffer should be at least
** MAX_NET_PATH_SIZE+1 bytes in length.
**********************************************************************/
                                        /* Return from ALogPrint()   */
#define ALOG_PR_PRINTER   1             /* File printed              */
#define ALOG_PR_FILE      2             /* File moved to file        */
#define ALOG_PR_USER      3             /* Mail to another user      */
#define ALOG_PR_CANCEL    4             /* Dialog canceled by user   */
#define ALOG_PR_ERROR     5             /* Unexpected error          */

#if 0
// Removed - use PRINTDLGSTRUCT
struct tagNLMPRINTDLGSTRUCT;            // incomplete definition so I can
                                        // typedef LPNLMPRINTDLGSTRUCT. This is
                                        // needed since the struct contains
                                        // a reference to a pointer to this
                                        // struct. Evidently C7 lets you
                                        // get away with this.
typedef struct tagNLMPRINTDLGSTRUCT FAR *LPNLMPRINTDLGSTRUCT;

                                        // Prototype for the
                                        // NPTNLMPRINTDLGPROC callback
                                        // routine.
typedef HGLOBAL (CALLBACK *NLMPRINTDLGPROC)(LPNLMPRINTDLGSTRUCT);


typedef struct tagNLMPRINTDLGSTRUCT
    {
    DWORD        dwSize;                // Size of this structure
    HWND         hWnd;                  // Window handle of current dialog
    LPSTR        lpszTitle;             // Title of Print Dlg
    LPSTR        lpszFile;              // Default file to print to
    NLMPRINTDLGPROC lpfnBuild;          // Function to build the print info
    LPARAM       lParam;                // Any data you want
    WORD         wHelpID;               // Help ID to use.
    WORD         wHelpIDOverwrite;      // Help ID to use for Overwrite dlg
    WORD         wHelpIDBrowse;         // Help ID to use for Print Browse dlg
    BOOL         bWordWrap;             // TRUE if word wrap print out
    } NLMPRINTDLGSTRUCT;

#ifdef __cplusplus
extern "C" {
#endif

    /*
        These functions are replaced by the Quake NPT project.
        Please use that instead.
    */

#define NptPrintDlg(a)  ALogPrint(a)

EXTERNC int SYM_EXPORT WINAPI ALogPrint(    /* Dlg to print a file       */
   LPPRINTDLGSTRUCT     lpPrintDlgStruct);

#ifdef __cplusplus
}
#endif

#endif // #if 0

#ifdef __cplusplus
extern "C" {
#endif

EXTERNC UINT SYM_EXPORT WINAPI KeyWord(                           // Perform "variable" substitution
   LPSTR lpRecord,                      // In: Raw log record
   char  lpKey,                         // In: Key name to check
   WORD  wRecType,                      // In: Record type
   DWORD FieldFlags,                    // In:
   LPSTR szDesc,                        // In: Description string
   WORD  wFieldLen,                     // In: Size of buffer size
   LPSTR szField,                       // Out: Return field replacement
   LPWORD lpwLen,                       // Out: Len of return data
   WORD  wSize);                        // IN: SIZE OF lpRecord

EXTERNC BOOL SYM_EXPORT WINAPI FilterLog(                         // Returns TRUE if record included
   LPALOG lpALog,                       // In:
   WORD   wRecType,
   DWORD  FieldFlags,
   LPSTR lpRecord,                      // In: Log record data
   LPALOGFILTER pFilter,                // In: Filter applied to record
   int FAR *TypeIdx,                    // Out:Index into typecode array
   WORD wSize);                         // IN: SIZE OF lpRecord

#ifdef __cplusplus
}
#endif

#endif // !defined(SYM_NLM)
#endif // _SYMALOG_H
