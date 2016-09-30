// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_RESTOREFILEDIALOG_H__BAF77D74_05E3_4339_81C8_4131733FE96B__INCLUDED_)
#define AFX_RESTOREFILEDIALOG_H__BAF77D74_05E3_4339_81C8_4131733FE96B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//
// RestoreFileDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRestoreFileDialog dialog
//
class CRestoreFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CRestoreFileDialog)

public:
	CRestoreFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
					   LPCTSTR lpszDefExt = NULL,
					   LPCTSTR lpszFileName = NULL,
					   DWORD dwFlags = OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST|OFN_EXPLORER,
					   LPCTSTR lpszFilter = NULL,
					   CWnd* pParentWnd = NULL);
	virtual ~CRestoreFileDialog(void);

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();

private:
	CString m_sTitle;

};
#endif // !defined(AFX_RESTOREFILEDIALOG_H__BAF77D74_05E3_4339_81C8_4131733FE96B__INCLUDED_)
