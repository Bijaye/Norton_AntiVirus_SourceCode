// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//PasswordManageWizardPage.h

#pragma once
#include "NewWizPage.h"
#include "PasswordManager.h"
#include "afxwin.h"


// CPasswordManageWizardPage dialog

class CPasswordManageWizardPage : public CNewWizPage
{
//REMOVED_FOR_VC6//	DECLARE_DYNAMIC(CPasswordManageWizardPage)

public:
	CPasswordManageWizardPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPasswordManageWizardPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_MANAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void checkForEnableOrDisable();
	virtual BOOL changePassword();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
//	afx_msg void OnCbnEditchangeManageAccountCombobox();
	afx_msg void OnEnChangeManageOldpassEditbox();
	afx_msg void OnEnChangeManageNewpassTextbox();
	afx_msg void OnEnChangeManageAccountEditbox();
	afx_msg void OnEnChangeManageConfirmpassTextbox();
	afx_msg void OnBnClickedManageChangeButton();
	afx_msg void OnEnable(BOOL bEnable);
	BOOL m_failureOccurred;
//	CComboBox m_accountComboBox;
	CString m_oldPassword;
	CString m_newPassword;
	CString m_confirmPassword;
	CButton m_passwordChangeButton;
	CStatic m_manageStatusText;
	CString m_account;
	CString m_serverGroup;
	void OnSetActive();
};
