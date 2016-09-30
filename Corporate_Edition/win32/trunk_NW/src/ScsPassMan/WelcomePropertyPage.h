// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#pragma once


// CWelcomePropertyPage dialog

class CWelcomePropertyPage : public CPropertyPage
{
//REMOVED_FOR_VC6//	DECLARE_DYNAMIC(CWelcomePropertyPage)

public:
	CWelcomePropertyPage();
	virtual ~CWelcomePropertyPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_WELCOME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
