//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// HELPDLG - contains class declarations for CHelperDialog and CHelperPage
//	     helper classes.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/HELPDLG.CPv  $
// 
//    Rev 1.0   20 May 1998 17:15:32   DBuches
// Initial revision.
// 
//    Rev 1.1   29 Dec 1997 16:53:58   DBuches
// Added help support.
// 
//    Rev 1.0   29 Dec 1997 16:33:38   DBuches
// Initial revision.
// 
//    Rev 1.1   20 Aug 1997 20:10:26   DBUCHES
// Fixed broken ? help in CHelperDialog derived dialogs.
//
//    Rev 1.0   20 Aug 1997 18:32:26   DBUCHES
// Initial revision.
//
//    Rev 1.8   28 Jul 1997 08:56:16   DBUCHES
// Fixed another problem with what's this help on grayed controls.
//
//    Rev 1.7   22 Jul 1997 13:13:50   DBUCHES
// Added call to NG_SetFontsInDialog()
//
//    Rev 1.6   21 Jul 1997 12:17:20   DBUCHES
// Fixed problem with context help on disabled controls.
//
//    Rev 1.5   20 Jun 1997 12:01:06   DBUCHES
// Added Help support.
//
//    Rev 1.4   11 Jun 1997 14:05:38   DALLEE
// Removed redefinition of WM_PASSWORD_CHECK. Define is now in SHEET.H.
//
//    Rev 1.3   10 Jun 1997 20:19:46   DBUCHES
// Added changes for alert password.
//
//    Rev 1.2   04 Jun 1997 22:15:50   DBUCHES
// Commented out help handlers.  Needed this for RoboHelp.
//
//    Rev 1.1   27 May 1997 20:56:12   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.0   14 May 1997 14:58:16   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "HelpDlg.h"
#include "Navhelplauncher.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////////////
// CHelperPage property page

IMPLEMENT_DYNCREATE(CHelperPage, CPropertyPage)

CHelperPage::CHelperPage() : CPropertyPage()
{
    Initialize();
}

CHelperPage::CHelperPage(UINT nIDTemplate):CPropertyPage(nIDTemplate)
{
    Initialize();
}

void CHelperPage::Initialize()
{
    //{{AFX_DATA_INIT(CHelperPage)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    m_dwHelpButtonHelpID = 0;
    m_psp.dwFlags |= PSP_HASHELP;
}

CHelperPage::~CHelperPage()
{
}

void CHelperPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CHelperPage)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHelperPage, CPropertyPage)
    //{{AFX_MSG_MAP(CHelperPage)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


// this is where we go through our how to map and see if the command comming through
// here is one which is in the map.  If it is, then lets fire of the how to help pop up.


BOOL CHelperPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // Check for help button click.
    if( LOWORD(wParam) == ID_HELP && m_dwHelpButtonHelpID)
        {
            NAVToolbox::CNAVHelpLauncher Help;
            Help.LaunchHelp(m_dwHelpButtonHelpID, GetSafeHwnd());

            return TRUE;
        }

    return CPropertyPage::OnCommand(wParam, lParam);
}


BOOL CHelperPage::OnInitDialog()
{
    return CPropertyPage::OnInitDialog();
}
