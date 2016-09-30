/////////////////////////////////////////////////////////////////////
// NLMPg.cpp : implementation file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/NLMPG.CPv   1.2   17 Aug 1997 15:19:58   DBUCHES  $
//
// Description:
//
//  This file contains the property page that allows a user to configure
//  the NLM alerts list.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/NLMPG.CPv  $
// 
//    Rev 1.2   17 Aug 1997 15:19:58   DBUCHES
// Fixed problem with image list mask value.
//
//    Rev 1.1   25 Apr 1997 11:03:26   JBRENNA
// Ported from Tesla.
//
//    Rev 1.0   13 Mar 1997 15:06:20   RFULLER
// Initial revision
//
//    Rev 1.19   02 Feb 1997 22:06:38   SEDWARD
// Fix for bug #80416.
//
//    Rev 1.18   27 Jan 1997 15:47:48   SEDWARD
// Fixed column creation in 'OnInitDialog'.
//
//    Rev 1.17   21 Jan 1997 19:17:38   SEDWARD
// Initialize m_clickedItemIndex in constructor, set to invalid value if
// no items are selected and the list control is clicked.
//
//    Rev 1.16   19 Jan 1997 23:43:26   SEDWARD
// Updated help maps, implemented multiple selection/removal of list control
// items.
//
//    Rev 1.15   16 Jan 1997 02:13:02   SEDWARD
// Lots of updates in help maps and menus.
//
//    Rev 1.14   14 Jan 1997 20:36:58   SEDWARD
// New help ID values.
//
//    Rev 1.13   14 Jan 1997 13:23:46   SEDWARD
// Updated help maps with new IDs, fixed class inheritance, added help button.
//
//    Rev 1.11   07 Jan 1997 15:08:14   SEDWARD
// The 'OnIncludeAllNlmsCheckbox' function handles edit button updates
// differently.
//
//    Rev 1.7   09 Dec 1996 20:47:04   SEDWARD
//
//    Rev 1.6   03 Dec 1996 19:48:46   SEDWARD
// Substantial changes (not frozen yet...).
//
//    Rev 1.5   02 Dec 1996 16:33:36   SEDWARD
// More big changes, nearing UI freeze.
//
//    Rev 1.4   26 Nov 1996 17:21:48   SEDWARD
// Leading backslashes no longer written to DAT file, domains get a trailing
// space/asterisk appened to them.
//
//    Rev 1.3   26 Nov 1996 00:56:18   SEDWARD
// More big changes.
//
//    Rev 1.2   25 Nov 1996 15:46:38   SEDWARD
// Major overhaul (more to come).
//
//    Rev 1.1   21 Nov 1996 17:07:40   SEDWARD
// Added vlog headers.
//
//    Rev 1.0   21 Nov 1996 16:55:06   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "AlertUI.h"
#include "AlertOpt.h"
#include "NLMPg.h"
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
DWORD   CNLMPg::m_dwWhatsThisMap[] =
{
    IDC_NLM_ADD_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_ADD_ITEM_BTN
    , IDC_NLM_MODIFY_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_MODIFY_ITEM_BTN
    , IDC_NLM_REMOVE_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_REMOVE_ITEM_BTN
    , IDC_INCLUDE_ALL_NLMS_CHECKBOX, IDH_NAVNT_ALERT_ALL_NAV_FOR_NETWARE_SERVERS
    , IDC_NLM_TARGET_LIST, IDH_NAVNT_WHATHIS_MESSERV_TARGETS_LSTBX
    , 0,0
};


DWORD   CNLMPg::m_dwHowToMap[] =
{
    ID_HELP, IDH_NAVNT_MESSENGER_SERVICE_DLG_HELP_BTN   // generic list help
    , ID_HT_ADD_NLM, IDH_NAVNT_TASK_ADD_A_NAV_FOR_NETWARE_ALERT_TARGET
    , ID_HT_INCLUDE_ALL_NLM, IDH_NAVNT_TASK_ADD_ALL_NAV_FOR_NETWARE_SERVERS_AS_TARGETS
    , ID_HT_MODIFY_NLM, IDH_NAVNT_TASK_EDIT_A_SINGLE_NAV_FOR_NETWARE_SERVER_TARGET
    , ID_HT_EDITLIST_NLM, IDH_NAVNT_TASK_EDIT_THE_NAV_FOR_NETWARE_TARGET_LIST
    , ID_HT_REMOVE_NLM, IDH_NAVNT_TASK_REMOVE_A_NAV_FOR_NETWARE_ALERT_TARGET
    , 0,0,
};


/////////////////////////////////////////////////////////////////////////////
// CNLMPg property page
/////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE(CNLMPg, CHelpPropertyPage)

CNLMPg::CNLMPg() : CHelpPropertyPage(CNLMPg::IDD)
{
	//{{AFX_DATA_INIT(CNLMPg)
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
    SetHowToMenuIndex(HOW_TO_INDEX_NLM);

}

CNLMPg::~CNLMPg()
{
    if (NULL != m_targetsListPtr)
        {
        delete  m_targetsListPtr;
        }
}

void CNLMPg::DoDataExchange(CDataExchange* pDX)
{
	CHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNLMPg)
	DDX_Control(pDX, IDC_INCLUDE_ALL_NLMS_CHECKBOX, m_allNLMCheckbox);
	DDX_Control(pDX, IDC_NLM_TARGET_LIST, m_nlmList);
	DDX_Control(pDX, IDC_NLM_REMOVE_BUTTON, m_nlmRemoveButton);
	DDX_Control(pDX, IDC_NLM_MODIFY_BUTTON, m_nlmModifyButton);
	DDX_Control(pDX, IDC_NLM_ADD_BUTTON, m_nlmAddButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNLMPg, CHelpPropertyPage)
	//{{AFX_MSG_MAP(CNLMPg)
	ON_BN_CLICKED(IDC_NLM_ADD_BUTTON, OnNlmAddButton)
	ON_NOTIFY(NM_CLICK, IDC_NLM_TARGET_LIST, OnClickNlmTargetList)
	ON_BN_CLICKED(IDC_NLM_REMOVE_BUTTON, OnNlmRemoveButton)
	ON_BN_CLICKED(IDC_INCLUDE_ALL_NLMS_CHECKBOX, OnIncludeAllNlmsCheckbox)
	ON_BN_CLICKED(IDC_NLM_MODIFY_BUTTON, OnNlmModifyButton)
    ON_COMMAND(ID_MODIFY, OnHandleNLMModify)
    ON_COMMAND(ID_ADD, OnHandleNLMAdd)
    ON_COMMAND(ID_REMOVE, OnHandleNLMRemove)
	ON_NOTIFY(NM_DBLCLK, IDC_NLM_TARGET_LIST, OnDblclkNlmTargetList)
	ON_WM_CONTEXTMENU()
    ON_COMMAND(IDR_WHATS_THIS_NLM_LISTCTRL, OnWhatsThisNLMListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()





/////////////////////////////////////////////////////////////////////////////
// CNLMPg member functions
/////////////////////////////////////////////////////////////////////////////



// ===== HandleRclickNLMTargetList ========================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CNLMPg::HandleRclickNLMTargetList(void)
{
    // TODO: Add your control notification handler code here

    auto    DWORD       dwPos;
    auto    POINT       pointScreen;

    // get the screen coordinates
    dwPos = GetMessagePos();
	pointScreen.x = LOWORD (dwPos);
	pointScreen.y = HIWORD (dwPos);

    // update the item index member variable
    GetClickedItemIndex(m_nlmList, m_clickedItemIndex);

    // update the edit buttons; if the master NLM checkbox is set, disable
    // all edit button, otherwise synchronize their states with the current
    // selection(s) in the list control
    if (1 == m_allNLMCheckbox.GetCheck())
        {
        m_nlmAddButton.EnableWindow(FALSE);
        m_nlmModifyButton.EnableWindow(FALSE);
        m_nlmRemoveButton.EnableWindow(FALSE);
        }
    else
        {
        SynchronizeListCtrlAndEditButtons(m_nlmList, m_nlmAddButton
                                                        , m_nlmModifyButton
                                                        , m_nlmRemoveButton);
        }

    // create the popup menu
    auto    CMenu       tempMenu;
    VERIFY(tempMenu.LoadMenu(IDR_RMENU_NLMPAGE_TARGET));

    auto    CMenu       *popupMenu = tempMenu.GetSubMenu(0);
    ASSERT(popupMenu != NULL);

    // synchronize the menu items with their corresponding edit button states
    SynchronizeEditButtonsAndContextMenu(m_nlmAddButton
                                                , m_nlmModifyButton
                                                , m_nlmRemoveButton
                                                , popupMenu);

    popupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON
                                                    , pointScreen.x
                                                    , pointScreen.y
                                                    , AfxGetMainWnd());

}  // end of "CNLMPg::HandleRclickNLMTargetList"



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

int     CNLMPg::InitListbox(void)
{
    auto    CAlertTarget*   pAlertTarget;
    auto    CString         tempStr;
    auto    int             numItems;

    // iterate through the master list and populate the listbox
    numItems = 0;
    m_targetsListPtr->ResetPosition();
    while (pAlertTarget = m_targetsListPtr->GetNextTarget(TARGET_NLM))
        {
        if (-1 != AddMachineTargetListItem(m_nlmList, pAlertTarget))
            {
            ++numItems;
            }
        }

    // if the "master" NLM item is in the listbox, make adjustments
    MasterNLMItemInit();

    return (numItems);

}  // end of "CNLMPg::InitListbox"



// ===== MasterNLMItemInit ================================================
//
//  Input:  nothing
//  Output: BOOL
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CNLMPg::MasterNLMItemInit(void)
{
    auto    BOOL        bResult = TRUE;
    auto    BOOL        foundMaster = FALSE;
    auto    CString     tempStr;
    auto    CString     searchStr;

    // see if the master NLM item is in the list control
    searchStr = DOUBLE_BACKSLASH;
    searchStr += MASTER_NLM_ITEM_STR;
    tempStr = m_nlmList.GetItemText(MASTER_NLM_ITEM_INDEX, 0);
    if (searchStr == tempStr)
        {
        foundMaster = TRUE;
        }

    if (TRUE == foundMaster)
        {
        // set the master item's display
        auto    LV_ITEM     masterItem;
        memset((void*)&masterItem, 0, sizeof(LV_ITEM));
        masterItem.mask = LVIF_TEXT | LVIF_IMAGE;
        masterItem.iItem = MASTER_NLM_ITEM_INDEX;
        masterItem.iSubItem = 0;
        masterItem.pszText = MASTER_NLM_ITEM_STR;
        masterItem.iImage = ATC_MASTER_NLM_BITMAP;
        if (0 == m_nlmList.SetItem(&masterItem))
            {
            bResult = FALSE;
            }
        else
            {
            // set the "All NLMs" checkbox
            m_allNLMCheckbox.SetCheck(1);

            // disable the edit buttons
            m_nlmAddButton.EnableWindow(FALSE);
            m_nlmRemoveButton.EnableWindow(FALSE);
            m_nlmModifyButton.EnableWindow(FALSE);

            // finally, update the other items ("TRUE" means the all NLMs
            // checkbox is checked)
            UpdateNLMItemIcons(TRUE);
            }
        }

    return (bResult);

}  // end of "CNLMPg::MasterNLMItemInit"



// ===== UpdateMasterNLMItem ==============================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL CNLMPg::UpdateMasterNLMItem(BOOL  checkedState)
{
    auto    BOOL            result = TRUE;

    if (0 == checkedState)
        {
        // the checkbox has been cleared, so remove the master NLM item...
        auto    CNLMTarget*       recPtr;
        recPtr = (CNLMTarget*)m_nlmList.GetItemData(MASTER_NLM_ITEM_INDEX);
        if (NULL != recPtr)
            {
            m_targetsListPtr->RemoveTarget(recPtr);
            }

        // remove the item from the list control
        result = m_nlmList.DeleteItem(MASTER_NLM_ITEM_INDEX);
        }
    else
        {
        // the checkbox has been set, so add the master NLM item...

        auto    CNLMTarget*     recPtr;
		try
			{
            recPtr = new CNLMTarget;
			}
        catch(CMemoryException)
            {
            }

		// add the new target to the master list
        auto    LV_ITEM         newItem;
        recPtr->SetAddress(MASTER_NLM_ITEM_STR);
        recPtr->SetPriority(1); // top priority
        m_targetsListPtr->AddTarget(recPtr);

        // initialize the list item structure
        memset((void*)&newItem, 0, sizeof(LV_ITEM));
        newItem.mask = LVIF_TEXT | LVIF_IMAGE;
        newItem.iItem = MASTER_NLM_ITEM_INDEX;
        newItem.iSubItem = 0;
        newItem.pszText = MASTER_NLM_ITEM_STR;
        newItem.iImage = ATC_MASTER_NLM_BITMAP;

        // insert the master NLM item in the list control
        if (-1 == m_nlmList.InsertItem(&newItem))
            {
            result = FALSE;
            }
        else
            {
            // finally, save a pointer to this item's corresponding record
            // with  the listbox item
            m_nlmList.SetItemData(0, (DWORD)recPtr);
            }
        }

    return (result);

}  // end of "CNLMPg::UpdateMasterNLMItem"



// ===== UpdateNLMItemIcons ===============================================
//
//  This function is called to update the display of all NLM items in the
//  list control.  If the "All NLMs" checkbox is checked, then gray checkbox
//  icons are used for the NLM list control items to indicate that they are
//  disabled.  If the "All NLMs" checkbox is not checked, then regular black
//  checkbox icons are used for the NLM list control items.
//
//  Input:
//      checkedState    -- the current state of the "All NLMs" checkbox
//
//  Output: the number of items whose icons have been reset
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

int CNLMPg::UpdateNLMItemIcons(BOOL  checkedState)
{
    auto    int         nIndex = 0;
    auto    int         numItems;
    auto    LV_ITEM     currentItem;


    // first see if there are any items to update
    numItems = m_nlmList.GetItemCount();
    if (numItems <= 0)
        {
        goto  allDone;
        }

    // initialize the list item structure
    memset((void*)&currentItem, 0, sizeof(LV_ITEM));
    currentItem.mask = LVIF_IMAGE;

    // loop and reset checkbox icons
    for (nIndex = 0; nIndex < numItems; ++nIndex)
        {
        currentItem.iItem = nIndex;
        currentItem.iSubItem = 0;
        if (0 != m_nlmList.GetItem(&currentItem))
            {
            // skip over the master NLM item if it's there
            if (ATC_MASTER_NLM_BITMAP == currentItem.iImage)
                {
                continue;
                }

            if (0 == checkedState)
                {
                // the "All NLMs" checkbox has been cleared, so use the
                // regular black icons
                if (ATC_GRAY_UNCHECKED_BITMAP == currentItem.iImage)
                    {
                    currentItem.iImage = ATC_UNCHECKED_BITMAP;
                    }
                else
                    {
                    currentItem.iImage = ATC_CHECKED_BITMAP;
                    }
                }
            else
                {
                // the "All NLMs" checkbox has been set, so use the gray icons
                if (ATC_UNCHECKED_BITMAP == currentItem.iImage)
                    {
                    currentItem.iImage = ATC_GRAY_UNCHECKED_BITMAP;
                    }
                else
                    {
                    currentItem.iImage = ATC_GRAY_CHECKED_BITMAP;
                    }
                }

            // change the item's checkbox icon
            m_nlmList.SetItem(&currentItem);
            }
        }

allDone:
    return (nIndex);

}  // end of "CNLMPg::UpdateNLMItemIcons"









/////////////////////////////////////////////////////////////////////////////
// CNLMPg message handlers
/////////////////////////////////////////////////////////////////////////////



// ===== OnClickNlmTargetList =============================================
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

void    CNLMPg::OnClickNlmTargetList(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: Add your control notification handler code here

    // if the master NLM checkbox is checked, disable the edit buttons
    if (1 == m_allNLMCheckbox.GetCheck())
        {
        m_nlmAddButton.EnableWindow(FALSE);
        m_nlmRemoveButton.EnableWindow(FALSE);
        m_nlmModifyButton.EnableWindow(FALSE);
        }
    else
        {
        auto    int         numSelectedItems;
        numSelectedItems = SynchronizeListCtrlAndEditButtons(m_nlmList
                                                        , m_nlmAddButton
                                                        , m_nlmModifyButton
                                                        , m_nlmRemoveButton);
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
            iconHit = GetClickedItemIndex(m_nlmList, m_clickedItemIndex);
            if  ((-1 != m_clickedItemIndex)
            &&  (TRUE == iconHit))
                {
                // ... toggle the checkbox
                auto    BOOL        checkedState;
                checkedState = ToggleListItemCheckbox(m_nlmList
                                                        , m_clickedItemIndex);
                recPtr = (CAlertTarget*)m_nlmList.GetItemData(m_clickedItemIndex);
                if (NULL != recPtr)
                    {
                    recPtr->SetActive(checkedState);
                    }
                }
            }
        }

    *pResult = 0;

}  // end of "CNLMPg::OnClickNlmTargetList"



// ==== OnCommand =========================================================
//
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

BOOL    CNLMPg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

    // if the user clicked on a control, store a pointer to its window in the
    // current context variable so it'll be ready for context help
    if (lParam > 0)
        {
        m_pCurrentContextWnd = GetDlgItem(LOWORD(wParam));
        }

    return CHelpPropertyPage::OnCommand(wParam, lParam);

}  // end of "CNLMPg::OnCommand"



// ==== OnContextMenu =====================================================
//
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

void    CNLMPg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here

    // we'll handle the list control ourselves
    auto    int         winID;
    winID = pWnd->GetDlgCtrlID();
    if (IDC_NLM_TARGET_LIST == winID)
        {
        HandleRclickNLMTargetList();
        return;
        }

    CHelpPropertyPage::OnContextMenu(pWnd, point);

}  // end of "CNLMPg::OnContextMenu"



// ===== OnDblclkNlmTargetList ============================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CNLMPg::OnDblclkNlmTargetList(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here

    if (-1 != m_clickedItemIndex)
        {
        OnNlmModifyButton();
        }

	*pResult = 0;

}  // end of "CNLMPg::OnDblclkNlmTargetList"



// ===== OnHandleNLMAdd ===================================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CNLMPg::OnHandleNLMAdd(void)
{
    // call the add function for the button
    OnNlmAddButton();

}  // end of "CNLMPg::OnHandleNLMAdd"



// ===== OnHandleNLMModify ================================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CNLMPg::OnHandleNLMModify(void)
{
    // call the modify function for the button
    OnNlmModifyButton();

}  // end of "CNLMPg::OnHandleNLMModify"



// ===== OnHandleNLMRemove ================================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CNLMPg::OnHandleNLMRemove(void)
{
    // call the remove function for the button
    OnNlmRemoveButton();

}  // end of "CNLMPg::OnHandleNLMRemove"



// ===== OnIncludeAllNlmsCheckbox =========================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CNLMPg::OnIncludeAllNlmsCheckbox(void)
{
	// TODO: Add your control notification handler code here


    // get the current state of the checkbox so we know what to do
    auto    int     bChecked;
    bChecked = m_allNLMCheckbox.GetCheck();

    // based on the checkbox state, add or remove the master NLM item
    UpdateMasterNLMItem(bChecked);

    // then update the other list control items (if any)
    UpdateNLMItemIcons(bChecked);

    // update the edit button states
    if (0 == bChecked)  // no master NLM item
        {
        m_nlmAddButton.EnableWindow(TRUE);
        m_nlmRemoveButton.EnableWindow(FALSE);
        m_nlmModifyButton.EnableWindow(FALSE);
        }
    else    // checkbox is checked
        {
        m_nlmAddButton.EnableWindow(FALSE);
        m_nlmRemoveButton.EnableWindow(FALSE);
        m_nlmModifyButton.EnableWindow(FALSE);
        }

}  // end of "CNLMPg::OnIncludeAllNlmsCheckbox"



// ===== OnInitDialog =====================================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL CNLMPg::OnInitDialog(void)
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
    result = m_nlmList.InsertColumn(0, &newCol);
    if (-1 == result)
        {
        AfxMessageBox(_T("m_nlmList.InsertColumn(NLM Target) failed"));
        }

    // create the image list object
    m_imageList.Create(IDB_ALERT_TREE, 16, 1, RGB(255, 0, 255));
    m_nlmList.SetImageList(&m_imageList, LVSIL_SMALL);

    // create and initialize the temporary alert options list
    m_targetsListPtr = CreateTempAlertTargetsList(TARGET_NLM);

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

}  // end of "CNLMPg::OnInitDialog"



// ===== OnNlmAddButton ===================================================
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

void    CNLMPg::OnNlmAddButton(void)
{
    // TODO: Add your control notification handler code here

    auto    CNLMTarget*     newRecPtr;

    try
        {
        newRecPtr = new CNLMTarget;
        }
    catch(CMemoryException)
        {
        }

    // handle the edit
    if (TRUE == HandleMachineTargetEdit(newRecPtr, EDIT_ADD, this))
        {
        // add the new target to the master list
        m_targetsListPtr->AddTarget(newRecPtr);

        // add to the list control
        AddMachineTargetListItem(m_nlmList, newRecPtr);
        }
    else
        {
        // release allocated memory
        delete  newRecPtr;
        }

}  // end of "CNLMPg::OnNlmAddButton"



// ===== OnNlmModifyButton ============================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CNLMPg::OnNlmModifyButton(void)
{
	// TODO: Add your control notification handler code here

    auto    CNLMTarget*     recPtr;

    // if the index value is valid and the user is not clicking on the
    // master NLM item...
    if (m_clickedItemIndex > -1)
        {
        // get a pointer to the item's data
        recPtr = (CNLMTarget*)m_nlmList.GetItemData(m_clickedItemIndex);
        if (TRUE == HandleMachineTargetEdit(recPtr, EDIT_MODIFY, this))
            {
            m_nlmList.SetItemText(m_clickedItemIndex, 0, recPtr->GetAddress());
            }
        }

}  // end of "CNLMPg::OnNlmModifyButton"



// ===== OnNlmRemoveButton ================================================
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

void    CNLMPg::OnNlmRemoveButton(void)
{
    // TODO: Add your control notification handler code here

    // if there are no selected items, return
    if (m_nlmList.GetSelectedCount() <= 0)
        {
        return;
        }

    // remove selected items
    RemoveSelectedListCtrlItems(m_targetsListPtr, m_nlmList);

    // if there are any items left in the list control, reset the current
    // item index
    if (m_nlmList.GetItemCount() > 0)
        {
        m_clickedItemIndex = 0;
        }

    // update the edit buttons
    SynchronizeListCtrlAndEditButtons(m_nlmList, m_nlmAddButton
                                                    , m_nlmModifyButton
                                                    , m_nlmRemoveButton);

}  // end of "CNLMPg::OnNlmRemoveButton"



// ===== OnOK =============================================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CNLMPg::OnOK(void)
{
	// TODO: Add your specialized code here and/or call the base class

    UpdateMasterAlertTargetsList(TARGET_NLM, m_targetsListPtr);

    CHelpPropertyPage::OnOK();

}  // end of "CNLMPg::OnOK"



// ==== OnWhatsThisNLMListCtrl ============================================
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

void    CNLMPg::OnWhatsThisNLMListCtrl(void)
{
    ::WinHelp (this->GetSafeHwnd(),  NAV_HELPFILENAME, HELP_CONTEXTPOPUP
                                , IDH_NAVNT_WHATHIS_MESSERV_TARGETS_LSTBX);

}  // end of "CNLMPg::OnWhatsThisNLMListCtrl"
