/////////////////////////////////////////////////////////////////////
// ATMachPg.h : header file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ATMACHPG.H_v   1.1   25 Apr 1997 11:04:44   JBRENNA  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ATMACHPG.H_v  $
// 
//    Rev 1.1   25 Apr 1997 11:04:44   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   15 Apr 1997 20:08:50   sedward
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////


#ifndef ALERT_MACHINE_PROPERTY_PAGE_HEADER
#define ALERT_MACHINE_PROPERTY_PAGE_HEADER


#include "ATListPg.h"


/////////////////////////////////////////////////////////////////////////////
//
// CAlertMachinePropertyPage
//
/////////////////////////////////////////////////////////////////////////////

class CAlertMachinePropertyPage : public CAlertListPropertyPage
{
    DECLARE_DYNCREATE(CAlertMachinePropertyPage)

// Construction
public:
    CAlertMachinePropertyPage();
    CAlertMachinePropertyPage(UINT  dlgID);
    ~CAlertMachinePropertyPage();

// Dialog Data
    //{{AFX_DATA(CAlertMachinePropertyPage)
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CAlertMachinePropertyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
    //{{AFX_MSG(CAlertMachinePropertyPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Data members
public:

// Member functions
public:
    virtual BOOL    HandleTargetEdit(CAlertTarget*  pAlertTarget, int  mode);
};

#endif  // ALERT_MACHINE_PROPERTY_PAGE_HEADER
