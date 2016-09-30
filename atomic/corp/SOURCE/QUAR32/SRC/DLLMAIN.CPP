/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QUAR32/VCS/dllmain.cpv   1.8   11 Nov 1998 11:25:42   rchinta  $
/////////////////////////////////////////////////////////////////////////////
//
// Dllmain.cpp - contains exported functions for ScanObj project
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUAR32/VCS/dllmain.cpv  $
// 
//    Rev 1.8   11 Nov 1998 11:25:42   rchinta
// Modified RegisterObject and UnRegisterObject to check for
// the proper return values instead of SUCCEEDED macro.
// 
//    Rev 1.7   09 Nov 1998 15:38:36   mdunn
// Fixed Chris's fix - the error checking was written wrong so the code still hung.
// 
//    Rev 1.6   22 Sep 1998 20:47:34   cnevare
// Modified UnregisterObject() to break when no more
// registry keys are found to delete.
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

#include "Quar32.h"
#define INITGUID
#include "iquaran.h"            // Interfaces

#include "global.h"
#include "navntutl.h"

/////////////////////////////////////////////////////////////////////////////
// Global Data

LONG   g_dwObjs = 0;           // Total number of objects in existance
                                // for this dll.

LONG   g_dwLocks = 0;          // Lock count.  Used by calls to Lock/UnLock
                                // Server() APIs

HINSTANCE g_hInstance = NULL;   // Global instance handle to this module.


                                // Runtime links to NT helper DLLs.
HINSTANCE g_hInstNAVNTUTL = NULL;
HINSTANCE g_hInstNetApi32 = NULL;
PFNCOPYALTERNATEDATASTREAMS g_pfnCopyAlternateDataStreams = NULL;
PFNNetWkstaGetInfo   g_pfnNetWkstaGetInfo = NULL;

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

        
        // 
        // If running on WinNT machine, load our NT helper dll.
        //
        if( IsWinNT() )
            {
            //
            // Load our helper libraries.
            //
            g_hInstNAVNTUTL = LoadLibrary( _T("NAVNTUTL.DLL") );

            g_hInstNetApi32 = LoadLibrary( _T("NetApi32.DLL") );                
            
            // 
            // Get our function pointers
            //
            g_pfnCopyAlternateDataStreams = (PFNCOPYALTERNATEDATASTREAMS)
                                            GetProcAddress( g_hInstNAVNTUTL,  
                                                            _T( "CopyAlternateDataStreams" ));

            g_pfnNetWkstaGetInfo = (PFNNetWkstaGetInfo) GetProcAddress( g_hInstNetApi32,
                                                            _T( "NetWkstaGetInfo") );
            }


        }
	else if (dwReason == DLL_PROCESS_DETACH)
        {
        //
        // Cleanup
        //
        if( g_hInstNAVNTUTL )
            {
            FreeLibrary( g_hInstNAVNTUTL );
            g_hInstNAVNTUTL = NULL;
            FreeLibrary( g_hInstNetApi32 );
            g_hInstNetApi32 = NULL;
            }

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
    if( IsEqualCLSID (rclsid, CLSID_QuarantineDLL) )
        {
        // Create factory
        CQuarantineDLLFactory *pCF = NULL;

        // Create class factory and make sure it supports the
        // interface being requested.
        pCF = new CQuarantineDLLFactory;

        if( pCF == NULL )
            return ResultFromScode( E_OUTOFMEMORY );

        hr = pCF->QueryInterface( riid, ppv );

        if( FAILED( hr ) )
            {
            delete pCF;
            pCF = NULL;
            }
        }
    else if( IsEqualCLSID (rclsid, CLSID_QuarantineItem) )
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
    else if( IsEqualCLSID (rclsid, CLSID_QuarantineOpts) )
        {
        // Create factory
        CQuarantineOptsFactory *pCF = NULL;

        // Create class factory and make sure it supports the
        // interface being requested.
        pCF = new CQuarantineOptsFactory;

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
    if( !RegisterObject( CLSID_QuarantineDLL ) ||
        !RegisterObject( CLSID_QuarantineOpts ) ||
        !RegisterObject( CLSID_QuarantineItem ) )
        return E_UNEXPECTED;

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{

    if( !UnregisterObject( CLSID_QuarantineDLL ) ||
        !UnregisterObject( CLSID_QuarantineOpts ) ||
        !UnregisterObject( CLSID_QuarantineItem ))
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

    if( 0 != (StringFromGUID2( rclsid, (LPOLESTR)wCLSIDStr, iGUIDSize)) )
        {
        // Need to convert to MBCS here.
#ifdef UNICODE
        lstrcpy( szCLSID, wCLSIDStr );
#else
        if( 0 == WideCharToMultiByte( CP_OEMCP,
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
        if( ERROR_SUCCESS == ( RegCreateKeyEx( HKEY_CLASSES_ROOT,            
                                       szRegKey,
                                       0,
                                       NULL,
                                       0,
                                       KEY_WRITE,
                                       NULL,
                                       &hKey,
                                       &dwResult ) ) )
            {
            if( ERROR_SUCCESS == ( RegCreateKeyEx( hKey,            
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
                if( ERROR_SUCCESS == ( RegSetValueEx( hKeyInproc,
                                              _T(""),
                                              0,
                                              REG_SZ,
                                              (LPBYTE)szModPath,
                                              lstrlen( szModPath ) + sizeof(TCHAR))) 
                    &&
                    ERROR_SUCCESS == ( RegSetValueEx( hKeyInproc,
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
    HKEY hKey=NULL;

    
        if( 0 != StringFromGUID2( rclsid, (LPOLESTR)wCLSIDStr, iGUIDSize)) 
        {
        // Need to convert to MBCS here.
#ifdef UNICODE        
        lstrcpy( szCLSID, wCLSIDStr );
#else
        if( 0 == WideCharToMultiByte( CP_OEMCP, 
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
        if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CLASSES_ROOT,
                                     szRegKey,
                                     0,
                                     KEY_WRITE | KEY_ENUMERATE_SUB_KEYS,
                                     &hKey ))
            {
            // Delete all sub keys first.
            TCHAR szBuffer[MAX_PATH+1];            
            DWORD dwIndex = 0;

			// Make sure we have a valid handle first
			if( hKey )
				{
				// Enumerate the keys to delete
                while( RegEnumKey( hKey, dwIndex, szBuffer, MAX_PATH +1 ) == ERROR_SUCCESS)
					{
					// Delete each key until no more left
					if( RegDeleteKey( hKey, szBuffer ) != ERROR_SUCCESS )
						break;
					dwIndex++;
					}
				}
		
            RegCloseKey(hKey);

            // Delete the GUID key
            if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CLASSES_ROOT,
                             _T("CLSID"),
                             0,
                             KEY_WRITE,
                             &hKey ))
                {
                RegDeleteKey( hKey, szCLSID );
                RegCloseKey(hKey);
                }
            return TRUE;
            }
        }
    
    return FALSE;
}


////////////////////////////////////////////////////////////////////////////
// Function name	: MapVStatus
//
// Description	    : Local helper function to map from an AVAPI VSTATUS to 
//                    one of our interfaces HRESULTS.
//
// Return type		: HRESULT representing the error/success for the VSTATUS
//
// Argument         : VSTATUS vStat -- Result returned by AVAPI
//
////////////////////////////////////////////////////////////////////////////
// 3/25/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
HRESULT MapVStatus( VSTATUS vStat )
    {
    switch (vStat)
        {
        case VS_OK:
            return S_OK;
        case VS_MEMALLOC:
            return E_AVAPI_VS_MEMALLOC;
        case VS_FILE_ERROR:
            return E_AVAPI_VS_FILE_ERROR;
        case VS_CRIT_ERROR:
            return E_AVAPI_VS_CRIT_ERROR;
        case VS_CANT_REPAIR:
            return E_AVAPI_VS_CANT_REPAIR;
        case VS_DRIVE_ERROR:
            return E_AVAPI_VS_DRIVE_ERROR;
        case VS_INVALID_ARG:
            return E_AVAPI_VS_INVALID_ARG;
        case VS_ENTRY_UNAVAIL:
            return E_AVAPI_VS_ENTRY_UNAVAIL;
        case VS_NO_EXIST:
            return E_AVAPI_VS_NO_EXIST;
        case VS_WRONG_VERSION:
            return E_AVAPI_VS_WRONG_VERSION;
        case VS_CANT_DELETE:
            return E_AVAPI_VS_CANT_DELETE;
        case VS_NO_ACCESS:
            return E_AVAPI_VS_NO_ACCESS;
        case VS_SORTERROR:
            return E_AVAPI_VS_SORTERROR;
        case VS_INSCAN_CREATION:
            return E_AVAPI_VS_INSCAN_CREATION;
        }
    
    return S_OK;
    }


