/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QUAR32/VCS/dllmain.cpv   1.5   11 Jun 1998 20:41:24   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// Dllmain.cpp - contains exported functions for ScanObj project
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUAR32/VCS/dllmain.cpv  $
// 
//    Rev 1.5   11 Jun 1998 20:41:24   DBuches
// Fixed problems with getting domain name.  We now run-time link with 
// the NT LanMan routines for getting the domain name.
// 
//    Rev 1.4   11 Jun 1998 13:45:56   DBuches
// Added code to dynamically load NAVNTUTL.DLL on WindowsNT systems.
// 
//    Rev 1.3   25 Mar 1998 21:48:38   JTaylor
// Added helper function MapVStatus for mapping AVAPI VSTATUS to HRESULTS.
// 
//    Rev 1.2   16 Mar 1998 18:44:54   SEDWARD
// Added CQuarantineOptsFactory.
//
//    Rev 1.1   25 Feb 1998 21:57:54   DBuches
// Added code to DLLGetClassObject() to support creating IQuarantineItem objects.
//
//    Rev 1.0   24 Feb 1998 17:33:08   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "factory.h"            // for ClassFactory

#define INITGUID
#include "iquaran.h"            // Interfaces
#include "qssnd.h"


/////////////////////////////////////////////////////////////////////////////
// Global Data

LONG   g_dwObjs = 0;           // Total number of objects in existance
                                // for this dll.

LONG   g_dwLocks = 0;          // Lock count.  Used by calls to Lock/UnLock
                                // Server() APIs

HINSTANCE g_hInstance = NULL;   // Global instance handle to this module.


                                // Runtime links to NT helper DLLs.
/*
HINSTANCE g_hInstNAVNTUTL = NULL;
HINSTANCE g_hInstNetApi32 = NULL;
PFNCOPYALTERNATEDATASTREAMS g_pfnCopyAlternateDataStreams = NULL;
PFNNetWkstaGetInfo   g_pfnNetWkstaGetInfo = NULL;
*/

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
        //
        // Cleanup
        //
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

    if( IsEqualCLSID (rclsid, CLSID_QsSnd) )
        {
        // Create factory
        CQuarantineItemFactory *pCF = NULL;

        // Create class factory and make sure it supports the
        // interface being requested.
        pCF = new CQuarantineItemFactory;

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
    if( !RegisterObject( CLSID_QsSnd ) )
        return E_UNEXPECTED;

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    if( !UnregisterObject( CLSID_QsSnd ) )
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
////////////////////////////////////////////////////////////////////////////
BOOL RegisterObject( REFCLSID rclsid )
{
    // registers objects supported by this module.
    OLECHAR wCLSIDStr[iGUIDSize];
    TCHAR szCLSID[iGUIDSize];
    TCHAR szRegKey[MAX_PATH];
    DWORD dwResult;
    HKEY hKey, hKeyInproc;

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
        wsprintf(szRegKey, _T("CLSID\\%s"), szCLSID);
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
                TCHAR szModPath[MAX_PATH];
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
                    // Clean up and return success.
                    RegCloseKey(hKeyInproc);
                    RegCloseKey(hKey);
                    return TRUE;
                    }
                RegCloseKey(hKeyInproc);
                }
            RegCloseKey(hKey);
            }
        }

    return FALSE;
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
////////////////////////////////////////////////////////////////////////////
BOOL UnregisterObject( REFCLSID rclsid )
{
    OLECHAR wCLSIDStr[iGUIDSize];
    TCHAR szCLSID[iGUIDSize];
    TCHAR szRegKey[MAX_PATH];
    HKEY hKey;

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
        wsprintf(szRegKey, _T("CLSID\\%s"), szCLSID);
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
            return TRUE;
            }
        }
    
    return FALSE;
}


