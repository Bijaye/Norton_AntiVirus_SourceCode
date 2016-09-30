// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#pragma once

#include "WizPage.h"

// CReview1PropertyPage dialog

class CReview1PropertyPage : public CWizardPropertyPage
{
	DECLARE_DYNAMIC(CReview1PropertyPage)

public:
	CReview1PropertyPage(CScanDeliverDLL*  pDLL);   // standard constructor
	~CReview1PropertyPage();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
// Dialog Data
	enum { IDD = IDD_REVIEW1 };
	BOOL m_bSubmitAddInfo;			//Boolean member for Submit Additional Information
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()	
};
