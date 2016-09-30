// TWIntroPage.h : header file
//

#if !defined(AFX_TWINTROPAGE_H__8AD9EAA5_5854_11D2_96F6_00C04FAC114C__INCLUDED_)
#define AFX_TWINTROPAGE_H__8AD9EAA5_5854_11D2_96F6_00C04FAC114C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "picture.h"
#include "wizardpg.h"

/////////////////////////////////////////////////////////////////////////////
// CTWIntroPage dialog -- The first page for the scan task wizard.

class CTWIntroPage : public CBaseWizPage
{
	DECLARE_DYNCREATE(CTWIntroPage)

// Construction
public:
	CTWIntroPage();
	~CTWIntroPage();

// Dialog Data
	//{{AFX_DATA(CTWIntroPage)
	//enum { IDD = IDD_TW_INTRO };
	CPicture	m_Picture;
	//}}AFX_DATA

    BOOL m_bEditingTask;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTWIntroPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTWIntroPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TWINTROPAGE_H__8AD9EAA5_5854_11D2_96F6_00C04FAC114C__INCLUDED_)
