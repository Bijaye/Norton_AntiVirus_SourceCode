/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_SELECTTARGETDIALOG_H__D07DA746_B959_11D3_97AC_00C04F688464__INCLUDED_)
#define AFX_SELECTTARGETDIALOG_H__D07DA746_B959_11D3_97AC_00C04F688464__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectTargetDialog.h : header file
//


//#include <afxtempl.h>     // for CList
#include "resource.h"
#include "myListCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CSelectTargetDialog dialog

class CSelectTargetDialog : public CDialog
{
// Construction
public:
	CSelectTargetDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectTargetDialog)
	enum { IDD = IDD_SELECTTARGETDIALOG };
	CMyListCtrl	m_TargetListCtrl;
	CButton	m_HelpButtonControl;
	CString	m_szHeading;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectTargetDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectTargetDialog)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnHelpButtonDialog2();
	afx_msg void OnDelete();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:

// IMPLEMENTATION
    void Initialize(DWORD nIDTitle,
                    DWORD nIDHeading,
                    DWORD nIDCol1,
                    CString& sTargetCommaList,
                    //CString * psTargetCommaList,
                    int iItemCount,
                    DWORD dwDefaultHelpID,
                    AVIS_HELP_ID_STRUCT* pHelpIdArray );

    DWORD    FillListWithData();
    DWORD    RemoveDuplicateItems();
    BOOL     IsItemDuplicate(LPTSTR lpszBuff);


// DIALOG DATA
    DWORD                   m_nIDTitle;
    DWORD                   m_nIDHeading;
    DWORD                   m_nIDCol1;
    DWORD                   m_dwDefaultHelpID;
    DWORD                   m_dwDefaultOverviewHelpID;
    DWORD                   m_dwInitialized;
    int                     m_iItemCount;
    DWORD                   m_dwListCtrlFlags;         // FLAGS PASSED TO myLISTCTRL
    AVIS_HELP_ID_STRUCT*    m_lpHelpIdArray;
    int                     m_iTotalReturnedItems;
    BOOL                    m_bListModified;
    CString                 m_sTargetStrings;          // Comma delimited list
    //CString *               m_psPassedTargetList;

    //DWORD                   m_nIDCol2;
    //LIST_DATA_STRUCT *m_lpItemList;


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTTARGETDIALOG_H__D07DA746_B959_11D3_97AC_00C04F688464__INCLUDED_)
