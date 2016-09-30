/////////////////////////////////////////////////////////////////////
// EmailPSh.cpp : implementation file
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:
//
// Description:
//
//  This file contains the property sheet for the SMTP email pages.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:
//
//
/////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "alertui.h"
#include "alrtres.h"
#include "EmailPSh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEmailPropertySheet

IMPLEMENT_DYNAMIC(CEmailPropertySheet, CNoContextPropertySheet)

CEmailPropertySheet::CEmailPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
    :CNoContextPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
    m_settingsDialogInitialized = FALSE;
}

CEmailPropertySheet::CEmailPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
    :CNoContextPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
    m_settingsDialogInitialized = FALSE;
}

CEmailPropertySheet::~CEmailPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CEmailPropertySheet, CNoContextPropertySheet)
	//{{AFX_MSG_MAP(CEmailPropertySheet)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()









/////////////////////////////////////////////////////////////////////////////
//
// CEmailPropertySheet member functions
//
/////////////////////////////////////////////////////////////////////////////




// ==== MinimalSmtpFieldsDefined ==========================================
//
//  This function checks to see if the minimal SMTP fields are initialized.
//
//  Input:  nothing
//
//  Output: a value of TRUE if the minimal SMTP fields are initialized, a
//          value of FALSE if they are not
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

BOOL    CEmailPropertySheet::MinimalSmtpFieldsDefined(void)
{
    auto    BOOL                result = FALSE;


    if  ((TRUE == SmtpSenderDefined())  &&  (TRUE == SmtpServerDefined()))
        {
        result = TRUE;
        }

    return (result);

}  // end of "CEmailPropertySheet::MinimalSmtpFieldsDefined"



// ==== SetSettingsDialogFlag =============================================
//
//  This function sets the member variable that indicates the "settings"
//  property page has been initialized.  This helps the "SmtpSenderDefined"
//  and "SmtpServerDefined" functions know whether its safe to check the
//  "gAlertOptsions" object or inspect the settings property page for valid
//  values.
//
//  Input:  nothing
//
//  Output: a value of TRUE if a SMTP mail server is defined, a value of
//          FALSE otherwise
//
// ========================================================================
//  Function created: 2/97, SEDWARD
// ========================================================================

void    CEmailPropertySheet::SetSettingsDialogFlag(void)
{
    m_settingsDialogInitialized = TRUE;

}  // end of "CEmailPropertySheet::SetSettingsDialogFlag"



// ==== SmtpSenderDefined =================================================
//
//  This function checks to see if a SMTP mail sender is defined.  If not,
//  a value of FALSE is returned so the property sheet can alert the user.
//
//  Input:  nothing
//
//  Output: a value of TRUE if a SMTP mail sender is defined, a value of
//          FALSE otherwise
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

BOOL    CEmailPropertySheet::SmtpSenderDefined(void)
{
    auto    BOOL                result = TRUE;
    auto    BOOL                verified = FALSE;
    auto    CEdit*              tempEditCtrl = NULL;
    auto    CPropertyPage*      pagePtr = NULL;
    auto    CString             tempStr;
    auto    int                 activeIndex;

    // get the index of the current page
    activeIndex = GetActiveIndex();

    // if the SMTP address list is the current page...
    if (SMTP_LIST_PAGE_INDEX == activeIndex)
        {
        // ... and if the settings page has not yet been intialized...
        if (FALSE == m_settingsDialogInitialized)
            {
            // ... make our determination based on what exists in the
            // navalert.dat file
            tempStr = gAlertOptions.GetSMTPAuthor();
            if (TRUE == tempStr.IsEmpty())
                {
                result = FALSE;
                }

            // flag that we've already completed verification
            verified = TRUE;
            }
        else
            {
            // ... else, make our determination based on what currently
            // exists in the settings property page; get a pointer to the
            // edit control
            pagePtr = GetPage(SMTP_SETTINGS_PAGE_INDEX);
            if (NULL != pagePtr)
                {
                tempEditCtrl = (CEdit*)pagePtr->GetDlgItem(IDC_EMAIL_FROMNAME_EDIT);
                }
            }
        }
    else if (SMTP_SETTINGS_PAGE_INDEX == activeIndex)
        {
        // get a pointer to the active page and its edit control
        pagePtr = GetActivePage();
        if (NULL != pagePtr)
            {
            tempEditCtrl = (CEdit*)pagePtr->GetDlgItem(IDC_EMAIL_FROMNAME_EDIT);
            }
        }

    // if we've not yet verified the "sender" status, and the edit control
    // pointer is valid, see if it's empty
    if ((FALSE == verified)  &&  (NULL != tempEditCtrl))
        {
        tempEditCtrl->GetWindowText(tempStr);
        if (TRUE == tempStr.IsEmpty())
            {
            result = FALSE;
            }
        }

    return (result);

}  // end of "CEmailPropertySheet::SmtpSenderDefined"



// ==== SmtpServerDefined =================================================
//
//  This function checks to see if a SMTP mail server is defined.  If not,
//  a value of FALSE is returned so the property sheet can alert the user.
//
//  Input:  nothing
//
//  Output: a value of TRUE if a SMTP mail server is defined, a value of
//          FALSE otherwise
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

BOOL    CEmailPropertySheet::SmtpServerDefined(void)
{
    auto    BOOL                result = TRUE;
    auto    BOOL                verified = FALSE;
    auto    CEdit*              tempEditCtrl = NULL;
    auto    CPropertyPage*      pagePtr = NULL;
    auto    CString             tempStr;
    auto    int                 activeIndex;

    // get the index of the current page
    activeIndex = GetActiveIndex();

    // if the SMTP address list is the current page...
    if (SMTP_LIST_PAGE_INDEX == activeIndex)
        {
        // ... and if the settings page has not yet been intialized...
        if (FALSE == m_settingsDialogInitialized)
            {
            // ... make our determination based on what exists in the
            // navalert.dat file
            tempStr = gAlertOptions.GetSMTPPostOffice();
            if (TRUE == tempStr.IsEmpty())
                {
                result = FALSE;
                }

            // flag that we've already completed verification
            verified = TRUE;
            }
        else
            {
            // ... else, make our determination based on what currently
            // exists in the settings property page; get a pointer to the
            // edit control
            pagePtr = GetPage(SMTP_SETTINGS_PAGE_INDEX);
            if (NULL != pagePtr)
                {
                tempEditCtrl = (CEdit*)pagePtr->GetDlgItem(IDC_SMTP_SERVER_EDIT);
                }
            }
        }
    else if (SMTP_SETTINGS_PAGE_INDEX == activeIndex)
        {
        // get a pointer to the active page and its edit control
        pagePtr = GetActivePage();
        if (NULL != pagePtr)
            {
            tempEditCtrl = (CEdit*)pagePtr->GetDlgItem(IDC_SMTP_SERVER_EDIT);
            }
        }

    // if we've not yet verified the "server" status, and the edit control
    // pointer is valid, see if it's empty
    if ((FALSE == verified)  &&  (NULL != tempEditCtrl))
        {
        tempEditCtrl->GetWindowText(tempStr);
        if (TRUE == tempStr.IsEmpty())
            {
            result = FALSE;
            }
        }

    return (result);

}  // end of "CEmailPropertySheet::SmtpServerDefined"








/////////////////////////////////////////////////////////////////////////////
//
// CEmailPropertySheet message handlers
//
/////////////////////////////////////////////////////////////////////////////



// ==== OnCommand =========================================================
//
//
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

BOOL    CEmailPropertySheet::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

    if  ((IDOK == LOWORD(wParam))  &&  (FALSE == MinimalSmtpFieldsDefined()))
        {
        auto    CString         captionStr;
        auto    CString         textStr;

        // load the message strings
        captionStr.LoadString(IDS_EMAIL_ALERT_CONFIGURATION);
        textStr.LoadString(IDS_MISSING_SMTP_SERVER_INFO);
        MessageBox(textStr, captionStr, MB_ICONEXCLAMATION | MB_OK);
        }

    return CNoContextPropertySheet::OnCommand(wParam, lParam);

}  // end of "CEmailPropertySheet::OnCommand"



// ==== OnContextMenu =====================================================
//
//  This purpose of this function is to "swallow" the context menu events
//  so that the user cannot get a context menu from the property sheet's
//  "OK", "Cancel" or "Help" buttons.  The reason for this is so that
//  derived property sheets are consistent with the rest of NAV, which has
//  no context menus on these buttons in its property sheets.  Hopefully
//  this will no longer be necessary once the "Options" property sheet is
//  converted from the Norton Control Panel to a true MFC property sheet.
//
//  Input:  the default MFC arguments
//  Output: nothing
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

void CEmailPropertySheet::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
    return;
}