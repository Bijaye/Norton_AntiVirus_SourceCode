#if !defined(AFX_DFCONFIGUREDIALOG_H__68A4E743_CCF8_11D2_A4C2_0004ACECC1E1__INCLUDED_)
#define AFX_DFCONFIGUREDIALOG_H__68A4E743_CCF8_11D2_A4C2_0004ACECC1E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DFConfigureDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DFConfigureDialog dialog

class DFConfigureDialog : public CDialog
{
// Construction
public:
	DFConfigureDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DFConfigureDialog)
	enum { IDD = IDD_CONFIGURE_DIALOG };
	CString	m_strMaxNumber;
	CString	m_strSubmissionInterval;
	CString	m_strUNCPath;
	CString	m_strArrivalInterval;
	CString	m_strDefBaseDir;
	CString	m_strBuildDefFilename;
	CString	m_strDatabaseRescanInterval;
	CString	m_strStatusUpdateInterval;
	CString	m_strDefImportInterval;
	CString	m_strDefImporterDir;
	CString	m_strUndeferrerInterval;
	CString	m_strAttributeInterval;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DFConfigureDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DFConfigureDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFCONFIGUREDIALOG_H__68A4E743_CCF8_11D2_A4C2_0004ACECC1E1__INCLUDED_)
