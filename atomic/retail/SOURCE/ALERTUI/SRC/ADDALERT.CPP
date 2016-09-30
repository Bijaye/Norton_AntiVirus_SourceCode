/////////////////////////////////////////////////////////////////////
// AddAlert.cpp : implementation file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ADDALERT.CPv   1.1   25 Apr 1997 11:03:14   JBRENNA  $
//
// Description:
//
//  This file contains the code for the general "add alert" window.  If the
//  user wishes to add a new alert target without having first specified
//  the type of target (ie., email, pager, NLM, etc.), this window appears.
//  Its purpose is to help the user first choose an alert type so he/she can
//  then proceed to a more specific data entry window.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ADDALERT.CPv  $
// 
//    Rev 1.1   25 Apr 1997 11:03:14   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   13 Mar 1997 15:06:18   RFULLER
// Initial revision
// 
//    Rev 1.5   09 Dec 1996 20:46:16   SEDWARD
// Used TARGET defines from the navalert project.
//
//    Rev 1.4   03 Dec 1996 19:48:24   SEDWARD
// Substantial changes (not frozen yet...).
//
//    Rev 1.3   26 Nov 1996 00:56:34   SEDWARD
// More big changes.
//
//    Rev 1.2   25 Nov 1996 15:46:14   SEDWARD
// No change.
//
//    Rev 1.1   21 Nov 1996 17:07:18   SEDWARD
// Added vlog headers.
//
//    Rev 1.0   21 Nov 1996 16:55:06   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "AlertUI.h"
#include "AddAlert.h"
#include "SMTPPg.h"
#include "AlertOpt.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlertTypesDlg dialog


CAlertTypesDlg::CAlertTypesDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CAlertTypesDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CAlertTypesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    // save a pointer to the parent window's alert type member
    m_alertTypePtr = &((CAlertOptsDlg*)pParent)->m_alertType;
}


void CAlertTypesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAlertTypesDlg)
	DDX_Control(pDX, IDC_ALERT_TARGET_LISTBOX, m_alertTargetsListbox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlertTypesDlg, CDialog)
    //{{AFX_MSG_MAP(CAlertTypesDlg)
	ON_LBN_DBLCLK(IDC_ALERT_TARGET_LISTBOX, OnDblclkAlertTargetListbox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// CAlertTypesDlg member functions
/////////////////////////////////////////////////////////////////////////////


// ==== AlertSelected =====================================================
//
//  This function is called when the user clicks on the "OK" button or
//  double-clicks on an item.  It then sets the alert type member variable
//  to the appropriate alert type.
//
//  Input:  nothing
//  Output: nothing (the alert type member variable is initialized)
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertTypesDlg::AlertSelected(void)
{
    auto    CString         currenItemStr;
    auto    int             currentItemIndex;

    // get the index of the current item
    currentItemIndex = m_alertTargetsListbox.GetCurSel();
    if (LB_ERR == currentItemIndex)
        {
        // let the parent window know that no alert types were selected
        *m_alertTypePtr = TARGET_NONE;
        }
    else
        {
        // each item in the listbox has a defined alert type value associated
        // with it
        *m_alertTypePtr = m_alertTargetsListbox.GetItemData(currentItemIndex);
        }

}  // end of "CAlertTypesDlg::AlertSelected"








/////////////////////////////////////////////////////////////////////////////
// CAlertTypesDlg message handlers (MFC)
/////////////////////////////////////////////////////////////////////////////



// ==== OnInitDialog ======================================================
//
//  This function initializes the listbox with alert type entries.  Each
//  entry has a defined alert type associated with it, so when the user
//  selects an item the alert type can be derived.
//
//  Input:  nothing
//  Output: a value of TRUE
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL CAlertTypesDlg::OnInitDialog(void)
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
    auto    CString         tempStr;
    auto    int             index = 0;


    // initialize the alert targets listbox

    // local machine...
    tempStr.LoadString(IDS_ALB_LOCAL_MACHINE_TARGET);
    m_alertTargetsListbox.AddString(tempStr);
    m_alertTargetsListbox.SetItemData(index, TARGET_LOCAL_MACHINE);
    ++index;

    // net messenger...
    tempStr.LoadString(IDS_ALB_NET_MESSAGE_TARGETS);
    m_alertTargetsListbox.AddString(tempStr);
    m_alertTargetsListbox.SetItemData(index, TARGET_MESSAGE_NETWORK);
    ++index;

    // email...
    tempStr.LoadString(IDS_ALB_EMAIL_TARGETS);
    m_alertTargetsListbox.AddString(tempStr);
    m_alertTargetsListbox.SetItemData(index, TARGET_EMAIL);
    ++index;

    // IFDEF_DISABLE
    #if 0
    // pager...
    tempStr.LoadString(IDS_ALB_PAGER_TARGETS);
    m_alertTargetsListbox.AddString(tempStr);
    m_alertTargetsListbox.SetItemData(index, TARGET_PAGER);
    ++index;
    #endif  // IFDEF_DISABLE

    // NLMs...
    tempStr.LoadString(IDS_ALB_NLM_TARGETS);
    m_alertTargetsListbox.AddString(tempStr);
    m_alertTargetsListbox.SetItemData(index, TARGET_NLM);
    ++index;

    // "forward"...
    tempStr.LoadString(IDS_ALB_FORWARD_TARGETS);
    m_alertTargetsListbox.AddString(tempStr);
    m_alertTargetsListbox.SetItemData(index, TARGET_FORWARD);
    ++index;

    // finally, make the first item automatically selected
    m_alertTargetsListbox.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CAlertTypesDlg::OnInitDialog"



// ==== OnOK ==============================================================
//
//  This function is called when the user selects the "OK" button.  It then
//  calls a function to handle the alert type selection.
//
//  Input:  nothing
//  Output: nothing (the alert type member variable is initialized)
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void CAlertTypesDlg::OnOK(void)
{
	// TODO: Add extra validation here

    AlertSelected();

    // call the parent handler
    CDialog::OnOK();

}  // end of "CAlertTypesDlg::OnOK"



// ==== OnDblclkAlertTargetListbox ========================================
//
//  This function is called when the user double-clicks on an item in the
//  listbox.  It then calls a function to handle the alert type selection.
//
//  Input:  nothing
//  Output: nothing (the alert type member variable is initialized)
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void CAlertTypesDlg::OnDblclkAlertTargetListbox(void)
{
	// TODO: Add your control notification handler code here

    AlertSelected();

    // call the parent handler
    CDialog::OnOK();

}  // end of "CAlertTypesDlg::OnDblclkAlertTargetListbox"
