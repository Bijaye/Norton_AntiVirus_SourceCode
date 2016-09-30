/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/custresp.h_v   1.1   27 May 1997 20:55:42   DBUCHES  $
/////////////////////////////////////////////////////////////////////////////
// CustResp.h : header file
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/N32OPTS/VCS/custresp.h_v  $
// 
//    Rev 1.1   27 May 1997 20:55:42   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.0   21 May 1997 14:37:06   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUSTRESP_H__36803742_D147_11D0_931D_0060977341AC__INCLUDED_)
#define AFX_CUSTRESP_H__36803742_D147_11D0_931D_0060977341AC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "helpdlg.h"


/////////////////////////////////////////////////////////////////////////////
// CCustomResponseDlg dialog

class CCustomResponseDlg : public CHelperDialog
{
// Construction
public:
	BOOL m_bAP;
    int m_iFile;
    int m_iMacro;
    int m_iBoot;

	CCustomResponseDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCustomResponseDlg)
	enum { IDD = IDD_CUSTOM_RESPONSE_DIALOG };
	int		m_iFileAction;
	int		m_iMacroAction;
	int		m_iBootAction;
	//}}AFX_DATA
    static DWORD m_dwWhatsThisMap[];
    static DWORD m_dwHowToMap[];


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomResponseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCustomResponseDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTRESP_H__36803742_D147_11D0_931D_0060977341AC__INCLUDED_)
