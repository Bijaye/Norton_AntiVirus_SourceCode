// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// VPDN_LU.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "navlu.h"
#include "liveupdate.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"
#include "DarwinResCommon.h"
#include "OSMigrat.h" //EA for Apllication migration of Win2K
DARWIN_DECLARE_RESOURCE_LOADER(_T("Vpdn_LURes.dll"), _T("Vpdn_LU"))

#include "sharedver.h"

#include "vpcommon.h"
#include "resource.h"
#include "clientreg.h"
#include "bldnum.h"

// licensing related includes
#include "licensehelper.h"
#include "slicwrap.h"
#include "SLICLicenseData.h"
#include "SLICWrap_i.c"

// All this lets us use ATL
#define _ATL_FREE_THREADED
#include <atlbase.h>
CComModule _Module;
#include <atlcom.h>

// Local prototypes

BOOL ProcessCommandLine( LPCTSTR lpCmdLine, LPSTR *lplpszzFilters, DWORD *lpdwFlags );

void LogLiveUpdateResults( LPTSTR pszLine );

DWORD WINAPI GlobalLog( EVENTBLOCK* log_record );

BOOL WINAPI LicenseCheck( BOOL silent_mode, BOOL* abort );

// Globals

const   TCHAR   g_szSingleInstanceMutex[] = _T("VPDN_LU Is Running");

// *************************************************************************
//
// Name:        SetSAVRegVal
//
// Description: Set a DWORD reg val under the SAV root key.
//
// *************************************************************************
DWORD SetSAVRegVal( const TCHAR* key_name, const TCHAR* value_name, DWORD val )
{
	DWORD err = ERROR_SUCCESS;
	HKEY sav_root = NULL;
	HKEY key = NULL;

	if( (err = RegOpenKeyEx( HKEY_LOCAL_MACHINE, REGHEADER, 0, KEY_SET_VALUE, &sav_root )) == ERROR_SUCCESS )
	{
		if( (err = RegOpenKeyEx( sav_root, key_name, 0, KEY_SET_VALUE, &key )) == ERROR_SUCCESS )
		{
			err = RegSetValueEx( key, value_name, 0, REG_DWORD, reinterpret_cast<LPBYTE>(&val), sizeof(val) );

			RegCloseKey( key );
		}

		RegCloseKey( sav_root );
	}

	return( err );
}

// *************************************************************************
//
// Name:        GetSAVRegVal
//
// Description: Retrieve a DWORD reg val under the SAV root key.
//
// *************************************************************************
DWORD GetSAVRegVal( const TCHAR* key_name, const TCHAR* value_name, DWORD& val )
{
	DWORD err = ERROR_SUCCESS;
	HKEY sav_root = NULL;
	HKEY key = NULL;

	DWORD data_size = sizeof(val);

	if( (err = RegOpenKeyEx( HKEY_LOCAL_MACHINE, REGHEADER, 0, KEY_QUERY_VALUE, &sav_root )) == ERROR_SUCCESS )
	{
		if( (err = RegOpenKeyEx( sav_root, key_name, 0, KEY_QUERY_VALUE, &key )) == ERROR_SUCCESS )
		{
			err = SymSaferRegQueryValueEx( key, value_name, NULL, NULL, reinterpret_cast<LPBYTE>(&val), &data_size );

			RegCloseKey( key );
		}

		RegCloseKey( sav_root );
	}

	return( err );
}

//*************************************************************************
// _tWinMain()
//
// int APIENTRY _tWinMain (
//      HINSTANCE   hInstance,
//      HINSTANCE   hPrevInstance,
//      LPTSTR      lpCmdLine,
//      int         nCmdShow )
//
// Returns:
//
//*************************************************************************
// 6/15/99 DALLEE, created.
//*************************************************************************

int APIENTRY _tWinMain( HINSTANCE   hInstance,
                        HINSTANCE   hPrevInstance,
                        LPTSTR      lpCmdLine,
                        int         nCmdShow )
{
    auto    HANDLE              hSingleInstanceMutex;

    auto    LPSTR               lpszzFilters;
    auto    DWORD               dwFlags;
	TCHAR szAppName[1024];
    TCHAR szLine[IMAX_PATH] = {0};
	BOOL check_license = FALSE;

    // Initialize locals
    hSingleInstanceMutex = NULL;
    lpszzFilters         = NULL;
    dwFlags              = 0;

    DWORD dwLURet; 
	UINT uidMsg;
	
	if(SUCCEEDED(g_ResLoader.Initialize()))
	{
		hInstance = g_ResLoader.GetResourceInstance();
	}
	else
	{
		return FALSE;
	}

    hSingleInstanceMutex = CreateMutex( NULL, FALSE, g_szSingleInstanceMutex );

    if ( NULL == hSingleInstanceMutex )
    {
        goto BailOut;
    }

    if ( ERROR_ALREADY_EXISTS == GetLastError() )
    {
		//
		// 1-469LK1 --- Find LiveUpdate Window and set focus to it
		//
		HWND hWnd = FindWindow(NULL, "LiveUpdate");
		if (hWnd)
		{
			SetForegroundWindow(hWnd);
			SetFocus(hWnd);
		}
		
        goto BailOut;
    }

    // Check command line.

    ProcessCommandLine( GetCommandLine(), &lpszzFilters, &dwFlags );

    CoInitialize( NULL );

	// Are we checking licenses?  This is just a courtesy to the user, if there's no license
	// we won't waste his time downloading defs and other content.  It's ultimately up to the
	// product to validate the license before consuming new content.

	if( License_IsLicenseEnforcingInstall() )
	{
		check_license = TRUE;
	}

    // LiveUpdate


	if( check_license )
	{
		dwLURet = DoLiveUpdate (lpszzFilters, dwFlags, LicenseCheck);
	}
	else
	{
		dwLURet = DoLiveUpdate (lpszzFilters, dwFlags, NULL);
	}

    // MMENDON 09-08-2000 STS Defect #345614
    //          When the user cancels, we will no longer
    //          write a failure message (or any message)
    //          to the event log.  Removed this case from 
    //          the switch statement below.
    if( LOWORD(dwLURet) == OAM_USERCANCEL )
    {
        goto BailOut;
    }
        // MMENDON 09-08-2000 End STS Defect #345614


	// There are cases in which background remediation will happen
	// and either Intall or a user-initiated LU session will attempt 
	// to run at the same time. This conditional is true when the second
	// (or more) attempt discovers this. 
    if( LOWORD(dwLURet) == OAM_ERROR &&
		HIWORD(dwLURet) == NAVLURET_ALREADY_RUNNING)
    {
		bool fSilent = dwFlags & NAVLU_SILENT;
		if (!fSilent)
		{
			TCHAR tszLURunning[1024];
			LoadString(hInstance, IDS_LIVEUPDATE_ALREADY_RUNNING, tszLURunning, sizeof(tszLURunning)/sizeof(tszLURunning[0]));                
			LoadString(hInstance,IDS_PROG_NAME,szAppName,1024);
 			MessageBox(NULL, tszLURunning, szAppName, MB_OK|MB_ICONERROR);
		}
		goto BailOut;
    }


    switch( LOWORD(dwLURet) )
    {
    case OAM_SUCCESS:
    case OAM_SUCCESSREBOOT:

		if( HIWORD(dwLURet) == NAVLURET_INVALID_LIC )
		{
			uidMsg = IDS_MANUAL_LIVEUPDATE_SUCCESS_INVALID_LICENSE;
		}
		else
		{
			uidMsg = IDS_MANUAL_LIVEUPDATE_SUCCESS;

			// If we are on a SAV Server, we must prompt Rtvscan to check data directories
			// for the latest defs.
			// On SAV Clients that use LU via vdpn_lu, the NAVLU.dll uses DefUtils to
			// apply downloaded definitions.  DefUtils invokes DefWatch, which sets this
			// key, prompting the SAV Client to discover that new defs are in use.
			// On SAV Servers that use LU via vdpn_lu, DefUtils is never invoked.

			DWORD sav_type = TYPE_IS_CLIENT;

			if( GetSAVRegVal( "", szReg_Val_SAVType, sav_type ) == ERROR_SUCCESS )
			{
				if( sav_type == TYPE_IS_SERVER )
				{
					// We are a SAV Server.
					// Set the ProductControl\NewPatternFile value to prompt Rtvscan to wake
					// up and look for new defs.

					SetSAVRegVal( szReg_Key_ProductControl, szReg_Val_NewPatternFile, 1 );
				}
			}
		}

        break;
        
    case OAM_NOTHINGNEW:

        // MMENDON 5-5-2000:    Fix for STS defect 331645
        //                      Handling logging case when virus defs are current

		if( HIWORD(dwLURet) == NAVLURET_INVALID_LIC )
		{
			uidMsg = IDS_MANUAL_LIVEUPDATE_CURRENT_INVALID_LICENSE;
		}
		else
		{
			uidMsg = IDS_MANUAL_LIVEUPDATE_DEFS_CURRENT;
		}

        break;

    case OAM_ERROR:
    case OAM_HOSTBUSY:
    case OAM_NOCARRIER:
    case OAM_CRITICALERROR:
    default:

		if( HIWORD(dwLURet) == NAVLURET_INVALID_LIC )
		{
			uidMsg = IDS_MANUAL_LIVEUPDATE_FAIL_INVALID_LICENSE;
		}
		else
		{
			uidMsg = IDS_MANUAL_LIVEUPDATE_FAIL;
		}

        break;
    }
    
    LoadString(hInstance, uidMsg, szLine, sizeof(szLine));                
    LogLiveUpdateResults(szLine);

BailOut:

    if ( lpszzFilters )
        GlobalFreePtr( lpszzFilters );

    if ( hSingleInstanceMutex )
        CloseHandle( hSingleInstanceMutex );

	CoUninitialize();

	return 0;
} // _tWinMain()


//*************************************************************************
// ProcessCommandLine()
//
// This routine processes the command line arguments and creates the
// required filter strings and flags for calling NAVLU's DoLiveUpdate().
//
// Command lines supported are:
//      /s  /S                              Silent mode
//      /f<ProductType> /F<ProductType>     Filter specified ProductType.
//                                              No quotes or spaces allowed.
//                                              ex: /FVirusDef
//												ex: /fUpdate
//
// BOOL ProcessCommandLine (
//      LPCTSTR lpCmdLine,
//      LPSTR   *lplpszzFilters,
//      DWORD   *lpdwFlags )
//
// Note: It is the caller's responsibility to call GlobalFreePtr() to free
// the memory allocated and assigned to lplpszzFilters.
//
// Returns:
//      TRUE on success, otherwise FALSE.
//*************************************************************************
// 6/15/99 DALLEE, created.
//*************************************************************************

BOOL ProcessCommandLine( LPCTSTR lpCmdLine, LPSTR *lplpszzFilters, DWORD *lpdwFlags )
{
    auto    LPTSTR  lpszParam;
    auto    TCHAR   chSwitch;

    auto    LPTSTR  lpszFilterIndex;
    auto    int     nFilterLength;

    // Default return values.

    *lpdwFlags = 0;

    *lplpszzFilters = (LPTSTR) GlobalAllocPtr( GHND, 1024 );

    if ( NULL == *lplpszzFilters )
        return ( FALSE );

    lpszFilterIndex = *lplpszzFilters;

    // Pick out parameters

    for ( lpszParam = (LPTSTR)(LPCTSTR)_tcschr( lpCmdLine, _T('/') );
          NULL != lpszParam;
          lpszParam = _tcschr( lpszParam, _T('/') ) )
    {
        lpszParam++;
        chSwitch = (TCHAR) CharLower( (LPTSTR) *lpszParam );

        switch( chSwitch )
        {
        case _T('s'):
            // Add subdirectories flag.

            *lpdwFlags |= NAVLU_SILENT;
            break;

        case _T('f'):
            // Find text of ProductType to filter and add to double null terminated list.

            lpszParam++;

            nFilterLength = _tcscspn( lpszParam, _T(" /") );

            if ( nFilterLength )
            {
                // Copy filter and advance past null terminator.

                _tcsncpy( lpszFilterIndex, lpszParam, nFilterLength );
                lpszFilterIndex += nFilterLength + 1;
            }
            break;
        }
    }

	// add the second null char
	*lpszFilterIndex = 0;

    return ( TRUE );
} // ProcessCommandLine()



// *************************************************************************
//
// Name:        LogLiveUpdateResults
//
// Description: Initialize the LDVP COM Objects and write a record to the log
//              indicate the success or failure of Manual LiveUpdate.
//
// Parameters:  LPTSTR pszLine  String indicating success or failure of 
//                              LiveUpdate
//
// Returns:     None.
//
// *************************************************************************
// 7/26/2000 - RCHINTA : Function created.
// *************************************************************************
void LogLiveUpdateResults(LPTSTR pszLine)
{
    EVENTBLOCK  eb = {0};
    TCHAR       szVersion[NAME_SIZE] = {0};
    int         iSize = 0;

	eb.Description = pszLine;
	eb.logger = LOGGER_VPDOWN;
	eb.hKey[0] = 0;
	eb.Category = GL_CAT_PATTERN;
	eb.Event = GL_EVENT_PATTERN_DOWNLOAD;

    iSize = sssnprintf(szVersion, sizeof(szVersion), _T("%u.%u.%u.%u"), MAINPRODUCTVERSION, SUBPRODUCTVERSION, INLINEPRODUCTVERSION, BUILDNUMBER );

    if ( GetLastError() == ERROR_SUCCESS &&
         iSize < sizeof(szVersion) )
    {
        eb.lpProductVersion = szVersion;
    }

	GlobalLog( &eb );
}

// *************************************************************************
//
// Name:        GlobalLog
//
// Description: Initialize the LDVP COM Objects and write a record to the log.
//
// Parameters:  EVENTBLOCK* log_record - Log record to send to the SAV log.
//
// Returns:     None.
//
// *************************************************************************
// 8/20/2002 - RSUTTON : Function created.
// *************************************************************************
DWORD WINAPI GlobalLog( EVENTBLOCK* log_record )
{
    GUID clsid_cliproxy      = _CLSID_CliProxy;
    GUID iid_ivirusprotect   = _IID_IVirusProtect;
    GUID iid_iutil2          = _IID_IUtil2;
    GUID iid_iservicecontrol = _IID_IServiceControl;

    IVirusProtect   *pRoot           = NULL;
    IVirusProtect   *pCliScanVP      = NULL;
    IServiceControl *pServiceControl = NULL;
    IUtil2          *pUtil2          = NULL;

    HRESULT         hr;
	HRESULT			hr_cominit;

    hr_cominit = CoInitialize( NULL );

    if( SUCCEEDED( hr_cominit ) )
    {
        hr = CoCreateInstance( clsid_cliproxy, NULL, CLSCTX_INPROC_SERVER, iid_ivirusprotect, reinterpret_cast<void **>( &pRoot ) );

        if ( SUCCEEDED( hr ) )
        {
            // Ensure service is running before using CliProxy. Otherwise, use CliScan.

            hr = pRoot->CreateByIID( iid_iservicecontrol, reinterpret_cast<void **>( &pServiceControl ) );

            if ( SUCCEEDED( hr ) )
            {
                if ( ERROR_SERVICE_NOT_RUNNING != pServiceControl->GetStatus() )
                {
                    hr = pRoot->CreateByIID( iid_iutil2, reinterpret_cast<void **>( &pUtil2 ) );
                }

                pServiceControl->Release();
            }

#if 0
            if ( NULL == pUtil2 )
            {
                // Couldn't use CliProxy, switch to CliScan.

                hr = CoCreateInstance( clsid_cliscan, NULL, CLSCTX_INPROC_SERVER, iid_ivirusprotect, reinterpret_cast<void **>( &pCliScanVP ) );

                if ( SUCCEEDED( hr ) )
                {
                    hr = pCliScanVP->CreateByIID( iid_iutil2, reinterpret_cast<void **>( &pUtil2 ) );

                    pCliScanVP->Release();
                }
            }
#endif
            if ( NULL != pUtil2 )
            {
                // Call LogExternalEvent to send our event to the SAV log.

                pUtil2->LogExternalEvent( 0, (PEXTERNALEVENTBLOCK)log_record );
                pUtil2->Release();
            }

            pRoot->Release();
        }

        CoUninitialize();
    }

	if( SUCCEEDED(hr_cominit) )
	{
		// Uninit COM out of the main block so that smart ptrs get a chance to do their job.

        CoUninitialize();
	}

    return ( 0 );
}

// *************************************************************************
//
// Name:        LicenseCheck
//
// Description: Initialize the SLIC wrapper COM Objects and check for a content license.
//              NOTE: This is just a courtesy check to determine if LiveUpdate should
//              download content.  It's ultimately up to the product to prevent
//              new content from being absorbed.
//
// Parameters:  
//
// Returns:     SLIC_STATUS value from slic.h.
//
// *************************************************************************
// 7/29/2003 - RSUTTON : Function created.
// *************************************************************************
BOOL WINAPI LicenseCheck( BOOL silent_mode, BOOL* abort )
{
	USES_CONVERSION;

 	// First, get an instance of the COM object that wraps SLIC.

	HRESULT hr_cominit = S_OK;
	SLIC_STATUS slic_status = SLICSTATUS_OK;
	bool download_content_ok = false;
    bool one_time_on = false;

    *abort = FALSE;

	hr_cominit = CoInitialize( NULL );

	if( SUCCEEDED( hr_cominit ) )
	{
		// If we can't get an instance, we can assume that this is an unlicensed version.

		CComPtr<ISLICLicenseRepositorySCS> slic;

		if( SUCCEEDED( slic.CoCreateInstance( CLSID_SLICLicenseRepository, NULL, CLSCTX_INPROC_SERVER ) ) )
		{
			HRESULT hr = S_OK;

			CComPtr<ISLICLicense> lic;

			// Check for a license.

			CComBSTR msg;

			hr = slic->CheckLicenseSCS( &slic_status, &msg, &lic.p );

			if( SUCCEEDED(hr) )
			{
				// If the call returned a license, check if it's in the active state.

				if( lic != NULL )
				{
					SSLICLicenseData lic_data;

					if( SUCCEEDED( lic->GetLicenseData( &lic_data ) ) )
					{
						if( SLIC_SUCCEEDED( slic_status ) &&
                            ( lic_data.lifecycle == SLIC_LIFECYCLE_NOWORRIES ||
                              lic_data.lifecycle == SLIC_LIFECYCLE_WARN ||
                              lic_data.lifecycle == SLIC_LIFECYCLE_GRACE ) )
						{
							download_content_ok = true;
						}
					}
				}
			}

            // If the one-time update hasn't been used, allow the
            // LiveUpdate session.  Once defs are downloaded and SAV consumes them,
            // the main service (Rtvscan) will clear this flag.

            if( download_content_ok == false )
            {
                one_time_on = License_IsOneTimeUpdateMode() ? true : false;

                if( one_time_on == true )
                {
                    download_content_ok = true;
                }
            }

			if( silent_mode == FALSE && License_AlertOnLU() )
			{
				// The license needs some alert work.

				if( slic->ShowAlertSCS( slic_status,
                                        lic,
                                        NULL,
                                        (F_ALERT_LIVEUPDATE | (one_time_on == true ? F_ALERT_ONETIME_ON : 0)) ) == S_FALSE )
                {
                    *abort = TRUE;
                }
			}
        }
    }

	if( SUCCEEDED(hr_cominit) )
	{
		// Uninit COM out of the main block so that smart ptrs get a chance to do their job.

        CoUninitialize();
	}

    return( download_content_ok == true ? TRUE : FALSE );
}

