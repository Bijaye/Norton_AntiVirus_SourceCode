// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// BackupPage.h: interface for the CBackupPage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BACKUPPAGE_H__01104EA0_2831_11D3_919E_00C04F980248__INCLUDED_)
#define AFX_BACKUPPAGE_H__01104EA0_2831_11D3_919E_00C04F980248__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WizardPage.h"

class CBackupPage : public CWizardPage  
{
	DECLARE_DYNCREATE(CBackupPage)

public:

	CBackupPage();
	virtual ~CBackupPage();

// Dialog Data
	//{{AFX_DATA(CBackupPage)
	enum { IDD = IDD_BACKUP };
	CStatic	m_ctlTitle;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBackupPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	void SizePage( int cx, int cy );
	long OnResultsDlgClosing( WPARAM, LPARAM );
	long OnCreateScanDlg( WPARAM wParam, LPARAM );
	LRESULT OnFillBin( WPARAM, LPARAM );
    LRESULT AddItemToView( IVBin2 *ptrVBin, VBININFO *vBinInfo );

	// Generated message map functions
	//{{AFX_MSG(CBackupPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	CResultsView *m_pResultsViewDlg;
	HWND          m_hChildWnd;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BACKUPPAGE_H__01104EA0_2831_11D3_919E_00C04F980248__INCLUDED_)
