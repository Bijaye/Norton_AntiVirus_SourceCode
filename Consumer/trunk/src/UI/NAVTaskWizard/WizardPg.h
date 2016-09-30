////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// WizardPg.h : header file
//

#ifndef AFX_WIZARDPG_H__F48DE343_D474_11D2_8D3B_EFF7DE59A93E__INCLUDED_
#define AFX_WIZARDPG_H__F48DE343_D474_11D2_8D3B_EFF7DE59A93E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CBaseWizPage is the base class for wizard page classes.  It provides
// the GetParentSheet() function, and sets default window styles common to
// all pages.

class CBaseWizPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CBaseWizPage)

// Construction
public:
    CBaseWizPage();
    CBaseWizPage(int iDlgID);
    virtual ~CBaseWizPage();

// Attributes
public:
// Dialog Data
    //{{AFX_DATA(CRepWizPage)
        // NOTE - ClassWizard will add data members here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CRepWizPage)
    public:
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    CPropertySheet* GetParentSheet() const
        { return static_cast<CPropertySheet*>(GetParent()); }

	static CString m_csProductName;

protected:
    // Generated message map functions
    //{{AFX_MSG(CRepWizPage)
        // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif  // ndef AFX_WIZARDPG_H__F48DE343_D474_11D2_8D3B_EFF7DE59A93E__INCLUDED_
