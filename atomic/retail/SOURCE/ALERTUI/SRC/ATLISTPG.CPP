/////////////////////////////////////////////////////////////////////
// ATListPg.cpp : implementation file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/atlistpg.cpv   1.3   31 Jul 1998 12:01:12   DBuches  $
//
// Description:
//      This class is used to handle most of the UI tasks associated with
//      the alert target property pages for NAV.  Since it is inherited
//      from "CAlertPropertyPage", it automatically contains functionality
//      needed to interface with the "CAlertOptions" class.  This class
//      does most of the "grunt-work" for UI, providing virtual functions
//      to be implemented by derived classes for more detailed tasks.
//
//      In order for a derived class to effectively use this class, the derived
//      class needs to do the following:
//
//          *) initialize the alert type data member
//          *) define help map arrays and initialize the context help map pointers
//          *) initialize the "how to" index for this dialog by calling
//             SetHowToMenuIndex()
//          *) initialize some control IDs
//             -- m_listCtrlContextMenuID
//             -- m_listCtrlWhatsThisMenuID
//             -- m_listCtrlWhatsThisHelpID
//             -- m_listCtrlID
//          *) initialize pointers to some base controls
//             -- m_listCtrlPtr
//             -- m_addButtonPtr
//             -- m_modifyButtonPtr
//             -- m_removeButtonPtr
//          *) initialize the column header arrays
//
//      See any of the derived alert target property pages for examples.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/atlistpg.cpv  $
// 
//    Rev 1.3   31 Jul 1998 12:01:12   DBuches
// Fixed problem with double clicking on the list.  Make sure we are on an item before calling OnModify.
//
//    Rev 1.2   17 Aug 1997 15:19:56   DBUCHES
// Fixed problem with image list mask value.
//
//    Rev 1.1   25 Apr 1997 11:04:22   JBRENNA
// Ported from Tesla.
//
//    Rev 1.0   15 Apr 1997 20:08:16   sedward
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AlertUI.h"
#include "AlertOpt.h"
#include "ATPropPg.h"
#include "ATListPg.h"
#include "help_ids.h"               // help IDs


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// the "what's this" and "how to" maps
DWORD   dwWhatsThisMap[] =
{
    IDC_ADD_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_ADD_ITEM_BTN
    , IDC_MODIFY_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_MODIFY_ITEM_BTN
    , IDC_REMOVE_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_REMOVE_ITEM_BTN
    , IDC_ALERTS_TARGETS_LIST, IDH_NAVNT_WHATHIS_MESSERV_TARGETS_LSTBX  // default
    , 0,0
};


DWORD   dwHowToMap[] =
{
    ID_HELP, IDH_NAVNT_MESSENGER_SERVICE_DLG_HELP_BTN   // generic list help
    , 0,0,
};



/////////////////////////////////////////////////////////////////////////////
// CAlertListPropertyPage property page
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CAlertListPropertyPage, CAlertPropertyPage)

CAlertListPropertyPage::CAlertListPropertyPage()
    : CAlertPropertyPage()
{
    m_totalDisplayStrings = 0;
    m_pHelpWhatsThisMap = dwWhatsThisMap;
    m_pHelpHowToMap = dwHowToMap;
    m_listCtrlID = IDC_ALERTS_TARGETS_LIST;
    m_listCtrlContextMenuID = 0;    // derived classes need to initialize this!
    m_listCtrlWhatsThisHelpID = 0;  // derived classes need to initialize this!
}



CAlertListPropertyPage::CAlertListPropertyPage(UINT  dlgID)
    : CAlertPropertyPage(dlgID)
{
    m_totalDisplayStrings = 0;
    m_pHelpWhatsThisMap = dwWhatsThisMap;
    m_pHelpHowToMap = dwHowToMap;
    m_listCtrlID = IDC_ALERTS_TARGETS_LIST;
    m_listCtrlContextMenuID = 0;    // derived classes need to initialize this!
    m_listCtrlWhatsThisHelpID = 0;  // derived classes need to initialize this!
}



CAlertListPropertyPage::~CAlertListPropertyPage()
{
}

void CAlertListPropertyPage::DoDataExchange(CDataExchange* pDX)
{
    CAlertPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAlertListPropertyPage)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlertListPropertyPage, CAlertPropertyPage)
    //{{AFX_MSG_MAP(CAlertListPropertyPage)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(NM_CLICK, IDC_ALERTS_TARGETS_LIST, OnClickAlertsTargetsList)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_MODIFY_BUTTON, OnModifyButton)
	ON_BN_CLICKED(IDC_REMOVE_BUTTON, OnRemoveButton)
    ON_COMMAND(ID_ADD, OnHandleAdd)
    ON_COMMAND(ID_MODIFY, OnHandleModify)
    ON_COMMAND(ID_REMOVE, OnHandleRemove)
	ON_NOTIFY(NM_DBLCLK, IDC_ALERTS_TARGETS_LIST, OnDblclkAlertsTargetsList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()





/////////////////////////////////////////////////////////////////////////////
// CAlertListPropertyPage member functions
/////////////////////////////////////////////////////////////////////////////



// ===== AddTargetToListCtrl ==============================================
//
//  This function adds a new alert target item to the list control, using the
//  input argument.
//
//  This function adds a new alert target item to the list control.  If the mode
//  argument specifies an "add" operation, then a new item is added to the list
//  control.  If the mode argument specifies a "modify" argument, then the item
//  referenced by "m_clickedItemIndex" is refreshed.
//
//  Input:
//      pAlertTarget    -- a pointer to the alert target
//      editMode        -- a flag indicating the edit mode (ie, "add" or "modify")
//
//  Output:
//      the target item's zero-based position in the list control
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

int     CAlertListPropertyPage::AddTargetToListCtrl(CAlertTarget*  pAlertTarget
                                                                , int  editMode)
{
	auto	CString				tempStr;
    auto    int                 newItemIndex;
    auto    LV_ITEM             newItem;

    // initialize the list item structure
    memset((void*)&newItem, 0, sizeof(LV_ITEM));
    newItem.mask = LVIF_TEXT | LVIF_IMAGE;
    newItem.iSubItem = 0;

    // insert the item display string and its checkbox
    GetDisplayString(pAlertTarget, tempStr, 0); // (virtual function)
    if (FALSE == tempStr.IsEmpty())
        {
        newItem.pszText = tempStr.GetBuffer(LIST_ITEM_MAX_STRLEN);
        if (TRUE == TargetIsActive(pAlertTarget))   // (virtual function)
            {
            newItem.iImage = ATC_CHECKED_BITMAP;
            }
        else
            {
            newItem.iImage = ATC_UNCHECKED_BITMAP;
            }

        // if we're updating an existing record, just refresh its display;
        // else, insert a new item
        if (EDIT_MODIFY == editMode)
            {
            newItemIndex = m_clickedItemIndex;
            newItem.iItem = newItemIndex;
            m_listCtrlPtr->SetItem(&newItem);
            }
        else
            {
            newItemIndex = m_listCtrlPtr->GetItemCount();
            newItem.iItem = newItemIndex;
            newItemIndex = m_listCtrlPtr->InsertItem(&newItem);
            }

        if (-1 != newItemIndex)
            {
            // save a pointer to this item's corresponding record with the
            // listbox item
            m_listCtrlPtr->SetItemData(newItemIndex, (DWORD)pAlertTarget);
            }
        }

    // if there are any substrings to be displayed, insert them as well
    if  ((m_totalDisplayStrings > 1)  &&  (-1 != newItemIndex))
        {
        for (int  index = 1; index < m_totalDisplayStrings; ++index)
            {
            GetDisplayString(pAlertTarget, tempStr, index); // (virtual function)
            if (FALSE == tempStr.IsEmpty())
                {
                newItem.mask = LVIF_TEXT;
                newItem.iItem = newItemIndex;
                newItem.iSubItem = index;
                newItem.pszText = tempStr.GetBuffer(MAX_LIST_SUBSTRING_LEN);
                m_listCtrlPtr->SetItem(&newItem);
                }
            }
        }

    return  (newItemIndex);

}  // end of "CAlertListPropertyPage::AddTargetToListCtrl"



// ===== GetClickedItemIndex ==============================================
//
//  This function sets this object's member variable with the index of the
//  most recently clicked item.  If the hit test is not successful, then
//  the member variable is initialized with an invalid value.  Furthermore,
//  it the item's icon was hit (ie, the checkbox), then a value of TRUE is
//  returned.
//
//  Input:  nothing
//  Output: a value of TRUE if an item's checkbox was hit, FALSE otherwise.
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

BOOL    CAlertListPropertyPage::GetClickedItemIndex(void)
{
    auto    BOOL        iconClicked = TRUE;
    auto    UINT        flags = 0;
    auto    POINT       pointScreen;
    auto    POINT       pointLVClient;
    auto    DWORD       dwPos;

    // get the screen coordinates
    dwPos = GetMessagePos();
	pointScreen.x = LOWORD (dwPos);
	pointScreen.y = HIWORD (dwPos);
	pointLVClient = pointScreen;

    // convert the point from screen to client coordinates,
    // relative to this control
    m_listCtrlPtr->ScreenToClient(&pointLVClient);

    // update this object's member variable
    m_clickedItemIndex = m_listCtrlPtr->HitTest(pointLVClient, &flags);
    if (-1 != m_clickedItemIndex)
        {
        // if the click did not occur on the checkbox, return FALSE
        if (0 == (flags & LVHT_ONITEMICON))
            {
            iconClicked = FALSE;
            }
        }

    return (iconClicked);

}  // end of "CAlertListPropertyPage::GetClickedItemIndex"



// ===== GetDisplayString =================================================
//
//  This virtual function does nothing, its implementation is left up to
//  derived classes.
//
//  Input:
//      pAlertTarget    -- a pointer to the alert target
//      displayStr      -- a reference to the display string
//      index           -- an index to the target display string
//
//  Output: a value of TRUE
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

BOOL    CAlertListPropertyPage::GetDisplayString(CAlertTarget*  pAlertTarget
                                                        , CString&  displayStr
                                                        , int  index)
{
    return (TRUE);

}  // end of "CAlertListPropertyPage::GetDisplayString"


// ===== HandleListCtrlWhatsThis ==========================================
//
//  This function handles the "what's this" help request for the list control.
//  The help ID used is whatever the derived class has initialized it to.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

void    CAlertListPropertyPage::HandleListCtrlWhatsThis(void)
{
    if (0 != m_listCtrlWhatsThisHelpID)
        {
        ::WinHelp(this->GetSafeHwnd(), NAV_HELPFILENAME, HELP_CONTEXTPOPUP
                                                , m_listCtrlWhatsThisHelpID);
        }

}  // end of "CAlertListPropertyPage::HandleListCtrlWhatsThis"



// ===== HandleRclickListCtrl =============================================
//
//  This function handles the right-click in the property page.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

void    CAlertListPropertyPage::HandleRclickListCtrl(void)
{
    // TODO: Add your control notification handler code here

    auto    DWORD       dwPos;
    auto    POINT       pointScreen;

    // get the screen coordinates
    dwPos = GetMessagePos();
	pointScreen.x = LOWORD (dwPos);
	pointScreen.y = HIWORD (dwPos);

    // update the item index member variable
    GetClickedItemIndex();

    // update the edit buttons
    SynchronizeListCtrlAndEditButtons();

    // create the popup menu
    auto    CMenu       tempMenu;
    VERIFY(tempMenu.LoadMenu(m_listCtrlContextMenuID));

    auto    CMenu       *popupMenu = tempMenu.GetSubMenu(0);
    ASSERT(popupMenu != NULL);

    // synchronize the menu items with their corresponding edit button states
    SynchronizeEditButtonsAndContextMenu(popupMenu);

    popupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON
                                                    , pointScreen.x
                                                    , pointScreen.y
                                                    , AfxGetMainWnd());

}  // end of "CAlertListPropertyPage::HandleRclickListCtrl"



// ==== HandleTargetEdit ==================================================
//
//  This virtual function does nothing, its implementation is left up to
//  derived classes.
//
//  Input:
//      pAlertTarget    -- a pointer to the alert target
//      mode            -- an integer to flag the mode (ie, add or modify)
//
//  Output: a value of FALSE
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

BOOL    CAlertListPropertyPage::HandleTargetEdit(CAlertTarget*  pAlertTarget
                                                                , int  mode)
{
    return  (FALSE);    // derived classes must implement this function!

}  // end of "CAlertListPropertyPage::HandleTargetEdit"



// ===== InitListbox ======================================================
//
//  This function initializes the list control with the contents of the
//  temporary alert targets list (pointed to by the member variable).
//
//  Input:  nothing
//  Output: the number of items added to the listbox
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

int     CAlertListPropertyPage::InitListbox(void)
{
    auto    CAlertTarget*   pAlertTarget;
    auto    CString         tempStr;
    auto    int             numItems;

    // iterate through the master list and populate the listbox
    numItems = 0;
    m_targetsListPtr->ResetPosition();
    while (pAlertTarget = m_targetsListPtr->GetNextTarget(m_alertType))
        {
        if (-1 != AddTargetToListCtrl(pAlertTarget))
            {
            ++numItems;
            }
        }

    return (numItems);

}  // end of "CAlertListPropertyPage::InitListbox"



// === RemoveSelectedListCtrlItems ==========================================
//
//  This function iterates through the list control member and removes all
//  selected items.  Additionally, the item's corresponding alert target
//  record is removed from the targets list member.
//
//  Input:
//      nothing
//
//  Output:
//      the total number of items removed
//
// ==========================================================================
//  Function created: 1/97, SEDWARD
// ==========================================================================

int CAlertListPropertyPage::RemoveSelectedListCtrlItems(void)
{
    auto    int         index;
    auto    int         numItems;
    auto    int         numItemsRemoved;
    auto    LV_ITEM     itemInfo;

    // initialize the list item information structure
    memset((void*)&itemInfo, 0, sizeof(LV_ITEM));
    itemInfo.mask = LVIF_STATE | LVIF_PARAM;
    itemInfo.stateMask = LVIS_SELECTED;

    // loop and process selected items
    numItemsRemoved = 0;
    numItems = m_listCtrlPtr->GetItemCount();
    for (index = 0; index < numItems; ++index)
        {
        // setup the structure index
        itemInfo.iItem = index;

        // if we can fetch the info and the current item is selected...
        if  ((TRUE == m_listCtrlPtr->GetItem(&itemInfo))
        &&  (itemInfo.state & LVIS_SELECTED))
            {
            // get a pointer to the record and have it removed from the list
            m_targetsListPtr->RemoveTarget((CAlertTarget*)itemInfo.lParam);

            // remove the item from the list control
            m_listCtrlPtr->DeleteItem(index);
            --index;
            ++numItemsRemoved;
            }
        }

    return (numItemsRemoved);

}  // end of "CAlertListPropertyPage::RemoveSelectedListCtrlItems"



// ===== SynchronizeEditButtonsAndContextMenu =============================
//
//  This function uses the enabled/disabled state of the button arguments
//  to enable or disable corresponding items in the menu argument.
//  NOTE: this function assumes that the menu items to be manipulated have
//        ID_ADD, ID_MODIFY and ID_REMOVE as menu IDs.
//
//  Input:
//      addButton       -- a reference to an "add" button
//      modifyButton    -- a reference to a "modify" button
//      removeButton    -- a reference to a "remove" button
//      menuPtr         -- a pointer to the target context menu
//
//  Output:
//      nothing
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

void    CAlertListPropertyPage::SynchronizeEditButtonsAndContextMenu(CMenu  *menuPtr)
{
    ASSERT(menuPtr != NULL);

    // add button
    if (TRUE == m_addButtonPtr->IsWindowEnabled())
        {
        menuPtr->EnableMenuItem(ID_ADD, MF_ENABLED);
        }
    else
        {
        menuPtr->EnableMenuItem(ID_ADD, MF_GRAYED);
        }

    // modify button
    if (TRUE == m_modifyButtonPtr->IsWindowEnabled())
        {
        menuPtr->EnableMenuItem(ID_MODIFY, MF_ENABLED);
        }
    else
        {
        menuPtr->EnableMenuItem(ID_MODIFY, MF_GRAYED);
        }

    // remove button
    if (TRUE == m_removeButtonPtr->IsWindowEnabled())
        {
        menuPtr->EnableMenuItem(ID_REMOVE, MF_ENABLED);
        }
    else
        {
        menuPtr->EnableMenuItem(ID_REMOVE, MF_GRAYED);
        }

}  // end of "CAlertListPropertyPage::SynchronizeEditButtonsAndContextMenu"



// ===== SynchronizeListCtrlAndEditButtons ================================
//
//  This function enables or disables the edit buttons based on the number
//  of items currently selected in the list control.
//
//  Input:
//      nothing
//
//  Output:
//      the total number of items currently selected in the list control
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

int CAlertListPropertyPage::SynchronizeListCtrlAndEditButtons(void)
{
	// TODO: Add your control notification handler code here

    auto    int         numSelectedItems;

    numSelectedItems = m_listCtrlPtr->GetSelectedCount();
    if (0 == numSelectedItems)
        {
        // if no items are selected, disable the modify and remove buttons
        m_addButtonPtr->EnableWindow(TRUE);
        m_modifyButtonPtr->EnableWindow(FALSE);
        m_removeButtonPtr->EnableWindow(FALSE);
        }
    else if (1 == numSelectedItems)
        {
        // only one item selected, all buttons are enabled
        m_addButtonPtr->EnableWindow(TRUE);
        m_modifyButtonPtr->EnableWindow(TRUE);
        m_removeButtonPtr->EnableWindow(TRUE);
        }
    else    // more than one item is selected
        {
        // enable the add and remove buttons, disable the modify button
        m_addButtonPtr->EnableWindow(TRUE);
        m_modifyButtonPtr->EnableWindow(FALSE);
        m_removeButtonPtr->EnableWindow(TRUE);
        }

    return (numSelectedItems);

}  // end of "CAlertListPropertyPage::SynchronizeListCtrlAndEditButtons"



// ===== ToggleListItemCheckbox ===========================================
//
//  This function toggles the display of a listbox item's checkbox.
//
//  NOTE: The checkbox is only toggled if it is an "active" checbox
//        (ie, a non-gray checkbox).
//
//  Input:
//      itemIndex   -- the list control item to toggle
//
//  Output: the new state of the checkbox (TRUE if it's checked, FALSE
//          if it's not)
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

BOOL    CAlertListPropertyPage::ToggleListItemCheckbox(int  itemIndex)
{
    auto    BOOL            isChecked;
    auto    LV_ITEM         listItemStruct;

    memset((void*)&listItemStruct, 0, sizeof(LV_ITEM));
    listItemStruct.iItem = itemIndex;
    listItemStruct.mask = LVIF_IMAGE;

    if (0 != m_listCtrlPtr->GetItem(&listItemStruct))
        {
        if (ATC_CHECKED_BITMAP == listItemStruct.iImage)
            {
            listItemStruct.iImage = ATC_UNCHECKED_BITMAP;
            isChecked = FALSE;
            }
        else if (ATC_UNCHECKED_BITMAP == listItemStruct.iImage)
            {
            listItemStruct.iImage = ATC_CHECKED_BITMAP;
            isChecked = TRUE;
            }
        }

    m_listCtrlPtr->SetItem(&listItemStruct);
    return (isChecked);

}   // end of "CAlertListPropertyPage::ToggleListItemCheckbox"















/////////////////////////////////////////////////////////////////////////////
// CAlertListPropertyPage message handlers
/////////////////////////////////////////////////////////////////////////////


// ===== OnAddButton ======================================================
//
//  This function allocates a new alert target object and passes it to the
//  edit handler.  If the edit handler indicates the object is to be saved,
//  its added to this object's temporary alert target list and also to the
//  list control.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

void    CAlertListPropertyPage::OnAddButton(void)
{
	// TODO: Add your control notification handler code here

    // allocate a new record
    auto    CAlertTarget*       newRecPtr;
    newRecPtr = GetNewAlertTarget();
    if (NULL != newRecPtr)
        {
        // handle the edit
        if (TRUE == HandleTargetEdit(newRecPtr, EDIT_ADD))  // virtual function
            {
            // add the new target to the list
            m_targetsListPtr->AddTarget(newRecPtr);

            // add the new target to the list control
            AddTargetToListCtrl(newRecPtr);
            }
        else
            {
            // release allocated memory
            delete  newRecPtr;
            }
        }

}   // end of "CAlertListPropertyPage::OnAddButton"



// ===== OnClickAlertsTargetsList =========================================
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

void    CAlertListPropertyPage::OnClickAlertsTargetsList(NMHDR* pNMHDR
                                                            , LRESULT* pResult)
{
    // TODO: Add your control notification handler code here

    auto    int         numSelectedItems;
    numSelectedItems = SynchronizeListCtrlAndEditButtons();
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
        iconHit = GetClickedItemIndex();
        if  ((-1 != m_clickedItemIndex)
        &&  (TRUE == iconHit))
            {
            // ... toggle the checkbox
            auto    BOOL        checkedState;
            checkedState = ToggleListItemCheckbox(m_clickedItemIndex);
            recPtr = (CAlertTarget*)m_listCtrlPtr->GetItemData(m_clickedItemIndex);
            if (NULL != recPtr)
                {
                // ... and update the alert target object's internal flag
                recPtr->SetActive(checkedState);
                }
            }
        }

    *pResult = 0;

}  // end of "CAlertListPropertyPage::OnClickAlertsTargetsList"



// ==== OnCommand =========================================================
//
//  This function updates this object's current context window pointer in case
//  a request is later made for context help.  If the "wParam" indicates the
//  "what's this" help for the list control has been requested, that is
//  handled as well.
//
//  Input:  the default MFC input arguments
//  Output: the default MFC return value
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

BOOL    CAlertListPropertyPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

    // if the user clicked on a control, store a pointer to its window in the
    // current context variable so it'll be ready for context help
    if (lParam > 0)
        {
        m_pCurrentContextWnd = GetDlgItem(LOWORD(wParam));
        }

    // if the message ID is equal to the "what's this" ID, handle it
    if (m_listCtrlWhatsThisMenuID == wParam)
        {
        HandleListCtrlWhatsThis();
        }

    return CAlertPropertyPage::OnCommand(wParam, lParam);

}  // end of "CAlertListPropertyPage::OnCommand"



// ===== OnContextMenu ====================================================
//
//  This function handles context-menu requests.  If the request has to do
//  with the list control, it is handled through this object; this is because
//  the list control context menus contain both commands (ie, add, modify,
//  remove) as well as help requests (ie, what's this, how to, infodesk) and
//  as such aren't handled automatically by the help property page class.
//
//  Input:  the default MFC input arguments
//  Output: the default MFC return value
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

void    CAlertListPropertyPage::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here

    // we'll handle the list control ourselves because its context menu
    // contains both commands and help items
    auto    int         winID = pWnd->GetDlgCtrlID();
    if (winID == m_listCtrlID)
        {
        HandleRclickListCtrl();
        return;
        }

    CAlertPropertyPage::OnContextMenu(pWnd, point);

}  // end of "CAlertListPropertyPage::OnContextMenu"



// ===== OnDblclkAlertsTargetsList ========================================
//
//  This function handles a double-click on the list control.
//
//  Input:  the default MFC input arguments
//  Output: the default MFC return value
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

void    CAlertListPropertyPage::OnDblclkAlertsTargetsList(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here

    // if an item was hit...
    GetClickedItemIndex();
    if (-1 != m_clickedItemIndex)
        {
        OnModifyButton();
        }

	*pResult = 0;

}  // end of "CAlertListPropertyPage::OnDblclkAlertsTargetsList"



// ===== OnHandleAdd ======================================================
//
//  This function handles an "add" request from the context menu.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

void    CAlertListPropertyPage::OnHandleAdd(void)
{
    // call the add function for the button
    OnAddButton();

}  // end of "CAlertListPropertyPage::OnHandleAdd"



// ===== OnHandleModify ===================================================
//
//  This function handles a "modify" request from the context menu.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

void    CAlertListPropertyPage::OnHandleModify(void)
{
    // call the modify function for the button
    OnModifyButton();

}  // end of "CAlertListPropertyPage::OnHandleModify"



// ===== OnHandleRemove ===================================================
//
//  This function handles a "remove" request from the context menu.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

void    CAlertListPropertyPage::OnHandleRemove(void)
{
    // call the remove function for the button
    OnRemoveButton();

}  // end of "CAlertListPropertyPage::OnHandleRemove"



// ==== OnInitDialog ======================================================
//
//  This function performs some basic intitialization for the property page.
//  It sets up the list control's headers, columns and has the control
//  populated.  It also makes a call to have this object's temporary alert
//  target list initialized.
//
//  Input:  the default MFC input arguments
//  Output: the default MFC return value
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

BOOL CAlertListPropertyPage::OnInitDialog(void)
{
	CAlertPropertyPage::OnInitDialog();

	// TODO: Add extra initialization here

    auto    CString         tempStr;
    auto    int             colWidth;
    auto    int             index;
    auto    LV_COLUMN       newCol;

    // setup the columns and column headers
    newCol.fmt = LVCFMT_LEFT;
    newCol.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_WIDTH;
    if (m_columnHeaders.GetSize() > 0)  // if there are any column headers...
        {
        newCol.mask |= LVCF_TEXT;
        }

    colWidth = LIST_CTRL_WIDTH_PIXELS / m_totalDisplayStrings;
    for (index = 0; index < m_totalDisplayStrings; ++index)
        {
        newCol.cx = colWidth;
        newCol.iSubItem = index;
        if (newCol.mask & LVCF_TEXT)    // if there are any column headers...
            {
            tempStr = m_columnHeaders[index];
            newCol.pszText = tempStr.GetBuffer(tempStr.GetLength());
            }
        m_listCtrlPtr->InsertColumn(index, &newCol);
        }

    // create the image list object
    m_imageList.Create(IDB_ALERT_TREE, 16, 1, RGB(255, 0, 255));
    m_listCtrlPtr->SetImageList(&m_imageList, LVSIL_SMALL);

    // create and initialize the temporary alert options list
    m_targetsListPtr = CreateTempAlertTargetsList();

    // initialize the contents of the list control
    InitListbox();

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CAlertListPropertyPage::OnInitDialog"



// ===== OnModifyButton ===================================================
//
//  This function fetches a pointer to the current alert target object (as
//  referenced by the clicked item index) and calls the handler to edit the
//  object.  If the edit changes are to be saved, the list control's display
//  is refreshed.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

void    CAlertListPropertyPage::OnModifyButton(void)
{
	// TODO: Add your control notification handler code here

    auto    CAlertTarget*       recPtr;
    if (-1 != m_clickedItemIndex)
        {
        // get a pointer to the item's data
        recPtr = (CAlertTarget*)m_listCtrlPtr->GetItemData(m_clickedItemIndex);
        if (TRUE == HandleTargetEdit(recPtr, EDIT_MODIFY))  // virtual function
            {
            AddTargetToListCtrl(recPtr, EDIT_MODIFY);
            }
        }

}   // end of "CAlertListPropertyPage::OnModifyButton"



// ===== OnOK =============================================================
//
//  This function updates the master alert target list with the contents of
//  this object's temporary alert target list.
//
//  Input:  the default MFC input arguments
//  Output: the default MFC return value
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

void    CAlertListPropertyPage::OnOK(void)
{
	// TODO: Add your specialized code here and/or call the base class

    UpdateMasterAlertTargetsList();
    CAlertPropertyPage::OnOK();

}  // end of "CAlertListPropertyPage::OnOK"



// ===== OnRemoveButton ===================================================
//
//  This function handles the removal of selected items in the list control.
//  It then makes a call to make the availability of the edit buttons in
//  synch with the current state of the list control.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

void CAlertListPropertyPage::OnRemoveButton(void)
{
	// TODO: Add your control notification handler code here

    // if there are no selected items, return
    if (m_listCtrlPtr->GetSelectedCount() <= 0)
        {
        return;
        }

    // remove selected items
    RemoveSelectedListCtrlItems();

    // if there are any items left in the list control, reset the current
    // item index
    if (m_listCtrlPtr->GetItemCount() > 0)
        {
        m_clickedItemIndex = 0;
        }

    // update the edit buttons
    SynchronizeListCtrlAndEditButtons();

}   // end of "CAlertListPropertyPage::OnRemoveButton"
