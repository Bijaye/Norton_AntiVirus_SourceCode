////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// FolderBrowseDlg.h : header file
//

#if !defined(AFX_FOLDERBROWSEDLG_H__12FDA320_B773_11D2_B9D1_00C04FAC114C__INCLUDED_)
#define AFX_FOLDERBROWSEDLG_H__12FDA320_B773_11D2_B9D1_00C04FAC114C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "nvbrowse.h"
#include "wndsize.h"
#include "resource.h"
#include "..\\NavTaskWizardRes\\resource.h"


/////////////////////////////////////////////////////////////////////////////
// CFolderBrowseDlg dialog -- Custom folder browse dialog, pulled from
// Atomic.

class CFolderBrowseDlg : public CDialog, public IValidate
{
// Construction
public:
	CFolderBrowseDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFolderBrowseDlg)
	enum { IDD = IDD_SELECT_SCAN_FOLDERS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

    BOOL           m_bScanning;         // TRUE => used to select folders for a scan
                                        // FALSE => being called from the task wizard

    CNVBrowse      m_browse;
    CWindowResizer m_resizer;
    CRect          m_rectMin;
    
    virtual void   OnItemChanged ();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFolderBrowseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFolderBrowseDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnScan();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CString m_csProductName;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FOLDERBROWSEDLG_H__12FDA320_B773_11D2_B9D1_00C04FAC114C__INCLUDED_)
