/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/SHEET.H_v   1.4   10 Jun 1997 20:19:26   DBUCHES  $
/////////////////////////////////////////////////////////////////////////////
//
//
// Sheet.h : header file
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
// $Log:   S:/N32OPTS/VCS/SHEET.H_v  $
// 
//    Rev 1.4   10 Jun 1997 20:19:26   DBUCHES
// Added changes for alert password.
// 
//    Rev 1.3   27 May 1997 22:20:16   DBUCHES
// Added SaveNTAlertOptions.
// 
//    Rev 1.2   27 May 1997 20:57:22   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
// 
//    Rev 1.1   18 May 1997 23:25:42   DBUCHES
// Made a global CIncludeDlg member.  This was needed because the 
// file inclusion extension list may be changed from more than one
// property page.
// 
//    Rev 1.0   14 May 1997 15:09:30   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_SHEET_H__0235E9CB_C1A7_11D0_8AD5_0060977341AC__INCLUDED_)
#define AFX_SHEET_H__0235E9CB_C1A7_11D0_8AD5_0060977341AC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "navsheet.h"
#include "cinclude.h"				// for File extensions dialog

#include "platform.h"

#pragma pack(push, 1)
#include "options.h"
#pragma pack(pop)

#define WM_PASSWORD_CHECK	(WM_USER + 5)


/////////////////////////////////////////////////////////////////////////////
// CNavOptionsSheet

class CNavOptionsSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CNavOptionsSheet)

// Construction
public:
	CNavOptionsSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CNavOptionsSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNavOptionsSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL m_bOptionsChanged;
	BOOL m_bPasswordValid;
    DWORD m_dwPassword;
	BOOL PasswordCheck(int iFeature);
	void GatherData();
	int m_iNumPages;
    int m_iPageIDs[N32OPTS_MAXPAGES];
    int InsertPages(int iNumPages, int* iPageIDs);
    BOOL GetPath(TCHAR * szTemp, int size);
	virtual ~CNavOptionsSheet();

    NAVOPTIONS navOpts;

	// Global include files dialog.
	CIncludeDlg	m_IncludeDlg;


	// Generated message map functions
protected:
	//{{AFX_MSG(CNavOptionsSheet)
	virtual BOOL OnInitDialog();
    afx_msg LONG OnPasswordCheckMessage( WPARAM wParam, LPARAM lParam );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    // Array of pointers to property pages.
    CPropertyPage** m_pPages;

    
private:
	void Init();
	CPropertyPage * AllocPage(int iPage);
    void DeletePages();
    CPropertyPage * CreateServerAlertPage();
    void SaveNTServerAlerts();
    BOOL GetExtensions(TCHAR *szExt);
    BOOL GetDefaultExtensions(TCHAR *szDefaultExt);
    BOOL WriteExtensions(TCHAR *szExt);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHEET_H__0235E9CB_C1A7_11D0_8AD5_0060977341AC__INCLUDED_)
