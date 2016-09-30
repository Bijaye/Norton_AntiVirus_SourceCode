// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WizPage.h"

// CUserInfo4PropertyPage dialog

class CUserInfo4PropertyPage : public CWizardPropertyPage
{
	DECLARE_DYNAMIC(CUserInfo4PropertyPage)

public:
	//CUserInfo4PropertyPage(CWnd* pParent = NULL);   // standard constructor
	CUserInfo4PropertyPage();
	~CUserInfo4PropertyPage();

// Dialog Data
	enum { IDD = IDD_USER_INFO4 };
	BOOL m_bPopupsWithBrowser;
	BOOL m_bPopupsWithOutBrowser;
	BOOL m_bSlowSysPerf;
	BOOL m_bBrowserHmPgMod;
	BOOL m_bUnwantedBtTbAdded2Browser;
	BOOL m_bUnwantedPgVisits;
	BOOL m_bAppsDoNotWork;	
	BOOL m_bUnwantedAppsAdded2Dsktop;

	virtual UINT  GetTemplateID(void) { return  IDD; }

	// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CUserInfo1PropertyPage)
    public:
    virtual LRESULT OnWizardNext();
    virtual LRESULT OnWizardBack();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

protected:
	//virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL UpdateConfigData(void);
	// Generated message map functions
    //{{AFX_MSG(CUserInfo1PropertyPage)
    virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
