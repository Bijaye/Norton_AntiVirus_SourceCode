/////////////////////////////////////////////////////////////////////
// ATSmtpPg.cpp : implementation file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ATSMTPPG.CPv   1.1   25 Apr 1997 11:04:36   JBRENNA  $
//
// Description:
//      This file contains the class implementations for both the SMTP alert
//      target property page and its corresponding "settings" property page.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ATSMTPPG.CPv  $
// 
//    Rev 1.1   25 Apr 1997 11:04:36   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   15 Apr 1997 20:08:26   sedward
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AlertUI.h"
#include "AlertOpt.h"
#include "ATSmtpPg.h"
#include "EmailPSh.h"
#include "help_ids.h"               // help IDs


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// the "what's this" and "how to" maps
DWORD   dwSmtpWhatsThisMap[] =
{
    IDC_ADD_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_ADD_ITEM_BTN
    , IDC_MODIFY_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_MODIFY_ITEM_BTN
    , IDC_REMOVE_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_REMOVE_ITEM_BTN
    , IDC_SMTP_TARGET_LIST, IDH_NAVNT_WHATHIS_MESSERV_TARGETS_LSTBX
    , 0,0
};


DWORD   dwSmtpHowToMap[] =
{
    ID_HELP, IDH_NAVNT_EMAIL_SETTINGS_DLG_HELP_BTN
    , ID_HT_DEFINE_SERVER_AND_MESSAGE_SMTP, IDH_NAVNT_TASK_DEFINE_THE_SMTP_SERVER_AND_MESSAGE_FIELDS
    , ID_HT_ADD_SMTP, IDH_NAVNT_TASK_ADD_AN_EMAIL_ALERT_TARGET
    , ID_HT_MODIFY_SMTP, IDH_NAVNT_TASK_EDIT_A_SINGLE_EMAIL_TARGET
    , ID_HT_EDITLIST_SMTP, IDH_NAVNT_TASK_EDIT_THE_EMAIL_TARGET_LIST
    , ID_HT_REMOVE_SMTP, IDH_NAVNT_TASK_REMOVE_AN_EMAIL_TARGET
    , 0,0,
};





/////////////////////////////////////////////////////////////////////////////
// CAlertSmtpPropertyPage property page
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CAlertSmtpPropertyPage, CAlertEmailPropertyPage)

CAlertSmtpPropertyPage::CAlertSmtpPropertyPage()
    : CAlertEmailPropertyPage()
{
    InitializeData();
}


CAlertSmtpPropertyPage::CAlertSmtpPropertyPage(UINT  dlgID)
    : CAlertEmailPropertyPage(dlgID)
{
    InitializeData();
}


CAlertSmtpPropertyPage::~CAlertSmtpPropertyPage()
{
}

void CAlertSmtpPropertyPage::DoDataExchange(CDataExchange* pDX)
{
    CAlertEmailPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAlertSmtpPropertyPage)
    DDX_Control(pDX, IDC_SMTP_TARGET_LIST, m_listCtrl);
	DDX_Control(pDX, IDC_REMOVE_BUTTON, m_removeButton);
	DDX_Control(pDX, IDC_MODIFY_BUTTON, m_modifyButton);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_addButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlertSmtpPropertyPage, CAlertEmailPropertyPage)
    //{{AFX_MSG_MAP(CAlertSmtpPropertyPage)
	ON_NOTIFY(NM_CLICK, IDC_SMTP_TARGET_LIST, OnClickSmtpTargetList)
	ON_NOTIFY(NM_DBLCLK, IDC_SMTP_TARGET_LIST, OnDblclkSmtpTargetList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()





/////////////////////////////////////////////////////////////////////////////
// CAlertSmtpPropertyPage member functions
/////////////////////////////////////////////////////////////////////////////




// ===== CopyAlertTargetInfo ==============================================
//
//  This function copies the necessary data items from the source alert target
//  to the destination alert target.
//
//  NOTE: this is an implementation of the virtual function inherited from
//        the CAlertPropertyPage class
//
//  Input:
//      source  -- a pointer to the source object
//      dest    -- a pointer to the destination object
//
//  Output: a value of TRUE
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

BOOL    CAlertSmtpPropertyPage::CopyAlertTargetInfo(CAlertTarget*  source
                                                        , CAlertTarget*  dest)
{
    auto    CEMailTarget*       tempDest = (CEMailTarget*)dest;
    auto    CEMailTarget*       tempSource = (CEMailTarget*)source;

    // copy the source object data over to the destination object
    tempDest->SetName(tempSource->GetName());
    tempDest->SetAddress(tempSource->GetAddress());
    tempDest->SetActive(tempSource->GetActive());
    tempDest->SetPriority(tempSource->GetPriority());
    return  (TRUE);

}  // end of "CAlertSmtpPropertyPage::CopyAlertTargetInfo"



// ===== GetDisplayString =================================================
//
//  This function is used to fetch a target string to display from the input
//  alert target record.
//
//  NOTE: this is an implementation of the virtual function inherited from
//        the CAlertListPropertyPage class
//
//  Input:
//      pAlertTarget    -- a pointer to the alert target record
//      displayStr      -- a reference to the display string
//      index           -- an index used to select which string to fetch from
//                         the alert target record
//
//  Output:
//      a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

BOOL    CAlertSmtpPropertyPage::GetDisplayString(CAlertTarget*  pAlertTarget
                                                        , CString&  displayStr
                                                        , int  index)
{
    auto    BOOL                result = TRUE;
    auto    CEMailTarget*       targetPtr = (CEMailTarget*)pAlertTarget;

    if (index > (m_totalDisplayStrings - 1))
        {
        result = FALSE;
        }
    else
        {
        switch (index)
            {
            case   0:
                displayStr = targetPtr->GetName();
                break;

            case   1:
                displayStr = targetPtr->GetAddress();
                break;

            default:
                result = FALSE;
                break;
            }
        }

    return  (result);

}  // end of "CAlertSmtpPropertyPage::GetDisplayString"



// ===== GetNewAlertTarget ================================================
//
//  This function creates a new alert target and returns the pointer to the
//  calling function.
//
//  NOTE: this is an implementation of the virtual function inherited from
//        the CAlertListPropertyPage class
//
//  Input:  nothing
//
//  Output: a pointer to the new object (NULL if error)
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

CAlertTarget*   CAlertSmtpPropertyPage::GetNewAlertTarget(void)
{
    auto    CEMailTarget*       newAlertTarget;

    try {
        newAlertTarget = new  CEMailTarget;
        }

    catch(CMemoryException)
        {
        newAlertTarget = NULL;
        throw;      // forward the exception object to the next handler
        }

    return  (newAlertTarget);

}  // end of "CAlertSmtpPropertyPage::GetNewAlertTarget"



// ===== InitializeData ===================================================
//
//  This function sets up the property page's data.
//
//  Input:
//      nothing
//
//  Output:
//      a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

BOOL    CAlertSmtpPropertyPage::InitializeData(void)
{
    auto    BOOL        result;

    // set the alert type
    m_alertType = TARGET_EMAIL;

    // setup the context help map pointers
    m_pHelpWhatsThisMap = dwSmtpWhatsThisMap;
    m_pHelpHowToMap = dwSmtpHowToMap;

    // initialize the "how to" index for this dialog
    SetHowToMenuIndex(HOW_TO_INDEX_SMTP);

    // initialize some control IDs
    m_listCtrlContextMenuID = IDR_RMENU_SMTPPAGE_TARGET;
    m_listCtrlWhatsThisMenuID = IDR_WHATS_THIS_SMTP_LISTCTRL;
    m_listCtrlWhatsThisHelpID = IDH_NAVNT_WHATHIS_MESSERV_TARGETS_LSTBX;
    m_listCtrlID = IDC_SMTP_TARGET_LIST;

    // initialize pointers to the controls
    m_listCtrlPtr = &m_listCtrl;
    m_addButtonPtr = &m_addButton;
    m_modifyButtonPtr = &m_modifyButton;
    m_removeButtonPtr = &m_removeButton;

    auto    CString     tempStr1;
    auto    CString     tempStr2;
    if  ((tempStr1.LoadString(IDS_COLUMN_HEADER_NAME))
    &&  (tempStr2.LoadString(IDS_COLUMN_HEADER_ADDRESS)))
        {
        m_totalDisplayStrings = 2;
        m_columnHeaders.SetSize(m_totalDisplayStrings);
        m_columnHeaders[0] = tempStr1;
        m_columnHeaders[1] = tempStr2;
        }
    else
        {
        m_totalDisplayStrings = 0;
        result = FALSE;
        }
    return (result);

}  // end of "CAlertSmtpPropertyPage::InitializeData"



// ===== TargetIsActive ===================================================
//
//  This function returns the "active" state of the input alert target.
//
//  NOTE: this is an implementation of the virtual function inherited from
//        the CAlertListPropertyPage class
//
//  Input:
//      pAlertTarget    -- a pointer to the alert target record
//
//  Output:
//      a value of TRUE if the target is active, FALSE if it is not
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

BOOL    CAlertSmtpPropertyPage::TargetIsActive(CAlertTarget*  pAlertTarget)
{
    auto    CEMailTarget*       targetPtr;
    targetPtr = (CEMailTarget*)pAlertTarget;
    return  (targetPtr->GetActive());

}  // end of "CAlertSmtpPropertyPage::TargetIsActive"

















/////////////////////////////////////////////////////////////////////////////
// CAlertSmtpPropertyPage message handlers
/////////////////////////////////////////////////////////////////////////////


// ===== OnInitDialog =====================================================
//
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

BOOL CAlertSmtpPropertyPage::OnInitDialog(void)
{
    CAlertEmailPropertyPage::OnInitDialog();

    // TODO: Add extra initialization here

    // set the page's caption
    auto    CString     tempStr;
    if (tempStr.LoadString(IDS_TARGET_PAGE_CAPTION))
        {
        SetWindowText(tempStr);
        }

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CAlertSmtpPropertyPage::OnInitDialog"



// ===== OnClickSmtpTargetList ============================================
//
//  This function handles the event where the user clicks on the listbox
//  control.  If an item's checkbox is hit, then its selected state is toggled.
//
//  Input:  the default MFC input arguments
//  Output: nothing
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

void    CAlertSmtpPropertyPage::OnClickSmtpTargetList(NMHDR* pNMHDR
                                                            , LRESULT* pResult)
{
	// TODO: Add your control notification handler code here

    CAlertListPropertyPage::OnClickAlertsTargetsList(pNMHDR, pResult);
	*pResult = 0;

}  // end of "CAlertSmtpPropertyPage::OnClickSmtpTargetList"



// ===== OnDblclkSmtpTargetList ===========================================
//
//  This function handles a double-click on the list control.
//
//  Input:  the default MFC input arguments
//  Output: nothing
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

void    CAlertSmtpPropertyPage::OnDblclkSmtpTargetList(NMHDR* pNMHDR
                                                            , LRESULT* pResult)
{
	// TODO: Add your control notification handler code here

    CAlertListPropertyPage::OnDblclkAlertsTargetsList(pNMHDR, pResult);
	*pResult = 0;

}  // end of "CAlertSmtpPropertyPage::OnDblclkSmtpTargetList"











/////////////////////////////////////////////////////////////////////////////
// CSmtpOptPpg property page
/////////////////////////////////////////////////////////////////////////////



// the "what's this" and "how to" maps
DWORD   CSmtpOptPpg::m_dwWhatsThisMap[] =
{
    IDC_STATIC_SMTP_SERVER_NAME, IDH_NAVNT_WHATHIS_SERVER_NAME_TXTBX
    , IDC_STATIC_MESSAGE_SUBJECT, IDH_NAVNT_WHATHIS_MESSAGE_SUBJECT_TXTBX
    , IDC_STATIC_MESSAGE_FROM, IDH_NAVNT_WHATHIS_MESSAGE_FROM_TXTBX
    , IDC_STATIC_SMTP_SERVER_SETTINGS, IDH_NAVNT_WHATHIS_SMTP_SERVER_SETTINGS_GRPBX
    , IDC_STATIC_GLOBAL_MAIL_SETTINGS, IDH_NAVNT_WHATHIS_GLOBAL_MAIL_SETTINGS_GRPBX
    , 0,0
};


DWORD   CSmtpOptPpg::m_dwHowToMap[] =
{
    ID_HELP, IDH_NAVNT_EMAIL_SETTINGS_DLG_HELP_BTN
    , ID_HT_DEFINE_SERVER_AND_MESSAGE_SMTP, IDH_NAVNT_TASK_DEFINE_THE_SMTP_SERVER_AND_MESSAGE_FIELDS
    , ID_HT_ADD_SMTP, IDH_NAVNT_TASK_ADD_AN_EMAIL_ALERT_TARGET
    , ID_HT_MODIFY_SMTP, IDH_NAVNT_TASK_EDIT_A_SINGLE_EMAIL_TARGET
    , ID_HT_EDITLIST_SMTP, IDH_NAVNT_TASK_EDIT_THE_EMAIL_TARGET_LIST
    , ID_HT_REMOVE_SMTP, IDH_NAVNT_TASK_REMOVE_AN_EMAIL_TARGET
    , 0,0
};


IMPLEMENT_DYNCREATE(CSmtpOptPpg, CHelpPropertyPage)

CSmtpOptPpg::CSmtpOptPpg() : CHelpPropertyPage(CSmtpOptPpg::IDD)
{
	//{{AFX_DATA_INIT(CSmtpOptPpg)
	m_szSmtpServerName = _T("");
	m_szSmtpAuthorName = _T("");
	m_szSmtpSubjectText = _T("");
	//}}AFX_DATA_INIT

    // set up the help map pointers
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_pHelpHowToMap = m_dwHowToMap;

    // initialize the "how to" index for this dialog
    SetHowToMenuIndex(HOW_TO_INDEX_SMTP);
}


CSmtpOptPpg::~CSmtpOptPpg()
{
}



void CSmtpOptPpg::DoDataExchange(CDataExchange* pDX)
{
    CHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSmtpOptPpg)
	DDX_Control(pDX, IDC_STATIC_MESSAGE_FROM, m_staticMessageFrom);
	DDX_Control(pDX, IDC_STATIC_MESSAGE_SUBJECT, m_staticMessageSubject);
	DDX_Control(pDX, IDC_STATIC_SMTP_SERVER_NAME, m_staticServerName);
	DDX_Text(pDX, IDC_SMTP_SERVER_EDIT, m_szSmtpServerName);
	DDX_Text(pDX, IDC_EMAIL_FROMNAME_EDIT, m_szSmtpAuthorName);
	DDX_Text(pDX, IDC_EMAIL_SUBJECT_EDIT, m_szSmtpSubjectText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSmtpOptPpg, CHelpPropertyPage)
	//{{AFX_MSG_MAP(CSmtpOptPpg)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()





/////////////////////////////////////////////////////////////////////////////
// CSmtpOptPpg message handlers



// ==== OnCommand =========================================================
//
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

BOOL    CSmtpOptPpg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

    // if the user clicked on a control, store a pointer to its window in the
    // current context variable so it'll be ready for context help
    if (lParam > 0)
        {
        m_pCurrentContextWnd = GetDlgItem(LOWORD(wParam));
        }

    return CHelpPropertyPage::OnCommand(wParam, lParam);

}  // end of "CSmtpOptPpg::OnCommand"



// ==== OnContextMenu =====================================================
//
//
// ========================================================================
//  Function created: 2/97, SEDWARD
// ========================================================================

void    CSmtpOptPpg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here

    // if the CWnd pointer equals "this" (possibly because the original
    // control is grayed-out), adjust the "pWnd" pointer if necessary
    auto    CWnd*       tempWndPtr;
    if (pWnd == this)
        {
        tempWndPtr = CheckGroupboxControl(&point, m_groupboxControls);
        if (NULL != tempWndPtr)
            {
            pWnd = tempWndPtr;
            }
        }

    CHelpPropertyPage::OnContextMenu(pWnd, point);

}  // end of "CSmtpOptPpg::OnContextMenu"



// ===== OnInitDialog =====================================================
//
//  Performs data exchange with the property page's edit controls,
//  modifying the global alert options.
//
//  Input:  nothing
//  Output: TRUE
//
// ========================================================================
//  Function created: 01/97, TSMITH
// ========================================================================
BOOL CSmtpOptPpg::OnInitDialog()
{
    // let the parent property sheet know that this property page has been
    // initialized
    auto    CEmailPropertySheet*    parentWndPtr;
    parentWndPtr = (CEmailPropertySheet*)GetParent();
    if (NULL != parentWndPtr)
        {
        parentWndPtr->SetSettingsDialogFlag();
        }

    CHelpPropertyPage::OnInitDialog();

    // initialize the groupbox controls array
    m_groupboxControls[0] = &m_staticMessageFrom;
    m_groupboxControls[1] = &m_staticMessageSubject;
    m_groupboxControls[2] = &m_staticServerName;
    m_groupboxControls[3] = NULL;

    m_szSmtpServerName = gAlertOptions.GetSMTPPostOffice( );
    m_szSmtpSubjectText = gAlertOptions.GetSMTPSubject( );
    m_szSmtpAuthorName = gAlertOptions.GetSMTPAuthor( );
    UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// ===== OnOK =============================================================
//
//  Populates the edit controls on the SMTP mail settings property page
//  from data in the global alert options.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 01/97, TSMITH
// ========================================================================
void CSmtpOptPpg::OnOK()
{
    UpdateData( TRUE );
    gAlertOptions.SetSMTPPostOffice( m_szSmtpServerName );
    gAlertOptions.SetSMTPSubject( m_szSmtpSubjectText );
    gAlertOptions.SetSMTPAuthor( m_szSmtpAuthorName );
    CHelpPropertyPage::OnOK();
}


