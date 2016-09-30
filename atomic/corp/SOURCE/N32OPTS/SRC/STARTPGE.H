/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/STARTPGE.H_v   1.5   19 Feb 1998 11:57:18   mdunn  $
/////////////////////////////////////////////////////////////////////////////
//
//
// Startpge.h : header file
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
// $Log:   S:/N32OPTS/VCS/STARTPGE.H_v  $
// 
//    Rev 1.5   19 Feb 1998 11:57:18   mdunn
// Ported NEC changes from ROSNEC.
// 
//    Rev 1.4   18 Nov 1997 20:36:26   jtaylor
// Removed most recent version.
// 
//    Rev 1.2   04 Jun 1997 22:15:08   DBUCHES
// Removed TSR checkbox.
// 
//    Rev 1.1   27 May 1997 20:57:34   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.0   14 May 1997 15:09:30   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_STARTPGE_H__2BCEBDF4_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_)
#define AFX_STARTPGE_H__2BCEBDF4_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "helpdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CStartupPage dialog

class CStartupPage : public CHelperPage
{
	DECLARE_DYNCREATE(CStartupPage)

// Construction
public:
	BOOL GetData(LPNAVOPTIONS pOptions);
	BOOL SetData(LPNAVOPTIONS pOptions);
	CStartupPage();
	~CStartupPage();

// Dialog Data
	//{{AFX_DATA(CStartupPage)
	enum { IDD = IDD_STARTUP_PAGE };
	BOOL	m_bBootRecord;
	int		m_iBypassKey;
	BOOL	m_bMBR;
	BOOL	m_bMemory;
	BOOL	m_bSysFiles;
	//}}AFX_DATA
    static DWORD m_dwWhatsThisMap[];
    static DWORD m_dwHowToMap[];


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStartupPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CStartupPage)
		virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STARTPGE_H__2BCEBDF4_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_)
