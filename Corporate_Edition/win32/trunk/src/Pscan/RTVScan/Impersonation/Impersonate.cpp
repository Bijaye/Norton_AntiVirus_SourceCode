// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2005 Symantec Corporation. All rights reserved.
//*************************************************************************
// file Impersonate.cpp :
// Simple wrapper that makes it easier to manage the lifetime of
// various impersonation APIs.
//


#include "SavAssert.h"
#include "Impersonate.h"
#include "dprintf.h"


CImpersonate::CImpersonate() :
				m_bRevertToSelfNeeded(FALSE)
{
}

CImpersonate::~CImpersonate()
{
	BOOL bResult = RevertToSelf();

	SAVASSERT(bResult);
}

/** 
 * BOOL ImpersonateLoggedOnUser(CAccessToken& objAccessToken)
 *
 * uses new accesstoken class
 *
**/
BOOL CImpersonate::ImpersonateLoggedOnUser(CAccessToken& objAccessToken)
{
    dprintfTag2(DEBUGIMPERSONATION, "Impersonation: Impersonating user(%s):session(%d)\n", objAccessToken.GetUserName(), objAccessToken.GetSessionId());
    
    BOOL bResult = FALSE;
    bResult = ::ImpersonateLoggedOnUser(objAccessToken);

	// We'll do a RevertToSelf() if any ImpersonateLoggedOnUser() call succeeds
	if (bResult)
    {
        m_objAccessToken = objAccessToken;
		m_bRevertToSelfNeeded = TRUE;
    }
    else
        dprintfTag1(DEBUGIMPERSONATION, "Impersonation: failed(%d)\n", GetLastError());

    return bResult;
}



//use cached token
BOOL CImpersonate::ImpersonateLoggedOnUser()
{
	BOOL bResult = FALSE;

    bResult = ::ImpersonateLoggedOnUser(m_objAccessToken);
    

	// We'll do a RevertToSelf() if any ImpersonateLoggedOnUser() call succeeds
	if (bResult)
		m_bRevertToSelfNeeded = TRUE;
  
	return bResult;

}

/* This has the same semantecs as Win32 API by the same name.
* This will be called automatically in the destructor if
* ImpersonateLoggedOnUser() was called on this instance with no
* balancing call to RevertToSelf().
*/
BOOL CImpersonate::RevertToSelf()
{
	BOOL bResult = TRUE;
    //don't do dprintfs in this code. Needed in dprintf caused endless loop
	if (m_bRevertToSelfNeeded)
	{
		bResult = ::RevertToSelf();
		if (bResult)
        {
			m_bRevertToSelfNeeded = FALSE;
        }
	}

	return bResult;
}
