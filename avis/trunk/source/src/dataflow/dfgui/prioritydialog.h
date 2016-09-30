#if !defined(AFX_PRIORITYDIALOG_H__4D0730D3_F9A3_11D2_A4E2_0004ACECC1E1__INCLUDED_)
#define AFX_PRIORITYDIALOG_H__4D0730D3_F9A3_11D2_A4E2_0004ACECC1E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PriorityDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPriorityDialog dialog

class CPriorityDialog : public CDialog
{
// Construction
public:
	CPriorityDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPriorityDialog)
	enum { IDD = IDD_CHANGE_PRIORITY };
	CString	m_trackingNumber;
	CString	m_priority;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPriorityDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPriorityDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRIORITYDIALOG_H__4D0730D3_F9A3_11D2_A4E2_0004ACECC1E1__INCLUDED_)
