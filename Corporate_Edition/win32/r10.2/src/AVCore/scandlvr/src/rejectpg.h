// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/REJECTPG.H_v   1.7   27 Apr 1998 02:06:08   SEDWARD  $
/////////////////////////////////////////////////////////////////////////////
//
// RejectPg.h: interface for the CRejectedFilesPropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/REJECTPG.H_v  $
// 
//    Rev 1.7   27 Apr 1998 02:06:08   SEDWARD
// Added OnWizardNext().
//
//    Rev 1.6   26 Apr 1998 17:25:34   SEDWARD
// Added GetTemplateID().
//
//    Rev 1.5   24 Apr 1998 18:17:20   SEDWARD
// Changed m_rejectedDescription to a CStatic control.
//
//    Rev 1.4   20 Apr 1998 18:04:42   SEDWARD
// Changed IDD string.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_REJECTPG_H__8B6F60A2_CC95_11D1_A7DC_0000E8D34392__INCLUDED_)
#define AFX_REJECTPG_H__8B6F60A2_CC95_11D1_A7DC_0000E8D34392__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ScanDeliverDLL.h"
#include "WizPage.h"


/////////////////////////////////////////////////////////////////////////////
// CRejectedFilesPropertyPage dialog

class CRejectedFilesPropertyPage : public CWizardPropertyPage
{
    DECLARE_DYNCREATE(CRejectedFilesPropertyPage)

// Construction
public:
    CRejectedFilesPropertyPage();
    CRejectedFilesPropertyPage(CScanDeliverDLL*  pDLL);
    ~CRejectedFilesPropertyPage();

// Dialog Data
    //{{AFX_DATA(CRejectedFilesPropertyPage)
	enum { IDD = IDD_REJECTED_FILES };
	CStatic	m_rejectedDescription;
    CListCtrl   m_rejectedFilesListCtrl;
	//}}AFX_DATA

    virtual UINT  GetTemplateID(void) { return  IDD; }


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CRejectedFilesPropertyPage)
    protected:
	virtual LRESULT OnWizardNext();
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CRejectedFilesPropertyPage)
    virtual BOOL OnInitDialog();
    afx_msg void OnItemchangedListRejectedFiles(NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    void InitializeColumnHeaders(void);
    void PopulateListCtrl(void);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REJECTPG_H__8B6F60A2_CC95_11D1_A7DC_0000E8D34392__INCLUDED_)