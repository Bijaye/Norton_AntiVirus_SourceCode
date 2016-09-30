#if !defined(AFX_COOKIESTATISTICS_H__838C6D03_CFEF_11D2_B98E_0004ACEC31AA__INCLUDED_)
#define AFX_COOKIESTATISTICS_H__838C6D03_CFEF_11D2_B98E_0004ACEC31AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CookieStatistics.h : header file
//

#include <afxtempl.h>
/////////////////////////////////////////////////////////////////////////////
// CCookieStatistics dialog

class CCookieLogStat : public CObject
{
public:

    CCookieLogStat()
    {
        m_JobType.Empty();
        m_JobCount = 0;
    };

    CString m_JobType;
    UINT m_JobCount;
    COleDateTimeSpan m_JobTime;
};

typedef CTypedPtrArray<CObArray, CCookieLogStat*> CCookieLogStatList;

class CCookieLog : public CObject
{
public:

    CCookieLog()
    {
        m_MachineName.Empty();
        m_JobType.Empty();
        m_CookieID.Empty();
        m_JobSequence.Empty();
        m_JobStartTime.Empty();
        m_JobEndTime.Empty();
        m_JobStatus.Empty();
    };
    CString m_MachineName;
    CString m_JobType;
    CString m_CookieID;
    CString m_JobSequence;
    CString m_JobStartTime;
    CString m_JobEndTime;
    CString m_JobStatus;
};

typedef CTypedPtrArray<CObArray, CCookieLog*> CCookieLogList;

class CCookieStatistics : public CDialog
{
// Construction
public:
	CString m_LogFileName;
	CCookieStatistics(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCookieStatistics();

    CCookieLogList m_CookieLogList;
    CCookieLogStatList m_CookieLogStatList;

// Dialog Data
	//{{AFX_DATA(CCookieStatistics)
	enum { IDD = IDD_DIALOG_COOKIE_STATISTICS };
	CEdit	m_CookieIDCtrl;
	CString	m_CookieID;
	CString	m_CookieStatistics;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCookieStatistics)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCookieStatistics)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void LogParseString(char *logBuf, CCookieLog *pCookieLog);
	void LoadCookieLog (FILE *logFile);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COOKIESTATISTICS_H__838C6D03_CFEF_11D2_B98E_0004ACEC31AA__INCLUDED_)
