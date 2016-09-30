/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/SCANPAGE.H_v   1.5   03 Apr 1998 13:25:40   MKEATIN  $
/////////////////////////////////////////////////////////////////////////////
//
//
// ScanPage.h : header file
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
// $Log:   S:/N32OPTS/VCS/SCANPAGE.H_v  $
// 
//    Rev 1.5   03 Apr 1998 13:25:40   MKEATIN
// Removed File Innoculation, Immediate Notify, and Sensor.
// 
//    Rev 1.4   13 Jun 1997 11:54:58   DBUCHES
// Added Bloodhound dialog.
// 
//    Rev 1.3   27 May 1997 20:57:12   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.2   21 May 1997 14:36:38   DBUCHES
// Added custom responst dialog.
//
//    Rev 1.1   18 May 1997 23:24:34   DBUCHES
// Now, use global copy of CIncludeDlg.
//
//    Rev 1.0   14 May 1997 15:09:28   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_SCANPAGE_H__2BCEBDDF_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_)
#define AFX_SCANPAGE_H__2BCEBDDF_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "helpdlg.h"
#include "cinclude.h"
#include "custresp.h"
#include "hounddlg.h"

/////////////////////////////////////////////////////////////////////////////
// CScanAdvDlg dialog

class CScanAdvDlg : public CHelperDialog
{
// Construction
public:
	CScanAdvDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScanAdvDlg)
	enum { IDD = IDD_ADVANCED_SCANNER_DIALOG };
	BOOL	m_bShowNetwork;
	BOOL	m_bStartFloppy;
	BOOL	m_bStartHard;
	BOOL	m_bStartNet;
	BOOL	m_bAllowStop;
	//}}AFX_DATA
    static DWORD m_dwWhatsThisMap[];
    static DWORD m_dwHowToMap[];


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScanAdvDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScanAdvDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnNetworkCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

};


/////////////////////////////////////////////////////////////////////////////
// CScannerPage dialog

class CScannerPage : public CHelperPage
{
	DECLARE_DYNCREATE(CScannerPage)

// Construction
public:
	BOOL GetData(LPNAVOPTIONS pOptions);
	BOOL SetData(LPNAVOPTIONS pOptions);
	CScannerPage();
	~CScannerPage();

// Dialog Data
	//{{AFX_DATA(CScannerPage)
	enum { IDD = IDD_SCANNER_PAGE };
	int		m_AllFiles;
	BOOL	m_bBootRecs;
	BOOL	m_bMBRCheck;
	BOOL	m_bMemory;
	BOOL	m_bContinue;
	BOOL	m_bDelete;
	BOOL	m_bExclude;
	int		m_iAction;
	BOOL	m_bRepair;
	BOOL	m_bCompressed;
	BOOL	m_bQuarantine;
	//}}AFX_DATA
    static DWORD m_dwWhatsThisMap[];
    static DWORD m_dwHowToMap[];

    CScanAdvDlg m_AdvDlg;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CScannerPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CScannerPage)
	afx_msg void OnAdvancedButton();
	afx_msg void OnSelectButton();
	afx_msg void OnAllFilesRadio();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangePromptCombo();
	afx_msg void OnCustomButton();
	afx_msg void OnScannerButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
    CCustomResponseDlg  m_CustDlg;
    CBloodhoundDlg     m_BloodhoundDlg;

};



//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCANPAGE_H__2BCEBDDF_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_)
