/////////////////////////////////////////////////////////////////////
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/SNMPAGE.CPv   1.1   28 Jul 1997 09:26:00   DBUCHES  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/SNMPAGE.CPv  $
// 
//    Rev 1.1   28 Jul 1997 09:26:00   DBUCHES
// Added context help to list control.
//
//    Rev 1.0   04 Jun 1997 21:18:48   DBUCHES
// Initial revision.
//
// SNMPage.cpp : implementation file
//

#include "stdafx.h"
#include "alertui.h"
#include "SNMPage.h"
#include "help_ids.h"
#include "wsipx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// the "what's this" and "how to" maps
DWORD   dwSNMPWhatsThisMap[] =
{
    IDC_ADD_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_ADD_ITEM_BTN
    , IDC_MODIFY_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_MODIFY_ITEM_BTN
    , IDC_REMOVE_BUTTON, IDH_NAVNT_WHATHIS_ALERTS_REMOVE_ITEM_BTN
    , IDC_ALERTS_TARGETS_LIST, IDH_NAVNT_WHATHIS_MESSERV_TARGETS_LSTBX
    , 0,0
};


DWORD   dwSNMPHowToMap[] =
{
    ID_HELP, IDH_NAVNT_MESSENGER_SERVICE_DLG_HELP_BTN   // generic list help
    , ID_HT_EDITLIST_FORWARD, IDH_NAVNT_TASK_EDIT_THE_RELAY_TO_REMOTE_MACHINES_TARGET_LIST
    , ID_HT_ADD_FORWARD, IDH_NAVNT_TASK_ADD_A_RELAY_TO_REMOTE_MACHINE_TARGET
    , ID_HT_MODIFY_FORWARD, IDH_NAVNT_TASK_EDIT_A_SINGLE_RELAY_TO_REMOTE_MACHINE_TARGET
    , ID_HT_REMOVE_FORWARD, IDH_NAVNT_TASK_REMOVE_A_RELAY_TO_REMOTE_MACHINES_TARGET
    , 0,0,
};



/////////////////////////////////////////////////////////////////////////////
// CAlertSNMPPropertyPage property page

IMPLEMENT_DYNCREATE(CAlertSNMPPropertyPage, CAlertEmailPropertyPage)

CAlertSNMPPropertyPage::CAlertSNMPPropertyPage() : CAlertEmailPropertyPage(CAlertSNMPPropertyPage::IDD)
{
	//{{AFX_DATA_INIT(CAlertSNMPPropertyPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
    InitializeData();
}

CAlertSNMPPropertyPage::~CAlertSNMPPropertyPage()
{
}

void CAlertSNMPPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CAlertEmailPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlertSNMPPropertyPage)
    DDX_Control(pDX, IDC_ALERTS_TARGETS_LIST, m_listCtrl);
	DDX_Control(pDX, IDC_REMOVE_BUTTON, m_removeButton);
	DDX_Control(pDX, IDC_MODIFY_BUTTON, m_modifyButton);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_addButton);
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlertSNMPPropertyPage, CAlertEmailPropertyPage)
	//{{AFX_MSG_MAP(CAlertSNMPPropertyPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlertSNMPPropertyPage message handlers

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
//  Function created: 6/97, DBUCHES
// ========================================================================

BOOL    CAlertSNMPPropertyPage::InitializeData(void)
{
    auto    BOOL        result = TRUE;

    // set the alert type
    m_alertType = TARGET_SNMP;

    // setup the context help map pointers
    m_pHelpWhatsThisMap = dwSNMPWhatsThisMap;
    m_pHelpHowToMap = dwSNMPHowToMap;

    // initialize the "how to" index for this dialog
    SetHowToMenuIndex(HOW_TO_INDEX_FORWARD);

    // initialize some control IDs
    m_listCtrlContextMenuID = IDR_RMENU_FORWARDPAGE_TARGET;
    m_listCtrlWhatsThisMenuID = IDR_WHATS_THIS_FORWARD_LISTCTRL;
    m_listCtrlWhatsThisHelpID = IDH_NAVNT_WHATHIS_MESSERV_TARGETS_LSTBX;
    m_listCtrlID = IDC_ALERTS_TARGETS_LIST;

    // initialize pointers to the controls
    m_listCtrlPtr = &m_listCtrl;
    m_addButtonPtr = &m_addButton;
    m_modifyButtonPtr = &m_modifyButton;
    m_removeButtonPtr = &m_removeButton;

    // initialize the column headers
    auto    CString     tempStr;
    if (tempStr.LoadString(IDS_COLUMN_HEADER_NAME))
        {
        m_totalDisplayStrings = 3;
        m_columnHeaders.SetSize(m_totalDisplayStrings);
        m_columnHeaders[0] = tempStr;
        tempStr.LoadString(IDS_COLUMN_HEADER_PROTOCOL);
        m_columnHeaders[1] = tempStr;
        tempStr.LoadString(IDS_COLUMN_HEADER_ADDRESS);
        m_columnHeaders[2] = tempStr;
        }
    else
        {
        m_totalDisplayStrings = 0;
        result = FALSE;
        }

    return (result);

}  // end of "CAlertForwardPropertyPage::InitializeData"


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

BOOL    CAlertSNMPPropertyPage::CopyAlertTargetInfo(CAlertTarget*  source
                                                        , CAlertTarget*  dest)
{
    auto    CSNMPTarget*    tempDest = (CSNMPTarget*)dest;
    auto    CSNMPTarget*    tempSource = (CSNMPTarget*)source;
    auto    SOCKADDR        SockAddr;


    // copy the source object data over to the destination object
    tempSource->GetAddress(&SockAddr);
    tempDest->SetAddress(&SockAddr);
    tempDest->SetName(tempSource->GetName());

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

BOOL    CAlertSNMPPropertyPage::GetDisplayString(CAlertTarget*  pAlertTarget
                                                        , CString&  displayStr
                                                        , int  index)
{
    auto    BOOL                result = TRUE;
    auto    CSNMPTarget*        targetPtr = (CSNMPTarget*)pAlertTarget;
    auto    SOCKADDR            SockAddr;

    // Get Address structure.
    targetPtr->GetAddress(&SockAddr);

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
                if( SockAddr.sa_family == AF_INET )
                    displayStr.LoadString(IDS_SNMP_PROTOCOL_INET);
                else if( SockAddr.sa_family == AF_IPX )
                    displayStr.LoadString(IDS_SNMP_PROTOCOL_IPX);
                else    // We're lost.
                    displayStr.Empty();
                break;

            case   2:
                // Need to convert SOCKADDR address format to displayable format
                if( SockAddr.sa_family == AF_INET )
                    {
                    PSOCKADDR_IN p = (PSOCKADDR_IN)&SockAddr;

                    displayStr.Format(_T("%d.%d.%d.%d"),
                                          p->sin_addr.S_un.S_un_b.s_b1,
                                          p->sin_addr.S_un.S_un_b.s_b2,
                                          p->sin_addr.S_un.S_un_b.s_b3,
                                          p->sin_addr.S_un.S_un_b.s_b4
                                          );
                    }
                else
                    {
                    // Build address string
                    PSOCKADDR_IPX p = (PSOCKADDR_IPX)&SockAddr;
                    CString sFormat;
                    displayStr.Empty();
                    for(int i = 0; i < 4; i++)
                        {
                        sFormat.Format(_T("%02X"), (BYTE)p->sa_netnum[i]);
                        displayStr += sFormat;
                        }
                    displayStr += _T(":");
                    for( i = 0; i < 6; i++)
                        {
                        sFormat.Format(_T("%02X"), (BYTE)p->sa_nodenum[i]);
                        displayStr += sFormat;
                        }
                    }
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

CAlertTarget*   CAlertSNMPPropertyPage::GetNewAlertTarget(void)
{
    auto    CSNMPTarget*       snmpAlertTarget;

    try {
        snmpAlertTarget = new  CSNMPTarget;
        }

    catch(CMemoryException)
        {
        snmpAlertTarget = NULL;
        throw;      // forward the exception object to the next handler
        }

    return  (snmpAlertTarget);

}  // end of "CAlertSmtpPropertyPage::GetNewAlertTarget"


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

BOOL    CAlertSNMPPropertyPage::TargetIsActive(CAlertTarget*  pAlertTarget)
{
    auto    CSNMPTarget*       targetPtr;
    targetPtr = (CSNMPTarget*)pAlertTarget;
    return  (targetPtr->GetActive());

}  // end of "CAlertSmtpPropertyPage::TargetIsActive"

// ==== HandleTargetEdit ==================================================
//
//  This function handles the editing of an alert target object.
//
//  NOTE: this is an implementation of the virtual function inherited from
//        the CAlertListPropertyPage class
//
//  Input:
//      pAlertTarget    -- a pointer to the alert target record
//      mode            -- an integer indicating the mode of the edit (ie, add
//                         or modify)
//
//  Output:
//      the result of the call to HandleMachineTargetEdit()
//
// ========================================================================
//  Function created: 3/97, SEDWARD
// ========================================================================

BOOL    CAlertSNMPPropertyPage::HandleTargetEdit(CAlertTarget*  pAlertTarget
                                                                    , int  mode)
{
	// TODO: Add your specialized code here and/or call the base class

    auto    BOOL        result;
    result = HandleSNMPEdit( pAlertTarget, mode);
    return  (result);

}  // end of "CAlertEmailPropertyPage::HandleTargetEdit"



