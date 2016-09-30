// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "stdafx.h"
#include <iostream> 
#include <fstream>
#include "clientreg.h"
#include "dwLdPntScan.h"
#include "resource.h"
#include "vpcommon.h"

#include "SymSaferRegistry.h"

// Global object count
extern LONG g_dwObjs;
extern HINSTANCE g_hInstance;



///////////////////////////////////////////////////////////////////////////////
//
// Function name: CLoadPointScanHandler::CLoadPointScanHandler
//
// Description: Constructor for class object
//
// Return type: 
//
//
///////////////////////////////////////////////////////////////////////////////
CLoadPointScanHandler::CLoadPointScanHandler()
{
    // 
    // Increment for global object count.
    // 
    InterlockedIncrement( &g_dwObjs );
    
    // 
    // Set defaults
    // 
    m_dwRef = 0;
    m_pUtil4 = NULL;
    m_pScan = NULL;
    m_pScanConfig = NULL;
    m_pRootCliProxy = NULL;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CLoadPointScanHandler::~CLoadPointScanHandler
//
// Description:   Destructor for class object
//
// Return type: 
//
//
///////////////////////////////////////////////////////////////////////////////
CLoadPointScanHandler::~CLoadPointScanHandler()
{
    // 
    // Decrement of global object count.
    // 
    InterlockedDecrement( &g_dwObjs );
}

///////////////////////////////////////////////////////////////////
// IUnknown implementation

////////////////////////////////////////////////////////////////////////////////
//
// Function name    : CLoadPointScanHandler::QueryInterface
// Description      : This function will return a requested COM interface
// Return type      : STDMETHODIMP
// Argument         : REFIID riid - REFIID of interface requested
// Argument         : void** ppv - pointer to requested interface
//
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CLoadPointScanHandler::QueryInterface( REFIID riid, void** ppv )
{
    *ppv = NULL;


    if( IsEqualIID( riid, IID_IUnknown )||
        IsEqualIID( riid, IID_DefWatchEventHandler) )
        *ppv = this;

    if( *ppv )
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return ResultFromScode( S_OK );
        }

    return ResultFromScode( E_NOINTERFACE );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name    : CLoadPointScanHandler::AddRef()
// Description      : Increments reference count for this object
// Return type      : ULONG
//
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CLoadPointScanHandler::AddRef()
{
    return ++m_dwRef;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name    : CLoadPointScanHandler::Release()
// Description      : Decrements reference count for this object
// Return type      : ULONG
//
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CLoadPointScanHandler::Release()
{
    if( --m_dwRef )
        return m_dwRef;

    delete this;
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CLoadPointScanHandler::OnNewDefsInstalled
//
// Description: This method is called when new virus definitions have arrived.
//
// Return type: HRESULT 
//
//
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CLoadPointScanHandler::OnNewDefsInstalled()
{
    // Is the Defwatch Quickscan enabled?
    if ( true == GetDefwatchQuickscanEnabled() )
    {
        BOOL bReturn = InitObjects();
        if (!bReturn) 
        {
            Cleanup();
            return (S_OK);
        }

        // Prompt Rtvscan to use the latest defs.
        // This also catches if the service isn't running.
        // Ensure we're on the latest defs before starting scan engine.

        DWORD latest = m_pUtil4->UseLatestDefs();
        if( latest != 0 && latest != 1 ) 
        {
            Cleanup();
            return (S_OK);
        }

        // Get the Defwatch scan options.
        DWORD dwError = m_pScanConfig->Open(NULL, HKEY_VP_ADMIN_SCANS, szReg_Key_Defwatch_Quick_Scan);
        if ( dwError != ERROR_SUCCESS ) 
        {
            Cleanup();
            return (S_OK);
        }

        // Open the scan object
        HRESULT hr = m_pScan->Open( NULL, m_pScanConfig );
        if ( FAILED(hr) ) 
        {
            Cleanup();
            return (S_OK);
        }

        hr = m_pScan->StartScan (TRUE /*Launch Async*/, FALSE);

        Cleanup();
    }

    return (S_OK);
}


// *************************************************************************
//
// Name:        CLoadPointScanHandler::InitObjects()
//
// Description: Initialize the LDVP COM Objects needed for scanning.
//
// Parameters:  None
//
// Returns:     TRUE is all the COM objects we need at created.
//
// *************************************************************************
BOOL CLoadPointScanHandler::InitObjects()
{
    BOOL            bReturn         = FALSE;
    HRESULT         hr;
    GUID            iid_root        = _IID_IVirusProtect,
                    iid_iscan2      = _IID_IScan2,
                    iid_iscanconfig = _IID_IScanConfig,
                    iid_iutil4      = _IID_IUtil4,
                    cliproxy        = _CLSID_CliProxy;


    // CliProxy will give us interfaces into the service to use latest defs.
    hr = CoCreateInstance(cliproxy,NULL,CLSCTX_INPROC_SERVER,iid_root,(LPVOID*)&m_pRootCliProxy);
    if ( FAILED(hr) )
        return (bReturn);

    // get the IUtil4 Interface
    hr = m_pRootCliProxy->CreateByIID(iid_iutil4,(void**)&m_pUtil4);
    if ( FAILED(hr) )
        return (bReturn);

    // get the IScanConfig Interface
    hr = m_pRootCliProxy->CreateByIID(iid_iscanconfig,(void**)&m_pScanConfig);
    if ( FAILED(hr) )
        return (bReturn);

    // get the IScan Interface
    hr = m_pRootCliProxy->CreateByIID(iid_iscan2,(void**)&m_pScan);
    if ( FAILED(hr) )
        return (bReturn);

    return (TRUE);    
}

// *************************************************************************
//
// Name:        CLoadPointScanHandler::Cleanup()    
//
// Description: Cleanup, release our COM objects.
//
// Parameters:  None
//
// Returns:     Always return S_OK
//
// *************************************************************************
STDMETHODIMP CLoadPointScanHandler::Cleanup()
{

    if(m_pUtil4)
        m_pUtil4->Release();

    if(m_pScan)
        m_pScan->Release();

    if(m_pScanConfig)
        m_pScanConfig->Release();

    if(m_pRootCliProxy)
        m_pRootCliProxy->Release();

    return S_OK;
}

// *************************************************************************
//
// Name:        CLoadPointScanHandler::GetDefwatchQuickscanEnabled()    
//
// Description: Check to see if the Defwatch Quickscan is enabled.
//              This value currently lives in "AdministratorOnly\General" settings.
//
// Parameters:  None
//
// Returns:     true  - Quickscan is enabled, or indeterminate.
//              false - Quickscan is disabled.
//
// *************************************************************************
bool CLoadPointScanHandler::GetDefwatchQuickscanEnabled( void )
{
    HKEY    hMainKey        = NULL;
    bool    bScanEnabled    = true; // Default to our scan being enabled.

    // Query for the "EnableDefwatchQuickscan" option.  Default to enabled.
    if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, 
                                       szReg_Key_Main "\\" szReg_Key_AdminOnly "\\" szReg_Key_General, 
                                       0,
                                       KEY_QUERY_VALUE,
                                       &hMainKey ) )
    {
        DWORD   dwEnabledValue = 1;
        // Value is there and true, or value is missing, defaulting to true.
        if ( ERROR_SUCCESS == SymSaferRegQueryDWORDValueA( hMainKey, 
                                                           szReg_Val_EnableDefwatchQuickscan, 
                                                           &dwEnabledValue ) )
        {
            if ( 0 == dwEnabledValue )
                bScanEnabled = false;
        }
    }

    return bScanEnabled;
}
