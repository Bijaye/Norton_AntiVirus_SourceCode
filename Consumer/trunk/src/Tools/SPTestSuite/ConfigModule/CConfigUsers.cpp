////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <lmerr.h>
#include "CConfigUsers.h"
#include "LogOutput.h"

CConfigUsers::~CConfigUsers()
{
    // If configured, delete users that were successfully created
    if( m_bDeleteUsersOnExit && m_pCont != NULL )
    {
        for( std::vector<CStringW>::iterator iter = m_vCreatedUsers.begin();
             iter != m_vCreatedUsers.end(); iter++ )
        {
            CStringW& strUserName = *iter;
            HRESULT hr = m_pCont->Delete(CComBSTR(L"user"), CComBSTR(strUserName));
            if( FAILED(hr) )
            {
                BBGTH::CLogOutput::Log(L"DeleteUser(\"%s\") failed: %08X\n", strUserName.GetString(), hr);
            }
        }
    }
}

HRESULT CConfigUsers::Initialize(bool bDeleteUsersOnExit)
{
    m_bDeleteUsersOnExit = bDeleteUsersOnExit;

    if( m_pCont != NULL )
        return S_FALSE;

    // Find the local computer name
    ATL::CString strComputerName;
    DWORD dwComputerNameLen = MAX_COMPUTERNAME_LENGTH+1;
    PWSTR szComputerName = strComputerName.GetBuffer(dwComputerNameLen);
    BOOL bSuccess = GetComputerName(szComputerName, &dwComputerNameLen);
    strComputerName.ReleaseBuffer();
    if( !bSuccess )
        return HRESULT_FROM_WIN32(GetLastError());

    // Retrieve and store the AD interface object for the local computer
    CString strComputerObject;
    strComputerObject.Format(L"WinNT://%s,Computer", strComputerName.GetString());
    return ADsGetObject(strComputerObject, IID_IADsContainer, (void**)&m_pCont);
}

HRESULT CConfigUsers::CreateUser(PCWSTR szUserName, PCWSTR szPassword)
{
    if( m_pCont == NULL )
        return E_FAIL;

    HRESULT hr;

    // Now add a user to the local computer object
    CComPtr<IDispatch> piUserDispatch;
    hr = m_pCont->Create(CComBSTR(L"user"), CComBSTR(szUserName), &piUserDispatch);
    if( FAILED(hr) )
        return hr;

    // Query the new object for the IADsUser interface
    CComQIPtr<IADsUser> pUser(piUserDispatch);
    if( pUser == NULL )
        return E_NOINTERFACE;

    // Set the user's password
    hr = pUser->SetPassword(CComBSTR(szPassword));
    if( FAILED(hr) )
        return hr;

    // Commit the change back to the parent object
    hr = pUser->SetInfo();

    // Save the created user name so it can be deleted when the object
    // is destructed
    if( SUCCEEDED(hr) )
    {
        m_vCreatedUsers.push_back(CStringW(szUserName));
    }
    else if( (hr & 0xFFFF) == NERR_UserExists ) // The account already exists
    {
        // The user exists, but was not created
        hr = S_FALSE;
    }

    return hr;
}
