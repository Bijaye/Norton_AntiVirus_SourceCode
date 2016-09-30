/////////////////////////////////////////////////////////////////////
// ForwrdPg.h : header file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/FORWRDPG.H_v   1.1   25 Apr 1997 11:03:46   JBRENNA  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/FORWRDPG.H_v  $
// 
//    Rev 1.1   25 Apr 1997 11:03:46   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   13 Mar 1997 15:06:24   RFULLER
// Initial revision
// 
//    Rev 1.6   13 Jan 1997 20:41:52   SEDWARD
// Added support for help.
//
//    Rev 1.5   06 Jan 1997 00:41:48   SEDWARD
// Now uses temporary CAlertOptions list for edits.
//
//    Rev 1.3   03 Dec 1996 19:50:40   SEDWARD
// Substantial changes (not frozen yet...)
//
//    Rev 1.2   02 Dec 1996 16:34:50   SEDWARD
// Big changes, nearing UI freeze.
//
//    Rev 1.1   25 Nov 1996 15:45:38   SEDWARD
// No change.
//
//    Rev 1.0   21 Nov 1996 17:26:04   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#include "helpdlg.h"                // base class for default help support




/////////////////////////////////////////////////////////////////////////////
// CForwardPg dialog
/////////////////////////////////////////////////////////////////////////////

class CForwardPg : public CHelpPropertyPage
{
	DECLARE_DYNCREATE(CForwardPg)

// Construction
public:
	CForwardPg();
	~CForwardPg();

// Dialog Data
	//{{AFX_DATA(CForwardPg)
	enum { IDD = IDD_FORWARD_PROP_PAGE };
	CListCtrl	m_forwardList;
	CButton	m_forwardRemoveButton;
	CButton	m_forwardModifyButton;
	CButton	m_forwardAddButton;
	//}}AFX_DATA

    static DWORD    m_dwWhatsThisMap[];
    static DWORD    m_dwHowToMap[];


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CForwardPg)
	public:
    virtual void OnOK(void);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CForwardPg)
    afx_msg void OnForwardAddButton(void);
    afx_msg void OnForwardRemoveButton(void);
	afx_msg void OnClickForwardTargetList(NMHDR* pNMHDR, LRESULT* pResult);
    virtual BOOL OnInitDialog(void);
    afx_msg void OnForwardModifyButton(void);
    afx_msg void OnHandleForwardModify(void);
    afx_msg void OnHandleForwardAdd(void);
    afx_msg void OnHandleForwardRemove(void);
	afx_msg void OnDblclkForwardTargetList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnWhatsThisForwardListCtrl(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    void    HandleRclickForwardTargetList(void);

    CAlertOptions*  m_targetsListPtr;

public:
    int             m_clickedItemIndex;
    CImageList      m_imageList;

    int     InitListbox(void);
};
