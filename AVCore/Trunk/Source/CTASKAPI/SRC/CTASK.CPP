// Copyright 1998 Symantec Corporation
//////////////////////////////////////////////////////////////////////////////
// $Header:   S:/CTASKAPI/VCS/CTASK.CPv   1.5   12 Jun 1998 16:36:40   mdunn  $
//
// Description: General CTask functions, not specific to either scheduler.
//
// Contains:
//
// See Also: MTSStuff.cpp, NPSStuff.cpp
//
//////////////////////////////////////////////////////////////////////////////
// $Log:   S:/CTASKAPI/VCS/CTASK.CPv  $ *
// 
//    Rev 1.5   12 Jun 1998 16:36:40   mdunn
// Added support for enabling/disabling events
// 
//    Rev 1.4   18 Apr 1998 17:10:00   mdunn
// Fixed string parsing when adding events (using RemoveDuplicatedParam) so
// that adding NAV LU events to MTS works!
// 
//    Rev 1.3   13 Apr 1998 19:46:12   mdunn
// Added auto-mode-selection in CTask::Init().  See comments in that function
// for more info.
// Also fixed a bug that was always setting m_bMTSInstalled to TRUE.
// 
//    Rev 1.2   09 Apr 1998 11:00:56   mdunn
// Couple more Alpha #ifdefs.
// 
//    Rev 1.1   09 Apr 1998 10:45:12   mdunn
// #ifdef'ed out MTS code when compiling for Alpha.
// 
//    Rev 1.0   06 Apr 1998 17:36:52   mdunn
// Initial revision.
//
//////////////////////////////////////////////////////////////////////////////

#include "platform.h"
#include <tchar.h>
#include <commctrl.h>
#include <afxpriv.h>
#include <string.h>

#include "ctask.h"


//////////////////////////////////////////////////////////////////////////////
// Globals...

char szEventNAVScan [SSIZE];
char szEventNAVLU [SSIZE];
char szNAVScanSection [SSIZE];
char szNAVLUSection [SSIZE];


#ifndef _M_ALPHA

const WORD s_awDayFlags[] =
{
    TASK_SUNDAY, TASK_MONDAY, TASK_TUESDAY, TASK_WEDNESDAY, TASK_THURSDAY,
    TASK_FRIDAY, TASK_SATURDAY
};
#else
const WORD s_awDayFlags[] = { 0 };
#endif


#ifndef _M_ALPHA

const WORD s_awMonthFlags[] =
{
    TASK_JANUARY, TASK_FEBRUARY, TASK_MARCH, TASK_APRIL, TASK_MAY, TASK_JUNE,
    TASK_JULY, TASK_AUGUST, TASK_SEPTEMBER, TASK_OCTOBER, TASK_NOVEMBER,
    TASK_DECEMBER
};
#else
const WORD s_awMonthFlags[] = { 0 };
#endif


//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//  CTask constructor & destructor


//////////////////////////////////////////////////////////////////////////
//
// Function:    CTask constructor
//
// Description:
//  Initializes the CTask object with info from a supplied SYMTASKREC.
//
// Input:
//  pTaskRec: [in] Holds info to copy into this object.  If NULL, the
//            object gets default values (mostly 0/NULL).
//
//////////////////////////////////////////////////////////////////////////

CTask::CTask ( SYMTASKREC* pTaskRec /*=NULL*/ )
{
    m_bNPSMode = TRUE;

    m_bInitialized         = FALSE;
    m_bCoInitializeCalled  = FALSE;
    m_bMTSInstalled        = FALSE;
    m_bNAVInstalled        = FALSE;

    m_pITaskScheduler = NULL;
    m_pIEnum          = NULL;

    m_bEnumeratingNPSTasks = FALSE;
    m_wNextTaskToEnum = 0;

    m_nTaskType  = CTT_INVALID;
    m_nFrequency = CTF_INVALID;

    m_bOSIsNT = GetVersion() & 0x80000000UL ? FALSE : TRUE;

    
    if ( NULL != pTaskRec )
        {
        m_pszDisplayName = &pTaskRec->szDisplayName[0];
        m_pszAppPath     = &pTaskRec->szAppPath[0];
        m_pszParams      = &pTaskRec->szParams[0];
        m_pszStartingDir = &pTaskRec->szStartingDir[0];
        m_pszComment     = &pTaskRec->szComment[0];
        m_pszMsg         = &pTaskRec->szMsg[0];
        m_pszScanTask    = &pTaskRec->szScanTask[0];

        m_nCmdShow       = pTaskRec->nCmdShow;
        m_bTaskEnabled   = pTaskRec->bEnabled;
        m_bAudibleAlert  = pTaskRec->bAudibleAlert;

        m_nTaskType      = pTaskRec->nTaskType;
        m_nFrequency     = pTaskRec->nFrequency;

        SetDateAndTime ( &pTaskRec->rStartTime );

        m_wDay2          = pTaskRec->wDay2;
        m_wDayOfWeek     = pTaskRec->wDayOfWeek;
        }
    else
        {
        m_pszDisplayName = NULL;
        m_pszAppPath     = NULL;
        m_pszParams      = NULL;
        m_pszStartingDir = NULL;
        m_pszComment     = NULL;
        m_pszMsg         = NULL;
        m_pszScanTask    = NULL;
        m_bTaskEnabled   = TRUE;
        m_bAudibleAlert  = TRUE;
        m_nCmdShow       = SW_SHOW;

        m_nTaskType      = CTT_INVALID;
        m_nFrequency     = CTF_INVALID;

        SetDateAndTime ( NULL );

        m_wDay2          = 0;
        m_wDayOfWeek     = 0;
        }
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CTask destructor
//
// Description:
//  Cleans up loose ends and releases OLE interface pointers.
//
//////////////////////////////////////////////////////////////////////////

CTask::~CTask()
{
    ResetEnum();

#ifndef _M_ALPHA

    if ( NULL != m_pITaskScheduler )
        {
        m_pITaskScheduler->Release();
        m_pITaskScheduler = NULL;
        }

#endif

    if ( m_bCoInitializeCalled )
        {
        CoUninitialize();
        }
}


//////////////////////////////////////////////////////////////////////////////
//  CTask public init function


//////////////////////////////////////////////////////////////////////////
//
// Function:    Init()
//
// Description:
//  Initializes a few private CTask variables.
//
// Input:
//  Nothing.
//
// Returns:
//  TRUE if the object is ready to use, FALSE if not.
//
// Note:
//  This function *must* be called before doing any saving or enumerating
//  of tasks.  Other member functions will fail if you don't!
//
//////////////////////////////////////////////////////////////////////////

BOOL CTask::Init()
{
BOOL          bRetVal = FALSE;
HRESULT       hres    = S_OK;
OSVERSIONINFO rOSInfo = { sizeof(OSVERSIONINFO) };
BOOL          bOSIs98 = FALSE;
BOOL          bSelectNPSMode;
TCHAR         szNPSPath [MAX_PATH];

    // Check the OS - if we're on 98, we may have to auto-select the
    // scheduler mode.

    if ( GetVersionEx ( &rOSInfo )  &&
         rOSInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS  &&
         rOSInfo.dwMajorVersion == 4  &&
         rOSInfo.dwMinorVersion >= 10 ) // yes, 10. MSVC docs say 1 but that's wrong.
        {
        bOSIs98 = TRUE;
        }


    if ( !InitNPSAPIGlobals() )
        {
        TRACE("InitNPSAPIGlobals() failed.");
        goto bailout;
        }

                                        // Read in the NAV dir.
    NpsGetMyPath ( m_szNAVDir );

                                        // Build the path to SCHEDULE.DAT
    if ( '\0' != *m_szNAVDir )
        {
        m_bNAVInstalled = TRUE;

        lstrcpy ( m_szNPSDatFilePath, m_szNAVDir );

        if ( '\\' != m_szNPSDatFilePath [ lstrlen (m_szNPSDatFilePath) - 1 ] )
            {
            lstrcat ( m_szNPSDatFilePath, _T("\\") );
            }

        lstrcat ( m_szNPSDatFilePath, szBinFile );
        }


#ifndef _M_ALPHA

    hres = CoInitialize ( NULL );

    m_bCoInitializeCalled = TRUE;

    if ( FAILED ( hres ) )
        {
        TRACE("CoInitialize() failed.\n");
        goto bailout;
        }

                                        // Get a pointer to the MTS interface.
    hres = CoCreateInstance (
               CLSID_CTaskScheduler,
               NULL,
               CLSCTX_INPROC_SERVER,
               IID_ITaskScheduler,
               (void **) &m_pITaskScheduler );

    if ( FAILED ( hres ) )
        {
        TRACE("Warning: CoCreateInstance() failed to create an ITaskScheduler.\n");
        }
    else
        {
        m_bMTSInstalled = TRUE;
        }


#endif  // ndef _M_ALPHA


    m_bInitialized = TRUE;

    bRetVal = TRUE;


                                        // Do auto-mode-selection here.
                                        // If we're on anything other than
                                        // 98, select NPS mode.  Otherwise,
                                        // on 98, select MTS mode _unless_
                                        // NSCHED32.EXE exists in the NAV
                                        // dir, in which case select NPS mode.

                                        // First set the default action.
    bSelectNPSMode = bOSIs98 ? FALSE : TRUE;


    if ( bOSIs98  &&  m_bNAVInstalled )
        {
                                        // Look for NSCHED32.EXE
        lstrcpy ( szNPSPath, m_szNAVDir );

        if ( '\\' != szNPSPath [ lstrlen (szNPSPath) - 1 ] )
            {
            lstrcat ( szNPSPath, _T("\\") );
            }

        lstrcat ( szNPSPath, _T("nsched32.exe") );

        if ( GetFileAttributes ( szNPSPath ) != 0xFFFFFFFF )
            {
            bSelectNPSMode = TRUE;      // The file existed!
            }
        }

    bSelectNPSMode ? SetNPSMode() : SetMTSMode();


bailout:
    return bRetVal;
}


//////////////////////////////////////////////////////////////////////////////
//  CTask mode switching functions


//////////////////////////////////////////////////////////////////////////
//
// Function:    SetNPSMode()
//
// Description:
//  Sets the CTask object to use NPS in future calls to save, enumerate,
//  and delete tasks.
//
// Input:
//  Nothing.
//
// Returns:
//  Nothing.
//
// Note:
//  Calling this function will reset any enumeration in progress.
//
//////////////////////////////////////////////////////////////////////////

void CTask::SetNPSMode()
{
    m_bNPSMode = TRUE;
    ResetEnum();
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    SetMTSMode()
//
// Description:
//  Sets the CTask object to use MTS in future calls to save, enumerate,
//  and delete tasks.
//
// Input:
//  Nothing.
//
// Returns:
//  Nothing.
//
// Note:
//  Calling this function will reset any enumeration in progress.
//
//////////////////////////////////////////////////////////////////////////

void CTask::SetMTSMode()
{
#ifndef _M_ALPHA
    m_bNPSMode = FALSE;
#endif

    ResetEnum();
}


//////////////////////////////////////////////////////////////////////////////
// Querying CTask status.


//////////////////////////////////////////////////////////////////////////
//
// Function:    IsNPSAvailable()
//
// Description:
//  Returns a BOOL indicating if NPS is installed on the system.
//
// Input:
//  Nothing.
//
// Returns:
//  TRUE if NPS is installed, FALSE if not.
//
// Note:
//  This function relies on the NAV InstalledApps reg value being present.
//
//////////////////////////////////////////////////////////////////////////

BOOL CTask::IsNPSAvailable()
{
    return m_bNAVInstalled;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    IsMTSAvailable()
//
// Description:
//  Returns a BOOL indicating if MTS is installed on the system.
//
// Input:
//  Nothing.
//
// Returns:
//  TRUE if MTS is installed, FALSE if not.
//
// Note:
//  This function does not verify that MTS is currently running.
//
//////////////////////////////////////////////////////////////////////////

BOOL CTask::IsMTSAvailable()
{
    return m_bMTSInstalled;
}


//////////////////////////////////////////////////////////////////////////////
//  CTask helper functions


//////////////////////////////////////////////////////////////////////////
//
// Function:    InitNPSAPIGlobals()
//
// Description:
//  Initializes some CTask globals and a few globals defined in NPSAPI.LIB.
//
// Input:
//  Nothing.
//
// Returns:
//  TRUE if everything was initialized OK, FALSE if not.
//
// Note:
//  Several strings are read from SCHEDULE.INI.  Therefore, "SCHEDULE.INI"
//  has been hard-coded, since we can't very well store the INI file name
//  in the INI file.
//
//////////////////////////////////////////////////////////////////////////

BOOL CTask::InitNPSAPIGlobals()
{
WORD wRet = FALSE;

    lstrcpy ( szNortonIni, NAV_SCHEDULE_INI );

                                        /* Load our file header         */
    if ( !GetPrivateProfileString (
              CTASK_SECTION, _T("FileHeaderSig"), _T(""),
              szHeaderFile, NPS_FILE_HDR_SIZE , szNortonIni ) )
        {
        goto NpsInitGlobalsExit;
        }


                                        /* Load our entry header        */
    if ( !GetPrivateProfileString (
              CTASK_SECTION, _T("EntryHeaderSig"), _T(""),
              szHeaderEntry, NPS_ENTRY_HDR_SIZE , szNortonIni ) )
        {
        goto NpsInitGlobalsExit;
        }



                                        /* Load our bin file name       */
    if ( !GetPrivateProfileString (
              CTASK_SECTION, _T("ScheduleBinFile"), _T(""),
              szBinFile, SYM_MAX_PATH , szNortonIni ) )
        {
        goto NpsInitGlobalsExit;
        }

                                        // Load "NAMEDEVENTS", but the
                                        //   'GetPrivateProfileString()' call
                                        //   in 'NpsNamedEventsLoad()' in
                                        //   NPSADEDT.CPP to retrieve a
                                        //   string from the seciton,
                                        //   [NamedEvents] in SCHEDULE.INI
                                        //   is case-insensitive. The section
                                        //   name was all capitalized so that
                                        //   a localizer will not translate
                                        //   the section name, which will
                                        //   prevent the section name from
                                        //   mis-matching.
    if ( !GetPrivateProfileString (
              CTASK_SECTION, _T("NamedEventsSectionName"), _T(""),
              szIniNamedEvents, SSIZE , szNortonIni ) )
        {
        goto NpsInitGlobalsExit;
        }

                                        // Load "Name="
    if ( !GetPrivateProfileString (
              CTASK_SECTION, _T("NameKey"), _T(""),
              szIniName, SSIZE , szNortonIni ) )
        {
        goto NpsInitGlobalsExit;
        }

                                        // Load "Prompt="
    if ( !GetPrivateProfileString (
              CTASK_SECTION, _T("PromptKey"), _T(""),
              szIniPrompt, SSIZE , szNortonIni ) )
        {
        goto NpsInitGlobalsExit;
        }

                                        // Load "CommandLine="
    if ( !GetPrivateProfileString (
              CTASK_SECTION, _T("CommandLineKey"), _T(""),
              szIniCmdLine, SSIZE , szNortonIni ) )
        {
        goto NpsInitGlobalsExit;
        }

                                        // Load "StartupDir="
    if ( !GetPrivateProfileString (
              CTASK_SECTION, _T("StartupDirKey"), _T(""),
              szIniStartupDir, SSIZE , szNortonIni ) )
        {
        goto NpsInitGlobalsExit;
        }

                                        // Load "RunStyle="
    if ( !GetPrivateProfileString (
              CTASK_SECTION, _T("RunStyleKey"), _T(""),
              szIniRunStyle, SSIZE , szNortonIni ) )
        {
        goto NpsInitGlobalsExit;
        }

                                        // Load "DefaultDesc="
    if ( !GetPrivateProfileString (
              CTASK_SECTION, _T("DefaultDescKey"), _T(""),
              szIniDefDesc, SSIZE , szNortonIni ) )
        {
        goto NpsInitGlobalsExit;
        }

                                        // Load "DefaultActionText="
    if ( !GetPrivateProfileString (
              CTASK_SECTION, _T("DefaultActionTextKey"), _T(""),
              szIniDefActionText, SSIZE , szNortonIni ) )
        {
        goto NpsInitGlobalsExit;
        }

                                        // Load "Display message"
    if ( !GetPrivateProfileString (
              CTASK_SECTION, _T("MessageEventName"), _T(""),
              szEventMessage, SSIZE , szNortonIni ) )
        {
        goto NpsInitGlobalsExit;
        }

                                        // Load "Run program"
    if ( !GetPrivateProfileString (
              CTASK_SECTION, _T("RunProgramEventName"), _T(""),
              szEventProgram, SSIZE , szNortonIni ) )
        {
        goto NpsInitGlobalsExit;
        }


                                        // Load NAV scan section name
    if ( !GetPrivateProfileString (
              CTASK_SECTION, _T("NAVScanSectionName"), _T(""),
              szNAVScanSection, SSIZE , szNortonIni ) )
        {
        goto NpsInitGlobalsExit;
        }

                                        // Load NAV LiveUpdate section name
    if ( !GetPrivateProfileString (
              CTASK_SECTION, _T("NAVLUSectionName"), _T(""),
              szNAVLUSection, SSIZE , szNortonIni ) )
        {
        goto NpsInitGlobalsExit;
        }


                                        // Load name of NAV Scan event
    if ( !GetPrivateProfileString ( szNAVScanSection, szIniName, "",
                                    szEventNAVScan, SSIZE, szNortonIni ))
        {
        goto NpsInitGlobalsExit;
        }

                                        // Load name of NAV LiveUpdate event
    if ( !GetPrivateProfileString ( szNAVLUSection, szIniName, "",
                                    szEventNAVLU, SSIZE, szNortonIni ))
        {
        goto NpsInitGlobalsExit;
        }


                                        // We are successful if we go here.
    wRet = TRUE;

NpsInitGlobalsExit:

    return wRet;

}


//////////////////////////////////////////////////////////////////////////////
//  CTask reading/writing task data


//////////////////////////////////////////////////////////////////////////
//
// Function:    SetTaskData()
//
// Description:
//  Sets the CTask object's data from the passed-in SYMTASKREC.
//
// Input:
//  pTaskInfo: [in] Pointer to a SYMTASK struct which holds the data
//             to store in the CTask object.  May not be NULL.
//
// Returns:
//  TRUE always.
//
//////////////////////////////////////////////////////////////////////////

BOOL CTask::SetTaskData ( SYMTASKREC* pTaskInfo )
{
    ASSERT ( NULL != pTaskInfo );

    //&^ This could use some bulletproofing and range checking of values.

    m_pszDisplayName = &pTaskInfo->szDisplayName[0];
    m_pszAppPath     = &pTaskInfo->szAppPath[0];
    m_pszParams      = &pTaskInfo->szParams[0];
    m_pszStartingDir = &pTaskInfo->szStartingDir[0];
    m_pszComment     = &pTaskInfo->szComment[0];
    m_pszMsg         = &pTaskInfo->szMsg[0];
    m_pszScanTask    = &pTaskInfo->szScanTask[0];
    
    m_nCmdShow       = pTaskInfo->nCmdShow;
    m_bTaskEnabled   = pTaskInfo->bEnabled;
    m_bAudibleAlert  = pTaskInfo->bAudibleAlert;
    m_nTaskType      = pTaskInfo->nTaskType;
    m_nFrequency     = pTaskInfo->nFrequency;

    SetDateAndTime ( &pTaskInfo->rStartTime );

    m_wDay2 = pTaskInfo->wDay2;
    m_wDayOfWeek = pTaskInfo->wDayOfWeek;

    return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    SetDateAndTime()
//
// Description:
//  Sets a task's start date and time.
//
// Input:
//  pStartTime: [in] Pointer to a SYSTEMTIME struct that holds a date
//              and time which will become the new start date/time.
//              If NULL, the date and time are reset to 0.
//
// Returns:
//  Nothing.
//
// Note:
//  This function can *not* be used to set frequency or other task
//  parameters.
//
//////////////////////////////////////////////////////////////////////////

void CTask::SetDateAndTime ( const SYSTEMTIME* pStartTime )
{
    if ( NULL != pStartTime )
        {
        m_wDay       = pStartTime->wDay;
        m_wMonth     = pStartTime->wMonth;
        m_wYear      = pStartTime->wYear;
        m_wHour      = pStartTime->wHour;
        m_wMinute    = pStartTime->wMinute;
        }
    else
        {
        m_wDay       = 0;
        m_wMonth     = 0;
        m_wYear      = 0;
        m_wHour      = 0;
        m_wMinute    = 0;
        }
}


//////////////////////////////////////////////////////////////////////////////
//  CTask saving functions


//////////////////////////////////////////////////////////////////////////
//
// Function:    Save()
//
// Description:
//  Saves the current CTask object data as a scheduled task.
//
// Input:
//  nAction: [in] An action to take if the current task name is the
//           same as an existing task.  Note that this is used only when
//           saving to MTS.  It is ignored in NPS mode, and the event
//           is always saved with the given name.
//
// Returns:
//  TRUE if the task was saved, FALSE if not.
//
//////////////////////////////////////////////////////////////////////////

BOOL CTask::Save ( TASKSAVEACTION nAction /*=CTSA_OVERWRITE*/ )
{
    return m_bNPSMode ? SaveToNPS ( nAction ) :
                        SaveToMTS ( nAction );
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    ConvertAndSave()
//
// Description:
//  Saves the current CTask data to MTS if the object is in NPS mode, or
//  to NPS if the object is in MTS mode.
//
// Input:
//  nAction: [in] See CTask::Save() above.
//
// Returns:
//  TRUE if the task was saved, FALSE if not.
//
//////////////////////////////////////////////////////////////////////////

BOOL CTask::ConvertAndSave ( TASKSAVEACTION nAction /*=CTSA_OVERWRITE*/ )
{
    return m_bNPSMode ? SaveToMTS ( nAction ) :
                        SaveToNPS ( nAction );
}


//////////////////////////////////////////////////////////////////////////////
//  CTask enumeration functions


//////////////////////////////////////////////////////////////////////////
//
// Function:    EnumTasks()
//
// Description:
//  Sets up the CTask object to begin enumerating tasks in the scheduler
//  of the current mode (NPS or MTS).
//
// Input:
//  Nothing.
//
// Returns:
//  TRUE if the CTask object is ready to begin enumerating, or FALSE if
//  not.
//
//////////////////////////////////////////////////////////////////////////

BOOL CTask::EnumTasks()
{
    return m_bNPSMode ? EnumNPSTasks() : EnumMTSTasks ( &m_pIEnum );
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    ResetEnum()
//
// Description:
//  Stops any enumeration in progress and resets internal enum-related
//  variables.
//
// Input:
//  Nothing.
//
// Returns:
//  Nothing.
//
// Note:
//  This function is called automatically from SetMTSMode(), SetNPSMode(),
//  and the CTask destructor.
//
//////////////////////////////////////////////////////////////////////////

void CTask::ResetEnum()
{
#ifndef _M_ALPHA
    if ( NULL != m_pIEnum )
        {
        m_pIEnum->Release();
        m_pIEnum = NULL;
        }
#endif

    m_bEnumeratingNPSTasks = FALSE;
    m_wNextTaskToEnum = 0;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    NextTask()
//
// Description:
//  Retrieves the next task from the scheduler of the current mode.
//
// Input:
//  pInfo: [out] If a task is successfully retrieved, the task's data is
//         copied to the struct pointed to by this param.  May not be NULL.
//
// Returns:
//  TRUE if a task was retrieved, or FALSE if there was an error or if
//  there are no more tasks left.
//
// Note:
//  You must call EnumTasks() and it must return TRUE before this function
//  will succeed.
//
//////////////////////////////////////////////////////////////////////////

BOOL CTask::NextTask ( SYMTASKREC* pInfo )
{
    ASSERT ( NULL != pInfo );

    if ( NULL == pInfo )
        return FALSE;

    if ( ( m_bNPSMode  &&  !m_bEnumeratingNPSTasks ) ||
         ( !m_bNPSMode  &&  NULL == m_pIEnum ) )
        {
        TRACE("NextTask() called before EnumTasks(). Exiting from NextTask().\n");
        return FALSE;
        }

    return m_bNPSMode ? NextNPSTask ( pInfo ) : 
                        NextMTSTask ( m_pIEnum, pInfo );
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    NextNAVTask()
//
// Description:
//  Like NextTask(), but only returns NAV Scan and NAV LU tasks.
//
// Input:
//  pInfo: [out] See NextTask() above.
//
// Returns:
//  TRUE if a task was retrieved, or FALSE if there was an error or if
//  there are no more NAV tasks.
//
// Note:
//  In MTS mode, this only retrieves NAV tasks created by CTask.
//
//////////////////////////////////////////////////////////////////////////

BOOL CTask::NextNAVTask ( SYMTASKREC* pInfo )
{
    ASSERT ( NULL != pInfo );

    if ( NULL == pInfo )
        return FALSE;
    
    while ( NextTask ( pInfo ) )
        {
        if ( pInfo->nTaskType == CTT_NAVLU  ||
             pInfo->nTaskType == CTT_NAVSCAN )
            {
            return TRUE;
            }
        }

    return FALSE;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    FindTask()
//
// Description:
//  Finds a task with a given display name.
//
// Input:
//  pTaskInfo: [in/out] Pointer to a SYMTASKREC.  The szDisplayName
//             member must contain the name to search for.
//
// Returns:
//  TRUE if a matching task was found and the task data was copied to
//  the SYMTASKREC struct, or FALSE if no matching task was found.
//
//////////////////////////////////////////////////////////////////////////

BOOL CTask::FindTask ( SYMTASKREC* pTaskInfo )
{
    ASSERT ( NULL != pTaskInfo );

    //&^bulletproof

    return m_bNPSMode ? FindNPSTask ( pTaskInfo ) :
                        FindMTSTask ( pTaskInfo );
}


//////////////////////////////////////////////////////////////////////////////
// Deleting tasks


//////////////////////////////////////////////////////////////////////////
//
// Function:    DeleteTask()
//
// Description:
//  Deletes a task with a given display name or type.
//
// Input:
//  pTaskrec: [in] Points to a SYMTASKREC whose szDisplayName or
//            nTaskType member is filled in.
//  bDeleteByName: [in] If TRUE, the szDisplayName field is used when
//                 searching for a task to delete.  If FALSE, the
//                 nTaskType field is used.
//
// Returns:
//  TRUE if a task was found and deleted, or FALSE if no task was found
//  or an error occured whilst deleting.
//
//////////////////////////////////////////////////////////////////////////

BOOL CTask::DeleteTask ( const SYMTASKREC* pTaskrec,
                         BOOL  bDeleteByName /*=TRUE*/ )
{
    return m_bNPSMode ? DeleteNPSTask ( pTaskrec, bDeleteByName ) :
                        DeleteMTSTask ( pTaskrec, bDeleteByName );
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    DeleteTask()
//
// Description:
//  Deletes a task with a given display name.
//
// Input:
//  szJobName/wszJobName: String containing the name to search for.
//
// Returns:
//  TRUE if a task was found and deleted, or FALSE if no task was found
//  or an error occured whilst deleting.
//
//////////////////////////////////////////////////////////////////////////

BOOL CTask::DeleteTask ( LPCSTR szJobName )
{
    return m_bNPSMode ? DeleteNPSTask ( szJobName ) : 
                        DeleteMTSTask ( szJobName );
}

BOOL CTask::DeleteTask ( LPCWSTR wszJobName )
{
    return m_bNPSMode ? DeleteNPSTask ( wszJobName ) : 
                        DeleteMTSTask ( wszJobName );
}


//////////////////////////////////////////////////////////////////////////////
//  CTask general utility functions


//////////////////////////////////////////////////////////////////////////
//
// Function:    CreateNewJobName()
//
// Description:
//  Given a job name, adds a suffix in an attempt to find a unique name that
//  can be added to MTS.
//
// Input:
//  wszJobName: [in/out] The job name (with or without suffix and .job
//              extension).
//  puCounter: [in/out] Pointer to a UINT that holds a counter used 
//             whilst looking for names.  The calling code should init
//             this value to 0 before calling this fn for the first time,
//             and not touch the value afterwords.
//
// Returns:
//  TRUE if a new name was created, FALSE if the hard-coded limit of 
//  names was reached (currently 100).
//
// Notes:
//  This isn't too logical... it puts all the burden on the calling code.
//  It really ought to be rewritten to be called _once_ and return a
//  unique name.
//  See the calls to this fn in CTask::SaveToMTS() for an example of how
//  silly it is.
//
//////////////////////////////////////////////////////////////////////////

BOOL CTask::CreateNewJobName ( LPWSTR wszJobName, UINT* puCounter )
{
WCHAR  wszWork [ MAX_DISPLAY_NAME_LENGTH + 5 ];
WCHAR  wszCopyTag [5];
LPWSTR pwszExt;
LPWSTR pwszLeftParen;
LPWSTR pwszRightParen;
BOOL   bRestoreJOBExt = FALSE;

    if ( *puCounter < 1 )
        *puCounter = 1;

    if ( *puCounter >= 99 )
        return FALSE;                   // max of 99 copies of a name.


    pwszExt = wcsrchr ( wszJobName, '.' );

                                        // Does the job name have the ".job"
                                        // extension?
    if ( NULL != pwszExt  &&
         0 == _wcsicmp ( pwszExt, L".job" ) )
        {
        *pwszExt = '\0';
        bRestoreJOBExt = TRUE;
        }


                                        // Now determine if there's already a
                                        // (x) suffix on the name.
    swprintf ( wszCopyTag, L"(%u)", *puCounter );

    pwszLeftParen = wcsrchr ( wszJobName, '(' );

    if ( NULL != pwszLeftParen  &&
         0 == _wcsicmp ( pwszLeftParen, wszCopyTag ) )
        {
                                        // The tag matched the number we were
                                        // expecting, now check that the tag is
                                        // at the end of the string.
        pwszRightParen = wcsrchr ( wszJobName, ')' );

        if ( NULL != *pwszRightParen  &&
             '\0' == *(1 + pwszRightParen) )
            {
            *pwszLeftParen = '\0';
            }
        }


    swprintf ( wszWork, L"%s(%u)%s",
               wszJobName, ++*puCounter,
               bRestoreJOBExt ? L".job" : L"" );

    wcscpy ( wszJobName, wszWork );

    return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    RemoveDuplicatedParam()
//
// Description:
//  Checks the passed-in list of params, and if the first two tokens
//  (using whitespace as the token separator) are identical, removes one
//  of the copies.
//
// Input:
//  szParams: [in/out] The list of params.  It is modified in-place if
//            two duplicate tokens are present.
//
// Returns:
//  Nothing.
//
// Note:
//  This is needed because of how NPS handles NAVLU command lines - see
//  the NAVLU saving code in CTask::SaveToMTS().
//
//////////////////////////////////////////////////////////////////////////
// 4/17/98 Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

void CTask::RemoveDuplicatedParam ( LPTSTR szParams )
{
TCHAR  szWork   [MAX_PATH];             // all work is done here
TCHAR  szToken1 [MAX_PATH];
TCHAR* pToken;
BOOL   bDuplicateFound = FALSE;

    lstrcpy ( szWork, szParams );

    pToken = _tcstok ( szWork, _T(" ") );

    if ( NULL != pToken )
        {
        lstrcpy ( szToken1, pToken );

        pToken = _tcstok ( NULL, _T(" ") );

        if ( NULL != pToken )
            {
            if ( 0 == lstrcmp ( szToken1, pToken ) )
                {
                bDuplicateFound = TRUE;
                }
            }
        }

    if ( bDuplicateFound )
        {
        lstrcpy ( szWork, szParams );

        pToken = _tcstok ( szWork, _T(" ") );
        pToken = _tcstok ( NULL, _T("") );

        lstrcpy ( szParams, pToken );
        }
}
