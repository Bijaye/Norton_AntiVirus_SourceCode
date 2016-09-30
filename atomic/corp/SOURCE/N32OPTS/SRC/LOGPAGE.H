/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/logpage.h_v   1.10   07 Oct 1998 10:54:42   TCASHIN  $
/////////////////////////////////////////////////////////////////////////////
//
//
// LogPage.h : header file
//
//
//
//
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/N32OPTS/VCS/logpage.h_v  $
// 
//    Rev 1.10   07 Oct 1998 10:54:42   TCASHIN
// The default IDD for the class should not be the NT version.
// 
//    Rev 1.9   06 Oct 1998 13:48:58   tcashin
// Added m_bAccessDenied and GetPath().
// 
//    Rev 1.8   23 Apr 1998 15:23:30   MKEATIN
// Re-added m_bInnoc.
// 
//    Rev 1.7   03 Apr 1998 13:25:36   MKEATIN
// Removed File Innoculation, Immediate Notify, and Sensor.
// 
//    Rev 1.6   30 Mar 1998 15:07:50   tcashin
// Added a Quarantine check box to the activity log options panel.
// 
//    Rev 1.5   21 Aug 1997 19:50:10   DBUCHES
// Added validation for activity log file.
// 
//    Rev 1.4   25 Jun 1997 15:16:06   DBUCHES
// Converted buttons to owner draw buttons.
// 
//    Rev 1.3   17 Jun 1997 13:13:44   DBUCHES
// Removed quarantine settings.
// 
//    Rev 1.2   16 Jun 1997 15:36:18   DBUCHES
// Fixed problem with data validation.
// 
//    Rev 1.1   27 May 1997 20:56:40   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.0   14 May 1997 15:02:34   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_LOGPAGE_H__2BCEBDF6_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_)
#define AFX_LOGPAGE_H__2BCEBDF6_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "helpdlg.h"
#include "iconbtn.h"

/////////////////////////////////////////////////////////////////////////////
// CLogPage dialog

class CLogPage : public CHelperPage
{
	DECLARE_DYNCREATE(CLogPage)

// Construction
public:
	BOOL GetData(LPNAVOPTIONS pOptions);
	BOOL SetData(LPNAVOPTIONS pOptions);
	CLogPage();
	~CLogPage();
	
// Dialog Data
	//{{AFX_DATA(CLogPage)
	enum { IDD = IDD_ACTIVITY_LOG_PAGE };
	BOOL	m_bComplete;
	CString	m_sFileName;
	BOOL	m_bKnown;
	BOOL	m_bInnoc;
	BOOL	m_bLimit;
	BOOL	m_bList;
	BOOL	m_bVirusLike;
	UINT	m_iSize;
    CIconButton m_BrowseBtn;
	BOOL	m_bQuarantine;
	BOOL	m_bAccessDenied;
	//}}AFX_DATA
    static DWORD m_dwWhatsThisMap[];
    static DWORD m_dwHowToMap[];


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLogPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLogPage)
	virtual BOOL OnInitDialog();
    virtual BOOL OnKillActive();
	afx_msg void OnFileopenButton();
	afx_msg void OnLimitCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL GetPath(TCHAR * szPath, int iSize);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGPAGE_H__2BCEBDF6_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_)
