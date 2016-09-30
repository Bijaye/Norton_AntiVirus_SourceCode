#if !defined(AFX_MODULEDIALOG_H__66A8FF58_D7CB_11D2_B98E_0004ACEC31AA__INCLUDED_)
#define AFX_MODULEDIALOG_H__66A8FF58_D7CB_11D2_B98E_0004ACEC31AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModuleDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModuleDialog dialog

class CModuleDialog : public CDialog
{
// Construction
public:
	CModuleDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModuleDialog)
	enum { IDD = IDD_DIALOG_MODULE_INFO };
	CEdit	m_ModuleArgumentsCtrl;
	CEdit	m_ModuleNameCtrl;
	CString	m_ModuleFilePath;
	CString	m_ModuleName;
	CString	m_ModuleArguments;
	BOOL	m_AutoRestart;
	UINT	m_AutoStopTime;
	UINT	m_RestartTime;
	BOOL	m_SendMail;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModuleDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModuleDialog)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODULEDIALOG_H__66A8FF58_D7CB_11D2_B98E_0004ACEC31AA__INCLUDED_)
