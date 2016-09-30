// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_ADDFILE_H__D6E8BDF0_29EB_11D3_919F_00C04F980248__INCLUDED_)
#define AFX_ADDFILE_H__D6E8BDF0_29EB_11D3_919F_00C04F980248__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddFile.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddFile dialog

class CAddFile : public CFileDialog
{
	DECLARE_DYNAMIC(CAddFile)

public:
	CAddFile(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

    BOOL m_bDelete;

protected:
	//{{AFX_MSG(CAddFile)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnCancelMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

    CString m_sTitle;    
    TCHAR   m_szInitialDir[5];
    int     m_iCtrlID;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDFILE_H__D6E8BDF0_29EB_11D3_919F_00C04F980248__INCLUDED_)
