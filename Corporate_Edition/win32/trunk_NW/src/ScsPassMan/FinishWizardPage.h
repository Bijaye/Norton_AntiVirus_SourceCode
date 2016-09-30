// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//FinishWizardPage.h

#pragma once
#include "NewWizPage.h"

// CFinishWizardPage dialog

class CFinishWizardPage : public CNewWizPage
{
//REMOVED_FOR_VC6//	DECLARE_DYNAMIC(CFinishWizardPage)

public:
	CFinishWizardPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFinishWizardPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_FINISH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT OnWizardBack( void );
	DECLARE_MESSAGE_MAP()

public:
	void OnSetActive();
};
