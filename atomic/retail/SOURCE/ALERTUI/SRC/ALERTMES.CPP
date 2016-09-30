/////////////////////////////////////////////////////////////////////
// AlertMes.cpp : implementation file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ALERTMES.CPv   1.4   12 Nov 1997 15:11:02   TSmith  $
//
// Description:
//
//  This file contains the code for the alert message box options.  It
//  allows the user to configure the virus alert message box for the local
//  machine.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ALERTMES.CPv  $
//
//    Rev 1.4   12 Nov 1997 15:11:02   TSmith
// Removed obsolete include of UniUtil.h
//
//    Rev 1.3   05 Nov 1997 15:32:48   TSMITH
// Changed usage of NQ_SetFontsInDialog to NG_SetFontsInDialog to pick up DBCS
// font handling in SYMNAV.
//
//    Rev 1.2   22 May 1997 16:15:46   DSACKIN
// Added support for ANSI as well as UNICODE.
//
//    Rev 1.1   25 Apr 1997 11:03:16   JBRENNA
// Ported from Tesla.
//
//    Rev 1.0   13 Mar 1997 15:06:18   RFULLER
// Initial revision
//
//    Rev 1.15   03 Feb 1997 18:20:28   SEDWARD
// Replaced 'NQFont' stuff with 'UniUtil' stuff.
//
//    Rev 1.14   22 Jan 1997 16:56:42   SEDWARD
// Added run-time DBCS check to 'OnOK' to calculate the MBCS buffer size.
//
//    Rev 1.13   21 Jan 1997 23:35:06   SEDWARD
// Used Win32 wide char / mbcs instead.
//
//    Rev 1.12   21 Jan 1997 18:38:30   SEDWARD
// Unicode conversion for the navopts alert message string.
//
//    Rev 1.11   20 Jan 1997 00:04:12   SEDWARD
// Fixed syntax error.
//
//    Rev 1.10   19 Jan 1997 23:40:40   SEDWARD
// Updated help maps, fixed message length bug (#76678).
//
//    Rev 1.9   17 Jan 1997 12:49:58   TSmith
// Modified 'CAlertMessageDlg::OnOK' to subtract 1 from buffer size passed to
// 'wcstombs'. This will leave room for the NULL terminator and prevents garbage
// from being placed at the end of the custom alert message if the user enters
// >= SIZE_ALERT_MSG+1 in the UI.
//
//    Rev 1.8   16 Jan 1997 02:13:32   SEDWARD
// Lots of updates in help maps and menus.
//
//    Rev 1.7   14 Jan 1997 13:21:16   SEDWARD
// Updated help maps with new IDs.
//
//    Rev 1.5   09 Dec 1996 20:46:52   SEDWARD
//
//    Rev 1.4   03 Dec 1996 19:49:02   SEDWARD
// Substantial changes (not frozen yet...).
//
//    Rev 1.3   27 Nov 1996 18:49:30   SEDWARD
// Hooked up NAVOPTS to the local alert message configuration dialog.
//
//    Rev 1.2   25 Nov 1996 15:46:16   SEDWARD
// No change.
//
//    Rev 1.1   21 Nov 1996 17:07:42   SEDWARD
// Added vlog headers.
//
//    Rev 1.0   21 Nov 1996 16:55:06   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "AlertUI.h"
#include "AlertMes.h"
#include "helpdlg.h"                // base class for default help support
#include "nvetwhlp.h"               // context help
#include "navutil.h"                // context help
#include "help_ids.h"               // help IDs
#include "ngui.h"
#include "dbcs.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// the "what's this" and "how to" maps
DWORD   CAlertMessageDlg::m_dwWhatsThisMap[] =
{
    IDC_MESSAGE_CONTENT_TEXT, IDH_NAVNT_WHATHIS_CUSTOM_ALERT_MESSAGE_TXTBX
    , IDC_DISPLAY_ALERT_MESG_CHECKBOX, IDH_NAVNT_WHATHIS_DISPLAY_ALERT_MESSAGE_CHKBX
    , IDC_ALERT_MESG_SOUND_CHECKBOX, IDH_NAVNT_WHATHIS_SOUND_AUDIBLE_ALERT_CHKBOX
    , 0,0
};


DWORD   CAlertMessageDlg::m_dwWhatsThisGrayMap[] =
{
    0,0
};


DWORD   CAlertMessageDlg::m_dwHowToMap[] =
{
    ID_HT_CREATE_ALERT_MESSAGE, IDH_NAVNT_TASK_CREATE_A_VIRUS_ALERT_MESSAGE
    , 0,0
};




/////////////////////////////////////////////////////////////////////////////
//
// CAlertMessageDlg dialog
//
/////////////////////////////////////////////////////////////////////////////


CAlertMessageDlg::CAlertMessageDlg(CWnd* pParent /*=NULL*/)
        : CHelpDialog(CAlertMessageDlg::IDD, pParent)
{
        //{{AFX_DATA_INIT(CAlertMessageDlg)
        //}}AFX_DATA_INIT

    // set up the help map pointers
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_pHelpWhatsThisGrayMap = m_dwWhatsThisGrayMap;
    m_pHelpHowToMap = m_dwHowToMap;

    // initialize the "how to" index for this dialog
    SetHowToMenuIndex(HOW_TO_INDEX_LOCAL_MACHINE);
}


void CAlertMessageDlg::DoDataExchange(CDataExchange* pDX)
{
        CHelpDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CAlertMessageDlg)
        DDX_Control(pDX, IDC_DISPLAY_ALERT_MESG_CHECKBOX, m_displayMessage);
        DDX_Control(pDX, IDC_ALERT_MESG_SOUND_CHECKBOX, m_alertSound);
        DDX_Control(pDX, IDC_ALERT_MESG, m_alertMessage);
        //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlertMessageDlg, CHelpDialog)
        //{{AFX_MSG_MAP(CAlertMessageDlg)
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAlertMessageDlg message handlers
/////////////////////////////////////////////////////////////////////////////



// ===== OnInitDialog =====================================================
//
//  This function initializes the controls in the dialog window, using the
//  current contents of the NAVOPTS structure.
//
//  Input:  nothing
//  Output: a value of TRUE
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL CAlertMessageDlg::OnInitDialog(void)
{
    // enable for DBCS fonts if necessary (non-quake call)
//    NG_SetFontsInDialog (GetSafeHwnd(), 0);

    // call parent function
    CHelpDialog::OnInitDialog();

        // TODO: Add extra initialization here

    // set the window title
    auto    CString         tempStr;
    if (tempStr.LoadString(IDS_LOCAL_MACHINE))
        {
        this->SetWindowText(tempStr);
        }

    // limit the length of the message text that can be entered
    auto    int     limitLength;
    if (DBCSInstalled())
        {
        limitLength = ((sizeof(glpNavOptions->alert.szAlertMsg)) / 2);
        }
    else
        {
        limitLength = sizeof(glpNavOptions->alert.szAlertMsg);
        }
    --limitLength;
    m_alertMessage.LimitText(limitLength);

#ifdef UNICODE

    // convert the message to Unicode and display it
    auto    size_t      numBytesConverted;
    auto    TCHAR       tempTBuf[MAX_PATH];
    memset(tempTBuf, 0, sizeof(tempTBuf));
    numBytesConverted = MultiByteToWideChar(CP_ACP, 0
                                , glpNavOptions->alert.szAlertMsg
                                , -1    // mbcs string is null-terminated
                                , tempTBuf
                                , MAX_PATH);
    if (numBytesConverted > 0)
        {
        m_alertMessage.SetWindowText(tempTBuf);
        }
#else // #ifdef UNICODE
   m_alertMessage.SetWindowText(glpNavOptions->alert.szAlertMsg);
#endif // #ifdef UNICODE


    // the "sound" checkbox
    m_alertSound.SetCheck(glpNavOptions->alert.bBeep);

    // the display alert message checkbox
    m_displayMessage.SetCheck(glpNavOptions->alert.bDispAlertMsg);

    return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CAlertMessageDlg::OnInitDialog"



// ===== OnOK =============================================================
//
//  This function copies the values in the dialog controls to the NAVOPTS
//  structure.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void CAlertMessageDlg::OnOK(void)
{
        // TODO: Add extra validation here

    auto    CString         tempStr;

    // clear old message text from struct
    memset(glpNavOptions->alert.szAlertMsg, 0
                                , sizeof(glpNavOptions->alert.szAlertMsg));

    // fetch the message content
    m_alertMessage.GetWindowText(tempStr);
    if (FALSE == tempStr.IsEmpty())
        {

#ifdef UNICODE

        // calculate the size of the MBCS buffer
        auto    int     mbcsBufSize;
        if (DBCSInstalled())
            {
            mbcsBufSize = ((sizeof(glpNavOptions->alert.szAlertMsg)) / 2);
            }
        else
            {
            mbcsBufSize = sizeof(glpNavOptions->alert.szAlertMsg);
            }
        --mbcsBufSize;

        // convert the string
        WideCharToMultiByte(CP_ACP, 0, LPCTSTR(tempStr)
                                    , -1    // unicode string is null terminated
                                    , glpNavOptions->alert.szAlertMsg
                                    , mbcsBufSize
                                    , NULL
                                    , NULL);

#else // #ifdef UNICODE

                                        // If we are running in ANSI mode, we
                                        // should just be able to string copy
                                        // over the new string.
        STRCPY(glpNavOptions->alert.szAlertMsg, tempStr);

#endif // #ifdef UNICODE

        }

    // the "sound" checkbox
    glpNavOptions->alert.bBeep = m_alertSound.GetCheck();

    // the display alert message checkbox
    glpNavOptions->alert.bDispAlertMsg = m_displayMessage.GetCheck();

    CHelpDialog::OnOK();

}  // end of "CAlertMessageDlg::OnOK"
