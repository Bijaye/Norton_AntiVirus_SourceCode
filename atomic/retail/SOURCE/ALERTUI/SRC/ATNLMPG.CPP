/////////////////////////////////////////////////////////////////////
// ATNlmPg.cpp : implementation file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ATNLMPG.CPv   1.1   25 Apr 1997 11:04:32   JBRENNA  $
//
// Description:
//
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ATNLMPG.CPv  $
// 
//    Rev 1.1   25 Apr 1997 11:04:32   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   15 Apr 1997 20:08:24   sedward
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AlertUI.h"
#include "AlertOpt.h"
#include "ATNlmPg.h"
#include "help_ids.h"               // help IDs


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// the "what's this" and "how to" maps
DWORD   dwNlmWhatsThisMap[] =
{
    IDC_ADD_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_ADD_ITEM_BTN
    , IDC_MODIFY_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_MODIFY_ITEM_BTN
    , IDC_REMOVE_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_REMOVE_ITEM_BTN
    , IDC_INCLUDE_ALL_NLMS_CHECKBOX, IDH_NAVNT_ALERT_ALL_NAV_FOR_NETWARE_SERVERS
    , IDC_NLM_TARGET_LIST, IDH_NAVNT_WHATHIS_MESSERV_TARGETS_LSTBX
    , 0,0
};


DWORD   dwNlmHowToMap[] =
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
// CAlertNlmPropertyPage property page
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CAlertNlmPropertyPage, CAlertMachinePropertyPage)

CAlertNlmPropertyPage::CAlertNlmPropertyPage() : CAlertMachinePropertyPage()
{
    InitializeData();
}


CAlertNlmPropertyPage::CAlertNlmPropertyPage(UINT  dlgID) : CAlertMachinePropertyPage(dlgID)
{
    InitializeData();
}


CAlertNlmPropertyPage::~CAlertNlmPropertyPage()
{
}

void CAlertNlmPropertyPage::DoDataExchange(CDataExchange* pDX)
{
    CAlertMachinePropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAlertNlmPropertyPage)
	DDX_Control(pDX, IDC_INCLUDE_ALL_NLMS_CHECKBOX, m_allNlmCheckbox);
	DDX_Control(pDX, IDC_REMOVE_BUTTON, m_removeButton);
	DDX_Control(pDX, IDC_MODIFY_BUTTON, m_modifyButton);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_addButton);
	DDX_Control(pDX, IDC_NLM_TARGET_LIST, m_listCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlertNlmPropertyPage, CAlertMachinePropertyPage)
    //{{AFX_MSG_MAP(CAlertNlmPropertyPage)
	ON_BN_CLICKED(IDC_INCLUDE_ALL_NLMS_CHECKBOX, OnIncludeAllNlmsCheckbox)
	ON_NOTIFY(NM_CLICK, IDC_NLM_TARGET_LIST, OnClickNlmTargetList)
	ON_NOTIFY(NM_DBLCLK, IDC_NLM_TARGET_LIST, OnDblclkNlmTargetList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()





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

BOOL    CAlertNlmPropertyPage::CopyAlertTargetInfo(CAlertTarget*  source
                                                        , CAlertTarget*  dest)
{
    auto    CNLMTarget*         tempDest = (CNLMTarget*)dest;
    auto    CNLMTarget*         tempSource = (CNLMTarget*)source;

    // copy the source object data over to the destination object
    tempDest->SetAddress(tempSource->GetAddress());
    tempDest->SetActive(tempSource->GetActive());
    tempDest->SetPriority(tempSource->GetPriority());
    return  (TRUE);

}  // end of "CAlertNlmPropertyPage::CopyAlertTargetInfo"



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

BOOL    CAlertNlmPropertyPage::GetDisplayString(CAlertTarget*  pAlertTarget
                                                        , CString&  displayStr
                                                        , int  index)
{
    auto    BOOL                result = TRUE;
    auto    CNLMTarget*         nlmPtr = (CNLMTarget*)pAlertTarget;

    if (index > (m_totalDisplayStrings - 1))
        {
        result = FALSE;
        }
    else
        {
        switch (index)
            {
            case   0:
                displayStr = nlmPtr->GetAddress();
                break;

            default:
                result = FALSE;
                break;
            }
        }

    return  (result);

}  // end of "CAlertNlmPropertyPage::GetDisplayString"



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

CAlertTarget*   CAlertNlmPropertyPage::GetNewAlertTarget(void)
{
    auto    CNLMTarget*         newAlertTarget;

    try {
        newAlertTarget = new  CNLMTarget;
        }

    catch(CMemoryException)
        {
        newAlertTarget = NULL;
        throw;      // forward the exception object to the next handler
        }

    return  (newAlertTarget);

}  // end of "CAlertNlmPropertyPage::GetNewAlertTarget"



// ===== HandleRclickListCtrl =============================================
//
//  This function populates the listbox control with the current contents
//  of the master record list.
//
//  NOTE: this is an implementation of the virtual function inherited from
//        the CAlertPropertyPage class
//
//  Input:  nothing
//  Output: the number of items added to the listbox
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

void    CAlertNlmPropertyPage::HandleRclickListCtrl(void)
{
	// TODO: Add your message handler code here

    // update the edit buttons; if the master NLM checkbox is set, disable
    // all edit button, otherwise synchronize their states with the current
    // selection(s) in the list control
    if (1 == m_allNlmCheckbox.GetCheck())
        {
        auto    DWORD       dwPos;
        auto    POINT       pointScreen;

        // get the screen coordinates
        dwPos = GetMessagePos();
        pointScreen.x = LOWORD (dwPos);
        pointScreen.y = HIWORD (dwPos);

        // update the item index member variable
        GetClickedItemIndex();

        m_addButton.EnableWindow(FALSE);
        m_modifyButton.EnableWindow(FALSE);
        m_removeButton.EnableWindow(FALSE);

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
        }
    else
        {
        CAlertListPropertyPage::HandleRclickListCtrl();
        }

}  // end of "CAlertNlmPropertyPage::HandleRclickListCtrl"



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

BOOL    CAlertNlmPropertyPage::InitializeData(void)
{
    auto    BOOL        result = TRUE;

    // initialize the alert type
    m_alertType = TARGET_NLM;

    // setup the context help map pointers
    m_pHelpWhatsThisMap = dwNlmWhatsThisMap;
    m_pHelpHowToMap = dwNlmHowToMap;

    // initialize the "how to" index for this dialog
    SetHowToMenuIndex(HOW_TO_INDEX_NLM);

    // initialize some control IDs
    m_listCtrlContextMenuID = IDR_RMENU_NLMPAGE_TARGET;
    m_listCtrlWhatsThisMenuID = IDR_WHATS_THIS_NLM_LISTCTRL;
    m_listCtrlWhatsThisHelpID = IDH_NAVNT_WHATHIS_MESSERV_TARGETS_LSTBX;
    m_listCtrlID = IDC_NLM_TARGET_LIST;

    // setup pointers to the controls
    m_listCtrlPtr = &m_listCtrl;
    m_addButtonPtr = &m_addButton;
    m_modifyButtonPtr = &m_modifyButton;
    m_removeButtonPtr = &m_removeButton;

    auto    CString     tempStr;
    if (tempStr.LoadString(IDS_COLUMN_HEADER_MACHINE))
        {
        m_totalDisplayStrings = 1;
        m_columnHeaders.SetSize(m_totalDisplayStrings);
        m_columnHeaders[0] = tempStr;
        }
    else
        {
        m_totalDisplayStrings = 0;
        result = FALSE;
        }

    return (result);

}  // end of "CAlertNlmPropertyPage::InitializeData"



// ===== InitListbox ======================================================
//
//  This function populates the listbox control with the current contents
//  of the master record list.
//
//  Input:  nothing
//  Output: the number of items added to the listbox
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

int CAlertNlmPropertyPage::InitListbox(void)
{
    // first let the parent class populate the listbox
    auto    int     result;
    result = CAlertListPropertyPage::InitListbox();

    // if the "master" NLM item is in the listbox, make adjustments
    MasterNLMItemInit();

    return (result);

}  // end of "CAlertNlmPropertyPage::InitListbox"



// ===== MasterNLMItemInit ================================================
//
//  Input:  nothing
//  Output: BOOL
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertNlmPropertyPage::MasterNLMItemInit(void)
{
    auto    BOOL        bResult = TRUE;
    auto    BOOL        foundMaster = FALSE;
    auto    CString     tempStr;
    auto    CString     searchStr;

    // see if the master NLM item is in the list control
    // searchStr = DOUBLE_BACKSLASH;
    searchStr += MASTER_NLM_ITEM_STR;
    tempStr = m_listCtrl.GetItemText(MASTER_NLM_ITEM_INDEX, 0);
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
        if (0 == m_listCtrl.SetItem(&masterItem))
            {
            bResult = FALSE;
            }
        else
            {
            // set the "All NLMs" checkbox
            m_allNlmCheckbox.SetCheck(1);

            // disable the edit buttons
            m_addButton.EnableWindow(FALSE);
            m_removeButton.EnableWindow(FALSE);
            m_modifyButton.EnableWindow(FALSE);

            // finally, update the other items ("TRUE" means the all NLMs
            // checkbox is checked)
            UpdateNLMItemIcons(TRUE);
            }
        }

    return (bResult);

}  // end of "CAlertNlmPropertyPage::MasterNLMItemInit"



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

BOOL    CAlertNlmPropertyPage::TargetIsActive(CAlertTarget*  pAlertTarget)
{
    auto    CNLMTarget*         nlmAlertTarget;
    nlmAlertTarget = (CNLMTarget*)pAlertTarget;
    return  (nlmAlertTarget->GetActive());

}  // end of "CAlertNlmPropertyPage::TargetIsActive"



// ===== UpdateMasterNLMItem ==============================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    CAlertNlmPropertyPage::UpdateMasterNLMItem(BOOL  checkedState)
{
    auto    BOOL            result = TRUE;

    if (0 == checkedState)
        {
        // the checkbox has been cleared, so remove the master NLM item...
        auto    CNLMTarget*       recPtr;
        recPtr = (CNLMTarget*)m_listCtrl.GetItemData(MASTER_NLM_ITEM_INDEX);
        if (NULL != recPtr)
            {
            m_targetsListPtr->RemoveTarget(recPtr);
            }

        // remove the item from the list control
        result = m_listCtrl.DeleteItem(MASTER_NLM_ITEM_INDEX);
        }
    else
        {
        // the checkbox has been set, so add the master NLM item...

        auto    CNLMTarget*     recPtr;
        try {
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
        if (-1 == m_listCtrl.InsertItem(&newItem))
            {
            result = FALSE;
            }
        else
            {
            // finally, save a pointer to this item's corresponding record
            // with  the listbox item
            m_listCtrl.SetItemData(0, (DWORD)recPtr);
            }
        }

    return (result);

}  // end of "CAlertNlmPropertyPage::UpdateMasterNLMItem"



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

int CAlertNlmPropertyPage::UpdateNLMItemIcons(BOOL  checkedState)
{
    auto    int         nIndex = 0;
    auto    int         numItems;
    auto    LV_ITEM     currentItem;


    // first see if there are any items to update
    numItems = m_listCtrl.GetItemCount();
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
        if (0 != m_listCtrl.GetItem(&currentItem))
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
            m_listCtrl.SetItem(&currentItem);
            }
        }

allDone:
    return (nIndex);

}  // end of "CAlertNlmPropertyPage::UpdateNLMItemIcons"

















/////////////////////////////////////////////////////////////////////////////
// CAlertNlmPropertyPage member functions
/////////////////////////////////////////////////////////////////////////////




// ===== OnClickNlmTargetList =============================================
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

void    CAlertNlmPropertyPage::OnClickNlmTargetList(NMHDR* pNMHDR
                                                        , LRESULT* pResult)
{
    // TODO: Add your control notification handler code here

    // if the master NLM checkbox is checked, disable the edit buttons
    if (1 == m_allNlmCheckbox.GetCheck())
        {
        m_addButton.EnableWindow(FALSE);
        m_removeButton.EnableWindow(FALSE);
        m_modifyButton.EnableWindow(FALSE);
        }
    else
        {
        CAlertListPropertyPage::OnClickAlertsTargetsList(pNMHDR, pResult);
        }

	*pResult = 0;

}  // end of "CAlertNlmPropertyPage::OnClickNlmTargetList"



// ===== OnDblclkNetmessageTargetList =====================================
//
//  This function handles a double-click on the list control.
//
//  Input:  the default MFC input arguments
//  Output: the default MFC return value
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

void    CAlertNlmPropertyPage::OnDblclkNlmTargetList(NMHDR* pNMHDR
                                                        , LRESULT* pResult)
{
	// TODO: Add your control notification handler code here

    CAlertListPropertyPage::OnDblclkAlertsTargetsList(pNMHDR, pResult);
	*pResult = 0;

}  // end of "CAlertNlmPropertyPage::OnDblclkNetmessageTargetList"



// ===== OnIncludeAllNlmsCheckbox =========================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

void    CAlertNlmPropertyPage::OnIncludeAllNlmsCheckbox(void)
{
	// TODO: Add your control notification handler code here


    // get the current state of the checkbox so we know what to do
    auto    int     bChecked;
    bChecked = m_allNlmCheckbox.GetCheck();

    // based on the checkbox state, add or remove the master NLM item
    UpdateMasterNLMItem(bChecked);

    // then update the other list control items (if any)
    UpdateNLMItemIcons(bChecked);

    // update the edit button states
    if (0 == bChecked)  // no master NLM item
        {
        m_addButton.EnableWindow(TRUE);
        m_removeButton.EnableWindow(FALSE);
        m_modifyButton.EnableWindow(FALSE);
        }
    else    // checkbox is checked
        {
        m_addButton.EnableWindow(FALSE);
        m_removeButton.EnableWindow(FALSE);
        m_modifyButton.EnableWindow(FALSE);
        }

}  // end of "CAlertNlmPropertyPage::OnIncludeAllNlmsCheckbox"



// ===== OnInitDialog =====================================================
//
//  Input:  nothing
//  Output: a value of TRUE
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

BOOL    CAlertNlmPropertyPage::OnInitDialog(void)
{
	CAlertMachinePropertyPage::OnInitDialog();

	// TODO: Add extra initialization here

    // if the "master" NLM item is in the listbox, make adjustments
    MasterNLMItemInit();

    // set the page's caption
    auto    CString     tempStr;
    if (tempStr.LoadString(IDS_TARGET_PAGE_CAPTION))
        {
        SetWindowText(tempStr);
        }

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CAlertNlmPropertyPage::OnInitDialog"