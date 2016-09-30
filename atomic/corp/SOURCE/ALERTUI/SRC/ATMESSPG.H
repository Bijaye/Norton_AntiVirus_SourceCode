/////////////////////////////////////////////////////////////////////
// ATMessPg.h : header file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ATMESSPG.H_v   1.1   25 Apr 1997 11:04:48   JBRENNA  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ATMESSPG.H_v  $
// 
//    Rev 1.1   25 Apr 1997 11:04:48   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   15 Apr 1997 20:08:54   sedward
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////


#ifndef ALERT_NET_MESSENGER_PROPERTY_PAGE_HEADER
#define ALERT_NET_MESSENGER_PROPERTY_PAGE_HEADER


#include "ATMachPg.h"


/////////////////////////////////////////////////////////////////////////////
//
// CAlertNetMessengerPropertyPage
//
/////////////////////////////////////////////////////////////////////////////

class CAlertNetMessengerPropertyPage : public CAlertMachinePropertyPage
{
    DECLARE_DYNCREATE(CAlertNetMessengerPropertyPage)

// Construction
public:
    CAlertNetMessengerPropertyPage();
    CAlertNetMessengerPropertyPage(UINT  dlgID);
    ~CAlertNetMessengerPropertyPage();

// Dialog Data
    //{{AFX_DATA(CAlertNetMessengerPropertyPage)
    enum { IDD = IDD_NETMESSAGE_PROP_PAGE };
	CListCtrl	m_listCtrl;
	CButton	m_removeButton;
	CButton	m_modifyButton;
	CButton	m_addButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CAlertNetMessengerPropertyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
    //{{AFX_MSG(CAlertNetMessengerPropertyPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickNetmessageTargetList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkNetmessageTargetList(NMHDR* pNMHDR, LRESULT* pResult);
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

#endif  // ALERT_NET_MESSENGER_PROPERTY_PAGE_HEADER