/////////////////////////////////////////////////////////////////////
// NetMesPg.cpp : implementation file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/NETMESPG.CPv   1.2   17 Aug 1997 15:19:56   DBUCHES  $
//
// Description:
//
//  This file contains the property page for editing the net messenger
//  alerts list.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/NETMESPG.CPv  $
// 
//    Rev 1.2   17 Aug 1997 15:19:56   DBUCHES
// Fixed problem with image list mask value.
//
//    Rev 1.1   25 Apr 1997 11:03:30   JBRENNA
// Ported from Tesla.
//
//    Rev 1.0   13 Mar 1997 15:06:20   RFULLER
// Initial revision
//
//    Rev 1.17   05 Feb 1997 23:07:24   SEDWARD
//
//    Rev 1.16   27 Jan 1997 15:47:44   SEDWARD
// Fixed column creation in 'OnInitDialog'.
//
//    Rev 1.15   21 Jan 1997 19:17:08   SEDWARD
// Initialize m_clickedItemIndex in constructor, set to invalid value if
// no items are selected and the list control is clicked.
//
//    Rev 1.14   20 Jan 1997 15:13:40   SEDWARD
// Added How To IDs for modifying a single target, and editing the entire list.
//
//    Rev 1.13   19 Jan 1997 23:43:22   SEDWARD
// Updated help maps, implemented multiple selection/removal of list control
// items.
//
//    Rev 1.12   16 Jan 1997 02:12:38   SEDWARD
// Lots of updates in help maps and menus.
//
//    Rev 1.11   14 Jan 1997 20:36:54   SEDWARD
// New help ID values.
//
//    Rev 1.10   14 Jan 1997 13:23:26   SEDWARD
// Updated help maps with new IDs, added help button.
//
//    Rev 1.8   06 Jan 1997 00:40:36   SEDWARD
// Now uses temporary CAlertOptions list for edits.
//
//    Rev 1.6   09 Dec 1996 20:46:40   SEDWARD
//
//    Rev 1.5   03 Dec 1996 19:49:08   SEDWARD
// Substantial changes (not frozen yet...).
//
//    Rev 1.4   02 Dec 1996 16:33:42   SEDWARD
// More big changes, nearing UI freeze.
//
//    Rev 1.3   26 Nov 1996 17:21:44   SEDWARD
// Leading backslashes no longer written to DAT file, domains get a trailing
// space/asterisk appened to them.
//
//    Rev 1.2   25 Nov 1996 15:46:18   SEDWARD
// Major overhaul (more to come).
//
//    Rev 1.1   21 Nov 1996 17:07:38   SEDWARD
// Added vlog headers.
//
//    Rev 1.0   21 Nov 1996 16:55:06   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "AlertUI.h"
#include "AlertOpt.h"
#include "NetMesPg.h"
#include "netbrwsd.h"               // MFC net browser
#include "helpdlg.h"                // base class for default help support
#include "nvetwhlp.h"               // context help
#include "navutil.h"                // context help
#include "help_ids.h"               // help IDs


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// the "what's this" and "how to" maps
DWORD   CNetMessagePg::m_dwWhatsThisMap[] =
{
    IDC_NETMESSAGE_TARGET_LIST, IDH_NAVNT_WHATHIS_MESSERV_TARGETS_LSTBX
    , IDC_NETMESSAGE_ADD_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_ADD_ITEM_BTN
    , IDC_NETMESSAGE_REMOVE_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_REMOVE_ITEM_BTN
    , IDC_NETMESSAGE_MODIFY_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_MODIFY_ITEM_BTN
    , 0, 0
};



DWORD   CNetMessagePg::m_dwHowToMap[] =
{
    ID_HELP, IDH_NAVNT_MESSENGER_SERVICE_DLG_HELP_BTN
    , ID_HT_ADD_NETMESSENGER, IDH_NAVNT_TASK_ADD_A_NT_MESSENGER_SERVICE_TARGET
    , ID_HT_REMOVE_NETMESSENGER, IDH_NAVNT_TASK_REMOVE_AN_NT_MESSENGER_SERVICE_TARGET
    , ID_HT_EDITLIST_NETMESSENGER, IDH_NAVNT_TASK_EDIT_THE_NT_MESSENGER_SERVICE_TARGET_LIST
    , ID_HT_MODIFY_NETMESSENGER, IDH_NAVNT_TASK_EDIT_A_SINGLE_NT_MESSENGER_SERVICE_TARGET
    , 0, 0
};




/////////////////////////////////////////////////////////////////////////////
// CNetMessagePg property page
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CNetMessagePg, CHelpPropertyPage)

CNetMessagePg::CNetMessagePg() : CHelpPropertyPage(CNetMessagePg::IDD)
{
    //{{AFX_DATA_INIT(CNetMessagePg)
	//}}AFX_DATA_INIT

    // set the page's caption
    static    CString     tempStr;
    if (tempStr.LoadString(IDS_TARGET_PAGE_CAPTION))
        {
        m_psp.dwFlags |= PSP_USETITLE;
        m_psp.pszTitle = tempStr;
        }

    // activate the "help" button flag
    m_psp.dwFlags |= PSP_HASHELP;

    // initialize the temporary alert targets list pointer
    m_targetsListPtr = NULL;

    // set up the help map pointers
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_pHelpHowToMap = m_dwHowToMap;

    // initialize the list member index
    m_clickedItemIndex = -1;

    // initialize the "how to" index for this dialog
    SetHowToMenuIndex(HOW_TO_INDEX_NET_MESSENGER);
}



CNetMessagePg::~CNetMessagePg()
{
    if (NULL != m_targetsListPtr)
        {
        delete  m_targetsListPtr;
        }
}



void CNetMessagePg::DoDataExchange(CDataExchange* pDX)
{
    CHelpPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNetMessagePg)
    DDX_Control(pDX, IDC_NETMESSAGE_TARGET_LIST, m_netMessageList);
    DDX_Control(pDX, IDC_NETMESSAGE_REMOVE_BUTTON, m_netMessageRemoveButton);
    DDX_Control(pDX, IDC_NETMESSAGE_MODIFY_BUTTON, m_netMessageModifyButton);
    DDX_Control(pDX, IDC_NETMESSAGE_ADD_BUTTON, m_netMessageAddButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNetMessagePg, CHelpPropertyPage)
    //{{AFX_MSG_MAP(CNetMessagePg)
    ON_BN_CLICKED(IDC_NETMESSAGE_ADD_BUTTON, OnNetMessageAddButton)
    ON_BN_CLICKED(IDC_NETMESSAGE_REMOVE_BUTTON, OnNetMessageRemoveButton)
    ON_NOTIFY(NM_CLICK, IDC_NETMESSAGE_TARGET_LIST, OnClickNetMessageTargetList)
	ON_BN_CLICKED(IDC_NETMESSAGE_MODIFY_BUTTON, OnNetMessageModifyButton)
    ON_COMMAND(ID_ADD, OnHandleNetMessageAdd)
    ON_COMMAND(ID_MODIFY, OnHandleNetMessageModify)
    ON_COMMAND(ID_REMOVE, OnHandleNetMessageRemove)
	ON_NOTIFY(NM_DBLCLK, IDC_NETMESSAGE_TARGET_LIST, OnDblclkNetmessageTargetList)
	ON_WM_CONTEXTMENU()
    ON_COMMAND(IDR_WHATS_THIS_NETMESSAGE_LISTCTRL, OnWhatsThisNetMessageListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CNetMessagePg member functions
/////////////////////////////////////////////////////////////////////////////





// ===== HandleRclickNetMessageList =======================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CNetMessagePg::HandleRclickNetMessageList(void)
{
    // TODO: Add your control notification handler code here

    auto    DWORD       dwPos;
    auto    POINT       pointScreen;

    // get the screen coordinates
    dwPos = GetMessagePos();
	pointScreen.x = LOWORD (dwPos);
	pointScreen.y = HIWORD (dwPos);

    // update the item index member variable
    GetClickedItemIndex(m_netMessageList, m_clickedItemIndex);

    // update the edit buttons
    SynchronizeListCtrlAndEditButtons(m_netMessageList, m_netMessageAddButton
                                                , m_netMessageModifyButton
                                                , m_netMessageRemoveButton);

    // create the popup menu
    auto    CMenu       tempMenu;
    VERIFY(tempMenu.LoadMenu(IDR_RMENU_NETMESSAGEPAGE_TARGET));

    auto    CMenu       *popupMenu = tempMenu.GetSubMenu(0);
    ASSERT(popupMenu != NULL);

    // synchronize the menu items with their corresponding edit button states
    SynchronizeEditButtonsAndContextMenu(m_netMessageAddButton
                                                , m_netMessageModifyButton
                                                , m_netMessageRemoveButton
                                                , popupMenu);

    popupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON
                                                    , pointScreen.x
                                                    , pointScreen.y
                                                    , AfxGetMainWnd());

}  // end of "CNetMessagePg::HandleRclickNetMessageList"



// ===== InitListbox ======================================================
//
//  This function populates the listbox control with the current contents
//  of the master record list.
//
//  Input:  nothing
//  Output: the number of items added to the listbox
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

int     CNetMessagePg::InitListbox(void)
{
    auto    CAlertTarget*   pAlertTarget;
    auto    CString         tempStr;
    auto    int             numItems;

    // iterate through the master list and populate the listbox
    numItems = 0;
    m_targetsListPtr->ResetPosition();
    while (pAlertTarget = m_targetsListPtr->GetNextTarget(TARGET_MESSAGE_NETWORK))
        {
        if (-1 != AddMachineTargetListItem(m_netMessageList, pAlertTarget))
            {
            ++numItems;
            }
        }

    return (numItems);

}  // end of "CNetMessagePg::InitListbox"







/////////////////////////////////////////////////////////////////////////////
// CNetMessagePg message handlers
/////////////////////////////////////////////////////////////////////////////





// ===== OnClickNetMessageTargetList ======================================
//
//  This function handles the event where the user clicks on the listbox
//  control.  If an item's checkbox is hit, then its state is toggled.
//
//  Input:  the default MFC input arguments
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CNetMessagePg::OnClickNetMessageTargetList(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: Add your control notification handler code here

    auto    int         numSelectedItems;
    numSelectedItems = SynchronizeListCtrlAndEditButtons(m_netMessageList
                                                , m_netMessageAddButton
                                                , m_netMessageModifyButton
                                                , m_netMessageRemoveButton);

    if (0 == numSelectedItems)
        {
        // invalidate the member variable so we don't reference anything stupid
        m_clickedItemIndex = -1;
        }
    else if (1 == numSelectedItems)
        {
        auto    BOOL            iconHit;
        auto    CAlertTarget*   recPtr;

        // if an item was hit...
        iconHit = GetClickedItemIndex(m_netMessageList, m_clickedItemIndex);
        if  ((-1 != m_clickedItemIndex)
        &&  (TRUE == iconHit))
            {
            // ... toggle the checkbox
            auto    BOOL        checkedState;
            checkedState = ToggleListItemCheckbox(m_netMessageList
                                                    , m_clickedItemIndex);
            recPtr = (CAlertTarget*)m_netMessageList.GetItemData(
                                                    m_clickedItemIndex);
            if (NULL != recPtr)
                {
                recPtr->SetActive(checkedState);
                }
            }
        }

    *pResult = 0;

}  // end of "CNetMessagePg::OnClickNetMessageTargetList"



// ==== OnCommand =========================================================
//
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

BOOL    CNetMessagePg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

    // if the user clicked on a control, store a pointer to its window in the
    // current context variable so it'll be ready for context help
    if (lParam > 0)
        {
        m_pCurrentContextWnd = GetDlgItem(LOWORD(wParam));
        }

    return CHelpPropertyPage::OnCommand(wParam, lParam);

}  // end of "CNetMessagePg::OnCommand"



// ==== OnContextMenu =====================================================
//
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

void    CNetMessagePg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here


    // we'll handle the list control ourselves
    auto    int         winID;
    winID = pWnd->GetDlgCtrlID();
    if (IDC_NETMESSAGE_TARGET_LIST == winID)
        {
        HandleRclickNetMessageList();
        return;
        }

    CHelpPropertyPage::OnContextMenu(pWnd, point);

}  // end of "CNetMessagePg::OnContextMenu"



// ===== OnDblclkForwardTargetList ========================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

void    CNetMessagePg::OnDblclkNetmessageTargetList(NMHDR* pNMHDR
                                                        , LRESULT* pResult)
{
	// TODO: Add your control notification handler code here

    if (-1 != m_clickedItemIndex)
        {
        OnNetMessageModifyButton();
        }

	*pResult = 0;

}  // end of "CNetMessagePg::OnDblclkNetmessageTargetList"



// ===== OnHandleNetMessageAdd ============================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CNetMessagePg::OnHandleNetMessageAdd(void)
{
    // call the add function for the button
    OnNetMessageAddButton();

}  // end of "CNetMessagePg::OnHandleNetMessageAdd"



// ===== OnHandleNetMessageModify =========================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CNetMessagePg::OnHandleNetMessageModify(void)
{
    // call the modify function for the button
    OnNetMessageModifyButton();

}  // end of "CNetMessagePg::OnHandleNetMessageModify"



// ===== OnHandleNetMessageRemove =========================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CNetMessagePg::OnHandleNetMessageRemove(void)
{
    // call the remove function for the button
    OnNetMessageRemoveButton();

}  // end of "CNetMessagePg::OnHandleNetMessageRemove"



// ===== OnInitDialog =====================================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL CNetMessagePg::OnInitDialog(void)
{
    CHelpPropertyPage::OnInitDialog();

	// TODO: Add extra initialization here

    // the first column
    auto    LV_COLUMN       newCol;
    auto    int             result;
    newCol.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_WIDTH;
    newCol.fmt = LVCFMT_LEFT;
    newCol.cx = 1000;    // num pixels
    newCol.iSubItem = 0;
    result = m_netMessageList.InsertColumn(0, &newCol);
    if (-1 == result)
        {
        AfxMessageBox(_T("m_netMessageList.InsertColumn(Network Message Targets) failed"));
        }

    // create the image list object
    m_imageList.Create(IDB_ALERT_TREE, 16, 1, RGB(255, 0, 255));
    m_netMessageList.SetImageList(&m_imageList, LVSIL_SMALL);

    // create and initialize the temporary alert options list
    m_targetsListPtr = CreateTempAlertTargetsList(TARGET_MESSAGE_NETWORK);

    // initialize the contents of the list control
    InitListbox();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CNetMessagePg::OnInitDialog"



// ===== OnNetMessageAddButton ============================================
//
//  This function pops up the network browser so that the user can add a
//  new alert target record.  Any new records are added to the contents of
//  this object's listbox control.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CNetMessagePg::OnNetMessageAddButton(void)
{
    // TODO: Add your control notification handler code here

    auto    CNetMsgTarget*      newRecPtr;
	try
		{
        newRecPtr = new CNetMsgTarget;
		}
    catch(CMemoryException)
        {
        }

    // handle the edit
    if (TRUE == HandleMachineTargetEdit(newRecPtr, EDIT_ADD, this))
        {
        // add the new target to the list
        m_targetsListPtr->AddTarget(newRecPtr);

        // add to the list control
        AddMachineTargetListItem(m_netMessageList, newRecPtr);
        }
    else
        {
        // release allocated memory
        delete  newRecPtr;
        }

}  // end of "CNetMessagePg::OnNetMessageAddButton"



// ===== OnNetMessageModifyButton =========================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CNetMessagePg::OnNetMessageModifyButton(void)
{
	// TODO: Add your control notification handler code here

    auto    CNetMsgTarget*      recPtr;

    if (-1 != m_clickedItemIndex)
        {
        // get a pointer to the item's data
        recPtr = (CNetMsgTarget*)m_netMessageList.GetItemData(m_clickedItemIndex);
        if (TRUE == HandleMachineTargetEdit(recPtr, EDIT_MODIFY, this))
            {
            m_netMessageList.SetItemText(m_clickedItemIndex, 0, recPtr->GetAddress());
            }
        }

}  // end of "CNetMessagePg::OnNetMessageModifyButton"



// ===== OnNetMessageRemoveButton =========================================
//
//  This function is called when the user presses the "Remove" button to
//  remove an existing target record.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CNetMessagePg::OnNetMessageRemoveButton(void)
{
    // TODO: Add your control notification handler code here

    // if there are no selected items, return
    if (m_netMessageList.GetSelectedCount() <= 0)
        {
        return;
        }

    // remove selected items
    RemoveSelectedListCtrlItems(m_targetsListPtr, m_netMessageList);

    // if there are any items left in the list control, reset the current
    // item index
    if (m_netMessageList.GetItemCount() > 0)
        {
        m_clickedItemIndex = 0;
        }

    // update the edit buttons
    SynchronizeListCtrlAndEditButtons(m_netMessageList, m_netMessageAddButton
                                                , m_netMessageModifyButton
                                                , m_netMessageRemoveButton);

}  // end of "CNetMessagePg::OnNetMessageRemoveButton"



// ===== OnOK =============================================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CNetMessagePg::OnOK(void)
{
	// TODO: Add your specialized code here and/or call the base class

    UpdateMasterAlertTargetsList(TARGET_MESSAGE_NETWORK, m_targetsListPtr);

    CHelpPropertyPage::OnOK();

}  // end of "CNetMessagePg::OnOK"



// ==== OnWhatsThisNetMessageListCtrl =====================================
//
//  This function handles the "What's This" events for the items in the
//  list control.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

void    CNetMessagePg::OnWhatsThisNetMessageListCtrl(void)
{
    ::WinHelp (this->GetSafeHwnd(),  NAV_HELPFILENAME, HELP_CONTEXTPOPUP
                                , IDH_NAVNT_WHATHIS_MESSERV_TARGETS_LSTBX);

}  // end of "CNetMessagePg::OnWhatsThisNetMessageListCtrl"
