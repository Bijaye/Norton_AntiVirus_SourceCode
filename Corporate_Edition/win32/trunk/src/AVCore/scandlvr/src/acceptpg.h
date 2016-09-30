// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/AcceptPg.h_v   1.5   26 Apr 1998 17:25:22   SEDWARD  $
/////////////////////////////////////////////////////////////////////////////
//
// AcceptPg.h: interface for the CAcceptedFilesPropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/AcceptPg.h_v  $
// 
//    Rev 1.5   26 Apr 1998 17:25:22   SEDWARD
// Added GetTemplateID().
//
//    Rev 1.4   20 Apr 1998 20:57:48   SEDWARD
// Removed OnKillActive() and OnSetActive().
//
//    Rev 1.3   20 Apr 1998 18:04:38   SEDWARD
// Changed IDD string.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACCEPTPG_H__8B6F60A3_CC95_11D1_A7DC_0000E8D34392__INCLUDED_)
#define AFX_ACCEPTPG_H__8B6F60A3_CC95_11D1_A7DC_0000E8D34392__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ScanDeliverDLL.h"
#include "WizPage.h"


/////////////////////////////////////////////////////////////////////////////
// CAcceptedFilesPropertyPage dialog

class CAcceptedFilesPropertyPage : public CWizardPropertyPage
{
    DECLARE_DYNCREATE(CAcceptedFilesPropertyPage)

// Construction
public:
    CAcceptedFilesPropertyPage();
    CAcceptedFilesPropertyPage(CScanDeliverDLL*  pDLL);
    ~CAcceptedFilesPropertyPage();

// Dialog Data
    //{{AFX_DATA(CAcceptedFilesPropertyPage)
    enum { IDD = IDD_ACCEPTED_FILES };
    CListCtrl   m_acceptedFilesListCtrl;
    //}}AFX_DATA

    virtual UINT  GetTemplateID(void) { return  IDD; }


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CAcceptedFilesPropertyPage)
	protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CAcceptedFilesPropertyPage)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    void InitializeColumnHeaders(void);
    void PopulateListCtrl(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCEPTPG_H__8B6F60A3_CC95_11D1_A7DC_0000E8D34392__INCLUDED_)