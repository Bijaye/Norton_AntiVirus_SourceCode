/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/GENPAGE.H_v   1.4   03 Apr 1998 13:25:34   MKEATIN  $
/////////////////////////////////////////////////////////////////////////////
//
//
// GenPage.h : header file
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
// $Log:   S:/N32OPTS/VCS/GENPAGE.H_v  $
// 
//    Rev 1.4   03 Apr 1998 13:25:34   MKEATIN
// Removed File Innoculation, Immediate Notify, and Sensor.
// 
//    Rev 1.3   10 Jun 1997 15:20:56   DBUCHES
// Removed quarantine options.
// 
//    Rev 1.2   02 Jun 1997 14:20:34   DBUCHES
// Added extra validation for text entry fields.
// 
//    Rev 1.1   27 May 1997 20:56:08   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.0   14 May 1997 15:02:32   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_GENPAGE_H__2BCEBDF8_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_)
#define AFX_GENPAGE_H__2BCEBDF8_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "helpdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CGeneralPage dialog

class CGeneralPage : public CHelperPage
{
	DECLARE_DYNCREATE(CGeneralPage)

// Construction
public:
	BOOL SetData(LPNAVOPTIONS pOptions);
	BOOL GetData(LPNAVOPTIONS pOptions);
	CGeneralPage();
	~CGeneralPage();

// Dialog Data
	//{{AFX_DATA(CGeneralPage)
	enum { IDD = IDD_GENERAL_PAGE };
	BOOL	m_bBackup;
	CString	m_sExtension;
	BOOL	m_bHimem;
	BOOL	m_bUseSystemInoc;
	CString	m_sQuarentineDir;
	//}}AFX_DATA
    static DWORD m_dwWhatsThisMap[];
    static DWORD m_dwHowToMap[];


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGeneralPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGeneralPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBackupCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    BOOL IsQuarantineTextValid();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENPAGE_H__2BCEBDF8_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_)
