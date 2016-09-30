/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_QPROPPAGE_H__F2FA7330_F9D4_11D2_A44F_00105AA739C9__INCLUDED_)
#define AFX_QPROPPAGE_H__F2FA7330_F9D4_11D2_A44F_00105AA739C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QPropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CQPropPage dialog

class CQPropPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CQPropPage)

// Construction
public:
	CQPropPage(UINT uID);
    CQPropPage() : m_pHelpWhatsThisMap( NULL ){};
	~CQPropPage();

// Dialog Data
	//{{AFX_DATA(CQPropPage)
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CQPropPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnHelpWhatsthis();
    afx_msg LRESULT OnHelpMessage(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
    static BOOL CALLBACK EnumProc( HWND hWnd, LPARAM lParam );
protected:
	// Generated message map functions
	//{{AFX_MSG(CQPropPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    DWORD GetHelpTopic( DWORD wID );
    
protected:
    DWORD *     m_pHelpWhatsThisMap;
	CString		m_sHelpFile;

private:
    CWnd *      m_pContextHelpWnd;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QPROPPAGE_H__F2FA7330_F9D4_11D2_A44F_00105AA739C9__INCLUDED_)
