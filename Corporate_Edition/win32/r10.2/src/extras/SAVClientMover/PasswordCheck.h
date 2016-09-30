// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// PasswordCheck.h: interface for the CPasswordCheck class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PASSWORDCHECK_H__5BFFBF64_4DC9_11D7_BBDD_0010A4972180__INCLUDED_)
#define AFX_PASSWORDCHECK_H__5BFFBF64_4DC9_11D7_BBDD_0010A4972180__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPasswordCheck  
{
public:
	HRESULT GetPassword( const CString &csServerName );
	CArray<CString, CString> m_CachedPasswords;
	CPasswordCheck();
	virtual ~CPasswordCheck();

};

#endif // !defined(AFX_PASSWORDCHECK_H__5BFFBF64_4DC9_11D7_BBDD_0010A4972180__INCLUDED_)
