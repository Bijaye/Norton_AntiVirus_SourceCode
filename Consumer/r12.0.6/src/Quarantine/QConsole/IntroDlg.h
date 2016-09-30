//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// IntroDlg.h : interface of the CIntroDlg class.  Used to present description
//	of types of items in Quarantine (post Eraser integration).
//  User can choose not to see intro dlg again.
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTRODLG_H__A1342AC4_8E57_466e_A8C8_C18D8024A9FD__INCLUDED_)
#define AFX_INTRODLG_H__A1342AC4_8E57_466e_A8C8_C18D8024A9FD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


// CIntroDlg dialog

class CIntroDlg : public CDialog
{
	DECLARE_DYNAMIC(CIntroDlg)

public:
	CIntroDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CIntroDlg();

// Dialog Data
	enum {IDD = IDD_INTRO_DIALOG};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	
protected:
	bool SetBoldfaceFont(HWND hWnd);
	bool IsHighContrastMode();

	CString m_sIntroHeading;
	CString m_sExpandedThreatHeading;
	CString m_sExpandedThreatDescription;
	CString m_sViralHeading;
	CString m_sViralDescription;
	BOOL m_bIntroCheck;

public:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

private:
	CBrush * m_pWhiteBkBrush;
	CString m_sAppName;

public:
	bool GetIntroEnabled();
};


#endif // !defined(AFX_INTRODLG_H__A1342AC4_8E57_466e_A8C8_C18D8024A9FD__INCLUDED_)
