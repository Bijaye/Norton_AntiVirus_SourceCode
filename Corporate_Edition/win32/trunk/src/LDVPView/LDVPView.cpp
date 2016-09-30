// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: LDVPView.cpp
//  Purpose: LDVPView Implementation file
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "stdafx.h"
#include <atlbase.h>
#include "LDVPView.h"
#include "virusbinpage.h"
#include "io.h"
#include "ComCache.h"
#include "SymSaferRegistry.h"
#ifdef _USE_CCW
#include "symcharconvert.h"
#endif	//_USE_CCW
#include "ComCache.h"
#include "Language.c"

#include "ldvpcom.c"

#include "userdir.h"

//Define NEEDKERNAL before including this file so I don't get undefined externals
#define NEEDKERNAL
#include "commisc.cpp"

#include "vphtmlhelp.h"

#include "DarwinResCommon.h"

DARWIN_DECLARE_RESOURCE_LOADER(_T("LDVPViewRes.dll"), _T("LDVPView"))

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLDVPViewApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0x8e9145be, 0x703d, 0x11d1, { 0x81, 0xc9, 0x00, 0xa0, 0xc9, 0x5c, 0x07, 0x56 } } ;
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

const GUID CDECL BASED_CODE taskGuid =
	{ 0x8e9145bd, 0x703d, 0x11d1, { 0x81, 0xc9, 0x00, 0xa0, 0xc9, 0x5c, 0x07, 0x56 } };

//Import my GUIDS from VpCommon
IMP_VPCOMMON_IIDS


typedef BOOL (WINAPI *NAVAPFunctionPointer) ();


/*-----------------------------------------------------------------------------*/
//
//	Global functions
//		These are the global callback functions that I need to implement
//			in order for calls into ScanDlgs to work.
//
/*-----------------------------------------------------------------------------*/
extern "C" DWORD GetFileStateForView(void *Context,LPARAM Data,char *LogLine,DWORD *State, DWORD *ViewType)
{
	IVirusAction2	*ptrAction2 = NULL;
	DWORD			dwRet = 0;

	TRACE("GetFileStateForView\n" );

	//First, create an object to send this information to
	if( SUCCEEDED( CoCreateLDVPObject( CLSID_CliProxy, IID_IVirusAction2, (void**)&ptrAction2 ) ) )
	{
		//And forward this call
		dwRet = ptrAction2->GetStateOfFileForView( NULL, LogLine, State, ViewType );
		//Release the object
		ptrAction2->Release();
	}

	return dwRet;
}

extern "C" DWORD TakeAction(void *Context,LPARAM Data,char *LogLine,DWORD Action,DWORD State,bool bSvcStopOrProcTerminate)
{
	IVirusAction2	*ptrAction2 = NULL;
	DWORD			dwRet = 0;

	TRACE("TakeAction\n" );

	//First, create an object to send this information to
	if( SUCCEEDED( CoCreateLDVPObject( CLSID_CliProxy, IID_IVirusAction2, (void**)&ptrAction2 ) ) )
	{
		//And forward this call
		dwRet = ptrAction2->TakeAction( NULL, LogLine, Action, State, bSvcStopOrProcTerminate);
		//Release the object
		ptrAction2->Release();
	}

	return dwRet;
}
#ifdef _USE_CCW
extern "C" DWORD GetCopyOfFile(void *Context,LPARAM Data,char *LogLine,wchar_t *DestFile,DWORD State)
#else
extern "C" DWORD GetCopyOfFile(void *Context,LPARAM Data,char *LogLine,char *DestFile,DWORD State)
#endif
{
	IVirusAction2	*ptrAction2 = NULL;
	DWORD			dwRet = 0;

	TRACE("GetFileFromBin\n" );

	//First, create an object to send this information to
	if( SUCCEEDED( CoCreateLDVPObject( CLSID_CliProxy, IID_IVirusAction2, (void**)&ptrAction2 ) ) )
	{
		//And forward this call
#ifdef _USE_CCW
		dwRet = ptrAction2->GetCopyOfFileW( NULL, LogLine, DestFile, State );
#else	//_USE_CCW
		dwRet = ptrAction2->GetCopyOfFile( NULL, LogLine, DestFile, State );
#endif	//_USE_CCW
		//Release the object
		ptrAction2->Release();
	}

	return dwRet;
}

extern "C" DWORD ViewClosed(void *Context, BOOL bNotUsed)
{
	TRACE("ViewClosed\n" );

	return 0;
}


extern "C" DWORD GetVBinData(void *Context,LPARAM Data,char *LogLine,void *pvbindata)
{
	TRACE("GetVBinData\n" );

    ((CVirusBinPage*)Context)->OnScanDeliver(0, Data);

    return 0;
}

//----------------------------------------------------------------
// ReadLogFiles
//----------------------------------------------------------------
UINT ReadLogFiles( CResultsView *pResultsViewDlg )
{
	HKEY						hKey;
	CString						strLogDir,
								strFile,
								strLogLine,
								strFindMask;
	BYTE						*pData;
	DWORD						dwSize = 0;
	CStdioFile					file;
	WIN32_FIND_DATA				data;
	HANDLE						handle;
	CFileException				e;
    CUserDir                    UserDataDir;

	if( !AddLogLineToResultsView )
		return -1;

    // Do we need to user private user logs?
    if ( UserDataDir.UserDirsNeeded(READ_USER_LOGS) )
    {
        pData = (BYTE*)strLogDir.GetBuffer( MAX_PATH );

        UserDataDir.GetUserDirectory( (LPTSTR)pData, MAX_PATH );

        strLogDir.ReleaseBuffer();

		if( !strLogDir.IsEmpty() )
		{
		    strLogDir += "\\Logs";
		    strFindMask = strLogDir + "\\*.Log";
		}
    }
    else
    {
        pData = (BYTE*)strLogDir.GetBuffer( MAX_PATH );

        // Look for the data in the new location first.
        if ( UserDataDir.GetAppDataDirectory( (LPTSTR)pData, MAX_PATH ) == ERROR_SUCCESS )
        {
            strLogDir.ReleaseBuffer();

		    if( !strLogDir.IsEmpty() )
		    {
		        strLogDir += "\\Logs";
		        strFindMask = strLogDir + "\\*.Log";
		    }
        }
        else
        {
	        // Find the current log file directory using the old convention.
	        // This is {home dir}\Logs
	        if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
										        szReg_Key_Main,
										        0,
										        KEY_QUERY_VALUE,
										        &hKey ) )
	        {
                dwSize = 0;

		        SymSaferRegQueryValueEx(	hKey,
							        szReg_Val_HomeDir,
							        NULL,
							        NULL,
							        NULL,
							        &dwSize );

		        if( dwSize != 0 )
		        {
			        pData = (BYTE*)strLogDir.GetBuffer( dwSize );
			        SymSaferRegQueryValueEx(	hKey,
								        szReg_Val_HomeDir,
								        NULL,
								        NULL,
								        (BYTE*)pData,
								        &dwSize );
			        strLogDir.ReleaseBuffer();

			        if( !strLogDir.IsEmpty() )
			        {
				        strLogDir += "\\Logs";
				        strFindMask = strLogDir + "\\*.Log";
			        }
		        }
	        }
        }
    }

	if( strLogDir.IsEmpty() )
		return -1;

	//Fill in the LogLines
	// I do this by loping through each file in the directory and opening it
	//	then reading each line and sending it through to the Results view
	memset( &data, 0x00, sizeof( data ));
	if( INVALID_HANDLE_VALUE != ( handle = FindFirstFile(	(LPCTSTR)strFindMask, &data ) ) )
	{
		do
		{
			strFile.Format( "%s\\%s", strLogDir, data.cFileName );
			//Open up the file
//			if( file.Open( strFile, CFile::modeRead, &e ) )
            if( file.Open( strFile, CFile::modeRead | CFile::shareDenyNone, &e ) )
			{
				//and send all log lines to the dialog
				while( file.ReadString( strLogLine ) )
				{
					AddLogLineToResultsView( pResultsViewDlg, NULL, (LPSTR)(LPCTSTR)strLogLine );
				}

				file.Close();
			}

		//Finally, get the next file for the next loop through
		}while( FindNextFile( handle, &data ) );

		FindClose( handle );
	}

	//Tell the ResultsView that I am done
	AddLogLineToResultsView( pResultsViewDlg, NULL, NULL );

	return 0;
}

//----------------------------------------------------------------
// InitInstance
//----------------------------------------------------------------
BOOL CLDVPViewApp::InitInstance()
{
    CUserDir    UserDir;

	BOOL bInit = FALSE;

	if (SUCCEEDED(g_ResLoader.Initialize()))
	{
		AfxSetResourceHandle(g_ResLoader.GetResourceInstance());

		bInit = COleControlModule::InitInstance();
		//Enable the control container for the OCX that I use
		// in the tasks
		AfxEnableControlContainer();


		//Check the language and load the proper DLL
        TCHAR lang[4];
		CString strFile,
				strTemplate,
				strAppDir;
		char	*szFilename;

		//Get the program full path
		szFilename = strAppDir.GetBuffer( _MAX_PATH );
		GetModuleFileName( AfxGetInstanceHandle(), szFilename, _MAX_PATH );
		strAppDir.ReleaseBuffer();
		// and chop off the filename
		strAppDir = strAppDir.Left( strAppDir.ReverseFind( '\\' ) );

		strTemplate = strAppDir + "\\%sRvpvw.dll";
		GetLanguage( lang, strFile.GetBuffer( strTemplate.GetLength() + 1 ), strTemplate.GetBuffer( strTemplate.GetLength() ) );
		strTemplate.ReleaseBuffer();
		strFile.ReleaseBuffer();

		//---------------------------------------------------
		//Set up the helpfile
		//---------------------------------------------------
        m_strHelpFileName = SAV_HTML_HELP;

        if ( -1 == access( m_strHelpFileName.GetBuffer(MAX_PATH), 0) )
        {
            m_strHelpFileName.Empty();
        }

        m_strHelpFileName.ReleaseBuffer();

		free ((void*)m_pszHelpFilePath );
		m_pszHelpFilePath = m_strHelpFileName;

		//---------------------------------------------------
		//Load the AppName
		//---------------------------------------------------
		m_strAppname.LoadString( IDS_APP_NAME );
		free ((void*)m_pszAppName );
		m_pszAppName = m_strAppname;

		InitDLL( AfxGetInstanceHandle() );

		//---------------------------------------------------
		// Check on private user directories
		//---------------------------------------------------

        if ( UserDir.UserDirsNeeded(CREATE_USER_LOGS) )
        {
            if ( !UserDir.UserDirectoryExists("Logs") )
            {
                UserDir.CreateUserDirectory("Logs");
            }
        }
	}

	return bInit;
}


//----------------------------------------------------------------
// ExitInstance
//----------------------------------------------------------------
int CLDVPViewApp::ExitInstance()
{
	m_pszAppName = NULL;
    m_pszHelpFilePath = NULL;

	if(NULL != g_ResLoader.GetResourceInstance())
	DeinitDLL( AfxGetInstanceHandle() );

	return COleControlModule::ExitInstance();
}




//----------------------------------------------------------------
// DLLRegisterServer
//----------------------------------------------------------------
STDAPI DllRegisterServer(void)
{
	HKEY				pKey;
	CString				strSubKey;
	DWORD				dwDisp;
	CCOMCache			comCache;
	BYTE				*szGUID;
	GUID				guid = taskGuid;

	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	//First, register this control for Caching
	comCache.Register( taskGuid, "LDVPView" );

	//Now, register it with the LDVP Client
	strSubKey.Format( "%s\\%s",	szReg_Key_Main, szReg_Key_GUIDS );

	UuidToString( &guid, &szGUID );

	if( szGUID )
	{
		if( ERROR_SUCCESS == RegCreateKeyEx(	HKEY_LOCAL_MACHINE,
												strSubKey,
												0,
												"",
												REG_OPTION_NON_VOLATILE,
												KEY_READ | KEY_WRITE | KEY_CREATE_SUB_KEY,
												NULL,
												&pKey,
												&dwDisp
												) )
		{
			RegSetValueEx( pKey,
							(const char*)szGUID,
							NULL,
							REG_SZ,
							(BYTE*)"",
							1 );

			RegCloseKey( pKey );
		}

		RpcStringFree( &szGUID );
	}

	return NOERROR;
}


//----------------------------------------------------------------
// DllUnregisterServer
//----------------------------------------------------------------
STDAPI DllUnregisterServer(void)
{
	HKEY		pKey;
	CString		strSubKey;
	CCOMCache	comCache;
	BYTE		*szGUID;
	GUID		guid = taskGuid;

	AFX_MANAGE_STATE(_afxModuleAddrThis);
	strSubKey.Format( "%s\\%s",	szReg_Key_Main, szReg_Key_GUIDS );

	UuidToString( &guid, &szGUID );

	if( szGUID )
	{
		if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
											strSubKey,
											REG_OPTION_NON_VOLATILE,
											KEY_READ | KEY_WRITE | KEY_CREATE_SUB_KEY,
											&pKey ) )
		{
			RegDeleteValue( pKey, (const char *)szGUID );

			RegCloseKey( pKey );
		}

		RpcStringFree( &szGUID );
	}

	comCache.UnRegister( taskGuid );


	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}

////////////////////////////////////////////////////////////////////////
//
// function name: CLDVPViewApp::WinHelpInternal
//
// description: WinHelpInternal override to trap WinHelpInternal calls and redirect 
//				them to HTMLHelp
// return type: none
//
///////////////////////////////////////////////////////////////////////
// 8/1/03 JGEIGER - Inserted this function manually (did not use class
//					wizard) to implement HTMLHelp for SAV. 
///////////////////////////////////////////////////////////////////////
void CLDVPViewApp::WinHelpInternal(DWORD dwData, UINT nCmd)
{
    CString helpFilePath = SAV_HTML_HELP;

	::HtmlHelp( AfxGetMainWnd()->GetSafeHwnd(), 
              helpFilePath, 
              HH_HELP_CONTEXT, 
              dwData);
}


/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/
