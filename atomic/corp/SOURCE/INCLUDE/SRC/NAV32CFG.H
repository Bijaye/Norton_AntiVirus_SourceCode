/* Copyright 1997 Symantec, Peter Norton Computing Group                */
//////////////////////////////////////////////////////////////////////////
//
// $Header:   S:/INCLUDE/VCS/nav32cfg.h_v   1.0   10 Apr 1997 17:01:04   DBUCHES  $
//
//
//
//////////////////////////////////////////////////////////////////////////
// Created by: DBUCHES   04-09-97
//////////////////////////////////////////////////////////////////////////
/* $Log:   S:/INCLUDE/VCS/nav32cfg.h_v  $ */
// 
//    Rev 1.0   10 Apr 1997 17:01:04   DBUCHES
// Initial revision.


// PropShet.h : header file
//
// This class defines custom modal property sheet 
// CNAVOptionsSheet.
 
#ifndef __NAV32CFG_H__
#define __NAV32CFG_H__

#ifdef BUILDING_EXTDLL
#define DECLSPEC_PROPPAGE _declspec( dllexport )
#else
#define DECLSPEC_PROPPAGE _declspec( dllimport )
#endif

                   
/////////////////////////////////////////////////////////////////////////////
// CProtectionPage dialog

class CProtectionPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CProtectionPage)

// Construction
public:
	CProtectionPage();
	~CProtectionPage();


// Dialog Data
	//{{AFX_DATA(CProtectionPage)
	//enum { IDD = IDD_PROPPAGE1 };
    int m_iResponse;
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProtectionPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CProtectionPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CSchedulePage dialog

class CSchedulePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSchedulePage)

// Construction
public:
	CSchedulePage();
    CSchedulePage(int iD);
	~CSchedulePage();

// Dialog Data
	//{{AFX_DATA(CSchedulePage)
	//enum { IDD = IDD_PROPPAGE2 };
	int		m_iFrequency;
	int		m_iDayNumber;
	int		m_iDay;
	int		m_iStartTime;
	BOOL	m_bScheduleEnable;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSchedulePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSchedulePage)
	afx_msg void OnSelendokRequencyCombo();
	virtual BOOL OnInitDialog();
	afx_msg void OnEnableScheduleChkbox();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
    virtual void DisableScheduleControls(BOOL bDisable = TRUE);

private:
	
	void HideMonthControls(BOOL bHide = TRUE);
	void HideDays(BOOL bHide = TRUE);
};


/////////////////////////////////////////////////////////////////////////////
// CLiveUpdatePage dialog

class CLiveUpdatePage : public CSchedulePage
{
	DECLARE_DYNCREATE(CLiveUpdatePage)

// Construction
public:
	CLiveUpdatePage();
	~CLiveUpdatePage();

// Dialog Data
	//{{AFX_DATA(CLiveUpdatePage)
	//enum { IDD = IDD_PROPPAGE3 };
    int m_iConnectType;
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLiveUpdatePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLiveUpdatePage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
    virtual void DisableScheduleControls(BOOL bDisable = TRUE);

};


/////////////////////////////////////////////////////////////////////////////
// CNAVOptionsSheet

class DECLSPEC_PROPPAGE CNAVOptionsSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CNAVOptionsSheet)

// Construction
public:
	CNAVOptionsSheet(CWnd* pWndParent = NULL);

// Attributes
public:
	CProtectionPage m_Page1;
	CSchedulePage m_Page2;
	CLiveUpdatePage m_Page3;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNAVOptionsSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNAVOptionsSheet();

// Generated message map functions
protected:
	//{{AFX_MSG(CNAVOptionsSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
    BOOL OnInitDialog();

};

/////////////////////////////////////////////////////////////////////////////







#endif	// __PROPSHET_H__
