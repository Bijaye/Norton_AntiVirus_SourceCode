// ScanTaskObj.cpp : Implementation of CNAVScanTask
#include "stdafx.h"

#include "ScanTask.h"

#include "NAVTasks.h"
#include "ScanTaskObj.h"
#include "comdef.h"
#include "NAVInfo.h"        // Toolbox
#include "OSInfo.h"         // Toolbox
#include <time.h>
#include "TaskWizard.h"
#include "NAVTrust.h"
#include "AVccModuleId.h"
#include "AVRESBranding.h"
#include "NAVErrorResource.h"
#include "ccResourceLoader.h"
#include "switches.h"

// Resource Loader
extern cc::CResourceLoader g_ResLoader;

//
// Our default task flags
//

#define SCHEDULE_BASE_FLAGS TASK_FLAG_RUN_ONLY_IF_LOGGED_ON 

// helper fn
BOOL ShellExecuteAndWait (LPCTSTR lpVerb, LPCTSTR lpFile, LPCTSTR lpParams)
{
    SHELLEXECUTEINFO seinfo;
    memset (&seinfo, 0, sizeof(seinfo));
    seinfo.cbSize = sizeof(SHELLEXECUTEINFO);
    seinfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    seinfo.hwnd = NULL;
    seinfo.lpVerb = lpVerb;
    seinfo.lpFile = lpFile;
    seinfo.lpParameters = lpParams;
    seinfo.lpDirectory = NULL;
    seinfo.nShow = SW_SHOW;

    BOOL bRet = ShellExecuteEx(&seinfo);
    if (bRet)
    {
        StahlSoft::WaitForSingleObjectWithMessageLoop(seinfo.hProcess,INFINITE);
        ::CloseHandle(seinfo.hProcess);
    }

    return bRet;
}


/////////////////////////////////////////////////////////////////////////////
// CNAVScanTask
CNAVScanTask::CNAVScanTask(void)
{
    CComBSTR bstrErrorClass ("NAVError.NAVCOMError");

    // Check NAVError module for Symantec Signature...
    if( NAVToolbox::NAVTRUST_OK == NAVToolbox::IsSymantecComServer(_T("NAVError.NAVCOMError")) )
    {
        if ( FAILED ( m_spError.CoCreateInstance( bstrErrorClass,
                                                  NULL,
                                                  CLSCTX_INPROC_SERVER)))
        {
        }
    }
	CBrandingRes BrandRes;
	m_csProductName = BrandRes.ProductName();
}

CNAVScanTask::~CNAVScanTask(void)
{
}

HRESULT CNAVScanTask::FinalConstruct(void)
{
    HRESULT hr = E_FAIL;
    if( SYM_OK == m_TaskLoader.Initialize(_Module.GetModuleInstance()) )
    {
        if( SYM_OK == m_TaskLoader.CreateObject(IID_IScanTask, IID_IScanTask, (void**)&m_pScanTask) )
            hr = S_OK;
    }

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// Properties
/////////////////////////////////////////////////////////////////////////////
// TaskName
STDMETHODIMP CNAVScanTask::put_TaskName(/*[in]*/ BSTR bstrTaskName)
{
	try
    {
        // Maks sure its a valid file name
	    if (forceError (ERR_INVALID_ARG_PUT_TASKNAME) || !bstrTaskName || !*bstrTaskName)
        {
            CCTRACEI ( "Inavlid arg in put_TaskName" );
            makeGenericError ( ERR_INVALID_ARG_PUT_TASKNAME, E_INVALIDARG, IDS_NAVERROR_INTERNAL );
            return E_INVALIDARG;
        }

        _bstr_t bstrTask = bstrTaskName;

        if ( !forceError (ERR_ERROR_PUT_TASKNAME) && m_pScanTask->SetName ( bstrTask ))
            return S_OK;
        else
        {
            CCTRACEI ( "Error in put_TaskName" );
            makeGenericError ( ERR_ERROR_PUT_TASKNAME, E_FAIL, IDS_NAVERROR_INTERNAL );
            return E_FAIL;
        }
    }
    catch ( _com_error e )
    {
        return e.Error ();
    }
}

STDMETHODIMP CNAVScanTask::get_TaskName(/*[out, retval]*/ BSTR *pbstrTaskName)
{

    if (forceError (ERR_INVALID_POINTER) || !pbstrTaskName)
    {
        CCTRACEI ( "Inavlid pointer in get_TaskName" );
        makeGenericError ( ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL );
        return E_POINTER;
    }

    *pbstrTaskName = T2BSTR ( m_pScanTask->GetName ());

	return S_OK;
}

// TaskType
STDMETHODIMP CNAVScanTask::get_TaskType(/*[out, retval]*/ long *peTaskType)
{
    if (forceError (ERR_INVALID_POINTER) || !peTaskType)
    {
        CCTRACEI ( "Inavlid pointer in get_TaskType" );
        makeGenericError ( ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL );
        return E_POINTER;
    }

	*peTaskType = m_pScanTask->GetType();

	return S_OK;
}

// Scheduled ?
STDMETHODIMP CNAVScanTask::get_Scheduled(/*[out, retval]*/ EScheduled *peScheduled)
{
    USES_CONVERSION;

    // Check the pointer
    //
    if (forceError (ERR_INVALID_POINTER) || !peScheduled)
    {
        CCTRACEI ( "Inavlid pointer in get_Scheduled" );
        makeGenericError ( ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }
    
    // Set it to Not Available by default
    //
    *peScheduled = EScheduled_na;
    
    // If you CAN'T schedule it, it must not be available. Duh.
    //
    if ( !m_pScanTask->GetCanSchedule() )
    {
        return S_OK;
    }

	CComPtr<ITaskScheduler> spTaskScheduler;

    if ( forceError (ERR_NO_SCHEDULER) ||
         FAILED ( spTaskScheduler.CoCreateInstance( CLSID_CTaskScheduler,
                  NULL,
                  CLSCTX_INPROC_SERVER)))
    {
        CCTRACEI ( "Failure : CoCreateInstance (CLSID_CTaskScheduler) in get_Scheduled" );
        makeGenericError ( ERR_NO_SCHEDULER, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }
    
    HRESULT hr = S_OK;
    IUnknown* pUnkTask = NULL;
    CComQIPtr <ITask> spITask;
    CComBSTR bstrScheduleName;
    getScheduleName ( bstrScheduleName );

	// Check if a scheduler file exist for this task
	if (SUCCEEDED(hr = spTaskScheduler->Activate( bstrScheduleName,
                                                  IID_ITask,
		                                          (&pUnkTask))))
    {
        spITask = pUnkTask;
        pUnkTask->Release ();
        CComPtr <ITaskTrigger> spITaskTrigger;

        // Get the first trigger time, if there is none count this as "not scheduled".
        //
        if ( FAILED ( spITask->GetTrigger ( 0, &spITaskTrigger )))
        {
    		// Not scheduled
	    	*peScheduled = EScheduled_no;
            hr = S_OK;
        }
        else
        {
    		// Scheduled
	    	*peScheduled = EScheduled_yes;
            hr = S_OK;
        }
    }
	else if (E_INVALIDARG == hr ||
             HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) == hr )
    {
		// Not scheduled
		*peScheduled = EScheduled_no;
		hr = S_OK;
	}
    else if ( HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED) == hr )
    {
		// Scheduled by some other user (other admin for My Computer)
        //
        *peScheduled = EScheduled_na;
		hr = S_OK;
    }

    if ( forceError (ERR_UNKNOWN_GET_SCHEDULED))
        hr = E_FAIL;

    if ( FAILED (hr))
    {
        CCTRACEI ( "Failure in get_Scheduled" );
        makeGenericError ( ERR_UNKNOWN_GET_SCHEDULED, E_FAIL, IDS_NAVERROR_INTERNAL );
    }

	return hr;
}

// LastRunTime
STDMETHODIMP CNAVScanTask::get_LastRunTime(/*[out, retval]*/ DATE *pdLastRunTime)
{
   
    time_t  timeCreationTime;

	if (forceError (ERR_INVALID_POINTER) || !pdLastRunTime)
    {
        CCTRACEI ( "Invalid pointer in get_LastRunTime" );
        makeGenericError ( ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL );
        return E_POINTER;
    }

	m_pScanTask->GetLastRunTime(timeCreationTime);

    struct tm* pTime;

    pTime = localtime(&timeCreationTime);

    timeCreationTime = mktime ( pTime );

    // Sanity check the result from mktime. On Brazilian mktime (0) returns 3600. Why???
    // Defect # 364009
    //
    if ( 100000 >= timeCreationTime )
        return S_FALSE;
    //
    // The following tm to DATE conversion routine has been lovingly
    // borrowed from MFC - OLEVAR.CPP - _AfxOleDateFromTm
    //
    // The unixTimeToSystemTime from MSDN was found to be inaccurate.
    //

    // One-based array of days in year at month start
    //
    int aiMonthDays[13] =
	    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

    WORD wYear = pTime->tm_year+1900;
    WORD wMonth = pTime->tm_mon + 1;

	//  Check for leap year and set the number of days in the month
	BOOL bLeapYear = ((wYear & 3) == 0) &&
		((wYear % 100) != 0 || (wYear % 400) == 0);

	int nDaysInMonth =
		aiMonthDays[wMonth] - aiMonthDays[(wMonth)-1] +
		((bLeapYear && pTime->tm_mday == 29 && wMonth == 2) ? 1 : 0);

	// Cache the date in days and time in fractional days
    //
	long nDate;
	double dblTime;
    double dtDest;

	//It is a valid date; make Jan 1, 1AD be 1
    //
	nDate = wYear*365L + wYear/4 - wYear/100 + wYear/400 +
		aiMonthDays[wMonth-1] + pTime->tm_mday;

	//  If leap year and it's before March, subtract 1:
	if (wMonth <= 2 && bLeapYear)
		--nDate;

	//  Offset so that 12/30/1899 is 0
	nDate -= 693959L;

	dblTime = (((long)pTime->tm_hour * 3600L) +  // hrs in seconds
		((long)pTime->tm_min * 60L) +  // mins in seconds
		((long)pTime->tm_sec)) / 86400.;

	dtDest = (double) nDate + ((nDate >= 0) ? dblTime : -dblTime);

    if ( !forceError (ERR_UNKNOWN_GET_LASTRUNTIME) && NULL != dtDest )
    {
        *pdLastRunTime = dtDest;
    	return S_OK;
    }

    CCTRACEI ( "Failure in get_LastRunTime" );
    makeGenericError ( ERR_UNKNOWN_GET_LASTRUNTIME, E_FAIL, IDS_NAVERROR_INTERNAL );
    return E_FAIL;
}

/////////////////////////////////////////////////////////////////////////////
// Methods
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CNAVScanTask::Scan(bool bWait)
{
    if ( !forceError (ERR_INVALID_SCAN_PATH) && (0 != _tcslen(m_pScanTask->GetPath())) )
    {
        // Command line is c:\progra~1\norton~1\navw32.exe /task:"c:\...\task.sca"
        //
        std::string strExePath;
        std::string strParameters;

        CNAVInfo NAVInfo;
    
        TCHAR szShortNAVDir [MAX_PATH] = {0};

        ::GetShortPathName ( NAVInfo.GetNAVDir (), szShortNAVDir, MAX_PATH );
        strExePath = szShortNAVDir;
        strExePath += _T("\\navw32.exe");

		SCANTASKTYPE scanType = m_pScanTask->GetType();
		if((scanComputer == scanType) 
			|| (scanRemovable == scanType)
			|| (scanAFloppy == scanType)
			|| (scanSelectedDrives == scanType)
			|| (scanFolder == scanType)
			|| (scanFiles == scanType)
			|| (scanCustom == scanType))
		{
			strParameters = SWITCH_DISABLE_ERASER_SCAN;
			strParameters += _T(" ");
		}
		
		strParameters += SWITCH_TASKFILE;
		strParameters += _T("\"");
        strParameters += m_pScanTask->GetPath();
        strParameters += _T("\"");

        if (bWait)
        {
            BOOL bSuccess = ShellExecuteAndWait (_T("open"), strExePath.c_str(), strParameters.c_str());
            if (forceError (ERR_FAILED_LAUNCH_SCAN) || !bSuccess)
            {
                CCTRACEI ( "Failed to ShellExecuteAndWait in Scan (%d)", ::GetLastError() );
                makeGenericError ( ERR_FAILED_LAUNCH_SCAN, E_FAIL, IDS_NAVERROR_INTERNAL );
                return E_FAIL;
            }
        }
        else
        {
            HINSTANCE hInst = ::ShellExecute(::GetDesktopWindow(), _T("open")            // Verb
	                                                                , strExePath.c_str()     // File
	                                                                , strParameters.c_str() // Parameters
	                                                                , NULL                  // Directory
	                                                                , SW_SHOW);             // ShowCmd
    	    // Since '.scan' files are associated with a scanning application we can run the file.
	        if (forceError (ERR_FAILED_LAUNCH_SCAN) ||
                HINSTANCE (32) >= hInst)
            {
                CCTRACEI ( "Failed to execute in Scan" );
                makeGenericError ( ERR_FAILED_LAUNCH_SCAN, E_FAIL, IDS_NAVERROR_INTERNAL );
                return E_FAIL;
            }
        }
    }
    else
    {
        CCTRACEI ( "Invalid path in Scan" );
        makeError ( ERR_INVALID_SCAN_PATH, E_FAIL );
        return E_FAIL;
    }

	return S_OK;
}

STDMETHODIMP CNAVScanTask::Schedule( BOOL* pbWeekly )
{
    // If you can't schedule this item, just return
    //    
    if ( !m_pScanTask->GetCanSchedule () )
    {
        CCTRACEI ( "Cannot schedule task" );
        return S_FALSE;
    }

    if (forceError (ERR_INVALID_POINTER) || !pbWeekly )
    {
        CCTRACEI ( "Invalid pointer in Schedule" );
        makeGenericError ( ERR_INVALID_POINTER, E_FAIL, IDS_NAVERROR_INTERNAL );
        return E_FAIL;
    }
    
    EScheduled eScheduled;
    get_Scheduled ( &eScheduled );

    if ( eScheduled == EScheduled_yes )
    {
        // Don't show if they were just scheduling the weekly scan.
        //
        if ( *pbWeekly == TRUE )
            return S_OK;

        if ( !forceError (ERR_FAILED_SHOW_SCHEDULE) && showSchedule () )
            return S_OK;
        else
        {
            CCTRACEI ( "Failed in showSchedule()");
            makeGenericError ( ERR_FAILED_SHOW_SCHEDULE, E_FAIL, IDS_NAVERROR_INTERNAL );
            return E_FAIL;
        }
    }

    // No schedule, so make a new one and show it!
    //
    if ( !forceError (ERR_UNKNOWN_SCHEDULE) && makeSchedule ( *pbWeekly == TRUE ))
    {
        if ( *pbWeekly == TRUE )
            return S_OK;

        // Don't show if they were just scheduling the weekly scan.
        //
        if ( showSchedule () )
            return S_OK;
    }

    CCTRACEI ( "Failed in Schedule");
    makeError ( ERR_UNKNOWN_SCHEDULE, E_FAIL );
    return E_FAIL;
}

STDMETHODIMP CNAVScanTask::Load(void)
{
    if ( forceError (ERR_UNKNOWN_LOAD) || !m_pScanTask->Load( m_pScanTask->GetPath() ))
    {
		CCTRACEI ( "Error in Load" );
        makeGenericError ( ERR_UNKNOWN_LOAD, E_FAIL, IDS_NAVERROR_INTERNAL );
        return E_FAIL;
    }

	return S_OK;
}

STDMETHODIMP CNAVScanTask::Save(void)
{
    if ( forceError (ERR_UNKNOWN_SAVE) || !m_pScanTask->Save())
    {
		CCTRACEI ( "Error in Save" );
        makeError ( ERR_UNKNOWN_SAVE, E_FAIL );
        return E_FAIL;
    }

	return S_OK;
}

STDMETHODIMP CNAVScanTask::Edit(void)
{
    USES_CONVERSION;    // ATL needs this to do OLE2...

    if ( !m_pScanTask->GetCanEdit () )
    {
        CCTRACEI ( "Attempt to Edit uneditable task");
        return S_FALSE;
    }

    CComBSTR bstrTemp;
    getScheduleName ( bstrTemp );

    _bstr_t bstrOldScheduleName ( bstrTemp);
    
    BSTR bstrOldTaskName;
    get_TaskName ( &bstrOldTaskName );
    _bstr_t bstOldTaskName (bstrOldTaskName,false);

    HRESULT hr = E_FAIL;

    int iMode = 1;
    bool bDuplicate = false;

    do
    {
        CTaskWizard NTW;
       
        hr = NTW.NAVTaskWizard (m_pScanTask, iMode);

        if ( forceError (ERR_UNKNOWN_EDIT) )
            hr = E_FAIL;

        if ( hr == S_OK )
        {
            // Check for duplicate name with the parent list
            //

            // Check for a valid digital signature on the COM Server before
            // loading it
			if( forceError (ERR_SECURITY_FAILED_NAVSCANTASKS) || 
               NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_NAVScanTasks) )
            {
                // Invalid signature...bail out
                CCTRACEI ("CNAVScanTask::Edit(): Invalid Symantec digital signature on NAVScanTasks COM Server");
                makeGenericError ( ERR_SECURITY_FAILED_NAVSCANTASKS, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
                return E_ACCESSDENIED;
            }

            CComPtr <INAVScanTasks> spTasks;

            if ( forceError (ERR_CREATE_NAVSCANTASKS) ||
                 FAILED (spTasks.CoCreateInstance ( CLSID_NAVScanTasks )))
            {
                CCTRACEI ( "Failure in CoCreateInstance ( CLSID_NAVScanTasks ) in Edit");
                makeGenericError ( ERR_CREATE_NAVSCANTASKS, E_FAIL, IDS_NAVERROR_INTERNAL );
                return E_FAIL;
            }

            BSTR bstrName;
            
            get_TaskName ( &bstrName );
            
            _bstr_t bstName (bstrName,false);

            // Validate the name, if it changed.
            //            
            HRESULT hrValidate = S_OK;

            if ( bstOldTaskName != bstName )
            {
                hrValidate = spTasks->ValidateName ( &bstrName );
            }
        
            if ( hrValidate == S_OK )
            {
                Save ();

                // If the task name changed we need to change
                // the scheduled task name to syncronize, 
                // if there is a schedule. We also need to do this
                // if there is a schedule with no triggers (EScheduled_no)
                //
                renameSchedule ( bstrOldScheduleName );

                return S_OK;
            }
            else if ( hrValidate == S_FALSE )
            {
                // Dup
                //
                iMode = 2; // Duplicate mode
                bDuplicate = true;
                
                // Tell the user they have a duplicate named task
                //
                CString strError;
                g_ResLoader.LoadString(IDS_Err_Dup_Name,strError);

                ::MessageBox ( NULL, strError, m_csProductName, MB_OK | MB_ICONWARNING );

                put_TaskName ( bstrOldTaskName );
            }
        }

    } while ( hr == S_OK && bDuplicate );

    if ( FAILED (hr))
    {
        CCTRACEI ( "Failure in Edit");
        makeGenericError ( ERR_UNKNOWN_EDIT, E_FAIL, IDS_NAVERROR_INTERNAL );
    }

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// INAVScanTaskCust interface
/////////////////////////////////////////////////////////////////////////////
// Properties
/////////////////////////////////////////////////////////////////////////////
// ScanTask
STDMETHODIMP CNAVScanTask::get_ScanTask(/*[out, retval]*/ VARIANT *pvScanTask)
{
   
    if (forceError (ERR_INVALID_POINTER) || !pvScanTask)
    {
        CCTRACEI ( "Invalid pointer in get_ScanTask");
        makeGenericError ( ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL );
        return E_POINTER;
    }

	::VariantInit(pvScanTask);

	// Pass back the CScanTask as a generic pointer
	pvScanTask->vt = VT_BYREF;
	pvScanTask->byref = m_pScanTask;

	return S_OK;
}

STDMETHODIMP CNAVScanTask::get_TaskPath(BSTR *pVal)
{
    
    if (forceError (ERR_INVALID_POINTER) || !pVal)
    {
        CCTRACEI ( "Invalid pointer in get_TaskPath");
        makeGenericError ( ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL );
        return E_POINTER;
    }

    *pVal = T2BSTR ( m_pScanTask->GetPath ());

	return S_OK;
}

STDMETHODIMP CNAVScanTask::get_CanEdit(BOOL *pVal)
{

	if (forceError (ERR_INVALID_POINTER) || !pVal)
    {
        CCTRACEI ( "Invalid pointer in get_CanEdit");
        makeGenericError ( ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL );
        return E_POINTER;
    }

    *pVal = (m_pScanTask->GetCanEdit());

	return S_OK;
}

STDMETHODIMP CNAVScanTask::get_CanSchedule(BOOL *pVal)
{
   
    if (forceError (ERR_INVALID_POINTER) || !pVal)
    {
        CCTRACEI ( "Invalid pointer in get_CanSchedule");
        makeGenericError ( ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL );
        return E_POINTER;
    }

    // Only allow the Admin to schedule the My Computer scan and quick scan.
    // For all other users it will appear as unschedulable.
    //
    if ( m_pScanTask->GetType () == scanMyComputer ||
         m_pScanTask->GetType () == scanQuick )
    {
        COSInfo OSInfo;
        if ( !OSInfo.IsAdminProcess ())
            *pVal = FALSE;
    }

    *pVal = (m_pScanTask->GetCanSchedule());

	return S_OK;
}

bool CNAVScanTask::makeSchedule( bool bDefaultWeekly )
{
    USES_CONVERSION;

    HRESULT hr = S_OK;

	CComPtr<ITaskScheduler> spTaskScheduler;

    hr = spTaskScheduler.CoCreateInstance( CLSID_CTaskScheduler,
                                           NULL,
                                           CLSCTX_INPROC_SERVER);
    if ( FAILED (hr))
    {
        CCTRACEE ("makeSchedule - failed CoCreate 0x%x", hr);
       return false;
    }

    IUnknown* pUnkTask = NULL;

    CComBSTR bstrScheduleName;
    getScheduleName ( bstrScheduleName );

    hr = spTaskScheduler -> NewWorkItem( bstrScheduleName,
                                         CLSID_CTask,
                                         IID_ITask,
                                         &pUnkTask );
    if ( FAILED (hr))
    {
        // If there is already a scheduled item we should 
        // just return true, since we are confirming the creation of
        // a schedule for this item.
        //
        if ( HRESULT_FROM_WIN32 (ERROR_FILE_EXISTS) == hr )
            return true;
        else
        {
            CCTRACEE ("makeSchedule - failed NewWorkItem 0x%x", hr);
            return false;
        }
    }
    
    // If we fail here it's probably because the task already exists.

    // We can switch to a smart pointer now.
    //
    CComQIPtr <ITask> spITask;
    spITask = pUnkTask;
    pUnkTask->Release();

    CComPtr <IPersistFile> spIPersistFile;

    // Put a nice friendly comment in the comment field
    //    
    CStringW cswComment;
    CString csFormat;
    g_ResLoader.LoadString(IDS_SCHEDULE_COMMENT,csFormat);
    cswComment.Format ( A2W(csFormat), A2W(m_csProductName) );
    
    hr = spITask -> SetComment ( cswComment );
    if ( FAILED (hr))
    {
        CCTRACEE ("makeSchedule - failed SetComment 0x%x", hr);
        return false;
    }

    // Command line is c:\progra~1\norton~1\navw32.exe /task:"c:\...\task.sca"
    //
    std::string strExePath;
    std::string strParameters;

    CNAVInfo NAVInfo;
    
    TCHAR szShortNAVDir [MAX_PATH] = {0};

    ::GetShortPathName ( NAVInfo.GetNAVDir (), szShortNAVDir, MAX_PATH );

    strExePath = szShortNAVDir;
    strExePath += _T("\\navw32.exe");

	SCANTASKTYPE scanType = m_pScanTask->GetType();
	if((scanComputer == scanType) 
		|| (scanRemovable == scanType)
		|| (scanAFloppy == scanType)
		|| (scanSelectedDrives == scanType)
		|| (scanFolder == scanType)
		|| (scanFiles == scanType)
		|| (scanCustom == scanType))
	{
		strParameters = SWITCH_DISABLE_ERASER_SCAN;
		strParameters += _T(" ");
	}

	strParameters += SWITCH_TASKFILE;
	strParameters += _T("\"");
	strParameters += m_pScanTask->GetPath();
    strParameters += _T("\"");

    hr = spITask -> SetApplicationName ( T2OLE (strExePath.c_str()) );
    if ( FAILED (hr))
    {
        CCTRACEE ("makeSchedule - failed SetApplicationName");
        return false;
    }
    
    hr = spITask -> SetParameters ( T2OLE (strParameters.c_str()) );
    if ( FAILED (hr))
    {
        CCTRACEE ("makeSchedule - failed SetParameters");        
        return false;
    }

    hr = spITask -> SetFlags ( SCHEDULE_BASE_FLAGS );
    if ( FAILED (hr))    // So we don't reset our base stuff
    {
        CCTRACEE ("makeSchedule - failed SetFlags");        
        return false;
    }

    //
    // All this junk is to put the COMPUTERNAME\username into the task. Otherwise
    // we run as a local system service.
    //
    // The COMPUTERNAME\username (no password) and TASK_FLAG_RUN_ONLY_IF_LOGGED_ON
    // flag is Task Scheduler's pseudo-impersonation method. This isn't really documented.
    // I figured it out by looking at the Windows Critical Update item in the scheduler.
    //
    // If the about step fails, it's probably because we are on an OS without
    // security, like 9x. Not stopping because of this error is easier than
    // putting in a bunch of OS checking that will get out of date.

    WCHAR wszAccountName [ 200 ];
    LPWSTR pwszAccountName = wszAccountName;

    WCHAR wszUserName [ 200 ];
    LPWSTR pwszUserName = wszUserName;
    DWORD dwUserNameSize = sizeof ( wszUserName );
    
    WCHAR wszComputerName [ 100 ];
    LPWSTR pwszComputerName = wszComputerName;
    DWORD dwComputerNameSize = sizeof ( wszComputerName );

    LPWSTR pwszPassword = NULL;

    if ( GetUserNameW(wszUserName, &dwUserNameSize) && GetComputerNameW (wszComputerName, &dwComputerNameSize))
    {
        wcscpy ( pwszAccountName, pwszComputerName );
        wcscat ( pwszAccountName, L"\\" );
        wcscat ( pwszAccountName, pwszUserName );

        hr = spITask -> SetAccountInformation( pwszAccountName,
                                          pwszPassword );
		if ( FAILED (hr) )
        {
            CCTRACEE(_T("CNAVScanTask::makeSchedule - SetAccountInformation(%S) failed. Error: 0x%08X"), pwszAccountName, hr);
        }
    }
    else
    {
        CCTRACEE ("makeSchedule - failed GetComputerName");        
        return false;
    }

    // If this is the weekly "My Computer" scan
    //
    if ( bDefaultWeekly )
    {
        CComPtr <ITaskTrigger> spITaskTrigger;
        WORD piNewTrigger;
        hr = spITask->CreateTrigger( &piNewTrigger,
                                     &spITaskTrigger);
        if ( FAILED (hr))
        {
            CCTRACEE ("makeSchedule - failed CreateTrigger 0x%x", hr);        
            return false;
        }

        TASK_TRIGGER trigger;
        ZeroMemory( &trigger, sizeof (TASK_TRIGGER) );

        SYSTEMTIME sysTime;

        GetSystemTime ( &sysTime );

        trigger.wBeginYear = sysTime.wYear;
        trigger.wBeginMonth = sysTime.wMonth;
        trigger.wBeginDay = sysTime.wDay;
        trigger.wStartHour = 12 + 8; // 8 PM (24-hour clock)

        TRIGGER_TYPE_UNION triggertype;
        ZeroMemory( &triggertype, sizeof ( TRIGGER_TYPE_UNION ));

        WEEKLY weekly;
        weekly.WeeksInterval = 1;               // Every week
        weekly.rgfDaysOfTheWeek = TASK_FRIDAY;  // On Friday
   
        triggertype.Weekly = weekly;
        trigger.Type = triggertype;

        trigger.cbTriggerSize = sizeof ( trigger );

        trigger.TriggerType = TASK_TIME_TRIGGER_WEEKLY;    // It's a weekly operation

        // Save the trigger

        hr = spITaskTrigger -> SetTrigger ( &trigger );
        if ( FAILED (hr))
        {
            CCTRACEE ("makeSchedule - failed SetTrigger 0x%x", hr);        
            return false;
        }

    }

    // Save the Task ( trigger is attached )
    hr = spITask->QueryInterface( IID_IPersistFile,
                                  (void **)&spIPersistFile);
    if ( FAILED (hr))
    {
        CCTRACEE ("makeSchedule - failed QI IPersistFile 0x%x", hr);
        return false;
    }

    // We don't change the return value on Save because the file might already exist. Bug
    // in IE 4.0 . Defect # 361077.
    //
    spIPersistFile -> Save( NULL, TRUE);

    return true;
}

void CNAVScanTask::getScheduleName(CComBSTR &bstrScheduleName)
{
    USES_CONVERSION;
    bstrScheduleName = m_csProductName;
    bstrScheduleName.Append (" - ");
    bstrScheduleName.Append ( m_pScanTask->GetName ());
    TCHAR szUserName [200] = {0};
    DWORD dwUserNameSize = 200;
    GetUserName ( &szUserName[0], &dwUserNameSize );
    if ( szUserName[0] != '\0' )
    {
        bstrScheduleName.Append (" - ");
        bstrScheduleName.Append (szUserName);
    }
}

bool CNAVScanTask::showSchedule()
{
    USES_CONVERSION;
    
    HRESULT hr = S_OK;

	CComPtr<ITaskScheduler> spTaskScheduler;

    if ( FAILED ( spTaskScheduler.CoCreateInstance( CLSID_CTaskScheduler,
                                    NULL,
                                    CLSCTX_INPROC_SERVER)))
    {
       return false;
    }

    IUnknown* pUnkTask = NULL; 

    CComQIPtr <ITask> spITask;
    CComBSTR bstrScheduleName;
    getScheduleName ( bstrScheduleName );

	// Check if a scheduler file exist for this task
	if (SUCCEEDED(hr = spTaskScheduler->Activate( bstrScheduleName,
                                                  IID_ITask,
                                                 (&pUnkTask))))
	{
        spITask = pUnkTask;
        pUnkTask->Release ();

        TASKPAGE tpType = TASKPAGE_SCHEDULE;
        
        BOOL bPersistChanges = TRUE;
        HPROPSHEETPAGE phPage;
        CComPtr <IProvideTaskPage> spIProvTaskPage;

        hr = spITask->QueryInterface(IID_IProvideTaskPage,
                                    (void **)&spIProvTaskPage);
        

        hr = spIProvTaskPage->GetPage(tpType,
                                     bPersistChanges,
                                     &phPage);

        if ( SUCCEEDED ( hr ))
        {
            PROPSHEETHEADER psh;
            ZeroMemory(&psh, sizeof(PROPSHEETHEADER));
            psh.dwSize = sizeof(PROPSHEETHEADER);
            psh.dwFlags = PSH_DEFAULT | PSH_NOAPPLYNOW | PSH_USEPAGELANG | PSH_MODELESS;
            psh.phpage = &phPage;
            psh.nPages = 1;

            HWND hwndParent = ::GetForegroundWindow ();

            if ( !IsWindow ( hwndParent ))
                hwndParent = ::GetDesktopWindow ();

            psh.hwndParent = hwndParent;
            
            // We get to customize the title
            //
            CComBSTR bstrScheduleName;
            getScheduleName ( bstrScheduleName );
            psh.pszCaption = OLE2T (bstrScheduleName);

            // Show it!
            HWND hwndDlg = (HWND)PropertySheet(&psh);

			if (hwndDlg == NULL)
			{
				CCTRACEE("Failed to create property sheet");
				return false;
			}

			// Check the width of created property page
		    RECT r;
			long lPropPageWidth = 0;
			long left = 0;
			long right = 0;

			HWND hPage = NULL;

			// PropSheet_IndexToHwnd() does not work here
			PropSheet_SetCurSel(hwndDlg, 0, 0);
			hPage = PropSheet_GetCurrentPageHwnd(hwndDlg);

			if (hPage == NULL || !GetWindowRect(hPage, &r))
			{
				CCTRACEE("Failed to find the first page");
				return false;
			}

			left = r.left;
			right = r.right;

			// Enumerate all children
			HWND hChild = NULL;
			while ((hChild = FindWindowEx(hPage, hChild, NULL, NULL)) != NULL)
			{
				if (GetWindowRect(hChild, &r))
				{
					if (r.right > right)
					{
						right = r.right;
						lPropPageWidth = right - left;
					}
				}
			}

			PropSheet_SetCurSel(hwndDlg, 0, 0);
            hPage = PropSheet_GetCurrentPageHwnd(hwndDlg);
            if (hPage != NULL && GetWindowRect(hPage, &r) && ((r.right - r.left + 2) < lPropPageWidth))
			{
				CCTRACEI("Resizing property page");

				// Create a fake page
				DLGTEMPLATE dt = {0};
				dt.style = DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU;
				dt.cy = 100;

				// Convert pixels to dialog units
				RECT rt = {0};
				rt.right = 100;
				MapDialogRect(hwndDlg, &rt);
				dt.cx = (lPropPageWidth * 100) / rt.right;

				// Create property page
				PROPSHEETPAGE pp = {0};
				pp.dwSize = sizeof(pp);
				pp.dwFlags = PSP_DLGINDIRECT | PSP_USETITLE;
				pp.pResource = &dt;
				pp.pszTitle = "";

				HPROPSHEETPAGE page = CreatePropertySheetPage(&pp);

				if (page != NULL && PropSheet_AddPage(hwndDlg, page))
				{
					PropSheet_RecalcPageSizes(hwndDlg);
					PropSheet_RemovePage(hwndDlg, 1, page);
				}
				else
				{
					CCTRACEW("Failed to create/add a fake page");
				}
			}

			// Message loop
			MSG msg;
			while (GetMessage( &msg, NULL, 0, 0 ))
			{ 
				if (!PropSheet_IsDialogMessage(hwndDlg, &msg))
		        { 
				    TranslateMessage(&msg); 
		            DispatchMessage(&msg); 
				}
				if (PropSheet_GetCurrentPageHwnd(hwndDlg) == NULL)
				{
					DestroyWindow(hwndDlg);
					break;
				}
		    } 

		    return true;
		}
	}
    return false;
}

STDMETHODIMP CNAVScanTask::DeleteSchedule()
{
   
    HRESULT hr = S_OK;

	CComPtr<ITaskScheduler> spTaskScheduler;

    if ( forceError (ERR_NO_SCHEDULER) ||
         FAILED ( spTaskScheduler.CoCreateInstance( CLSID_CTaskScheduler,
                                    NULL,
                                    CLSCTX_INPROC_SERVER)))
    {
        CCTRACEI ( "CoCreateInstance( CLSID_CTaskScheduler) failed in DeleteSchedule");
        makeGenericError ( ERR_NO_SCHEDULER, E_FAIL, IDS_NAVERROR_INTERNAL );
        return E_FAIL;
    }

    CComBSTR bstrScheduleName;
    getScheduleName ( bstrScheduleName );
    return ( spTaskScheduler->Delete ( bstrScheduleName ));
}


// Used to syncronize our Task name with our Schedule name
//
bool CNAVScanTask::renameSchedule(BSTR bstrOldScheduleName)
{
    HRESULT hr = S_OK;

	CComPtr<ITaskScheduler> spTaskScheduler;

    if ( FAILED ( spTaskScheduler.CoCreateInstance( CLSID_CTaskScheduler,
                                    NULL,
                                    CLSCTX_INPROC_SERVER)))
       return false;

    IUnknown* pUnkTask = NULL;

    //
    // Read in old task
    //

    if ( FAILED ( spTaskScheduler -> Activate( bstrOldScheduleName,
                                               IID_ITask,
                                               (&pUnkTask))))
    {
        // Don't releas the pUnkTask since it's bogus!
        return false;
    }
    
    // If we fail here it's probably because the task already exists.

    // We can switch to a smart pointer now.
    //
    CComQIPtr <ITask> spIOldTask;
    spIOldTask = pUnkTask;
    pUnkTask->Release();

    CComPtr <IPersistFile> spIOldPersistFile;

    if ( FAILED ( spIOldTask->QueryInterface( IID_IPersistFile,
                                           (void **)&spIOldPersistFile)))
        return false;

    //
    // Get filename
    //
    
    LPOLESTR lpstrOldFileName;
    
    if ( FAILED ( spIOldPersistFile -> GetCurFile ( &lpstrOldFileName )))
        return false;

    // Ditch the LPOLESTR and just store it locally instead
    //
    WCHAR lpszOldFileName[MAX_PATH] = {0};
    wcsncpy ( lpszOldFileName, lpstrOldFileName, MAX_PATH );

    LPMALLOC lpMalloc;
    CoGetMalloc ( 1, &lpMalloc );
    lpMalloc->Free ( lpstrOldFileName );

    //
    // Create new task
    //
    CComBSTR bstrScheduleName;
    getScheduleName ( bstrScheduleName );

    IUnknown *pINewTask = NULL;

    if ( FAILED ( spTaskScheduler -> NewWorkItem( bstrScheduleName,
                                                  CLSID_CTask,
                                                  IID_ITask,
                                                  (&pINewTask) )))
    {
        // Don't releas the pINewTask since it's bogus!
        return false;
    }

    // We can switch to a smart pointer now.
    //
    CComQIPtr <ITask> spINewTask;
    spINewTask = pINewTask;
    pINewTask->Release();

    CComPtr <IPersistFile> spINewPersistFile;

    if ( FAILED ( spINewTask->QueryInterface( IID_IPersistFile,
                                              (void **)&spINewPersistFile)))
        return false;

    //
    // Get new filename
    //
    LPOLESTR lpolestrNewFileName;
    
    if ( FAILED ( spINewPersistFile -> GetCurFile ( &lpolestrNewFileName )))
        return false;

    // Ditch the LPOLESTR and just store it locally instead
    //
    WCHAR lpszNewFileName[MAX_PATH] = {0};
    wcsncpy ( lpszNewFileName, lpolestrNewFileName, MAX_PATH );

    lpMalloc->Free ( lpolestrNewFileName );

    //
    // Save old task in new filename
    //
    if ( FAILED ( spIOldPersistFile -> Save( lpszNewFileName,
                                             FALSE)))   // Save As ...
        return false;

    //
    // Delete old task
    //
    if ( FAILED ( spTaskScheduler -> Delete ( lpszOldFileName )))
        return false;

    return true;
}

STDMETHODIMP CNAVScanTask::get_NAVError(INAVCOMError **pVal)
{
	if ( !m_spError )
        return E_FAIL;

    *pVal = m_spError;
    (*pVal)->AddRef (); // We get a ref and the caller gets one

	return S_OK;
}

void CNAVScanTask::makeError(long lMessageID, long lHResult)
{
	if ( !m_spError )
		return;

	CString csMessage;
	CString csFormat;

    g_ResLoader.LoadString(lMessageID, csFormat);
	csMessage = csFormat;

	switch(lMessageID)
	{
	case ERR_UNKNOWN_SAVE:
	case ERR_UNKNOWN_SCHEDULE:
		csMessage.Format(csFormat, m_csProductName);
		break;

	default:
		break;
	}

	CComBSTR bstrErrorMessage( csMessage );
	m_spError->put_Message ( bstrErrorMessage );
	m_spError->put_ModuleID ( AV_MODULE_ID_SCAN_TASKS );
	m_spError->put_ErrorID ( lMessageID );
	m_spError->put_HResult ( lHResult );
}

void CNAVScanTask::makeGenericError(long lErrorID, long lHResult, long lNAVErrorResID)
{
	if ( !m_spError )
		return;

	m_spError->put_ModuleID ( AV_MODULE_ID_SCAN_TASKS );
	m_spError->put_ErrorID ( lErrorID );
	m_spError->put_ErrorResourceID ( lNAVErrorResID );
	m_spError->put_HResult ( lHResult );
}

bool CNAVScanTask::forceError(long lErrorID)
{
    if ( !m_spError )
        return false;

    long lTempErrorID = 0;
    long lTempModuleID = 0;

    m_spError->get_ForcedModuleID ( &lTempModuleID );
    m_spError->get_ForcedErrorID ( &lTempErrorID );

    if ( lTempModuleID == AV_MODULE_ID_SCAN_TASKS &&
         lTempErrorID == lErrorID )
         return true;
    else
        return false;
}


STDMETHODIMP CNAVScanTask::Scan(void)
{
    return Scan(false);
}


STDMETHODIMP CNAVScanTask::ScanAndWait(void)
{
    return Scan(true);
}