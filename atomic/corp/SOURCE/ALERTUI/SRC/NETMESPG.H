/////////////////////////////////////////////////////////////////////
// NetMesPg.h : header file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/NETMESPG.H_v   1.1   25 Apr 1997 11:03:48   JBRENNA  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/NETMESPG.H_v  $
// 
//    Rev 1.1   25 Apr 1997 11:03:48   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   13 Mar 1997 15:06:24   RFULLER
// Initial revision
// 
//    Rev 1.7   13 Jan 1997 20:41:54   SEDWARD
// Added support for help.
//
//    Rev 1.6   06 Jan 1997 00:42:32   SEDWARD
// Now uses temporary CAlertOptions list for edits.
//
//    Rev 1.4   03 Dec 1996 19:50:44   SEDWARD
// Substantial changes (not frozen yet...)
//
//    Rev 1.3   02 Dec 1996 16:34:52   SEDWARD
// Big changes, nearing UI freeze.
//
//    Rev 1.2   26 Nov 1996 17:21:46   SEDWARD
// Leading backslashes no longer written to DAT file, domains get a trailing
// space/asterisk appened to them.
//
//    Rev 1.1   25 Nov 1996 15:45:40   SEDWARD
// No change.
//
//    Rev 1.0   21 Nov 1996 17:26:06   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////



#include "helpdlg.h"                // base class for default help support




/////////////////////////////////////////////////////////////////////////////
// CNetMessagePg dialog
/////////////////////////////////////////////////////////////////////////////

class CNetMessagePg : public CHelpPropertyPage
{
    DECLARE_DYNCREATE(CNetMessagePg)

// Construction
public:
    CNetMessagePg();
    ~CNetMessagePg();

// Dialog Data
    //{{AFX_DATA(CNetMessagePg)
    enum { IDD = IDD_NETMESSAGE_PROP_PAGE };
    CListCtrl   m_netMessageList;
    CButton m_netMessageRemoveButton;
    CButton m_netMessageModifyButton;
    CButton m_netMessageAddButton;
	//}}AFX_DATA

    static DWORD    m_dwWhatsThisMap[];
    static DWORD    m_dwHowToMap[];


// Overrides
	// ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CNetMessagePg)
	public:
    virtual void OnOK(void);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
    //{{AFX_MSG(CNetMessagePg)
    afx_msg void OnNetMessageAddButton(void);
    afx_msg void OnNetMessageRemoveButton(void);
    afx_msg void OnClickNetMessageTargetList(NMHDR* pNMHDR, LRESULT* pResult);
    virtual BOOL OnInitDialog(void);
	afx_msg void OnNetMessageModifyButton(void);
    afx_msg void OnHandleNetMessageAdd(void);
    afx_msg void OnHandleNetMessageModify(void);
    afx_msg void OnHandleNetMessageRemove(void);
	afx_msg void OnDblclkNetmessageTargetList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnWhatsThisNetMessageListCtrl(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    void    HandleRclickNetMessageList(void);

    CAlertOptions*  m_targetsListPtr;


public:
    int             m_clickedItemIndex;
    CImageList      m_imageList;

    int     InitListbox(void);
    void    UpdateNetMessageList(void);
};
