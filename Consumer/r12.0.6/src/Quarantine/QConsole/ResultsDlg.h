//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// ResultsDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/ResultsDlg.h_v  $
// 
//    Rev 1.4   10 Apr 1998 14:14:06   DBuches
// Added Compressed file type.
// 
//    Rev 1.3   07 Apr 1998 16:49:40   DBuches
// 1st pass at startup scanning.
// 
//    Rev 1.2   31 Mar 1998 15:02:46   DBuches
// Added GetAction() method.
// 
//    Rev 1.1   13 Mar 1998 15:21:58   DBuches
// Added support for repair.
// 
//    Rev 1.0   11 Mar 1998 15:19:02   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESULTSDLG_H__2A0722E3_B7B4_11D1_910B_00C04FAC114A__INCLUDED_)
#define AFX_RESULTSDLG_H__2A0722E3_B7B4_11D1_910B_00C04FAC114A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "sortlist.h"
#include "afxtempl.h"


/////////////////////////////////////////////////////////////////////////////
// CResultsListCtrl class delcaration

class CResultsListCtrl : public CSortedListCtrl
{
    DECLARE_DYNAMIC( CResultsListCtrl )

public:
    CResultsListCtrl(){};
    ~CResultsListCtrl(){};
    virtual int CompareFunc( LPARAM lParam1, LPARAM lParam2 );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResultsListCtrl)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CResultsListCtrl)
    afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CRepairAction
class CRepairAction
{
public:
    // Construction
    CRepairAction();
    ~CRepairAction();

    // Operators
    void SetAction(int iAction, LPCTSTR szData);
    int GetAction(){ return m_iRepairAction; }; 
    
    CString GetName(){ return m_sData; };
    
    void SetIconIndex(int iIconIndex) { m_iIconIndex = iIconIndex; };
    int GetIconIndex(){ return m_iIconIndex; };
    
    CString Format();
    int Compare( CRepairAction *p);

    // Overrides
    BOOL operator == ( int iAction );

private:
    // Repair action taken
    int m_iRepairAction;

    // Extra data.
    CString m_sData;

    // Icon index
    int m_iIconIndex;

};

typedef CTypedPtrList< CPtrList, CRepairAction* > CRepairActionList;


/////////////////////////////////////////////////////////////////////////////
// CResultsListItem

class CResultsListItem
{
public:
    CResultsListItem() :
      m_iListItem(NULL),
      m_dwSuccessfulOperations(NULL),
      m_dwFailedOperations(NULL)
    {};
    ~CResultsListItem()
    {
         m_dwSuccessfulOperations = 0;
    };

    // public data
    CRepairAction   m_Action;
    CString         m_sItemName;
    int             m_iListItem;

    DWORD m_dwSuccessfulOperations;
    DWORD m_dwFailedOperations;

    CRepairActionList m_aSubActionList;

};


/////////////////////////////////////////////////////////////////////////////
// CResultsDlg dialog

class CResultsDlg : public CDialog
{
// Construction
public:
	CResultsDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CResultsDlg();

// Dialog Data
	//{{AFX_DATA(CResultsDlg)
	enum { IDD = IDD_RESULTS_DIALOG };
	CResultsListCtrl	m_ListCtrl;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResultsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CResultsDlg)
	virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedBtnDetails();
    afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnListViewItemDoubleClick(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
    void SetImageList( CImageList* pImageList );
    void SetCaption( int iID ){ m_iCaptionID = iID; }

    // List of items for list control
    CTypedPtrList< CPtrList, CResultsListItem* > m_aItemList;

    BOOL m_bDetails;

private:
	void SetupListCtrl();
    CResultsListItem * GetCurSelItem();

private:
    CImageList * m_pImageList;
    int m_iCaptionID;
};


/////////////////////////////////////////////////////////////////////////////
// Column IDs

const int RESULTS_FILE_NAME = 0;
const int RESULTS_ACTION    = 1;
const int RESULTS_MAX       = (RESULTS_ACTION + 1);


/////////////////////////////////////////////////////////////////////////////
// Action taken Values
const int RESULTS_ACTION_RESTORED        = 0;
const int RESULTS_ACTION_REPAIRED        = 1;
const int RESULTS_ACTION_STILL_INFECTED  = 2;
const int RESULTS_ACTION_NO_ACTION       = 3;
const int RESULTS_ACTION_ERROR           = 4;
const int RESULTS_ACTION_COMPRESSED_ITEM = 5;
const int RESULTS_ACTION_DELETED		 = 6;
const int RESULTS_ACTION_CANCELLED       = 7;

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESULTSDLG_H__2A0722E3_B7B4_11D1_910B_00C04FAC114A__INCLUDED_)
