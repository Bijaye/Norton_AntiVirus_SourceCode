////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// DrivePickerDlg.h : header file
//

#if !defined(AFX_DRIVEPICKERDLG_H__B25D420D_E4B9_4087_B63E_5536CADE79EC__INCLUDED_)
#define AFX_DRIVEPICKERDLG_H__B25D420D_E4B9_4087_B63E_5536CADE79EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "..\\NavTaskWizardRes\\Resource.h"
#include "ScanTaskInterface.h"

/////////////////////////////////////////////////////////////////////////////
// CDrivePickerDlg dialog

class CDrivePickerDlg : public CDialog
{
// Construction
public:

	CDrivePickerDlg(avScanTask::IScanTask* pScanTask, CWnd* pParent = NULL);   // standard constructor

// Destruction    
    virtual ~CDrivePickerDlg();

// Dialog Data
	enum { IDD = IDD_SELECT_SCAN_DRIVES };

    // Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrivePickerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

protected:
    CListCtrl   m_listDrives;
	avScanTask::IScanTask*  m_pScanTask;
	int         m_nSelected;
    CButton     m_btnScan;

// Implementation
protected:

    // Generated message map functions
	//{{AFX_MSG(CDrivePickerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnScan();
    afx_msg void OnCancel();
	afx_msg void OnClickDriveList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownDriveList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Generated message map functions
	//LRESULT OnCancel(WORD code, WORD id, HWND hwndCtrl, BOOL& bHandled);
	//LRESULT OnKeydown(int id, NMHDR* pNMHDR, BOOL& bHandled);
	//LRESULT OnClick(int id, NMHDR* pNMHDR, BOOL& bHandled);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRIVEPICKERDLG_H__B25D420D_E4B9_4087_B63E_5536CADE79EC__INCLUDED_)
