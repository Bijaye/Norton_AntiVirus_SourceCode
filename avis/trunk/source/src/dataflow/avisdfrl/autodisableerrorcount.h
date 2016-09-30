#if !defined(AFX_AUTODISABLEERRORCOUNT_H__E44AED16_78CB_11D2_B97C_0004ACEC31AA__INCLUDED_)
#define AFX_AUTODISABLEERRORCOUNT_H__E44AED16_78CB_11D2_B97C_0004ACEC31AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoDisableErrorCount.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAutoDisableErrorCount dialog

class CAutoDisableErrorCount : public CDialog
{
// Construction
public:
	CAutoDisableErrorCount(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAutoDisableErrorCount)
	enum { IDD = IDD_DIALOG_AUTO_DISABLE_ERROR_COUNT };
	int		m_ErrorCount;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoDisableErrorCount)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAutoDisableErrorCount)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTODISABLEERRORCOUNT_H__E44AED16_78CB_11D2_B97C_0004ACEC31AA__INCLUDED_)
