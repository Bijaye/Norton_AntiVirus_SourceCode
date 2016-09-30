/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/appage.h_v   1.5   15 Apr 1998 17:36:56   MKEATIN  $
/////////////////////////////////////////////////////////////////////////////
//
// APPage.h : header file
//
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
// $Log:   S:/N32OPTS/VCS/appage.h_v  $
// 
//    Rev 1.5   15 Apr 1998 17:36:56   MKEATIN
// Added heuristics dialog to the AP pannel.
// 
//    Rev 1.4   19 Feb 1998 11:57:14   mdunn
// Ported NEC changes from ROSNEC.
// 
//    Rev 1.3   27 May 1997 20:55:24   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.2   21 May 1997 14:36:36   DBUCHES
// Added custom responst dialog.
//
//    Rev 1.1   18 May 1997 23:24:32   DBUCHES
// Now, use global copy of CIncludeDlg.
//
//    Rev 1.0   14 May 1997 15:01:22   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_APPAGE_H__2BCEBDF0_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_)
#define AFX_APPAGE_H__2BCEBDF0_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "cinclude.h"
#include "helpdlg.h"
#include "custresp.h"
#include "hounddlg.h"


/////////////////////////////////////////////////////////////////////////////
// CAdvApDlg dialog

class CAdvApDlg : public CHelperDialog
{
// Construction
public:
	CAdvApDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAdvApDlg)
	enum { IDD = IDD_ADVANCED_AUTOPROTECT_DIALOG };
	int		m_iBootRecord;
	int		m_iDos;
	int		m_iFloppyBR;
	int		m_iLowLevel;
	int		m_iProgram;
	BOOL	m_bFloppyBoth;
	BOOL	m_bFloppyReboot;
	BOOL	m_bFloppyAccess;
	//}}AFX_DATA
    static DWORD m_dwWhatsThisMap[];
    static DWORD m_dwHowToMap[];


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdvApDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdvApDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnFloppyRebootCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAdvApDlgNec dialog

class CAdvApDlgNec : public CHelperDialog
{
// Construction
public:
	CAdvApDlgNec(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAdvApDlgNec)
	enum { IDD = IDD_ADVANCED_AUTOPROTECT_DIALOG_NEC };
		// NOTE: the ClassWizard will add data members here
	int		m_iDos;
	int		m_iProgram;
	BOOL	m_bFloppyReboot;
	BOOL	m_bFloppyAccess;
//}}AFX_DATA
    static DWORD m_dwWhatsThisMap[];
    static DWORD m_dwHowToMap[];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdvApDlgNec)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdvApDlgNec)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAutoProtectPage dialog

class CAutoProtectPage : public CHelperPage
{
	DECLARE_DYNCREATE(CAutoProtectPage)

// Construction
public:
	BOOL GetData(LPNAVOPTIONS pOptions);
	BOOL SetData(LPNAVOPTIONS pOptions);
	CAutoProtectPage();
	~CAutoProtectPage();

// Dialog Data
	//{{AFX_DATA(CAutoProtectPage)
	enum { IDD = IDD_AUTOPORTECT_PAGE };
	int		m_AllFiles;
	BOOL	m_bContinue;
	BOOL	m_bCreated;
	BOOL	m_bDelete;
	BOOL	m_bDisable;
	BOOL	m_bExclude;
	BOOL	m_bLoad;
	BOOL	m_bOpened;
	int		m_iPrompt;
	BOOL	m_bRepair;
	BOOL	m_bRun;
	BOOL	m_bShowIcon;
	BOOL	m_bStop;
    BOOL    m_bFloppyAccess;
    BOOL    m_bFloppyReboot;
	BOOL	m_bQuarantine;
	//}}AFX_DATA
    static DWORD m_dwWhatsThisMap[];
    static DWORD m_dwHowToMap[];

    CAdvApDlg    m_AdvancedDlg;
    CAdvApDlgNec m_AdvancedDlgNec;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAutoProtectPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAutoProtectPage)
	afx_msg void OnAllFilesRadio();
	afx_msg void OnHeuristicButton();
	afx_msg void OnAdvancedButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelectButton();
	afx_msg void OnSelchangePromptCombo();
	afx_msg void OnCustomButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
    CCustomResponseDlg m_CustDlg;
    CBloodhoundDlg     m_BloodhoundDlg;
};



//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPAGE_H__2BCEBDF0_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_)
