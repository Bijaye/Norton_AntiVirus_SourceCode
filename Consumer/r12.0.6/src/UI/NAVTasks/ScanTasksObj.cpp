// ScanTasksObj.cpp : Implementation of CNAVScanTasks
#include "StdAfx.h"

#include "ScanTask.h"
#include "NAVTasks.h"
#include "ScanTaskObj.h"
#include "ScanTasksObj.h"
#include <algorithm>
#include "NAVTrust.h"
#include "AVccModuleId.h"
#include "AVRESBranding.h"
#include "NAVErrorResource.h"
#include "ccResourceLoader.h"
#include "ccWebWnd_i.c"
#include "FullOrQuickScanDlg.h"

// Resource Loader
extern cc::CResourceLoader g_ResLoader;


/////////////////////////////////////////////////////////////////////////////
// CNAVScanTasks
CNAVScanTasks::CNAVScanTasks(void)
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

CNAVScanTasks::~CNAVScanTasks(void)
{
    emptyTaskList();
}

HRESULT CNAVScanTasks::FinalConstruct(void)
{
    try
    {
        // Load the tasks.
        // - First from the All Users folder, then from the current users' folder.
        //
        StahlSoft::HRX hrx;
        hrx <<  addTasks(g_NAVInfo.GetNAVCommonDir());
        hrx <<  addTasks(g_NAVInfo.GetNAVUserDir());
        return S_OK;
    }
    catch(_com_error& e)
    {
        return e.Error();
    }
    catch(...)
    {
        return E_UNEXPECTED;
    }
}

HRESULT CNAVScanTasks::FinalRelease (void)
{
   emptyTaskList ();
   return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
// Properties
STDMETHODIMP CNAVScanTasks::get_Item(/*[in]*/ VARIANT vIndex, /*[out, retval]*/ INAVScanTask** ppItem)
{
	// Check our script security
    //
    if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        makeGenericError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }
    
    HRESULT hr;
	ContainerType::iterator it;

	if (forceError (ERR_INVALID_POINTER) || !ppItem)
    {
        CCTRACEI ( "Invalid arg in get_Item");
        makeGenericError ( ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL );
        return E_POINTER;
    }

	if (SUCCEEDED(hr = find(vIndex, it)))
    {
        *ppItem = *it;
        (*it)->AddRef();
    }

    if (forceError (ERR_UNKNOWN_GET_ITEM))
        hr = E_FAIL;

    if ( FAILED (hr))
        makeGenericError ( ERR_UNKNOWN_GET_ITEM, hr, IDS_NAVERROR_INTERNAL );
    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// Methods
STDMETHODIMP CNAVScanTasks::Add(/*[in]*/ INAVScanTask* pScanTask)
{
	// Check our script security
    //
    if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        makeGenericError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    if (forceError (ERR_INVALID_ARG_ADD) || !pScanTask)
    {
        CCTRACEI ( "Invalid arg in Add");
        makeGenericError ( ERR_INVALID_ARG_ADD, E_INVALIDARG, IDS_NAVERROR_INTERNAL );
        return E_INVALIDARG;
    }

    INAVScanTask* pLocalScanTask = pScanTask;

    pLocalScanTask->AddRef();

	m_coll.push_back(pLocalScanTask);

	return S_OK;
}

STDMETHODIMP CNAVScanTasks::Remove(/*[in]*/ VARIANT vItem)
{
	// Check our script security
    //
    if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        makeGenericError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    HRESULT hr;
	ContainerType::iterator it;

	if (SUCCEEDED(hr = find(vItem, it)))
    {
        (*it)->Release();
		m_coll.erase(it);  // Take it away
    }

    if ( forceError (ERR_UNKNOWN_REMOVE))
        hr = E_FAIL;

    if ( FAILED (hr))
        makeGenericError ( ERR_UNKNOWN_REMOVE, hr, IDS_NAVERROR_INTERNAL );

    return hr;
}

STDMETHODIMP CNAVScanTasks::Create(/*[out, retval]*/ INAVScanTask** ppItem)
{
	CCTRACEI ( "Create()" );

    USES_CONVERSION;
    
    // Check our script security
    //
    if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        makeGenericError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    CCTRACEI ( "Making new objects" );
    HRESULT hr;
	CComObject<CNAVScanTask>* pNAVScanTask = NULL;
	CComPtr<INAVScanTask> spINAVScanTask;

	if (forceError (ERR_INVALID_POINTER) || !ppItem)
    {
        CCTRACEI ( "Invalid pointer in Create");
        makeGenericError ( ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL );
        return E_POINTER;
    }

	*ppItem = NULL;

    // Check for a valid digital signature on the COM Server before
    // loading it
	if( forceError (ERR_SECURITY_FAILED) || NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_NAVScanTasks) )
    {
        // Invalid signature...bail out
        CCTRACEI ("CNAVScanTasks::Create(): Invalid Symantec digital signature on NAVScanTasks COM Server");
        makeGenericError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

	// Create a scan task object - NAVError object is is the CNAVScanTask object.
    //
	if (FAILED(hr = CComObject<CNAVScanTask>::CreateInstance(&pNAVScanTask)))
    {
        CCTRACEI ( "CreateInstance(&pNAVScanTask) failed in Create");
        return hr;
    }

	pNAVScanTask->AddRef();
	hr = pNAVScanTask->QueryInterface(&spINAVScanTask);
	pNAVScanTask->Release();

	if ( forceError (ERR_CREATE_NAVSCANTASK) )
        hr = E_FAIL;

    if ( FAILED(hr))
    {
        CCTRACEI ( "QueryInterface(&spINAVScanTask) failed in Create");
        makeGenericError ( ERR_CREATE_NAVSCANTASK, hr, IDS_NAVERROR_INTERNAL );
        return hr;
    }
    
    // Only custom scans can be created.
	pNAVScanTask->m_pScanTask->SetType(scanCustom);

    // You can edit and schedule custom scans!
    //
    pNAVScanTask->m_pScanTask->SetCanEdit ( true );
    pNAVScanTask->m_pScanTask->SetCanSchedule ( true );
	
    // We don't like duplicate scan task names!
    //
    bool bDuplicate = false;
    int iMode = 0;  // modeCreate
	
	//Saving off Current working directory, since the MS open file dlg changes it
	TCHAR currentDirectory[MAX_PATH + 1];
	DWORD dwRet;
	dwRet = GetCurrentDirectory(MAX_PATH + 1, currentDirectory);
	if(0 == dwRet || dwRet > MAX_PATH + 1)
	{
		CCTRACEE("CNAVScanTasks::Create() - GetCurrentDirectory() failed");
	}
	

    // Start the Wizard and let it do it's magic. Poof!
    //
    do 
	{
        CCTRACEI ( "Launching Wizard" );

        CTaskWizard NTW;
        hr = NTW.NAVTaskWizard (pNAVScanTask->m_pScanTask, iMode);
        
        if ( forceError (ERR_UNKNOWN_CREATE))
            hr = E_FAIL;

        if ( S_OK == hr)
        {
            // Check for duplicate task names
            //
            CComBSTR bstrTaskName;
            
            pNAVScanTask->get_TaskName ( &bstrTaskName);

            if ( S_FALSE == ValidateName ( &bstrTaskName ) )
            {
                bDuplicate = true;
                iMode = 2;  // modeDuplicate
 
                // Tell the user they have a duplicate named task
                //
                CString csError;
				g_ResLoader.LoadString(IDS_Err_Dup_Name,csError);

                ::MessageBox ( ::GetDesktopWindow (), csError, m_csProductName, MB_OK | MB_ICONWARNING );
            }
            else
            {
                // Good name
                //
                bDuplicate = false;
            }
        }
        
    } while ( bDuplicate && S_OK == hr);
    
	//Restoring current directory
	if(!SetCurrentDirectory(currentDirectory))
	{
		CCTRACEE("CNAVScanTasks::Create() - SetCurrentDirectory() failed");
	}


    if ( S_OK == hr)
    {
        // Save will generate a new custom scan name
        //    
        std::string strFilePath;

        if ( generateFilePath ( strFilePath ) )
        {
            pNAVScanTask->m_pScanTask->SetPath ( strFilePath.c_str() );

            if (SUCCEEDED (hr = pNAVScanTask->Save ()))
            {
                // Add the new object to the collection.
                //
                (*ppItem) = spINAVScanTask;
                (*ppItem)->AddRef();
                m_coll.push_back(spINAVScanTask.Detach());

                return S_OK;
            }
        }

        // Tell the user we couldn't save the task
        //
        CString csFormat, csError;
		g_ResLoader.LoadString(IDS_Err_Cant_Save_Task, csFormat);
        csError.Format(csFormat, m_csProductName);

        ::MessageBox ( ::GetDesktopWindow (), csError, m_csProductName, MB_OK | MB_ICONERROR );

    }
    
    if ( FAILED (hr))
    {
         CCTRACEI ( "Failure in Create");
         makeGenericError ( ERR_UNKNOWN_CREATE, hr, IDS_NAVERROR_INTERNAL );
    }

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// Implementation

bool CNAVScanTasks::generateFilePath( std::string& strFilePath )
{
    // Generate a new custom scan name in the user's directory.
    //
    // Add the beginning of the full path "C:\documents and ..."
    //
    std::string strTemp;

    strTemp = g_NAVInfo.GetNAVUserDir();
    //strTemp = _T("A:"); // Good for testing out of disk space conditions

    strTemp.append ( _T("\\") );
    strTemp.append ( szTaskSubDir );
    strTemp.append ( _T("\\") );

    TCHAR szTempPath  [MAX_PATH] = {0};
    TCHAR szFilePath  [MAX_PATH] = {0};
    TCHAR szPrefix [] = "";

    if ( -1 == ::GetFileAttributes ( strTemp.c_str() ))
    {
        // The Tasks folder doesn't exist so make it.
        // If we fail to make it, we can't make the task so bail.
        //
        if ( !g_NAVInfo.MakeNAVUserDir())
            return false;

        if ( !CreateDirectory ( strTemp.c_str(), NULL ))
            return false;
    }

    if ( 0 != ::GetTempFileName ( strTemp.c_str(), szPrefix, 0, szTempPath ))
    {
        // change the file extension to '.scn' instead of '.tmp' so the shell will associate it
    	// with NAVW.EXE
        ::_tcscpy ( szFilePath, szTempPath );
        ::_tcscpy(::_tcsrchr(szFilePath, _T('.')), ".");
        ::_tcscat ( szFilePath, szTaskFileExtension );

        // It actually creates the .tmp file, which we don't want. We will 
        // rename the .tmp file to a .scn file.
        //
        if ( !MoveFile ( szTempPath, szFilePath ))
        {
            // .SCA file already exists.
            // Don't leave the .tmp file around.
            //
            DeleteFile ( szTempPath );
            return false;
        }

        strFilePath = szFilePath;

        return true;
    }
    else
        return false;
}

HRESULT CNAVScanTasks::find(VARIANT vItem, ContainerType::iterator& it)
{
	switch(vItem.vt)
	{
	case VT_I2:  // Index
	case VT_I4:
	case VT_R4:
	case VT_R8:
	case VT_I1:
	case VT_UI1:
	case VT_UI2:
	case VT_UI4:
	case VT_I8:
	case VT_UI8:
	case VT_INT:
		::VariantChangeType(&vItem, &vItem, 0, VT_UINT);
		/* FALL THROUGH */
	case VT_UINT:
		if (vItem.uintVal < 0 || vItem.uintVal >= m_coll.size())
        {
            CCTRACEI ( "Invalid arg in find VT_UINT");
            return E_INVALIDARG;
        }

		it = m_coll.begin() + vItem.uintVal;
		break;

	case VT_DISPATCH:  // Find the object
	case VT_UNKNOWN:
		if (m_coll.end() == (it = find_if(m_coll.begin(), m_coll.end(), _TestPtr(vItem.punkVal))))
        {
            CCTRACEI ( "Invalid arg in find VT_UNKNOWN");
            return E_INVALIDARG;
        }
		break;

	case VT_BSTR:  // Search by name
		if (m_coll.end() == (it = find_if(m_coll.begin(), m_coll.end(), _TestName(vItem.bstrVal))))
        {
            CCTRACEI ( "Invalid arg in find VT_BSTR");
            return E_INVALIDARG;
        }
		break;

	default:
		return E_INVALIDARG;
	}

	return S_OK;
}

void CNAVScanTasks::emptyTaskList()
{
	for_each(m_coll.begin(), m_coll.end(), _Remove());
	m_coll.clear();
}

HRESULT CNAVScanTasks::addTasks(const char* pszDir)
{
	TCHAR szTasksDir[MAX_PATH]= {0};

    // Each .scan file in the Tasks directory gets listed in the scan
    // task list.

	HANDLE hFindFile = 0;
	WIN32_FIND_DATA FileData = { 0 };

	::_tcscpy( szTasksDir, pszDir);
    ::_tcscat( szTasksDir, "\\");
    ::_tcscat( szTasksDir, szTaskSubDir);
    ::_tcscat( szTasksDir, "\\*.");
    ::_tcscat( szTasksDir, szTaskFileExtension);

	if ( INVALID_HANDLE_VALUE != (hFindFile = ::FindFirstFile (szTasksDir, &FileData)))
	{
		do
		{
			HRESULT hr;
			CComObject<CNAVScanTask>* pNAVScanTask = NULL;
			CComPtr<INAVScanTask> spINAVScanTask;

			// Skip directories
			if ( FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY )
				continue;

            // Check for a valid digital signature on the COM Server before
            // loading it
            if( forceError (ERR_SECURITY_FAILED) || NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecComServer(CLSID_NAVScanTasks) )
            {
                // Invalid signature...bail out
                CCTRACEI ("CNAVScanTasks::addTasks(): Invalid Symantec digital signature on NAVScanTasks COM Server");
                makeGenericError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
                return E_ACCESSDENIED;
            }

			// Create a scan task object
			if (FAILED(hr = CComObject<CNAVScanTask>::CreateInstance(&pNAVScanTask)))
            {
                CCTRACEI ( "CreateInstance(&pNAVScanTask) failed in addTasks");
                return hr;
            }

			pNAVScanTask->AddRef();
			hr = pNAVScanTask->QueryInterface(&spINAVScanTask);
			pNAVScanTask->Release();
            
			if (FAILED(hr))
            {
                CCTRACEI ( "QueryInterface(&spINAVScanTask) failed in addTasks");
                return hr;
            }

			// Load the task
			::_tcscpy(::_tcsrchr(szTasksDir, _T('\\'))+1, FileData.cFileName);
			
            // Don't return if the load fails. Why? Otherwise if you have one
            // bad/corrupted task the whole list is empty.
            //
            if ( pNAVScanTask->m_pScanTask->Load(szTasksDir, false))
            {
			    // Add the new object to the collection.
			    m_coll.push_back(spINAVScanTask.Detach());
            }
   

		} while (::FindNextFile(hFindFile, &FileData));

		::FindClose(hFindFile);
	}


    return S_OK;
}

// Remove all .scn files from the All Users\...\Tasks dir.
// We leave all the individual users' data, just delete the built-ins
//
STDMETHODIMP CNAVScanTasks::DeleteAll()
{
	// Check our script security
    //
    if (forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        makeGenericError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    //
    // Remove ALL the Norton AntiVirus schedules
    //

    HRESULT hr = S_OK;

	CComPtr<ITaskScheduler> spTaskScheduler;

    if ( SUCCEEDED ( spTaskScheduler.CoCreateInstance( CLSID_CTaskScheduler,
                                       NULL,
                                       CLSCTX_INPROC_SERVER)))
    {
        CComPtr<IEnumWorkItems> spIEnum;

        if ( SUCCEEDED ( spTaskScheduler -> Enum(&spIEnum)))
        {
        
            TCHAR lpszScheduleName [MAX_PATH] = {0};
            WCHAR lpwszScheduleName [MAX_PATH] = {0};
            g_ResLoader.Initialize();
            ::LoadString ( g_ResLoader.GetResourceInstance(), IDS_NAME, lpszScheduleName, MAX_PATH );
        
            if ( -1 != mbstowcs ( lpwszScheduleName, lpszScheduleName, MAX_PATH ))
            {
                LPWSTR *lpwszName;
        
                DWORD dwFetchedTasks = 0;

                while ( SUCCEEDED ( spIEnum->Next(  1,
                                                    &lpwszName,
                                                    &dwFetchedTasks))
                        && (dwFetchedTasks != 0))
                {
                    // Compare front of schedule to "Norton AntiVirus"
                    //
                    if ( 0 == wcsncmp ( lpwszName[0], lpwszScheduleName, wcslen (lpwszScheduleName) ))
                    {
                        //::MessageBox ( ::GetDesktopWindow (), lpszTempName, "Deleting", MB_OK );
                        spTaskScheduler->Delete ( lpwszName[0] );                
                    }
                    //else
                        //::MessageBox ( ::GetDesktopWindow (), lpszTempName, "OK", MB_OK );
                }
            
                CoTaskMemFree(lpwszName);
            }
        }        
    }

    // Now delete the built-in scan tasks. Leave the users'.
    //    
    TCHAR szTasksDir[MAX_PATH]= {0};
    TCHAR szFilePath[MAX_PATH]= {0};

	HANDLE hFindFile = 0;
	WIN32_FIND_DATA FileData = { 0 };

	::_tcscpy( szTasksDir, g_NAVInfo.GetNAVCommonDir());
    ::_tcscat( szTasksDir, "\\");
    ::_tcscat( szTasksDir, szTaskSubDir);
    ::_tcscat( szTasksDir, "\\*.");
    ::_tcscat( szTasksDir, szTaskFileExtension);

	if ( INVALID_HANDLE_VALUE != (hFindFile = ::FindFirstFile (szTasksDir, &FileData)))
    {
        do 
        {
			// Skip directories
			if ( FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY )
				continue;
            
        	::_tcscpy ( szFilePath, g_NAVInfo.GetNAVCommonDir());
            ::_tcscat ( szFilePath, "\\");
            ::_tcscat ( szFilePath, szTaskSubDir);
            ::_tcscat ( szFilePath, "\\" );
            ::_tcscat ( szFilePath, FileData.cFileName );

            deleteFileAgressive ( szFilePath );

        } while (::FindNextFile(hFindFile, &FileData));

        ::FindClose(hFindFile);
    }

	return S_OK;
}

// Schedule the default weekly scan of "My Computer"
//
STDMETHODIMP CNAVScanTasks::ScheduleMyComputer()
{
	// Check our script security
    //
    if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        makeGenericError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    if (!forceError (ERR_NO_MY_COMPUTER))
    {
        ContainerType::iterator it;
        long lType = 0;

        // Find the My Computer task.
        //
        for ( it = m_coll.begin (); it < m_coll.end(); it++ )
        {
            CComPtr <INAVScanTask> spTask = *it;
            spTask->get_TaskType ( &lType);

            if ( lType == scanMyComputer )
            {
                // Schedule the weekly default
                //
                BOOL bWeekly = TRUE;
                HRESULT hr = spTask->Schedule ( &bWeekly );
				CCTRACEI(_T("CNAVScanTasks::ScheduleMyComputer() : spTask->Schedule() == 0x%08X\n"), hr);
                if ( forceError (ERR_SCHEDULE_FAILED) || FAILED (hr))
                    makeError ( ERR_SCHEDULE_FAILED, hr );

                return hr;
            }
        }
    }

    // Uh oh, couldn't find it.
    //
    CCTRACEI ( "Couldn't find task for ScheduleMyComputer");
    makeError ( ERR_NO_MY_COMPUTER, E_FAIL );
    return E_FAIL;
}

STDMETHODIMP CNAVScanTasks::FullOrQuickScan()
{
    // Check our script security
    //
    if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        makeGenericError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    // Launch the UI component to find out if they want a full system scan
    // or the "quick" eraser scan
    CFullOrQuickScanDlg dlg;
    int nRet = dlg.DoModal();

    if( nRet == IDOK )
    {
        if( dlg.GetUserAction() == CFullOrQuickScanDlg::USER_ACTION_QUICK )
        {
            return QuickScan();
        }

        return ScanMyComputer();
    }

    return S_OK;
}

STDMETHODIMP CNAVScanTasks::QuickScan()
{
    // Check our script security
    //
    if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        makeGenericError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    if ( !forceError (ERR_NO_QUICKSCANTASK))
    {
        ContainerType::iterator it;
        long lType = 0;

        // Find the My Computer task.
        //
        for ( it = m_coll.begin (); it < m_coll.end(); it++ )
        {
            CComPtr <INAVScanTask> spTask = *it;
            spTask->get_TaskType ( &lType);

            if ( lType == scanQuick )
            {
                // Run the scan
                //
                HRESULT hr = spTask->Scan ();

                if ( forceError (ERR_FAILED_LAUNCH_SCAN))
                    hr = E_FAIL;

                if ( FAILED (hr))
                    makeGenericError ( ERR_FAILED_LAUNCH_SCAN, hr, IDS_NAVERROR_INTERNAL );

                return hr;
            }

        }
    }

    // Uh oh, couldn't find it.
    //
    CCTRACEE ( "Couldn't find task for QuickScan");
    makeError ( ERR_NO_QUICKSCANTASK, E_FAIL );
    return E_FAIL;
}

STDMETHODIMP CNAVScanTasks::ScanMyComputer(bool bWait)
{
	// Check our script security
    //
    if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        makeGenericError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    if ( !forceError (ERR_NO_MY_COMPUTER))
    {
        ContainerType::iterator it;
        long lType = 0;

        // Find the My Computer task.
        //
        for ( it = m_coll.begin (); it < m_coll.end(); it++ )
        {
            CComPtr <INAVScanTask> spTask = *it;
            spTask->get_TaskType ( &lType);

            if ( lType == scanMyComputer )
            {
                // Run the scan
                //
                HRESULT hr = S_OK;
                if (bWait)
                {
                    hr = spTask->ScanAndWait ();
                }
                else
                {
                    hr = spTask->Scan ();
                }
            
                if ( forceError (ERR_FAILED_LAUNCH_SCAN))
                    hr = E_FAIL;

                if ( FAILED (hr))
                    makeGenericError ( ERR_FAILED_LAUNCH_SCAN, hr, IDS_NAVERROR_INTERNAL );
            
                return hr;
            }
        
        }
    }

    // Uh oh, couldn't find it.
    //
    CCTRACEE ( "Couldn't find task for ScanMyComputer");
    makeError ( ERR_NO_MY_COMPUTER, E_FAIL );
    return E_FAIL;
}

// This function will try to remove the read-only attrib on a file
// if it is set, prior to deleting it.
//
bool CNAVScanTasks::deleteFileAgressive(LPCTSTR lpcszFilePath)
{
    // Try to remove the read-only attrib if it is set
    //
    DWORD dwFileAttribs = 0;
    dwFileAttribs = GetFileAttributes ( lpcszFilePath );
    
    if ( 0xFFFFFFFF != dwFileAttribs )
    {
        if ( dwFileAttribs & FILE_ATTRIBUTE_READONLY )
        {
            // File is marked read-only, let's try to fix that
            //
            dwFileAttribs = dwFileAttribs & (~FILE_ATTRIBUTE_READONLY);
            
            SetFileAttributes ( lpcszFilePath, dwFileAttribs );
        }
    }

    // Truncate the file to 0 bytes so the NProtect doesn't
    // put it in the protected Recycle Bin
    //
    FILE * pFile = 0;
    
    pFile = _tfopen ( lpcszFilePath, "w" );

    if ( pFile )
        fclose ( pFile );

    // Try delete
    //
    return (TRUE == DeleteFile ( lpcszFilePath ));
}

STDMETHODIMP CNAVScanTasks::Delete(VARIANT vItem)
{
	// Check our script security
    //
    if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        makeGenericError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    CComPtr <INAVScanTask> sm_pScanTask;
    HRESULT hr = E_FAIL;
    USES_CONVERSION;

    if ( SUCCEEDED (get_Item ( vItem, &sm_pScanTask )))
    {
        // If we aren't allowed to edit it, don't delete it.
        //
        BOOL bCanEdit = false;

        sm_pScanTask->get_CanEdit ( &bCanEdit );

        if ( !bCanEdit )
        {
            CCTRACEI ( "Delete called on uneditable task" );
            return S_FALSE;
        }
        
        // Schedule, if there is one
        //        
        sm_pScanTask->DeleteSchedule ();
        
        CComBSTR bstrTaskPath;

        sm_pScanTask->get_TaskPath( &bstrTaskPath );

        // Delete the file itself
        //
        deleteFileAgressive ( OLE2T(bstrTaskPath) );

        // Remove it from memory
        //
        hr = Remove (vItem);

        if ( forceError (ERR_FAILED_DELETE))
            hr = E_FAIL;

        if ( FAILED (hr))
            makeError ( ERR_FAILED_DELETE, hr );

        return hr;
    }

    // Failed in get_Item which will populate the NAVError itself.
    return E_FAIL;
}

STDMETHODIMP CNAVScanTasks::get_MyComputer(/*[out]*/ INAVScanTask **ppMyComputer)
{
	// Check our script security
    //
    if ( forceError (ERR_SECURITY_FAILED) || !IsItSafe())
    {
        makeGenericError ( ERR_SECURITY_FAILED, E_ACCESSDENIED, IDS_NAVERROR_INTERNAL_REINSTALL );
        return E_ACCESSDENIED;
    }

    if ( !forceError (ERR_NO_MY_COMPUTER))
    {
        ContainerType::iterator it;
        long lType = 0;

        // Find the My Computer task.
        //
        for ( it = m_coll.begin (); it < m_coll.end(); it++ )
        {
            CComPtr <INAVScanTask> spTask = *it;
            spTask->get_TaskType ( &lType);

            if ( lType == scanMyComputer )
            {
                (*ppMyComputer) = spTask;
                (*ppMyComputer)->AddRef ();
                return S_OK;
            }
        
        }
    }

    // Uh oh, couldn't find it.
    //
    CCTRACEI ( "Couldn't find task for ScanMyComputer");
    makeError ( ERR_NO_MY_COMPUTER, E_FAIL );
    return E_FAIL;
}


STDMETHODIMP CNAVScanTasks::ValidateName(BSTR *pbstrTaskName)
{
    try
    {
        if ( forceError (ERR_INVALID_POINTER) || !*pbstrTaskName )
        {
            CCTRACEI ( "Invalid pointer in ValidateName");
            makeGenericError ( ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL );
            return E_POINTER;
        }

        // Check for duplicate task names
        //
        ContainerType::iterator it;

        // Find the My Computer task.
        //
        for ( it = m_coll.begin (); it != m_coll.end(); it++ )
        {
            CComPtr <INAVScanTask> spTask = *it;
            _bstr_t bstrNewName = *pbstrTaskName;
            
            BSTR bstrTempName;
            
            spTask->get_TaskName ( &bstrTempName);

            _bstr_t bstrTestName(bstrTempName,false);
        
            if ( 0 == _tcsicmp ( bstrTestName, bstrNewName ))
            {
                return S_FALSE;
            }
        }

        return S_OK;
    }
    catch ( _com_error e )
    {
        return e.Error ();
    }
}

STDMETHODIMP CNAVScanTasks::get_TaskIndex(BSTR *pbstrTaskName, long *pVal)
{
    *pVal = -1;

    if (forceError (ERR_INVALID_POINTER) || !*pbstrTaskName )
    {
        CCTRACEI ( "Invalid pointer in get_TaskIndex");
        makeGenericError ( ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL );
        return E_POINTER;
    }

    if ( !forceError (ERR_FAILED_FIND_TASK))
    {
        // Find that task name!
        //
        ContainerType::iterator it;
        int iIndex = 0;

        // Find the task.
        //
        for ( it = m_coll.begin (); it != m_coll.end(); it++ )
        {
            CComPtr <INAVScanTask> spTask = *it;
            _bstr_t bstrNewName = *pbstrTaskName;
        
            BSTR bstrTempName;
        
            spTask->get_TaskName ( &bstrTempName);

            _bstr_t bstrTestName(bstrTempName,false);
    
            if ( 0 == _tcsicmp ( bstrTestName, bstrNewName ))
            {
                *pVal = iIndex;
                return S_OK;
            }

            iIndex ++;
        }
    }

    makeGenericError ( ERR_FAILED_FIND_TASK, E_FAIL, IDS_NAVERROR_INTERNAL );
    return E_FAIL;
}

STDMETHODIMP CNAVScanTasks::get_NAVError(INAVCOMError **pVal)
{
	if ( !m_spError )
        return E_FAIL;

    *pVal = m_spError;
    (*pVal)->AddRef (); // We get a ref and the caller gets one

	return S_OK;
}

void CNAVScanTasks::makeError(long lMessageID, long lHResult)
{
    if ( !m_spError )
        return;

	CString csMessage;
	CString csFormat;

	g_ResLoader.LoadString(lMessageID,csFormat);
	csMessage = csFormat;

	switch(lMessageID)
	{
	case ERR_UNKNOWN_SAVE:
	case ERR_UNKNOWN_SCHEDULE:
	case ERR_NO_MY_COMPUTER:
	case ERR_NO_QUICKSCANTASK:
	case ERR_SCHEDULE_FAILED:
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

void CNAVScanTasks::makeGenericError(long lErrorID, long lHResult, long lNAVErrorResID)
{
	if ( !m_spError )
		return;

	m_spError->put_ModuleID ( AV_MODULE_ID_SCAN_TASKS );
	m_spError->put_ErrorID ( lErrorID );
	m_spError->put_ErrorResourceID ( lNAVErrorResID );
	m_spError->put_HResult ( lHResult );
}

bool CNAVScanTasks::forceError(long lErrorID)
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

STDMETHODIMP CNAVScanTasks::ScanMyComputer()
{
    return ScanMyComputer(false);
}

STDMETHODIMP CNAVScanTasks::ScanMyComputerAndWait()
{
    return ScanMyComputer(true);
}
