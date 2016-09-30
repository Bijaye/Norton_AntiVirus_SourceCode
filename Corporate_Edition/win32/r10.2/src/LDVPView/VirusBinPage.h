// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_VIRUSBINPAGE_H__494AC227_7180_11D1_81CC_00A0C95C0756__INCLUDED_)
#define AFX_VIRUSBINPAGE_H__494AC227_7180_11D1_81CC_00A0C95C0756__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// VirusBinPage.h : header file
//

////////////////////////////////////////////////////////////////////////////
//Date: 12/28/2004		KTALINKI
//Modified the class to use IScanDeliverDll3 instead of IScanDeliverDll2 interface
////////////////////////////////////////////////////////////////////////////
#include "WizardPage.h"

#define INITGUID
#include "iquaran.h"
#include "iscandeliver.h"

/////////////////////////////////////////////////////////////////////////////
// CVirusBinPage dialog

class CVirusBinPage : public CWizardPage
{
	DECLARE_DYNCREATE(CVirusBinPage)

private:
	HWND          m_hChildWnd;
	CResultsView *m_pResultsViewDlg;

    //IScanDeliverDLL2* m_pScanDeliver;
	IScanDeliverDLL3* m_pScanDeliver;

protected:
	void SizePage( int cx, int cy );
	long OnResultsDlgClosing( WPARAM, LPARAM );
	long OnCreateScanDlg( WPARAM wParam, LPARAM );
	LRESULT OnFillBin( WPARAM, LPARAM );
    LRESULT AddItemToView( IVBin2 *ptrVBin, VBININFO *vBinInfo );

// Construction
public:
	HINSTANCE m_hInst;

	CVirusBinPage();
	~CVirusBinPage();
	LRESULT OnScanDeliver( WPARAM wParam, LPARAM lParam);

// Dialog Data
	//{{AFX_DATA(CVirusBinPage)
	enum { IDD = IDD_VIRUS_BIN };
	CStatic	m_ctlTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CVirusBinPage)
	public:
	virtual BOOL OnWizardFinish();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void CheckDefwatchKeys();
	BOOL m_bCheckDefwatchKeys;
	CString m_szSNDLogString;
	BOOL GetDefinitionsDir(CString *lpDefsDir);
	BOOL GetInstallDir(CString *lpInstallDir);
	BOOL GetDefinitionDate(SYSTEMTIME* lpDefDate);
	// Generated message map functions
	//{{AFX_MSG(CVirusBinPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIRUSBINPAGE_H__494AC227_7180_11D1_81CC_00A0C95C0756__INCLUDED_)
