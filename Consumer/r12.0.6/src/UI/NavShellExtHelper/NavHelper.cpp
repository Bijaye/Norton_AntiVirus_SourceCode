// NavHelper.cpp : Implementation of CNavHelper

#include "stdafx.h"
#include "NavHelper.h"

#include "ScanTask.h"
#include "navtrust.h"       // For loading Sym COM objects
#include "IEContextMenuHelper.h"

// I don't believe this is needed anymore - Javed 6/5/2005
//#include "DJSMAR00_Static.h"

#include "InstOptsNames.h"
#include "NAVSettingsHelperEx.h"
#include "SSOsinfo.h"
//#include "InstOptsNames.h"
#include "NavUIHelp.h"
#include "NAVUIHTM_Resource.h"
#include "TraceHR.h"

// PEP Headers
#define _V2AUTHORIZATION_SIMON
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "ComponentPepHelper.h"
#include "PepMiddleMan.h"

#include "SuiteOwnerHelper.h"
#include "NAVOptHelperEx.h"
#include "switches.h"
// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
#include "ccTrace.h"
#include "ccSymDebugOutput.h"
#include "NavTaskLoader.h"
#include "avresbranding.h"

using namespace std;
bool _g_cIsDiskFull(LPCTSTR lpcRoot);

// CNavHelper
// Utility functions object
ccLib::COSInfo  g_OSInfo;
CNAVInfo g_NAVInfo;


typedef LANGID (WINAPI *LPFNGetSystemDefaultUILanguage)();
typedef LANGID (WINAPI *LPFNGetSystemDefaultLangID)();
typedef LANGID (WINAPI *LPFNGetUserDefaultUILanguage)();
typedef LANGID (WINAPI *LPFNGetUserDefaultLangID)();


void ProcessLanguageIds()
{
    CCTRACEI(_T("ProcessLanguageIds - GetSystemDefaultLCID() returned %d"), GetSystemDefaultLCID());
    CCTRACEI(_T("ProcessLanguageIds - GetUserDefaultLCID() returned %d"), GetSystemDefaultLCID());

    CPINFOEX cpinfoACP;
    CPINFOEX cpinfoTACP;

    ZeroMemory(&cpinfoACP, sizeof(CPINFOEX));
    ZeroMemory(&cpinfoTACP, sizeof(CPINFOEX));

    GetCPInfoEx(CP_ACP, NULL, &cpinfoACP);
    GetCPInfoEx(CP_THREAD_ACP, NULL, &cpinfoTACP);
    
    CCTRACEI(_T("ProcessLanguageIds - CP_ACP = %s"), cpinfoACP.CodePageName);
    CCTRACEI(_T("ProcessLanguageIds - CP_THREAD_ACP = %s"), cpinfoTACP.CodePageName);

    LPFNGetSystemDefaultUILanguage FNGetSystemDefaultUILanguage = 0;
    LPFNGetSystemDefaultLangID FNGetSystemDefaultLangID = 0;
    LPFNGetUserDefaultUILanguage FNGetUserDefaultUILanguage = 0;
    LPFNGetUserDefaultLangID FNGetUserDefaultLangID = 0;

    HMODULE hmKernel32 = GetModuleHandle(_T("Kernel32.DLL"));
    if(hmKernel32)
    {
        FNGetSystemDefaultUILanguage = (LPFNGetSystemDefaultUILanguage)GetProcAddress(hmKernel32, "GetSystemDefaultUILanguage");
        FNGetSystemDefaultLangID = (LPFNGetSystemDefaultLangID)GetProcAddress(hmKernel32, "GetSystemDefaultLangID");
        FNGetUserDefaultUILanguage = (LPFNGetUserDefaultUILanguage)GetProcAddress(hmKernel32, "GetUserDefaultUILanguage");
        FNGetUserDefaultLangID = (LPFNGetUserDefaultLangID)GetProcAddress(hmKernel32, "GetUserDefaultLangID");
    }

    if( !(FNGetSystemDefaultUILanguage && FNGetSystemDefaultLangID && FNGetUserDefaultUILanguage && FNGetUserDefaultLangID) )
    {
        CCTRACEI(_T("ProcessLanguageIds - Unable to get language functions."));
        return;
    }
    
    CCTRACEI(_T("ProcessLanguageIds - GetSystemDefaultUILanguage() returned %d"), FNGetSystemDefaultUILanguage());
    CCTRACEI(_T("ProcessLanguageIds - GetSystemDefaultLangID() returned %d"), FNGetSystemDefaultLangID());
    CCTRACEI(_T("ProcessLanguageIds - GetUserDefaultUILanguage() returned %d"), FNGetUserDefaultUILanguage());
    CCTRACEI(_T("ProcessLanguageIds - GetUserDefaultLangID() returned %d"), FNGetUserDefaultLangID());


};


static BOOL SafeArrayGet(SAFEARRAY FAR *psa, long lIndex, TSTRING *sz ) //LPCSTR *pstr)
{
    _bstr_t cbstrVal;
    HRESULT hResult = SafeArrayGetElement(psa,&lIndex, cbstrVal.GetAddress());
    
    //
    // As a general rule, CP_ACP is a pretty safe bet, however, on THAI
    //  machines, it seems like it will fail from this COM object for 
    //  reasons I don't understand.  So, I'm gonna work around it by
    //  comparing he results strings and make sure that no corruption
    //  has occurred.
    //
    
    *sz = CW2T((LPCWSTR)cbstrVal, CP_ACP);  
    if(0 != wcscmp((LPCWSTR)CT2W(sz->c_str(), CP_ACP), (LPCWSTR)cbstrVal))
    {
        CCTRACEI(_T("SafeArrayGet - CP_ACP conversion failed. sz = %s"), sz->c_str());

        // CP_ACP didn't work, try CP_THREAD_ACP
        *sz = CW2T((LPCWSTR)cbstrVal, CP_THREAD_ACP);
        if(0 != wcscmp((LPCWSTR)CT2W(sz->c_str(), CP_THREAD_ACP), (LPCWSTR)cbstrVal))
        {
            CCTRACEI(_T("SafeArrayGet - CP_THREAD_ACP conversion failed. sz = %s"), sz->c_str());
            return FALSE;
        }
        
        CCTRACEI(_T("SafeArrayGet - CP_THREAD_ACP conversion succeeded. sz = %s"), sz->c_str());
    }


    return SUCCEEDED(hResult);
}

// Returns 0 if no items to add, else returns actual number of items.
int	CNavHelper::Add2List(VARIANTARG vt,VTSTR &vStr)
{
	if( vt.vt != ( VT_BSTR | VT_ARRAY ) )
	{
		return 0;
	}

	long lLBound,lUBound,lElements;
	SAFEARRAY *pSa = vt.parray;
	HRESULT hr = S_OK;
	// Get lower array bound.
	hr = SafeArrayGetLBound(pSa, 1, &lLBound);	
	// Get upper array bound.
	hr = SafeArrayGetUBound(pSa, 1, &lUBound);

	// Number of elements in the OLE safearray.
	lElements = lUBound-lLBound+1; 	

	// Make sure we have items to work with.
	if(lElements == 0)
	{
		return 0;
	}

	vStr.clear();
	for (long lIndex = 0; lIndex < lElements; lIndex++)
	{
		TSTRING sz;
		SafeArrayGet(pSa,lIndex,&sz);
		vStr.push_back(sz);
	}
	return lElements;
}


HRESULT CNavHelper::RunNavW(VARIANTARG vtItems, VARIANTARG vtDeniels)
{
//	MessageBox(NULL,"RunNavW()","Break",MB_OK);
	TRACEHR (hr);
	if( S_OK != (hr = IsConfigured() ))
		return hr;

    ProcessLanguageIds();

    hr = S_OK;
#ifdef IMPERSONATE_CLIENT
	hr = ::CoImpersonateClient();
	if(FAILED(hr))
		return hr;
#endif
    USES_CONVERSION;
	TCHAR szTempFilePath[MAX_PATH*2];
	NavScanTaskLoader taskLoader;
	IScanTaskPtr pScanTask;
	try
	{
		SYMRESULT SR = SYM_OK;
		if( SYM_OK != (SR =  taskLoader.CreateObject(&pScanTask) ) ) {
			CCTRACEE(_T("CNavHelper::RunNavW() Failed to get IScanTaskPtr %d"),SR);
			return Error(IDS_Err_Internal,IID_INavHelper);
		}

		Add2List(vtItems,m_vItems);
		Add2List(vtDeniels,m_vDeniels);
		// Make sure we have items to work with.
		if(m_vItems.size() == 0) {
			// Show inaccessible items.
			if( !ShowDeniels(::GetActiveWindow()) ) {
				CString csError;
				hr = ErrorFromResource(IDS_NOITEMS,IID_INavHelper,csError);
				CCTRACEE(_T("CNavHelper::RunNavW() Failed - %s"),csError);
				return hr;
			}
			return S_OK;		// we have already alrted the user of failure
		}

		pScanTask->SetType(scanCustom);
		// Save the list of the files to scan into a task file.
		for (vector<TSTRING>::iterator it = m_vItems.begin(); m_vItems.end() != it; it++)
		{
			// Mark everything as a file even folders and drives.
			// Since this will be picked by NAVW.EXE we don't care what type of an item it is.
			// NAVW feed the information to the scanner and it'll figure out what to do.
			if (!pScanTask->AddItem(typeFile, subtypeNone, NULL, (*it).c_str(), 0)) {
				CCTRACEE(_T("CNavHelper::RunNavW() !pScanTask->AddItem() - %s"),(*it).c_str());
				return  Error(IDS_Err_ScanTaskItem,IID_INavHelper);
			}
		}

		// Get a temp file name.
		DWORD_PTR dwError = generateTempScanFile( szTempFilePath, MAX_PATH*2 );
		if(dwError != NOERROR) {
			CCTRACEE(_T("CNavHelper::RunNavW() IDS_Err_ScanTaskFile"));
			return  Error(IDS_Err_ScanTaskFile,IID_INavHelper);
		}
		pScanTask->SetPath ( szTempFilePath );
	}
	catch(_com_error& e) {
		hr = e;
	}
	catch(...) {
        hr = E_UNEXPECTED;
		CCTRACEE("*** Error");
	}
	if(FAILED(hr))
		return hr;

	if (!pScanTask->Save()) {
        DWORD dwLastError = ::GetLastError();
		DeleteFile ( szTempFilePath );

        // Tell the user we couldn't save the task
        int nReturn = 0;
        if(ERROR_DISK_FULL == dwLastError || ERROR_HANDLE_DISK_FULL == dwLastError) {
            nReturn = IDS_Err_Cant_Save_Task;
        }
        else {
		    TCHAR szRoot[] = _T("?:\\");
			szRoot[0] = szTempFilePath[0];
			nReturn = ( _g_cIsDiskFull(szRoot)?IDS_Err_Cant_Save_Task:IDS_Err_ScanTaskFile );
        }
		CCTRACEE(_T("CNavHelper::RunNavW() Failed to save scan task"));
        return  Error(nReturn,IID_INavHelper);
	}

    // Command line is c:\progra~1\norton~1\navw32.exe /SE- /ttask:"c:\...\task.sca"
    std::string strExePath, strParameters;
    CNAVInfo NAVInfo;
    TCHAR szShortNAVDir [MAX_PATH] = {0};
    TCHAR szShortTaskName [MAX_PATH] = {0};

    ::GetShortPathName ( NAVInfo.GetNAVDir (), szShortNAVDir, MAX_PATH );
    ::GetShortPathName ( szTempFilePath, szShortTaskName, MAX_PATH );

    strExePath = szShortNAVDir;
    strExePath += _T("\\navw32.exe");
    strParameters = SWITCH_DISABLE_ERASER_SCAN;
    strParameters += _T(" /ttask:\"");
    strParameters += szShortTaskName;
    strParameters += _T("\"");

	CCTRACEI(_T("CNavHelper::RunNavW()- ShellExecute(\"Open %s%s\"\")"));
	if (HINSTANCE (32) >= ::ShellExecute(   ::GetDesktopWindow(),
                                            _T("open"),         // Verb
	                                        strExePath.c_str(),     // File
                                            strParameters.c_str(),  // Parameters
                                            NULL,                   // Directory
	                                        SW_SHOW))               // ShowCmd
    {
		return Error(IDS_Err_ScanTaskRun,IID_INavHelper);
    }

	return hr;
}

// GHS: 362840 I am returning the reason for the failure if it is NOT ERROR_FILE_EXISTS
DWORD_PTR CNavHelper::generateTempScanFile( LPTSTR szOut, int nBufferSize )
{
	TCHAR szTempDir[ MAX_PATH*2 ];
	TCHAR szTempFile[ 32 ];

    DWORD dwLastError = NOERROR;
	// Fetch temp directory.
	if( 0 == ::GetTempPath(MAX_PATH*2, szTempDir) )
    {
        dwLastError = ::GetLastError();
        return (dwLastError == NOERROR)?(DWORD_PTR)(-1):dwLastError;
    }

	// Keep trying to generate a temp file.
	int iIndex = 0;
	for(;; ++iIndex)
	{
		// Copy directory name to output buffer.
		_tcsncpy( szOut, szTempDir, nBufferSize );

		// Construct file name.  File will be in the form
		// scanxxxx.sca.
		wsprintf( szTempFile, "scan%d.%s", iIndex, szTaskFileExtension );
		_tcsncat( szOut, szTempFile, nBufferSize );

		// Try to create the file.
		HANDLE hFile = CreateFile( szOut, GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL );
		if( hFile != INVALID_HANDLE_VALUE )
		{
			// Success.  Close the handle and return.
			CloseHandle( hFile );
			return NOERROR;
		}

		// An error occured.  If the error states that the file
		// already exists, keep processing, else bail out.
        // GHS: 362840 I am return the reason for the failure if it is NOT ERROR_FILE_EXISTS
        DWORD dwLastError = ::GetLastError();
		if( dwLastError != ERROR_FILE_EXISTS )
            return dwLastError;
	}

	// Return error status.
	return NOERROR;
}

bool CNavHelper::ShowDeniels(HWND hWnd)
{
	if(m_vDeniels.size() != 0)
	{
		TCHAR szMsg[256];
		CString szTitle(m_csProductName);
		::LoadString(_AtlBaseModule.GetResourceInstance(), IDS_ACCESSDENIED, szMsg,   SIZEOF(szMsg));

		TSTRING strMsg = szMsg;
		// Create list of files denied access to
		for (vector<TSTRING>::iterator it = m_vDeniels.begin(); m_vDeniels.end() != it; it++)
		{
			strMsg += _T("\n");
			strMsg += *it;
		}

		// Means nothing is selected.
		MessageBox(hWnd, strMsg.c_str(), szTitle, MB_ICONEXCLAMATION | MB_OK);
		m_vDeniels.clear();

		return true;
	}

	return false;
}

// GHS: 362840 I am checking for low disk space and return that as an error when it occurs.
bool _g_cIsDiskFull(LPCTSTR lpcRoot)
{
    
    ULARGE_INTEGER uliFreeBytesAvailable        = {0};  // bytes available to caller
    ULARGE_INTEGER uliTotalNumberOfBytes        = {0};  // bytes on disk
    ULARGE_INTEGER uliTotalNumberOfFreeBytes    = {0};  // free bytes on disk
    
    
    BOOL bRet = ::GetDiskFreeSpaceEx(
        lpcRoot,                 // directory name
        &uliFreeBytesAvailable,    // bytes available to caller
        &uliTotalNumberOfBytes,    // bytes on disk
        &uliTotalNumberOfFreeBytes // free bytes on disk
        );
    
    if(!bRet || (uliFreeBytesAvailable.QuadPart < 1000) )
        return true;

    return false;
}
