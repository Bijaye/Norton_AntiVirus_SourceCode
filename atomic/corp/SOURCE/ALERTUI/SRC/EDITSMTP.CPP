/////////////////////////////////////////////////////////////////////
// EditSMTP.cpp : implementation file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/EDITSMTP.CPv   1.3   12 Nov 1997 15:10:10   TSmith  $
//
// Description:
//  This file contains the dialog that allows a user to enter a new SMTP
//  email record, or modify and existing record.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/EDITSMTP.CPv  $
// 
//    Rev 1.3   12 Nov 1997 15:10:10   TSmith
// Removed obsolete include of UniUtil.h
// 
//    Rev 1.2   05 Nov 1997 15:33:40   TSMITH
// Changed usage of NQ_SetFontsInDialog to NG_SetFontsInDialog to pick up DBCS
// font handling in SYMNAV.
// 
//    Rev 1.1   25 Apr 1997 11:03:20   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.1   15 Apr 1997 20:03:38   sedward
// Include ATSmtp.h instead of SMTPPg.h.
//
//    Rev 1.10   03 Feb 1997 18:20:48   SEDWARD
// Replaced 'NQFont' stuff with 'UniUtil' stuff.
//
//    Rev 1.9   28 Jan 1997 17:20:46   SEDWARD
// Added 'howto' for single edit SMTP record dialog.
//
//    Rev 1.8   21 Jan 1997 18:39:20   SEDWARD
// DBCS-enabling of the dialog.
//
//    Rev 1.7   16 Jan 1997 02:13:54   SEDWARD
// Lots of updates in help maps and menus.
//
//    Rev 1.6   15 Jan 1997 13:58:26   SEDWARD
// Added 'UpdateOKButton()'.
//
//    Rev 1.4   14 Jan 1997 13:22:26   SEDWARD
// Update help maps with new IDs, fixed class inheritance (now from CHelpDialog
// instead of CDialog).
//
//    Rev 1.2   25 Nov 1996 15:46:30   SEDWARD
// No change.
//
//    Rev 1.1   21 Nov 1996 17:07:46   SEDWARD
// Added vlog headers.
//
//    Rev 1.0   21 Nov 1996 16:55:06   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////




#include "stdafx.h"
#include "AlertUI.h"
#include "EditSMTP.h"
#include "ATSmtpPg.h"
#include "helpdlg.h"                // base class for default help support
#include "nvetwhlp.h"               // context help
#include "navutil.h"                // context help
#include "help_ids.h"               // help IDs
#include "ngui.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



// the "what's this" and "how to" maps
DWORD   CEmailEditSMTP::m_dwWhatsThisMap[] =
{
    IDC_EMAIL_NAME_STATIC, IDH_NAVNT_WHATHIS_EDIT_STMP_NAME_TXTBX
    , IDC_EMAIL_ADDRESS_STATIC, IDH_NAVNT_WHATHIS_EDIT_STMP_ADDRESS_TXTBX
    , 0,0
};


DWORD   CEmailEditSMTP::m_dwWhatsThisGrayMap[] =
{
        0,0
};


DWORD   CEmailEditSMTP::m_dwHowToMap[] =
{
    ID_HT_ADD_SMTP, IDH_NAVNT_TASK_ADD_AN_EMAIL_ALERT_TARGET
    , ID_HT_MODIFY_SMTP, IDH_NAVNT_TASK_EDIT_A_SINGLE_EMAIL_TARGET
    , 0,0
};



/////////////////////////////////////////////////////////////////////////////
// CEmailEditSMTP dialog
/////////////////////////////////////////////////////////////////////////////


CEmailEditSMTP::CEmailEditSMTP(CWnd* pParent /*=NULL*/)
    : CHelpDialog(CEmailEditSMTP::IDD, pParent)
{
    //{{AFX_DATA_INIT(CEmailEditSMTP)
        m_internetAddressStr = _T("");
        m_internetNameStr = _T("");
        //}}AFX_DATA_INIT

    // set up the help map pointers
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_pHelpWhatsThisGrayMap = m_dwWhatsThisGrayMap;
    m_pHelpHowToMap = m_dwHowToMap;

    // initialize the "how to" index for this dialog
    SetHowToMenuIndex(HOW_TO_INDEX_SINGLE_SMTP_EDIT);
}


void CEmailEditSMTP::DoDataExchange(CDataExchange* pDX)
{
        CHelpDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CEmailEditSMTP)
        DDX_Control(pDX, IDOK, m_OKButton);
        DDX_Control(pDX, IDC_EMAIL_NAME, m_internetName);
        DDX_Control(pDX, IDC_EMAIL_ADDRESS, m_internetAddress);
        DDX_Text(pDX, IDC_EMAIL_ADDRESS, m_internetAddressStr);
        DDX_Text(pDX, IDC_EMAIL_NAME, m_internetNameStr);
        //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEmailEditSMTP, CHelpDialog)
    //{{AFX_MSG_MAP(CEmailEditSMTP)
        ON_EN_CHANGE(IDC_EMAIL_NAME, OnChangeEmailName)
    ON_COMMAND(IDR_WHATS_THIS, OnWhatsThis)
        ON_EN_CHANGE(IDC_EMAIL_ADDRESS, OnChangeEmailAddress)
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CEmailEditSMTP member functions
/////////////////////////////////////////////////////////////////////////////

// ==== UpdateOKButton ====================================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

void    CEmailEditSMTP::UpdateOKButton(void)
{
        // TODO: Add your specialized code here and/or call the base class

    // only enable the OK button if both fields have SOMETHING in them...
    if  ((m_internetName.LineLength() > 0)
    &&  (m_internetAddress.LineLength() > 0))
        {
        m_OKButton.EnableWindow(TRUE);
        }
    else
        {
        m_OKButton.EnableWindow(FALSE);
        }

}  // end of "CEmailEditSMTP::UpdateOKButton"







/////////////////////////////////////////////////////////////////////////////
// CEmailEditSMTP message handlers
/////////////////////////////////////////////////////////////////////////////



// ===== OnChangeEmailAddress =============================================
//
//  This function is called when the contents of the "address" edit field
//  are changed.  The "OK" button is enabled/disabled accordingly.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CEmailEditSMTP::OnChangeEmailAddress(void)
{
        // TODO: Add your control notification handler code here

    // update the OK button based on field contents
    UpdateOKButton();

}   // end of "CEmailEditSMTP::OnChangeEmailAddress"



// ===== OnChangeEmailName ================================================
//
//  This function is called when the contents of the "name" edit field are
//  changed.  The "OK" button is enabled/disabled accordingly.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void CEmailEditSMTP::OnChangeEmailName(void)
{
        // TODO: Add your control notification handler code here

    // update the OK button based on field contents
    UpdateOKButton();

}   // end of "CEmailEditSMTP::OnChangeEmailName"



// ==== OnCommand =========================================================
//
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

BOOL    CEmailEditSMTP::OnCommand(WPARAM wParam, LPARAM lParam)
{
        // TODO: Add your specialized code here and/or call the base class

    // if the user clicked on a control, store a pointer to its window in the
    // current context variable so it'll be ready for context help
    if (lParam > 0)
        {
        m_pCurrentContextWnd = GetDlgItem(LOWORD(wParam));
        }

        return CHelpDialog::OnCommand(wParam, lParam);

}  // end of "CEmailEditSMTP::OnCommand"



// ===== OnInitDialog =====================================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL CEmailEditSMTP::OnInitDialog(void)
{
    // enable for DBCS fonts if necessary (non-quake call)
    NG_SetFontsInDialog (GetSafeHwnd(), 0);

    // call parent function
    CHelpDialog::OnInitDialog();

        // TODO: Add extra initialization here

    // enable or disable the OK button
    UpdateOKButton();

    // all ready, so set focus on the "name" edit field
    m_internetName.SetFocus();

    return FALSE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE

}   // end of "CEmailEditSMTP::OnInitDialog"



// ===== OnOK =============================================================
//
//  This function is called when the user presses the "OK" button.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void CEmailEditSMTP::OnOK()
{
        // TODO: Add extra validation here

    // set the focus back to the email address field
    m_internetName.SetFocus();

    // call the parent handler
        CHelpDialog::OnOK();

}   // end of "CEmailEditSMTP::OnOK"



// ==== OnWhatsThis =======================================================
//
//  This function handles the "What's This" events.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

void    CEmailEditSMTP::OnWhatsThis(void)
{
        // TODO: Add your specialized code here and/or call the base class

    CHelpDialog::OnWhatsThis();

}  // end of "CEmailEditSMTP::OnWhatsThis"
