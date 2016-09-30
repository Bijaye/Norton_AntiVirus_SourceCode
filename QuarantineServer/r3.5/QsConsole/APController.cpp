// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2005 Symantec Corporation. All rights reserved.
//***************************************************************************
// CAPController:
//
// Utility object to enable/disable AP for this process.
// Helper for global DisableAP()/EnableAP() functions.
//

#include "stdafx.h"
#include "apcontroller.h"
#include "qscommon.h"
#include "platform.h"
#include "apcomm.h"

CAPController::CAPController(void)
    : m_bInitialized( false )
    , m_hNavapLib( NULL )
    , m_pfnUnprotectProcess( NULL )
    , m_pfnProtectProcess( NULL )
{
    InitializeCriticalSection( &m_csInitialization );
}

CAPController::~CAPController(void)
{
    Deinitialize();

    DeleteCriticalSection( &m_csInitialization );
}

// CAPController::Initialize()
//
// Loads the protect/unprotect functions for AP.
//
// MT safe, okay to call repeatedly.
//
DWORD CAPController::Initialize()
{
    auto    DWORD   dwResult = ERROR_SUCCESS;

    if ( !m_bInitialized )
    {
        EnterCriticalSection( &m_csInitialization );

        if ( !m_bInitialized )
        {
            dwResult = ERROR_MOD_NOT_FOUND;

            DWORD dwSize = MAX_PATH * sizeof( TCHAR );
            CString sLibPath;

            // Open installed apps regkey.
            CRegKey reg;
            if( ERROR_SUCCESS == reg.Open( HKEY_LOCAL_MACHINE, REGKEY_INSTALLED_APPS, KEY_READ ) )
            {
                LONG lResult = reg.QueryStringValue( REGKEY_INSTALLED_NAV, sLibPath.GetBuffer( MAX_PATH ), &dwSize );
                sLibPath.ReleaseBuffer();

                if( ERROR_SUCCESS == lResult )
                {
                    // Build full path to lib.
                    sLibPath += _T("\\");
                    sLibPath += NAVAPI_NAME;

                    // Load module.
                    m_hNavapLib = LoadLibrary( sLibPath );

                    if ( NULL != m_hNavapLib )
                    {
                        // Get funtion pointers.
                        m_pfnUnprotectProcess   = (NAVAPFunctionPointer) GetProcAddress( m_hNavapLib, szNAVAPUnprotectProcess );
                        m_pfnProtectProcess     = (NAVAPFunctionPointer) GetProcAddress( m_hNavapLib, szNAVAPProtectProcess );

                        if ( m_pfnUnprotectProcess && m_pfnProtectProcess )
                        {
                            m_bInitialized = true;
                            dwResult = ERROR_SUCCESS;
                        }
                        else
                        {
                            m_pfnUnprotectProcess   = NULL;
                            m_pfnProtectProcess     = NULL;

                            FreeLibrary( m_hNavapLib );
                            m_hNavapLib = NULL;

                            dwResult = ERROR_PROC_NOT_FOUND;
                        }
                    }
                }
            }
        }

        LeaveCriticalSection( &m_csInitialization );
    }

    return ( dwResult );
}

DWORD CAPController::Deinitialize()
{
    if ( m_bInitialized )
    {
        EnterCriticalSection( &m_csInitialization );

        if ( m_bInitialized )
        {
            m_pfnUnprotectProcess   = NULL;
            m_pfnProtectProcess     = NULL;

            FreeLibrary( m_hNavapLib );
            m_hNavapLib = NULL;

            m_bInitialized = false;
        }

        LeaveCriticalSection( &m_csInitialization );
    }

    return ( ERROR_SUCCESS );
}

// CAPController::UnprotectProcess()
//
// Adds the current process to AP's process exclusion list.
// Should be followed by a matching call to CAPContorller::ProtectProcess().
//
DWORD CAPController::UnprotectProcess()
{
    DWORD   dwResult;

    dwResult = Initialize();

    if ( ERROR_SUCCESS == dwResult )
    {
        if ( FALSE == m_pfnUnprotectProcess() )
            dwResult = ERROR_GEN_FAILURE;
    }

    return ( dwResult );
}

// CAPController::ProtectProcess()
//
// Removes the current process from AP's process exclusion list.
// Should be called once for each call to CAPController::UnprotectProcess()
//
DWORD CAPController::ProtectProcess()
{
    DWORD   dwResult;

    dwResult = Initialize();

    if ( ERROR_SUCCESS == dwResult )
    {
        if ( FALSE == m_pfnProtectProcess() )
            dwResult = ERROR_GEN_FAILURE;
    }

    return ( dwResult );
}
