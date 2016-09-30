/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/CINCLUDE.H_v   1.2   20 Jun 1997 12:01:00   DBUCHES  $
/////////////////////////////////////////////////////////////////////////////
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
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/N32OPTS/VCS/CINCLUDE.H_v  $
// 
//    Rev 1.2   20 Jun 1997 12:01:00   DBUCHES
// Added Help support.
// 
//    Rev 1.1   27 May 1997 20:55:32   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.0   14 May 1997 15:01:24   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#ifndef _CINCLUDE_H_
#define _CINCLUDE_H_



#include "extlist.h"
#include "helpdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CIncludeDlg dialog

class CIncludeDlg : public CHelperDialog
{
// Construction
public:
	BOOL m_bScanNoExtFile;
	CString m_sDefaultExtensions;
	CString m_sExtensions;
    BOOL m_bOptionsChanged;
	CIncludeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CIncludeDlg)
	enum { IDD = IDD_EXTENSION_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
    static DWORD m_dwWhatsThisMap[];
    static DWORD m_dwHowToMap[];


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIncludeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIncludeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRemoveButton();
	afx_msg void OnDefaultButton();
	afx_msg void OnNewButton();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void PopulateList();
    CExtensionList ext;
};



/////////////////////////////////////////////////////////////////////////////
// CNewExtDlg dialog

class CNewExtDlg : public CHelperDialog
{
// Construction
public:
	CExtensionList * pExt;
	CNewExtDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewExtDlg)
	enum { IDD = IDD_ADD_EXT_DLG };
	CString	m_sExtText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewExtDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewExtDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



#endif
