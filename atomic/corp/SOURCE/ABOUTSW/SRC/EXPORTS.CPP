// Copyright 1997 Symantec
// -------------------------------------------------------------------------
// EXPORTS.CPP
// Contains exported entry points for this sample Standard About extension page
// These are the standard COM exports necessary for
// self register (DllRegisterServer), unregister (DllUnregisterServer),
// and object creation (DllGetClassObject).
//
// There is also a helper C export to invoke the standard about COM interface (DoStdAbout).
//
// $Header:   S:/ABOUTSW/VCS/exports.cpv   1.5   29 Oct 1998 21:32:28   CEATON  $
// -------------------------------------------------------------------------
// $Log:   S:/ABOUTSW/VCS/exports.cpv  $
// 
//    Rev 1.5   29 Oct 1998 21:32:28   CEATON
// Fixed problem where parent registry key was not deleted during uninstall.
// 
// 
//    Rev 1.4   29 Oct 1998 19:53:52   CEATON
// Added call to GenerateSystemWorksKey() to get product-specific key.
// 
// 
//    Rev 1.3   22 Oct 1998 19:43:22   CEATON
// Removed code which appended the name "SystemWorks" to the defined key value.
// 
// 
//    Rev 1.2   21 Oct 1998 18:21:00   CEATON
// Changed registry key text to #define value in ABOUTSW.H.
// 
// 
//    Rev 1.1   13 Oct 1998 18:40:16   rchinta
// Removed unused code.  Cleaned up DllUnregisterServer().
// 
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "symlean.h"

// Initialize GUIDS
#pragma data_seg( ".text" )
#define INITGUID
#include <initguid.h>
#include <cguid.h>
#include "regint.h"
#include "n32behav.h"
#include "aboutint.h"
#include "slhelper.h"
#include "sloldmap.h"
#include "aboutsw.h"
#include "aboutdlg.h"				    // extension object
#include "sysworks.h"
#include "regguid.h"					// class ID's for on-line reg.
//#undef INITGUID
#pragma data_seg()

#include "resource.h"
#include "navver.h"


///////////////////////////////////////
// Global Variables

LONG g_cLock = 0;						// Used by LockContainer

HINSTANCE ghInst;

#if defined (WIN32)
    #define IS_WIN32 TRUE
#else
    #define IS_WIN32 FALSE
#endif

#define IS_NT      IS_WIN32 && (BOOL)(GetVersion() < 0x80000000)


/////////////////////////////////////////////////////////////////////////////
// Local functions
void FormatNoticeText( CString *lpstrNoticeText, BOOL bShowNavBuild );



/////////////////////////////////////////////////////////////////////////////
// Exports

///////////////////////////////////////
// DllMain

STDAPI_(BOOL) APIENTRY DllMain(
	HINSTANCE hDll,
	DWORD dwReason,
	LPVOID /* lpReserved */ )
{
#ifdef _DEBUG
	TCHAR szMod[_MAX_PATH];
	GetModuleFileName( hDll, szMod, _MAX_PATH );
#endif

	switch ( dwReason )
	{
		case DLL_PROCESS_ATTACH:
		{
			TRACE2( _T( "DLL_PROCESS_ATTACH: %s, (Process %08lX)\n" ),
				szMod, GetCurrentProcessId() );

			// save instance handle
            ghInst=hDll;
            SetInstanceHandle( hDll );
			break;
		}

		case DLL_PROCESS_DETACH:
			TRACE2( _T( "DLL_PROCESS_DETACH: %s, (Process %08lX)\n" ),
				szMod, GetCurrentProcessId() );
			break;

		case DLL_THREAD_ATTACH:
			TRACE2( _T( "DLL_THREAD_ATTACH: %s, (Thread %08lX)\n" ),
				szMod, GetCurrentThreadId() );
			break;

		case DLL_THREAD_DETACH:
			TRACE2( _T( "DLL_THREAD_DETACH: %s, (Thread %08lX)\n" ),
				szMod, GetCurrentThreadId() );
			break;

		default:
			break;
	}

	return TRUE;
}

///////////////////////////////////////
// DllGetClassObject

STDAPI DllGetClassObject( REFCLSID rclsid, REFIID riid,
	LPVOID FAR* ppvObj )
{
	IClassFactory* pFactory = NULL;

//	ABOUT BOX SYSTEM WORKS EXTENSION -----------------------------------------------------

	if ( IsEqualCLSID( rclsid, CLSID_CNavAboutSWExt ))
		pFactory = new CAboutExtFactory();

//	-------------------------------------------------------------------------

	// add tests for additional objects here...

	if ( pFactory )
	{
		if ( SYM_FAILED( pFactory->QueryInterface( riid, ppvObj )))
		{
			delete pFactory;
			*ppvObj = NULL;
			return E_INVALIDARG;
		}
	}
	else
		return CLASS_E_CLASSNOTAVAILABLE;

	return NOERROR;
}

///////////////////////////////////////
// DllCanUnloadNow

STDAPI DllCanUnloadNow()
{
	TRACE1( "DllCanUnloadNow: %d open refs\n", g_cRefThis );
	BOOL bCanUnload = ( g_cRefThis == 0 ) && ( g_cLock == 0 );
	return ( bCanUnload ? S_OK : S_FALSE );
}

///////////////////////////////////////
// DllRegisterServer

STDAPI DllRegisterServer()
{

    //	ABOUT BOX SYSTEMWORKS EXTENSION 
	OLECHAR szClassID[_MAX_PATH] = { L"" };
    CslString strExtName( IDS_COMPONENT_NAME );
    ASSERT( !strExtName.IsEmpty() );

    if ( StringFromGUID2( CLSID_CNavAboutSWExt, szClassID, _MAX_PATH ) > 0 )
	{
		CslString strClassID( szClassID );
                CslString strSystemWorksKey;
                HKEY hKey;

                GenerateSystemWorksKey( &hKey,strSystemWorksKey );

		CslRegistryKey regKey( hKey, strSystemWorksKey, KEY_SET_VALUE );
		VERIFY( regKey.SetValue( _T( "" ), (LPCTSTR)strClassID ) );
	    return ( RegisterComObject( strExtName, CLSID_CNavAboutSWExt ) );
	}

	return E_UNEXPECTED;
}

///////////////////////////////////////
// DllUnregisterServer

STDAPI DllUnregisterServer()
{
//	ABOUT BOX SYSTEMWORKS EXTENSION 
        CslString strExtName( IDS_COMPONENT_NAME );
        CslString strSystemWorksKey;
        HKEY hKey;
	
        GenerateSystemWorksKey( &hKey,strSystemWorksKey );

        CslString strCompany( IDS_COMPANY_NAME );     
        CslString strProduct( NAV_IS_OEM() ? IDS_PRODUCT_NAME_OEM : IDS_PRODUCT_NAME );

        CslString strFormat( IDS_FORMAT_PARENT_REGKEY );

        strSystemWorksKey = "";

        strSystemWorksKey.Format( strFormat, 
            (LPCTSTR) strCompany, 
            (LPCTSTR) strProduct );

	CslRegistryKey::DeleteKey( hKey, strSystemWorksKey );

	VERIFY( UnregisterComObject( CLSID_CNavAboutSWExt ) );
	
    return S_OK;
}


// END OF LINE ////////////////////////////////////////////////////////////////

