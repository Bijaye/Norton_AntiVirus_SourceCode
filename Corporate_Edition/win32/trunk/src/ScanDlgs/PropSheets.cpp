// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////
//
//  Author: Randy Templeton
//
//	Purpose: Defines basic functions to place OCXs in a general windows propertySheet Page
//		So they and get used by explorer and the control panel
//	
#include "stdafx.h"
#include "filesystemui.h"
#include "PropSheets.h"
#include "resource.h"
#include "IResultItem.h"
#include "scaninf_i.c"
#include "ReloadServicesDlg.h"
#include "PasswordDlg.h"
#include "ConfigHistory.h"
#include <comdef.h>

#include "ClientReg.h"
#include "OSUtils.h"
#include "SymSaferRegistry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const LPCTSTR CPropSheetEx::DragQueryFileWexportName            = "DragQueryFileW";
const LPCTSTR CPropSheetEx::GetShortPathNameWexportName         = "GetShortPathNameW";
const LPCTSTR CPropSheetEx::ShellDllName                        = "shell32.dll";
const LPCTSTR CPropSheetEx::KernelDllName                       = "kernel32.dll";

CMapPtrToPtr	g_DlgMap;
extern HRESULT CoCreateScanEngine( IID iInterface, void **ppv );

interface IResultsViewImp : public IResultsView
{

	STDMETHOD(AddProgress)(/*in*/PPROGRESSBLOCK Block)=0;
	STDMETHOD(AddLogLine)(/*in*/LPARAM Data,/*in*/char *line,/*in*/char *Description)=0;
};

/////////////////////////////////////////////////////////////////////////////
// CVariantMap

IMPLEMENT_DYNCREATE(CPropSheetEx, CCmdTarget)

CPropSheetEx::CPropSheetEx() : dragQueryFileWptr(NULL), getShortPathnameWptr(NULL), shellLibraryHandle(NULL), kernelLibraryHandle(NULL), osSupportsNoBestFitChars(false)
{
    OSVERSIONINFO osInfo;

    // To keep the application running as long as an OLE automation
    //  object is active, the constructor calls AfxOleLockApp.

    AfxOleLockApp();

    // SafeDragQueryFile initialization
    // Load pointers to DragQueryFileW and GetShortPathNameW, if available, for NT.  Note that on Win9x the
    // functions are exported from these DLLs, but they do not function.
    if (IsWinNT())
    {
		TCHAR szPath[MAX_PATH];
		ZeroMemory(szPath, sizeof(szPath));
		::GetSystemDirectory( szPath, MAX_PATH );
		_tcscat( szPath, _T("\\"));
		_tcscat( szPath, ShellDllName );
        shellLibraryHandle = LoadLibrary(szPath);
        if (shellLibraryHandle != NULL)
            dragQueryFileWptr = (DragQueryFileWfunc) GetProcAddress(shellLibraryHandle, DragQueryFileWexportName);
		ZeroMemory(szPath, sizeof(szPath));
		::GetSystemDirectory( szPath, MAX_PATH );
		_tcscat( szPath, _T("\\"));
		_tcscat( szPath, KernelDllName );
        kernelLibraryHandle = LoadLibrary(KernelDllName);
        if (kernelLibraryHandle != NULL)
            getShortPathnameWptr = (GetShortPathNameWfunc) GetProcAddress(kernelLibraryHandle, GetShortPathNameWexportName);
    }
    // Determine if OS supports WC_NO_BEST_FIT_CHARS.  Only Win98+ and Win2000+ do.
    ZeroMemory(&osInfo, sizeof(osInfo));
    osInfo.dwOSVersionInfoSize = sizeof(osInfo);
    GetVersionEx(&osInfo);
    if ((osInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && ((osInfo.dwMajorVersion > 4) || ((osInfo.dwMajorVersion == 4) && (osInfo.dwMinorVersion > 0))))
        osSupportsNoBestFitChars = true;
    else if ((osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) && (osInfo.dwMajorVersion >= 5))
        osSupportsNoBestFitChars = true;
}

CPropSheetEx::~CPropSheetEx()
{
    // To terminate the application when all objects created with
    //  with OLE automation, the destructor calls AfxOleUnlockApp.

    AfxOleUnlockApp();

    // SafeDragQueryFile shutdown
    dragQueryFileWptr = NULL;
    getShortPathnameWptr = NULL;
    if (shellLibraryHandle != NULL)
    {
        FreeLibrary(shellLibraryHandle);
        shellLibraryHandle = NULL;
    }
    if (kernelLibraryHandle != NULL)
    {
        FreeLibrary(kernelLibraryHandle);
        kernelLibraryHandle = NULL;
    }
}

void CPropSheetEx::OnFinalRelease()
{
	// When the last reference for an automation object is released
	//	OnFinalRelease is called.  This implementation deletes the 
	//	object.  Add additional cleanup required for your object before
	//	deleting it from memory.

	delete this;
}


STDMETHODIMP_(ULONG) CPropSheetEx::XScanImpl::AddRef()
{

    METHOD_PROLOGUE(CPropSheetEx, ScanImpl)    
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CPropSheetEx::XScanImpl::Release()
{
    METHOD_PROLOGUE(CPropSheetEx, ScanImpl)    
	return pThis->ExternalRelease();
}

STDMETHODIMP CPropSheetEx::XScanImpl::QueryInterface(
    REFIID iid, void FAR* FAR* ppvObj)
{
    METHOD_PROLOGUE(CPropSheetEx, ScanImpl)
    return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}


STDMETHODIMP CPropSheetEx::XScanImpl::ShowDetectableVirusesNoProvider(/*in*/LPTSTR buffer,/*in*/unsigned char __RPC_FAR* szComputerName )
{
	ShowDetectableVirusesNoProvider(buffer, szComputerName );
	return S_OK;
}


STDMETHODIMP CPropSheetEx::XScanImpl::CreateConfigPage(ULONG cMode, LONG  *pHandle)
{
 
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	METHOD_PROLOGUE(CPropSheetEx, ScanImpl) 
	PROPSHEETPAGE pp;

	pThis->m_nPageType=cMode;
	   
	pp.dwSize = sizeof(PROPSHEETPAGE); 
    pp.dwFlags=PSP_USEICONID|PSP_USECALLBACK;     
	pp.hInstance=AfxGetInstanceHandle();  
	pp.pszTemplate = MAKEINTRESOURCE(IDD_PROPERTY_PAGE);
	pp.pszIcon=MAKEINTRESOURCE(IDI_SHIELD);
	pp.pszTitle=0;
	pp.pfnDlgProc=PageDlgProc;
	pp.lParam=(LPARAM)pThis;
	pp.pfnCallback=SheetProc;
	pp.pcRefParent=0;

	*pHandle = (LONG)CreatePropertySheetPage(&pp);
	
	if (*pHandle)
	{
		
		AddRef();
		return S_OK;
	}
	return E_FAIL;
}
			

STDMETHODIMP CPropSheetEx::XScanImpl::ShowPasswordDlg(void)
{

	METHOD_PROLOGUE(CPropSheetEx, ScanImpl)
	CPasswordDlg dlg;

	if (!dlg.CheckPassword(""))
	{
		if (dlg.DoModal()==IDOK) 
			return S_OK;
		else
			return S_FALSE;
	}
	return S_OK;
}

STDMETHODIMP CPropSheetEx::XScanImpl::GetServicesState( long *lState )
{
    METHOD_PROLOGUE(CPropSheetEx, ScanImpl)

	HKEY	pKey;
	DWORD	dwRunning = 0,
			dwSize = sizeof( DWORD );
	BOOL	bServiceRunning = FALSE,
			bVPTrayRunning = (BOOL)FindWindow( VPTRAY_CLASS, NULL);


	//Find out if the service is running by looking at the registry key
	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
										szReg_Key_Main,
										0,
										KEY_READ,
										&pKey) )
	{
		//Read the ServiceRunning value
		if( SymSaferRegQueryValueEx(	pKey,
							szReg_Val_ServiceRunning,
							NULL,
							NULL,
							(BYTE*)&dwRunning,
							&dwSize	) == ERROR_SUCCESS ){

			bServiceRunning = (BOOL)(dwRunning != KEYVAL_NOTRUNNING);	
		}
		RegCloseKey( pKey );
	}

	// Since this method returns a value that can be set into a 
	//	SetCheck call, the return values are as follows:
	// 1 == All services running
	// 2 == one, but not both, services running
	// 0 == no services running
	if( bVPTrayRunning && bServiceRunning )
		*lState = 1;
	else if( bVPTrayRunning || bServiceRunning )
		*lState = 2;
	else
		*lState = 0;

	return S_OK;
}

STDMETHODIMP CPropSheetEx::XScanImpl::UnLoadServices()
{
	HRESULT				hr = E_FAIL;
	HWND				hWnd;
	CString				strRegValue;
	HKEY				pKey;
	CString				strRTVDir,
						strRTVExe;
	IServiceControl		*ptrService = NULL;
//	long				lServiceState;

	if( hWnd = FindWindow( VPTRAY_CLASS, NULL) )
	{
		PostMessage( hWnd, WM_DESTROY, 0, 0 );
	}
	
	//Now, remove it from the runline
	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
										szReg_Val_vprun,
										0,
										KEY_SET_VALUE,
										&pKey) )
	{
		RegDeleteValue(	pKey, szReg_Val_vptray );

		RegCloseKey( pKey );
	}

	//Now, unload the Service
	if( SUCCEEDED( CoCreateLDVPObject( CLSID_CliProxy, IID_IServiceControl, (void**)&ptrService ) ) )
	{
		//Stop the service
		hr = ptrService->Stop();
		//And tell it not to start later
		ptrService->SetLoadMode( 0 );
		ptrService->Release();
	}

	//If I think I stopped the services, double-check
//	if( SUCCEEDED( hr ) )
//		hr = (SUCCEEDED(GetServicesState( &lServiceState ))) ? E_FAIL : S_OK;

	return hr;
}

STDMETHODIMP CPropSheetEx::XScanImpl::LoadServices()
{
    METHOD_PROLOGUE(CPropSheetEx, ScanImpl)

	HRESULT				hr = E_FAIL;
	HKEY				pKey;
	CString				strDir,
						strExe;
	STARTUPINFO			si;
	PROCESS_INFORMATION pInfo;
	IVirusProtect		*ptrVirusProtect = NULL;
	IServiceControl		*ptrService = NULL;
//	long				lServiceState;

	//Find out where LDVPTray is and load it
	//First, get the info from the main key
	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
										szReg_Key_Main,
										0,
										KEY_READ,
										&pKey) )
	{
		DWORD dwSize = MAX_PATH;
		LPSTR pTemp = strDir.GetBuffer( MAX_PATH );
		SymSaferRegQueryValueEx(pKey,
						szReg_Val_LocalAppDir,
						NULL,
						NULL,
						(BYTE*)pTemp,
						&dwSize );
		strDir.ReleaseBuffer();

		RegCloseKey( pKey );
	}

	//Now, try to start it
	if( !strDir.IsEmpty() )
	{
		strExe.Format( "%s\\vptray.exe", strDir ); // Note: full path to exe must not have quotes around it for this case.

		memset( &si, 0x00, sizeof( si ) );
		memset( &pInfo, 0x00, sizeof( pInfo ) );
		
		si.cb = sizeof( si );

        // Note: using full path in strExe is safe without quotes, cannot use quotes in first parameter.  It is interpreted as part of executable name.
		if( CreateProcess(	strExe,
						NULL,
						NULL,
						NULL,
						FALSE,
						CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP | NORMAL_PRIORITY_CLASS,
						NULL,
						strDir,
						&si,
						&pInfo ) ){
			CloseHandle(pInfo.hProcess);
			CloseHandle(pInfo.hThread);
		}

		//And add it to the Run line
		if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
											szReg_Val_vprun,
											0,
											KEY_SET_VALUE,
											&pKey) )
		{
			
			CString sTemp(strDir);
			CString sShortPath;
			
			//Fix for defect 384649.If Long path name are put in the run key some occasion OS fails to execute vptray hence putting
			//short path name
			if(GetShortPathName(sTemp, sShortPath.GetBuffer(strDir.GetLength()),strDir.GetLength()) != 0)
				strExe.Format( "%s\\vptray.exe", sShortPath );
			
			RegSetValueEx(	pKey,
							szReg_Val_vptray,
							0,
							REG_SZ,
							(BYTE*)(LPCTSTR)strExe,
							strExe.GetLength() );
			
			sShortPath.ReleaseBuffer();

			RegCloseKey( pKey );
		}
	}

	//Now, load the Service
	if( SUCCEEDED( CoCreateLDVPObject( CLSID_CliProxy, IID_IServiceControl, (void**)&ptrService ) ) )
	{
		//Start the service
		hr = ptrService->Start();

		//And tell it to start later as well
		ptrService->SetLoadMode( 1 );
		ptrService->Release();
	}

	//Now, if hr is S_OK, then I think I started both vptray and the service - let's double-check
//	if( SUCCEEDED( hr ) )
//		hr = (SUCCEEDED(GetServicesState( &lServiceState ) ) ) ? S_OK : E_FAIL;

	return hr;
}

STDMETHODIMP CPropSheetEx::XScanImpl::ServiceRunningDlg( BSTR bstrOption )
{
    METHOD_PROLOGUE(CPropSheetEx, ScanImpl)

	CReloadServicesDlg	dlg;
	CString				strText,
						strOption( bstrOption );
	long				lServicesState;
	HRESULT				hr = E_FAIL;

	AfxFormatString1( strText, IDS_SERVICES_NOT_RUNNING, strOption );
	dlg.m_text = strText;

	//Only if the service is NOT running do I want to display this message
	GetServicesState( &lServicesState );
	if( lServicesState != 1 )
	{
		if( IDOK == dlg.DoModal() )
		{
			hr = LoadServices();

			//If I couldn't load the services, display an error dialog
			if( FAILED( hr ) )
			{
				LPSTR	szError = NULL;
				CString	strError;

				//First, see if it is a standard windows error message
				if( FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
									NULL,
									(DWORD)hr,
									0,
									szError,
									256,
									NULL ) )
				{
					//Display the message
					AfxMessageBox( szError );
					LocalFree( szError );
				}
				else
				{
					//If I couldn't format the message, then it is probably a
					// LDVP-specific error code
					if( hr == ERROR_REG_FAIL )
						strError.LoadString( IDS_ERROR_REG_FAIL );
					else if( hr == ERROR_SERVICE_HUNG )
						strError.LoadString( IDS_ERROR_SERVICE_HUNG );
					else
						strError.LoadString( IDS_ERROR_UNKNOWN );

					AfxMessageBox( strError );
				}
			}
		}
	}
	
	return hr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Function name: IsDiskInRemovableDrive
//
// Description  : Determine if there is a disk in the removable drive
//                specified by sFile.
//                
//
// Return type  : BOOL
//
// Returns      TRUE    if there is a disk in removable drive
//              FALSE   if the drive is empty 
//////////////////////////////////////////////////////////////////////////////////////////////////
//  4/07/2000   RCHINTA - Created
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL IsDiskInRemovableDrive(LPCTSTR sFile)
{
	BOOL 			bRetry = FALSE;
	UINT			uintPrevState = SetErrorMode( SEM_FAILCRITICALERRORS );
	WIN32_FIND_DATA	info;
	CString			strDrive;
	HANDLE			hFind;
	BOOL            bDiskInDrive = FALSE;

	do
	{
		//Start by setting Retry to FALSE. This is so
		//	it will be reset each time I go through the
		//	loop.
		bRetry = FALSE;

		strDrive.Format("%s*.*", sFile);	    
		memset( &info, '\0', sizeof( info ) );
		
		//If the FindFirst fails, then we can't get to the
		//	drive.
		if( INVALID_HANDLE_VALUE != ( hFind = FindFirstFile( (LPCTSTR)strDrive,  &info ) ) )
		{
			bRetry = FALSE;
			bDiskInDrive = TRUE;
		}
		else // check for empty disk
		{               

            if(GetLastError() == ERROR_NOT_READY)   // empty CDROM or removable drive
            {
				CString strError;
                CString strTitle;
				strDrive.Format( "%s", sFile );	    
		
                strTitle.LoadString(IDS_TITLE);
				AfxFormatString1( strError, IDS_DRIVE_NOT_READY, (LPCTSTR)strDrive );
				if( IDRETRY == MessageBox(NULL,strError, strTitle, MB_RETRYCANCEL | MB_ICONEXCLAMATION ) )
				{   
					bRetry = TRUE;
				}
				else
				{
                    bRetry = FALSE;
					FindClose( hFind );
				}

			}   // end of check for ERROR_NOT_READY

		}   //  done with the check for empty disk

		FindClose( hFind );
	}while( bRetry );
							
	SetErrorMode( uintPrevState );

    return bDiskInDrive;
}


STDMETHODIMP CPropSheetEx::XScanImpl::ScanDropFiles( long hDrop )
{
	METHOD_PROLOGUE(CPropSheetEx, ScanImpl)

	HRESULT hr;
	IScanConfig *pConfig=NULL,
				*pManual=NULL;
	IScan       *pScan=NULL;
	TCHAR sNewKey[MAX_PATH] = _T("\0");
    TCHAR sFile[IMAX_PATH] = _T("\0");
	TCHAR sNum[40];
	SYSTEMTIME tTime;
	FILETIME   fTime;
	UINT cCount,i;
	DWORD dwAttr,tval=1,fval=0;
	BOOL bDoBoot=FALSE;
	CString sTitle;
	CString sDirve;
    BOOL    usedDefaultChar                 = FALSE;
    WCHAR   wideNameBuffer[IMAX_PATH]       = {L""};
    WCHAR   shortNameBuffer[2*MAX_PATH+1]   = {L""};

	//ASSERT(FALSE);
	hr = CoCreateLDVPObject(CLSID_CliProxy,IID_IScanConfig,(void**)&pConfig);
	if (hr != S_OK) goto _cleanup;
	
	hr = CoCreateLDVPObject(CLSID_CliProxy,IID_IScanConfig,(void**)&pManual);
	if (hr != S_OK) goto _cleanup;

	hr = CoCreateLDVPObject(CLSID_CliProxy,IID_IScan,(void**)&pScan);
	if (hr != S_OK) goto _cleanup;

	hr = pManual->Open(NULL,HKEY_VP_USER_SCANS,szReg_Key_Scan_Defaults);
	if (hr != S_OK) goto _cleanup;

	// generate a unique temp key name under custom tasks
	lstrcpy(sNewKey,"TEMP");
	GetSystemTime(&tTime);
	SystemTimeToFileTime(&tTime,&fTime);
	ultoa(fTime.dwLowDateTime,sNum,10);
	lstrcat(sNewKey,sNum);
	ultoa(fTime.dwHighDateTime,sNum,10);
	lstrcat(sNewKey,sNum);

	hr = pConfig->Open(NULL,HKEY_VP_USER_SCANS,sNewKey);
	if (hr != S_OK) goto _cleanup;

	hr = pConfig->ChangeCloseMode(TRUE);

	hr = pConfig->CopyFromInterface(pManual);
	if (hr != S_OK) goto _cleanup;


	// make sure we scan all files
	if (hDrop)
	{
		hr = pConfig->SetOption(szReg_Val_ScanAllDrives,REG_DWORD,(BYTE*)&fval,sizeof(DWORD));
		//hr = pConfig->SetOption(szReg_Val_AllFiles,REG_DWORD,(BYTE*)&fval,sizeof(DWORD));
		cCount = DragQueryFile((HDROP)hDrop,(UINT)-1,sFile,IMAX_PATH);
	}
	else // we want to scan the computer
	{
		hr = pConfig->SetOption(szReg_Val_ScanAllDrives,REG_DWORD,(BYTE*)&tval,sizeof(DWORD));

		hr = pConfig->SetOption(szReg_Val_ScanBootSector,REG_DWORD,(BYTE*)&tval,sizeof(DWORD));
	}
	
	// set the directories
	hr = pConfig->SetSubOption(szReg_Key_Directories);
	hr = pConfig->ClearOptions();


	// loop through each dir in the drop list and addit to the config
	for (i=0; (i < cCount) && hDrop;i++)
	{
		sFile[0]='\0';

        // Get the directory's name
        pThis->SafeDragQueryFile((HDROP) hDrop, i, sFile, sizeof(sFile));

        // Save the SFN if the path is greater than or equal to MAX_PATH.
        // This could occur with UNC's that include very long LFN, long share names etc.
        if( lstrlen(sFile) >= MAX_PATH)
            GetShortPathName(sFile, sFile, IMAX_PATH);

        // Check if the path is a UNC path
        if(sFile[0] == _T('\\') && sFile[1] == _T('\\') )
        {
            if (ShowPasswordDlg() != S_OK)
                //If we don't break out, the file gets scanned anyway if the user 
                //selects 'Cancel' in the dialog box.  Also below in next 'else' block
 				goto _cleanup;
        }
        else
        {

	        // we were a path now check the root for a network
	        char t=sFile[3];
	        sFile[3]='\0';
	        dwAttr = GetDriveType(sFile);
	        sFile[3]=t;

	        if (dwAttr ==DRIVE_REMOTE)
	        {
		        if (ShowPasswordDlg() != S_OK)
 					goto _cleanup;		//see note in preceding 'if' block

	        }
            else if ( (dwAttr == DRIVE_REMOVABLE || dwAttr == DRIVE_CDROM) && _tcslen(sFile) == _tcslen(_T("A:\\")) )
            {
                if(!IsDiskInRemovableDrive(sFile))
                    goto _cleanup;
            }   // done with remote drive or removable drive
        }

		if (sFile[lstrlen(sFile) -1] == _T(' ') )
		{
			TCHAR sTmp[IMAX_PATH] = _T("\0");
			lstrcpy(sTmp,"\\\\?\\");
			lstrcat(sTmp,sFile);
			dwAttr = GetFileAttributes(sTmp);
		}
		else
			dwAttr = GetFileAttributes(sFile);

		if (dwAttr != (DWORD)-1 && dwAttr & FILE_ATTRIBUTE_DIRECTORY)
		{
			// we know we are a directory now see if we are a drive
			dwAttr = GetDriveType(sFile);
			if (dwAttr != DRIVE_NO_ROOT_DIR && dwAttr != DRIVE_UNKNOWN)
			{
				
				bDoBoot = TRUE;	// we are a drive
				sFile[2]='\0';
			}		
 
		
			pConfig->SetOption(sFile,REG_DWORD,(BYTE*)&tval,sizeof(DWORD));
		}
	}
 
	// set the files
	hr = pConfig->SetSubOption(NULL);
	hr = pConfig->SetSubOption(szReg_Key_Files);
	hr = pConfig->ClearOptions();

	// loop through each file in the drop list and set it as an option
	for (i=0; (i < cCount) && hDrop;i++)
	{
		sFile[0]='\0';

        // Get the file's name.
        pThis->SafeDragQueryFile((HDROP) hDrop, i, sFile, sizeof(sFile));

        // Save the SFN if the path is greater than or equal to MAX_PATH.
        // This could occur with UNC's that include very long LFN, long share names etc.
        if( lstrlen(sFile) >= MAX_PATH)
            GetShortPathName(sFile, sFile, IMAX_PATH);

		if (sFile[lstrlen(sFile) -1] == _T(' ') )
		{
			TCHAR sTmp[IMAX_PATH] = _T("\0");
			lstrcpy(sTmp,"\\\\?\\");
			lstrcat(sTmp,sFile);
			dwAttr = GetFileAttributes(sTmp);
			if (dwAttr != (DWORD)-1 && !(dwAttr & FILE_ATTRIBUTE_DIRECTORY))
			{
				hr = pConfig->SetOption(sTmp,REG_DWORD,(BYTE*)&tval,sizeof(DWORD));
			}
		}
		else
		{
			dwAttr = GetFileAttributes(sFile);		
			if (dwAttr != (DWORD)-1 && !(dwAttr & FILE_ATTRIBUTE_DIRECTORY))
			{
				hr = pConfig->SetOption(sFile,REG_DWORD,(BYTE*)&tval,sizeof(DWORD));
			}
		}
		
	}

	//ASSERT(FALSE);
	hr = pConfig->SetSubOption(NULL);

	// do we want the boot sector?
	if (bDoBoot)
		hr = pConfig->SetOption(szReg_Val_ScanBootSector,REG_DWORD,(BYTE*)&tval,sizeof(DWORD));

	// never lock the scan
	hr = pConfig->SetOption(szReg_Value_ScanLocked,REG_DWORD,(BYTE*)&fval,sizeof(DWORD));

    hr = pConfig->SetOption(szReg_Val_ScanLoadpoints,REG_DWORD,(BYTE*)&fval,sizeof(DWORD));
    hr = pConfig->SetOption(szReg_Val_ScanERASERDefs,REG_DWORD,(BYTE*)&fval,sizeof(DWORD));
    hr = pConfig->SetOption(szReg_Val_ScanProcesses,REG_DWORD,(BYTE*)&fval,sizeof(DWORD));
    
	// always do the status dialog
	hr = pConfig->SetOption(szReg_Value_DisplayStatusDlg,REG_DWORD,(BYTE*)&tval,sizeof(DWORD));
	
	// set the status dialog title
	sTitle.LoadString(IDS_TITLE);
	hr = pConfig->SetOption(szReg_Value_ScanTitle,REG_SZ,(LPBYTE)(LPCSTR)sTitle,sTitle.GetLength()+1);
	
	hr = pScan->Open(NULL,pConfig);
	if (hr == S_OK)
		hr = pScan->StartScan(TRUE /*Launch Async*/);

_cleanup:
	if (pConfig)
        pConfig->Release();
	if (pManual)
        pManual->Release();
	if (pScan)
        pScan->Release();

	return hr;
}

HRESULT CPropSheetEx::SafeDragQueryFile( HDROP dropHandle, UINT fileNumber, LPTSTR filenameBuffer, DWORD filenameBufferSize )
// As DragQueryFile, but does it's best to retrieve a *good* ANSI MBCS filename for the identified file.
// Filenames are stored in Unicode, which does not always translate to the shell's ANSI code page, resulting in
// bad filenames.  The workaround code will return an alternate filename (SFN) usable to open the file.
// FilenameBufferSize is in bytes.
// Returns S_OK on success and no char translation issues detected, S_FALSE on success with translation done, E_FAIL if GetShortPathNameW
// needed but not found, else the error code of the failure.
{
    BOOL    usedDefaultChar                 = FALSE;
    WCHAR   wideNameBuffer[2*MAX_PATH+1]    = {L""};
    WCHAR   shortNameBuffer[2*MAX_PATH+1]   = {L""};
    DWORD   returnValDW                     = ERROR_FILE_NOT_FOUND;
    HRESULT returnValHR                     = E_FAIL;

    // Get the file's name.
    // The shell's code page is ANSI.  It translates the filename from Unicode into this codepage before returning
    // the filename.  If there are characters outside the code page, such as high ASCII characters, they will be
    // translated to the default character, and consequently we will not be able to open/scan the file.
    // Are we on NT or higher, with DragQueryFileW available?
    if (dragQueryFileWptr != NULL)
    {
        // Yes (default char is ?)
        ZeroMemory(&wideNameBuffer, sizeof(wideNameBuffer));
        dragQueryFileWptr(dropHandle, fileNumber, wideNameBuffer, sizeof(wideNameBuffer)/sizeof(wideNameBuffer[0]));
        // Can all characters in this filename be translated directly into shell's ANSI code page?
        if (osSupportsNoBestFitChars)
        {
            returnValDW = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, wideNameBuffer, -1, filenameBuffer, filenameBufferSize, NULL, &usedDefaultChar);
        }
        else
        {
            // NT4 does not support WC_NO_BEST_FIT_CHARS, so we must provide an alternate mechanism for it.  To do this, we perform a round trip conversion
            // and then compare the results
            WCHAR returnTripWideName[2*MAX_PATH+1] = {L""};
            WideCharToMultiByte(CP_ACP, NULL, wideNameBuffer, -1, filenameBuffer, filenameBufferSize, NULL, NULL);
            MultiByteToWideChar(CP_ACP, NULL, filenameBuffer, -1, returnTripWideName, sizeof(returnTripWideName)/sizeof(returnTripWideName[0]));
            if (wcscmp(wideNameBuffer, returnTripWideName) == 0)
                usedDefaultChar = false;
            else
                usedDefaultChar = true;
        }
        if (returnValDW != 0)
        {
            if (usedDefaultChar)
            {
                // No.  Since all of our APIs are MBCS, we need an MBCS filename we can use in the ANSI code page.  Simply translating
                // using the OEM code page won't work because the file APIs are ANSI, and SetFileApisToOEM does not appear to work here.
                // The solution is to use the Unicode version of GetShortPathName, which creates a path using only ANSI characters, and
                // then translate this back to MBCS.  This is not a documented behavior - it is an observed one.  In some cases
                // GetShortPathNameW fails to do this (e.g., alt-205 and alt-210), and subsequent file API calls on this filename will fail.
                if (getShortPathnameWptr != NULL)
                {
                    getShortPathnameWptr(wideNameBuffer, shortNameBuffer, sizeof(shortNameBuffer)/sizeof(shortNameBuffer[0]));
                    returnValDW = WideCharToMultiByte(CP_ACP, NULL, shortNameBuffer, -1, filenameBuffer, filenameBufferSize, NULL, &usedDefaultChar);
                    if (returnValDW != 0)
                        returnValHR = S_FALSE;
                    else
                        returnValHR = HRESULT_FROM_WIN32(GetLastError());
                }
                else
                {
                    returnValHR = E_FAIL;
                }
            }
            else
            {
                returnValHR = S_OK;
            }
        }
        else
        {
            returnValHR = HRESULT_FROM_WIN32(GetLastError());
        }
    }
    else
    {
        // No (normal Win9x case).  Use the filename we're given.  No way to tell if there are any character
        // translation issues in this case (known MS issue they opted not to address) - we can only assume success (default char is _).
        DragQueryFile(dropHandle, fileNumber, filenameBuffer, filenameBufferSize);
        returnValHR = S_OK;
    }
    
    return returnValHR;
}

STDMETHODIMP CPropSheetEx::XScanImpl::ShowConfigHistoryDlg(HWND hParent,long flags,LPUNKNOWN pUnk,LPUNKNOWN pUnk2)
{
	METHOD_PROLOGUE(CPropSheetEx, ScanImpl)

	HRESULT hr = E_FAIL;

	CConfigHistory dlg(CWnd::FromHandle(hParent), (flags & HO_SHOWAPPLYALL) ? true : false, (flags & HO_READONLY) ? true : false);

	//If we can get a GenericConfig, use it
	if (SUCCEEDED(hr = pUnk->QueryInterface(IID_IGenericConfig,(void**)&dlg.m_pGenConfig)))
		//NO ADDREFF here because I am simply casting a member variable to point to the other
		//			member variable. Both will go away at the same time.
		dlg.m_pConfig = dlg.m_pGenConfig;
	else
		//Otherwise, try to get the IConfig
		hr = pUnk->QueryInterface(IID_IConfig,(void**)&dlg.m_pConfig);

	// get the second I Config if there is onle
	if (pUnk2)
	{
		hr = pUnk2->QueryInterface(IID_IGenericConfig,(void**)&dlg.m_pConfig2);
		if (FAILED(hr)) dlg.m_pConfig2=NULL;
	}

	if( SUCCEEDED(hr) )
	{
		if (dlg.DoModal()==IDOK)
			hr = S_OK;
		else 
			hr = S_FALSE;
		dlg.m_pConfig->Release();
		dlg.m_pGenConfig = NULL;
		if (dlg.m_pConfig2) dlg.m_pConfig2->Release();
	}

	return hr;
}




// {4DEF8DD1-C4D1-11d1-82DA-00A0C9749EEF}
IMPLEMENT_OLECREATE(CPropSheetEx, "LANDesk.VirusProtect.ScanDlgs", 
	0x4def8dd1, 0xc4d1, 0x11d1, 0x82, 0xda, 0x0, 0xa0, 0xc9, 0x74, 0x9e, 0xef);


// Note: we add support for IID_IVariantMap to support typesafe binding
// from VBA.  This IID must match the GUID that is attached to the 
// dispinterface in the .ODL file.

BEGIN_INTERFACE_MAP(CPropSheetEx, CCmdTarget)
	INTERFACE_PART(CPropSheetEx,IID_IScanDialogs, ScanImpl)
END_INTERFACE_MAP()










/////////////////////////////////////////////////////////////////////////////////////
// This function is likly to be called on a separat thead so OleInitalize and
// AfxEnableControl Container are called to ensure the create of OXCs on this thread
BOOL WINAPI CPropSheetEx::PageDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
		//
		// Create A new LDVUI OCK as a child of the page
		// we must cast it to a BOOL before returning.
		//
		case WM_INITDIALOG:
		{
			//	DWORD dwThread = NTxGetCurrentThreadId();
			//	char t[255];
			//	wsprintf(t,"thread id=%d",dwThread);
			//	MessageBox(NULL,t,"",MB_OK);
			CoInitialize(NULL);

			RECT r;
			HRESULT hr;
			IScanConfig *pConfig;

			AfxEnableControlContainer();

			CWnd *pWnd = CWnd::FromHandle(hwndDlg);
			pWnd->GetClientRect(&r);
			r.top +=10;	
			r.left +=10;
			CFileSystemUI *pUI = NULL;
			try
			{
				pUI = new CFileSystemUI;	
			}
			catch(std::bad_alloc &){return FALSE;}
			if (!pUI->Create(_T(""), WS_CHILD | WS_VISIBLE,
					r, pWnd, 1234,NULL,FALSE))
			{
				delete pUI;	// we faild to create the ocx so clean up
			}
			else
			{
 				int PageType = ((CPropSheetEx*)((PROPSHEETPAGE*)lParam)->lParam)->m_nPageType;
			
				pUI->SetType(PageType);
				hr = CoCreateLDVPObject(CLSID_CliProxy,IID_IScanConfig,(void**)&pConfig);

				if (hr==S_OK)
				{
					if (PageType==CONFIG_TYPE_IDLE)
						hr = pConfig->Open(NULL,HKEY_VP_ADMIN_SCANS,szReg_Key_Idle);
					else
					{
						hr = pConfig->Open(NULL,HKEY_VP_USER_SCANS,szReg_Key_Scan_Defaults);
						//if (hr==S_OK)
						//	hr = pConfig->SetSubOption();
					}

					pUI->SetConfigInterface(pConfig);
					pUI->Load();
				}

				pConfig->Release();
				g_DlgMap[hwndDlg]=pUI;
			}
			return TRUE;
		}
		case WM_DESTROY:
		{
			VOID* pUI;
			if (g_DlgMap.Lookup(hwndDlg,pUI))
			{
				((CFileSystemUI*)pUI)->DestroyWindow();
				delete (CFileSystemUI*)pUI;
			}
			CoUninitialize();
			break;
		}
		case WM_NOTIFY:
			if (((LPNMHDR) lParam)->code==PSN_APPLY)
			{
				VOID* pUI;
				if (g_DlgMap.Lookup(hwndDlg,pUI))
				{
					((CFileSystemUI*)pUI)->Store();
				}
			}
			break;
    }
	return FALSE;
}

UINT WINAPI CPropSheetEx::SheetProc(HWND hwnd, UINT msg, LPPROPSHEETPAGE pPage)
{
	
	if (msg==PSPCB_RELEASE)
	{
		//MessageBox(NULL,"Page release","",MB_OK);
		((CPropSheetEx*)pPage->lParam)->ExternalRelease();
	}
	return 1;
}
