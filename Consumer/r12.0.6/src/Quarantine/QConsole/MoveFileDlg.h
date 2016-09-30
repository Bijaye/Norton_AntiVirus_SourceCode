/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/movefiledlg.h_v   1.2   21 May 1998 11:28:26   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// MoveFileDlg.h : header file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/movefiledlg.h_v  $
// 
//    Rev 1.2   21 May 1998 11:28:26   DBuches
// Added help topics.
// 
//    Rev 1.1   20 May 1998 17:13:40   DBuches
// 1st pass at help.
// 
//    Rev 1.0   06 Mar 1998 21:09:36   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOVEFILEDLG_H__2DB73A43_B55A_11D1_910A_00C04FAC114A__INCLUDED_)
#define AFX_MOVEFILEDLG_H__2DB73A43_B55A_11D1_910A_00C04FAC114A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//

/////////////////////////////////////////////////////////////////////////////
// CMoveFileDlg dialog

class CMoveFileDlg : public CFileDialog
{
	DECLARE_DYNAMIC(CMoveFileDlg)

public:
	CMoveFileDlg(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

    BOOL m_bDelete;

protected:
	//{{AFX_MSG(CMoveFileDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    CString m_sTitle;
    
    TCHAR   m_szInitialDir[5];

    int     m_iCtrlID;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOVEFILEDLG_H__2DB73A43_B55A_11D1_910A_00C04FAC114A__INCLUDED_)
