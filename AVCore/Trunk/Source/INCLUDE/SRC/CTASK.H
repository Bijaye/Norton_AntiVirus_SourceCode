// Copyright 1998 Symantec Corporation
//////////////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/CTask.h_v   1.4   12 Jun 1998 16:36:04   mdunn  $
//
// Description: Declaration of the CTask task scheduling class.
//
//////////////////////////////////////////////////////////////////////////////
// $Log:   S:/INCLUDE/VCS/CTask.h_v  $
// 
//    Rev 1.4   12 Jun 1998 16:36:04   mdunn
// Added support for enabling/disabling events.
// 
//    Rev 1.3   18 Apr 1998 17:10:56   mdunn
// Added RemoveDuplicatedParam()
// 
//    Rev 1.2   09 Apr 1998 11:00:08   mdunn
// Tiny little Alpha fix - just replaced a missing semicolon.
// 
//    Rev 1.1   09 Apr 1998 10:44:30   mdunn
// Fudged a few of the MTS types so the class will still compile for Alpha.
// 
//    Rev 1.0   06 Apr 1998 17:36:34   mdunn
// Initial revision.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _M_ALPHA
    #pragma pack(push, 8)
    #include "mstask.h"
    #pragma pack(pop)
#endif

#include "npsapi.h"

#define MAX_DISPLAY_NAME_LENGTH MAX_DESCR
#define MAX_MTS_JOB_NAME_LENGTH (MAX_DESCR-5)   // -5 for ".job\0"
#define MAX_MSG_LENGTH          MAX_CMDLINE     // NPS msg length limit
#define MAX_COMMENT_LENGTH      MAX_CMDLINE     // should be enough space
#define MAX_STARTUP_DIR_LENGTH  SYM_MAX_PATH

#define NAVW_EXE_9x                 _T("navw32.exe")
#define NAVW_EXE_NT                 _T("navwnt.exe")
#define NAVLU_EXE                   _T("navlu32.exe /scheduled")

#define NAV_SCAN_DEFAULT_PARAMS     _T("/L")
#define NAV_LU_DEFAULT_PARAMS       _T("")

#define NAV_SCAN_HANDLER_EXE        _T("ScnHndlr.exe")

#define NAV_SCHEDULE_INI            _T("SCHEDULE.INI")
#define CTASK_SECTION               _T("CSymTask")
#define MTS_MESSAGE_DEFAULT_APP     _T("NTaskMgr.exe")

//////////////////////////////////////////////////////////////////////////////

enum TASKTYPE
{
    CTT_INVALID,
    CTT_NAVSCAN,                        // weekly scan
    CTT_NAVLU,                          // run LU to update NAV
    CTT_MESSAGE,                        // NPS only, display message
    CTT_RUNAPP,                         // run any ol' app
    CTT_NAVSCANTASK,                    // run a scan task (new in Odyssey)
    CTT_NOT_NAV,                        // MTS only - an event that wasn't
                                        // created by NAV
    CTT_LAST = CTT_NOT_NAV
};

//////////////////////////////////////////////////////////////////////////////
      
enum TASKFREQUENCY
{
    CTF_INVALID,
    CTF_HOURLY,
    CTF_DAILY,
    CTF_WEEKLY,
    CTF_WEEKDAYS,
    CTF_MONTHLY,
    CTF_TWICEMONTHLY,                   // for NAV LU only
    CTF_YEARLY,
    CTF_ONCE
};

//////////////////////////////////////////////////////////////////////////////

enum TASKSAVEACTION
{
    CTSA_OVERWRITE_IF_EXISTS,
    CTSA_FAIL_IF_EXISTS,
    CTSA_RENAME_IF_EXISTS
};

//////////////////////////////////////////////////////////////////////////////

// NOTE: These errors aren't actually used at present.  Maybe sometime
// in the future....

#define TASK_ERROR_BASE 0xA000F000UL
#define TASK_STATUS_BASE 0x2000F000UL

#define CTE(x) ((HRESULT)(TASK_ERROR_BASE + x))
#define CTS(x) ((HRESULT)(TASK_STATUS_BASE + x))

#define CTS_OK                          0
#define CTS_NO_MORE_ITEMS               CTS(1)
#define CTS_ITEM_NOT_FOUND              CTS(2)

#define CTE_GENERAL_FAILURE             CTE(1)
#define CTE_STRING_READ_ERROR           CTE(2)
#define CTE_NOT_INITIALIZED             CTE(3)
#define CTE_NAV_NOT_INSTALLED           CTE(4)
#define CTE_MTS_NOT_INSTALLED           CTE(5)
#define CTE_REQD_PARAM_NOT_SET          CTE(6)
#define CTE_ENUM_NOT_BEGUN              CTE(7)
#define CTE_ENUM_FAILED                 CTE(8)
#define CTE_COMMAND_LINE_TOO_LONG       CTE(9)
#define CTE_NPS_EVENTS_PUT_FAILED       CTE(10)
#define CTE_NO_MORE_ITEMS               CTE(11)
#define CTE_ITEM_NOT_FOUND              CTE(12)
#define CTE_ENUM_INVALID_VALUE_PASSED   CTE(13)

//////////////////////////////////////////////////////////////////////////////

struct SYMTASKREC
{
    TCHAR szDisplayName [ MAX_DESCR ];
    TCHAR szAppPath [ MAX_PATH ];
    TCHAR szParams [ MAX_PATH ];
    TCHAR szStartingDir [ MAX_PATH ];
    TCHAR szComment [ MAX_COMMENT_LENGTH ]; // MTS only
    TCHAR szMsg [ MAX_MSG_LENGTH ];         // NPS only - message text
    TCHAR szScanTask [ MAX_PATH ];          // Odyssey scan task name
    int           nCmdShow;                 // NPS only
    BOOL          bEnabled;
    BOOL          bAudibleAlert;            // NPS only

    TASKTYPE      nTaskType;
    TASKFREQUENCY nFrequency;

    SYSTEMTIME    rStartTime;           // start & trigger time
    WORD          wDay2;                // MTS only - day 2 of a twice-monthly task
    WORD          wDayOfWeek;           // weekly events only= Sun=0, Mon=1, etc.

    SYSTEMTIME    rLastRunTime;         // Date/time the event last ran
    SYSTEMTIME    rNextRunTime;         // MTS only - next scheduled run

    SYMTASKREC()
        {
        *szDisplayName = *szAppPath = *szParams = *szStartingDir =
            *szComment = *szMsg = *szScanTask = '\0';

        nCmdShow = SW_SHOW;
        bEnabled = TRUE;
        bAudibleAlert = TRUE;
        nTaskType = CTT_INVALID;
        nFrequency = CTF_INVALID;
        ZeroMemory ( &rStartTime, sizeof(SYSTEMTIME) );
        ZeroMemory ( &rLastRunTime, sizeof(SYSTEMTIME) );
        ZeroMemory ( &rNextRunTime, sizeof(SYSTEMTIME) );
        wDay2 = 0;
        wDayOfWeek = 0;
        }
};

//////////////////////////////////////////////////////////////////////////////

#ifdef _M_ALPHA
                                        // Fudge a few types, since we
                                        // don't have an MTS lib for Alpha.
typedef int  ITaskScheduler;
typedef int  IEnumWorkItems;
struct  TASK_TRIGGER { int x; };

#endif

//////////////////////////////////////////////////////////////////////////////

class CTask
{
// Construction & destruction

public:
    CTask ( SYMTASKREC* pTaskRec = NULL );
    ~CTask();


// Data members

private:
    // State variables
    BOOL            m_bNPSMode;         // NPS/MTS flag
    BOOL            m_bInitialized;     // General initialized flag
    BOOL            m_bCoInitializeCalled;
    BOOL            m_bMTSInstalled;    // MTS functions active?
    BOOL            m_bNAVInstalled;    // NPS functions active?

    // OLE interface pointers
    ITaskScheduler* m_pITaskScheduler;
    IEnumWorkItems* m_pIEnum;

    // NPS event enumeration stuff
    BOOL            m_bEnumeratingNPSTasks;
    WORD            m_wNextTaskToEnum;  // index of next task to return

    // Directories
    TCHAR           m_szNAVDir [ MAX_PATH ];
    TCHAR           m_szNPSDatFilePath [ MAX_PATH ];

    // General flags
    BOOL            m_bOSIsNT;

    // General stuff
    TASKTYPE    m_nTaskType;
    LPTSTR      m_pszDisplayName;
    LPTSTR      m_pszAppPath;
    LPTSTR      m_pszParams;
    LPTSTR      m_pszStartingDir;
    LPTSTR      m_pszMsg;               // NPS only (msg to display)
    LPTSTR      m_pszScanTask;          // Odyssey scan task name
    LPTSTR      m_pszComment;           // MTS only
    BOOL        m_bTaskEnabled;
    BOOL        m_bAudibleAlert;        // NPS only
    int         m_nCmdShow;             // NPS only
    
    // Trigger date & time
    TASKFREQUENCY m_nFrequency;
    WORD          m_wDayOfWeek;         // Sun=0, Mon=1, etc.
    WORD          m_wDay;               // 1-31
    WORD          m_wDay2;              // 1-31, for LU tasks only
    WORD          m_wMonth;             // Jan=1, Feb=2, etc.
    WORD          m_wYear;              // The real year number
    WORD          m_wHour;
    WORD          m_wMinute;

    TASK_TRIGGER m_rTrigger;            // MTS struct
    //&^ put NPS struct(s) here!


// Functions

private:
    BOOL GetNAVDir();
    BOOL InitNPSAPIGlobals();

    BOOL SaveToNPS ( TASKSAVEACTION nAction = CTSA_OVERWRITE_IF_EXISTS );
    BOOL SaveToMTS ( TASKSAVEACTION nAction = CTSA_OVERWRITE_IF_EXISTS );

    BOOL EnumNPSTasks();
    BOOL EnumMTSTasks ( IEnumWorkItems** );

    BOOL NextNPSTask ( SYMTASKREC* );
    BOOL NextMTSTask ( IEnumWorkItems*, SYMTASKREC* );

    BOOL DeleteNPSTask ( const SYMTASKREC*, BOOL bDeleteByName );
    BOOL DeleteNPSTask ( TASKTYPE nType );
    BOOL DeleteNPSTask ( LPCSTR );
    BOOL DeleteNPSTask ( LPCWSTR );

    BOOL DeleteMTSTask ( const SYMTASKREC*, BOOL bDeleteByName );
    BOOL DeleteMTSTask ( TASKTYPE nType );
    BOOL DeleteMTSTask ( LPCSTR );
    BOOL DeleteMTSTask ( LPCWSTR );

    BOOL FindNPSTask ( SYMTASKREC* );
    BOOL FindMTSTask ( SYMTASKREC* );

    void FillTriggerScheduleFields();

    void ReturnNPSScheduleInfo ( const NPSEVNT* pEvent,
                                 SYMTASKREC*    pInfo );
    void ReturnMTSScheduleInfo ( const TASK_TRIGGER* pTrig,
                                 SYMTASKREC*         pInfo );

    BOOL CreateNewJobName ( LPWSTR wszJobName, UINT* puCounter );

    void RemoveDuplicatedParam ( LPTSTR szParams );

public:
    // General initialization
    BOOL Init();

    // Querying if a scheduler is available
    BOOL IsNPSAvailable();
    BOOL IsMTSAvailable();

    // Mode switching
    void SetNPSMode();
    void SetMTSMode();

    // Setting up tasks.
    BOOL SetTaskData ( SYMTASKREC* );
    void SetDateAndTime ( const SYSTEMTIME* );

    // Saving tasks
                                        // Save using current mode
    BOOL Save ( TASKSAVEACTION nAction = CTSA_OVERWRITE_IF_EXISTS );

                                        // Save in other mode's format
    BOOL ConvertAndSave ( TASKSAVEACTION nAction = CTSA_OVERWRITE_IF_EXISTS );

    // Deleting tasks
    BOOL DeleteTask ( const SYMTASKREC*, BOOL bDeleteByName = TRUE );
    BOOL DeleteTask ( LPCSTR );
    BOOL DeleteTask ( LPCWSTR );

    // Enumerating tasks
    BOOL EnumTasks();                   // Start the enum process
    void ResetEnum();
    BOOL NextTask ( SYMTASKREC* );      // Read in next task
    BOOL NextNAVTask ( SYMTASKREC* );   // Read in next scan/LU task

    // Finding a task by name
    BOOL FindTask ( SYMTASKREC* );
};

//////////////////////////////////////////////////////////////////////////////
// Global variables from NPSAPI.LIB

extern char szIniNamedEvents[SSIZE];
extern char szIniName[SSIZE];
extern char szIniPrompt[SSIZE];
extern char szIniCmdLine[SSIZE];
extern char szIniStartupDir[SSIZE];
extern char szIniRunStyle[SSIZE];
extern char szHeaderFile[];
extern char szHeaderEntry[];
extern char szEventMessage[];
extern char szEventProgram[];
extern char szNortonIni[];
extern char szBinFile[];

//////////////////////////////////////////////////////////////////////////////
// Other globals

extern char szNAVScanSection[];
extern char szNAVLUSection[];

extern char szEventNAVScan[];
extern char szEventNAVLU[];

extern const WORD s_awDayFlags[];
extern const WORD s_awMonthFlags[];
