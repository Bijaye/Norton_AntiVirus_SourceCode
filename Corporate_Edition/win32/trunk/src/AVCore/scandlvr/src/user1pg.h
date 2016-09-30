// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/User1Pg.h_v   1.4   30 Jun 1998 15:43:10   sedward  $
/////////////////////////////////////////////////////////////////////////////
//
// User1Pg.h: interface for the CUserInfo1PropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/User1Pg.h_v  $
// 
//    Rev 1.4   30 Jun 1998 15:43:10   sedward
// Enhanced email address validation (defect #118024).
//
//    Rev 1.3   26 May 1998 13:03:40   SEDWARD
// Email address validation.
//
//    Rev 1.2   27 Apr 1998 02:07:50   SEDWARD
// Added RequiredFieldsFilled().
//
//    Rev 1.1   26 Apr 1998 17:25:38   SEDWARD
// Added GetTemplateID().
//
//    Rev 1.0   20 Apr 1998 16:26:12   SEDWARD
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_USER_INFO_H__8D28F8F4_C3FA_11D1_A7C2_0000E8D34392__INCLUDED_)
#define AFX_USER_INFO_H__8D28F8F4_C3FA_11D1_A7C2_0000E8D34392__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WizPage.h"

#define AT_SIGN_CHAR        '@'
#define PERIOD_CHAR         '.'
#define SPACE_CHAR          ' '


/////////////////////////////////////////////////////////////////////////////
// CUserInfo1PropertyPage dialog

class CUserInfo1PropertyPage : public CWizardPropertyPage
{
    DECLARE_DYNCREATE(CUserInfo1PropertyPage)

// Construction
public:
    CUserInfo1PropertyPage();
    ~CUserInfo1PropertyPage();

// Dialog Data
    //{{AFX_DATA(CUserInfo1PropertyPage)
    enum { IDD = IDD_USER_INFO1 };
    CString m_szFax;
    CString m_szPhone;
    CString m_szEmail;
    CString m_szFirstName;
    CString m_szLastName;
    CString m_szCompany;
    //}}AFX_DATA

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

// Implementation
protected:
    virtual BOOL UpdateConfigData(void);
    // Generated message map functions
    //{{AFX_MSG(CUserInfo1PropertyPage)
    virtual BOOL OnInitDialog();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    BOOL    IsEmailAddressValid(void);
    BOOL    RequiredFieldsFilled(void);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USER_INFO_H__8D28F8F4_C3FA_11D1_A7C2_0000E8D34392__INCLUDED_)