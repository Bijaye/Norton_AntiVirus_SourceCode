/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_CONFIRMATIONDIALOG_H__F34AB8B9_EB0B_11D2_9792_00C04F688464__INCLUDED_)
#define AFX_CONFIRMATIONDIALOG_H__F34AB8B9_EB0B_11D2_9792_00C04F688464__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfirmationDialog.h : header file
//


#include <afxtempl.h>     // for CList

#include "resource.h"
#include "myListCtrl.h"
//#include "SubmitOrDeliver.h"



// PASS A REF TO THE ITEM DATA INTO THE DIALOG
//typedef struct tagLIST_DATA_STRUCT
//{
//    int      iItem;
//    LPTSTR   lpszItemStr;
//    LPTSTR   lpszSubItemStr;
//    LPARAM   lParam;
//    TCHAR             szItemStr[259];
//    TCHAR             szSubItemStr[259];
//} LIST_DATA_STRUCT;

typedef struct tagLIST_DATA_STRUCT
{
    //CString           sItemStr;
    //CString           sSubItemStr;
    TCHAR             sItemStr[259];
    TCHAR             sSubItemStr[259];
    LPARAM            lParam;
    int               iItem;
    int               iIsSelected;
    int               iIsDisabled;
} LIST_DATA_STRUCT;
// LIST_RESULT_DATA_STRUCT






/////////////////////////////////////////////////////////////////////////////
// CConfirmationDialog dialog

class CConfirmationDialog : public CDialog
{
// Construction
public:
	CConfirmationDialog(CWnd* pParent = NULL);   // standard constructor
    void Initialize(DWORD nIDTitle,DWORD nIDHeading,DWORD nIDCol1,
                    DWORD nIDCol2, DWORD dwDefaultHelpID,
                    LIST_DATA_STRUCT *pItemList, int iItemCount,
                    AVIS_HELP_ID_STRUCT* pHelpIdArray );
                     
    HRESULT ReturnSelectedItem( int nItem, LPARAM lParam );



// DIALOG DATA
    DWORD m_nIDTitle;
    DWORD m_nIDHeading;
    DWORD m_nIDCol1;
    DWORD m_nIDCol2;
    DWORD m_dwDefaultHelpID;
    DWORD m_dwDefaultOverviewHelpID;
    DWORD m_dwInitialized;
    int   m_iItemCount;
    LIST_DATA_STRUCT *m_lpItemList;
    DWORD m_dwListCtrlFlags;         // FLAGS PASSED TO myLISTCTRL

    //CList<LIST_DATA_STRUCT, LIST_DATA_STRUCT&>  m_SelectedItemList;

    AVIS_HELP_ID_STRUCT*   m_lpHelpIdArray;
    int   m_iTotalSelectedItems;

	//{{AFX_DATA(CConfirmationDialog)
	enum { IDD = IDD_CONFIRMDIALOG };
	CButton	m_HelpButtonCtrl;
	CMyListCtrl	m_ListCtrl;
	CString	m_szHeading;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfirmationDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
    DWORD CConfirmationDialog::FillListWithData();


	// Generated message map functions
	//{{AFX_MSG(CConfirmationDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnHelpButtonDialog2();
	virtual void OnOK();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIRMATIONDIALOG_H__F34AB8B9_EB0B_11D2_9792_00C04F688464__INCLUDED_)
