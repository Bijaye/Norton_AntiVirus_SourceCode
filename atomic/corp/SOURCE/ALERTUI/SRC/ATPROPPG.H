/////////////////////////////////////////////////////////////////////
// ATPropPg.h : header file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ATPROPPG.H_v   1.1   25 Apr 1997 11:04:50   JBRENNA  $
//
// Description:
//      This class is used primarily to interface with the alert targets class
//      (ie, "CAlertOptions").  Its main focus is the non-UI tasks needed to
//      implement the alert target property pages for NAV.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ATPROPPG.H_v  $
// 
//    Rev 1.1   25 Apr 1997 11:04:50   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   15 Apr 1997 20:08:56   sedward
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////


#ifndef ALERT_TARGET_PROPERTY_PAGE_HEADER
#define ALERT_TARGET_PROPERTY_PAGE_HEADER


#include "at_opts.h"                // For CAlertOptions class
#include "helpdlg.h"                // base class for default help support


/////////////////////////////////////////////////////////////////////////////
//
// CAlertPropertyPage
//
/////////////////////////////////////////////////////////////////////////////

class CAlertPropertyPage : public CHelpPropertyPage
{
    DECLARE_DYNCREATE(CAlertPropertyPage)

// Construction
public:
    CAlertPropertyPage();
    CAlertPropertyPage(UINT  dlgID);
    virtual ~CAlertPropertyPage();

// Dialog Data
    //{{AFX_DATA(CAlertPropertyPage)
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CAlertPropertyPage)
	public:
    virtual void OnOK(void);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
    //{{AFX_MSG(CAlertPropertyPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Functions
public:
    virtual BOOL            CopyAlertTargetInfo(CAlertTarget*  source
                                                        , CAlertTarget*  dest);
    CAlertOptions*          CreateTempAlertTargetsList(void);
    virtual CAlertTarget*   GetNewAlertTarget(void);
    virtual BOOL            TargetIsActive(CAlertTarget*  pAlertTarget);
    void                    UpdateMasterAlertTargetsList(void);



// Data members
public:
    CAlertOptions*  m_targetsListPtr;
    int             m_alertType;

protected:

};

#endif  // ALERT_TARGET_PROPERTY_PAGE_HEADER