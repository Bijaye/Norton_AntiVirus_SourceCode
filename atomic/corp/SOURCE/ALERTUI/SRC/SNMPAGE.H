/////////////////////////////////////////////////////////////////////
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/SNMPage.h_v   1.0   04 Jun 1997 21:18:54   DBUCHES  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/SNMPage.h_v  $
// 
//    Rev 1.0   04 Jun 1997 21:18:54   DBUCHES
// Initial revision.
// 
#if !defined(AFX_SNMPAGE_H__6E1D87EE_DC58_11D0_8AE6_0060977341AC__INCLUDED_)
#define AFX_SNMPAGE_H__6E1D87EE_DC58_11D0_8AE6_0060977341AC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SNMPage.h : header file
//
#include "atmailpg.h"

/////////////////////////////////////////////////////////////////////////////
// CAlertSNMPPropertyPage dialog

class CAlertSNMPPropertyPage : public CAlertEmailPropertyPage
{
	DECLARE_DYNCREATE(CAlertSNMPPropertyPage)

// Construction
public:
	CAlertSNMPPropertyPage();
	~CAlertSNMPPropertyPage();

    BOOL TargetIsActive(CAlertTarget*  pAlertTarget);
    BOOL CopyAlertTargetInfo(CAlertTarget*  source, CAlertTarget*  dest);
    CAlertTarget *CAlertSNMPPropertyPage::GetNewAlertTarget(void);
    BOOL HandleTargetEdit(CAlertTarget*  pAlertTarget, int  mode);
    BOOL CAlertSNMPPropertyPage::GetDisplayString(CAlertTarget*  pAlertTarget
                                                        , CString&  displayStr
                                                        , int  index);
    
// Dialog Data
	//{{AFX_DATA(CAlertSNMPPropertyPage)
	enum { IDD = IDD_SNMP_PROP_PAGE };
    CListCtrl	m_listCtrl;
	CButton	m_removeButton;
	CButton	m_modifyButton;
	CButton	m_addButton;

		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAlertSNMPPropertyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAlertSNMPPropertyPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
    BOOL InitializeData();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SNMPAGE_H__6E1D87EE_DC58_11D0_8AE6_0060977341AC__INCLUDED_)
