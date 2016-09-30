// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//ConnectWizardPage.h

#pragma once
#include "NewWizPage.h"
#include "PasswordManager.h"
#include "afxwin.h"
#include <string>
#include <set>

typedef std::set<std::string> StringSet;

// CConnectWizardPage dialog
class CConnectWizardPage : public CNewWizPage
{
//REMOVED_FOR_VC6//	DECLARE_DYNAMIC(CConnectWizardPage)

public:
	CConnectWizardPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConnectWizardPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_CONNECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT OnWizardBack( void );
	virtual LRESULT OnWizardNext( void );
//	virtual BOOL connectToPrimaryServer();
//	virtual void checkForEnableOrDisable();
	DECLARE_MESSAGE_MAP()

public:
	//afx_msg void OnBnClickedConnectNetworkBrowseButton();
//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
//	afx_msg void OnEnChangeConnectUsernameEditbox();
//	afx_msg void OnEnChangeConnectPasswordEditbox();
	afx_msg void OnEnable(BOOL bEnable);
	BOOL m_failureOccurred;
	CComboBox m_serverGroupComboBox;
	CString m_scsUsername;
	CString m_scsPassword;
	CString m_serverGroup;
	CStatic m_connectStatusText;
	void OnSetActive();

private:
	BOOL getServerGroupNames( StringSet& stringSet );
};