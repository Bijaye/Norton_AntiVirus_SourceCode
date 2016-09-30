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

	void SetProcessName(const char* szProcessName);
	void SetEmailInfo( const char* szSubject,
                       const char* szSender, 
		               const char* szRecipient);
	void SetActionQuar(bool bEnabled);

    int DoModal (); // Returns index of the action combo box

protected:

	// Process Name.
	tstring m_szProcessName;

	// Email info (sender, recipient(s), and subject).
	tstring m_strSender;
	tstring m_strRecipient;
	tstring m_strSubject;

	// Enable Quarantine.
	bool m_bQuarEnabled;

};

#endif // !defined(AFX_OEHEURUIDLG_H__66BAF363_7BF9_440A_8C99_3C722E70CC18__INCLUDED_)