// NetBrAppDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNetBrAppDlg dialog

#include "Netbrwsd.h"

class CNetBrAppDlg : public CDialog
{
// Construction
public:
	CNetBrAppDlg(CWnd* pParent = NULL);	// standard constructor

	void DisplaySettings( void );
	void RetrieveSettings( void );

	void SetCheckIfFlag( UINT uControlID, DWORD dwFlag );
    void GetCheckSetFlag( UINT uControlID, DWORD dwFlag, DWORD *lpdwFlagList );

// Dialog Data
	//{{AFX_DATA(CNetBrAppDlg)
	enum { IDD = IDD_NETBRAPP_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetBrAppDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
    CNetBrowsDlg m_cNetBrowsDlg;

	// Generated message map functions
	//{{AFX_MSG(CNetBrAppDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBrowseServer();
	afx_msg void OnBrowserTypeDefault();
	afx_msg void OnBrowserTypeNlm();
	afx_msg void OnBrowserTypeForward();
	afx_msg void OnBrowserTypeNetMessage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
