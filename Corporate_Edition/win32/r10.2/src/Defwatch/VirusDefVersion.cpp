// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// VirusDefVersion.cpp: implementation of the CVirusDefVersion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VirusDefVersion.h"
#include "defwatch.h"
#include "SymSaferRegistry.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CVirusDefVersion::CVirusDefVersion
//
// Description: Consructor
//
///////////////////////////////////////////////////////////////////////////////
// 11/7/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CVirusDefVersion::CVirusDefVersion()
{
    // 
    // Set up defaults.  
    // 
    ZeroMemory( &m_stDefDate, sizeof( SYSTEMTIME ) );
    m_dwDefRevision = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CVirusDefVersion::CVirusDefVersion
//
// Description: Constructor.
//
// Argument         : WORD wY
// Argument         : WORD wM
// Argument         : WORD wD
// Argument         : DWORD dwRev
//
///////////////////////////////////////////////////////////////////////////////
// 11/7/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CVirusDefVersion::CVirusDefVersion( WORD wY, WORD wM, WORD wD, DWORD dwRev ) 
{
    // 
    // Call base class
    // 
    ZeroMemory( &m_stDefDate, sizeof( SYSTEMTIME ) );

    // 
    // Save off date info.
    //
    m_stDefDate.wYear = wY;
    m_stDefDate.wMonth = wM;
    m_stDefDate.wDay = wD;
    m_dwDefRevision = dwRev;
    
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CVirusDefVersion::~CVirusDefVersion
//
// Description: Destructor
//
///////////////////////////////////////////////////////////////////////////////
// 11/7/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CVirusDefVersion::~CVirusDefVersion()
{

}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CVirusDefVersion::Load
//
// Description: Loads virus def version info from registry.
//
// Return type: BOOL TRUE on success, FALSE otherwise.
//
//
///////////////////////////////////////////////////////////////////////////////
// 11/7/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CVirusDefVersion::Load( const LPTSTR szRegKey )
{
    HKEY    hKey;
    DWORD   dwType = REG_BINARY;
    DWORD   dwSize = sizeof( SYSTEMTIME ) + sizeof( DWORD );
    BYTE    buffer[ sizeof( SYSTEMTIME ) + sizeof( DWORD ) ];    
    
    // 
    // Open specified key
    // 
    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, szRegKey, 0, KEY_READ, &hKey))
        {
        if( ERROR_SUCCESS == SymSaferRegQueryValueEx(hKey, DEFWATCH_DEFVER_VAL, 0, &dwType, buffer, &dwSize) )
            {
            // 
            // Transfer data to members
            // 
            CopyMemory( &m_stDefDate, buffer, sizeof( SYSTEMTIME ) );
            m_dwDefRevision = *((DWORD*) &buffer[ sizeof(SYSTEMTIME) ]);
            RegCloseKey( hKey );
            return TRUE;
            }

        // 
        // Cleanup
        //         
        RegCloseKey( hKey );
        }

    // 
    // Failure.
    // 
    return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CVirusDefVersion::Save
//
// Return type: BOOL TRUE on success
//
// Argument         : LPTSTR szRegKey
//
///////////////////////////////////////////////////////////////////////////////
// 11/7/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CVirusDefVersion::Save( const LPTSTR szRegKey )
{
    HKEY    hKey;
    DWORD   dwType = REG_BINARY;
    DWORD   dwSize = sizeof( SYSTEMTIME ) + sizeof( DWORD );
    BYTE    buffer[ sizeof( SYSTEMTIME ) + sizeof( DWORD ) ];    
    
    // 
    // Copy data to buffer.
    // 
    CopyMemory( buffer, &m_stDefDate, sizeof( SYSTEMTIME ) );
    CopyMemory( &buffer[ sizeof( SYSTEMTIME ) ], &m_dwDefRevision, sizeof( DWORD ) );

    // 
    // Open specified key
    // 
    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, szRegKey, 0, KEY_WRITE, &hKey))
        {
        if( ERROR_SUCCESS == RegSetValueEx(hKey, DEFWATCH_DEFVER_VAL, 0, dwType, buffer, dwSize) )
            {
            RegCloseKey( hKey );
            return TRUE;
            }
        
        // 
        // Cleanup
        // 
        RegCloseKey( hKey );
        }

    // 
    // Failure.
    // 
    return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CVirusDefVersion::operator==
//
// Return type: int 
//
// Argument         : CVirusDefVersion& other
//
///////////////////////////////////////////////////////////////////////////////
// 11/7/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
int CVirusDefVersion::operator==( const CVirusDefVersion& other)
{
    // 
    // Test for equality
    // 
    if( m_stDefDate.wDay != other.m_stDefDate.wDay ||
        m_stDefDate.wMonth != other.m_stDefDate.wMonth ||
        m_stDefDate.wYear != other.m_stDefDate.wYear ||
        m_dwDefRevision != other.m_dwDefRevision )
        {
        return 0;
        }

    // 
    // Looks equal
    // 
    return 1;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: CVirusDefVersion::operator!=
//
// Return type: int 
//
// Argument         : CVirusDefVersion& other
//
///////////////////////////////////////////////////////////////////////////////
// 11/7/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
int CVirusDefVersion::operator!=( const CVirusDefVersion& other)
{
    return !(*this == other);
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CVirusDefVersion::operator=
//
// Return type: CVirusDefVersion& 
//
// Argument         : CVirusDefVersion& copy
//
///////////////////////////////////////////////////////////////////////////////
// 11/7/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CVirusDefVersion& CVirusDefVersion::operator=( const CVirusDefVersion& copy )
{
    // 
    // Copy data members
    // 
    m_stDefDate = copy.m_stDefDate;
    m_dwDefRevision = copy.m_dwDefRevision;

    return *this;
}

