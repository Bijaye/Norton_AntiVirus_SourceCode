/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/EXCLPAGE.H_v   1.8   03 Apr 1998 13:25:38   MKEATIN  $
/////////////////////////////////////////////////////////////////////////////
//
//
// ExclPage.h : header file
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
// $Log:   S:/N32OPTS/VCS/EXCLPAGE.H_v  $
// 
//    Rev 1.8   03 Apr 1998 13:25:38   MKEATIN
// Removed File Innoculation, Immediate Notify, and Sensor.
// 
//    Rev 1.7   19 Feb 1998 11:57:16   mdunn
// Ported NEC changes from ROSNEC.
// 
//    Rev 1.1   05 Nov 1997 13:39:26   mdunn
// POST-SHIP: Fixed CExcludeEdit so that the OK button is greyed out if
// the edit box is empty (and on 95, if all the exclusion type checkboxes
// are unchecked).  This prevents the user from entering an empty exclusion
// which NAV treats the same as *.*
// This is currently in STS as incident #128184.
// 
//    Rev 1.0   12 Sep 1997 17:44:54   RFULLER
// Initial revision
// 
//    Rev 1.6   16 Jul 1997 12:56:42   DBUCHES
// Added code to validate exclusion text.
// 
//    Rev 1.5   02 Jul 1997 14:06:26   DBUCHES
// Added handler for double click on listbox.
// 
//    Rev 1.4   25 Jun 1997 15:16:06   DBUCHES
// Converted buttons to owner draw buttons.
// 
//    Rev 1.3   24 Jun 1997 15:59:04   DBUCHES
// Fixed text display.
// 
//    Rev 1.2   27 May 1997 20:55:54   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.1   16 May 1997 15:27:42   DBUCHES
// Finished hooking up Exclusions.  1st pass.
//
//    Rev 1.0   14 May 1997 15:02:28   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_EXCLPAGE_H__2BCEBDF7_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_)
#define AFX_EXCLPAGE_H__2BCEBDF7_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "helpdlg.h"
#include "iconbtn.h"
/////////////////////////////////////////////////////////////////////////////
// CDedit  - Dead Edit control used for displaying exclusion flags.

class CDedit : public CEdit
{
// Construction
public:
	CDedit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDedit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDedit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDedit)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CExcludePage dialog

class CExcludePage : public CHelperPage
{
	DECLARE_DYNCREATE(CExcludePage)

// Construction
public:
	EXCLUDE m_Exclude;
	BOOL GetData(LPNAVOPTIONS pOptions);
	BOOL SetData(LPNAVOPTIONS pOptions);
	CExcludePage();
	~CExcludePage();

// Dialog Data
	//{{AFX_DATA(CExcludePage)
	enum { IDD = IDD_EXCLUDE_PAGE };
	CDedit	m_Edit;
	CListBox	m_ListBox;
	//}}AFX_DATA
    static DWORD m_dwWhatsThisMap[];
    static DWORD m_dwHowToMap[];



// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CExcludePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CExcludePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeExcludeList();
	afx_msg void OnEditButton();
	afx_msg void OnNewButton();
	afx_msg void OnRemoveButton();
	afx_msg void OnDblclkExcludeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    int m_iMaxDisplayChars;
	void SetScrollWidth();
	BOOL GetSelectedExcludeItem(EXCLUDEITEM & ExcludeItem, UINT *pIndex, UINT *pListBoxIndex);
	BOOL EnableButtons();
	void PopulateList();
};



/////////////////////////////////////////////////////////////////////////////
// CExcludeEdit dialog

class CExcludeEdit : public CHelperDialog
{
// Construction
public:
	BOOL m_bNew;
	CExcludeEdit(CWnd* pParent = NULL);   // standard constructor
    EXCLUDEITEM m_item;
// Dialog Data
	//{{AFX_DATA(CExcludeEdit)
	enum { IDD = IDD_EXCLUDE_EDIT };
	BOOL	m_DOSReadOnly;
	BOOL	m_bKnown;
	BOOL	m_bLowLevel;
	BOOL	m_bSubDir;
	CString	m_sFileName;
	BOOL	m_bWriteFloppy;
	BOOL	m_bWriteHD;
	BOOL	m_bWriteProgram;
    CIconButton m_BrowseBtn;
	//}}AFX_DATA
    static DWORD m_dwWhatsThisMap[];
    static DWORD m_dwHowToMap[];


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExcludeEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExcludeEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnFileopenButton();
	virtual void OnOK();
    afx_msg void OnChangeExclusionText();
    afx_msg void OnClickedExclusionCheckbox();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL IsExcludeNameValid();
    void UpdateOKButtonStatus();
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXCLPAGE_H__2BCEBDF7_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_)
