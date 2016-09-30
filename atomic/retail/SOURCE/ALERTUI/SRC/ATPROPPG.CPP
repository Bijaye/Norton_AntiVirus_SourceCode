/////////////////////////////////////////////////////////////////////
// ATPropPg.cpp : implementation file
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ATPROPPG.CPv   1.1   25 Apr 1997 11:04:34   JBRENNA  $
//
// Description:
//      This class is used primarily to interface with the alert targets class
//      (ie, "CAlertOptions").  Its main focus is the non-UI tasks needed to
//      implement the alert target property pages for NAV.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ATPROPPG.CPv  $
// 
//    Rev 1.1   25 Apr 1997 11:04:34   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   15 Apr 1997 20:08:26   sedward
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AlertUI.h"
#include "at_opts.h"                // For CAlertOptions class
#include "AlertOpt.h"
#include "ATPropPg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




/////////////////////////////////////////////////////////////////////////////
// CAlertPropertyPage property page
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CAlertPropertyPage, CHelpPropertyPage)

CAlertPropertyPage::CAlertPropertyPage() : CHelpPropertyPage()
{
    m_alertType = 0;
    m_targetsListPtr = NULL;
}


CAlertPropertyPage::CAlertPropertyPage(UINT  dlgID) : CHelpPropertyPage(dlgID)
{
    m_alertType = 0;
    m_targetsListPtr = NULL;
}


CAlertPropertyPage::~CAlertPropertyPage()
{
    if (NULL != m_targetsListPtr)
        {
        m_targetsListPtr->DestroyAll();
        delete  m_targetsListPtr;
        }
}

void CAlertPropertyPage::DoDataExchange(CDataExchange* pDX)
{
    CHelpPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAlertPropertyPage)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlertPropertyPage, CHelpPropertyPage)
    //{{AFX_MSG_MAP(CAlertPropertyPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()





/////////////////////////////////////////////////////////////////////////////
// CAlertPropertyPage member functions
/////////////////////////////////////////////////////////////////////////////


// ===== CopyAlertTargetInfo ==============================================
//
//  Input:
//      source  -- a pointer to the source object
//      dest    -- a pointer to the destination object
//
//  Output: a value of FALSE (a derived class should implement this function
//          and return TRUE upon success)
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

BOOL    CAlertPropertyPage::CopyAlertTargetInfo(CAlertTarget*  source
                                                        , CAlertTarget*  dest)
{
    // this function should be implemented in a derived class and return TRUE
    return  (FALSE);

}  // end of "CAlertPropertyPage::CopyAlertTargetInfo"



// ===== CreateTempAlertTargetsList =======================================
//
//  This function creates and initializes a temporary alert targets list so
//  the user may edit its contents to his/her heart's content without
//  affecting the master list.  Only records matching the page's alert type
//  are copied from the master list to the new list.
//
//  Input:  nothing
//
//  Output: a value of TRUE if no errors occur, FALSE otherwise
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

CAlertOptions*  CAlertPropertyPage::CreateTempAlertTargetsList(void)
{
    // create a new alert targets list object
    try {
        m_targetsListPtr = new CAlertOptions;
        }

    catch(CMemoryException)
        {
        m_targetsListPtr = NULL;
        throw;      // forward the exception object to the next handler
        }

    // loop through the master alert targets list and copy all the records
    // to the temporary list
    auto    CAlertTarget*       pAlertTarget;
    auto    CAlertTarget*       pNewAlertTarget;

    gAlertOptions.ResetPosition();
    while (pAlertTarget = gAlertOptions.GetNextTarget(m_alertType))
        {
        pNewAlertTarget = GetNewAlertTarget();
        if (NULL != pNewAlertTarget)
            {
            CopyAlertTargetInfo(pAlertTarget, pNewAlertTarget);

            // add the new record to the new list
            m_targetsListPtr->AddTarget(pNewAlertTarget);
            }
        }

    return (m_targetsListPtr);

}  // end of "CAlertPropertyPage::CreateTempAlertTargetsList"



// ===== GetNewAlertTarget ================================================
//
//  This virtual function allocates a new alert target object and returns a
//  pointer to the object.
//
//  Input:  nothing
//
//  Output: a pointer to the new object
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

CAlertTarget*   CAlertPropertyPage::GetNewAlertTarget(void)
{
    auto    CAlertTarget*       newAlertTarget;

    try {
        newAlertTarget = new  CAlertTarget;
        }

    catch(CMemoryException)
        {
        newAlertTarget = NULL;
        throw;      // forward the exception object to the next handler
        }

    return  (newAlertTarget);

}  // end of "CAlertPropertyPage::GetNewAlertTarget"



// ===== TargetIsActive ===================================================
//
//  This virtual function does nothing, its implementation is left up to
//  derived classes.
//
//  Input:
//      pAlertTarget    -- a pointer to the alert target
//
//  Output:
//      a value of FALSE
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

BOOL    CAlertPropertyPage::TargetIsActive(CAlertTarget*  pAlertTarget)
{
    return (FALSE);

}  // end of "CAlertPropertyPage::TargetIsActive"



// ===== UpdateMasterAlertTargetsList =====================================
//
//  This function replaces all current target records in the master alert
//  targets list with those in the temporary targets list.
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

void    CAlertPropertyPage::UpdateMasterAlertTargetsList(void)
{
    auto    CAlertTarget*       pAlertTarget;
    auto    CAlertTarget*       pNewAlertTarget;

    // first remove all records of the matching target type from the
    // master list
    gAlertOptions.ResetPosition();
    while (pAlertTarget = gAlertOptions.GetNextTarget(m_alertType))
        {
        gAlertOptions.RemoveTarget(pAlertTarget);
        }

    // if there are any records in the list argument, add them to the master
    // targets list
    if (m_targetsListPtr->GetTargetCount() > 0)
        {
        m_targetsListPtr->ResetPosition();
        while (pAlertTarget = m_targetsListPtr->GetNextTarget(m_alertType))
            {
            pNewAlertTarget = GetNewAlertTarget();
            if  ((NULL != pNewAlertTarget)
            &&  (TRUE == CopyAlertTargetInfo(pAlertTarget, pNewAlertTarget)))
                {
                // add the new record to the global list
                gAlertOptions.AddTarget(pNewAlertTarget);
                }
            }
        }

}  // end of "CAlertPropertyPage::UpdateMasterAlertTargetsList"











/////////////////////////////////////////////////////////////////////////////
// CAlertPropertyPage message handlers
/////////////////////////////////////////////////////////////////////////////


// ==== OnCommand =========================================================
//
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

BOOL    CAlertPropertyPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

    return CHelpPropertyPage::OnCommand(wParam, lParam);

}  // end of "CAlertPropertyPage::OnCommand"



// ===== OnOK =============================================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

void    CAlertPropertyPage::OnOK(void)
{
	// TODO: Add your specialized code here and/or call the base class

    CHelpPropertyPage::OnOK();

}  // end of "CAlertPropertyPage::OnOK"