///////////////////////////////////////////////////////////////////////
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005, Symantec Corporation, All rights reserved.
///////////////////////////////////////////////////////////////////////
// WinFW.cpp
///////////////////////////////////////////////////////////////////////
//	Created: JZ, 1/21/2005
///////////////////////////////////////////////////////////////////////
#include "pscan.h"
#if (defined(WIN64) || defined(WIN32)) 
#include "ConfigWF.h"
#include "wow64helpers.h"
#include <atlbase.h>
#include <shlobj.h>
#include <sys/stat.h>
///////////////////////////////////////////////////////////////////////
//	Check to see if windows firewall configuration required for SCS
//
//	Check to see if binary value szReg_WinFW_Setting under scs root
//	exists. If it does, then needed, otherwise, not
//
///////////////////////////////////////////////////////////////////////
bool IsWFConfigNeeded()
{
	CRegKey reg;
	if(reg.Open (HKEY_LOCAL_MACHINE,szReg_Key_Main,KEY_READ) == ERROR_SUCCESS)
	{
		DWORD dwValue = 0;
		DWORD dwSize = sizeof(DWORD);//ERROR_FILE_NOT_FOUND/ERROR_INVALID_DATA/ERROR_MORE_DATA
		HRESULT hr = reg.QueryBinaryValue (szReg_WinFW_Setting, &dwValue,&dwSize);
		if(SUCCEEDED(hr))
			return true;
		if(hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
			return false;
		if(hr == HRESULT_FROM_WIN32(ERROR_MORE_DATA))
			return true;
	}
	return false;
}
///////////////////////////////////////////////////////////////////////
//	GetVersionInfo - Get file version information
//	lpszFileName - path to the binary 
//	wMajor - return major version
//	wMinor - minor version
//	wBuild - build number
//	wQFE - quick fix engineering number
///////////////////////////////////////////////////////////////////////
bool 
GetVersionInfo( IN	LPCTSTR lpszFileName,
				OUT	WORD& wMajor,
				OUT	WORD& wMinor,
				OUT	WORD& wBuild,
				OUT WORD& wQFE )
{
	wMajor = wMinor = wBuild = wQFE = 0;
	DWORD dwDummy = 0;
	bool bRet = false;
	//
	//	figure out how much memory we need
	//
	DWORD dwSize  = ::GetFileVersionInfoSize(const_cast< LPTSTR >( lpszFileName ), &dwDummy );

	if ( dwSize > 0 )
	{		
		LPBYTE lpbyVIB = (LPBYTE)new BYTE [ dwSize ];

		//
		//	try to get the version information
		//
		if ( lpbyVIB != NULL && ::GetFileVersionInfo( const_cast< LPTSTR >( lpszFileName ), 0, dwSize, lpbyVIB ) )
		{
			UINT   uLen    = 0;
			LPVOID lpVSFFI = NULL;
			
			if ( ::VerQueryValue( lpbyVIB, _T( "\\" ), (LPVOID*)&lpVSFFI, &uLen ) )
			{
				VS_FIXEDFILEINFO vsffi;
				::CopyMemory( &vsffi, lpVSFFI, sizeof( VS_FIXEDFILEINFO ) );
				if(vsffi.dwSignature == VS_FFI_SIGNATURE )
				{
					wMajor = HIWORD(vsffi.dwFileVersionMS );
					wMinor = LOWORD(vsffi.dwFileVersionMS);
					wBuild = HIWORD(vsffi.dwFileVersionLS);
					wQFE = LOWORD(vsffi.dwFileVersionLS);
					bRet = true;
				}
			}
		}
		if(lpbyVIB)
			delete [] lpbyVIB;
	}

	return bRet;
}

///////////////////////////////////////////////////////////////////////
//	Get path to RTVScan.exe, PDS.exe and NSCTop.exe if they exist
//	if the returned path is empty, then the corresponding file does 
//	not exist
///////////////////////////////////////////////////////////////////////
HRESULT 
GetPathes(OUT tstring& strRTVScan,	//path to RTVScan.exe
		  OUT tstring& strNSCTop,	//path to NSCTop.exe
		  OUT tstring& strPDS		//path to PDS.exe
		  )
{
	CRegKey reg;
	//
	//	Read InstalledApps
	//
	if(reg.Open(HKEY_LOCAL_MACHINE,szReg_Key_Sym_InstalledApps,KEY_READ ) == ERROR_SUCCESS)
	{
		TCHAR szPath[MAX_PATH] = {0};
		DWORD dwSize = MAX_PATH;
		//
		//	read SAVCE value to get path to RTVScan.exe
		//
		if(reg.QueryStringValue(_T("SAVCE"),szPath,&dwSize) == ERROR_SUCCESS)
		{
			strRTVScan = szPath;
			strRTVScan += _T("RTVScan.exe");
			struct _stat myStat;
			if(_tstat(strRTVScan.c_str (),&myStat))
				strRTVScan = _T("");
		}
		else
		{
			strRTVScan = _T("");//no SAV installed
		}
		dwSize = MAX_PATH;
		//
		//	read SSCADMIN to get path of NSCTOP.exe 
		//
		if(reg.QueryStringValue(_T("SSCADMIN"),szPath,&dwSize) == ERROR_SUCCESS)
		{
			strNSCTop = szPath;
			strNSCTop += _T("NSCTop.exe");
			struct _stat myStat;
			if(_tstat(strNSCTop.c_str (),&myStat))
				strNSCTop = _T("");

		}
		else
		{
			strNSCTop = _T("");//not 
		}
		//
		//	PDS.exe always installed in system directory
		//
		dwSize = MAX_PATH;
		if(IsWow64())
		{
typedef UINT (WINAPI *_GetSystemWow64Directory)(LPTSTR,UINT );
			_GetSystemWow64Directory GetSystemWow64Dir = NULL;
			HMODULE hKernel32 = GetModuleHandle( _T("KERNEL32.DLL") );
			if( hKernel32 != NULL )
			{
#ifdef UNICODE
				GetSystemWow64Dir = (_GetSystemWow64Directory)GetProcAddress( hKernel32, _T("GetSystemWow64DirectoryW") );
#else
				GetSystemWow64Dir = (_GetSystemWow64Directory)GetProcAddress( hKernel32, _T("GetSystemWow64DirectoryA") );
#endif
				if( GetSystemWow64Dir != NULL )
				{
					if(GetSystemWow64Dir( szPath,MAX_PATH))
					{
						strPDS = szPath;
						strPDS += _T("\\CBA\\PDS.exe");
						struct _stat myStat;
						if(_tstat(strPDS.c_str (),&myStat))
						strPDS = _T("");
					}
				}
			}
		}
		else
		{

			if(SUCCEEDED(SHGetFolderPath(NULL,CSIDL_SYSTEM,NULL,SHGFP_TYPE_CURRENT,szPath)))
			{
				strPDS = szPath;
				strPDS += _T("\\CBA\\PDS.exe");
			}
			else
			{
				;//just ignor
			}
		}
	}
	else
		return HRESULT_FROM_WIN32(GetLastError());
	return S_OK;
}
///////////////////////////////////////////////////////////////////////
//	
//	see if we configured once before
//	
///////////////////////////////////////////////////////////////////////
bool IsConfiguredAtLeastOnce()
{
	CRegKey reg;
	tstring szWinFWPath = szReg_Key_Main;
	szWinFWPath += _T("\\");
	szWinFWPath += szReg_WinFW_Config;
	//
	//	if ConfWinFW key exists, then we configured it at least onece
	//
	if(reg.Open(HKEY_LOCAL_MACHINE,szWinFWPath.c_str (),KEY_READ) == ERROR_SUCCESS)
		return true;
	return false;
}
///////////////////////////////////////////////////////////////////////
//	See if SCS or the windows has been updated since last configuration
//	
//
///////////////////////////////////////////////////////////////////////
HRESULT IsUpdated(bool& Updated)
{
	Updated= true;
	//
	//	if not configured yet, then must be updated
	//
	HRESULT hr = S_OK;
	if(IsConfiguredAtLeastOnce() == false)
	{
		Updated = true;
		return S_OK;
	}

	//
	//	find the pathes
	//
	tstring strRTVScan, strNSCTop,strPDS;
	if(FAILED(hr = GetPathes(strRTVScan,strNSCTop,strPDS)))
		return hr;

	//
	//	See if windows version changed
	//

	CRegKey reg;
	tstring szWinFWPath = szReg_Key_Main;
	szWinFWPath += _T("\\");
	szWinFWPath += szReg_WinFW_Config;
	if(SUCCEEDED(hr = reg.Open(HKEY_LOCAL_MACHINE,szWinFWPath.c_str (),KEY_READ)))
	{


		OSVERSIONINFO vI;
		DWORD lSize =sizeof(vI);
		if(SUCCEEDED(hr = reg.QueryBinaryValue(szReg_WinFW_WinVer, &vI,&lSize)))
		{
			OSVERSIONINFO vI2;
			vI2.dwOSVersionInfoSize = sizeof(vI2);
			if(!GetVersionEx(&vI2))
				return HRESULT_FROM_WIN32(GetLastError());
			if(	vI2.dwBuildNumber != vI.dwBuildNumber ||
				vI2.dwMajorVersion != vI.dwMajorVersion ||
				vI2.dwMinorVersion != vI.dwMinorVersion ||
				vI2.dwPlatformId != vI.dwPlatformId)
			{
				return S_OK;//OS changed/updated
			}
		}
		else
			return hr;
	}
	else
		return hr;
	

	//
	//	see if RTVScan changed/updated?
	//
	WORD wMajor,wMinor,wBuild,wQFE;
	if(strRTVScan.length ())
	{
		DWORD dwVH,dwVL;
		if(FAILED(hr = reg.QueryDWORDValue (szReg_WinFW_RTVScanVM,dwVH)) ||
			FAILED(hr= reg.QueryDWORDValue(szReg_WinFW_RTVScanVL,dwVL)))
			return hr;
		if(! GetVersionInfo(strRTVScan.c_str (),wMajor,wMinor,wBuild,wQFE))
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}

		if((DWORD)MAKELONG(wMinor,wMajor) != dwVH || (DWORD)MAKELONG(wQFE,wBuild) != dwVL)
		{
			return S_OK;//updated
		}
	}
	//
	//	see if NSCTOP changed/updated?
	//
	if(strNSCTop.length ())
	{
		DWORD dwVH,dwVL;
		if(FAILED(hr = reg.QueryDWORDValue (szReg_WinFW_NSCTopVM,dwVH)) ||
			FAILED(hr= reg.QueryDWORDValue(szReg_WinFW_NSCTopVL,dwVL)))
			return hr;
		if(! GetVersionInfo(strNSCTop.c_str (),wMajor,wMinor,wBuild,wQFE))
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}

		if((DWORD)MAKELONG(wMinor,wMajor) != dwVH || (DWORD)MAKELONG(wQFE,wBuild) != dwVL)
		{
			return S_OK;//updated
		}
	}
	//
	//	see if PDS.exe changed
	//
	if(strPDS.length ())
	{
		DWORD dwVH,dwVL;
		if(FAILED(hr = reg.QueryDWORDValue (szReg_WinFW_PDSVM,dwVH)) ||
			FAILED(hr= reg.QueryDWORDValue(szReg_WinFW_PDSVL,dwVL)))
			return hr;
		if(! GetVersionInfo(strPDS.c_str (),wMajor,wMinor,wBuild,wQFE))
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}

		if((DWORD)MAKELONG(wMinor,wMajor) != dwVH || (DWORD)MAKELONG(wQFE,wBuild) != dwVL)
		{
			return S_OK;//updated
		}
	}
	//
	//	nothing updated
	//
	Updated = false;
	return S_OK;
}
///////////////////////////////////////////////////////////////////////
//	Update OS,SCS Version information in the registry
///////////////////////////////////////////////////////////////////////
HRESULT UpdateInfo()
{
	//
	//	find the path
	//
	tstring strRTVScan, strNSCTop,strPDS;
	HRESULT hr;
	if(FAILED(hr = GetPathes(strRTVScan,strNSCTop,strPDS)))
		return hr;

	//
	//write windows version info
	//

	CRegKey reg;
	tstring szWinFWPath = szReg_Key_Main;
	szWinFWPath += _T("\\");
	szWinFWPath += szReg_WinFW_Config;
	if(SUCCEEDED(hr = reg.Create(HKEY_LOCAL_MACHINE,szWinFWPath.c_str ())))
	{
		//
		//	get os info
		//
		OSVERSIONINFO vI2;
		vI2.dwOSVersionInfoSize = sizeof(vI2);
		if(!GetVersionEx(&vI2))
			return HRESULT_FROM_WIN32(GetLastError());
		//
		//	write it into the reg
		//
		LONG lSize =sizeof(vI2);
		if(FAILED(hr = reg.SetBinaryValue(szReg_WinFW_WinVer, &vI2,lSize)))
			return hr;
	}
	else
		return hr;
	

	//
	//write RTVScan info
	//
	WORD wMajor,wMinor,wBuild,wQFE;
	if(strRTVScan.length ())
	{
		
		if( GetVersionInfo(strRTVScan.c_str (),wMajor,wMinor,wBuild,wQFE))
		{


			if(	FAILED(hr = reg.SetDWORDValue (szReg_WinFW_RTVScanVM,MAKELONG(wMinor,wMajor))) ||
				FAILED(hr= reg.SetDWORDValue(szReg_WinFW_RTVScanVL,MAKELONG(wQFE,wBuild))))
				return hr;
		}
		else
			return HRESULT_FROM_WIN32(GetLastError());
	}
	if(strNSCTop.length ())
	{
				
		if( GetVersionInfo(strNSCTop.c_str (),wMajor,wMinor,wBuild,wQFE))
		{


			if(	FAILED(hr = reg.SetDWORDValue (szReg_WinFW_NSCTopVM,MAKELONG(wMinor,wMajor))) ||
				FAILED(hr= reg.SetDWORDValue(szReg_WinFW_NSCTopVL,MAKELONG(wQFE,wBuild))))
				return hr;
		}
		else
			return HRESULT_FROM_WIN32(GetLastError());
	}
	if(strPDS.length ())
	{
		
		if( GetVersionInfo(strPDS.c_str (),wMajor,wMinor,wBuild,wQFE))
		{


			if(	FAILED(hr = reg.SetDWORDValue (szReg_WinFW_PDSVM,MAKELONG(wMinor,wMajor))) ||
				FAILED(hr= reg.SetDWORDValue(szReg_WinFW_PDSVL,MAKELONG(wQFE,wBuild))))
				return hr;
		}
		else
			return HRESULT_FROM_WIN32(GetLastError());
	}
	return S_OK;
}

HRESULT ConfigWF4SCS()
{
	//
	//	if we dont need to configure it
	//
	if(!IsWFConfigNeeded())
		return S_OK;

	bool bChanged = false;
	HRESULT hr = S_OK;
	//
	//	if never configured or if the OS or SCS updated
	//
	if(	IsConfiguredAtLeastOnce() == false	||
		(SUCCEEDED((hr= IsUpdated(bChanged))) && bChanged)
		)
	{
		//
		//	configure the firewall
		//
		tstring strRTVScan, strNSCTop,strPDS;
		HRESULT hr;
		if(FAILED(hr = GetPathes(strRTVScan, strNSCTop,strPDS)))
			return hr;
		SYMAPP symApps [4];
		int i  = 0;
		if(strRTVScan.length ())
		{
			symApps[i].strPath = strRTVScan;
			symApps[i].strFriendlyName = LS(IDS_SYMAV_SERVICE);
			symApps[i].bTCP = true;
			symApps[i].lPort = 2967;
			symApps[i].bSubnetOnly = false;
			i++;
		}
		if(strNSCTop.length ())
		{
			symApps[i].strPath = strNSCTop;
			symApps[i].strFriendlyName = LS(IDS_NSCTOP_SERVICE);
			symApps[i].bTCP = true;
			symApps[i].lPort = 2967;
			symApps[i].bSubnetOnly = false;
			i++;
		}
		if(strPDS.length ())
		{
			symApps[i].strPath = strPDS;
			symApps[i].strFriendlyName = LS(IDS_PDS);
			symApps[i].bTCP = false;
			symApps[i].lPort = 38293;
			symApps[i].bSubnetOnly = false;
			i++;
		}
		SYMAPP_LIST apps;
		for(int j = 0; j <i; j++)
			apps.push_back(symApps[j]);

		TCHAR szPath[MAX_PATH];
		ZeroMemory(szPath, sizeof(szPath));
		::GetSystemDirectory( szPath, MAX_PATH );
		_tcscat( szPath, _T("\\"));
		_tcscat( szPath, s_szWinFW_Dll);

		HMODULE hWinDll = LoadLibrary(szPath);
		if(hWinDll)
		{
			_ConfigWinFw4Apps pfConfigWinFw4Apps = (_ConfigWinFw4Apps)GetProcAddress(hWinDll,_T("ConfigWinFw4Apps"));
			if(pfConfigWinFw4Apps)
			{
				if(SUCCEEDED(hr = pfConfigWinFw4Apps(s_szWinFW_Dll,  apps)))
				{

					//
					//	and save SCS and OS Info
					//
					hr = UpdateInfo();
				}
			}
			else
				hr = HRESULT_FROM_WIN32(GetLastError());

			FreeLibrary(hWinDll);
		}
		
	}
	return hr;
	
}
///////////////////////////////////////////////////////////////////////
//	Unconfigure Windows Firewall for SCS
///////////////////////////////////////////////////////////////////////
HRESULT UnConfigWF4SCS()
{
	//
	//	if not configured, just return
	//
	if(IsConfiguredAtLeastOnce() == false	)
		return true;
	//
	// remove the regkey and values (best effort
	//
	CRegKey reg;
	HRESULT hr ;
	if(SUCCEEDED(hr = reg.Open(HKEY_LOCAL_MACHINE,szReg_Key_Main,KEY_READ|KEY_WRITE)))
	{
		reg.RecurseDeleteKey(szReg_WinFW_Config);
	}
	//
	//	remove configurations
	//
	tstring strRTVScan, strNSCTop,strPDS;
	if(FAILED(hr = GetPathes(strRTVScan, strNSCTop,strPDS)))
		return hr;
	SYMAPP symApps [4];
	int i  = 0;
	if(strRTVScan.length ())
	{
		symApps[i].strPath = strRTVScan;
		symApps[i].strFriendlyName = LS(IDS_SYMAV_SERVICE);
		symApps[i].bTCP = true;
		symApps[i].lPort = 2967;
		symApps[i].bSubnetOnly = false;
		i++;
	}
	if(strNSCTop.length ())
	{
		symApps[i].strPath = strNSCTop;
		symApps[i].strFriendlyName = LS(IDS_NSCTOP_SERVICE);
		symApps[i].bTCP = true;
		symApps[i].lPort = 2967;
		symApps[i].bSubnetOnly = false;
		i++;
	}
	if(strPDS.length ())
	{
		symApps[i].strPath = strPDS;
		symApps[i].strFriendlyName = LS(IDS_PDS);
		symApps[i].bTCP = false;
		symApps[i].lPort = 38293;
		symApps[i].bSubnetOnly = false;
		i++;
	}
	SYMAPP_LIST apps;
	for(int j = 0; j <i; j++)
		apps.push_back(symApps[j]);
	HMODULE hWinDll = LoadLibrary(s_szWinFW_Dll);
	if(hWinDll)
	{
		_UnConfigWinFw4Apps pfUnConfigWinFw4Apps = (_UnConfigWinFw4Apps)GetProcAddress(hWinDll,_T("UnConfigWinFw4Apps"));
		if(pfUnConfigWinFw4Apps)
            hr =pfUnConfigWinFw4Apps(s_szWinFW_Dll,  apps);
		else
			hr= HRESULT_FROM_WIN32(GetLastError());
		FreeLibrary(hWinDll);
	}
	else
		hr = HRESULT_FROM_WIN32(GetLastError());

	return hr;
}
#endif //!# WIN32 