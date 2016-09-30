//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// RemediationPage.h : header file
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RemediationPage_H__47B210C4_A968_11D1_A523_0000C06F46D0__INCLUDED_)
#define AFX_RemediationPage_H__47B210C4_A968_11D1_A523_0000C06F46D0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "proppage.h"

/////////////////////////////////////////////////////////////////////////////
// CRemediationPage dialog

class CRemediationPage : public CItemPage
{
    DECLARE_DYNCREATE(CRemediationPage)

    // Construction
public:
    CRemediationPage();
    ~CRemediationPage();

    // Dialog Data
    //{{AFX_DATA(CRemediationPage)
    enum { IDD = IDD_REMEDIATION_PAGE };
    CString	m_cszVirusName;
    CListCtrl m_cwndComponentsList;
    CListCtrl m_cwndDetailsList;
    //}}AFX_DATA

    void SetVirusIndex( unsigned long ulVID ) { m_ulVID = ulVID; }

    // Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CRemediationPage)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CRemediationPage)
    virtual BOOL OnInitDialog();
    afx_msg void OnComponentListSelChange(NMHDR *pNMHDR, LRESULT *pResult);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

protected:
    void FillImageList(CImageList &cImageList);
    void FillComponentList();
    void FillDetailsList(INT iComponentGroup);

private:
    CSymPtr<IScanVirusInfo> m_pScanVirusInfo;
    unsigned long m_ulVID;
    CImageList m_cimgSmallIcons;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RemediationPage_H__47B210C4_A968_11D1_A523_0000C06F46D0__INCLUDED_)
