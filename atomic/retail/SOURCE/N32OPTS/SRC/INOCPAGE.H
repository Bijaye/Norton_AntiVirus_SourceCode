/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/INOCPAGE.H_v   1.6   28 May 1998 14:39:12   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
//
// InocPage.h : header file
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
// $Log:   S:/N32OPTS/VCS/INOCPAGE.H_v  $
// 
//    Rev 1.6   28 May 1998 14:39:12   DBuches
// Fixed misc control enabling bugs.
// 
//    Rev 1.5   10 Apr 1998 00:27:50   MKEATIN
// Made inoculation related changes.
// 
//    Rev 1.4   10 Jun 1997 15:20:32   DBUCHES
// Removed quarantine options.
// 
//    Rev 1.3   02 Jun 1997 14:20:34   DBUCHES
// Added extra validation for text entry fields.
// 
//    Rev 1.2   27 May 1997 20:56:30   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.1   23 May 1997 15:57:30   DBUCHES
// Added setting of extra Quarantine bits.
//
//    Rev 1.0   14 May 1997 15:02:34   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_INOCPAGE_H__2BCEBDF9_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_)
#define AFX_INOCPAGE_H__2BCEBDF9_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "helpdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CInocPage dialog

class CInocPage : public CHelperPage
{
	DECLARE_DYNCREATE(CInocPage)

// Construction
public:
	BOOL GetData(LPNAVOPTIONS pOptions);
	BOOL SetData(LPNAVOPTIONS pOptions);
	CInocPage();
	~CInocPage();

// Dialog Data
	//{{AFX_DATA(CInocPage)
	enum { IDD = IDD_INOC_PAGE };
	BOOL	m_bBootRecord;
	int		m_iChanged;
	CString	m_sFilePath;
	BOOL	m_bInnoc;
	BOOL	m_bPrograms;
	BOOL	m_bRepair;
	BOOL	m_bStop;
	BOOL	m_bContinue;
	//}}AFX_DATA
    static DWORD m_dwWhatsThisMap[];
    static DWORD m_dwHowToMap[];



// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInocPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CInocPage)
    virtual BOOL OnKillActive();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeChangedCombo();
	afx_msg void OnBootrecordCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    BOOL IsInocDirValid();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INOCPAGE_H__2BCEBDF9_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_)
