// Copyright 1998 Symantec Corporation
//////////////////////////////////////////////////////////////////////////////
// $Header:   S:/CTASKAPI/VCS/MTSStuff.cpv   1.5   28 Aug 1998 15:09:20   JBRENNA  $
//
// Description: Functions for interfacing with the Windows Task Scheduler.
//
// Contains:
//
// See Also: NPSStuff.cpp, CTask.cpp
//
//////////////////////////////////////////////////////////////////////////////
// $Log:   S:/CTASKAPI/VCS/MTSStuff.cpv  $ *
// 
//    Rev 1.5   28 Aug 1998 15:09:20   JBRENNA
// Port changed from QAKN to trunk:
// 1. Rev 1.4.1.1   16 Jul 1998 18:12:36   mdunn
//    Fixed 118626 - Before making a new task, the job name is examined and any
//    characters which aren't valid in a filename are changed.  (See
//    MakeLegalFilename().)
// 
//    Rev 1.4   12 Jun 1998 16:36:40   mdunn
// Added support for enabling/disabling events
// 
//    Rev 1.3   04 May 1998 19:16:36   mdunn
// ReturnMTSScheduleInfo() no longer chokes when it gets a
// TASK_TIME_TRIGGER_MONTHLYDOW trigger.
// 
//    Rev 1.2   18 Apr 1998 17:10:54   mdunn
// Fixed string parsing when adding events (using RemoveDuplicatedParam) so
// that adding NAV LU events to MTS works!
// 
//    Rev 1.1   09 Apr 1998 10:43:58   mdunn
// Stubbed out MTS functions when compiling for Alpha.
// 
//    Rev 1.0   06 Apr 1998 17:37:06   mdunn
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

void MakeLegalFileName ( LPWSTR );
void MakeLegalCmdLineParam ( LPTSTR );

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//
// Function:    MakeLegalFilename()
//
// Description:
//  Takes the passed-in string and removes any characters that are not
//  legal in a filename.  " is replaced with ' and all others are replaced
//  with _
//
// Input:
//  wszName: [in/out] The string to operate on
//
// Returns:
//  Nothing.
//
//////////////////////////////////////////////////////////////////////////
//  7/16/98 Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

void MakeLegalFilename ( LPWSTR wszName )
{
    ASSERT ( wszName != NULL );

    if ( NULL == wszName )
        return;

    for ( ; '\0' != *wszName; wszName++ )
        {
        switch ( *wszName )
            {
            case '"':
                *wszName = '\'';
            break;

            case '/':   case '\\':  case ':':   case '*':
            case '?':   case '<':   case '>':   case '|':
                *wszName = '_';
            break;
            }
        }
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    MakeLegalCmdLineParam()
//
// Description:
//  Changes " to ' in the passed-in string so the string can be used as
//  a quoted command line parameter.
//
// Input:
//  szParam: [in/out] The string to operate on.
//
// Returns:
//  Nothing.
//
//////////////////////////////////////////////////////////////////////////
//  7/16/98 Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

void MakeLegalCmdLineParam ( LPTSTR szParam )
{
    ASSERT ( szParam != NULL );

    if ( NULL == szParam )
        return;

    for ( ; '\0' != *szParam; szParam++ )
        {
        if ( '"' == *szParam )
            *szParam = '\'';
        }
}


//////////////////////////////////////////////////////////////////////////////


BOOL CTask::SaveToMTS ( TASKSAVEACTION nAction )
{
#ifdef _M_ALPHA
    return FALSE;
#else

BOOL          bRetVal    = FALSE;
HRESULT       hr         = S_OK;
IUnknown*     pIU        = NULL;
IPersistFile* pIPF       = NULL;
ITask*        pITask     = NULL;
ITaskTrigger* pITaskTrig = NULL;
DWORD         dwTaskFlags;
DWORD         dwTrigFlags;
WORD          wTrigNumber;
WCHAR         wszJobName [MAX_DESCR];
TCHAR         szTemp [MAX_CMDLINE] = {0};
BYTE          byTypeData;
TCHAR         szFixedParams  [MAX_PATH] = {0};
TCHAR*        pToken;

    USES_CONVERSION;

    ASSERT ( m_nTaskType != CTT_INVALID );
    ASSERT ( m_nFrequency != CTT_INVALID );

    
    if ( !m_bInitialized )
        {
        TRACE("Object wasn't initialized!\n");
        return FALSE;
        }

    if ( m_nTaskType == CTT_RUNAPP &&
         ( NULL == m_pszAppPath || NULL == m_pszDisplayName   ||
           '\0' == *m_pszAppPath || '\0' == *m_pszDisplayName   ) )
        {
        TRACE("Required param not set (app path or job name)\n");
        return FALSE;
        }
       
    if ( m_nTaskType == CTT_MESSAGE  &&
         ( NULL == m_pszMsg || '\0' == *m_pszMsg  ||
           NULL == m_pszDisplayName  || '\0' == *m_pszDisplayName ) )
        {
        TRACE("Required param not set (message text or display name for message event)\n");
        return FALSE;
        }
       
    if ( ( m_nTaskType == CTT_NAVLU   ||
           m_nTaskType == CTT_NAVSCAN || 
           m_nTaskType == CTT_NAVSCANTASK ) &&
         !m_bNAVInstalled )
        {
        TRACE("Can't add a NAV LU or NAV scan job when NAV isn't installed\n");
        return FALSE;
        }

    if ( m_nTaskType == CTT_NAVSCANTASK && 
         ( NULL == m_pszDisplayName || '\0' == *m_pszDisplayName ||
           NULL == m_pszScanTask || '\0' == *m_pszScanTask ) )
        {
        TRACE("Required param not set (display name or scan task name)\n");
        return FALSE;
        }
    

    // Construct the job name.  For NAVSCAN and NAVLU jobs, we use a default
    // name if the client passed a NULL display name or an empty string.

    switch ( m_nTaskType )
        {
        case CTT_NAVSCAN:
            {
            if ( NULL == m_pszDisplayName  ||
                 '\0' == *m_pszDisplayName )
                {
                GetPrivateProfileString (
                    szNAVScanSection, szIniDefDesc,
                    szEventNAVScan, szTemp, MAX_DESCR,
                    szNortonIni );
                                        
                wcscpy ( wszJobName, T2OLE(szTemp) );
                }
            else
                wcscpy ( wszJobName, T2OLE(m_pszDisplayName) );
            }
        break;

        case CTT_NAVLU:
            {
            if ( NULL == m_pszDisplayName  ||
                 '\0' == *m_pszDisplayName )
                {
                GetPrivateProfileString (
                    szNAVLUSection, szIniDefDesc,
                    szEventNAVLU, szTemp, MAX_DESCR,
                    szNortonIni );
                                        
                wcscpy ( wszJobName, T2OLE(szTemp) );
                }
            else
                wcscpy ( wszJobName, T2OLE(m_pszDisplayName) );
            }
        break;

        case CTT_RUNAPP:
        case CTT_MESSAGE:
        case CTT_NAVSCANTASK:
            wcscpy ( wszJobName, T2OLE(m_pszDisplayName) );
        break;

        default:
            ASSERT(0);                  // should never get here
        break;
        }


    // Create a new MTS task.
    // Note that the INetSDK docs say that if the job you add has the same name
    // as an already-existing job, NewWorkItem() will fail. bzzzt!  Wrong, MS, 
    // but thank you for playing.  Nothing fails 'till the call to
    // IPersistFile::Save().  For that reason, we try to activate a task
    // with the given name to determine if it exists.
    // [Added 6/7/99: MS fixed this in IE 5, but this code is still necessary
    //  so we work with IE 4.]

    MakeLegalFilename ( wszJobName );

    hr = m_pITaskScheduler->Activate ( wszJobName, IID_ITask, &pIU );

    if ( SUCCEEDED (hr) )
        {
        pIU->Release();
        pIU = NULL;

        if ( CTSA_FAIL_IF_EXISTS == nAction )
            {
            TRACE("A task with the specified name already exists, and nAction is set to fail. Task not saved.\n");
            goto bailout;
            }
        else if ( CTSA_OVERWRITE_IF_EXISTS == nAction )
            {
            TRACE("A task with the specified name already exists - deleting it.\n");

            hr = m_pITaskScheduler->Delete ( wszJobName );

            if ( FAILED(hr) )
                {
                TRACE("Couldn't delete existing task.\n");
                goto bailout;
                }
            }
        else                            // Create a new name
            {
            UINT uCounter = 0;

            for (;;)
                {
                if ( !CreateNewJobName ( wszJobName, &uCounter ) )
                    {
                    TRACE("Error: Unable to find a unique job name (reached hard-coded limit of 100 copies).\n");
                    goto bailout;
                    }

                hr = m_pITaskScheduler->Activate ( wszJobName, 
                                                   IID_ITask, &pIU );

                if ( SUCCEEDED(hr) )
                    {
                    pIU->Release();
                    pIU = NULL;
                    }
                else
                    {
                    break;              // Found a name that doesn't already exist.
                    }
                }
            }
        }


    hr = m_pITaskScheduler->NewWorkItem ( wszJobName, CLSID_CTask,
                                          IID_ITask, &pIU );

    if ( FAILED(hr) )
        {
        TRACE("Couldn't create new work item.\n");
        goto bailout;
        }


    // We now have an IUnknown pointer.  This is queried for an ITask
    // pointer on the work item we just added.
    
    hr = pIU->QueryInterface ( IID_ITask, (void **) &pITask );

    if ( FAILED (hr) )
        {
        TRACE("Error: IUnknown failed to yield ITask\n");
        goto bailout;
        }
    

    // Cleanup pIUnknown, as we are done with it.
    
    pIU->Release();
    pIU = NULL;


/***** Password stuff not needed on 9x.  We'll need to reinstate this if
       we decide to do MTS stuff on NT.

    //
    // We need to see if we support security, and we
    // do this by calling ITask::SetAccountInformation
    // and checking if the failure code is SCHED_E_NO_SECURITY_SERVICES
    //

    hr = pITask->SetAccountInformation(lpwszUserName, lpwszPassword);
    if (hr != SCHED_E_NO_SECURITY_SERVICES)
    {
    
        // Check to see if username is null

        if (*lpwszPassword == L'\0')
        {
            // If password was null, chance username is, too.

            if (*lpwszUserName == L'\0')
            {
                wprintf(L"Enter username to run job %s as: ",lpwszJobName);
                wscanf(L"%s",lpwszUserName);
                wprintf(L"\n");
            }
    
            wprintf(L"Enter password to user %s: ",lpwszUserName);
            wscanf(L"%s",lpwszPassword);
            wprintf(L"\n");
        }
    }

    // Set the account information using ITask::SetAccountInformation
    // This fails for Win9x, but we ignore the failure.

    hr = pITask->SetAccountInformation(lpwszUserName, lpwszPassword);
    if ((FAILED(hr)) && (hr != SCHED_E_NO_SECURITY_SERVICES))
    {
        wprintf(L"Error: Failed to set credentials on task object %x\n",hr);
        pITask->Release();
        return hr;
    }
*****/


    // Set the app name.

    if ( m_nTaskType == CTT_NAVSCAN )
        {
        GetPrivateProfileString ( szNAVScanSection,
                                  szIniCmdLine,
                                  m_bOSIsNT ? NAVW_EXE_NT : NAVW_EXE_9x,
                                  m_pszAppPath, MAX_CMDLINE,
                                  szNortonIni );
        }
    else if ( m_nTaskType == CTT_NAVLU )
        {
        GetPrivateProfileString ( szNAVLUSection,
                                  szIniCmdLine,
                                  LUALL_EXE,
                                  m_pszAppPath, MAX_CMDLINE,
                                  szNortonIni );
        }
    else if ( m_nTaskType == CTT_MESSAGE )
        {
        GetPrivateProfileString ( CTASK_SECTION,
                                  _T("MessageApp"),
                                  MTS_MESSAGE_DEFAULT_APP,
                                  m_pszAppPath, MAX_CMDLINE,
                                  szNortonIni );
        }
    else if ( m_nTaskType == CTT_NAVSCANTASK )
        {
        wsprintf ( m_pszAppPath, _T("%s\\%s"),
                   m_szNAVDir, NAV_SCAN_HANDLER_EXE );
        }


    // Set the app's parameters.

    if ( m_nTaskType == CTT_MESSAGE )
        {
        // We need to change any quotes to apostrophes in the description
        // and message strings, since they will be on a command line and
        // embedded quotes will cause problems.

        MakeLegalCmdLineParam ( m_pszMsg );
        MakeLegalCmdLineParam ( m_pszDisplayName );

        wsprintf ( szTemp, _T("/m \"%s\" /t \"%s\""),
                   m_pszMsg, m_pszDisplayName );

//        hr = pITask->SetParameters ( T2OLE(szTemp) );
        }
    else if ( m_nTaskType == CTT_NAVSCANTASK )
        {
		// We are saving the full path name of the ScanTask instead of only the ScanTask name.
		// In Odyssey (NAV 2000), it was saved as /s:<taskname>
		// In Turbo (NAV 2001), it is changed to just saving the full path name of the ScanTask.
        wsprintf ( szTemp, _T("\"%s\""), m_pszScanTask );
        }
    else if ( NULL != m_pszParams  &&  '\0' != *m_pszParams )
        {
        lstrcpy ( szTemp, m_pszParams );//&^
//        hr = pITask->SetParameters ( T2OLE(m_pszParams) );
        }
    else if ( m_nTaskType == CTT_NAVSCAN )
        {
                                        // Use default params.
        GetPrivateProfileString (
            szNAVScanSection, szIniDefActionText,
            NAV_SCAN_DEFAULT_PARAMS,
            szTemp, MAX_CMDLINE, szNortonIni );

//        hr = pITask->SetParameters ( T2OLE(szTemp) );
        }
    else if ( m_nTaskType == CTT_NAVLU )
        {
                                        // Use default params.
        GetPrivateProfileString (
            szNAVLUSection, szIniDefActionText,
            NAV_LU_DEFAULT_PARAMS,
            szTemp, MAX_CMDLINE, szNortonIni );

//        hr = pITask->SetParameters ( T2OLE(szTemp) );
        }
//    else
//        {
//        hr = S_OK;                      // no params were given
//        }


    // ***** Time to tweak the app path & param strings.  We need to do
    // this because Atomic's SCHEDULE.INI entries for NAVLU have the app
    // path as "luall -s", with the app name and a param
    // together.  That does Bad Things to MTS since it can't figure out
    // what to do with that string.  So, I parse the app path string
    // and pick off the first token, which becomes the app name, and the
    // remainder gets prepended to the parameters string.
    //
    // ***** This assumes that the SCHEDULE.INI entries for NAVLU and
    // NAV scan events have *NO* spaces in the app name.
    //
    // At this point, m_pszAppPath has the app path, and szTemp holds
    // the params.  Once we're done, the app path (alone) will be in
    // m_pszAppPath and the full list of params will be in szFixedParams.

    if ( m_nTaskType == CTT_NAVLU || m_nTaskType == CTT_NAVSCAN )
        {
                                        // strtok() will insert a NULL char
                                        // after the app name if there are
                                        // any tokens after it.
        pToken = _tcstok ( m_pszAppPath, _T(" ") );

        if ( NULL != pToken )
            {
                                        // Grab the rest of the string,
                                        // which are the cmd line params.
            pToken = _tcstok ( NULL, _T("") );
            
            if ( NULL != pToken )
                {
                lstrcpy ( szFixedParams, pToken );
                lstrcat ( szFixedParams, _T(" ") );
                }
            }

        lstrcat ( szFixedParams, szTemp );

        RemoveDuplicatedParam ( szFixedParams );
        }
    else
        {
        lstrcpy ( m_pszAppPath, m_pszAppPath );
        lstrcpy ( szFixedParams, szTemp );
        }


    // Set the app name.

    hr = pITask->SetApplicationName ( T2OLE(m_pszAppPath) );

    if ( FAILED (hr) )
        {
        TRACE("Error: Failed to set command name (with params)\n"); 
        goto bailout;
        }


    hr = pITask->SetParameters ( T2OLE(szFixedParams) );

    if ( FAILED (hr) )
        {
        TRACE("Error: Failed to set parameters\n");
        goto bailout;
        }
        

    // Set the starting directory

    if ( m_nTaskType == CTT_NAVSCAN  ||  m_nTaskType == CTT_NAVLU  ||
         m_nTaskType == CTT_MESSAGE  ||  m_nTaskType == CTT_NAVSCANTASK )
        {
        hr = pITask->SetWorkingDirectory ( T2OLE(m_szNAVDir) );
        }
	else if ( m_nTaskType == CTT_NAVLU )
	{
		// We're now calling LuAll instead of NavLU32.exe.  LuAll's 
		// directory specified in the SharedUsage key of the registry.
		TCHAR szDirectory[ SYM_MAX_PATH ];
		HKEY hKey= 0;
		DWORD dwSize = sizeof( szDirectory );

        if ( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, 
			                               TEXT("SOFTWARE\\Symantec\\SharedUsage"), 0, 
										   KEY_READ, &hKey ) )
        {
            if ( ERROR_SUCCESS == RegQueryValueEx( hKey, TEXT("LiveUpdate1"), 0, 0, 
				                                  (LPBYTE)szDirectory, &dwSize ) )
            {
		        hr = pITask->SetWorkingDirectory ( T2OLE(szDirectory) );

            }
            RegCloseKey(hKey);
		}
	}
    else if ( NULL != m_pszStartingDir && '\0' != *m_pszStartingDir )
        {
        hr = pITask->SetWorkingDirectory ( T2OLE(m_pszStartingDir) );
        }
    else
        {
        hr = S_OK;
        }

    if ( FAILED(hr) )
        {
        TRACE("Error: Couldn't set starting directory.\n");
        goto bailout;
        }


    // Set the job comment.

    if ( NULL != m_pszComment  &&  '\0' != *m_pszComment )
        {
        hr = pITask->SetComment ( T2OLE(m_pszComment) );

        if ( FAILED (hr) )
            {
            TRACE("Error: Task comment could not be set\n");
            goto bailout;
            }
        }


    // Store the task type so the enumeration functions can get the
    // type quickly.

    byTypeData = m_nTaskType;

    hr = pITask->SetWorkItemData ( 1, &byTypeData );

    if ( FAILED(hr) )
        {
        TRACE("Couldn't store task type!\n");
        goto bailout;
        }


    // Set the flags on the task object
    
                                        // These are the default flags for
                                        // events created via the MTS UI.
    dwTaskFlags = TASK_FLAG_DONT_START_IF_ON_BATTERIES |
                  TASK_FLAG_KILL_IF_GOING_ON_BATTERIES;

    // Note that I _don't_ set TASK_FLAG_DELETE_WHEN_DONE because setting it
    // makes MTS not notify if the event is missed!  It just blows the event
    // away.  How mean.

    if ( m_bOSIsNT )
        {
        dwTaskFlags |= TASK_FLAG_INTERACTIVE;
        }

    if ( !m_bTaskEnabled )
        {
        dwTaskFlags |= TASK_FLAG_DISABLED;
        }

    hr = pITask->SetFlags ( dwTaskFlags );

    if ( FAILED (hr) )
        {
        TRACE("Error: Could not set task flags\n");
        goto bailout;
        }


    // Now, create a trigger to run the task at our specified time.

    hr = pITask->CreateTrigger ( &wTrigNumber, &pITaskTrig );

    if ( FAILED (hr) )
        {
        TRACE("Error: Could not create a new trigger\n");
        goto bailout;
        }


    // Now, fill in the trigger as necessary.

    dwTrigFlags = 0;
    ZeroMemory ( &m_rTrigger, sizeof (TASK_TRIGGER) );

    m_rTrigger.cbTriggerSize = sizeof (TASK_TRIGGER);
    m_rTrigger.wBeginYear    = m_wYear;
    m_rTrigger.wBeginMonth   = m_wMonth;
    m_rTrigger.wBeginDay     = m_wDay;
    m_rTrigger.wStartHour    = m_wHour;
    m_rTrigger.wStartMinute  = m_wMinute;

    FillTriggerScheduleFields();


    // Add this trigger to the task using ITaskTrigger::SetTrigger

    hr = pITaskTrig->SetTrigger ( &m_rTrigger );

    if ( FAILED (hr) )
        {
        TRACE("Error: Failed to set trigger to desired values.\n");
        goto bailout;
        }


    // Save the changes with IPersistFile::Save()

    hr = pITask->QueryInterface ( IID_IPersistFile, (void **) &pIPF );

    if ( FAILED (hr) )
        {
        TRACE("Error: Could not get an IPersistFile interface from the task.\n");
        goto bailout;
        }


    hr = pIPF->Save ( NULL, FALSE );

    if ( FAILED (hr) )
        {
        TRACE("Error: Could not save the task.\n");
        goto bailout;
        }


bailout:
    // Clean up all the OLE stuff.
    
    if ( pITask != NULL )
        {
        pITask->Release();
        }

    if ( pITaskTrig != NULL )
        {
        pITaskTrig->Release();
        }

    if ( pIPF != NULL )
        {
        pIPF->Release();
        }

    return SUCCEEDED (hr);

#endif  // ndef _M_ALPHA
}


// This fn fills the TASK_TRIGGER member variable according to the CTF_* 
// frequency.
void CTask::FillTriggerScheduleFields()
{
#ifndef _M_ALPHA

    switch ( m_nFrequency )
        {
        case CTF_HOURLY:
            {
            m_rTrigger.TriggerType = TASK_TIME_TRIGGER_DAILY;
            m_rTrigger.Type.Daily.DaysInterval = 1;

                                        // Repeat for 23 hrs, 59 min.
            m_rTrigger.MinutesDuration = 24*60 - 1;

                                        // Run every hour.
            m_rTrigger.MinutesInterval = 60;
            }
        break;

        case CTF_DAILY:
            {
            m_rTrigger.TriggerType = TASK_TIME_TRIGGER_DAILY;

            m_rTrigger.Type.Daily.DaysInterval = 1;
            }
        break;

        case CTF_WEEKLY:
            {
            m_rTrigger.TriggerType = TASK_TIME_TRIGGER_WEEKLY;

            m_rTrigger.Type.Weekly.WeeksInterval = 1;

            m_rTrigger.Type.Weekly.rgfDaysOfTheWeek =
                s_awDayFlags [ m_wDayOfWeek ];
            }
        break;

        case CTF_WEEKDAYS:
            {
            m_rTrigger.TriggerType = TASK_TIME_TRIGGER_WEEKLY;

            m_rTrigger.Type.Weekly.WeeksInterval = 1;

            m_rTrigger.Type.Weekly.rgfDaysOfTheWeek =
                TASK_MONDAY   | TASK_TUESDAY | TASK_WEDNESDAY |
                TASK_THURSDAY | TASK_FRIDAY;
            }
        break;

        case CTF_MONTHLY:
        case CTF_TWICEMONTHLY:
            {
            m_rTrigger.TriggerType = TASK_TIME_TRIGGER_MONTHLYDATE;

            m_rTrigger.Type.MonthlyDate.rgfDays = 1 << (m_wDay - 1);

            m_rTrigger.Type.MonthlyDate.rgfMonths =
                TASK_JANUARY | TASK_FEBRUARY | TASK_MARCH | TASK_APRIL | TASK_MAY |
                TASK_JUNE | TASK_JULY | TASK_AUGUST | TASK_SEPTEMBER | TASK_OCTOBER |
                TASK_NOVEMBER | TASK_DECEMBER;

            if ( CTF_TWICEMONTHLY == m_nFrequency )
                {
                m_rTrigger.Type.MonthlyDate.rgfDays |= 1 << (m_wDay2 - 1);
                }
            }
        break;

        case CTF_YEARLY:
            {
            m_rTrigger.TriggerType = TASK_TIME_TRIGGER_MONTHLYDATE;

            m_rTrigger.Type.MonthlyDate.rgfDays = 1 << (m_wDay - 1);

            m_rTrigger.Type.MonthlyDate.rgfMonths =
                s_awMonthFlags [ m_wMonth - 1 ];
            }
        break;

        case CTF_ONCE:
            {
            m_rTrigger.TriggerType = TASK_TIME_TRIGGER_ONCE;
            }
        break;

        default:
            {
            ASSERT(0);                  // should never get here
            }
        break;
        }   // end switch

#endif  // ndef _M_ALPHA
}


void CTask::ReturnMTSScheduleInfo ( const TASK_TRIGGER* pTrig,
                                    SYMTASKREC*         pInfo )
{
#ifndef _M_ALPHA

    switch ( pTrig->TriggerType )
        {
        case TASK_TIME_TRIGGER_DAILY:
            {
            // Two cases here - if the trigger is set to repeat every
            // 60 minutes, then we'll call it an hourly event.

            if ( 60 == pTrig->MinutesInterval )
                {
                pInfo->nFrequency = CTF_HOURLY;
                }
            else
                {
                pInfo->nFrequency = CTF_DAILY;
                }
            }
        break;

        case TASK_TIME_TRIGGER_WEEKLY:
            {
            // We need to get the wDayOfWeek member of the SYSTIME member
            // of the SYMTASKREC.  Note that this only supports weekly events
            // occuring on _one_ day each week.  If an event occurs on 
            // every day M-F, then it's a "weekdays" event.
            
            if ( ( TASK_MONDAY | TASK_TUESDAY | TASK_WEDNESDAY |
                       TASK_THURSDAY | TASK_FRIDAY ) ==
                 ( pTrig->Type.Weekly.rgfDaysOfTheWeek &
                        ( TASK_MONDAY | TASK_TUESDAY | TASK_WEDNESDAY |
                          TASK_THURSDAY | TASK_FRIDAY ) ) )
                {
                pInfo->nFrequency = CTF_WEEKDAYS;
                }
            else
                {
                pInfo->nFrequency = CTF_WEEKLY;

                for ( int i = 0; i <= 6; i++ )
                    {
                    if ( pTrig->Type.Weekly.rgfDaysOfTheWeek & s_awDayFlags[i] )
                        {
                        pInfo->wDayOfWeek = i;
                        break;
                        }
                    }
                }
            }
        break;

        case TASK_TIME_TRIGGER_MONTHLYDATE:
            {
            // This one's a bit weird because monthly and yearly events both
            // have type MONTHLYDATE.  We first have to determine if the event
            // is monthly or yearly, by counting how many months the event occurs
            // in.  If it occurs in more than 1, then assume it's monthly and
            // occurs in all 12 months.

            int i;                      // loop counter
            int nNumMonths;

            for ( i = 0, nNumMonths = 0; i <= 11; i++ )
                {
                if ( s_awMonthFlags[i] ==
                       (pTrig->Type.MonthlyDate.rgfMonths & s_awMonthFlags[i]) )
                    {
                    nNumMonths++;

                    if ( nNumMonths > 1 )
                        break;
                    }
                }

            ASSERT ( 0 != nNumMonths );


            if ( nNumMonths > 1 )
                {
                                        // This is a monthly event.
                pInfo->nFrequency = CTF_MONTHLY;
        
                                        // Determine the day of the month the
                                        // event runs on.
                for ( i = 0; i <= 30; i++ )
                    {
                    if ( pTrig->Type.MonthlyDate.rgfDays & ( 1 << i ) )
                        {
                        pInfo->rStartTime.wDay = i + 1;
                        break;
                        }
                    }

                i++;
                                        // Check for a second day, for twice-
                                        // monthly events.
                for ( ; i <= 30; i++ )
                    {
                    if ( pTrig->Type.MonthlyDate.rgfDays & ( 1 << i ) )
                        {
                        pInfo->nFrequency = CTF_TWICEMONTHLY;
                        pInfo->wDay2 = i + 1;
                        break;
                        }
                    }
                }   // end if (nNumMonths == 1)
            else
                {
                pInfo->nFrequency = CTF_YEARLY;

                                        // Determine which month the event
                                        // runs on.
                for ( i = 0; i <= 11; i++ )
                    {
                    if ( pTrig->Type.MonthlyDate.rgfMonths & s_awMonthFlags[i] )
                        {
                        pInfo->rStartTime.wMonth = i + 1;
                        break;
                        }
                    }

                                        // And the day....
                for ( i = 0; i <= 30; i++ )
                    {
                    if ( pTrig->Type.MonthlyDate.rgfDays & ( 1 << i ) )
                        {
                        pInfo->rStartTime.wDay = i + 1;
                        break;
                        }
                    }
                }   // end else
            }
        break;

        case TASK_TIME_TRIGGER_ONCE:
            pInfo->nFrequency = CTF_ONCE;
            // Nothing more to do here, it was already done in NextMTSTask().
        break;

        case TASK_TIME_TRIGGER_MONTHLYDOW:
            // CTask never creates a task with this type of trigger, so I
            // won't do anything with it.  Currently, the only time this
            // type is encountered is when install/NAVLU enumerates events
            // looking for NAV tasks.  In those cases, the task type
            // won't be CTT_NAVSCAN or CTT_NAVLU so nothing will be done
            // to the task anyway and I can just do nothing here.
        break;

        default:
            ASSERT(0);                  // should never get here
        break;
        }   // end switch

#endif  // ndef _M_ALPHA
}


BOOL CTask::FindMTSTask ( SYMTASKREC* pTaskInfo )
{
#ifdef _M_ALPHA
    return FALSE;
#else

BOOL            bRetVal = FALSE;
IEnumWorkItems* pIEnum = NULL;
SYMTASKREC      rTaskInfo;
TCHAR           szWork [ MAX_DESCR ];
LPTSTR          szExt;

    ASSERT ( !m_bNPSMode );
    ASSERT ( m_pITaskScheduler != NULL );
    ASSERT ( pTaskInfo != NULL );
    ASSERT ( *pTaskInfo->szDisplayName != '\0' );

    if ( !m_bInitialized )
        {
        TRACE("Task object wasn't initialized.\n");
        return FALSE;
        }

    if ( NULL == pTaskInfo  ||
         '\0' == *pTaskInfo->szDisplayName )
        {
        TRACE("Required param in task rec not set (rec pointer or display name was empty)\n");
        return FALSE;
        }

    if ( !EnumMTSTasks ( &pIEnum ) )
        {
        TRACE("CTask::EnumMTSTasks() returned FALSE.\n");
        return FALSE;
        }

                                        // We need to add ".job" to the
                                        // search name if it isn't there,
                                        // since the task names that MTS
                                        // returns have a ".job" extension.
    lstrcpy ( szWork, rTaskInfo.szDisplayName );

    szExt = _tcsrchr ( pTaskInfo->szDisplayName, '.' );

    if ( NULL != szExt  &&
         0    != _tcsnicmp ( szExt, _T(".job"), 4 ) )
        {
        lstrcat ( szWork, _T(".job") );
        }


    while ( NextMTSTask ( pIEnum, &rTaskInfo ) ) 
        {
        if ( 0 == lstrcmpi ( szWork,
                             pTaskInfo->szDisplayName ) )
            {
                                        // Found a match!
            CopyMemory ( pTaskInfo, &rTaskInfo, sizeof(SYMTASKREC) );
        
            bRetVal = TRUE;
            break;
            }
        }

    pIEnum->Release();

    return bRetVal;

#endif  // ndef _M_ALPHA
}


BOOL CTask::EnumMTSTasks ( IEnumWorkItems** ppIEnum )
{
#ifdef _M_ALPHA
    return FALSE;
#else

BOOL      bRetVal      = FALSE;
HRESULT   hr           = S_OK;

    ASSERT ( !m_bNPSMode );
    ASSERT ( NULL != m_pITaskScheduler );

    if ( NULL == m_pITaskScheduler )
        return FALSE;

    if ( !m_bInitialized )
        {
        TRACE("Task object not initialized.\n");
        return FALSE;
        }
        

    // Get an enumeration interface, using ITaskScheduler::Enum

    hr = m_pITaskScheduler->Enum ( ppIEnum );

    if ( FAILED (hr) )
        {
        TRACE("Failed to get enumerator\n");
        *ppIEnum = NULL;
        }
    else
        {
        bRetVal = TRUE;
        }

    return bRetVal;

#endif  // ndef _M_ALPHA
}


BOOL CTask::NextMTSTask ( IEnumWorkItems* pIEnum, SYMTASKREC* pInfo )
{
#ifdef _M_ALPHA
    return FALSE;
#else

BOOL          bRetVal      = FALSE;
HRESULT       hr;
IUnknown*     pIU          = NULL;
ITask*        pITask       = NULL;
ITaskTrigger* pITrig       = NULL;
LPWSTR*       pwszNames    = NULL;
TCHAR*        pExtension;
LPWSTR        wszWork      = NULL;
WORD          wTrigCount   = 0;
TASK_TRIGGER  rTrigger     = {sizeof(TASK_TRIGGER)};
WORD          wDataSize;
BYTE*         pbyTaskData;
DWORD         dwTaskFlags;

    USES_CONVERSION;

    ASSERT ( !m_bNPSMode );
    ASSERT ( NULL != m_pITaskScheduler );

    if ( !m_bInitialized )
        {
        TRACE("Task object not initialized.\n");
        return FALSE;
        }
      

    // Get a single work item, using IEnumWorkItems::Next

    hr = pIEnum->Next ( 1, &pwszNames, NULL );

    if ( hr == S_FALSE )
        {
        TRACE("IEnumWorkItems::Next() returned S_FALSE - no more items.\n");
        goto bailout;
        }

#ifdef _DEBUG
    TRACE1("Enumerating job \"%s\"\n", OLE2T(pwszNames[0]));
#endif


    // Attach to the work item, using ITaskScheduler::Activate

    hr = m_pITaskScheduler->Activate ( pwszNames[0], IID_ITask, &pIU );

    if ( FAILED (hr) )
        {
        TRACE("Error: ITaskScheduler::Activate() failed\n");
        goto bailout;
        }


    // Query the returned IUnknown for an ITask

    hr = pIU->QueryInterface ( IID_ITask, (void **) &pITask );

    pIU->Release();
    pIU = NULL;

    if ( FAILED (hr) )
        {
        TRACE("Error: QueryInterface() for ITask failed.\n");
        goto bailout;
        }


                                        // Read the task type from the work
                                        // item's data area.  If the data isn't
                                        // there or is a number we don't recognize,
                                        // then the type is set to CTT_NOT_NAV,
                                        // indicating NAV's UI can skip it.
    hr = pITask->GetWorkItemData ( &wDataSize, &pbyTaskData );

    if ( FAILED(hr) )
        {
        pInfo->nTaskType = CTT_NOT_NAV;
        }
    else 
        {
        if ( wDataSize != 1  ||  *pbyTaskData > CTT_LAST )
            {
            pInfo->nTaskType = CTT_NOT_NAV;
            CoTaskMemFree ( (void*) pbyTaskData );
            }
        else
            {
            pInfo->nTaskType = (TASKTYPE) *pbyTaskData;
            }

        CoTaskMemFree ( (void*) pbyTaskData );
        }


    // Use ITask::GetTriggerCount to get count of triggers

    hr = pITask->GetTriggerCount(&wTrigCount);

    if ( FAILED (hr) )
        {
        TRACE("Error: Failed to count triggers\n");
        goto bailout;
        }
    
    if ( wTrigCount == 0 )
        {
        TRACE("Event has no triggers - bailing out\n");
        goto bailout;
        }

#ifdef _DEBUG
    if ( wTrigCount > 1 )
        {
                                        // Not really an error, but for now I'm
                                        // only supporting one trigger per event,
                                        // so show a trace message.
        TRACE("Warning: Event has more than one trigger - all triggers after the first will be ignored.\n");
        }
#endif


    // Get the task's first trigger

    hr = pITask->GetTrigger ( 0, &pITrig );

    if ( FAILED (hr) )
        {
        TRACE("Error: Failed to get trigger\n");
        goto bailout;
        }

    hr = pITrig->GetTrigger ( &rTrigger );

    if ( FAILED(hr) )
        {
        TRACE("Call to GetTrigger() failed.\n");
        goto bailout;
        }


    // Copy all the supported fields to the SYMTASKREC.

    if ( NULL != pInfo->szDisplayName )
        {
        lstrcpyn ( pInfo->szDisplayName, OLE2T(pwszNames[0]), MAX_DESCR );
        }


    // The task name will probably have ".job" tacked on to the end, so remove
    // it if it's there.

    pExtension = _tcsrchr ( pInfo->szDisplayName, '.' );

    if ( NULL != pExtension  &&
         0 == _tcsnicmp ( pExtension, _T(".job"), 4 ))
        {
        *pExtension = '\0';
        }


    if ( NULL != pInfo->szAppPath )
        {
        hr = pITask->GetApplicationName ( &wszWork );

        if ( FAILED(hr) )
            {
            TRACE("ITask::GetApplicationName() failed\n");
            goto bailout;
            }

        lstrcpyn ( pInfo->szAppPath, OLE2T(wszWork), MAX_PATH );

        CoTaskMemFree ( wszWork );
        wszWork = NULL;
        }

                                        //&^ !!! Pick off /m and store it in
                                        // m_pszMsg for message events
                                        //&^ Pick off /s:<task> for scheduled
                                        // scan tasks.
    if ( NULL != pInfo->szParams )
        {
        hr = pITask->GetParameters ( &wszWork );

        if ( FAILED(hr) )
            {
            TRACE("ITask::GetParameters() failed\n");
            goto bailout;
            }

        lstrcpyn ( pInfo->szParams, OLE2T(wszWork), MAX_PATH );

        CoTaskMemFree ( wszWork );
        wszWork = NULL;


        TCHAR  szParams [MAX_CMDLINE];
        TCHAR* pSwitch;
        TCHAR* pMsgText;
        TCHAR* pScanTaskName;

        // Make a copy of the params that we can mess with.
        lstrcpy ( szParams, pInfo->szParams );


        if ( CTT_MESSAGE == pInfo->nTaskType )
            {
            // For message events, pick off the text of the message, which will
            // follow the /m switch.

            // Locate the /m switch.
            pSwitch = _tcsstr ( szParams, _T("/m "));

            if ( NULL != pSwitch )
                {
                // Find the opening quote of the message text.
                pMsgText = _tcschr ( pSwitch, '"' );

                if ( NULL != pMsgText )
                    {
                    // Pick out the text between the quotes.
                    pMsgText = _tcstok ( pMsgText, _T("\"") );

                    if ( NULL != pMsgText )
                        {
                        // and copy it to the task rec.
                        lstrcpy ( pInfo->szMsg, pMsgText );
                        }
                    }
                }
            }
        else if ( CTT_NAVSCANTASK == pInfo->nTaskType )
            {
            // Locate the /s switch. (Odyssey used the /s switch)
			// No longer using the /s switch for ScanTasks, just the
			// full path name of the ScanTask.
            pSwitch = _tcsstr ( szParams, _T("\"") );

            if ( NULL != pSwitch )
                {
                // Everything after the first quote, up to the closing quote,
                // is the task name.

                pScanTaskName = _tcstok ( &pSwitch[0], _T("\"") );

                if ( NULL != pScanTaskName )
                    {
                    lstrcpy ( pInfo->szScanTask, pScanTaskName );
                    }
                }
            }
        }   // end if (NULL != pInfo->szParams)


    if ( NULL != pInfo->szStartingDir )
        {
        hr = pITask->GetWorkingDirectory ( &wszWork );

        if ( FAILED(hr) )
            {
            TRACE("ITask::GetWorkingDirectory() failed\n");
            goto bailout;
            }

        lstrcpyn ( pInfo->szStartingDir, OLE2T(wszWork), MAX_PATH );

        CoTaskMemFree ( wszWork );
        wszWork = NULL;
        }


    if ( NULL != pInfo->szComment )
        {
        hr = pITask->GetComment ( &wszWork );

        if ( FAILED(hr) )
            {
            TRACE("ITask::GetComment() failed\n");
            pInfo->szComment[0] = '\0'; // clear out the comment field
            goto bailout;
            }

        lstrcpyn ( pInfo->szComment, OLE2T(wszWork), MAX_COMMENT_LENGTH );

        CoTaskMemFree ( wszWork );
        wszWork = NULL;
        }


    hr = pITask->GetFlags ( &dwTaskFlags );

    if ( FAILED(hr) )
        {
        TRACE("ITask::GetFlags() failed.\n");
        goto bailout;
        }

    pInfo->bEnabled = (dwTaskFlags & TASK_FLAG_DISABLED) ? FALSE : TRUE;



    hr = pITask->GetMostRecentRunTime ( &pInfo->rLastRunTime );

    if ( FAILED(hr) )
        {
        TRACE("ITask::GetMostRecentRunTime() failed\n");
        goto bailout;
        }

    hr = pITask->GetNextRunTime ( &pInfo->rNextRunTime );

    if ( FAILED(hr) )
        {
        TRACE("ITask::GetNextRunTime() failed\n");
        goto bailout;
        }

    ZeroMemory ( &pInfo->rStartTime, sizeof(SYSTEMTIME) );

    pInfo->rStartTime.wYear   = rTrigger.wBeginYear;
    pInfo->rStartTime.wMonth  = rTrigger.wBeginMonth;
    pInfo->rStartTime.wDay    = rTrigger.wBeginDay;
    pInfo->rStartTime.wHour   = rTrigger.wStartHour;
    pInfo->rStartTime.wMinute = rTrigger.wStartMinute;

    ReturnMTSScheduleInfo ( &rTrigger, pInfo );

    // We don't do audible alerts with MTS.
    pInfo->bAudibleAlert = FALSE;

    bRetVal = TRUE;

bailout:
    if ( NULL != pwszNames )
        {
        // Clean up each element in the array of job names, then
        // clean up the final array.
        CoTaskMemFree ( pwszNames[0] );
        CoTaskMemFree ( pwszNames );
        }

    if ( NULL != wszWork )
        {
        CoTaskMemFree ( wszWork );
        }

    if ( NULL != pITask )
        {
        pITask->Release();
        }

    return bRetVal; 

#endif  // ndef _M_ALPHA
}


BOOL CTask::DeleteMTSTask ( const SYMTASKREC* pTask,
                            BOOL  bDeleteByName )
{
#ifdef _M_ALPHA
    return FALSE;
#else

    ASSERT ( NULL != m_pITaskScheduler );
    ASSERT ( NULL != pTask );

    if ( bDeleteByName )
        {
        return DeleteMTSTask ( pTask->szDisplayName );
        }
    else
        {
        return DeleteMTSTask ( pTask->nTaskType );
        }

#endif  // ndef _M_ALPHA
}


BOOL CTask::DeleteMTSTask ( TASKTYPE nType )
{
#ifdef _M_ALPHA
    return FALSE;
#else

BOOL            bRetVal = FALSE;
SYMTASKREC      rTaskInfo;
IEnumWorkItems* pIEnum = NULL;

    ASSERT ( NULL != m_pITaskScheduler );

    if ( EnumMTSTasks ( &pIEnum ) )
        {
        while ( NextMTSTask ( pIEnum, &rTaskInfo ) )
            {
            if ( rTaskInfo.nTaskType == nType )
                {
                bRetVal = DeleteMTSTask ( rTaskInfo.szDisplayName );
                break;
                }
            }

        pIEnum->Release();
        }

    return bRetVal;

#endif  // ndef _M_ALPHA
}


BOOL CTask::DeleteMTSTask ( LPCSTR szJobName )
{
#ifdef _M_ALPHA
    return FALSE;
#else

HRESULT hr;

    USES_CONVERSION;

    ASSERT ( NULL != m_pITaskScheduler );
    ASSERT ( NULL != szJobName );

    hr = m_pITaskScheduler->Delete ( A2OLE(szJobName) );

    return SUCCEEDED(hr);

#endif  // ndef _M_ALPHA
}


BOOL CTask::DeleteMTSTask ( LPCWSTR wszJobName )
{
#ifdef _M_ALPHA 
    return FALSE;
#else

HRESULT hr;

    ASSERT ( NULL != m_pITaskScheduler );
    ASSERT ( NULL != wszJobName );

    hr = m_pITaskScheduler->Delete ( wszJobName );

    return SUCCEEDED(hr);

#endif  // ndef _M_ALPHA
}
