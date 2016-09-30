// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//PasswordManager.cpp

#include "stdafx.h"
#include "PasswordManager.h"
#include "SecureZeroMemory.h"

//ADDED_FOR_VC6
#include <atlbase.h>

//Initialize our instance to null
PasswordManager* PasswordManager::m_instance = NULL;

/*
 * Return the one instance of the PasswordManager.  If the instance is null, allocate a new one.
 */
PasswordManager* PasswordManager::getInstance()
{
	try
	{
		if ( NULL == m_instance )
		{
			m_instance = new PasswordManager();
		}
	}
	catch(std::bad_alloc &){}
	return m_instance;
}

/*
 * Call this when you're done with the password manager to release memory back to the heap.
 */
void PasswordManager::destroy()
{
	//Don't have to check for null, deleting a null pointer is safe
	delete m_instance;
	//Set the static instance back to null so that the next getInstance() call will work properly
	m_instance = NULL;
}

PasswordManager::PasswordManager()
{
	//Initialize COM
	::CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );

	HRESULT hr = ::CoCreateInstance( CLSID_TopologyContainer, NULL, CLSCTX_LOCAL_SERVER, IID_IPasswordBroker, (void**)&m_pPasswordBroker );
	
	if ( S_OK != hr )
	{
		m_pPasswordBroker = NULL;
	}
}

PasswordManager::~PasswordManager()
{
	//Release our COM pointer to the password broker.
    if ( NULL != m_pPasswordBroker )
    {
        m_pPasswordBroker->Release();
    }

	//Uninitialize COM
	::CoUninitialize();

	//Release our instance (Ouch! Don't do this! We are calling the destructor from the destructor! )
	//Use destory() instead
	//if ( NULL != m_instance )
	//{
	//	delete m_instance;
	//}
}

/*
 * IPasswordBroker's UnlockDomain() call will be used as authentication.
 */
BOOL PasswordManager::unlockServerGroup( LPCTSTR serverGroup, LPCTSTR password, LPCTSTR userName)
{
	//PasswordBroker parameters must be BSTRs
	CComBSTR bstrServerGroup = serverGroup;
	CComBSTR bstrPassword    = password;
	CComBSTR bstrUserName	 = userName;

	HRESULT result = m_pPasswordBroker->UnlockDomain( bstrServerGroup, bstrPassword, bstrUserName );

	//Sensitive strings need to be cleaned in memory (1-UAF93)
	SECURE_BSTR_ZERO_MEMORY( bstrServerGroup );
	SECURE_BSTR_ZERO_MEMORY( bstrPassword );
	SECURE_BSTR_ZERO_MEMORY( bstrUserName );

	if ( S_OK == result )
	{
		return TRUE;
	}

	return FALSE;
}

/*
 * Change a server group's password.
 */
BOOL PasswordManager::changeServerGroupPassword( LPCTSTR serverGroup, LPCTSTR userName, LPCTSTR oldPassword, LPCTSTR newPassword )
{
	//PasswordBroker parameters must be BSTRs
	CComBSTR bstrServerGroup = serverGroup;
	CComBSTR bstrUserName	 = userName;
	CComBSTR bstrOldPassword = oldPassword;
	CComBSTR bstrNewPassword = newPassword; 

	//TODO: fix the call to change the account info
	HRESULT result = E_FAIL;
	//HRESULT result = m_pPasswordBroker->ChangeDomainPassword( bstrServerGroup, bstrUserName, bstrOldPassword, bstrNewPassword );

	//Sensitive strings need to be cleaned in memory (1-UAF93)
	SECURE_BSTR_ZERO_MEMORY( bstrServerGroup );
	SECURE_BSTR_ZERO_MEMORY( bstrUserName );
	SECURE_BSTR_ZERO_MEMORY( bstrOldPassword );
	SECURE_BSTR_ZERO_MEMORY( bstrNewPassword );

	if ( S_OK == result )
	{
		return TRUE;
	}

	return FALSE;
}