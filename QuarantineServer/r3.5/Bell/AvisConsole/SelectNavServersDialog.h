#if !defined(AFX_SELECTNAVSERVERSDIALOG_H__D07DA746_B959_11D3_97AC_00C04F688464__INCLUDED_)
#define AFX_SELECTNAVSERVERSDIALOG_H__D07DA746_B959_11D3_97AC_00C04F688464__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectNavServersDialog.h : header file
//


#include "resource.h"
#include "myListCtrl.h"
//#include "ListNavServers.h"



/////////////////////////////////////////////////////////////////////////////
// CSelectNavServersDialog dialog

class CSelectNavServersDialog : public CDialog
{
// Construction
public:
	CSelectNavServersDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectNavServersDialog)
	enum { IDD = IDD_SELECT_NAV_SERVER_DIALOG };  //IDD_SELECTTARGETDIALOG
	CMyListCtrl	m_NavServerListCtrl;
	CButton	m_HelpButtonControl;
	CString	m_szHeading;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectNavServersDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectNavServersDialog)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual BOOL OnInitDialog();
	afx_msg void OnHelpButtonDialog2();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:

// IMPLEMENTATION
    void Initialize(DWORD nIDTitle,
                    DWORD nIDHeading,
                    DWORD nIDCol1,
                    CMyListCtrl* pListCtrl,
                    DWORD dwDefaultHelpID,
                    AVIS_HELP_ID_STRUCT* pHelpIdArray );

    DWORD    FillListWithData();
    BOOL     IsItemDuplicate( LPTSTR lpszNewItem );


// DIALOG DATA
    //SServerData*            m_pSServerData;
    CMyListCtrl*            m_pCallerListCtrl;      // CALLER"S LIST CTRL
    DWORD                   m_nIDTitle;
    DWORD                   m_nIDHeading;
    DWORD                   m_nIDCol1;
    DWORD                   m_dwDefaultHelpID;
    DWORD                   m_dwDefaultOverviewHelpID;
    DWORD                   m_dwInitialized;
    //int                     m_iItemCount;
    DWORD                   m_dwListCtrlFlags;         // FLAGS PASSED TO myLISTCTRL
    AVIS_HELP_ID_STRUCT*    m_lpHelpIdArray;
    int                     m_iTotalReturnedItems;
    BOOL                    m_bListModified;


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTNAVSERVERSDIALOG_H__D07DA746_B959_11D3_97AC_00C04F688464__INCLUDED_)
