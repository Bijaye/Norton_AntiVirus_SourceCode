/////////////////////////////////////////////////////////////////////
// ATMailPg.cpp : implementation file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ATMAILPG.CPv   1.1   25 Apr 1997 11:04:26   JBRENNA  $
//
// Description:
//
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ATMAILPG.CPv  $
// 
//    Rev 1.1   25 Apr 1997 11:04:26   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   15 Apr 1997 20:08:22   sedward
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AlertUI.h"
#include "AlertOpt.h"
#include "ATMailPg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




/////////////////////////////////////////////////////////////////////////////
// CAlertEmailPropertyPage property page
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CAlertEmailPropertyPage, CAlertListPropertyPage)

CAlertEmailPropertyPage::CAlertEmailPropertyPage() : CAlertListPropertyPage()
{
}


CAlertEmailPropertyPage::CAlertEmailPropertyPage(UINT  dlgID) : CAlertListPropertyPage(dlgID)
{
}


CAlertEmailPropertyPage::~CAlertEmailPropertyPage()
{
}

void CAlertEmailPropertyPage::DoDataExchange(CDataExchange* pDX)
{
    CAlertListPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAlertEmailPropertyPage)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlertEmailPropertyPage, CAlertListPropertyPage)
    //{{AFX_MSG_MAP(CAlertEmailPropertyPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()





/////////////////////////////////////////////////////////////////////////////
// CAlertEmailPropertyPage member functions
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

BOOL    CAlertEmailPropertyPage::HandleTargetEdit(CAlertTarget*  pAlertTarget
                                                                    , int  mode)
{
	// TODO: Add your specialized code here and/or call the base class

    auto    BOOL        result;
    result = HandleSMTPEdit(pAlertTarget, mode); // global function
    return  (result);

}  // end of "CAlertEmailPropertyPage::HandleTargetEdit"




















/////////////////////////////////////////////////////////////////////////////
// CAlertEmailPropertyPage message handlers
/////////////////////////////////////////////////////////////////////////////