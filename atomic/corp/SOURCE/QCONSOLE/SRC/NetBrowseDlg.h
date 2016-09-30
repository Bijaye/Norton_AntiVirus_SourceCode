/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/netbrowsedlg.h_v   1.1   20 May 1998 17:13:40   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// NetBrowseDlg.h : header file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/netbrowsedlg.h_v  $
// 
//    Rev 1.1   20 May 1998 17:13:40   DBuches
// 1st pass at help.
// 
//    Rev 1.0   08 May 1998 17:22:34   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETBROWSEDLG_H__33890F64_E6A4_11D1_9118_00C04FAC114A__INCLUDED_)
#define AFX_NETBROWSEDLG_H__33890F64_E6A4_11D1_9118_00C04FAC114A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "nettree.h"
#include "helpdlg.h"
#include "nrescont.h"


/////////////////////////////////////////////////////////////////////////////
// CNetBrowseDlg dialog

class CNetBrowseDlg : public CHelperDialog
{
// Construction
public:
	CNetBrowseDlg(CWnd* pParent = NULL);   // standard constructor

    CString m_sServer;

// Help maps
    static DWORD m_dwWhatsThisMap[];
    static DWORD m_dwHowToMap[];


// Dialog Data
	//{{AFX_DATA(CNetBrowseDlg)
	enum { IDD = IDD_NET_BROWSE };
    CNetworkTreeCtrl    m_treeCtrl;
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetBrowseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNetBrowseDlg)
    afx_msg void OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
    virtual void OnOK();
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETBROWSEDLG_H__33890F64_E6A4_11D1_9118_00C04FAC114A__INCLUDED_)
