////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// OEHUIDlg.h: interface for the COEHeurUIDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OEHEURUIDLG_H__66BAF363_7BF9_440A_8C99_3C722E70CC18__INCLUDED_)
#define AFX_OEHEURUIDLG_H__66BAF363_7BF9_440A_8C99_3C722E70CC18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseAlertUI.h"

class COEHeurUIDlg : 
    public CBaseAlertUI
{
public:
	COEHeurUIDlg();
	virtual ~COEHeurUIDlg();

	void SetProcessName(const wchar_t* szProcessName);
	void SetEmailInfo( const wchar_t* szSubject,
                       const wchar_t* szSender, 
		               const wchar_t* szRecipient);
	void SetActionQuar(bool bEnabled);

    int DoModal (); // Returns index of the action combo box

protected:

	// Process Name.
	tstring m_szProcessName;

	// Email info (sender, recipient(s), and subject).
    std::wstring m_strSender;
	std::wstring m_strRecipient;
	std::wstring m_strSubject;

	// Enable Quarantine.
	bool m_bQuarEnabled;

};

#endif // !defined(AFX_OEHEURUIDLG_H__66BAF363_7BF9_440A_8C99_3C722E70CC18__INCLUDED_)