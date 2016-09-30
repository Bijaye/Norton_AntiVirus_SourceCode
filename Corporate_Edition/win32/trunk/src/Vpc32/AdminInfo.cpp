// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// AdminInfo.cpp: implementation of the CAdminInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wprotect32.h"
#include "AdminInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// Function name:   CAdminInfo
//
// Description:     Constructor. Loads the NAVNTUTL DLL if we're running on NT
//
// Return type:     None
//
///////////////////////////////////////////////////////////////////////////////
// 2/9/2000 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CAdminInfo::CAdminInfo()
{
    OSVERSIONINFO os;


    // Initialize everything
    m_dwHiddenDrives = 0;
    m_hNAVNTUTL = NULL;
    m_pfnProcessIsNormalUser = NULL;
    m_pfnPolicyCheckHideDrives = NULL;
    m_pfnPolicyCheckRunOnly = NULL;
    m_pfnProcessCanUpdateRegKey = NULL;

    // See which platform we're running on.
    os.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &os );
    
    // Save the platform ID
    m_dwPlatformId = os.dwPlatformId;

    if ( os.dwPlatformId == VER_PLATFORM_WIN32_NT )
    {
        // Load NAVNTUTL.DLL if we're running on NT
        m_hNAVNTUTL = LoadLibrary(NAVNTUTL_DLL_NAME);

        if ( m_hNAVNTUTL )
        {
            m_pfnProcessIsNormalUser = 
                    (PFNProcessIsNormalUser) GetProcAddress( m_hNAVNTUTL, PROCESSISNORMALUSER);
            m_pfnPolicyCheckHideDrives = 
                    (PFNPolicyCheckHideDrives) GetProcAddress( m_hNAVNTUTL, POLICYCHECKHIDEDRIVES);
            m_pfnPolicyCheckRunOnly = 
                    (PFNPolicyCheckRunOnly) GetProcAddress( m_hNAVNTUTL, POLICYCHECKRUNONLY);
            m_pfnProcessCanUpdateRegKey = 
                    (PFNProcessCanUpdateRegKey) GetProcAddress( m_hNAVNTUTL, PROCESSCANUPDATEREGKEY);
        }
    }    
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name:   ~CAdminInfo
//
// Description:     Destructor
//
// Return type:     None
//
///////////////////////////////////////////////////////////////////////////////
// 2/9/2000 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CAdminInfo::~CAdminInfo()
{
    // Free the DLL if we're running on NT
    if ( m_dwPlatformId == VER_PLATFORM_WIN32_NT )
    {
        if ( m_hNAVNTUTL )
        {
            FreeLibrary( m_hNAVNTUTL );

            m_hNAVNTUTL = NULL;
            m_pfnProcessIsNormalUser = NULL;
            m_pfnPolicyCheckHideDrives = NULL;
            m_pfnPolicyCheckRunOnly = NULL;
            m_pfnProcessCanUpdateRegKey = NULL;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name:   GetHiddenDriveList
//
// Description:     If we're running on Windows 2000, return the hidden drive
//                  list based on the policy "Hide these specified drives in
//                  My Computer".
//
// Return type:     DWORD - Standard error code
//                  LPDWORD lpdwHiddenDrives - bit mask indicating hidden drives
//
///////////////////////////////////////////////////////////////////////////////
// 2/9/2000 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD CAdminInfo::GetHiddenDriveList(LPDWORD lpdwHiddenDrives)
{
    // If the function pointer is valid, just call it.
    if ( m_pfnPolicyCheckHideDrives )
        return m_pfnPolicyCheckHideDrives(lpdwHiddenDrives);
    else
    {
        // There are no hidden drives
        *lpdwHiddenDrives = 0;
        return ERROR_SUCCESS;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name:   IsProcessNormalUser
//
// Description:     We have users and Administrators. See if we're a user.
//
// Return type:     DWORD - Standard error codes.
//
///////////////////////////////////////////////////////////////////////////////
// 2/9/2000 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD CAdminInfo::IsProcessNormalUser(LPBOOL lpbNormalUser)
{
    // If the function pointer is valid, just call it.
    if ( m_pfnProcessIsNormalUser )
        return m_pfnProcessIsNormalUser((ULONG *)lpbNormalUser);
    else
    {
        // We're not running on NT so we assume the user
        // has more rights.
        *lpbNormalUser = 0;
        return ERROR_SUCCESS;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name:   AppIsAllowedToRun
//
// Description:     See if the app is allowed to run. On Windows 2000 this
//                  check the policy "Run Only Allowed Windows Applications".
//
// Return type:     DWORD - Standard error codes.
//
///////////////////////////////////////////////////////////////////////////////
// 2/9/2000 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD CAdminInfo::AppIsAllowedToRun(LPTSTR lpszAppName, LPBOOL lpbAllowed)
{
    // If the function pointer is valid, just call it.
    if ( m_pfnPolicyCheckRunOnly )
        return m_pfnPolicyCheckRunOnly( lpszAppName, (ULONG *)lpbAllowed);
    else
    {
        // We're not running on NT so we assume the app is allowed
        *lpbAllowed = TRUE;
        return ERROR_SUCCESS;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name:   CanProcessUpdateRegKey
//
// Description:     See if the specified process is allowed to updated a
//                  registry key
//
// Parameters:      [in]  - HANDLE hProcessToken - handle of the process
//                          to check. If it's NULL, the current process is
//                          used.
//
//                  [in]  - HKEY hMainKey - Any valid predefined HKEY
//
//                  [in]  - LPCTSTR lpszRegKey - Any valid key string
//
// Return type:     BOOL  - TRUE is the process can update the key
//
///////////////////////////////////////////////////////////////////////////////
// 3/23/2000 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CAdminInfo::CanProcessUpdateRegKey( HANDLE hProcessToken, HANDLE hMainKey, LPCTSTR lpszRegKey )
{
    BOOL    bUpdate = FALSE;

    
    if ( m_pfnProcessCanUpdateRegKey )
    {
        // Call the function in NAVNTUTL.DLL 
        m_pfnProcessCanUpdateRegKey( hProcessToken, hMainKey, lpszRegKey, (ULONG *)&bUpdate );
    }
    else
    {
        // If the function pointer is NULL, return TRUE.
        bUpdate = TRUE;
    }

    return bUpdate;        
}
