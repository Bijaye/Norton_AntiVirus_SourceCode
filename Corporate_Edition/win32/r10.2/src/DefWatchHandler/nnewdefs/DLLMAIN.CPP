// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QUAR32/VCS/DLLMAIN.CPv   1.3   25 Mar 1998 21:48:38   JTaylor  $
/////////////////////////////////////////////////////////////////////////////
//
// Dllmain.cpp - contains exported functions for ScanObj project
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUAR32/VCS/DLLMAIN.CPv  $
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#define INITGUID
#include "newdef.h"
#include "factory.h"            // for ClassFactory
#include "defwatch.h"
#include <new>
#include <time.h>
#include "SymSaferStrings.h"
#include "FineTimeImplementation.c"


/////////////////////////////////////////////////////////////////////////////
// Global Data

LONG   g_dwObjs = 0;           // Total number of objects in existance
                                // for this dll.

LONG   g_dwLocks = 0;          // Lock count.  Used by calls to Lock/UnLock
                                // Server() APIs

HINSTANCE g_hInstance = NULL;   // Global instance handle to this module.

/////////////////////////////////////////////////////////////////////////////
// Local constants

const int iGUIDSize = 64;
const TCHAR szInproc32[] = _T("InProcServer32");
const TCHAR szThreadingModel[] = _T("ThreadingModel");
const TCHAR szApartment[] = _T("Apartment");

/////////////////////////////////////////////////////////////////////////////
// Local prototypes
BOOL RegisterObject( REFCLSID rclsid );
BOOL UnregisterObject( REFCLSID rclsid );

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	    {
        g_hInstance = hInstance;
        }
	else if (dwReason == DLL_PROCESS_DETACH)
        {
        }
	return TRUE;    // ok
}


/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
	if( g_dwObjs || g_dwLocks )
        return ResultFromScode(S_FALSE);
    else
        return ResultFromScode(S_OK);
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	HRESULT hr;

    //
    // Make sure the class ID is a class we support.
    //
    if( IsEqualCLSID (rclsid, CLSID_DefWatchEventHandler) )
        {
        // Create factory
        CDefWatchEventHandlerFactory *pCF = NULL;

		try
		{
			// Create class factory and make sure it supports the
			// interface being requested.
			pCF = new CDefWatchEventHandlerFactory;
		}
		catch (std::bad_alloc &) {}
        if( pCF == NULL )
            return ResultFromScode( E_OUTOFMEMORY );

        hr = pCF->QueryInterface( riid, ppv );

        if( FAILED( hr ) )
            {
            delete pCF;
            pCF = NULL;
            }
        }
    else
        return ResultFromScode (CLASS_E_CLASSNOTAVAILABLE);


    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    if( !RegisterObject( CLSID_DefWatchEventHandler ) )
        return E_UNEXPECTED;

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    if( !UnregisterObject( CLSID_DefWatchEventHandler ) )
        return E_UNEXPECTED;

    return S_OK;
}



////////////////////////////////////////////////////////////////////////////
// Function name	: RegisterObject
//
// Description	    : Local helper routine to register an object
//
// Return type		: BOOL TRUE on success
//
// Argument         : REFCLSID rclsid - class to register
//
////////////////////////////////////////////////////////////////////////////
// 2/23/98 DBUCHES - Function created / Header added.
// 4/8/99  TCASHIN - Register as a DefWatch handler
////////////////////////////////////////////////////////////////////////////
BOOL RegisterObject( REFCLSID rclsid )
{
    // registers objects supported by this module.
    OLECHAR wCLSIDStr[iGUIDSize];
    TCHAR   szCLSID[iGUIDSize];
    TCHAR   szRegKey[MAX_PATH];
    TCHAR   szModPath[MAX_PATH];
    DWORD   dwResult;
    HKEY    hKey, hKeyInproc;
    BOOL    bReturn = FALSE;

    if( SUCCEEDED(StringFromGUID2( rclsid, (LPOLESTR)wCLSIDStr, iGUIDSize)) )
        {
        // Need to convert to MBCS here.
#ifdef UNICODE
        lstrcpy( szCLSID, wCLSIDStr );
#else
        if( FALSE == WideCharToMultiByte( CP_OEMCP,
                                          0,
                                          wCLSIDStr,
                                          -1,
                                          szCLSID,
                                          iGUIDSize,
                                          NULL,
                                          NULL) )
            {
            return FALSE;
            }
#endif
        sssnprintf(szRegKey, sizeof(szRegKey), _T("CLSID\\%s"), szCLSID);
        // Create reg key for this CLSID
        if( SUCCEEDED( RegCreateKeyEx( HKEY_CLASSES_ROOT,            
                                       szRegKey,
                                       0,
                                       NULL,
                                       0,
                                       KEY_WRITE,
                                       NULL,
                                       &hKey,
                                       &dwResult ) ) )
            {
            if( SUCCEEDED( RegCreateKeyEx( hKey,            
                                           szInproc32,
                                           0,
                                           NULL,
                                           0,
                                           KEY_WRITE,
                                           NULL,
                                           &hKeyInproc,
                                           &dwResult ) ) )
                {
                // Need to get path to this module.
                GetModuleFileName( g_hInstance, szModPath, MAX_PATH );
                GetShortPathName( szModPath, szModPath, MAX_PATH );
            
                // Set values for this key.
                if( SUCCEEDED( RegSetValueEx( hKeyInproc,
                                              _T(""),
                                              0,
                                              REG_SZ,
                                              (LPBYTE)szModPath,
                                              lstrlen( szModPath ) + sizeof(TCHAR))) 
                    &&
                    SUCCEEDED( RegSetValueEx( hKeyInproc,
                                              szThreadingModel,
                                              0,
                                              REG_SZ,
                                              (LPBYTE)szApartment,
                                              lstrlen( szApartment ) + sizeof(TCHAR))) 
                   )
                    {
                    // Return success.
                    bReturn = TRUE;
                    }
                RegCloseKey(hKeyInproc);
                }
            RegCloseKey(hKey);
            }
        }

    // Register as a DefWatch handler
    if ( bReturn )
    {       
        LPTSTR lpExeName = _tcsrchr( szModPath, '\\' );

        if ( lpExeName == NULL )
            lpExeName = szModPath;
        else
            lpExeName = CharNext(lpExeName);

        if( SUCCEEDED( RegCreateKeyEx( HKEY_LOCAL_MACHINE,            
                                       DEFWATCH_HANDLERS_REG_KEY,
                                       0,
                                       NULL,
                                       0,
                                       KEY_WRITE,
                                       NULL,
                                       &hKey,
                                       &dwResult ) ) )
        {            
            // Set values for this key.
            RegSetValueEx( hKey,
                           lpExeName,
                           0,
                           REG_SZ,
                           (LPBYTE)szCLSID,
                           lstrlen( szCLSID ) + sizeof(TCHAR)); 

            RegCloseKey( hKey );
        }
    }


    return bReturn;
}


////////////////////////////////////////////////////////////////////////////
// Function name	: UnregisterObject
//
// Description	    : Local helper function to unregister obejcts
//
// Return type		: BOOL TRUE on success
//
// Argument         : REFCLSID rclsid
//
////////////////////////////////////////////////////////////////////////////
// 2/23/98 DBUCHES - Function created / Header added.
// 4/8/99  TCASHIN - Unregister as a DefWatch handler
////////////////////////////////////////////////////////////////////////////
BOOL UnregisterObject( REFCLSID rclsid )
{
    OLECHAR wCLSIDStr[iGUIDSize];
    TCHAR   szCLSID[iGUIDSize];
    TCHAR   szRegKey[MAX_PATH];
    TCHAR   szModPath[MAX_PATH];
    HKEY    hKey;
    BOOL    bReturn = FALSE;

    if( SUCCEEDED(StringFromGUID2( rclsid, (LPOLESTR)wCLSIDStr, iGUIDSize)) )
        {
        // Need to convert to MBCS here.
#ifdef UNICODE        
        lstrcpy( szCLSID, wCLSIDStr );
#else
        if( FALSE == WideCharToMultiByte( CP_OEMCP, 
                                          0,
                                          wCLSIDStr,
                                          -1,
                                          szCLSID,
                                          iGUIDSize, 
                                          NULL,
                                          NULL) )
            {
            return FALSE;
            }
#endif
        sssnprintf(szRegKey, sizeof(szRegKey), _T("CLSID\\%s"), szCLSID);
        if( SUCCEEDED( RegOpenKeyEx( HKEY_CLASSES_ROOT,
                                     szRegKey,
                                     0,
                                     KEY_WRITE | KEY_ENUMERATE_SUB_KEYS,
                                     &hKey )))
            {
            // Delete all sub keys first.
            TCHAR szBuffer[MAX_PATH+1];            
            DWORD dwIndex = 0;
            while( RegEnumKey( hKey, dwIndex, szBuffer, MAX_PATH +1 ) != ERROR_NO_MORE_ITEMS)
                {
                RegDeleteKey( hKey, szBuffer );
                dwIndex++;
                }
            RegCloseKey(hKey);
            
            // Delete the CLSID key
            if( SUCCEEDED( RegOpenKeyEx( HKEY_CLASSES_ROOT,
                             _T("CLSID"),
                             0,
                             KEY_WRITE,
                             &hKey )))
                {
                RegDeleteKey( hKey, szCLSID );
                RegCloseKey(hKey);
                }
            bReturn = TRUE;
            }
        }

    // Unregister as a DefWatch handler
    if ( bReturn )
    {   
        LPTSTR lpExeName = NULL;

        if( SUCCEEDED( RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                                     DEFWATCH_HANDLERS_REG_KEY,
                                     0,
                                     KEY_WRITE | KEY_ENUMERATE_SUB_KEYS,
                                     &hKey )))
        {
            // Need to get path to this module.
            GetModuleFileName( g_hInstance, szModPath, MAX_PATH );
            GetShortPathName( szModPath, szModPath, MAX_PATH );

            lpExeName = _tcsrchr( szModPath, '\\' );

            if ( lpExeName == NULL )
                lpExeName = szModPath;
            else
                lpExeName = CharNext(lpExeName);

            RegDeleteValue( hKey, lpExeName);

            RegCloseKey( hKey );
        }
    }
    
    return bReturn;
}

void Real_dprintfTagged(DEBUGFLAGTYPE dwTag, const char *format,...)
{
	va_list marker;

	va_start(marker, format);
	Real_vdprintfCommon(dwTag, format, marker);
	va_end(marker);
}

void Real_dprintf(const char *format,...)
{
	// Calls to this function used to not have the call time prefixed in the logged line.
	va_list marker;

	va_start(marker, format);
	Real_vdprintfCommon(0, format, marker);
	va_end(marker);
}
