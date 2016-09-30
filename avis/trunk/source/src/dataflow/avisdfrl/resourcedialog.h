#if !defined(AFX_RESOURCEDIALOG_H__DCE13BF3_56EC_11D2_B974_0004ACEC31AA__INCLUDED_)
#define AFX_RESOURCEDIALOG_H__DCE13BF3_56EC_11D2_B974_0004ACEC31AA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ResourceDialog.h : header file
//

#define ADD_RESOURCE        1
#define MODIFY_RESOURCE     2
#define COPY_RESOURCE       3

#include "DFResource.h"

/////////////////////////////////////////////////////////////////////////////
// CResourceDialog dialog

class CResourceDialog : public CDialog
{
// Construction
public:
	int m_DialogType;
	CDFResourceList * m_pDFResourceList;
	CResourceDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CResourceDialog)
	enum { IDD = IDD_DIALOG_RESOURCE_INFO };
	CComboBox	m_MachineCtrl;
	CStatic	m_TimeoutCountStaticCtrl;
	CStatic	m_ErrorCountStaticCtrl;
	CEdit	m_TimeoutCountEditCtrl;
	CEdit	m_ErrorCountEditCtrl;
	CButton	m_TimeoutCountResetBtn;
	CButton	m_ErrorCountResetBtn;
	CStatic	m_JobIDPromptCtrl;
	CComboBox	m_JobIDCtrl;
	CString	m_JobID;
	CString	m_Program;
	CString	m_Timeout;
	CString	m_User;
	CString	m_ntMachineName;
	UINT	m_ErrorCount;
	UINT	m_TimeoutCount;
	CString	m_Machine;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResourceDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CResourceDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonErrorCountReset();
	afx_msg void OnButtonTimeoutCountReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESOURCEDIALOG_H__DCE13BF3_56EC_11D2_B974_0004ACEC31AA__INCLUDED_)
