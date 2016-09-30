//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// User2Pg.h: interface for the CUserInfo2PropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/User2Pg.h_v  $
// 
//    Rev 1.2   01 Jun 1998 17:06:14   SEDWARD
// Added support for the country drop-down listbox.
//
//    Rev 1.1   26 Apr 1998 17:25:40   SEDWARD
// Added GetTemplateID().
//
//    Rev 1.0   20 Apr 1998 16:26:12   SEDWARD
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_USER2PG_H__3392A3A1_D62B_11D1_A7F1_0000E8D34392__INCLUDED_)
#define AFX_USER2PG_H__3392A3A1_D62B_11D1_A7F1_0000E8D34392__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WizPage.h"


/////////////////////////////////////////////////////////////////////////////
// CUserInfo2PropertyPage dialog

class CUserInfo2PropertyPage : public CWizardPropertyPage
{
    DECLARE_DYNCREATE(CUserInfo2PropertyPage)

// Construction
public:
    CUserInfo2PropertyPage();
    ~CUserInfo2PropertyPage();

// Dialog Data
    //{{AFX_DATA(CUserInfo2PropertyPage)
	enum { IDD = IDD_USER_INFO2 };
    CString m_szCountry;
    CString m_szAddress1;
    CString m_szAddress2;
    CString m_szAtlasNumber;
    CString m_szCity;
    CString m_szState;
    CString m_szZipCode;
	//}}AFX_DATA

    virtual UINT  GetTemplateID() { return  IDD; }

// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CUserInfo2PropertyPage)
    public:
    virtual LRESULT OnWizardNext();
    virtual LRESULT OnWizardBack();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    virtual BOOL UpdateConfigData();
    // Generated message map functions
    //{{AFX_MSG(CUserInfo2PropertyPage)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    void UpdateUserCountryCode();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USER2PG_H__3392A3A1_D62B_11D1_A7F1_0000E8D34392__INCLUDED_)
