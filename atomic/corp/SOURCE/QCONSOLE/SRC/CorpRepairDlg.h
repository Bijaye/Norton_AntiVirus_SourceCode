/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/CorpRepairDlg.h_v   1.1   18 May 1998 13:34:08   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// CorpRepairDlg.h : header file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/CorpRepairDlg.h_v  $
// 
//    Rev 1.1   18 May 1998 13:34:08   DBuches
// Added restore text.
// 
//    Rev 1.0   11 May 1998 15:43:36   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_CORPREPAIRDLG_H__DCF76D74_E91A_11D1_9118_00C04FAC114A__INCLUDED_)
#define AFX_CORPREPAIRDLG_H__DCF76D74_E91A_11D1_9118_00C04FAC114A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CCorpRepairDlg dialog

class CCorpRepairDlg : public CDialog
{
// Construction
public:
	CCorpRepairDlg(CWnd* pParent = NULL, BOOL bRepair = FALSE);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCorpRepairDlg)
	enum { IDD = IDD_CORP_REPAIR_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCorpRepairDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

    BOOL m_bRepair;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCorpRepairDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CORPREPAIRDLG_H__DCF76D74_E91A_11D1_9118_00C04FAC114A__INCLUDED_)
