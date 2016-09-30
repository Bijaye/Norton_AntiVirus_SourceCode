/////////////////////////////////////////////////////////////////////
// ATForwPg.cpp : implementation file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ATFORWPG.CPv   1.1   25 Apr 1997 11:04:40   JBRENNA  $
//
// Description:
//
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ATFORWPG.CPv  $
// 
//    Rev 1.1   25 Apr 1997 11:04:40   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   15 Apr 1997 20:08:28   sedward
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AlertUI.h"
#include "AlertOpt.h"
#include "ATForwPg.h"
#include "help_ids.h"               // help IDs


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// the "what's this" and "how to" maps
DWORD   dwForwardWhatsThisMap[] =
{
    IDC_ADD_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_ADD_ITEM_BTN
    , IDC_MODIFY_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_MODIFY_ITEM_BTN
    , IDC_REMOVE_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_REMOVE_ITEM_BTN
    , IDC_ALERTS_TARGETS_LIST, IDH_NAVNT_WHATHIS_MESSERV_TARGETS_LSTBX
    , 0,0
};


DWORD   dwForwardHowToMap[] =
{
    ID_HELP, IDH_NAVNT_MESSENGER_SERVICE_DLG_HELP_BTN   // generic list help
    , ID_HT_EDITLIST_FORWARD, IDH_NAVNT_TASK_EDIT_THE_RELAY_TO_REMOTE_MACHINES_TARGET_LIST
    , ID_HT_ADD_FORWARD, IDH_NAVNT_TASK_ADD_A_RELAY_TO_REMOTE_MACHINE_TARGET
    , ID_HT_MODIFY_FORWARD, IDH_NAVNT_TASK_EDIT_A_SINGLE_RELAY_TO_REMOTE_MACHINE_TARGET
    , ID_HT_REMOVE_FORWARD, IDH_NAVNT_TASK_REMOVE_A_RELAY_TO_REMOTE_MACHINES_TARGET
    , 0,0,
};




/////////////////////////////////////////////////////////////////////////////
// CAlertForwardPropertyPage property page
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CAlertForwardPropertyPage, CAlertMachinePropertyPage)


CAlertForwardPropertyPage::CAlertForwardPropertyPage()
    : CAlertMachinePropertyPage()
{
    InitializeData();
}


CAlertForwardPropertyPage::CAlertForwardPropertyPage(UINT  dlgID)
    : CAlertMachinePropertyPage(dlgID)
{
    InitializeData();
}


CAlertForwardPropertyPage::~CAlertForwardPropertyPage()
{
}

void CAlertForwardPropertyPage::DoDataExchange(CDataExchange* pDX)
{
    CAlertMachinePropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAlertForwardPropertyPage)
	DDX_Control(pDX, IDC_FORWARD_TARGET_LIST, m_listCtrl);
	DDX_Control(pDX, IDC_REMOVE_BUTTON, m_removeButton);
	DDX_Control(pDX, IDC_MODIFY_BUTTON, m_modifyButton);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_addButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlertForwardPropertyPage, CAlertMachinePropertyPage)
    //{{AFX_MSG_MAP(CAlertForwardPropertyPage)
	ON_NOTIFY(NM_CLICK, IDC_FORWARD_TARGET_LIST, OnClickForwardTargetList)
	ON_NOTIFY(NM_DBLCLK, IDC_FORWARD_TARGET_LIST, OnDblclkForwardTargetList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()





/////////////////////////////////////////////////////////////////////////////
// CAlertForwardPropertyPage member functions
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

BOOL    CAlertForwardPropertyPage::CopyAlertTargetInfo(CAlertTarget*  source
                                                        , CAlertTarget*  dest)
{
    auto    CForwardTarget*     tempDest = (CForwardTarget*)dest;
    auto    CForwardTarget*     tempSource = (CForwardTarget*)source;

    // copy the source object data over to the destination object
    tempDest->SetAddress(tempSource->GetAddress());
    tempDest->SetActive(tempSource->GetActive());
    tempDest->SetPriority(tempSource->GetPriority());
    return  (TRUE);

}  // end of "CAlertForwardPropertyPage::CopyAlertTargetInfo"



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

BOOL    CAlertForwardPropertyPage::GetDisplayString(CAlertTarget*  pAlertTarget
                                                        , CString&  displayStr
                                                        , int  index)
{
    auto    BOOL                result = TRUE;
    auto    CForwardTarget*     forwardPtr = (CForwardTarget*)pAlertTarget;

    if (index > (m_totalDisplayStrings - 1))
        {
        result = FALSE;
        }
    else
        {
        switch (index)
            {
            case   0:
                displayStr = forwardPtr->GetAddress();
                break;

            default:
                result = FALSE;
                break;
            }
        }

    return  (result);

}  // end of "CAlertForwardPropertyPage::GetDisplayString"



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

CAlertTarget*   CAlertForwardPropertyPage::GetNewAlertTarget(void)
{
    auto    CForwardTarget*     newAlertTarget;

    try {
        newAlertTarget = new  CForwardTarget;
        }

    catch(CMemoryException)
        {
        newAlertTarget = NULL;
        throw;      // forward the exception object to the next handler
        }

    return  (newAlertTarget);

}  // end of "CAlertForwardPropertyPage::GetNewAlertTarget"



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

BOOL    CAlertForwardPropertyPage::InitializeData(void)
{
    auto    BOOL        result = TRUE;

    // set the alert type
    m_alertType = TARGET_FORWARD;

    // setup the context help map pointers
    m_pHelpWhatsThisMap = dwForwardWhatsThisMap;
    m_pHelpHowToMap = dwForwardHowToMap;

    // initialize the "how to" index for this dialog
    SetHowToMenuIndex(HOW_TO_INDEX_FORWARD);

    // initialize some control IDs
    m_listCtrlContextMenuID = IDR_RMENU_FORWARDPAGE_TARGET;
    m_listCtrlWhatsThisMenuID = IDR_WHATS_THIS_FORWARD_LISTCTRL;
    m_listCtrlWhatsThisHelpID = IDH_NAVNT_WHATHIS_MESSERV_TARGETS_LSTBX;
    m_listCtrlID = IDC_FORWARD_TARGET_LIST;

    // initialize pointers to the controls
    m_listCtrlPtr = &m_listCtrl;
    m_addButtonPtr = &m_addButton;
    m_modifyButtonPtr = &m_modifyButton;
    m_removeButtonPtr = &m_removeButton;

    // initialize the column headers
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

}  // end of "CAlertForwardPropertyPage::InitializeData"



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

BOOL    CAlertForwardPropertyPage::TargetIsActive(CAlertTarget*  pAlertTarget)
{
    auto    CForwardTarget*     forwardAlertTarget;
    forwardAlertTarget = (CForwardTarget*)pAlertTarget;
    return  (forwardAlertTarget->GetActive());

}  // end of "CAlertForwardPropertyPage::TargetIsActive"

















/////////////////////////////////////////////////////////////////////////////
// CAlertForwardPropertyPage message handlers
/////////////////////////////////////////////////////////////////////////////



// ===== OnClickForwardTargetList =========================================
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

void    CAlertForwardPropertyPage::OnClickForwardTargetList(NMHDR* pNMHDR
                                                            , LRESULT* pResult)
{
	// TODO: Add your control notification handler code here

    CAlertListPropertyPage::OnClickAlertsTargetsList(pNMHDR, pResult);
	*pResult = 0;

}  // end of "CAlertForwardPropertyPage::OnClickForwardTargetList"



// ===== OnDblclkForwardTargetList ========================================
//
//  This function handles a double-click on the list control.
//
//  Input:  the default MFC input arguments
//  Output: the default MFC return value
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

void    CAlertForwardPropertyPage::OnDblclkForwardTargetList(NMHDR* pNMHDR
                                                            , LRESULT* pResult)
{
	// TODO: Add your control notification handler code here

    CAlertListPropertyPage::OnDblclkAlertsTargetsList(pNMHDR, pResult);
	*pResult = 0;

}  // end of "CAlertForwardPropertyPage::OnDblclkForwardTargetList"



// ===== OnInitDialog =====================================================
//
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

BOOL    CAlertForwardPropertyPage::OnInitDialog(void)
{
	CAlertMachinePropertyPage::OnInitDialog();

    // TODO: Add extra initialization here

    // set the page's caption
    auto    CString     tempStr;
    if (tempStr.LoadString(IDS_TARGET_PAGE_CAPTION))
        {
        SetWindowText(tempStr);
        }

    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CAlertForwardPropertyPage::OnInitDialog"