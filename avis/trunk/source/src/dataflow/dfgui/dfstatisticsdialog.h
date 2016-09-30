#if !defined(AFX_DFSTATISTICSDIALOG_H__2E99BE45_CD83_11D2_A4C2_0004ACECC1E1__INCLUDED_)
#define AFX_DFSTATISTICSDIALOG_H__2E99BE45_CD83_11D2_A4C2_0004ACECC1E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DFStatisticsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDFStatisticsDialog dialog

class CDFStatisticsDialog : public CDialog
{
// Construction
public:
	CDFStatisticsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDFStatisticsDialog)
	enum { IDD = IDD_STATISTICS_DIALOG };
	CString	m_strAverageProcessingTime;
	CString	m_strNumberOfSamples;
	CString	m_strNumberOfSuccessfulSamples;
	CString	m_strStartTime;
	CString	m_strDeferredSamples;
	CString	m_strRescannedSamples;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDFStatisticsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDFStatisticsDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFSTATISTICSDIALOG_H__2E99BE45_CD83_11D2_A4C2_0004ACECC1E1__INCLUDED_)
