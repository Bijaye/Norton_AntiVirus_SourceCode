#if !defined(AFX_ACTIVITYLOGSETTING_H__CA825393_7357_11D2_B97C_0004ACEC31AA__INCLUDED_)
#define AFX_ACTIVITYLOGSETTING_H__CA825393_7357_11D2_B97C_0004ACEC31AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ActivityLogSetting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CActivityLogSetting dialog

class CActivityLogSetting : public CDialog
{
// Construction
public:
	CActivityLogSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CActivityLogSetting)
	enum { IDD = IDD_DIALOG_ACTIVITY_LOG_SETTING };
	UINT	m_LogBufferSize;
	CString	m_LogFileName;
	UINT	m_MaxLogFileSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActivityLogSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CActivityLogSetting)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTIVITYLOGSETTING_H__CA825393_7357_11D2_B97C_0004ACEC31AA__INCLUDED_)
