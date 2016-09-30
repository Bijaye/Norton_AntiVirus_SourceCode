/////////////////////////////////////////////////////////////////////
// SMTPPg.cpp : implementation file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/SMTPPG.CPv   1.2   17 Aug 1997 15:19:58   DBUCHES  $
//
// Description:
//
//  This file contains the property page for editing the SMTP alerts list.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/SMTPPG.CPv  $
// 
//    Rev 1.2   17 Aug 1997 15:19:58   DBUCHES
// Fixed problem with image list mask value.
//
//    Rev 1.1   25 Apr 1997 11:03:22   JBRENNA
// Ported from Tesla.
//
//    Rev 1.0   13 Mar 1997 15:06:20   RFULLER
// Initial revision
//
//    Rev 1.20   18 Feb 1997 01:41:56   SEDWARD
// If the 'settings' property page is initialized, notify the parent property
// sheet (OnInitDialog).
//
//    Rev 1.19   02 Feb 1997 16:36:32   SEDWARD
// Added support for context menus that are grayed out AND in groupbox controls.
//
//    Rev 1.18   30 Jan 1997 00:49:44   SEDWARD
// Hooked up help buttons for both property pages.
//
//    Rev 1.16   27 Jan 1997 15:48:20   SEDWARD
// Moved column headers in list control to the string table.
//
//    Rev 1.15   21 Jan 1997 19:17:40   SEDWARD
// Initialize m_clickedItemIndex in constructor, set to invalid value if
// no items are selected and the list control is clicked.
//
//    Rev 1.14   19 Jan 1997 23:43:28   SEDWARD
// Updated help maps, implemented multiple selection/removal of list control
// items.
//
//    Rev 1.13   16 Jan 1997 02:13:04   SEDWARD
// Lots of updates in help maps and menus.
//
//    Rev 1.12   14 Jan 1997 20:37:00   SEDWARD
// New help ID values.
//
//    Rev 1.11   14 Jan 1997 13:24:44   SEDWARD
// Updated help maps with new IDs, added help button.
//
//    Rev 1.9   07 Jan 1997 18:18:58   TSmith
// Added class 'CSmtpOptPpg' for the SMTP mail settings property page.
//
//    Rev 1.8   06 Jan 1997 00:39:36   SEDWARD
// Now uses temporary CAlertOptions list for edits.
//
//    Rev 1.6   09 Dec 1996 20:47:12   SEDWARD
//
//    Rev 1.5   03 Dec 1996 19:49:10   SEDWARD
// Substantial changes (not frozen yet...).
//
//    Rev 1.4   02 Dec 1996 16:32:58   SEDWARD
// More big changes, nearing UI freeze.
//
//    Rev 1.3   26 Nov 1996 00:56:36   SEDWARD
// More big changes.
//
//    Rev 1.2   25 Nov 1996 15:46:36   SEDWARD
// Major overhaul (more to come).
//
//    Rev 1.1   21 Nov 1996 17:07:32   SEDWARD
// Added vlog headers.
//
//    Rev 1.0   21 Nov 1996 16:55:06   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "AlertUI.h"
#include "AlertOpt.h"
#include "SMTPPg.h"
#include "EditSMTP.h"
#include "EmailPSh.h"
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
DWORD   CEmailSMTPPg::m_dwWhatsThisMap[] =
{
    IDC_EMAIL_ADD_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_ADD_ITEM_BTN
    , IDC_EMAIL_MODIFY_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_MODIFY_ITEM_BTN
    , IDC_EMAIL_REMOVE_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_REMOVE_ITEM_BTN
    , IDC_EMAIL_LISTVIEW, IDH_NAVNT_WHATHIS_MESSERV_TARGETS_LSTBX
    , 0,0
};


DWORD   CEmailSMTPPg::m_dwHowToMap[] =
{
    ID_HELP, IDH_NAVNT_EMAIL_SETTINGS_DLG_HELP_BTN
    , ID_HT_DEFINE_SERVER_AND_MESSAGE_SMTP, IDH_NAVNT_TASK_DEFINE_THE_SMTP_SERVER_AND_MESSAGE_FIELDS
    , ID_HT_ADD_SMTP, IDH_NAVNT_TASK_ADD_AN_EMAIL_ALERT_TARGET
    , ID_HT_MODIFY_SMTP, IDH_NAVNT_TASK_EDIT_A_SINGLE_EMAIL_TARGET
    , ID_HT_EDITLIST_SMTP, IDH_NAVNT_TASK_EDIT_THE_EMAIL_TARGET_LIST
    , ID_HT_REMOVE_SMTP, IDH_NAVNT_TASK_REMOVE_AN_EMAIL_TARGET
    , 0,0
};



/////////////////////////////////////////////////////////////////////////////
// CEmailSMTPPg property page
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CEmailSMTPPg, CHelpPropertyPage)

CEmailSMTPPg::CEmailSMTPPg() : CHelpPropertyPage(CEmailSMTPPg::IDD)
{
    //{{AFX_DATA_INIT(CEmailSMTPPg)
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
    SetHowToMenuIndex(HOW_TO_INDEX_SMTP_TARGETS);
}


CEmailSMTPPg::~CEmailSMTPPg()
{
    if (NULL != m_targetsListPtr)
        {
        delete  m_targetsListPtr;
        }
}

void CEmailSMTPPg::DoDataExchange(CDataExchange* pDX)
{
    CHelpPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CEmailSMTPPg)
	DDX_Control(pDX, IDC_EMAIL_MODIFY_BUTTON, m_emailModifyButton);
	DDX_Control(pDX, IDC_EMAIL_REMOVE_BUTTON, m_emailRemoveButton);
	DDX_Control(pDX, IDC_EMAIL_ADD_BUTTON, m_emailAddButton);
	DDX_Control(pDX, IDC_EMAIL_LISTVIEW, m_emailList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEmailSMTPPg, CHelpPropertyPage)
    //{{AFX_MSG_MAP(CEmailSMTPPg)
    ON_NOTIFY(NM_CLICK, IDC_EMAIL_LISTVIEW, OnClickEmailList)
	ON_BN_CLICKED(IDC_EMAIL_ADD_BUTTON, OnEmailAddButton)
	ON_BN_CLICKED(IDC_EMAIL_REMOVE_BUTTON, OnEmailRemoveButton)
	ON_BN_CLICKED(IDC_EMAIL_MODIFY_BUTTON, OnEmailModifyButton)
    ON_COMMAND(ID_ADD, OnHandleSMTPAdd)
    ON_COMMAND(ID_MODIFY, OnHandleSMTPModify)
    ON_COMMAND(ID_REMOVE, OnHandleSMTPRemove)
	ON_NOTIFY(NM_DBLCLK, IDC_EMAIL_LISTVIEW, OnDblclkEmailListview)
	ON_WM_CONTEXTMENU()
    ON_COMMAND(IDR_WHATS_THIS_SMTP_LISTCTRL, OnWhatsThisSMTPListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()





/////////////////////////////////////////////////////////////////////////////
// CEmailSMTPPg member functions
/////////////////////////////////////////////////////////////////////////////



// ===== AddListboxItem ===================================================
//
//  This function adds another item to the email listbox.  It uses the
//  information in the input argument to set the fields/display.
//
//  Input:
//      emailRecPtr -- a pointer to a CEMailTarget record
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CEmailSMTPPg::AddListboxItem(CEMailTarget*  emailRecPtr)
{
    // add the strings to the list control
    auto    int             numItems;
    numItems = m_emailList.GetItemCount();

    auto    TCHAR*          charPtr;
    auto    CString         tempStr;
    auto    LV_ITEM         newItem;

    // column one
    memset((void*)&newItem, 0, sizeof(LV_ITEM));
    newItem.mask = LVIF_TEXT | LVIF_IMAGE;
    newItem.iItem = numItems;
    newItem.iSubItem = 0;
    tempStr = emailRecPtr->GetName();
    charPtr = tempStr.GetBuffer(100);   // cheeze...
    newItem.pszText = charPtr;
    newItem.iImage = ATC_CHECKED_BITMAP;
    m_emailList.InsertItem(&newItem);

    // column two
    newItem.mask = LVIF_TEXT;
    newItem.iItem = numItems;
    newItem.iSubItem = 1;
    tempStr = emailRecPtr->GetAddress();
    charPtr = tempStr.GetBuffer(100);   // cheeze...
    newItem.pszText = charPtr;
    m_emailList.SetItem(&newItem);

    // finally, save a pointer to this item's corresponding email
    // record
    m_emailList.SetItemData(numItems, (DWORD)emailRecPtr);

}  // end of "CEmailSMTPPg::AddListboxItem"



// ===== CreateTempAlertTargetsList =======================================
//
//  This function creates and initializes a temporary alert targets list so
//  the user may edit its contents to his/her heart's content without
//  affecting the master list.  Only the email records from the master list
//  are copied over to the temporary list.
//
//  Input:  nothing
//
//  Output: a value of TRUE if no errors occur, FALSE otherwise
//
// ========================================================================
//  Function created: 12/96, SEDWARD
// ========================================================================

BOOL    CEmailSMTPPg::CreateTempAlertTargetsList(void)
{
    auto    BOOL        allWentWell = TRUE;

    // create a temporary alert targets list object
    try
        {
        m_targetsListPtr = new CAlertOptions;
        }

    catch(CMemoryException)
        {
        allWentWell = FALSE;
        throw;      // forward the exception object to the next handler
        }

    // loop through the master alert targets list and copy all the email
    // records to the temporary list
    gAlertOptions.ResetPosition();
    auto    CEMailTarget*   emailRecPtr;
    auto    CEMailTarget*   tempRecPtr;
    while (emailRecPtr = (CEMailTarget*)gAlertOptions.GetNextTarget(TARGET_EMAIL))
        {
        // allocate a new record for the temporary list
        try
            {
            tempRecPtr = new CEMailTarget;
            }

        catch(CMemoryException)
            {
            }

        // copy the master record's data over to the temporary list record
        tempRecPtr->SetName(emailRecPtr->GetName());
        tempRecPtr->SetAddress(emailRecPtr->GetAddress());
        tempRecPtr->SetActive(emailRecPtr->GetActive());

        // add the record to the temporary list
        m_targetsListPtr->AddTarget(tempRecPtr);
        }

    return (allWentWell);

}  // end of "CEmailSMTPPg::CreateTempAlertTargetsList"



// ===== GetListItemDataPtr ===============================================
//
//  This function uses the input index value to fetch a pointer to a
//  listbox item's data (the "lparam").
//
//  Input:  nIndex  -- the target listbox item index
//
//  Output: if the pointer was successfully retrieved, then a pointer to
//          a CEmailSMTPPg structure is returned; else, a NULL pointer is
//          returned
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

CEMailTarget*   CEmailSMTPPg::GetListItemDataPtr(int  nIndex)
{
    auto    LV_ITEM         listItemStruct;
    auto    CEMailTarget*   emailPtr = NULL;

    // initialize the list view item structure
    memset((void*)&listItemStruct, 0, sizeof(LV_ITEM));
    listItemStruct.iItem = nIndex;
    listItemStruct.mask = LVIF_PARAM;

    // fetch the email info from the current listview item
    if (0 != m_emailList.GetItem(&listItemStruct))
        {
        emailPtr = (CEMailTarget*)listItemStruct.lParam;
        }
    else
        {
        emailPtr = NULL;
        }

    return (emailPtr);

}  // end of "CEmailSMTPPg::GetListItemDataPtr"



// ===== HandleRclickEmailList ============================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CEmailSMTPPg::HandleRclickEmailList(void)
{
    // TODO: Add your control notification handler code here

    auto    DWORD       dwPos;
    auto    POINT       pointScreen;

    // get the screen coordinates
    dwPos = GetMessagePos();
	pointScreen.x = LOWORD (dwPos);
	pointScreen.y = HIWORD (dwPos);

    // update the item index member variable
    GetClickedItemIndex(m_emailList, m_clickedItemIndex);

    // update the edit buttons
    SynchronizeListCtrlAndEditButtons(m_emailList, m_emailAddButton
                                                , m_emailModifyButton
                                                , m_emailRemoveButton);

    // create the popup menu
    auto    CMenu       tempMenu;
    VERIFY(tempMenu.LoadMenu(IDR_RMENU_SMTPPAGE_TARGET));

    auto    CMenu       *popupMenu = tempMenu.GetSubMenu(0);
    ASSERT(popupMenu != NULL);

    // synchronize the menu items with their corresponding edit button states
    SynchronizeEditButtonsAndContextMenu(m_emailAddButton
                                                    , m_emailModifyButton
                                                    , m_emailRemoveButton
                                                    , popupMenu);

    popupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON
                                                    , pointScreen.x
                                                    , pointScreen.y
                                                    , AfxGetMainWnd());

}  // end of "CEmailSMTPPg::HandleRclickEmailList"



// ===== InitListbox ======================================================
//
//  This function populates the listbox control with the current contents
//  of the master email list.
//
//  Input:  nothing
//  Output: the number of items added to the listbox
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

int     CEmailSMTPPg::InitListbox(void)
{
    // iterate through the master email list and add items
    auto    CEMailTarget*   emailRecPtr;
    auto    CString         tempStr;
    auto    int             numItems;
    auto    LV_ITEM         newItem;
    auto    TCHAR*          charPtr;

    // iterate through the master list and populate the listbox
    numItems = 0;
    m_targetsListPtr->ResetPosition();
    while (emailRecPtr = (CEMailTarget*)m_targetsListPtr->GetNextTarget(TARGET_EMAIL))
        {
        // insert the listbox column items...

        // column one (the user name)
        memset((void*)&newItem, 0, sizeof(LV_ITEM));
        newItem.mask = LVIF_TEXT | LVIF_IMAGE;
        newItem.iItem = numItems;
        newItem.iSubItem = 0;
        tempStr = emailRecPtr->GetName();
        charPtr = tempStr.GetBuffer(LIST_ITEM_MAX_STRLEN);
        newItem.pszText = charPtr;
        if (emailRecPtr->GetActive())
            {
            newItem.iImage = ATC_CHECKED_BITMAP;
            }
        else
            {
            newItem.iImage = ATC_UNCHECKED_BITMAP;
            }
        m_emailList.InsertItem(&newItem);

        // column two (the mail address)
        newItem.mask = LVIF_TEXT;
        newItem.iItem = numItems;
        newItem.iSubItem = 1;
        tempStr = emailRecPtr->GetAddress();
        charPtr = tempStr.GetBuffer(LIST_ITEM_MAX_STRLEN);
        newItem.pszText = charPtr;
        m_emailList.SetItem(&newItem);

        // finally, save a pointer to this item's corresponding email
        // record with the listbox item
        m_emailList.SetItemData(numItems, (DWORD)emailRecPtr);
        ++numItems;
        }

    return (numItems);

}  // end of "CEmailSMTPPg::InitListbox"



// ===== UpdateListboxItem ================================================
//
//  This function updates the display of a target listbox item with the
//  contents of the input email record argument.
//
//  Input:
//      itemIndex   -- the index of the target listbox item
//      recPtr      -- a pointer to a valid email record
//
//  Output: ???
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL CEmailSMTPPg::UpdateListboxItem(int  itemIndex, CEMailTarget*  recPtr)
{
    auto    BOOL            bRetVal = TRUE;
    auto    CString         tempStr;
    auto    int             result;
    auto    LV_ITEM         itemInfo;
    auto    TCHAR*          charPtr;

    // update the "name" portion
    memset((void*)&itemInfo, 0, sizeof(LV_ITEM));
    itemInfo.mask = LVIF_TEXT;
    itemInfo.iItem = itemIndex;
    itemInfo.iSubItem = 0;
    tempStr = recPtr->GetName();
    charPtr = tempStr.GetBuffer(100);
    itemInfo.pszText = charPtr;
    result = m_emailList.SetItem(&itemInfo);
    if (-1 == result)
        {
        bRetVal = FALSE;
        }

    // update the "address" portion
    itemInfo.mask = LVIF_TEXT;
    itemInfo.iItem = itemIndex;
    itemInfo.iSubItem = 1;
    tempStr = recPtr->GetAddress();
    charPtr = tempStr.GetBuffer(100);
    itemInfo.pszText = charPtr;
    result = m_emailList.SetItem(&itemInfo);
    if (-1 == result)
        {
        bRetVal = FALSE;
        }

    return (bRetVal);

}  // end of "CEmailSMTPPg::UpdateListboxItem"



// ===== UpdateMasterAlertTargetsList =====================================
//
//  This function replaces all current email records in the master alert
//  targets list with those in the temporary targets list.
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 12/96, SEDWARD
// ========================================================================

void    CEmailSMTPPg::UpdateMasterAlertTargetsList(void)
{
    auto    CEMailTarget*   emailRecPtr;
    auto    CEMailTarget*   newRecPtr;

    // remove all email records from the master alert targets list
    gAlertOptions.ResetPosition();
    while (emailRecPtr = (CEMailTarget*)gAlertOptions.GetNextTarget(TARGET_EMAIL))
        {
        gAlertOptions.RemoveTarget(emailRecPtr);
        }

    // if there are any records in the temporary targets list, add them to
    // the master targets list
    if (m_targetsListPtr->GetTargetCount() > 0)
        {
        m_targetsListPtr->ResetPosition();
        while (emailRecPtr = (CEMailTarget*)m_targetsListPtr
                                                ->GetNextTarget(TARGET_EMAIL))
            {
            // allocate a new record for the master list
            try
                {
                newRecPtr = new CEMailTarget;
                }

            catch(CMemoryException)
                {
                }

            // copy the temporary record's data over to the master list record
            // and add it to the master list
            newRecPtr->SetName(emailRecPtr->GetName());
            newRecPtr->SetAddress(emailRecPtr->GetAddress());
            newRecPtr->SetActive(emailRecPtr->GetActive());
            gAlertOptions.AddTarget(newRecPtr);
            }
        }

}  // end of "CEmailSMTPPg::UpdateMasterAlertTargetsList"









/////////////////////////////////////////////////////////////////////////////
// CEmailSMTPPg message handlers
/////////////////////////////////////////////////////////////////////////////



// ===== OnClickEmailList =================================================
//
//  This function handles the event when a user clicks on the email listbox.
//
//  Input:  the default MFC input arguments
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CEmailSMTPPg::OnClickEmailList(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: Add your control notification handler code here

    auto    int         numSelectedItems;
    numSelectedItems = SynchronizeListCtrlAndEditButtons(m_emailList
                                                , m_emailAddButton
                                                , m_emailModifyButton
                                                , m_emailRemoveButton);

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
        iconHit = GetClickedItemIndex(m_emailList, m_clickedItemIndex);
        if  ((-1 != m_clickedItemIndex)
        &&  (TRUE == iconHit))
            {
            // ... toggle the checkbox
            auto    BOOL        checkedState;
            checkedState = ToggleListItemCheckbox(m_emailList
                                                    , m_clickedItemIndex);
            recPtr = (CAlertTarget*)m_emailList.GetItemData(
                                                    m_clickedItemIndex);
            if (NULL != recPtr)
                {
                recPtr->SetActive(checkedState);
                }
            }
        }

    *pResult = 0;

}   // end of "CEmailSMTPPg::OnClickEmailList"



// ==== OnCommand =========================================================
//
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

BOOL CEmailSMTPPg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

    // if the user clicked on a control, store a pointer to its window in the
    // current context variable so it'll be ready for context help
    if (lParam > 0)
        {
        m_pCurrentContextWnd = GetDlgItem(LOWORD(wParam));
        }

	return CHelpPropertyPage::OnCommand(wParam, lParam);

}  // end of "CEmailSMTPPg::OnCommand"



// ==== OnContextMenu =====================================================
//
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

void    CEmailSMTPPg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here

    // we'll handle the list control ourselves
    auto    int         winID;
    winID = pWnd->GetDlgCtrlID();
    if (IDC_EMAIL_LISTVIEW == winID)
        {
        HandleRclickEmailList();
        return;
        }

    CHelpPropertyPage::OnContextMenu(pWnd, point);

}  // end of "CEmailSMTPPg::OnContextMenu"



// ===== OnDblclkEmailListview ============================================
//
//  Input:  the default MFC input arguments
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CEmailSMTPPg::OnDblclkEmailListview(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here

    if (-1 != m_clickedItemIndex)
        {
        OnEmailModifyButton();
        }

	*pResult = 0;

}  // end of "CEmailSMTPPg::OnDblclkEmailListview"



// ===== OnEmailAddButton =================================================
//
//  This function is called when the user presses the "Add" button to add
//  a new email record.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CEmailSMTPPg::OnEmailAddButton(void)
{
	// TODO: Add your control notification handler code here

    // allocate a new email info record
    auto    CEMailTarget*       emailRecPtr;
    try
        {
        emailRecPtr = new CEMailTarget;
        }

    catch(CMemoryException)
        {
        }

    // if the user enters a record and clicks "OK"...
    auto    BOOL        result;
    result = HandleSMTPEdit(emailRecPtr);
    if (TRUE == result)
        {
        // ... add the record to the master list object
        m_targetsListPtr->AddTarget(emailRecPtr);

        // ... and then to the listbox
        AddListboxItem(emailRecPtr);
        }
    else
        {
        // release the memory we allocated
        delete  emailRecPtr;
        }

}  // end of "CEmailSMTPPg::OnEmailAddButton"



// ===== OnEmailModifyButton ==============================================
//
//  This function is called when the user presses the "Modify" button to
//  modify an existing email record.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CEmailSMTPPg::OnEmailModifyButton(void)
{
	// TODO: Add your control notification handler code here

    auto    CEMailTarget*       emailRecPtr;

    // get a pointer to the target record and store temporarily in the
    // member variable
    if (m_clickedItemIndex >= 0)
        {
        emailRecPtr = GetListItemDataPtr(m_clickedItemIndex);

        // if the user wants to save changes, update the list display
        if  ((NULL != emailRecPtr)
        &&  (TRUE == HandleSMTPEdit(emailRecPtr, EDIT_MODIFY)))
            {
            UpdateListboxItem(m_clickedItemIndex, emailRecPtr);

            // disable the Modify and remove buttons
            m_emailRemoveButton.EnableWindow(FALSE);
            m_emailModifyButton.EnableWindow(FALSE);
            }
        }

}  // end of "CEmailSMTPPg::OnEmailModifyButton"



// ===== OnEmailRemoveButton ==============================================
//
//  This function is called when the user presses the "Remove" button to
//  remove an existing email record.
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CEmailSMTPPg::OnEmailRemoveButton(void)
{
    // TODO: Add your control notification handler code here

    // if there are no selected items, return
    if (m_emailList.GetSelectedCount() <= 0)
        {
        return;
        }

    // remove selected items
    RemoveSelectedListCtrlItems(m_targetsListPtr, m_emailList);

    // if there are any items left in the list control, reset the current
    // item index
    if (m_emailList.GetItemCount() > 0)
        {
        m_clickedItemIndex = 0;
        }

    // update the edit buttons
    SynchronizeListCtrlAndEditButtons(m_emailList, m_emailAddButton
                                                , m_emailModifyButton
                                                , m_emailRemoveButton);

}  // end of "CEmailSMTPPg::OnEmailRemoveButton"



// ===== OnHandleSMTPAdd ==================================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CEmailSMTPPg::OnHandleSMTPAdd(void)
{
	// TODO: Add your control notification handler code here

    // call the same function used for the add button
    OnEmailAddButton();

}  // end of "CEmailSMTPPg::OnHandleSMTPAdd"



// ===== OnHandleSMTPModify ===============================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CEmailSMTPPg::OnHandleSMTPModify(void)
{
	// TODO: Add your control notification handler code here

    // call the same function used for the modify button
    OnEmailModifyButton();


}  // end of "CEmailSMTPPg::OnHandleSMTPModify"



// ===== OnHandleSMTPRemove ===============================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CEmailSMTPPg::OnHandleSMTPRemove(void)
{
	// TODO: Add your control notification handler code here

    // call the same function used for the remove button
    OnEmailRemoveButton();


}  // end of "CEmailSMTPPg::OnHandleSMTPRemove"



// ===== OnInitDialog =====================================================
//
//  This function creates the listbox columns and initializes the display
//  of the listbox.
//
//  Input:  nothing
//  Output: a value of TRUE
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL CEmailSMTPPg::OnInitDialog(void)
{
    CHelpPropertyPage::OnInitDialog();

	// TODO: Add extra initialization here

    auto    CString     tempStr;
    static  CString     addressStr;
    static  CString     nameStr;

    // create the columns...

    // the "name" column
    auto    LV_COLUMN       newCol;
    newCol.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
    newCol.fmt = LVCFMT_LEFT;
    newCol.cx = 150;    // num pixels
    if (nameStr.LoadString(IDS_SMTP_NAME_HEADER))
        {
        newCol.pszText = nameStr.GetBuffer(nameStr.GetLength());
        }

    newCol.iSubItem = 0;
    m_emailList.InsertColumn(0, &newCol);

    // the "address" column
    newCol.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
    newCol.fmt = LVCFMT_LEFT;
    newCol.cx = 150;    // num pixels
    if (addressStr.LoadString(IDS_SMTP_ADDRESS_HEADER))
        {
        newCol.pszText = addressStr.GetBuffer(addressStr.GetLength());
        }

    newCol.iSubItem = 1;
    m_emailList.InsertColumn(1, &newCol);

    // create the image list object
    m_imageList.Create(IDB_ALERT_TREE, 16, 1, RGB(255, 0, 255));
    m_emailList.SetImageList(&m_imageList, LVSIL_SMALL);

    // create and initialize the temporary alert options list
    CreateTempAlertTargetsList();

    // populate the listbox with records
    InitListbox();

    // set the page's caption
    if (tempStr.LoadString(IDS_TARGET_PAGE_CAPTION))
        {
        SetWindowText(tempStr);
        }

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CEmailSMTPPg::OnInitDialog"



// ===== OnOK =============================================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CEmailSMTPPg::OnOK(void)
{
	// TODO: Add your specialized code here and/or call the base class

    // replace the master email records with those in the temporary list
    UpdateMasterAlertTargetsList();

    CHelpPropertyPage::OnOK();

}  // end of "CEmailSMTPPg::OnOK"



// ==== OnWhatsThisSMTPListCtrl ===========================================
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

void    CEmailSMTPPg::OnWhatsThisSMTPListCtrl(void)
{
    ::WinHelp (this->GetSafeHwnd(),  NAV_HELPFILENAME, HELP_CONTEXTPOPUP
                                , IDH_NAVNT_WHATHIS_MESSERV_TARGETS_LSTBX);

}  // end of "CEmailSMTPPg::OnWhatsThisSMTPListCtrl"








/////////////////////////////////////////////////////////////////////////////
// CSmtpOptPpg property page



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

    // activate the "help" button flag
    m_psp.dwFlags |= PSP_HASHELP;

    // set up the help map pointers
    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_pHelpHowToMap = m_dwHowToMap;

    // initialize the "how to" index for this dialog
    SetHowToMenuIndex(HOW_TO_INDEX_SMTP_TARGETS);
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
