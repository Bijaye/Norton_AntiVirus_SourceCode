// Copyright 1997 Symantec Corporation, Peter Norton Product Group
//**********************************************************************
// $Header:   S:/PRIVMAN/VCS/PMANDLG.H_v   1.0   25 Jun 1997 16:06:34   DSACKIN  $
//
// Description:   PrivMan Main Dlg processing.
//
//**********************************************************************
// $Log:   S:/PRIVMAN/VCS/PMANDLG.H_v  $
// 
//    Rev 1.0   25 Jun 1997 16:06:34   DSACKIN
// Initial revision.
// 
//    Rev 1.1   08 May 1997 10:19:02   pvenkat
// Added new member variables for bitmaps on treelist (top & topmask).
// 
//**********************************************************************
//

// PrivMANDlg.h : header file
//
#include        <afxcmn.h>
#include        <winnetwk.h>
#include        <lmcons.h>
#include        <lmserver.h>
#include        <lmaccess.h>
#include        <lmapibuf.h>


#define		SRC_USER			1
#define		SRC_LOCAL_GROUP		2
#define		SRC_GLOBAL_GROUP	3


/////////////////////////////////////////////////////////////////////////////
// CPrivMANDlg dialog

class CPrivMANDlg : public CDialog
{
// Construction
public:
	CPrivMANDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPrivMANDlg)
	enum { IDD = IDD_PRIVMAN_DIALOG };
	CTreeCtrl	m_Tree;
	CListBox	m_SrcList;
	CListBox	m_SelList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrivMANDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPrivMANDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg LONG OnRefresh (UINT wParam, LPARAM lParam);
	afx_msg void OnUsers();
	afx_msg void OnGroups();
	afx_msg void OnSelchangeGrpusr();
	afx_msg void OnSelchangedDomainList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkDomainList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelete();
	afx_msg void OnClearAll();
	afx_msg void OnAdd();
	afx_msg void OnGrant();
	afx_msg void OnRevoke();
	afx_msg void OnGlobalGroups();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	HTREEITEM	m_hCurItem;
	HCURSOR		m_hWaitCursor;
	CImageList	m_Image;
	int			m_nDomainBM;
	int			m_nDomainSelBM;
	int			m_nMachineBM;
	int			m_nMachineSelBM;
   int         m_nTopBM;
   int         m_nTopSelBM;

	BOOL	GetAllMachines (DWORD dwType, LPTSTR lpszServer, LPDWORD lpdwCount, LPSERVER_INFO_100 *lplpList);
	VOID	EnumMachines(HTREEITEM hItem, LPTSTR lpszServer);
	VOID	InitImages();
	VOID	Refresh();
	VOID	RefreshSrcList (short nType);
};
