// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_GETSTATUS_H__919B98F1_61FD_11D1_AB13_00A0C90F8F6F__INCLUDED_)
#define AFX_GETSTATUS_H__919B98F1_61FD_11D1_AB13_00A0C90F8F6F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GetStatus.h : header file
//


//#include "sendtointel.h"
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// Local defines
#define STATUSITEM_NOT_PROCESSED 0xffffffff


/////////////////////////////////////////////////////////////////////////////
// CGetStatus dialog

class CGetStatus : public CDialog
{
// Construction
public:
	DWORD GetNumberOfItemsProcessed();
	DWORD CleanupQuarantine(DWORD dwAction);
	LONG MinY;
	LONG MinX;
	BOOL m_bInitialized;
	DWORD FillAndGetStatus(void);
	CGetStatus(CResultsView *ResulstView);
	CGetStatus(CWnd* pParent = NULL);   // standard constructor
	CResultsView *m_ResultsView;
	DWORD State;
	DWORD Action;

// Dialog Data
	//{{AFX_DATA(CGetStatus)
	enum { IDD = IDD_GETSTATUS };
	CStatic	m_StatusText;
	CButton	m_Help;
	CButton	m_Cancel;
	CAnimateCtrl	m_Avi;
	CButton	m_OK;
	CEdit	m_TotalCount;
	CListCtrl	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetStatus)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_szDefaultRestoreDir;
	BOOL FileExists(LPCTSTR lpFile, BOOL bPrompt);
	BOOL CheckInfectorFlags(CString &szLogLine, DWORD dwInfectorFlags);
    BOOL CheckInfectorFlags(CResultItem *pResultItem, DWORD dwInfectorFlags);
	DWORD GetItemStatus(CString &str, DWORD State, DWORD Action, CString &szLogLine);
	BOOL ReplaceFile(LPTSTR lpTempFile, LPCTSTR lpNewFile);
	BOOL RestoreFile(LPTSTR lpTempFile, LPCTSTR lpOriginalFile);
    BOOL IsExpandedThreat( CResultItem *pResultItem );
#ifdef _USE_CCW
	BOOL FileExists(LPCWSTR lpFile, BOOL bPrompt);
#endif	

typedef struct {
	CResultItem *Log;
	DWORD State;
	DWORD Data;
    DWORD dwResult;
	} STATUSITEM;


	DWORD SetQuarItemToBackup(STATUSITEM *pItem);
	int CleanVBinItem(STATUSITEM* pItemStatus);
	int CleanLegacyVBinItem(STATUSITEM* pItemStatus, char * szInfectionLogLine );
    int CleanRemediationVBinItem( STATUSITEM *pAnomalyStatus, IVBinSession *pVBinSession, VBININFO *pRemediationInfo );
	int CleanAnomalyVBinItem(STATUSITEM* pAnomalyStatus);
	int UndoAnomalyVBinItems(STATUSITEM* pAnomalyStatus, LPCTSTR sOriginalFileName);
	int UndoVBinItem(STATUSITEM *pItemStatus);
	BOOL CreateTempFileName(STATUSITEM* pItemStatus, LPTSTR lpTempFileName);
	DWORD GetResult(STATUSITEM *pItem);
	DWORD GetNumInfectedItemsInSession(STATUSITEM *pItem);

	// Generated message map functions
	//{{AFX_MSG(CGetStatus)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnGetMinMaxInfo( MINMAXINFO* lpMMI );
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnItemchangingList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHelp();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL m_bUserCancel;
	DWORD m_dwNumberOfItemsProcessed;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETSTATUS_H__919B98F1_61FD_11D1_AB13_00A0C90F8F6F__INCLUDED_)
