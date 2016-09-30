////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// OEHeurUI.h: interface for the COEHeurUI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OEHEURUI_H__970EA803_605C_481E_B083_6E93EF0CA35D__INCLUDED_)
#define AFX_OEHEURUI_H__970EA803_605C_481E_B083_6E93EF0CA35D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OEHUIInterface.h"

class COEHeurUI : public IOEHeurUIW, 
				  public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
	SYM_INTERFACE_MAP_BEGIN()                
		SYM_INTERFACE_ENTRY( IID_IOEHeurUIW, IOEHeurUIW ) 
	SYM_INTERFACE_MAP_END()  
	
	COEHeurUI();
	virtual ~COEHeurUI();

	// IOEHeurUI overrides.
	void SetProcessName(const wchar_t* szProcessName);
	void SetEmailInfo(const wchar_t* szSubject, const wchar_t* szSender, 
		const wchar_t* szRecipient);
	bool Show(OEHUI_TYPE UIType, OEHACTION* pAction = NULL);
	void DeleteObject();

private:
	// Process Name.
	::std::wstring m_strProcessName;

	// Email sender.
	::std::wstring m_strSender;

	// Email recipient(s).
	::std::wstring m_strRecipient;

	// Email subject.
	::std::wstring m_strSubject;
};

#endif // !defined(AFX_OEHEURUI_H__970EA803_605C_481E_B083_6E93EF0CA35D__INCLUDED_)
