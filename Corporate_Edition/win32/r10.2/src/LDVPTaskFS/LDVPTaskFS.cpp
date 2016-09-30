// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: LDVPTaskFS.cpp 
//  Purpose: LDVPTaskFS Implementation file
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "stdafx.h"
#include "LDVPTaskFS.h"
#include "io.h"
#include "ComCache.h"
#include "ls.h"

#include "ComCache.h"
#include "ldvpcom.c"
#include "Language.c"

//Define NEEDKERNAL before including this file so I don't get undefined externals
#define NEEDKERNAL
#include "commisc.cpp"

#include "vphtmlhelp.h"
#include "SymSaferRegistry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CLDVPTaskFSApp NEAR theApp;
HINSTANCE hInstLang = NULL; // For access to acta.str resources.

const GUID CDECL BASED_CODE _tlid =
		{ 0x64b4a5ab, 0x799, 0x11d1, { 0x81, 0x2a, 0x0, 0xa0, 0xc9, 0x5c, 0x7, 0x56 } };

const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

const IID BASED_CODE taskGuid =
	{ 0x64B4A5AE, 0x0799, 0x11d1, { 0x81, 0x2A, 0x0, 0xa0, 0xc9, 0x5c, 0x7, 0x56 } };

//Import my GUIDS from vpcommon
IMP_VPCOMMON_IIDS

DWORD GetClientType()
{
	HKEY	pKey;
	DWORD	dwTemp = CLIENT_TYPE_CONNECTED,
			dwSize;

	//Read thew registry to determine if we are a Client Lite installation
	//First, get the info from the main key
	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
										szReg_Key_Main,
										0,
										KEY_READ,
										&pKey) )
	{
		dwSize = sizeof( DWORD );
		SymSaferRegQueryValueEx(	pKey,
							szReg_Val_Client_Type,
							NULL,
							NULL,
							(BYTE*)&dwTemp,
							&dwSize );

		RegCloseKey(pKey);
	}
	
	return dwTemp;
}

//----------------------------------------------------------------
// InitInstance
//----------------------------------------------------------------
BOOL CLDVPTaskFSApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();
	
	m_hInstance = NULL;

	if (bInit)
	{
		//Enable the control container for the OCX that I use
		// in the tasks
		AfxEnableControlContainer();

		InitDLL( AfxGetInstanceHandle() );

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

	    strTemplate = strAppDir + "\\%sRTSK.DLL";
		GetLanguage( lang, strFile.GetBuffer( strTemplate.GetLength() + 1 ), strTemplate.GetBuffer( strTemplate.GetLength() ) );
		strTemplate.ReleaseBuffer();
		strFile.ReleaseBuffer();

		m_hInstance = hInstLang = LoadLibrary(strFile);

		if (m_hInstance) // if load failed just use english
		{
			AfxSetResourceHandle( m_hInstance );
		}
		if (!hInstLang) // if load failed just use english
		{
			hInstLang = AfxGetInstanceHandle();
		}
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


		//Load the AppName
		m_strAppname.LoadString( IDS_APP_NAME );
		free ((void*)m_pszAppName );
		m_pszAppName = m_strAppname;
	}

	return bInit;
}


//----------------------------------------------------------------
// ExitInstance
//----------------------------------------------------------------
int CLDVPTaskFSApp::ExitInstance()
{
	//Set the AppName pointer to NULL
	m_pszAppName = NULL;
	m_pszHelpFilePath = NULL;

	DeinitDLL( AfxGetInstanceHandle() );

// NOTE: Win95 will IPF when returning from this method
//			if we unload the resource DLL, even if we
//			re-load the old resource handle. That being the case,
//			I am simply going to let the unload of the app remove
//			the DLL from memory.
//	if( m_hInstance )
//	{
//		FreeLibrary( m_hInstance );
//	}
	
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
	comCache.Register( taskGuid, "LDVPTask" );

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
// function name: CLDVPTaskFSApp::WinHelpInternal
//
// description: WinHelpInternal override to trap WinHelpInternal calls and redirect 
//				them to HTMLHelp
// return type: none
//
///////////////////////////////////////////////////////////////////////
// 8/1/03 JGEIGER - Inserted this function manually (did not use class
//					wizard) to implement HTMLHelp for SAV. 
///////////////////////////////////////////////////////////////////////
void CLDVPTaskFSApp::WinHelpInternal(DWORD dwData, UINT nCmd)
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
