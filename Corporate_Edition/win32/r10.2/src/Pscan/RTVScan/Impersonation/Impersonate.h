////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Impersonate.h  
//  
// Simple wrapper that makes it easier to manage the lifetime of
// various impersonation APIs.
//
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005, Symantec Corporation, All rights reserved.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __IMPERSONATE_H
#define __IMPERSONATE_H

#include <windows.h>
#include "accesstoken.h"

class CImpersonate
{
public:
	CImpersonate();
    virtual ~CImpersonate();

    /** 
     * BOOL ImpersonateLoggedOnUser(CAccessToken& objAccessToken)
     *
     * uses new accesstoken class
     *
    **/
    BOOL ImpersonateLoggedOnUser(CAccessToken& objAccessToken);

    //use cached token
    BOOL ImpersonateLoggedOnUser();

	/* This has the same semantecs as Win32 API by the same name.
	* This will be called automatically in the destructor if
	* ImpersonateLoggedOnUser() was called on this instance with no
	* balancing call to RevertToSelf().
	*/
	BOOL RevertToSelf();

    operator HANDLE(){ return m_objAccessToken; }

protected:
	CImpersonate(const CImpersonate&);
	bool operator==(const CImpersonate&);
	CImpersonate& operator= (const CImpersonate&);

private:
	BOOL m_bRevertToSelfNeeded;
    CAccessToken    m_objAccessToken;
};

#endif // __IMPERSONATE_H
