// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ScanWiz.cpp : implementation file
//

#include "stdafx.h"
#include "ScanDlvr.h"
#include "ScanWiz.h"
#include "WizPage.h"
#include "WrapUpPg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScanDeliverWizard

IMPLEMENT_DYNAMIC(CScanDeliverWizard, CPropertySheet)

CScanDeliverWizard::CScanDeliverWizard(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CScanDeliverWizard::CScanDeliverWizard(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CScanDeliverWizard::~CScanDeliverWizard()
{
}


BEGIN_MESSAGE_MAP(CScanDeliverWizard, CPropertySheet)
	//{{AFX_MSG_MAP(CScanDeliverWizard)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScanDeliverWizard message handlers



// ==== GetIndexFromResID =================================================
//
//  This function returns the index of a target property page, given its
//  resource ID (ie, IDD_DIALOG).
//
//  Input:
//      resID   -- the resource ID (ie, IDD_DIALOG) of a property page
//
//  Output:
//      The corresponding index of the page if a matching resource ID is
//      found.  If a matching ID is not found, a value of -1 is returned.
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

int CScanDeliverWizard::GetIndexFromResID(UINT  resID)
{
    auto    CWizardPropertyPage*    ptrCurrentPage = NULL;
    auto    int                     nIndex = 0;
    auto    int                     nTargetIndex = -1;
    auto    int                     nPageCount = 0;

    // get the number of pages currently in the property sheet
    nPageCount = GetPageCount();
    if (nPageCount < 0)
        {
        goto  Exit_Function;
        }

    // loop through the pages and see if we find a matching resource ID
    for (nIndex = 0; nIndex < nPageCount; ++nIndex)
        {
        ptrCurrentPage = (CWizardPropertyPage*)GetPage(nIndex);
        if (resID == (UINT)ptrCurrentPage->GetTemplateID())
            {
            nTargetIndex = nIndex;
            break;
            }
        }


Exit_Function:

    return (nTargetIndex);

}  // end of "CScanDeliverWizard::GetIndexFromResID"



// ==== SetWrapUpPageText =================================================
//
//  This function sets the text field contents for the final wizard panel.
//
//  Input:
//      dwWrapUpType    -- a DWORD indicating the type of "wrap up" the
//                         caller wants.  The final property page will
//                         use this value to reference target strings in
//                         the resource table for display.
//  Output:
//      A value of TRUE if all goes well, FALSE if not.
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverWizard::SetWrapUpPageText(DWORD  dwWrapUpType)
{
    auto    BOOL                    bResult = FALSE;
    auto    CWrapUpPropertyPage*    pagePtr;
    auto    int                     nIndex;

    nIndex = GetIndexFromResID(IDD_WRAP_UP);
    if (nIndex >= 0)
        {
        pagePtr = (CWrapUpPropertyPage*)GetPage(nIndex);
        if ((pagePtr)  &&  (pagePtr->SetTextContent(dwWrapUpType)))
            {
            bResult = TRUE;
            }
        }

    return (bResult);

}  // end of "CScanDeliverWizard::SetWrapUpPageText"



// ==== GoToWrapUpPage ====================================================
//
//  This function lets the caller set the final wizard page text content
//  and make the final page the next page in one function call.
//
//  Input:
//      dwWrapUpType    -- a DWORD indicating the type of "wrap up" the
//                         caller wants.  The final property page will
//                         use this value to reference target strings in
//                         the resource table for display.
//  Output:
//      A value of TRUE if all goes well, FALSE if not.
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverWizard::GoToWrapUpPage(DWORD  dwWrapUpType)
{
    auto    BOOL        bResult = FALSE;
    auto    int         nIndex = 0;

    // set the text to display
    if (TRUE == SetWrapUpPageText(dwWrapUpType))
        {
        nIndex = GetIndexFromResID(IDD_WRAP_UP);
        if (nIndex >= 0)
            {
            SetActivePage(nIndex);
            }
        }

    return (bResult);

}  // end of "CScanDeliverWizard::GoToWrapUpPage"



// ==== OnCommand =========================================================
//
//
// ========================================================================
//  Function created: 6/98, SEDWARD (thanks DB!)
// ========================================================================

BOOL    CScanDeliverWizard::OnCommand(WPARAM  wParam, LPARAM  lParam)
{
    auto    CString     captionStr;
    auto    CString     mesgStr;
    auto    int         nResult = 0;

    // if we receive a "cancel" message, verify that the user wishes to do so;
    // else, forward the message downstream to the parent handler
    if (IDCANCEL == LOWORD(wParam))
        {
        // pop up the message box
        captionStr.LoadString(IDS_CAPTION_STANDARD_MESGBOX);
        mesgStr.LoadString(IDS_ARE_YOU_SURE_YOU_WANT_TO_CANCEL);
        nResult = MessageBox(mesgStr, captionStr, MB_ICONWARNING
                                                    | MB_YESNO
                                                    | MB_DEFBUTTON2);
        if (IDYES != nResult)
            {
            // "TRUE" means we handled the message ourselves
            return TRUE;
            }
        }

    return CPropertySheet::OnCommand( wParam, lParam );

}  // end of "CScanDeliverWizard::OnCommand"





