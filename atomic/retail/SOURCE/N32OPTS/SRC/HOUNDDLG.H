/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/hounddlg.h_v   1.1   25 Jun 1997 11:54:46   DBUCHES  $
/////////////////////////////////////////////////////////////////////////////
//
//
// houndDlg.h : Contains class definition for CBloodhoundDlg.
//
//
//
//
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/N32OPTS/VCS/hounddlg.h_v  $
// 
//    Rev 1.1   25 Jun 1997 11:54:46   DBUCHES
// Hooked up more help topics.
// 
//    Rev 1.0   13 Jun 1997 13:41:18   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_HOUNDDLG_H__8D886733_E414_11D0_8AE8_0060977341AC__INCLUDED_)
#define AFX_HOUNDDLG_H__8D886733_E414_11D0_8AE8_0060977341AC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "helpdlg.h"
// houndDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBloodhoundDlg dialog

class CBloodhoundDlg : public CHelperDialog
{
// Construction
public:
	int m_iProtectionLevel;
	CBloodhoundDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBloodhoundDlg)
	enum { IDD = IDD_BLOODHOUND_DIALOG };
	CSliderCtrl	m_SliderCtrl;
	BOOL	m_bEnabled;
	//}}AFX_DATA
    static DWORD m_dwWhatsThisMap[];
    static DWORD m_dwHowToMap[];


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBloodhoundDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBloodhoundDlg)
	afx_msg void OnEnableCheck();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void SetProtectionText();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOUNDDLG_H__8D886733_E414_11D0_8AE8_0060977341AC__INCLUDED_)
