/////////////////////////////////////////////////////////////////////
// ATMessPg.cpp : implementation file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ATMESSPG.CPv   1.1   25 Apr 1997 11:04:28   JBRENNA  $
//
// Description:
//
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ATMESSPG.CPv  $
// 
//    Rev 1.1   25 Apr 1997 11:04:28   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   15 Apr 1997 20:08:22   sedward
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AlertUI.h"
#include "AlertOpt.h"
#include "ATMessPg.h"
#include "help_ids.h"               // help IDs


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// the "what's this" and "how to" maps
DWORD   dwNetMessengerWhatsThisMap[] =
{
    IDC_ADD_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_ADD_ITEM_BTN
    , IDC_MODIFY_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_MODIFY_ITEM_BTN
    , IDC_REMOVE_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_REMOVE_ITEM_BTN
    , IDC_ALERTS_TARGETS_LIST, IDH_NAVNT_WHATHIS_MESSERV_TARGETS_LSTBX
    , 0,0
};


DWORD   dwNetMessengerHowToMap[] =
{
    ID_HELP, IDH_NAVNT_MESSENGER_SERVICE_DLG_HELP_BTN   // generic list help
    , ID_HT_ADD_NETMESSENGER, IDH_NAVNT_TASK_ADD_A_NT_MESSENGER_SERVICE_TARGET
    , ID_HT_REMOVE_NETMESSENGER, IDH_NAVNT_TASK_REMOVE_AN_NT_MESSENGER_SERVICE_TARGET
    , ID_HT_EDITLIST_NETMESSENGER, IDH_NAVNT_TASK_EDIT_THE_NT_MESSENGER_SERVICE_TARGET_LIST
    , ID_HT_MODIFY_NETMESSENGER, IDH_NAVNT_TASK_EDIT_A_SINGLE_NT_MESSENGER_SERVICE_TARGET
    , 0,0,
};





/////////////////////////////////////////////////////////////////////////////
// CAlertNetMessengerPropertyPage property page
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CAlertNetMessengerPropertyPage, CAlertMachinePropertyPage)

CAlertNetMessengerPropertyPage::CAlertNetMessengerPropertyPage()
    : CAlertMachinePropertyPage()
{
    InitializeData();
}


CAlertNetMessengerPropertyPage::CAlertNetMessengerPropertyPage(UINT  dlgID)
    : CAlertMachinePropertyPage(dlgID)
{
    InitializeData();
}


CAlertNetMessengerPropertyPage::~CAlertNetMessengerPropertyPage()
{
}

void CAlertNetMessengerPropertyPage::DoDataExchange(CDataExchange* pDX)
{
    CAlertMachinePropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAlertNetMessengerPropertyPage)
    DDX_Control(pDX, IDC_NETMESSAGE_TARGET_LIST, m_listCtrl);
	DDX_Control(pDX, IDC_REMOVE_BUTTON, m_removeButton);
	DDX_Control(pDX, IDC_MODIFY_BUTTON, m_modifyButton);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_addButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlertNetMessengerPropertyPage, CAlertMachinePropertyPage)
    //{{AFX_MSG_MAP(CAlertNetMessengerPropertyPage)
	ON_NOTIFY(NM_CLICK, IDC_NETMESSAGE_TARGET_LIST, OnClickNetmessageTargetList)
	ON_NOTIFY(NM_DBLCLK, IDC_NETMESSAGE_TARGET_LIST, OnDblclkNetmessageTargetList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAlertNetMessengerPropertyPage:: member functions
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

BOOL    CAlertNetMessengerPropertyPage::CopyAlertTargetInfo(CAlertTarget*  source
                                                        , CAlertTarget*  dest)
{
    auto    CNetMsgTarget*      tempDest = (CNetMsgTarget*)dest;
    auto    CNetMsgTarget*      tempSource = (CNetMsgTarget*)source;

    // copy the source object data over to the destination object
    tempDest->SetAddress(tempSource->GetAddress());
    tempDest->SetActive(tempSource->GetActive());
    tempDest->SetPriority(tempSource->GetPriority());
    return  (TRUE);

}  // end of "CAlertNetMessengerPropertyPage::CopyAlertTargetInfo"



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

BOOL    CAlertNetMessengerPropertyPage::GetDisplayString(CAlertTarget*  pAlertTarget
                                                        , CString&  displayStr
                                                        , int  index)
{
    auto    BOOL                result = TRUE;
    auto    CNetMsgTarget*      netMessengerPtr = (CNetMsgTarget*)pAlertTarget;

    if (index > (m_totalDisplayStrings - 1))
        {
        result = FALSE;
        }
    else
        {
        switch (index)
            {
            case   0:
                displayStr = netMessengerPtr->GetAddress();
                break;

            default:
                result = FALSE;
                break;
            }
        }

    return  (result);

}  // end of "CAlertNetMessengerPropertyPage::GetDisplayString"



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

CAlertTarget*   CAlertNetMessengerPropertyPage::GetNewAlertTarget(void)
{
    auto    CNetMsgTarget*      newAlertTarget;

    try {
        newAlertTarget = new  CNetMsgTarget;
        }

    catch(CMemoryException)
        {
        newAlertTarget = NULL;
        throw;      // forward the exception object to the next handler
        }

    return  (newAlertTarget);

}  // end of "CAlertNetMessengerPropertyPage::GetNewAlertTarget"



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

BOOL    CAlertNetMessengerPropertyPage::InitializeData(void)
{
    auto    BOOL        result = TRUE;

    // setup the alert type
    m_alertType = TARGET_MESSAGE_NETWORK;

    // setup the context help map pointers
    m_pHelpWhatsThisMap = dwNetMessengerWhatsThisMap;
    m_pHelpHowToMap = dwNetMessengerHowToMap;

    // initialize the "how to" index for this dialog
    SetHowToMenuIndex(HOW_TO_INDEX_NET_MESSENGER);

    // initialize the parent class pointers
    m_listCtrlPtr = &m_listCtrl;
    m_addButtonPtr = &m_addButton;
    m_modifyButtonPtr = &m_modifyButton;
    m_removeButtonPtr = &m_removeButton;

    // initialize some control IDs
    m_listCtrlContextMenuID = IDR_RMENU_NETMESSAGEPAGE_TARGET;
    m_listCtrlWhatsThisMenuID = IDR_WHATS_THIS_NETMESSAGE_LISTCTRL;
    m_listCtrlWhatsThisHelpID = IDH_NAVNT_WHATHIS_MESSERV_TARGETS_LSTBX;
    m_listCtrlID = IDC_NETMESSAGE_TARGET_LIST;

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

} // end of "CAlertNetMessengerPropertyPage::InitializeData"



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

BOOL    CAlertNetMessengerPropertyPage::TargetIsActive(
                                                    CAlertTarget*  pAlertTarget)
{
    auto    CNetMsgTarget*      netMessengerAlertTarget;
    netMessengerAlertTarget = (CNetMsgTarget*)pAlertTarget;
    return  (netMessengerAlertTarget->GetActive());

}  // end of "CAlertNetMessengerPropertyPage::TargetIsActive"




















/////////////////////////////////////////////////////////////////////////////
// CAlertNetMessengerPropertyPage member functions
/////////////////////////////////////////////////////////////////////////////


// ===== OnClickNetmessageTargetList ======================================
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

void    CAlertNetMessengerPropertyPage::OnClickNetmessageTargetList(NMHDR* pNMHDR
                                                                , LRESULT* pResult)
{
	// TODO: Add your control notification handler code here

    CAlertListPropertyPage::OnClickAlertsTargetsList(pNMHDR, pResult);
	*pResult = 0;

}  // end of "CAlertNetMessengerPropertyPage::OnClickNetmessageTargetList"



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

void    CAlertNetMessengerPropertyPage::OnDblclkNetmessageTargetList(NMHDR* pNMHDR
                                                                , LRESULT* pResult)
{
	// TODO: Add your control notification handler code here

    CAlertListPropertyPage::OnDblclkAlertsTargetsList(pNMHDR, pResult);
	*pResult = 0;

}  // end of "CAlertNetMessengerPropertyPage::OnDblclkNetmessageTargetList"



// ===== OnInitDialog =====================================================
//
//  Input:  nothing
//  Output: nothing
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

BOOL    CAlertNetMessengerPropertyPage::OnInitDialog(void)
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

}  // end of "CAlertNetMessengerPropertyPage::OnInitDialog"