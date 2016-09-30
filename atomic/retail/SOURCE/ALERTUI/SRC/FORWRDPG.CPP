/////////////////////////////////////////////////////////////////////
// ForwrdPg.cpp : implementation file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/FORWRDPG.CPv   1.2   17 Aug 1997 15:19:56   DBUCHES  $
//
// Description:
//
//  This file contains the property page for editing the forward alerts list.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/FORWRDPG.CPv  $
// 
//    Rev 1.2   17 Aug 1997 15:19:56   DBUCHES
// Fixed problem with image list mask value.
//
//    Rev 1.1   25 Apr 1997 11:03:32   JBRENNA
// Ported from Tesla.
//
//    Rev 1.0   13 Mar 1997 15:06:20   RFULLER
// Initial revision
//
//    Rev 1.14   27 Jan 1997 15:47:16   SEDWARD
// Fixed column creation in 'OnInitDialog'.
//
//    Rev 1.13   21 Jan 1997 19:17:36   SEDWARD
// Initialize m_clickedItemIndex in constructor, set to invalid value if
// no items are selected and the list control is clicked.
//
//    Rev 1.12   19 Jan 1997 23:43:20   SEDWARD
// Updated help maps, implemented multiple selection/removal of list control
// items.
//
//    Rev 1.11   16 Jan 1997 02:13:56   SEDWARD
// Lots of updates in help maps and menus.
//
//    Rev 1.10   14 Jan 1997 13:23:12   SEDWARD
// Added help button.
//
//    Rev 1.8   06 Jan 1997 00:38:50   SEDWARD
// Now uses temporary CAlertOptions list for edits.
//
//    Rev 1.6   09 Dec 1996 20:47:22   SEDWARD
//
//    Rev 1.5   03 Dec 1996 19:48:52   SEDWARD
// Substantial changes (not frozen yet...).
//
//    Rev 1.4   02 Dec 1996 16:33:20   SEDWARD
// More big changes, nearing UI freeze.
//
//    Rev 1.3   26 Nov 1996 17:21:50   SEDWARD
// Leading backslashes no longer written to DAT file, domains get a trailing
// space/asterisk appened to them.
//
//    Rev 1.2   25 Nov 1996 15:46:26   SEDWARD
// Major overhaul (more to come).
//
//    Rev 1.1   21 Nov 1996 17:07:36   SEDWARD
// Added vlog headers.
//
//    Rev 1.0   21 Nov 1996 16:55:06   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "AlertUI.h"
#include "AlertOpt.h"
#include "ForwrdPg.h"
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
DWORD   CForwardPg::m_dwWhatsThisMap[] =
{
    IDC_FORWARD_ADD_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_ADD_ITEM_BTN
    , IDC_FORWARD_MODIFY_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_MODIFY_ITEM_BTN
    , IDC_FORWARD_REMOVE_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_REMOVE_ITEM_BTN
    , IDR_WHATS_THIS_FORWARD_LISTCTRL, IDH_NAVNT_WHATHIS_MESSERV_TARGETS_LSTBX
    , 0,0
};


DWORD   CForwardPg::m_dwHowToMap[] =
{
    ID_HELP, IDH_NAVNT_MESSENGER_SERVICE_DLG_HELP_BTN   // generic list help
    , ID_HT_ADD_FORWARD, IDH_NAVNT_TASK_ADD_A_RELAY_TO_REMOTE_MACHINE_TARGET
    , ID_HT_MODIFY_FORWARD, IDH_NAVNT_TASK_EDIT_A_SINGLE_RELAY_TO_REMOTE_MACHINE_TARGET
    , ID_HT_EDITLIST_FORWARD, IDH_NAVNT_TASK_EDIT_THE_RELAY_TO_REMOTE_MACHINES_TARGET_LIST
    , ID_HT_REMOVE_FORWARD, IDH_NAVNT_TASK_REMOVE_A_RELAY_TO_REMOTE_MACHINES_TARGET
    , 0,0
};




/////////////////////////////////////////////////////////////////////////////
// CForwardPg property page
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CForwardPg, CHelpPropertyPage)

CForwardPg::CForwardPg() : CHelpPropertyPage(CForwardPg::IDD)
{
	//{{AFX_DATA_INIT(CForwardPg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    // set the caption for the dialog/page
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
    SetHowToMenuIndex(HOW_TO_INDEX_FORWARD);
}


CForwardPg::~CForwardPg()
{
    if (NULL != m_targetsListPtr)
        {
        delete  m_targetsListPtr;
        }
}

void CForwardPg::DoDataExchange(CDataExchange* pDX)
{
    CHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CForwardPg)
	DDX_Control(pDX, IDC_FORWARD_TARGET_LIST, m_forwardList);
	DDX_Control(pDX, IDC_FORWARD_REMOVE_BUTTON, m_forwardRemoveButton);
	DDX_Control(pDX, IDC_FORWARD_MODIFY_BUTTON, m_forwardModifyButton);
	DDX_Control(pDX, IDC_FORWARD_ADD_BUTTON, m_forwardAddButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CForwardPg, CHelpPropertyPage)
	//{{AFX_MSG_MAP(CForwardPg)
	ON_BN_CLICKED(IDC_FORWARD_ADD_BUTTON, OnForwardAddButton)
	ON_BN_CLICKED(IDC_FORWARD_REMOVE_BUTTON, OnForwardRemoveButton)
	ON_NOTIFY(NM_CLICK, IDC_FORWARD_TARGET_LIST, OnClickForwardTargetList)
	ON_BN_CLICKED(IDC_FORWARD_MODIFY_BUTTON, OnForwardModifyButton)
    ON_COMMAND(ID_MODIFY, OnHandleForwardModify)
    ON_COMMAND(ID_ADD, OnHandleForwardAdd)
    ON_COMMAND(ID_REMOVE, OnHandleForwardRemove)
	ON_NOTIFY(NM_DBLCLK, IDC_FORWARD_TARGET_LIST, OnDblclkForwardTargetList)
	ON_WM_CONTEXTMENU()
    ON_COMMAND(IDR_WHATS_THIS_FORWARD_LISTCTRL, OnWhatsThisForwardListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()





/////////////////////////////////////////////////////////////////////////////
// CForwardPg member functions
/////////////////////////////////////////////////////////////////////////////


// ===== HandleRclickForwardTargetList ====================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CForwardPg::HandleRclickForwardTargetList(void)
{
    // TODO: Add your control notification handler code here


    auto    DWORD       dwPos;
    auto    POINT       pointScreen;

    // get the screen coordinates
    dwPos = GetMessagePos();
	pointScreen.x = LOWORD (dwPos);
	pointScreen.y = HIWORD (dwPos);

    // update the item index member variable
    GetClickedItemIndex(m_forwardList, m_clickedItemIndex);

    // update the edit buttons
    SynchronizeListCtrlAndEditButtons(m_forwardList, m_forwardAddButton
                                                    , m_forwardModifyButton
                                                    , m_forwardRemoveButton);

    // create the popup menu
    auto    CMenu       tempMenu;
    VERIFY(tempMenu.LoadMenu(IDR_RMENU_FORWARDPAGE_TARGET));

    auto    CMenu       *popupMenu = tempMenu.GetSubMenu(0);
    ASSERT(popupMenu != NULL);

    // synchronize the menu items with their corresponding edit button states
    SynchronizeEditButtonsAndContextMenu(m_forwardAddButton
                                                , m_forwardModifyButton
                                                , m_forwardRemoveButton
                                                , popupMenu);

    popupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON
                                                    , pointScreen.x
                                                    , pointScreen.y
                                                    , AfxGetMainWnd());

}  // end of "CForwardPg::HandleRclickForwardTargetList"



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

int     CForwardPg::InitListbox(void)
{
    auto    CAlertTarget*   pAlertTarget;
    auto    CString         tempStr;
    auto    int             numItems;

    // iterate through the master list and populate the listbox
    numItems = 0;
    m_targetsListPtr->ResetPosition();
    while (pAlertTarget = m_targetsListPtr->GetNextTarget(TARGET_FORWARD))
        {
        if (-1 != AddMachineTargetListItem(m_forwardList, pAlertTarget))
            {
            ++numItems;
            }
        }

    return (numItems);

}  // end of "CForwardPg::InitListbox"







/////////////////////////////////////////////////////////////////////////////
// CForwardPg message handlers
/////////////////////////////////////////////////////////////////////////////




// ===== OnClickForwardTargetList =========================================
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

void    CForwardPg::OnClickForwardTargetList(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: Add your control notification handler code here

    auto    int         numSelectedItems;
    numSelectedItems = SynchronizeListCtrlAndEditButtons(m_forwardList
                                                , m_forwardAddButton
                                                , m_forwardModifyButton
                                                , m_forwardRemoveButton);

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
        iconHit = GetClickedItemIndex(m_forwardList, m_clickedItemIndex);
        if  ((-1 != m_clickedItemIndex)
        &&  (TRUE == iconHit))
            {
            // ... toggle the checkbox
            auto    BOOL        checkedState;
            checkedState = ToggleListItemCheckbox(m_forwardList
                                                    , m_clickedItemIndex);
            recPtr = (CAlertTarget*)m_forwardList.GetItemData(
                                                    m_clickedItemIndex);
            if (NULL != recPtr)
                {
                recPtr->SetActive(checkedState);
                }
            }
        }

    *pResult = 0;

}  // end of "CForwardPg::OnClickForwardTargetList"



// ==== OnCommand =========================================================
//
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

BOOL    CForwardPg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

    // if the user clicked on a control, store a pointer to its window in the
    // current context variable so it'll be ready for context help
    if (lParam > 0)
        {
        m_pCurrentContextWnd = GetDlgItem(LOWORD(wParam));
        }

    return CHelpPropertyPage::OnCommand(wParam, lParam);

}  // end of "CForwardPg::OnCommand"



// ==== OnContextMenu =====================================================
//
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

void    CForwardPg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here

    // we'll handle the list control ourselves
    auto    int         winID;
    winID = pWnd->GetDlgCtrlID();
    if (IDC_FORWARD_TARGET_LIST == winID)
        {
        HandleRclickForwardTargetList();
        return;
        }

    CHelpPropertyPage::OnContextMenu(pWnd, point);

}  // end of "CForwardPg::OnContextMenu"



// ===== OnDblclkForwardTargetList ========================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

void    CForwardPg::OnDblclkForwardTargetList(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here

    // if an item was hit...
    if (-1 != m_clickedItemIndex)
        {
        OnForwardModifyButton();
        }

	*pResult = 0;

}  // end of "CForwardPg::OnDblclkForwardTargetList"



// ===== OnForwardAddButton ===============================================
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

void    CForwardPg::OnForwardAddButton(void)
{
    // TODO: Add your control notification handler code here

    auto    CForwardTarget*     newRecPtr;
    try
        {
        newRecPtr = new CForwardTarget;
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
        AddMachineTargetListItem(m_forwardList, newRecPtr);
        }
    else
        {
        // release allocated memory
        delete  newRecPtr;
        }

}  // end of "CForwardPg::OnForwardAddButton"



// ===== OnForwardModifyButton ============================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CForwardPg::OnForwardModifyButton(void)
{
	// TODO: Add your control notification handler code here

    auto    CForwardTarget*     recPtr;

    if (-1 != m_clickedItemIndex)
        {
        // get a pointer to the item's data
        recPtr = (CForwardTarget*)m_forwardList.GetItemData(m_clickedItemIndex);
        if (TRUE == HandleMachineTargetEdit(recPtr, EDIT_MODIFY, this))
            {
            m_forwardList.SetItemText(m_clickedItemIndex, 0, recPtr->GetAddress());
            }
        }

}  // end of "CForwardPg::OnForwardModifyButton"



// ===== OnForwardRemoveButton ============================================
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

void    CForwardPg::OnForwardRemoveButton(void)
{
    // TODO: Add your control notification handler code here

    // if there are no selected items, return
    if (m_forwardList.GetSelectedCount() <= 0)
        {
        return;
        }

    // remove selected items
    RemoveSelectedListCtrlItems(m_targetsListPtr, m_forwardList);

    // if there are any items left in the list control, reset the current
    // item index
    if (m_forwardList.GetItemCount() > 0)
        {
        m_clickedItemIndex = 0;
        }

    // update the edit buttons
    SynchronizeListCtrlAndEditButtons(m_forwardList, m_forwardAddButton
                                                    , m_forwardModifyButton
                                                    , m_forwardRemoveButton);

}  // end of "CForwardPg::OnForwardRemoveButton"



// ===== OnHandleForwardAdd ===============================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CForwardPg::OnHandleForwardAdd(void)
{
    // call the modify function for the button
    OnForwardAddButton();

}  // end of "CForwardPg::OnHandleForwardAdd"



// ===== OnHandleForwardModify ============================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CForwardPg::OnHandleForwardModify(void)
{
    // call the modify function for the button
    OnForwardModifyButton();

}  // end of "CForwardPg::OnHandleForwardModify"



// ===== OnHandleForwardRemove ============================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CForwardPg::OnHandleForwardRemove(void)
{
    // call the modify function for the button
    OnForwardRemoveButton();

}  // end of "CForwardPg::OnHandleForwardRemove"



// ===== OnInitDialog =====================================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CForwardPg::OnInitDialog(void)
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
    result = m_forwardList.InsertColumn(0, &newCol);
    if (-1 == result)
        {
        AfxMessageBox(_T("m_forwardList.InsertColumn(Forward Target) failed"));
        }

    // create the image list object
    m_imageList.Create(IDB_ALERT_TREE, 16, 1, RGB(255, 0, 255));
    m_forwardList.SetImageList(&m_imageList, LVSIL_SMALL);

    // create and initialize the temporary alert options list
    m_targetsListPtr = CreateTempAlertTargetsList(TARGET_FORWARD);

    // initialize the contents of the list control
    InitListbox();

    // set the page's caption
    auto    CString     tempStr;
    if (tempStr.LoadString(IDS_TARGET_PAGE_CAPTION))
        {
        SetWindowText(tempStr);
        }

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CForwardPg::OnInitDialog"



// ===== OnOK =============================================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CForwardPg::OnOK(void)
{
	// TODO: Add your specialized code here and/or call the base class

    UpdateMasterAlertTargetsList(TARGET_FORWARD, m_targetsListPtr);

    CHelpPropertyPage::OnOK();

}  // end of "CForwardPg::OnOK"



// ==== OnWhatsThisForwardListCtrl ========================================
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

void    CForwardPg::OnWhatsThisForwardListCtrl(void)
{
    ::WinHelp (this->GetSafeHwnd(), NAV_HELPFILENAME, HELP_CONTEXTPOPUP
                                , IDH_NAVNT_WHATHIS_MESSERV_TARGETS_LSTBX);

}  // end of "CForwardPg::OnWhatsThisForwardListCtrl"
