// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//WelcomeWizardPage.h

#pragma once
#include "NewWizPage.h"
#include "afxwin.h"

// CWelcomeWizardPage dialog

class CWelcomeWizardPage : public CNewWizPage
{
//REMOVED_FOR_VC6//	DECLARE_DYNAMIC(CWelcomeWizardPage)

public:
	CWelcomeWizardPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWelcomeWizardPage();
	CStatic& getWelcomeText();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_WELCOME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CStatic m_welcomeText;
public:
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEnable(BOOL bEnable);
};
