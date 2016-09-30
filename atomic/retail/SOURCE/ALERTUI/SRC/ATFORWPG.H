/////////////////////////////////////////////////////////////////////
// ATForwPg.h : header file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ATFORWPG.H_v   1.1   25 Apr 1997 11:04:54   JBRENNA  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ATFORWPG.H_v  $
// 
//    Rev 1.1   25 Apr 1997 11:04:54   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   15 Apr 1997 20:09:00   sedward
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////


#ifndef ALERT_FORWARD_PROPERTY_PAGE_HEADER
#define ALERT_FORWARD_PROPERTY_PAGE_HEADER


#include "ATMachPg.h"


/////////////////////////////////////////////////////////////////////////////
//
// CAlertForwardPropertyPage
//
/////////////////////////////////////////////////////////////////////////////

class CAlertForwardPropertyPage : public CAlertMachinePropertyPage
{
    DECLARE_DYNCREATE(CAlertForwardPropertyPage)

// Construction
public:
    CAlertForwardPropertyPage(void);
    CAlertForwardPropertyPage(UINT  dlgID);
    ~CAlertForwardPropertyPage();

// Dialog Data
    //{{AFX_DATA(CAlertForwardPropertyPage)
	enum { IDD = IDD_FORWARD_PROP_PAGE };
	CListCtrl	m_listCtrl;
	CButton	m_removeButton;
	CButton	m_modifyButton;
	CButton	m_addButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CAlertForwardPropertyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
    //{{AFX_MSG(CAlertForwardPropertyPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickForwardTargetList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkForwardTargetList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Functions
protected:
    BOOL                    InitializeData(void);

public:
    virtual BOOL            CopyAlertTargetInfo(CAlertTarget*  source
                                                        , CAlertTarget*  dest);
    virtual BOOL            GetDisplayString(CAlertTarget*  pAlertTarget
                                            , CString&  displayStr, int  index);
    virtual CAlertTarget*   GetNewAlertTarget(void);
    virtual BOOL            TargetIsActive(CAlertTarget*  pAlertTarget);

// Data members
public:
};

#endif  // ALERT_FORWARD_PROPERTY_PAGE_HEADER