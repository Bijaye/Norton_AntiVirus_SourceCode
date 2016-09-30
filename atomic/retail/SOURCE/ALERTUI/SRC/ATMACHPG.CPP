/////////////////////////////////////////////////////////////////////
// ATMachPg.cpp : implementation file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ATMACHPG.CPv   1.1   25 Apr 1997 11:04:24   JBRENNA  $
//
// Description:
//
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ATMACHPG.CPv  $
// 
//    Rev 1.1   25 Apr 1997 11:04:24   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   15 Apr 1997 20:08:20   sedward
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AlertUI.h"
#include "AlertOpt.h"
#include "ATMachPg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




/////////////////////////////////////////////////////////////////////////////
// CAlertMachinePropertyPage property page
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CAlertMachinePropertyPage, CAlertListPropertyPage)

CAlertMachinePropertyPage::CAlertMachinePropertyPage() : CAlertListPropertyPage()
{
}


CAlertMachinePropertyPage::CAlertMachinePropertyPage(UINT  dlgID) : CAlertListPropertyPage(dlgID)
{
}


CAlertMachinePropertyPage::~CAlertMachinePropertyPage()
{
}

void CAlertMachinePropertyPage::DoDataExchange(CDataExchange* pDX)
{
    CAlertListPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAlertMachinePropertyPage)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlertMachinePropertyPage, CAlertListPropertyPage)
    //{{AFX_MSG_MAP(CAlertMachinePropertyPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()





/////////////////////////////////////////////////////////////////////////////
// CAlertMachinePropertyPage member functions
/////////////////////////////////////////////////////////////////////////////

// ==== HandleTargetEdit ==================================================
//
//  This function handles the editing of an alert target object.
//
//  NOTE: this is an implementation of the virtual function inherited from
//        the CAlertListPropertyPage class
//
//  Input:
//      pAlertTarget    -- a pointer to the alert target record
//      mode            -- an integer indicating the mode of the edit (ie, add
//                         or modify)
//
//  Output:
//      the result of the call to HandleMachineTargetEdit()
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

BOOL    CAlertMachinePropertyPage::HandleTargetEdit(CAlertTarget*  pAlertTarget
                                                                    , int  mode)
{
	// TODO: Add your specialized code here and/or call the base class

    auto    BOOL        result;
    result = HandleMachineTargetEdit(pAlertTarget, mode, this); // global function
    return  (result);

}  // end of "CAlertMachinePropertyPage::HandleTargetEdit"




















/////////////////////////////////////////////////////////////////////////////
// CAlertMachinePropertyPage message handlers
/////////////////////////////////////////////////////////////////////////////

