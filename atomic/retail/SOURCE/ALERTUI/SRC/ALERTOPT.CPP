/////////////////////////////////////////////////////////////////////
// AlertOpt.cpp : implementation file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ALERTOPT.CPv   1.11   12 Nov 1997 15:10:40   TSmith  $
//
// Description:
//
//  This file contains the code for the main alert options dialog that is
//  displayed in the "Alerts" tab in the NAV options.  It allows the user
//  to graphically view the current alert targets from a MFC tree control,
//  as well as add, modify or remove alert targets.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ALERTOPT.CPv  $
//
//    Rev 1.11   12 Nov 1997 15:10:40   TSmith
// Removed obsolete include of UniUtil.h
//
//    Rev 1.10   05 Nov 1997 15:33:32   TSMITH
// Changed usage of NQ_SetFontsInDialog to NG_SetFontsInDialog to pick up DBCS
// font handling in SYMNAV.
//
//    Rev 1.9   17 Jul 1997 13:33:50   DBUCHES
// Added code to handle help button.
//
//    Rev 1.8   15 Jul 1997 18:22:50   DBUCHES
// Changed image list mask color
//
//    Rev 1.7   14 Jul 1997 16:39:08   DBUCHES
// Added help topics.
//
//    Rev 1.6   10 Jun 1997 20:11:50   DBUCHES
// Added password support.
//
//    Rev 1.5   04 Jun 1997 21:32:18   DBUCHES
// Added OnHandleSNMPEdit().
//
//    Rev 1.4   04 Jun 1997 21:14:46   DBUCHES
// Changed to support SNMP Alerts.
//
//    Rev 1.3   27 May 1997 20:33:00   DBUCHES
// Save options on GetData() call.
//
//    Rev 1.2   27 May 1997 20:14:06   DBUCHES
// Changes to support integration with N32OPTS.
//
//    Rev 1.1   25 Apr 1997 11:03:18   JBRENNA
// Ported from Tesla.
//
//    Rev 1.1   15 Apr 1997 20:01:34   sedward
// Uses new CAlert property page classes.
//
//    Rev 1.38   05 Feb 1997 23:04:26   SEDWARD
// Modified 'OnContextMenu', added 'm_alertTree' to 'm_groupboxControls' array.
//
//    Rev 1.37   03 Feb 1997 18:20:44   SEDWARD
// Replaced 'NQFont' stuff with 'UniUtil' stuff.
//
//    Rev 1.36   02 Feb 1997 22:05:58   SEDWARD
// Fix for bugs #80418 and #80417.
//
//    Rev 1.35   02 Feb 1997 16:34:54   SEDWARD
// Added support for context menus that are grayed out AND in groupbox controls.
//
//    Rev 1.34   30 Jan 1997 15:11:58   SEDWARD
// Use the 'CNoContextPropertySheet' class for property sheets so that the lack
// of context menus on the OK/Cancel/Help buttons is consistent with the
// behavior of the Norton Control Panel.
//
//    Rev 1.33   28 Jan 1997 20:01:08   SEDWARD
// InitControls is now called by the corresponding export function.
//
//    Rev 1.32   27 Jan 1997 21:39:24   SEDWARD
// Allow removal of the 'master NLM item' from the main tree control.
//
//    Rev 1.30   27 Jan 1997 15:30:22   SEDWARD
// Removed 'AdjustPopupMenuItems' function.
//
//    Rev 1.29   22 Jan 1997 16:55:48   SEDWARD
// Added 'OnSelChanged' function (fix for #79159).
//
//    Rev 1.28   21 Jan 1997 18:38:10   SEDWARD
// Enhanced tree control clicks.
//
//    Rev 1.26   21 Jan 1997 13:42:40   SEDWARD
// Right-menu clicks now make the item selected in the tree control.
//
//    Rev 1.25   20 Jan 1997 21:21:50   SEDWARD
// Now the "Add" button is always active for parent branches.
//
//    Rev 1.24   20 Jan 1997 00:17:42   SEDWARD
//
//    Rev 1.22   16 Jan 1997 02:13:44   SEDWARD
// Lots of updates in help maps and menus.
//
//    Rev 1.21   14 Jan 1997 20:36:42   SEDWARD
// New help ID values.
//
//    Rev 1.20   14 Jan 1997 13:21:36   SEDWARD
// Updated help maps with new IDs, nuked the "Apply" button.
//
//    Rev 1.18   07 Jan 1997 18:56:40   TSmith
// Added SMTP mail property page instantiation to
// 'CAlertOptsDlg::HandleEmailListEdit'.
//
//    Rev 1.17   07 Jan 1997 15:06:36   SEDWARD
// Changed handling the enabling/disabling of the edit buttons when a parent
// item is hilited.  If the parent has child items underneath it, then only the
// "Modify" button is enabled; if the parent has no children, then both the
// "Add" and "Modify" buttons are enabled.
//
//    Rev 1.13   09 Dec 1996 20:47:44   SEDWARD
//
//    Rev 1.11   04 Dec 1996 16:16:48   SEDWARD
// Enabled the Remove right-menu item from the main tree control.
//
//    Rev 1.10   03 Dec 1996 21:07:38   SEDWARD
// Added Net Messenger icon for main tree control.
//
//    Rev 1.7   02 Dec 1996 16:33:32   SEDWARD
// More big changes, nearing UI freeze.
//
//    Rev 1.6   27 Nov 1996 19:00:40   SEDWARD
// Removed int 3h...
//
//    Rev 1.5   27 Nov 1996 18:49:32   SEDWARD
// Hooked up NAVOPTS to the local alert message configuration dialog.
//
//    Rev 1.4   26 Nov 1996 17:21:52   SEDWARD
// Leading backslashes no longer written to DAT file, domains get a trailing
// space/asterisk appened to them.
//
//    Rev 1.3   26 Nov 1996 00:56:32   SEDWARD
// More big changes.
//
//    Rev 1.2   25 Nov 1996 15:46:40   SEDWARD
// Major overhaul (more to come).
//
//    Rev 1.1   21 Nov 1996 17:07:44   SEDWARD
// Added vlog headers.
//
//    Rev 1.0   21 Nov 1996 16:55:06   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////




#include "stdafx.h"
#include <tchar.h>
#include "afxtempl.h"
#include "AlertUI.h"
#include "AlertOpt.h"
#include "winnetwk.h"
#include "AlertMes.h"
#include "EmailPSh.h"
#include "NoCtxtPS.h"               // no context property sheet

#include "at_opts.h"                // for CAlertOptions class
#include "nvetwhlp.h"               // context help
#include "navutil.h"                // context help
#include "helpdlg.h"                // base class for default help support
#include "help_ids.h"               // help IDs

#include "ATForwPg.h"               // new CAlertPropertyPage classes
#include "ATMessPg.h"
#include "ATNlmPg.h"
#include "ATSmtpPg.h"
#include "snmpage.h"


#include "dllinst.h"
#include "ngui.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// the "what's this" and "how to" maps
DWORD   CAlertOptsDlg::m_dwWhatsThisMap[] =
{
    IDC_ALERTS_GROUPBOX, IDH_NAVNT_WHATHIS_CONFIG_ALERT_TARGETS_GRPBX,
    IDC_ADD_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_ADD_BTN,
    IDC_REMOVE_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_REMOVE_BTN,
    IDC_MODIFY_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_MODIFY_BTN,
    IDC_ALERT_TREE, IDH_NAVNT_WHATHIS_ALERT_TARGETS_LSTBX,
    IDC_ENABLE_GROUPBOX, IDH_NAVNT_WHATHIS_SEND_ALERTS_GENERATED_BY_GRPBX,
    IDC_AUTO_PROTECT_CHECKBOX, IDH_NAVNT_WHATHIS_AUTOPROTECT_CHKBX,
    IDC_SCANNER_CHECKBOX, IDH_NAVNT_WHATHIS_WIN_SCANNER_CHKBX,
    IDC_SAM_CHECK, IDH_SAM_VIA_APPLETALK_CHKBX,
    IDC_IPX_CHECK, IDH_NAV_VIA_IPX_CHKBX,
    0,0
};


DWORD   CAlertOptsDlg::m_dwWhatsThisGrayMap[] =
{
        0,0
};


DWORD   CAlertOptsDlg::m_dwHowToMap[] =
{
    ID_HELP, IDH_NAVNT_CONFIG_ALERTS_HELP_BTN
    , ID_HT_SETUP_ALERT_TARGETS, IDH_NAVNT_TASK_SET_UP_ALERT_TARGETS

    // howto SMTP
    , ID_HT_DEFINE_SERVER_AND_MESSAGE_SMTP, IDH_NAVNT_TASK_DEFINE_THE_SMTP_SERVER_AND_MESSAGE_FIELDS
    , ID_HT_ADD_SMTP, IDH_NAVNT_TASK_ADD_AN_EMAIL_ALERT_TARGET
    , ID_HT_MODIFY_SMTP, IDH_NAVNT_TASK_EDIT_A_SINGLE_EMAIL_TARGET
    , ID_HT_EDITLIST_SMTP, IDH_NAVNT_TASK_EDIT_THE_EMAIL_TARGET_LIST
    , ID_HT_REMOVE_SMTP, IDH_NAVNT_TASK_REMOVE_AN_EMAIL_TARGET

    // howto NLM
    , ID_HT_ADD_NLM, IDH_NAVNT_TASK_ADD_A_NAV_FOR_NETWARE_ALERT_TARGET
    , ID_HT_INCLUDE_ALL_NLM, IDH_NAVNT_TASK_ADD_ALL_NAV_FOR_NETWARE_SERVERS_AS_TARGETS
    , ID_HT_MODIFY_NLM, IDH_NAVNT_TASK_EDIT_A_SINGLE_NAV_FOR_NETWARE_SERVER_TARGET
    , ID_HT_EDITLIST_NLM, IDH_NAVNT_TASK_EDIT_THE_NAV_FOR_NETWARE_TARGET_LIST
    , ID_HT_REMOVE_NLM, IDH_NAVNT_TASK_REMOVE_A_NAV_FOR_NETWARE_ALERT_TARGET

    // howto net messenger
    , ID_HT_ADD_NETMESSENGER, IDH_NAVNT_TASK_ADD_A_NT_MESSENGER_SERVICE_TARGET
    , ID_HT_REMOVE_NETMESSENGER, IDH_NAVNT_TASK_REMOVE_AN_NT_MESSENGER_SERVICE_TARGET
    , ID_HT_EDITLIST_NETMESSENGER, IDH_NAVNT_TASK_EDIT_THE_NT_MESSENGER_SERVICE_TARGET_LIST
    , ID_HT_MODIFY_NETMESSENGER, IDH_NAVNT_TASK_EDIT_A_SINGLE_NT_MESSENGER_SERVICE_TARGET

    // howto forward targets
    , ID_HT_ADD_FORWARD, IDH_NAVNT_TASK_ADD_A_RELAY_TO_REMOTE_MACHINE_TARGET
    , ID_HT_MODIFY_FORWARD, IDH_NAVNT_TASK_EDIT_A_SINGLE_RELAY_TO_REMOTE_MACHINE_TARGET
    , ID_HT_EDITLIST_FORWARD, IDH_NAVNT_TASK_EDIT_THE_RELAY_TO_REMOTE_MACHINES_TARGET_LIST
    , ID_HT_REMOVE_FORWARD, IDH_NAVNT_TASK_REMOVE_A_RELAY_TO_REMOTE_MACHINES_TARGET

    // local machine
    , ID_HT_CREATE_ALERT_MESSAGE, IDH_NAVNT_TASK_CREATE_A_VIRUS_ALERT_MESSAGE
    , 0,0
};

// Helper functions.
static BOOL CALLBACK EnumProc( HWND hwnd, LPARAM lParam );
#define WM_PASSWORD_CHECK       (WM_USER + 5)

/////////////////////////////////////////////////////////////////////////////
// CAlertOptsDlg dialog
/////////////////////////////////////////////////////////////////////////////

// this is the MFC-generated constructor (not used by navopts)
CAlertOptsDlg::CAlertOptsDlg() : CHelpPropertyPage(CAlertOptsDlg::IDD)
{
    //{{AFX_DATA_INIT(CAlertOptsDlg)
        m_IPXCheckBox = FALSE;
        m_SAMCheckBox = FALSE;
        //}}AFX_DATA_INIT
        // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
        // m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    // create the image list object for the tree control
    m_imageList.Create(IDB_ALERT_TREE, 16, 1, RGB(255, 0, 255));

    // set everything else to known values
    m_alertType = NULL;
    m_hIcon = NULL;
    m_localMachineTreeHandle = NULL;
    m_netMessageTreeHandle = NULL;
    m_SMTPTreeHandle = NULL;
    m_nlmTreeHandle = NULL;
    m_forwardTreeHandle = NULL;
    m_rightClickHandle = NULL;

    // set up the help map pointers
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
//    m_pHelpWhatsThisGrayMap = m_dwWhatsThisGrayMap;
    m_pHelpHowToMap = m_dwHowToMap;

    // initialize the "how to" index for this dialog
    SetHowToMenuIndex(HOW_TO_INDEX_ALERT_TARGETS);
    m_bPassDisabled = FALSE;

        // Enable help button
        m_psp.dwFlags |= PSP_HASHELP;

}  // end of constructor



// this constructor allows us to pass the handle argument to the create
// dialog function so we can create the parent/child relationship
CAlertOptsDlg::CAlertOptsDlg(HWND  parentHandle)
    : CHelpPropertyPage(CAlertOptsDlg::IDD)
{
    // create the image list object for the tree control
    m_imageList.Create(IDB_ALERT_TREE, 16, 1, RGB(255, 0, 255));

    // set everything else to known values
    m_alertType = NULL;
    m_hIcon = NULL;
    m_localMachineTreeHandle = NULL;
    m_netMessageTreeHandle = NULL;
    m_SMTPTreeHandle = NULL;
    m_nlmTreeHandle = NULL;
    m_forwardTreeHandle = NULL;
    m_rightClickHandle = NULL;

    // set up the help map pointers
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
//    m_pHelpWhatsThisGrayMap = m_dwWhatsThisGrayMap;
    m_pHelpHowToMap = m_dwHowToMap;

    // initialize the "how to" index for this dialog
    SetHowToMenuIndex(HOW_TO_INDEX_ALERT_TARGETS);

    // create the dialog
    CreateDlg(parentHandle);
    m_bPassDisabled = FALSE;

        // Enable help button
        m_psp.dwFlags |= PSP_HASHELP;


}  // end of constructor


// Destructor
CAlertOptsDlg::~CAlertOptsDlg()
{
}

void CAlertOptsDlg::DoDataExchange(CDataExchange* pDX)
{
    CHelpPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAlertOptsDlg)
        DDX_Control(pDX, IDC_ADD_BUTTON, m_addButton);
        DDX_Control(pDX, IDC_MODIFY_BUTTON, m_modifyButton);
        DDX_Control(pDX, IDC_REMOVE_BUTTON, m_removeButton);
        DDX_Control(pDX, IDC_SCANNER_CHECKBOX, m_scannerCheckbox);
        DDX_Control(pDX, IDC_AUTO_PROTECT_CHECKBOX, m_apCheckbox);
        DDX_Control(pDX, IDC_ALERT_TREE, m_alertTree);
        DDX_Check(pDX, IDC_IPX_CHECK, m_IPXCheckBox);
        DDX_Check(pDX, IDC_SAM_CHECK, m_SAMCheckBox);
        //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAlertOptsDlg, CHelpPropertyPage)
    //{{AFX_MSG_MAP(CAlertOptsDlg)
        ON_WM_SYSCOMMAND()
        ON_WM_PAINT()
        ON_WM_QUERYDRAGICON()
        ON_NOTIFY(NM_CLICK, IDC_ALERT_TREE, OnClickAlertTree)
    ON_COMMAND(ID_EDITSMTPTARGETS, OnHandleEmailListEdit)
    ON_COMMAND(ID_ADD_FORWARD_TARGET, OnHandleForwardAdd)
    ON_COMMAND(ID_ADD_SMTP_TARGET, OnHandleSMTPAdd)
    ON_COMMAND(ID_ADD_NETMESSAGE_TARGET, OnHandleNetMessageAdd)
    ON_COMMAND(ID_EDITNETMESSAGETARGETS, OnHandleNetMessageListEdit)
    ON_COMMAND(ID_ADD_NLM_TARGET, OnHandleNLMAdd)
    ON_COMMAND(ID_EDITNLMTARGETS, OnHandleNLMListEdit)
    ON_COMMAND(ID_EDITFORWARDTARGETS, OnHandleForwardListEdit)
    ON_COMMAND(ID_EDITLOCALMACHINE, OnHandleLocalMachineEdit)
        ON_NOTIFY(NM_DBLCLK, IDC_ALERT_TREE, OnDblclkAlertTree)
    ON_COMMAND(ID_MODIFY_FORWARD_TARGET, OnRmenuForwardModify)
    ON_COMMAND(ID_MODIFY_NETMESSAGE_TARGET, OnRmenuNetMessageModify)
    ON_COMMAND(ID_MODIFY_SMTP_TARGET, OnRmenuSMTPModify)
    ON_COMMAND(ID_MODIFY_NLM_TARGET, OnRmenuNLMModify)
    ON_COMMAND(ID_REMOVE_FORWARD_TARGET, OnRmenuRemoveTreeChild)
        ON_BN_CLICKED(IDC_SCANNER_CHECKBOX, OnScannerCheckbox)
        ON_BN_CLICKED(IDC_AUTO_PROTECT_CHECKBOX, OnAutoProtectCheckbox)
        ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
        ON_BN_CLICKED(IDC_MODIFY_BUTTON, OnModifyButton)
        ON_BN_CLICKED(IDC_REMOVE_BUTTON, OnRemoveButton)
        ON_WM_CONTEXTMENU()
    ON_COMMAND(IDR_WHATS_THIS_ALERT_TREE, OnWhatsThisAlertTree)
        ON_NOTIFY(TVN_SELCHANGED, IDC_ALERT_TREE, OnSelchangedAlertTree)
    ON_COMMAND(ID_MODIFY_SNMP_TARGET, OnRmenuSNMPModify)
        ON_BN_CLICKED(IDC_IPX_CHECK, OnIpxCheck)
    ON_COMMAND(ID_REMOVE_NETMESSAGETARGET, OnRmenuRemoveTreeChild)
    ON_COMMAND(ID_REMOVE_SMTP_TARGET, OnRmenuRemoveTreeChild)
    ON_COMMAND(ID_REMOVE_NLM_TARGET, OnRmenuRemoveTreeChild)
    ON_COMMAND(ID_ADD_SNMP_TARGET, OnHandleSNMPAdd)
    ON_COMMAND(ID_REMOVE_SNMP_TARGET, OnRmenuRemoveTreeChild)
    ON_COMMAND(ID_EDITSNMPTARGETS, OnHandleSNMPListEdit)
        ON_BN_CLICKED(IDC_SAM_CHECK, OnSamCheck)
        ON_MESSAGE(WM_PASSWORD_CHECK, OnPasswordCheck )
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()






/////////////////////////////////////////////////////////////////////////////
//
// CAlertOptsDlg member functions
//
/////////////////////////////////////////////////////////////////////////////




// ===== AddEmailTargetBranch =============================================
//
//  This function adds a new item to the email branch.
//
//  Input:
//      parentHandle    -- tree handle to the parent email branch
//      recPtr          -- record to be added
//
//  Output: the handle of the new child item, NULL otherwise
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

HTREEITEM   CAlertOptsDlg::AddEmailTargetBranch(HTREEITEM  parentHandle
                                                    , CAlertTarget*  recPtr)
{
    auto    CEMailTarget*   emailRecPtr = (CEMailTarget*)recPtr;
    auto    CString         displayStr;
    auto    HTREEITEM       childHandle;
    auto    TV_ITEM         treeItemInfo;

    // get a formatted string appropriate for display
    GetEmailDisplayStr(emailRecPtr, displayStr);

    // insert the new tree item
    childHandle = m_alertTree.InsertItem(displayStr, parentHandle);
    if  (NULL != childHandle)
        {
        memset((void*)&treeItemInfo, 0, sizeof(TV_ITEM));
        treeItemInfo.hItem = childHandle;
        treeItemInfo.mask = TVIF_SELECTEDIMAGE | TVIF_IMAGE | TVIF_PARAM;
        if (TRUE == emailRecPtr->GetActive())
            {
            treeItemInfo.iSelectedImage = ATC_CHECKED_BITMAP;
            treeItemInfo.iImage = ATC_CHECKED_BITMAP;
            }
        else
            {
            treeItemInfo.iSelectedImage = ATC_UNCHECKED_BITMAP;
            treeItemInfo.iImage = ATC_UNCHECKED_BITMAP;
            }

        // save a pointer to the target data structure
        treeItemInfo.lParam = (DWORD)recPtr;
        m_alertTree.SetItem(&treeItemInfo);
        }

    return (childHandle);

}  // end of "CAlertOptsDlg::AddEmailTargetBranch"



// ==== AddMachineTargetBranch ============================================
//
//  This function adds a new "machine" branch to the alert targets tree
//  control.  The tree handle argument determines under which branch the new
//  item will be inserted.
//
//  Input:
//      parentHandle    -- the parent tree handle
//      pAlertTarget    -- a pointer to a CAlertTarget object
//
//  Output: the handle of the new child item, NULL otherwise
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

HTREEITEM   CAlertOptsDlg::AddMachineTargetBranch(HTREEITEM  parentHandle
                                            , CAlertTarget*  pAlertTarget)
{
    auto    BOOL                validType = TRUE;
    auto    CString             tempStr;
    auto    CForwardTarget*     forwardPtr;
    auto    CNetMsgTarget*      netMessPtr;
    auto    CNLMTarget*         nlmPtr;
    auto    CSNMPTarget*        snmpTargetPtr;
    auto    DWORD               dwTargetResourceType = RESOURCE_TYPE_SERVER;
    auto    HTREEITEM           childHandle = NULL;
    auto    TV_ITEM             treeItemInfo;

    // get a pointer of the correct type to the input argument and set the
    // display string
    switch (pAlertTarget->GetTargetType())
        {
        case TARGET_MESSAGE_NETWORK:
            netMessPtr = (CNetMsgTarget*)pAlertTarget;
            tempStr = netMessPtr->GetAddress();
            dwTargetResourceType = netMessPtr->GetResourceType();
            break;

        case TARGET_NLM:
            nlmPtr = (CNLMTarget*)pAlertTarget;
            tempStr = nlmPtr->GetAddress();
            break;

        case TARGET_FORWARD:
            forwardPtr = (CForwardTarget*)pAlertTarget;
            tempStr = forwardPtr->GetAddress();
            break;

        case TARGET_SNMP:
            snmpTargetPtr = (CSNMPTarget*)pAlertTarget;
            tempStr = snmpTargetPtr->GetName();
            // Need to set to anything *BUT* RESOURCE_TYPE_SERVER
            dwTargetResourceType++;
            break;

        default:
            validType = FALSE;
            break;
        }

    // only do special string processing if this target is a server
    if (RESOURCE_TYPE_SERVER == dwTargetResourceType)
        {
        // if the machine string is not a domain, insert double-backslashes
        // (for display purposes only)
        if  ((FALSE == IsDomainString(tempStr))
        &&  (MASTER_NLM_ITEM_STR != tempStr))
            {
            InsertServerBackslashes(tempStr);
            }
        }

    // actually attempt to insert the new item
    if (TRUE == validType)
        {
        childHandle = m_alertTree.InsertItem(tempStr, parentHandle);
        }

    // if all has gone well up to this point, set the checkbox and save
    // the address of the alert object
    if  (NULL != childHandle)
        {
        memset((void*)&treeItemInfo, 0, sizeof(TV_ITEM));
        treeItemInfo.hItem = childHandle;
        treeItemInfo.mask = TVIF_SELECTEDIMAGE | TVIF_IMAGE | TVIF_PARAM;

        // set the checkbox for display
        if (pAlertTarget->GetActive())
            {
            treeItemInfo.iSelectedImage = ATC_CHECKED_BITMAP;
            treeItemInfo.iImage = ATC_CHECKED_BITMAP;
            }
        else
            {
            treeItemInfo.iSelectedImage = ATC_UNCHECKED_BITMAP;
            treeItemInfo.iImage = ATC_UNCHECKED_BITMAP;
            }

        // save a pointer to the target data structure
        treeItemInfo.lParam = (DWORD)pAlertTarget;
        m_alertTree.SetItem(&treeItemInfo);
        }

    return (childHandle);

}  // end of "CAlertOptsDlg::AddMachineTargetBranch"



// === CreateDlg ==========================================================
//
//  This function contains the call to create the dialog.  It uses the
//  HWND argument to create a parent/child relationship between the dialog
//  and the HWND.  (NOTE: This was done so we could use an MFC dialog in
//  the Norton Control Panel.  After many different approaches on how to
//  get an MFC dialog to behave in the NCP/Quake model, the easiest and
//  most direct was to provide a blank dialog template for Quake, then use
//  that dialog's handle as a parent window.  This way, visual clipping
//  is handled automatically, and all messages get routed to the correct
//  dialog).
//
//  Input:  parentHandle    -- the parent window's HWND
//
//  Output: the result of the MFC "Create" function
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::CreateDlg(HWND  parentHandle)
{
    static  CWnd*       parentWndPtr;
    parentWndPtr = FromHandle(parentHandle);

    // create the alert options dialog with a parent relationship
    return (Create(IDD_ALERT_OPTS, parentWndPtr));

}  // end of "CAlertOptsDlg::CreateDlg"



// ==== EnableEditButtons =================================================
//
//  This function enables or disables the main dialog edit buttons based on
//  the value in the flags argument.
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::EnableEditButtons(DWORD  flags)
{
    if (flags & ENABLE_ADD_BUTTON)
        {
        m_addButton.EnableWindow(TRUE);
        }
    else
        {
        m_addButton.EnableWindow(FALSE);
        }

    if (flags & ENABLE_MODIFY_BUTTON)
        {
        m_modifyButton.EnableWindow(TRUE);
        }
    else
        {
        m_modifyButton.EnableWindow(FALSE);
        }

    if (flags & ENABLE_REMOVE_BUTTON)
        {
        m_removeButton.EnableWindow(TRUE);
        }
    else
        {
        m_removeButton.EnableWindow(FALSE);
        }

}  // end of "CAlertOptsDlg::EnableEditButtons"



// ==== GetChildTreeHandle ================================================
//
//  This function attempts to find a child tree handle whose text matches
//  that of the input text argument.
//
//  Input:
//      parentHandle    -- the parent tree handle whose children to search
//      childStr        -- the target child item text
//
//  Output:
//      If a matching child item is found, its corresponding tree handle is
//      returned; else, a value of NULL is returned.
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

HTREEITEM   CAlertOptsDlg::GetChildTreeHandle(HTREEITEM  parentHandle
                                                        , CString  childStr)
{
    auto    BOOL            notFound = TRUE;
    auto    HTREEITEM       childHandle = NULL;
    auto    HTREEITEM       tempHandle;

    if (m_alertTree.ItemHasChildren(parentHandle))
        {
        tempHandle = m_alertTree.GetChildItem(parentHandle);
        if (NULL != tempHandle)
            {
            do  {
                auto    CString     tempStr;
                tempStr = m_alertTree.GetItemText(tempHandle);
                if (childStr == tempStr)
                    {
                    // save the return value, break out of the loop
                    childHandle = tempHandle;
                    notFound = FALSE;
                    }
                else
                    {
                    // keep looping
                    tempHandle = m_alertTree.GetNextSiblingItem(tempHandle);
                    }

                }  while ((TRUE == notFound)  &&  (NULL != tempHandle));
            }
        }

    return (childHandle);

}  // end of "CAlertOptsDlg::GetChildTreeHandle"



// ==== GetHitTreeItemHandle ==============================================
//
//  This function is used to extract the handle to a tree item when the user
//  clicks on it.  Additionally, if the user supplies a flags argument it
//  will be initialized with the results of the tree's hit test function.
//
//  Input:  nothing
//
//  Output: the handle of the tree item that was hit, or NULL if none
//          were hit
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

HTREEITEM   CAlertOptsDlg::GetHitTreeItemHandle(UINT*  treeFlags)
{
    auto    BOOL        iconClicked = TRUE;
    auto    DWORD       dwPos;
    auto    HTREEITEM   treeItemHandle;
    auto    POINT       pointScreen;
    auto    POINT       pointTVClient;
    auto    UINT        localFlags;

    // get the screen coordinates
    dwPos = GetMessagePos();
        pointScreen.x = LOWORD (dwPos);
        pointScreen.y = HIWORD (dwPos);
    pointTVClient = pointScreen;

    // convert the point from screen to client coordinates, relative to
    // this control
    m_alertTree.ScreenToClient(&pointTVClient);

    // if any tree item was hit, return the handle; else, return NULL
    if (NULL == treeFlags)
        {
        treeItemHandle = m_alertTree.HitTest(pointTVClient, &localFlags);
        }
    else
        {
        treeItemHandle = m_alertTree.HitTest(pointTVClient, treeFlags);
        }

    return (treeItemHandle);

}  // end of "CAlertOptsDlg::GetHitTreeItemHandle"



// ===== GetRecMenuID =====================================================
//
//  This function derives a menu ID for the tree handle argument to be used
//  when a right-click is trapped.  The menu ID is determined by the type
//  of record the lparam associated with the tree handle.
//
//  Input:
//      treeHandle  -- the tree handle for which we want a menu ID
//
//  Output:
//      If the tree handle contains a valid record type, a corresponding
//      menu ID is returned.  Otherwise, an invalid value (-1) is returned.
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

int   CAlertOptsDlg::GetRecMenuID(HTREEITEM  treeHandle)
{
    auto  CAlertTarget*     recPtr;
    auto  int               menuID;

    // extract the pointer to the data record from the tree handle
    recPtr = (CAlertTarget*)m_alertTree.GetItemData(treeHandle);

    // based on the type of record we're dealing with, derive a menu ID
    switch (recPtr->GetTargetType())
        {
        case TARGET_MESSAGE_NETWORK:
            menuID = IDR_ATC_RMENU_NETMESSAGE_TARGET;
            break;

        case TARGET_NLM:
            menuID = IDR_ATC_RMENU_NLM_TARGET;
            break;

        case TARGET_FORWARD:
            menuID = IDR_ATC_RMENU_FORWARD_TARGET;
            break;

        case TARGET_EMAIL:
            menuID = IDR_ATC_RMENU_SMTP_TARGET;
            break;

        case TARGET_SNMP:
            menuID = IDR_ATC_RMENU_SNMP_TARGET;
            break;

        default:
            menuID = -1;
            break;
        }

    return (menuID);

}  // end of "CAlertOptsDlg::GetRecMenuID"



// ==== HandleAlertTypeEdit ===============================================
//
//  This function calls the appropriate alert type edit dialog, based on
//  the current value of the alert type member variable.
//
//  NOTE: this function assumes that the alert type member variable has
//        already been initialized to the desired target value.
//
//  Input:  nothing
//  Output: a value of TRUE if the editing of the target item reaches
//          successful completion, FALSE if not
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::HandleAlertTypeEdit(void)
{
    auto    BOOL        retVal = FALSE;

    if (NULL != m_alertType)
        {
        switch(m_alertType)
            {
            case  TARGET_LOCAL_MACHINE:
                retVal = HandleLocalMachineEdit();
                break;

            case  TARGET_MESSAGE_NETWORK:
                retVal = HandleNetMessageListEdit();
                break;

            case  TARGET_EMAIL:
                retVal = HandleEmailListEdit();
                break;

            case  TARGET_NLM:
                retVal = HandleNLMListEdit();
                break;

            case  TARGET_FORWARD:
                retVal = HandleForwardListEdit();
                break;

            case TARGET_SNMP:
                retVal = HandleSNMPListEdit();
                break;

            default:
                break;

            }  // end of switch
        }

    return (retVal);

}  // end of "CAlertOptsDlg::HandleAlertTypeEdit"



// ==== HandleChildCheckbox ===============================================
//
//  This function is called when the user clicks on a child item's checkbox.
//  Based on the child item's alert type, it calls the appropriate handling
//  function.
//
//  NOTE: The checkbox is only toggled if it is an "active" checbox
//        (ie, a non-gray checkbox).
//
//  Input:
//      treeHandle  -- the handle of the child tree item
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::HandleChildCheckbox(HTREEITEM  treeHandle)
{
    auto    CAlertTarget*   recPtr;
    auto    TV_ITEM         itemInfo;

    // get a pointer to the corresponding record object
    recPtr = (CAlertTarget*)m_alertTree.GetItemData(treeHandle);
    ASSERT(recPtr != NULL);
    if (NULL != recPtr)
        {
        // get the current state of the item's checkbox
        memset((void*)&itemInfo, 0, sizeof(TV_ITEM));
        itemInfo.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
        itemInfo.hItem = treeHandle;
        if (0 != m_alertTree.GetItem(&itemInfo))
            {
            // toggle the checkbox state
            if (ATC_UNCHECKED_BITMAP == itemInfo.iImage)
                {
                itemInfo.iSelectedImage = ATC_CHECKED_BITMAP;
                itemInfo.iImage = ATC_CHECKED_BITMAP;
                recPtr->SetActive(TRUE);
                }
            else if (ATC_CHECKED_BITMAP == itemInfo.iImage)
                {
                itemInfo.iSelectedImage = ATC_UNCHECKED_BITMAP;
                itemInfo.iImage = ATC_UNCHECKED_BITMAP;
                recPtr->SetActive(FALSE);
                }

            m_alertTree.SetItem(&itemInfo);
            }
        }

}  // end of "CAlertOptsDlg::HandleChildCheckbox"



// ==== HandleChildItemClick ==============================================
//
//  This function continues the processing of a child-item click.  If a
//  checkbox was hit, then a function is called to toggle its display and
//  the corresponding alert target record's flag.
//
//
//  Input:
//      treeHandle  -- the tree control handle of the clicked item
//      itemFlags   -- the flags returned from "CTreeCtrl::HitTest"
//
//  Output:
//      A value of TRUE if the child item is "live" (is available for
//      normal manipulation), FALSE if it is not.
//
// ========================================================================
//  Function created: 12/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::HandleChildItemClick(HTREEITEM  treeHandle
                                                        , UINT  itemFlags)
{
    auto    BOOL        retVal = TRUE;

    // if a checkbox icon was hit, toggle it
    if (TVHT_ONITEMICON & itemFlags)
        {
        HandleChildCheckbox(treeHandle);
        }

    return (retVal);

}  // end of "CAlertOptsDlg::HandleChildItemClick"



// ==== HandleChildItemEdit ===============================================
//
//  This function is called when the user double-clicks on a child item.
//  It calls the appropriate function to handle the selection.
//
//  Input:  treeHandle  -- the tree handle of the target child item
//
//  Output: a value of FALSE if nothing is processed, otherwise the return
//          value of the child item's corresponding "Modify" function.
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::HandleChildItemEdit(HTREEITEM  treeHandle)
{
    auto    BOOL            retVal = FALSE;
    auto    CAlertTarget*   recPtr;

    // if the current alert type is NLM and the master NLM item is present
    if  ((TARGET_NLM == m_alertType)
    &&  (TRUE == MasterNLMItemInTargetsList()))
        {
        // do nothing return FALSE
        retVal = FALSE;
        }
    else
        {
        // get the pointer to the data record from the tree handle
        recPtr = (CAlertTarget*)m_alertTree.GetItemData(treeHandle);

        // based on the type of record we're dealing with, call the appropriate
        // editor
        if (NULL != recPtr)
            {
            switch (recPtr->GetTargetType())
                {
                case TARGET_EMAIL:
                    retVal = HandleSMTPModify(treeHandle);
                    break;

                case TARGET_FORWARD:
                    retVal = HandleForwardModify(treeHandle);
                    break;

                case TARGET_MESSAGE_NETWORK:
                    retVal = HandleNetMessageModify(treeHandle);
                    break;

                case TARGET_NLM:
                    retVal = HandleNLMModify(treeHandle);
                    break;

                case TARGET_SNMP:
                    retVal = HandleSNMPModify(treeHandle);
                    break;

                default:
                    break;
                }
            }
        }

    return (retVal);

}  // end of "CAlertOptsDlg::HandleChildItemEdit"



// ==== HandleEmailListEdit ===============================================
//
//  This function calls the email edit dialog.  If the user clicks "OK" to
//  save any changes, a function is called to update the tree display.
//
//  Input:  nothing
//
//  Output: a value of TRUE if the edit reaches successful completion and
//          no errors occur, FALSE otherwise
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::HandleEmailListEdit(void)
{
    auto    BOOL            retVal = FALSE;
    auto    CString         tempStr;

    // fetch the property sheet title and create the property sheet
    tempStr.LoadString(IDS_EMAIL_ALERT_TARGETS);
    auto    CEmailPropertySheet emailPropSheet(tempStr);

    // no "apply" button
    emailPropSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;

    // add the email property page to the property sheet
    auto    CAlertSmtpPropertyPage  emailPropPage(IDD_SMTP_PROP_PAGE);
    emailPropSheet.AddPage(&emailPropPage);

        // add email option settings property page to the sheet
    auto    CSmtpOptPpg         smtpOptPropPage;
    emailPropSheet.AddPage(&smtpOptPropPage);

    auto    int             nResponse = emailPropSheet.DoModal();
        if (IDOK == nResponse)
            {
            // TODO: Place code here to handle when the dialog is
            //  dismissed with OK

        // the list has been edited, so refresh the email tree branch display
        retVal = UpdateEmailTreeDisplay();
        }
    else if (nResponse == IDCANCEL)
            {
            // TODO: Place code here to handle when the dialog is
            //  dismissed with Cancel
        }

    return (retVal);

}  // end of "CAlertOptsDlg::HandleEmailListEdit"



// ==== HandleForwardListEdit =============================================
//
//  This function calls the forward alert target edit dialog.  If the user
//  clicks "OK" to save any changes, a function is called to update the tree
//  display.
//
//  Input:  nothing
//
//  Output: a value of TRUE if the edit reaches successful completion and
//          no errors occur, FALSE otherwise
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::HandleForwardListEdit(void)
{
    auto    BOOL            retVal = TRUE;
    auto    CString         tempStr;

    // fetch the property sheet title and create the property sheet
    tempStr.LoadString(IDS_FORWARD_ALERT_TARGETS);
    auto    CNoContextPropertySheet     forwardPropSheet(tempStr);

    // no "apply" button
    forwardPropSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;

    // add the property page to the property sheet
    auto    CAlertForwardPropertyPage   forwardPropPage(IDD_FORWARD_PROP_PAGE);
    forwardPropSheet.AddPage(&forwardPropPage);

    auto    int             nResponse = forwardPropSheet.DoModal();
    if (IDOK == nResponse)
            {
            // TODO: Place code here to handle when the dialog is
            //  dismissed with OK

        // the list has been edited, so refresh the email tree branch display
        retVal = UpdateMachineTargetTreeDisplay(m_forwardTreeHandle);
        }
    else if (nResponse == IDCANCEL)
            {
            // TODO: Place code here to handle when the dialog is
            //  dismissed with Cancel
        }

    return (retVal);

}  // end of "CAlertOptsDlg::HandleForwardListEdit"



// ==== HandleForwardModify ===============================================
//
//  This function is called to handle the modification of an existing
//  record.
//
//  Input:  treeHandle  -- the tree handle of the target item
//
//  Output: a value of TRUE if the edit reaches successful completion and
//          no errors occur, FALSE otherwise
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::HandleForwardModify(HTREEITEM  treeHandle)
{
    auto    BOOL            retVal = FALSE;

    if (NULL != treeHandle)
        {
        // extract the pointer to the data record from the tree handle
        auto    CForwardTarget*      recPtr;
        recPtr = (CForwardTarget*)m_alertTree.GetItemData(treeHandle);

        // allow the user to edit the record
        if (TRUE == HandleMachineTargetEdit(recPtr, EDIT_MODIFY, this))
            {
            // refresh the tree branch display
            retVal = UpdateMachineTargetTreeDisplay(m_forwardTreeHandle);
            }
        }

    return (retVal);

}  // end of "CAlertOptsDlg::HandleForwardModify"



// ==== HandleLocalMachineEdit ============================================
//
//  This function calls the local machine edit dialog.
//
//  Input:  nothing
//  Output: a value of TRUE if the edit reaches successful completion, FALSE
//          if not
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::HandleLocalMachineEdit(void)
{
    auto    BOOL                retVal;
    auto    CAlertMessageDlg    dlg;

    if (IDOK == dlg.DoModal())
        {
        retVal = TRUE;
        }
    else
        {
        retVal = FALSE;
        }

    return (retVal);

}  // end of "CAlertOptsDlg::HandleLocalMachineEdit"



// ==== HandleNetMessageListEdit ==========================================
//
//  This function calls the machine alert target edit dialog.  If the user
//  clicks "OK" to save any changes, a function is called to update the tree
//  display.
//
//  Input:  nothing
//
//  Output: a value of TRUE if the edit reaches successful completion and
//          no errors occur, FALSE otherwise
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::HandleNetMessageListEdit(void)
{
    auto    BOOL            retVal = FALSE;
    auto    CString         tempStr;

    // fetch the property sheet title and create the property sheet
    tempStr.LoadString(IDS_NET_MESSAGE_TARGETS);
    auto    CNoContextPropertySheet     netMessagePropSheet(tempStr);

    // no "apply" button
    netMessagePropSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;

    // add the property page to the property sheet
    auto    CAlertNetMessengerPropertyPage      netMessagePage(IDD_NETMESSAGE_PROP_PAGE);
    netMessagePropSheet.AddPage(&netMessagePage);

    auto    int             nResponse = netMessagePropSheet.DoModal();
    if (IDOK == nResponse)
            {
            // TODO: Place code here to handle when the dialog is
            //  dismissed with OK

        // the list has been edited, so refresh the email tree branch display
        retVal = UpdateMachineTargetTreeDisplay(m_netMessageTreeHandle);
        }
    else if (nResponse == IDCANCEL)
            {
            // TODO: Place code here to handle when the dialog is
            //  dismissed with Cancel
        }

    return (retVal);

}  // end of "CAlertOptsDlg::HandleNetMessageListEdit"



// ==== HandleNetMessageModify ============================================
//
//  This function is called to handle the modification of an existing
//  record.
//
//  Input:  treeHandle  -- the tree handle of the target item
//
//  Output: a value of TRUE if the edit reaches successful completion and
//          no errors occur, FALSE otherwise
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::HandleNetMessageModify(HTREEITEM  treeHandle)
{
    auto    BOOL            retVal = FALSE;

    if (NULL != treeHandle)
        {
        // extract the pointer to the data record from the tree handle
        auto    CNetMsgTarget*      recPtr;
        recPtr = (CNetMsgTarget*)m_alertTree.GetItemData(treeHandle);

        // allow the user to edit the record
        if (TRUE == HandleMachineTargetEdit(recPtr, EDIT_MODIFY, this))
            {
            // refresh the tree branch display
            retVal = UpdateMachineTargetTreeDisplay(m_netMessageTreeHandle);
            }
        }

    return (retVal);

}  // end of "CAlertOptsDlg::HandleNetMessageModify"



// ==== HandleNLMListEdit =================================================
//
//  This function calls the NLM alert target edit dialog.  If the user
//  clicks "OK" to save any changes, a function is called to update the tree
//  display.
//
//  Input:  nothing
//
//  Output: a value of TRUE if the edit reaches successful completion and
//          no errors occur, FALSE otherwise
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::HandleNLMListEdit(void)
{
    auto    BOOL            retVal = FALSE;
    auto    CString         tempStr;

    // fetch the property sheet title and create the property sheet
    tempStr.LoadString(IDS_NLM_ALERT_TARGETS);
    auto    CNoContextPropertySheet     NLMPropSheet(tempStr);

    // no "apply" button
    NLMPropSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;

    // add the property page to the property sheet
    auto    CAlertNlmPropertyPage       NLMPropPage(IDD_NLM_PROP_PAGE);
    NLMPropSheet.AddPage(&NLMPropPage);

    auto    int             nResponse = NLMPropSheet.DoModal();
    if (IDOK == nResponse)
            {
            // TODO: Place code here to handle when the dialog is
            //  dismissed with OK

        // the list has been edited, so refresh the email tree branch display
        retVal = UpdateMachineTargetTreeDisplay(m_nlmTreeHandle);
        }
    else if (nResponse == IDCANCEL)
            {
            // TODO: Place code here to handle when the dialog is
            //  dismissed with Cancel
        }

    return (retVal);

}  // end of "CAlertOptsDlg::HandleNLMListEdit"



// ==== HandleNLMModify ===================================================
//
//  This function is called to handle the modification of an existing
//  record.
//
//  Input:  treeHandle  -- the tree handle of the target item
//
//  Output: a value of TRUE if the edit reaches successful completion and
//          no errors occur, FALSE otherwise
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::HandleNLMModify(HTREEITEM  treeHandle)
{
    auto    BOOL            retVal = FALSE;

    if (NULL != treeHandle)
        {
        // extract the pointer to the data record from the tree handle
        auto    CNLMTarget*     recPtr;
        recPtr = (CNLMTarget*)m_alertTree.GetItemData(treeHandle);

        // allow the user to edit the record
        if (TRUE == HandleMachineTargetEdit(recPtr, EDIT_MODIFY, this))
            {
            // refresh the tree branch display
            retVal = UpdateMachineTargetTreeDisplay(m_nlmTreeHandle);
            }
        }

    return (retVal);

}  // end of "CAlertOptsDlg::HandleNLMModify"

BOOL    CAlertOptsDlg::HandleSNMPListEdit(void)
{
    auto    BOOL            retVal = FALSE;
    auto    CString         tempStr;

    // fetch the property sheet title and create the property sheet
    tempStr.LoadString(IDS_SNMP_MESSAGE_TARGETS);
    auto    CNoContextPropertySheet     snmpMessagePropSheet(tempStr);

    // no "apply" button
    snmpMessagePropSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;

    // add the property page to the property sheet
    auto    CAlertSNMPPropertyPage      snmpMessagePage;
    snmpMessagePropSheet.AddPage(&snmpMessagePage);

    auto    int             nResponse = snmpMessagePropSheet.DoModal();
    if (IDOK == nResponse)
            {
            // TODO: Place code here to handle when the dialog is
            //  dismissed with OK

        // the list has been edited, so refresh the email tree branch display
        retVal = UpdateMachineTargetTreeDisplay(m_SNMPTreeHandle);
        }
    else if (nResponse == IDCANCEL)
            {
            // TODO: Place code here to handle when the dialog is
            //  dismissed with Cancel
        }

    return (retVal);

}  // end of "CAlertOptsDlg::HandleNetMessageListEdit"




// ==== HandleParentEdit ==================================================
//
//  This function calls up the editor for a parent target item.  It's
//  typically used when a user double-clicks on a parent item that has no
//  children or clicks on the add button.
//
//  Input:  treeHandle  -- the parent tree handle
//
//  Output: a value of TRUE if the edit reaches successful completion and
//          no errors occur, FALSE otherwise
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::HandleParentEdit(HTREEITEM  treeHandle)
{
    auto    BOOL        retVal;

    if (treeHandle == m_netMessageTreeHandle)
        {
        retVal = HandleNetMessageListEdit();
        }
    else if (treeHandle == m_SMTPTreeHandle)
        {
        retVal = HandleEmailListEdit();
        }
    else if (treeHandle == m_nlmTreeHandle)
        {
        retVal = HandleNLMListEdit();
        }
    else if (treeHandle == m_forwardTreeHandle)
        {
        retVal = HandleForwardListEdit();
        }
    else if (treeHandle == m_localMachineTreeHandle)
        {
        retVal = HandleLocalMachineEdit();
        }
    else if (treeHandle == m_SNMPTreeHandle)
        {
        retVal = HandleSNMPListEdit();
        }

    else
        {
        retVal = FALSE;
        }

    return (retVal);

}  // end of "CAlertOptsDlg::HandleParentEdit"



// ==== HandleRclickAlertTree =============================================
//
//  This function is called when the user right-clicks on the alert targets
//  tree control.  If a valid tree item was hit, then its menu ID is fetched
//  and displayed; else, a default menu is displayed.
//
//  Input:  the default MFC input arguments
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::HandleRclickAlertTree(void)
{
    // TODO: Add your control notification handler code here

    auto    DWORD       dwPos;
    auto    POINT       pointScreen;
    auto    UINT        menuID = -1;
    auto    UINT        hitFlags;

    // get the screen coordinates
    dwPos = GetMessagePos();
        pointScreen.x = LOWORD (dwPos);
        pointScreen.y = HIWORD (dwPos);

    // if a valid alert type has been selected, process it (we save the tree
    // handle ourselves because an actual right-menu selection can specify
    // a different tree handle)
    m_rightClickHandle = GetHitTreeItemHandle(&hitFlags);
    if (NULL != m_rightClickHandle)
        {
        // update the alert type data member variable
        m_alertType = MapTreeHandleToAlertType(m_rightClickHandle);

        // make the clicked item the selected item
        m_alertTree.SelectItem(m_rightClickHandle);

        // get a menuID
        if (TRUE == IsParentTreeHandle(m_rightClickHandle))
            {
            // for parent handles, the tree handle's lparam is the menuID
            menuID = (int)m_alertTree.GetItemData(m_rightClickHandle);
            }
        else
            {
            // menu ID is derived from record pointed to by lparam
            menuID = GetRecMenuID(m_rightClickHandle);
            }
        }

    if (-1 == menuID)
        {
        menuID = IDR_ATC_RMENU_GENERIC;
        }

    // create the popup menu
    auto    CMenu       tempMenu;
    VERIFY(tempMenu.LoadMenu(menuID));

    auto    CMenu       *popupMenu = tempMenu.GetSubMenu(0);
    ASSERT(popupMenu != NULL);

    // synchronize the edit buttons with the current tree handle
    UpdateEditButtons(m_rightClickHandle);

    // synchronize the menu items with their corresponding edit button states
    SynchronizeEditButtonsAndContextMenu(popupMenu);

    popupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON
                                                    , pointScreen.x
                                                    , pointScreen.y
                                                    , this);

}  // end of "CAlertOptsDlg::HandleRclickAlertTree"



// ==== HandleRemoveTreeChild =============================================
//
//  This function removes a child item from both the alert targets list and
//  the tree control.
//
//  Input: treeHandle   -- the tree handle of the target child item
//
//  Output: a value of TRUE if the item is successfully removed, FALSE if
//          it is not
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::HandleRemoveTreeChild(HTREEITEM  treeHandle)
{
    auto    BOOL                result = TRUE;

    // first see if we're removing the "master NLM item"
    auto    BOOL        bIsMasterNLMItem;
    bIsMasterNLMItem = IsMasterNLMTreeHandle(treeHandle);

    // remove the alert object from the global alert options list
    auto    CAlertTarget*       recPtr;
    recPtr = (CAlertTarget*)m_alertTree.GetItemData(treeHandle);
    if (NULL != recPtr)
        {
        gAlertOptions.RemoveTarget(recPtr);
        }

    // remove the item from the tree control
    result = m_alertTree.DeleteItem(treeHandle);

    // if the master NLM item is being removed, update that branch's children
    if (TRUE == bIsMasterNLMItem)
        {
        UpdateNLMTreeChildren(FALSE);   // FALSE means no master item
        }

    return (result);

}  // end of "CAlertOptsDlg::HandleRemoveTreeChild"



// ==== HandleSMTPModify ==================================================
//
//  This function is called to handle the modification of an SMTP existing
//  record.
//
//  Input:  nothing
//
//  Output: if no errors occur a value of TRUE is returned, FALSE otherwise
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::HandleSMTPModify(HTREEITEM  treeHandle)
{
    auto    BOOL            retVal = TRUE;
    auto    CEMailTarget*   recPtr;

    if (NULL != treeHandle)
        {
        // extract the pointer to the data record from the tree handle
        recPtr = (CEMailTarget*)m_alertTree.GetItemData(treeHandle);

        // allow the user to edit the record
        if (TRUE == HandleSMTPEdit(recPtr, EDIT_MODIFY))
            {
            // update the tree display
            retVal = UpdateEmailTreeDisplay();
            }
        }

    return (retVal);

}  // end of "CAlertOptsDlg::HandleSMTPModify"

// ==== HandleSMTPModify ==================================================
//
//  This function is called to handle the modification of an SMTP existing
//  record.
//
//  Input:  nothing
//
//  Output: if no errors occur a value of TRUE is returned, FALSE otherwise
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::HandleSNMPModify(HTREEITEM  treeHandle)
{
    auto    BOOL            retVal = TRUE;
    auto    CSNMPTarget*   recPtr;

    if (NULL != treeHandle)
        {
        // extract the pointer to the data record from the tree handle
        recPtr = (CSNMPTarget*)m_alertTree.GetItemData(treeHandle);

        // allow the user to edit the record
        if (TRUE == HandleSNMPEdit(recPtr, EDIT_MODIFY))
            {
            HTREEITEM tempHandle = m_alertTree.GetParentItem(treeHandle);

            // update the tree display
            retVal = UpdateMachineTargetTreeDisplay(tempHandle);
            }
        }

    return (retVal);

}  // end of "CAlertOptsDlg::HandleSMTPModify"


// ==== InitControls ======================================================
//
//  This function initializes the controls in the dialog window.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

void    CAlertOptsDlg::InitControls(void)
{
    // populate the tree control
    InitTreeControl();

    // initialize the checkboxes
    m_apCheckbox.SetCheck(gAlertOptions.GetAPRemoteAlertsEnabled());
    m_scannerCheckbox.SetCheck(gAlertOptions.GetScannerRemoteAlertsEnabled());
    m_IPXCheckBox = gAlertOptions.GetIPXAlertsEnabled();
    m_SAMCheckBox = gAlertOptions.GetATPAlertsEnabled();
    UpdateData(FALSE);

    // set the current tree branch item
    m_alertTree.SelectItem(m_localMachineTreeHandle);

    // enable the "add" and "modify" buttons
    auto    DWORD       buttonFlags;
    buttonFlags = ENABLE_MODIFY_BUTTON | ENABLE_ADD_BUTTON;
    EnableEditButtons(buttonFlags);

}  // end of "CAlertOptsDlg::InitControls"



// ==== InitTreeControl ===================================================
//
//  This function initializes the alert targets tree control.
//
//  Input:  nothing
//
//  Output: if no errors occur a value of TRUE is returned, FALSE otherwise
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::InitTreeControl(void)
{
    auto    BOOL        retVal;

    m_alertTree.SetImageList(&m_imageList, TVSIL_NORMAL);

    // populate the tree control
    InsertParentBranches();

    retVal = ReadAlertTargetRecords();
    if (TRUE == retVal)
        {
        auto    BOOL        masterPresent;
        masterPresent = MasterNLMItemInTargetsList();
        UpdateMasterNLMTreeBranch(masterPresent);

        // based on the remote alert options, enable or disable the tree control
        if  ((FALSE == gAlertOptions.GetAPRemoteAlertsEnabled())
        &&  (FALSE == gAlertOptions.GetScannerRemoteAlertsEnabled()))
            {
            // disable the tree control
            m_alertTree.EnableWindow(FALSE);
            }
        else
            {
            // enable the tree control
            m_alertTree.EnableWindow(TRUE);
            }
        }

    return (retVal);

}  // end of "CAlertOptsDlg::InitTreeControl"



// ==== InsertParentBranches ==============================================
//
//  This function inserts the "parent" branches in the alert targets tree
//  control.  As each branch is inserted into the tree, the tree handle is
//  saved in a member variable for future reference.  Additionally, the
//  "lparam" of each item is initialized with its own menu ID (for right
//  click menus).
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::InsertParentBranches(void)
{
    auto    CString         tempStr;
    auto    TV_ITEM         treeItemInfo;

    // local machine
    tempStr.LoadString(IDS_ATC_LOCAL_MACHINE);
    m_localMachineTreeHandle = m_alertTree.InsertItem(tempStr);
    if (NULL != m_localMachineTreeHandle)
        {
        treeItemInfo.hItem = m_localMachineTreeHandle;
        treeItemInfo.mask = TVIF_SELECTEDIMAGE | TVIF_IMAGE | TVIF_PARAM;
        treeItemInfo.iSelectedImage = ATC_LOCAL_MACHINE_BITMAP;
        treeItemInfo.iImage = ATC_LOCAL_MACHINE_BITMAP;
        treeItemInfo.lParam = IDR_ATC_RMENU_LOCAL_MACHINE;
        m_alertTree.SetItem(&treeItemInfo);
        }

    // forward alert targets
    tempStr.LoadString(IDS_ATC_FORWARD_TARGETS);
    m_forwardTreeHandle = m_alertTree.InsertItem(tempStr);
    if (NULL != m_forwardTreeHandle)
        {
        treeItemInfo.hItem = m_forwardTreeHandle;
        treeItemInfo.mask = TVIF_SELECTEDIMAGE | TVIF_IMAGE | TVIF_PARAM;
        treeItemInfo.iSelectedImage = ATC_FORWARD_BITMAP;
        treeItemInfo.iImage = ATC_FORWARD_BITMAP;
        treeItemInfo.lParam = IDR_ATC_RMENU_FORWARD;
        m_alertTree.SetItem(&treeItemInfo);
        }

    // network message targets
    tempStr.LoadString(IDS_ATC_NET_MESSAGE_TARGETS);
    m_netMessageTreeHandle = m_alertTree.InsertItem(tempStr);
    if (NULL != m_netMessageTreeHandle)
        {
        treeItemInfo.hItem = m_netMessageTreeHandle;
        treeItemInfo.mask = TVIF_SELECTEDIMAGE | TVIF_IMAGE | TVIF_PARAM;
        treeItemInfo.iSelectedImage = ATC_NET_MESSAGE_BITMAP;
        treeItemInfo.iImage = ATC_NET_MESSAGE_BITMAP;
        treeItemInfo.lParam = IDR_ATC_RMENU_NETMESSAGE;
        m_alertTree.SetItem(&treeItemInfo);
        }

    // email targets
    tempStr.LoadString(IDS_ATC_EMAIL_TARGETS);
    m_SMTPTreeHandle = m_alertTree.InsertItem(tempStr);
    if (NULL != m_SMTPTreeHandle)
        {
        treeItemInfo.hItem = m_SMTPTreeHandle;
        treeItemInfo.mask = TVIF_SELECTEDIMAGE | TVIF_IMAGE | TVIF_PARAM;
        treeItemInfo.iSelectedImage = ATC_EMAIL_BITMAP;
        treeItemInfo.iImage = ATC_EMAIL_BITMAP;
        treeItemInfo.lParam = IDR_ATC_RMENU_SMTP;
        m_alertTree.SetItem(&treeItemInfo);
        }

    // NLMs targets
    tempStr.LoadString(IDS_ATC_NLM_TARGETS);
    m_nlmTreeHandle = m_alertTree.InsertItem(tempStr);
    if (NULL != m_nlmTreeHandle)
        {
        treeItemInfo.hItem = m_nlmTreeHandle;
        treeItemInfo.mask = TVIF_SELECTEDIMAGE | TVIF_IMAGE | TVIF_PARAM;
        treeItemInfo.iSelectedImage = ATC_NLM_BITMAP;
        treeItemInfo.iImage = ATC_NLM_BITMAP;
        treeItemInfo.lParam = IDR_ATC_RMENU_NLM;
        m_alertTree.SetItem(&treeItemInfo);
        }

    // SNMPs targets
    tempStr.LoadString(IDS_ATC_SNMP_TARGETS);
    m_SNMPTreeHandle = m_alertTree.InsertItem(tempStr);
    if (NULL != m_nlmTreeHandle)
        {
        treeItemInfo.hItem = m_SNMPTreeHandle;
        treeItemInfo.mask = TVIF_SELECTEDIMAGE | TVIF_IMAGE | TVIF_PARAM;
        treeItemInfo.iSelectedImage = ATC_SNMP_BITMAP;
        treeItemInfo.iImage = ATC_SNMP_BITMAP;
        treeItemInfo.lParam = IDR_ATC_RMENU_SNMP;
        m_alertTree.SetItem(&treeItemInfo);
        }


}  // end of "CAlertOptsDlg::InsertParentBranches"



// ==== IsMasterNLMTreeHandle =============================================
//
//  This function checks if the input tree handle is the "master NLM item."
//
//  Input:
//      treeHandle  -- the target tree handle
//
//  Output:
//      a value of TRUE if the target tree handle is the "master NLM item",
//      a value of FALSE if it is not
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::IsMasterNLMTreeHandle(HTREEITEM  treeHandle)
{
        // TODO: Add your specialized code here and/or call the base class
    auto    BOOL        result;

    if (MASTER_NLM_ITEM_STR == m_alertTree.GetItemText(treeHandle))
        {
        result = TRUE;
        }
    else
        {
        result = FALSE;
        }

    return (result);

}  // end of "CAlertOptsDlg::IsMasterNLMTreeHandle"



// ==== IsParentTreeHandle ================================================
//
//  This function tests if the input tree handle is a "parent" item.
//
//  Input:  the target tree handle
//  Output: a value of TRUE if the input tree handle is recognized as a
//          parent handle, FALSE otherwise.
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::IsParentTreeHandle(HTREEITEM  treeHandle)
{
    auto    BOOL        result;

    if  ((m_netMessageTreeHandle == treeHandle)
    ||  (m_SMTPTreeHandle == treeHandle)
    ||  (m_nlmTreeHandle == treeHandle)
    ||  (m_forwardTreeHandle == treeHandle)
    ||  (m_localMachineTreeHandle == treeHandle)
    ||  (m_SNMPTreeHandle == treeHandle))
        {
        result = TRUE;
        }
    else
        {
        result = FALSE;
        }

    return (result);

}  // end of "CAlertOptsDlg::IsParentTreeHandle"



// ==== MapTreeHandleToAlertType ==========================================
//
//  This function sets the alert type data member to the appropriate value
//  based on the value of the tree handle argument.
//
//  Input:  the target tree handle
//  Output: nothing (the alert type member variable is updated)
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

DWORD   CAlertOptsDlg::MapTreeHandleToAlertType(HTREEITEM  treeHandle)
{
    auto    HTREEITEM   tempHandle = treeHandle;
    auto    int         alertType;

    // if the current item is a child item, get its parent handle
    if (FALSE == IsParentTreeHandle(tempHandle))
        {
        tempHandle = m_alertTree.GetParentItem(tempHandle);
        }

    // based on the parent handle, update the current alert type
    if (m_netMessageTreeHandle == tempHandle)
        {
        alertType = TARGET_MESSAGE_NETWORK;
        }
    else if (m_SMTPTreeHandle == tempHandle)
        {
        alertType = TARGET_EMAIL;
        }
    else if (m_nlmTreeHandle == tempHandle)
        {
        alertType = TARGET_NLM;
        }
    else if (m_forwardTreeHandle == tempHandle)
        {
        alertType = TARGET_FORWARD;
        }
    else if (m_localMachineTreeHandle == tempHandle)
        {
        alertType = TARGET_LOCAL_MACHINE;
        }
    else if (m_SNMPTreeHandle == tempHandle)
        {
        alertType = TARGET_SNMP;
        }
    else
        {
        alertType = TARGET_NONE;
        }

    return (alertType);

}  // end of "CAlertOptsDlg::MapTreeHandleToAlertType"



// ==== ReadAlertTargetRecords ============================================
//
//  This function reads in the alert target records and calls the appropriate
//  functions to add items to the tree control.
//
//  Input:  nothing
//
//  Output: a value of TRUE if the records are successfully read, FALSE if not
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::ReadAlertTargetRecords(void)
{
    auto    BOOL                result = TRUE;
    auto    CAlertTarget*       pAlertTarget;

    // read in the alert options data file
    if (gAlertOptions.LoadOptions())
        {
        // read in each target and set the parent tree handle accordingly
        while (pAlertTarget = gAlertOptions.GetNextTarget())
            {
            switch (pAlertTarget->GetTargetType())
                {
                case TARGET_MESSAGE_NETWORK:
                    AddMachineTargetBranch(m_netMessageTreeHandle, pAlertTarget);
                    break;

                case TARGET_EMAIL:
                    AddEmailTargetBranch(m_SMTPTreeHandle, pAlertTarget);
                    break;

                case TARGET_NLM:
                    AddMachineTargetBranch(m_nlmTreeHandle, pAlertTarget);
                    break;

                case TARGET_FORWARD:
                    AddMachineTargetBranch(m_forwardTreeHandle, pAlertTarget);
                    break;

                case TARGET_SNMP:
                    AddMachineTargetBranch(m_SNMPTreeHandle, pAlertTarget);
                    break;

                default:
                    break;
                }
            }
        }
    else
        {
        result = FALSE;
        }

    return (result);

}  // end of "CAlertOptsDlg::ReadAlertTargetRecords"



// ===== SynchronizeEditButtonsAndContextMenu =============================
//
//  This function uses the enabled/disabled state of the button arguments
//  to enable or disable corresponding items in the menu argument.
//  NOTE: this function assumes that the first three menu items are "Add",
//        "Modify" and "Remove", in that order
//
//  Input:
//      menuPtr         -- a pointer to the target context menu
//
//  Output:
//      nothing
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

void    CAlertOptsDlg::SynchronizeEditButtonsAndContextMenu(CMenu  *menuPtr)
{
    ASSERT(menuPtr != NULL);

    // add button -- position 0
    if (TRUE == m_addButton.IsWindowEnabled())
        {
        menuPtr->EnableMenuItem(0, MF_ENABLED | MF_BYPOSITION);
        }
    else
        {
        menuPtr->EnableMenuItem(0, MF_GRAYED | MF_BYPOSITION);
        }

    // modify button -- position 1
    if (TRUE == m_modifyButton.IsWindowEnabled())
        {
        menuPtr->EnableMenuItem(1, MF_ENABLED | MF_BYPOSITION);
        }
    else
        {
        menuPtr->EnableMenuItem(1, MF_GRAYED | MF_BYPOSITION);
        }

    // remove button -- position 2
    if (TRUE == m_removeButton.IsWindowEnabled())
        {
        menuPtr->EnableMenuItem(2, MF_ENABLED | MF_BYPOSITION);
        }
    else
        {
        menuPtr->EnableMenuItem(2, MF_GRAYED | MF_BYPOSITION);
        }

}  // end of "CAlertOptsDlg::SynchronizeEditButtonsAndContextMenu"



// ==== UpdateEditButtons =================================================
//
//  This function determines the availability of the edit buttons based on
//  the attributes of the input tree handle argument (ie, if it's a parent
//  or a child branch, etc.).
//
//  Input:
//      treeHandle  -- the current tree handle
//
//  Output:
//      nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::UpdateEditButtons(HTREEITEM  treeHandle)
{
    auto    DWORD           buttonFlags = 0;

    // update the edit buttons
    if (FALSE == IsParentTreeHandle(treeHandle))
        {
        if  ((TARGET_NLM == m_alertType)
        &&  (TRUE == MasterNLMItemInTargetsList()))
            {
            if (TRUE == IsMasterNLMTreeHandle(treeHandle))
                {
                // must be the master NLM item, allow its removal
                buttonFlags = ENABLE_REMOVE_BUTTON;
                }
            else
                {
                // master NLM item is in the tree, all other NLM children
                // are unavailable
                buttonFlags = 0;
                }
            }
        else
            {
            // enable all buttons
            buttonFlags = ENABLE_ADD_BUTTON | ENABLE_MODIFY_BUTTON
                                            | ENABLE_REMOVE_BUTTON;
            }
        }
    else    // must be a parent branch
        {
        // enable the add and modify buttons
        buttonFlags = ENABLE_ADD_BUTTON | ENABLE_MODIFY_BUTTON;
        }

    // refresh the edit buttons
    EnableEditButtons(buttonFlags);

}  // end of "CAlertOptsDlg::UpdateEditButtons"



// ==== UpdateEmailTreeDisplay ============================================
//
//  This function updates the display of the email targets in the alert
//  targets tree control.  It first runs through the existing child branches
//  and marks them for removal.  Then records from the master alert targets
//  is added to the tree control.  Finally, any branches marked for removal
//  are deleted from the tree control.
//
//  Input:  nothing
//
//  Output: if no errors occur a value of TRUE is returned, FALSE otherwise
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::UpdateEmailTreeDisplay(void)
{
    auto    BOOL        retVal = TRUE;

    // if there are any email children branches, tag them for removal by
    // setting their "lparam" members to an invalid value
    if (TRUE == m_alertTree.ItemHasChildren(m_SMTPTreeHandle))
        {
        auto    HTREEITEM       currentChild;
        auto    HTREEITEM       nextChild;

        currentChild = m_alertTree.GetChildItem(m_SMTPTreeHandle);
        if (NULL != currentChild)
            {
            do  {
                nextChild = m_alertTree.GetNextSiblingItem(currentChild);
                m_alertTree.SetItemData(currentChild, INVALID_LPARAM_VALUE);
                if (NULL != nextChild)
                    {
                    currentChild = nextChild;
                    }

                }  while (NULL != nextChild);
            }
        }

    // traverse the master list and add new tree branches
    auto    CAlertTarget*       pAlertTarget;
    gAlertOptions.ResetPosition();
    while (pAlertTarget = gAlertOptions.GetNextTarget(TARGET_EMAIL))
        {
        if (NULL == AddEmailTargetBranch(m_SMTPTreeHandle, pAlertTarget))
            {
            retVal = FALSE;
            }
        }

    // finally, remove any tagged child items from the tree branch
    if (TRUE == m_alertTree.ItemHasChildren(m_SMTPTreeHandle))
        {
        auto    DWORD           lParamValue;
        auto    HTREEITEM       currentChild;
        auto    HTREEITEM       nextChild;

        currentChild = m_alertTree.GetChildItem(m_SMTPTreeHandle);
        if (NULL != currentChild)
            {
            do  {
                nextChild = m_alertTree.GetNextSiblingItem(currentChild);
                lParamValue = m_alertTree.GetItemData(currentChild);
                if (INVALID_LPARAM_VALUE == lParamValue)
                    {
                    m_alertTree.DeleteItem(currentChild);
                    }

                if (NULL != nextChild)
                    {
                    currentChild = nextChild;
                    }

                }  while (NULL != nextChild);
            }
        }

    // refresh the display
    m_alertTree.RedrawWindow();

    return (retVal);

}  // end of "CAlertOptsDlg::UpdateEmailTreeDisplay"



// ==== UpdateMachineTargetTreeDisplay ====================================
//
//  This function refreshes the tree display of a target "machine" branch.
//  If first deletes all the target children items, then uses the current
//  contents of the global alerts targets list to reinsert new child items.
//
//  Input:
//      parentHandle    -- the tree handle to the parent branch
//
//  Output: if no errors occur a value of TRUE is returned, FALSE otherwise
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::UpdateMachineTargetTreeDisplay(HTREEITEM  parentHandle)
{
    auto    BOOL        result = FALSE;

    // derive the alert type
    auto    DWORD       alertType;
    alertType = MapTreeHandleToAlertType(parentHandle);
    if (TARGET_NONE == alertType)
        {
        result = FALSE;
        goto  allDone;
        }

    // if there are any children branches, tag them for removal by setting
    // their "lparam" members to an invalid value
    if (TRUE == m_alertTree.ItemHasChildren(parentHandle))
        {
        auto    HTREEITEM       currentChild;
        auto    HTREEITEM       nextChild;

        currentChild = m_alertTree.GetChildItem(parentHandle);
        if (NULL != currentChild)
            {
            do  {
                nextChild = m_alertTree.GetNextSiblingItem(currentChild);
                m_alertTree.SetItemData(currentChild, INVALID_LPARAM_VALUE);
                if (NULL != nextChild)
                    {
                    currentChild = nextChild;
                    }

                }  while (NULL != nextChild);
            }
        }

    // traverse the master list; if we find a record whose alert type
    // matches the input argument, add it to the appropriate tree branch
    auto    CAlertTarget*   pAlertTarget;
    gAlertOptions.ResetPosition();
    while (pAlertTarget = gAlertOptions.GetNextTarget(alertType))
        {
        if (NULL == AddMachineTargetBranch(parentHandle, pAlertTarget))
            {
            // flag the error
            result = FALSE;
            }
        }

    // finally, remove any tagged child items from the tree branch
    if (TRUE == m_alertTree.ItemHasChildren(parentHandle))
        {
        auto    DWORD           lParamValue;
        auto    HTREEITEM       currentChild;
        auto    HTREEITEM       nextChild;

        currentChild = m_alertTree.GetChildItem(parentHandle);
        if (NULL != currentChild)
            {
            do  {
                nextChild = m_alertTree.GetNextSiblingItem(currentChild);
                lParamValue = m_alertTree.GetItemData(currentChild);
                if (INVALID_LPARAM_VALUE == lParamValue)
                    {
                    m_alertTree.DeleteItem(currentChild);
                    }

                if (NULL != nextChild)
                    {
                    currentChild = nextChild;
                    }

                }  while (NULL != nextChild);
            }
        }

    // if we're dealing with the NLM branch we need to handle the possible
    // presence of the master NLM item
    if (TARGET_NLM == alertType)
        {
        auto    BOOL        masterPresent;
        masterPresent = MasterNLMItemInTargetsList();
        if (TRUE == masterPresent)
            {
            result = UpdateMasterNLMTreeBranch(masterPresent);
            }
        }

    // refresh the display
    m_alertTree.RedrawWindow();

allDone:
    return (result);

}  // end of "CAlertOptsDlg::UpdateMachineTargetTreeDisplay"



// ==== UpdateMasterNLMTreeBranch =========================================
//
//  This function is called after the master NLM item is inserted or removed
//  from the tree (ie, when the alert targets are first read in, the master
//  NLM item looks like all of the other NLM children).  The icon of the
//  master item is changed, and then a function to update any other NLM
//  children is called.
//
//  Input:
//      masterPresent   -- a BOOL indicating the presence or absence of the
//                         master NLM item
//
//  Output: a value of TRUE if no errors occur, FALSE otherwise.
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::UpdateMasterNLMTreeBranch(BOOL  masterPresent)
{
    auto    BOOL            result = TRUE;
    auto    HTREEITEM       masterNLMBranch;

    // if the master NLM item is in the alert targets list...
    if (TRUE == masterPresent)
        {
        // find the master NLM child branch (it should be the first one)
        masterNLMBranch = GetChildTreeHandle(m_nlmTreeHandle
                                                    , MASTER_NLM_ITEM_STR);
        if (NULL == masterNLMBranch)
            {
            result = FALSE;
            }
        else
            {
            // update the icon of the master NLM item
            auto    TV_ITEM         treeInfo;
            memset((void*)&treeInfo, 0, sizeof(TV_ITEM));
            treeInfo.hItem = masterNLMBranch;
            treeInfo.mask = TVIF_SELECTEDIMAGE | TVIF_IMAGE;
            treeInfo.iSelectedImage = ATC_MASTER_NLM_BITMAP;
            treeInfo.iImage = ATC_MASTER_NLM_BITMAP;
            m_alertTree.SetItem(&treeInfo);
            }
        }

    // if there are other NLM children, update their icons
    UpdateNLMTreeChildren(masterPresent);

    return (result);

}  // end of "CAlertOptsDlg::UpdateMasterNLMTreeBranch"



// ==== UpdateNLMTreeChildren =============================================
//
//  This function is called after the master NLM item has been added or
//  removed from the tree.  If the master has been added, the children icons
//  are changed from normal "black" to "gray"; the reverse happens when the
//  master has been removed from the tree.
//
//  Input:
//      masterPresent   -- a BOOL indicating the presence or absence of the
//                         master NLM item
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::UpdateNLMTreeChildren(BOOL  masterPresent)
{
    auto    HTREEITEM       childHandle;
    auto    TV_ITEM         treeInfo;

    // if there are no children, get outta here
    if (FALSE == m_alertTree.ItemHasChildren(m_nlmTreeHandle))
        {
        goto  allDone;
        }

    // try to access the first child so we can loop...
    childHandle = m_alertTree.GetChildItem(m_nlmTreeHandle);
    if (NULL == childHandle)
        {
        goto  allDone;
        }


    // setup the tree item structure
    memset((void*)&treeInfo, 0, sizeof(TV_ITEM));
    treeInfo.mask = TVIF_SELECTEDIMAGE | TVIF_IMAGE;


    // loop and touch all NLM children
    do  {
        // skip over the master NLM item
        if (MASTER_NLM_ITEM_STR == m_alertTree.GetItemText(childHandle))
            {
            childHandle = m_alertTree.GetNextSiblingItem(childHandle);
            continue;
            }

        // get the current icon for this child item
        treeInfo.hItem = childHandle;
        if (0 != m_alertTree.GetItem(&treeInfo))
            {
            // toggle the checkbox state
            if (TRUE == masterPresent)
                {
                // use gray icons
                if (ATC_UNCHECKED_BITMAP == treeInfo.iImage)
                    {
                    treeInfo.iSelectedImage = ATC_GRAY_UNCHECKED_BITMAP;
                    treeInfo.iImage = ATC_GRAY_UNCHECKED_BITMAP;
                    }
                else if (ATC_CHECKED_BITMAP == treeInfo.iImage)
                    {
                    treeInfo.iSelectedImage = ATC_GRAY_CHECKED_BITMAP;
                    treeInfo.iImage = ATC_GRAY_CHECKED_BITMAP;
                    }
                }
            else    // master NLM item not in tree
                {
                // use normal icons
                if (ATC_GRAY_UNCHECKED_BITMAP == treeInfo.iImage)
                    {
                    treeInfo.iSelectedImage = ATC_UNCHECKED_BITMAP;
                    treeInfo.iImage = ATC_UNCHECKED_BITMAP;
                    }
                else if (ATC_GRAY_CHECKED_BITMAP == treeInfo.iImage)
                    {
                    treeInfo.iSelectedImage = ATC_CHECKED_BITMAP;
                    treeInfo.iImage = ATC_CHECKED_BITMAP;
                    }
                }

            // change the current tree handle's icon
            m_alertTree.SetItem(&treeInfo);
            }

        childHandle = m_alertTree.GetNextSiblingItem(childHandle);

        }  while (NULL != childHandle);

allDone:
    return;

}  // end of "CAlertOptsDlg::UpdateNLMTreeChildren"








/////////////////////////////////////////////////////////////////////////////
// CAlertOptsDlg message handlers
/////////////////////////////////////////////////////////////////////////////





// ==== OnAddButton =======================================================
//
//  This function calls the appropriate dialog to add a new alert target
//  record.
//
//  Input: nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 12/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnAddButton(void)
{
        // TODO: Add your control notification handler code here

    // get the currently-selected tree handle and derive the alert type
    auto    DWORD           alertType;
    auto    HTREEITEM       treeHandle;
    treeHandle = m_alertTree.GetSelectedItem();
    alertType = MapTreeHandleToAlertType(treeHandle);
    if  ((TARGET_NONE != alertType)  &&  (NULL != treeHandle))
        {
        if (TRUE == IsParentTreeHandle(treeHandle))
            {
            // on parent items, the add button does the same thing as the
            // modify button
            OnModifyButton();
            }
        else
            {
            // call the appropriate child item add function
            switch(alertType)
                {
                case  TARGET_EMAIL:
                    OnHandleSMTPAdd();
                    break;

                case  TARGET_FORWARD:
                    OnHandleForwardAdd();
                    break;

                case  TARGET_MESSAGE_NETWORK:
                    OnHandleNetMessageAdd();
                    break;

                case  TARGET_NLM:
                    OnHandleNLMAdd();
                    break;

                case  TARGET_SNMP:
                    OnHandleSNMPAdd();
                    break;

                default:
                    break;
                }
            }
        }

}  // end of "CAlertOptsDlg::OnAddButton"



// ==== OnAutoProtectCheckbox =============================================
//
//  This function is called when the user clicks on the auto-protect
//  checkbox from the main alert options dialog window.  If all the
//  checkboxes that enable remote alerts are cleared, then the tree control
//  is disabled to reflect that state.
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnAutoProtectCheckbox(void)
{
        // TODO: Add your control notification handler code here

    if  ((0 == m_apCheckbox.GetCheck())
    &&  (0 == m_scannerCheckbox.GetCheck()))
        {
        m_alertTree.EnableWindow(FALSE);
        }
    else
        {
        m_alertTree.EnableWindow(TRUE);
        }

    gAlertOptions.SetAPRemoteAlertsEnabled(m_apCheckbox.GetCheck());

}  // end of "CAlertOptsDlg::OnAutoProtectCheckbox"



// ==== OnClickAlertTree ==================================================
//
//  This function is called when the user clicks on the alert targets tree
//  control.  If a tree item is hit, then the appropriate functions are
//  called for further processing.
//
//  Input:  the default MFC input arguments
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnClickAlertTree(NMHDR* pNMHDR, LRESULT* pResult)
{
        // TODO: Add your control notification handler code here

    // if a valid alert type has been selected, process it
    auto    HTREEITEM       treeHandle;
    auto    UINT            hitFlags;

    treeHandle = GetHitTreeItemHandle(&hitFlags);
    if  ((NULL != treeHandle)
    &&  (TVHT_ONITEM & hitFlags))
        {
        // update the alert type data member variable
        m_alertType = MapTreeHandleToAlertType(treeHandle);

        // if it's a child item, make sure the checkbox icon is handled
        if (FALSE == IsParentTreeHandle(treeHandle))
            {
            HandleChildItemClick(treeHandle, hitFlags);
            }

        UpdateEditButtons(treeHandle);
        }

    *pResult = 0;

}  // end of "CAlertOptsDlg::OnClickAlertTree"



// ==== OnCommand =========================================================
//
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
        // TODO: Add your specialized code here and/or call the base class

    // Handle help button click
    if( LOWORD(wParam) == ID_HELP )
        {
        CString sHelpFile =  NAV_HELPFILENAME;
        sHelpFile += _T(" >TASK");

        ::WinHelp (GetSafeHwnd(), sHelpFile,
            HELP_CONTEXT,  IDH_NAVNT_CONFIG_ALERTS_HELP_BTN );
        return TRUE;
        }


    // if the message concerns a control, store a pointer to its window in
    // the current context variable so it'll be ready for context help
    if (lParam > 0)
        {
        m_pCurrentContextWnd = GetDlgItem(LOWORD(wParam));
        }

    return CHelpPropertyPage::OnCommand(wParam, lParam);

}  // end of "CAlertOptsDlg::OnCommand"



// ==== OnContextMenu =====================================================
//
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnContextMenu(CWnd* pWnd, CPoint point)
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

    // we'll handle the tree control ourselves
    auto    int         winID;
    winID = pWnd->GetDlgCtrlID();
    if  ((IDC_ALERT_TREE == winID)
    &&  (TRUE == pWnd->IsWindowEnabled()))
        {
        HandleRclickAlertTree();
        return;
        }

    CHelpPropertyPage::OnContextMenu(pWnd, point);

}  // end of "CAlertOptsDlg::OnContextMenu"



// ==== OnDblclkAlertTree =================================================
//
//  This function handles double-clicks on the alert tree control.
//
//  Input:  the default MFC input arguments
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnDblclkAlertTree(NMHDR* pNMHDR, LRESULT* pResult)
{
        // TODO: Add your control notification handler code here

    auto    HTREEITEM                   treeHandle;
    auto    UINT        hitFlags;

    treeHandle = GetHitTreeItemHandle(&hitFlags);
    if  ((NULL != treeHandle)
    &&  (TVHT_ONITEM & hitFlags))
        {
        if (FALSE == IsParentTreeHandle(treeHandle))
            {
            // edit children automatically
            HandleChildItemEdit(treeHandle);
            }
        else
            {
            if (m_localMachineTreeHandle == treeHandle)
                {
                // edit local machine automatically (has no children)
                HandleLocalMachineEdit();
                }
            else if (0 == m_alertTree.ItemHasChildren(treeHandle))
                {
                // if a parent has no children, bring up its editor
                HandleParentEdit(treeHandle);
                }
            }
        }

        *pResult = 0;

}  // end of "CAlertOptsDlg::OnDblclkAlertTree"



// ==== OnHandleEmailListEdit =============================================
//
//  This function is called when the user selects the "edit email list"
//  item from the email list's right menu (see the message map at the top
//  of this file).
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnHandleEmailListEdit(void)
{
    HandleEmailListEdit();

}  // end of "CAlertOptsDlg::OnHandleEmailListEdit"



// ==== OnHandleForwardAdd ================================================
//
//  This function is called to add a new forward alert target record.
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnHandleForwardAdd(void)
{
    auto    CForwardTarget*     newRecPtr;

    try {
        newRecPtr = new CForwardTarget;
        }
    catch(CMemoryException)
        {
        }

    // handle the edit
    if (TRUE == HandleMachineTargetEdit(newRecPtr, EDIT_ADD, this))
        {
        // add the new target to the master list
        gAlertOptions.AddTarget(newRecPtr);

        // the list has been edited, so refresh the tree branch display
        UpdateMachineTargetTreeDisplay(m_forwardTreeHandle);
        }
    else
        {
        // release allocated memory
        delete  newRecPtr;
        }

}  // end of "CAlertOptsDlg::OnHandleForwardAdd"



// ==== OnHandleForwardListEdit ===========================================
//
//  This function is called when the user selects the "edit forward targets
//  list" item from the forward list's right menu (see the message map at the
//  top of this file).
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnHandleForwardListEdit(void)
{
    HandleForwardListEdit();

}  // end of "CAlertOptsDlg::OnHandleForwardListEdit"



// ==== OnHandleLocalMachineEdit ==========================================
//
//  This function is called when the user selects the "edit local machine
//  list" item from the local machine's right menu (see the message map at the
//  top of this file).
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnHandleLocalMachineEdit(void)
{
    HandleLocalMachineEdit();

}  // end of "CAlertOptsDlg::OnHandleLocalMachineEdit"



// ==== OnHandleNetMessageAdd =============================================
//
//  This function is called to add a new net messenger alert target record.
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnHandleNetMessageAdd(void)
{
    auto    CNetMsgTarget*      newRecPtr;

    try {
        newRecPtr = new CNetMsgTarget;
        }
    catch(CMemoryException)
        {
        }

    // handle the edit
    if (TRUE == HandleMachineTargetEdit(newRecPtr, EDIT_ADD, this))
        {
        // add the new target to the master list
        gAlertOptions.AddTarget(newRecPtr);

        // the list has been edited, so refresh the tree branch display
        UpdateMachineTargetTreeDisplay(m_netMessageTreeHandle);
        }
    else
        {
        // release allocated memory
        delete  newRecPtr;
        }

}  // end of "CAlertOptsDlg::OnHandleNetMessageAdd"



// ==== OnHandleNetMessageListEdit ========================================
//
//  This function is called when the user selects the "edit network message
//  targets list" item from the email list's right menu (see the message map
//  at the top of this file).
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnHandleNetMessageListEdit(void)
{
    HandleNetMessageListEdit();

}  // end of "CAlertOptsDlg::OnHandleNetMessageListEdit"



// ==== OnHandleNLMAdd ====================================================
//
//  This function is called to add a new NLM record.
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnHandleNLMAdd(void)
{
    auto    CNLMTarget*         newRecPtr;

    try {
        newRecPtr = new CNLMTarget;
        }
    catch(CMemoryException)
        {
        }

    // handle the edit
    if (TRUE == HandleMachineTargetEdit(newRecPtr, EDIT_ADD, this))
        {
        // add the new target to the master list
        gAlertOptions.AddTarget(newRecPtr);

        // the list has been edited, so refresh the tree branch display
        UpdateMachineTargetTreeDisplay(m_nlmTreeHandle);
        }
    else
        {
        // release allocated memory
        delete  newRecPtr;
        }

}  // end of "CAlertOptsDlg::OnHandleNLMAdd"



// ==== OnHandleNLMListEdit ===============================================
//
//  This function is called when the user selects the "edit NLM targets
//  list" item from the email list's right menu (see the message map at the
//  top of this file).
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnHandleNLMListEdit(void)
{
    HandleNLMListEdit();

}  // end of "CAlertOptsDlg::OnHandleNLMListEdit"



// ==== OnHandleSMTPAdd ===================================================
//
//  This function is called when the user selects the "Add" item from the
//  popup menu when right-clicking on a child email alert target.  It
//  allocates space for a new email record, and if the user chooses to save
//  the new record, the display of the alert targets tree control is updated.
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnHandleSMTPAdd(void)
{
    // allocate a new email info record
    auto    BOOL            result;
    auto    CEMailTarget*   emailRecPtr;
    try {
        emailRecPtr = new CEMailTarget;
        }

    catch(CMemoryException)
        {
        }

    result = HandleSMTPEdit(emailRecPtr);
    if (TRUE == result)
        {
        // add the new email record to the master email list
                gAlertOptions.AddTarget(emailRecPtr);

        // update the tree display
        UpdateEmailTreeDisplay();
        }
    else
        {
        delete  emailRecPtr;
        }

}  // end of "CAlertOptsDlg::OnHandleSMTPAdd"

// ==== OnHandleSNMPAdd ===================================================
//
//  This function is called when the user selects the "Add" item from the
//  popup menu when right-clicking on a child email alert target.  It
//  allocates space for a new email record, and if the user chooses to save
//  the new record, the display of the alert targets tree control is updated.
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnHandleSNMPAdd(void)
{
    // allocate a new email info record
    auto    BOOL            result;
    auto    CSNMPTarget*    snmpRecPtr;
    try {
        snmpRecPtr = new CSNMPTarget;
        }

    catch(CMemoryException)
        {
        }

    result = HandleSNMPEdit(snmpRecPtr);
    if (TRUE == result)
        {
        // add the new email record to the master email list
                gAlertOptions.AddTarget(snmpRecPtr);

        UpdateMachineTargetTreeDisplay(m_SNMPTreeHandle);
        }
    else
        {
        delete  snmpRecPtr;
        }

}  // end of "CAlertOptsDlg::OnHandleSMTPAdd"



// ==== OnInitDialog ======================================================
//
//  This function basically initializes the tree control.
//
//  Input:  nothing
//
//  Output: a value of TRUE
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertOptsDlg::OnInitDialog(void)
{
    // enable for DBCS fonts if necessary (non-quake call)
//    NG_SetFontsInDialog (GetSafeHwnd(), 0);

    // call parent function
    CHelpPropertyPage::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
        //  when the application's main window is not a dialog
        SetIcon(m_hIcon, TRUE);                 // Set big icon
        SetIcon(m_hIcon, FALSE);                // Set small icon

        // TODO: Add extra initialization here

    // initialize the groupbox controls array (this array contains pointers to
    // each control contained in a group control; when context help is selected
    // on such an item and that item is grayed-out, this array is used to
    // identify the control; see the "OnContextMenu" handler for details)
    m_groupboxControls[0] = &m_addButton;
    m_groupboxControls[1] = &m_modifyButton;
    m_groupboxControls[2] = &m_removeButton;
    m_groupboxControls[3] = &m_scannerCheckbox;
    m_groupboxControls[4] = &m_apCheckbox;
    m_groupboxControls[5] = &m_alertTree;
    m_groupboxControls[6] = NULL;

        InitControls();

        PostMessage(WM_PASSWORD_CHECK, 0,0L);

    return TRUE;  // return TRUE  unless you set the focus to a control

}  // end of "CAlertOptsDlg::OnInitDialog"




// ==== OnModifyButton ====================================================
//
//  This function calls the parent or child item edit function, whichever
//  is appropriate for the currently-selected tree handle.
//
//  Input: nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 12/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnModifyButton(void)
{
        // TODO: Add your control notification handler code here

    // get the currently-selected tree handle and derive the alert type
    auto    HTREEITEM       treeHandle;
    treeHandle = m_alertTree.GetSelectedItem();
    if (NULL != treeHandle)
        {
        if (TRUE == IsParentTreeHandle(treeHandle))
            {
            HandleParentEdit(treeHandle);
            }
        else
            {
            HandleChildItemEdit(treeHandle);
            }
        }

}  // end of "CAlertOptsDlg::OnModifyButton"



// ==== OnPaint ===========================================================
//
// If you add a minimize button to your dialog, you will need the code
// below to draw the icon.  For MFC applications using the document/view
// model, this is automatically done for you by the framework.
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnPaint(void)
{
        if (IsIconic())
        {
                CPaintDC dc(this); // device context for painting

                SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

                // Center icon in client rectangle
                int cxIcon = GetSystemMetrics(SM_CXICON);
                int cyIcon = GetSystemMetrics(SM_CYICON);
                CRect rect;
                GetClientRect(&rect);
                int x = (rect.Width() - cxIcon + 1) / 2;
                int y = (rect.Height() - cyIcon + 1) / 2;

                // Draw the icon
                dc.DrawIcon(x, y, m_hIcon);
        }
        else
        {
        CHelpPropertyPage::OnPaint();
        }

}  // end of "CAlertOptsDlg::OnPaint"



// ==== OnQueryDragIcon ===================================================
//
// The system calls this to obtain the cursor to display while the user
// drags the minimized window.
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

HCURSOR CAlertOptsDlg::OnQueryDragIcon(void)
{
        return (HCURSOR) m_hIcon;

}  // end of "CAlertOptsDlg::OnQueryDragIcon"



// ==== OnRemoveButton ====================================================
//
//  This function calls the routine to remove a child tree control item.
//
//  Input: nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 12/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnRemoveButton(void)
{
        // TODO: Add your control notification handler code here

    auto    HTREEITEM       treeHandle;

    // get the currently-selected tree handle
    treeHandle = m_alertTree.GetSelectedItem();

    // if it's not a parent handle, call the child item removal function
    if (FALSE == IsParentTreeHandle(treeHandle))
        {
        HandleRemoveTreeChild(treeHandle);
        }

}  // end of "CAlertOptsDlg::OnRemoveButton"



// ==== OnRmenuForwardModify ==============================================
//
//  This function is called when the user selects the right-menu item to
//  modify this type of record.
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnRmenuForwardModify(void)
{
    if (NULL != m_rightClickHandle)
        {
        HandleForwardModify(m_rightClickHandle);
        }

}  // end of "CAlertOptsDlg::OnRmenuForwardModify"

// ==== OnRmenuSNMPModify ==============================================
//
//  This function is called when the user selects the right-menu item to
//  modify this type of record.
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnRmenuSNMPModify(void)
{
    if (NULL != m_rightClickHandle)
        {
        HandleSNMPModify(m_rightClickHandle);
        }

}  // end of "CAlertOptsDlg::OnRmenuForwardModify"




// ==== OnRmenuNetMessageModify ===========================================
//
//  This function is called when the user selects the right-menu item to
//  modify this type of record.
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnRmenuNetMessageModify(void)
{
    if (NULL != m_rightClickHandle)
        {
        HandleNetMessageModify(m_rightClickHandle);
        }

}  // end of "CAlertOptsDlg::OnRmenuNetMessageModify"



// ==== OnRmenuNLMModify ==================================================
//
//  This function is called when the user selects the right-menu item to
//  modify this type of record.
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnRmenuNLMModify(void)
{
    if (NULL != m_rightClickHandle)
        {
        HandleNLMModify(m_rightClickHandle);
        }

}  // end of "CAlertOptsDlg::OnRmenuNLMModify"



// ==== OnRmenuRemoveTreeChild ============================================
//
//  This function is called when the user chooses to remove a child tree
//  item via a right-menu selection.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnRmenuRemoveTreeChild(void)
{
    if (NULL != m_rightClickHandle)
        {
        HandleRemoveTreeChild(m_rightClickHandle);
        }

}  // end of "CAlertOptsDlg::OnRmenuRemoveTreeChild"



// ==== OnRmenuSMTPModify =================================================
//
//  This function is called when the user selects the "Modify" item from
//  the popup menu when right-clicking on a child email alert target.
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnRmenuSMTPModify(void)
{
    if (NULL != m_rightClickHandle)
        {
        HandleSMTPModify(m_rightClickHandle);
        }

}  // end of "CAlertOptsDlg::OnRmenuSMTPModify"



// ==== OnScannerCheckbox =================================================
//
//  This function is called when the user clicks on the windows scanner
//  checkbox from the main alert options dialog window.  If all the
//  checkboxes that enable remote alerts are cleared, then the tree control
//  is disabled to reflect that state.
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnScannerCheckbox(void)
{
        // TODO: Add your control notification handler code here

    if  ((0 == m_scannerCheckbox.GetCheck())
    &&  (0 == m_apCheckbox.GetCheck()))
        {
        m_alertTree.EnableWindow(FALSE);

        // reset this so if the "Configure" button is clicked, the
        // "CAlertTypesDlg" will pop up instead of a default edit dialog
        m_alertType = TARGET_NONE;
        }
    else
        {
        m_alertTree.EnableWindow(TRUE);
        }

    gAlertOptions.SetScannerRemoteAlertsEnabled(m_scannerCheckbox.GetCheck());

}  // end of "CAlertOptsDlg::OnScannerCheckbox"



// ==== OnSelchangedAlertTree =============================================
//
//  This function is called after the selected item in the tree control
//  changes.
//
//  Input:  the default MFC input arguments
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void CAlertOptsDlg::OnSelchangedAlertTree(NMHDR* pNMHDR, LRESULT* pResult)
{
    auto    NM_TREEVIEW*    pNMTreeView = (NM_TREEVIEW*)pNMHDR;
        // TODO: Add your control notification handler code here

    // process the selected item
    auto    HTREEITEM       treeHandle;
    treeHandle = m_alertTree.GetSelectedItem();
    if (NULL != treeHandle)
        {
        // update the alert type data member variable
        m_alertType = MapTreeHandleToAlertType(treeHandle);
        UpdateEditButtons(treeHandle);
        }

    *pResult = 0;

}  // end of "CAlertOptsDlg::OnSelchangedAlertTree"



// ==== OnSysCommand ======================================================
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{

    CHelpPropertyPage::OnSysCommand(nID, lParam);

}  // end of "CAlertOptsDlg::OnSysCommand"



// ==== OnWhatsThisAlertTree ==============================================
//
//  This function handles the "What's This" events for the items in the
//  alert targets tree control.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

void    CAlertOptsDlg::OnWhatsThisAlertTree(void)
{
        // TODO: Add your specialized code here and/or call the base class

    ::WinHelp (this->GetSafeHwnd(),  NAV_HELPFILENAME, HELP_CONTEXTPOPUP
                                    , IDH_NAVNT_WHATHIS_ALERT_TARGETS_LSTBX);

}  // end of "CAlertOptsDlg::OnWhatsThisAlertTree"


BOOL CAlertOptsDlg::GetData(LPNAVOPTIONS pOptions)
{
    // Save off options.
    gAlertOptions.SaveOptions();
        return TRUE;

}

BOOL CAlertOptsDlg::SetData(LPNAVOPTIONS pOptions)
{
                                        // Return TRUE.  This function does nothing
                                        // but needs to be implemented nontheless.
                                        // for compatiblity with N32OPTS
        return TRUE;
}


LRESULT CAlertOptsDlg::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
                                            // Set resources to point to this DLLs resource
                                            // chain.
        SetResourceHandleToThisModule();
        LRESULT lResult = CHelpPropertyPage::WindowProc( message, wParam, lParam);

        return lResult;
}


void CAlertOptsDlg::OnIpxCheck()
{
        UpdateData();
    gAlertOptions.SetIPXAlertsEnabled(m_IPXCheckBox);
}

void CAlertOptsDlg::OnSamCheck()
{
        UpdateData();
    gAlertOptions.SetATPAlertsEnabled(m_SAMCheckBox);
}

void CAlertOptsDlg::OnHandleSNMPListEdit()
{
        HandleSNMPListEdit();
}


BOOL CAlertOptsDlg::OnSetActive()
{
    BOOL bRet = CPropertyPage::OnSetActive();

    // If we have been disabled because the user did not enter the correct
    // password, give him another chance.
    if( m_bPassDisabled )
        {
        if( GetParent()->SendMessage(WM_PASSWORD_CHECK, PASSWORD_VIEWALERTOPT, 0L))
            {
            EnableChildWindows(TRUE);
            m_bPassDisabled = FALSE;
            }
        }

        return bRet;
}


void CAlertOptsDlg::EnableChildWindows(BOOL bEnable)
{

    if( bEnable == FALSE )
        {
        // Empty our list of children.
        m_WindowList.RemoveAll();

        // Build list of child windows and thier states...
        EnumChildWindows(GetSafeHwnd(), EnumProc, (LPARAM) this);
        }
    else
        {
        // Go thru list, enabling all windows.
        POSITION tPos = m_WindowList.GetHeadPosition();
        while(tPos != NULL)
            {
            CHILDWINDOW cw = m_WindowList.GetNext(tPos);
            ::EnableWindow(cw.hWnd, cw.bEnabled);
            }
        }
}

static BOOL CALLBACK EnumProc( HWND hwnd, LPARAM lParam )
{
    CAlertOptsDlg * pThis = (CAlertOptsDlg *)lParam;
    CHILDWINDOW cw;

    // Save off window state
    cw.bEnabled = IsWindowEnabled(hwnd);
    cw.hWnd = hwnd;

    // Add to list
    pThis->m_WindowList.AddTail(cw);

    // Disable the window.
    EnableWindow(hwnd, FALSE);

    return TRUE;

}

afx_msg LRESULT CAlertOptsDlg::OnPasswordCheck(WPARAM wParam, LPARAM lParam)
{

    // Make sure user has right to view this page.
    if( GetParent()->SendMessage(WM_PASSWORD_CHECK, PASSWORD_VIEWALERTOPT, 0L))
        {
        // See if we were previously disabled because the user did not enter
        // the correct password.
        if(m_bPassDisabled)
            {
            EnableChildWindows(TRUE);
            m_bPassDisabled = FALSE;
            }
        }
    else
        {
        // Disable all child windows untill the user enters the correct
        // password.
        m_bPassDisabled = TRUE;
        EnableChildWindows(FALSE);
        }

    return 0;
}

