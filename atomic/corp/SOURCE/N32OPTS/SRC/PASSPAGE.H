/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/passpage.h_v   1.2   11 Jul 1997 12:30:20   DBUCHES  $
/////////////////////////////////////////////////////////////////////////////
//
//
// PassPage.h : header file
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
// $Log:   S:/N32OPTS/VCS/passpage.h_v  $
// 
//    Rev 1.2   11 Jul 1997 12:30:20   DBUCHES
// Set horizontal scroll width of listbox correctly.
// 
//    Rev 1.1   27 May 1997 20:56:56   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.0   14 May 1997 15:09:26   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_PASSPAGE_H__159C3AEF_C317_11D0_8AD6_0060977341AC__INCLUDED_)
#define AFX_PASSPAGE_H__159C3AEF_C317_11D0_8AD6_0060977341AC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "helpdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CPasswordPage dialog

class CPasswordPage : public CHelperPage
{
// Construction
public:
	void HighlightSelectedItems(BOOL bEnable);
	UINT MapItemToFeature(int iIndex);
	void PopulateList();
	void UpdateSelections(BOOL bUpdate = TRUE);
	DWORD m_dwPassword;
    BYTE  m_abProtectFeature[MAX_PROTECT_FEATURE];

	BOOL SetData(LPNAVOPTIONS pOptions);
	BOOL GetData(LPNAVOPTIONS pOptions);
    CPasswordPage();
    ~CPasswordPage();
// Dialog Data
	//{{AFX_DATA(CPasswordPage)
	enum { IDD = IDD_PASSWORD_PAGE };
	CListBox	m_ListBox;
	int		m_iMaximum;
	BOOL	m_bPassword;
	//}}AFX_DATA
    static DWORD m_dwWhatsThisMap[];
    static DWORD m_dwHowToMap[];


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPasswordPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPasswordPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnPasswordCheck();
	afx_msg void OnMaximumRadio();
	afx_msg void OnSelchangeList();
	afx_msg void OnSetpwButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    void SetScrollWidth();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSPAGE_H__159C3AEF_C317_11D0_8AD6_0060977341AC__INCLUDED_)
