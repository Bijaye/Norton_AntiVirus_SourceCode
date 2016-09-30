/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/exportdlg.h_v   1.1   21 May 1998 11:29:06   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// ExportDlg.h : header file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/exportdlg.h_v  $
// 
//    Rev 1.1   21 May 1998 11:29:06   DBuches
// Added help topic IDs.
// 
//    Rev 1.0   14 Apr 1998 00:41:56   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPORTDLG_H__9A282804_D32B_11D1_9118_00C04FAC114A__INCLUDED_)
#define AFX_EXPORTDLG_H__9A282804_D32B_11D1_9118_00C04FAC114A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CExportDlg dialog

class CExportDlg : public CFileDialog
{
// Construction
public:
   CExportDlg(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

// Dialog Data
	//{{AFX_DATA(CExportDlg)
	enum { IDD = IDD_EXPORT_DIALOG };
	int		m_iExport;
	int		m_iFormat;
	//}}AFX_DATA


    int     m_iViewMode;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK( );
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExportDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    CString m_sTitle;

    int     m_iCtrlID;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPORTDLG_H__9A282804_D32B_11D1_9118_00C04FAC114A__INCLUDED_)
