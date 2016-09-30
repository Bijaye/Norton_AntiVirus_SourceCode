/////////////////////////////////////////////////////////////////////
// ATMailPg.h : header file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ATMAILPG.H_v   1.1   25 Apr 1997 11:04:46   JBRENNA  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ATMAILPG.H_v  $
// 
//    Rev 1.1   25 Apr 1997 11:04:46   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   15 Apr 1997 20:08:52   sedward
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////


#ifndef ALERT_EMAIL_PROPERTY_PAGE_HEADER
#define ALERT_EMAIL_PROPERTY_PAGE_HEADER


#include "ATListPg.h"


/////////////////////////////////////////////////////////////////////////////
//
// CAlertEmailPropertyPage
//
/////////////////////////////////////////////////////////////////////////////

class CAlertEmailPropertyPage : public CAlertListPropertyPage
{
    DECLARE_DYNCREATE(CAlertEmailPropertyPage)

// Construction
public:
    CAlertEmailPropertyPage();
    CAlertEmailPropertyPage(UINT  dlgID);
    ~CAlertEmailPropertyPage();

// Dialog Data
    //{{AFX_DATA(CAlertEmailPropertyPage)
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CAlertEmailPropertyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
    //{{AFX_MSG(CAlertEmailPropertyPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Data members
public:

// Member functions
public:
    virtual BOOL    HandleTargetEdit(CAlertTarget*  pAlertTarget, int  mode);
};

#endif  // ALERT_EMAIL_PROPERTY_PAGE_HEADER