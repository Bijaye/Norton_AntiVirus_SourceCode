/////////////////////////////////////////////////////////////////////
// AlertUI.cpp : implementation file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ALERTUI.CPv   1.5   07 Jul 1998 12:26:34   DBuches  $
//
// Description:
//  Contains the initialization routines for the DLL, as well as some
//  general purpose (non-class) functions.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ALERTUI.CPv  $
// 
//    Rev 1.5   07 Jul 1998 12:26:34   DBuches
// Added help file support.
// 
//    Rev 1.4   04 Jun 1997 21:14:42   DBUCHES
// Changed to support SNMP Alerts.
// 
//    Rev 1.3   27 May 1997 22:15:42   DBUCHES
// Added SaveAlertOptions()
// 
//    Rev 1.2   27 May 1997 20:13:12   DBUCHES
// Converted this project to an MFC extension DLL.  Many changes.
// 
//    Rev 1.1   25 Apr 1997 11:03:10   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.1   15 Apr 1997 20:02:44   sedward
// Removed functions now handled by CAlertListPropertyPage class.
//
//    Rev 1.18   02 Feb 1997 16:35:52   SEDWARD
// Added 'CheckGroupboxControl'.
//
//    Rev 1.17   28 Jan 1997 20:01:32   SEDWARD
// Added 'InitControlsEx'.
//
//    Rev 1.16   27 Jan 1997 15:30:44   SEDWARD
// Removed 'navHelpFilenameStr' variable.
//
//    Rev 1.15   19 Jan 1997 23:42:44   SEDWARD
// Updated help maps, implemented multiple selection/removal of list control
// items.
//
//    Rev 1.14   13 Jan 1997 20:41:14   SEDWARD
// Added support for help.
//
//    Rev 1.13   06 Jan 1997 00:31:40   SEDWARD
// Fixed pointer problem in HandleMachineTargetEdit.
//
//    Rev 1.10   09 Dec 1996 20:47:34   SEDWARD
//
//    Rev 1.9   03 Dec 1996 19:48:42   SEDWARD
// Substantial changes (not frozen yet...).
//
//    Rev 1.7   02 Dec 1996 19:33:44   SEDWARD
// Added 'GetAlertOptDlgHwnd'
//
//    Rev 1.6   02 Dec 1996 16:33:40   SEDWARD
// More big changes, nearing UI freeze.
//
//    Rev 1.5   27 Nov 1996 18:49:14   SEDWARD
// Hooked up NAVOPTS to the local alert message configuration dialog.
//
//    Rev 1.4   26 Nov 1996 17:21:02   SEDWARD
// Leading backslashes no longer written to DAT file, domains get a trailing
// space/asterisk appened to them.
//
//    Rev 1.3   26 Nov 1996 00:56:24   SEDWARD
// More big changes.
//
//    Rev 1.2   25 Nov 1996 15:46:32   SEDWARD
// Major overhaul (more to come).
//
//    Rev 1.1   21 Nov 1996 17:07:46   SEDWARD
// Added vlog headers.
//
//    Rev 1.0   21 Nov 1996 16:55:06   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////




#include "stdafx.h"
#include "at_uiexp.h"       // exported function prototypes
#include "AlertUI.h"
#include "AlertOpt.h"
#include "ATSmtpPg.h"
#include "EditSMTP.h"
#include "dllinst.h"
#include "SNMPEdit.h"
#include "navwhlp5.h"

// global variables
global  CAlertOptsDlg*      gAlertOptsDlgPtr;   // alert options dialog
global  CAlertOptions       gAlertOptions;      // alert options class
global  LPNAVOPTIONS        glpNavOptions;      // NAV options

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlertUIApp
/////////////////////////////////////////////////////////////////////////////

//static AFX_EXTENSION_MODULE AlertUIDLL = { NULL, NULL };
AFX_EXTENSION_MODULE EXTENSIONDLL = { NULL, NULL };            //**Make sure this variable is changed to the name shown here!****

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("ALERT.DLL Initializing!\n");

		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(EXTENSIONDLL, hInstance))
			return 0;

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("ALERTUI.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(EXTENSIONDLL);
	}
	return 1;   // ok
}





// "global" functions (don't belong to any particular class)


// ==== CheckGroupboxControl ==============================================
//
//  This function checks if the input CPoint pointer is within the bounds
//  of controls specified to be in a groupbox control.  If so, a pointer
//  to the "real" window is returned, otherwise a NULL value is returned.
//
//  NOTE: this is necessary because NAV currently permits context help on
//  grayed out controls.  In the OnContext handler, if the target item is
//  grayed out, the CWnd pointer will be equal to "this".  The "CHelpDialog"
//  and "CHelpPropertyPage" classes attempt to resolve this situation with
//  a call to "ChildWindowFromPoint", but if the target control is in a
//  groupbox a pointer will to the groupbox will be returned, NOT a pointer
//  to the control within the groupbox.
//
//  Input:  nothing
//      pointPtr    -- a pointer to the CPoint object that is passed into
//                     the OnContext routine
//
//      wndPtrArray -- a pointer to an array of CWnd pointers (ie, each
//                     pointer in the array points to a control in a
//                     groupbox)
//
//  Output:
//      If the input CPoint structure falls within the boundaries of one
//      of the controls specifiec to be in a groupbox, a pointer to the
//      "real" window is returned; else, a value of NULL is returned.
//
// ========================================================================
//  Function created: 1/97, SEDWARD
// ========================================================================

CWnd*   CheckGroupboxControl(CPoint*  pointPtr, CWnd**  wndPtrArray)
{
    auto    int         index;
    auto    CRect       tempRect;
    auto    CWnd*       tempWndPtr = NULL;
    auto    CWnd*       trueWndPtr = NULL;

    for (index = 0; NULL != wndPtrArray[index]; ++index)
        {
        tempWndPtr = wndPtrArray[index];
        tempWndPtr->GetWindowRect(tempRect);
        if (TRUE == tempRect.PtInRect(*pointPtr))
            {
            trueWndPtr = tempWndPtr;
            break;
            }
        }

    return (trueWndPtr);

}  // end of "CheckGroupboxControl"



// ==== GetEmailDisplayStr ================================================
//
//
//  Input:
//      recPtr  -- a pointer to a valid CEMailTarget object
//
//  Output: nothing
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    GetEmailDisplayStr(CEMailTarget*  recPtr, CString&  displayStr)
{
    displayStr = _T("<");
    displayStr += recPtr->GetName();
    displayStr += _T(">  ");
    displayStr += recPtr->GetAddress();

}  // end of "GetEmailDisplayStr"



// ==== HandleMachineTargetEdit ===========================================
//
//  This function handles the editing of a "machine" alert target.  The
//  input record pointer is used to derive the type of network browser to
//  create, and the "mode" argument is used to indicate an add or modify
//  edit mode.
//
//  Input:
//      recPtr  -- a pointer to a valid CAlertTarget object
//      mode    -- a flag indicating either "add" or "modify" mode
//
//
//  Output: a value of TRUE if an edit was successfully completed, FALSE
//          if not
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    HandleMachineTargetEdit(CAlertTarget*  recPtr, int  mode
                                                        , CWnd*  parentWnd)
{
    auto    BOOL                result = TRUE;
    auto    CForwardTarget*     forwardPtr;
    auto    CNetMsgTarget*      netMessPtr;
    auto    CNLMTarget*         nlmPtr;
    auto    CString             tempStr;
    auto    int                 netBrowseType;
    auto    int                 targetType;
    auto    int                 strLen;
    auto    DWORD               dwTargetResourceType=RESOURCE_TYPE_SERVER;

    if (NULL == recPtr)
        {
        result = FALSE;
        goto  allDone;
        }

    // get a pointer of the correct type to the input argument
    targetType = recPtr->GetTargetType();
    switch (targetType)
        {
        case TARGET_MESSAGE_NETWORK:
            netMessPtr = (CNetMsgTarget*)recPtr;
            if (EDIT_MODIFY == mode)
                {
                tempStr = netMessPtr->GetAddress();
                dwTargetResourceType = netMessPtr->GetResourceType();
                }
            netBrowseType = BROWSER_TYPE_NETWORK_MESSAGE;
            break;

        case TARGET_NLM:
            nlmPtr = (CNLMTarget*)recPtr;
            if (EDIT_MODIFY == mode)
                {
                tempStr = nlmPtr->GetAddress();
                }
            netBrowseType = BROWSER_TYPE_NLM;
            break;

        case TARGET_FORWARD:
            forwardPtr = (CForwardTarget*)recPtr;
            if (EDIT_MODIFY == mode)
                {
                tempStr = forwardPtr->GetAddress();
                }
            netBrowseType = BROWSER_TYPE_FORWARD;
            break;

        default:
            result = FALSE;
            break;
        }

    if (TRUE == result)
        {
        // only modify string if the resource is of type "server"
        if ( dwTargetResourceType == RESOURCE_TYPE_SERVER )
            {
            // if we're not dealing with a domain, insert the dummy
            // backslashes for looks
            if (FALSE == IsDomainString(tempStr))
                {
                InsertServerBackslashes(tempStr);
                }
            }

        // if we're in "modify" mode, initialize the net browser's field
        // with the record's current address string
        auto    CNetBrowsDlg    cBrowseDlg(netBrowseType, parentWnd);
        if (EDIT_MODIFY == mode)
            {
            cBrowseDlg.SetInitialBrowseItemName(tempStr);
            }

        //
        // Set up help.
        //
        cBrowseDlg.SetHelpFileInfo( IDH_NAVNT_NETWORK_BROWSER_DLG_HELP_BTN, NAV_HELPFILENAME );
        
        // browse...
        result = cBrowseDlg.DoModal();
        if (IDOK == result)
            {
            tempStr = cBrowseDlg.GetSelectedItemName();
            dwTargetResourceType = cBrowseDlg.GetSelectedItemType();

            switch(dwTargetResourceType)
                {
                case  RESOURCE_TYPE_DOMAIN:
                    // if the resulting string is a domain, append the
                    // domain string
                    if (FALSE == IsDomainString(tempStr))
                        {
                        // no leading backslashes, must be a domain
                        tempStr += APPEND_DOMAIN_STR;
                        }
                    break;

                case  RESOURCE_TYPE_SERVER:
                    // remove the two leading backslashes before writing to
                    // the data file
                    strLen = tempStr.GetLength();
                    tempStr = tempStr.Right(strLen - 2);
                    break;

                case  RESOURCE_TYPE_CUSTOM:
                    break;

                default:
                    break;
                }

            // update the target record's address field
            switch (targetType)
                {
                case TARGET_MESSAGE_NETWORK:
                    netMessPtr->SetAddress(tempStr);
                    netMessPtr->SetResourceType(dwTargetResourceType);
                    break;

                case TARGET_NLM:
                    nlmPtr->SetAddress(tempStr);
                    break;

                case TARGET_FORWARD:
                    forwardPtr->SetAddress(tempStr);
                    break;

                default:
                    break;
                }
            }
        }

allDone:
    return (result);

}  // end of "HandleMachineTargetEdit"



// ==== HandleSMTPEdit ====================================================
//
//  This function is used to handle the editing of SMTP alert targets.
//
//  Input:
//      recPtr  -- a pointer to a valid CEMailTarget object
//      mode    -- indicates whether we're in "add" or "modify" mode
//
//  Output: the result of the CDialog::DoModal call
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    HandleSMTPEdit(CAlertTarget*  recPtr, int  mode)
{
    // prompt the user for new record information
    auto    BOOL            result = FALSE;
    auto    CEMailTarget*   emailRecPtr;
    auto    CEmailEditSMTP  dlg;
    auto    int             nResponse;
    auto    int             targetType;


    // get a pointer of the correct type to the input argument
    targetType = recPtr->GetTargetType();
    switch (targetType)
        {
        case  TARGET_EMAIL:
            // cast the input pointer
            emailRecPtr = (CEMailTarget*)recPtr;

            // if we're modifying an existing record, initialize the dialog's
            // edit fields with the record's contents
            if (EDIT_MODIFY == mode)
                {
                dlg.m_internetNameStr = emailRecPtr->GetName();
                dlg.m_internetAddressStr = emailRecPtr->GetAddress();
                }

            // set the edit dialog on the screen
            nResponse = dlg.DoModal();
            if (IDOK == nResponse)
                {
                emailRecPtr->SetName(dlg.m_internetNameStr);
                emailRecPtr->SetAddress(dlg.m_internetAddressStr);
                result = IsValidEmailRec(emailRecPtr);
                }
            break;

        default:
            break;
        }

    return (result);

}  // end of "HandleSMTPEdit"

// ==== HandleSNMPEdit ====================================================
//
//  This function is used to handle the editing of SMTP alert targets.
//
//  Input:
//      recPtr  -- a pointer to a valid CEMailTarget object
//      mode    -- indicates whether we're in "add" or "modify" mode
//
//  Output: the result of the CDialog::DoModal call
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    HandleSNMPEdit(CAlertTarget*  recPtr, int  mode)
{
    // prompt the user for new record information
    auto    BOOL            result = FALSE;
    auto    CSNMPTarget*    snmpRecPtr;
    auto    CSNMPEditDlg    dlg;
    auto    int             nResponse;
    auto    int             targetType;


    // get a pointer of the correct type to the input argument
    targetType = recPtr->GetTargetType();
    switch (targetType)
        {
        case  TARGET_SNMP:
            // cast the input pointer
            snmpRecPtr = (CSNMPTarget*)recPtr;

            // if we're modifying an existing record, initialize the dialog's
            // edit fields with the record's contents
            if (EDIT_MODIFY == mode)
                {
                dlg.m_sTargetName = snmpRecPtr->GetName();
                snmpRecPtr->GetAddress(&dlg.m_SockAddr);
                }

            // set the edit dialog on the screen
            nResponse = dlg.DoModal();
            if (IDOK == nResponse)
                {
                snmpRecPtr->SetName(dlg.m_sTargetName);
                snmpRecPtr->SetAddress(&dlg.m_SockAddr);
                result = IDOK;
                }
            break;

        default:
            break;
        }

    return (result);

}  // end of "HandleSMTPEdit"


// ===== InsertServerBackslashes ==========================================
//
//  This function inserts leading double-backslashes at the beginning of the
//  input string if they are not already there.
//
//  Input:
//      serverStr   -- the string containing the server name
//
//  Output:
//      A value of TRUE if the backslashes are inserted, FALSE otherwise
//
// ========================================================================
//  Function created: 12/96, SEDWARD
// ========================================================================

BOOL    InsertServerBackslashes(CString&  serverStr)
{
    auto    BOOL        bInserted;

    // only insert the leading backslashes if they're not already there
    if (FALSE == serverStr.IsEmpty())
        {
        if  ((serverStr[0] != SINGLE_BACKSLASH)
        &&  (serverStr[1] != SINGLE_BACKSLASH))
            {
            serverStr = DOUBLE_BACKSLASH + serverStr;
            bInserted = TRUE;
            }
        }
    else
        {
        bInserted = FALSE;
        }

    return (bInserted);

}  // end of "InsertServerBackslashes"



// ===== IsDomainString ===================================================
//
//  This function tests the input string to see if it's in "domain" format
//  (ie, a domain string appended to the end).
//
//  Input:  inputStr    -- a reference to the candidate string
//  Output: a value of TRUE if the input string is determined to be a domain
//          string, FALSE otherwise.
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    IsDomainString(CString&  inputStr)
{
    auto    BOOL        result = FALSE;

    if (APPEND_DOMAIN_STR == inputStr.Right(1))
        {
        result = TRUE;
        }

    return (result);

}  // end of "IsDomainString"



// === IsValidEmailRec ======================================================
//
//  This function is used to check the validity of a target email record.
//
//  Input:  recPtr  -- a pointer to an email target object
//  Output: a value of TRUE if the record is valid, FALSE otherwise
//
// ==========================================================================
//  Function created: 11/96, SEDWARD
// ==========================================================================

BOOL    IsValidEmailRec(CEMailTarget*  recPtr)
{
    auto    BOOL        result;

    // if either the name or address fields are emtpy, the record is invalid
    if  ((_tcslen(recPtr->GetName()) <= 0)
    ||  (_tcslen(recPtr->GetAddress()) <= 0))
        {
        result = FALSE;
        }
    else
        {
        result = TRUE;
        }

    return (result);

}  // end of "IsValidEmailRec"



// ==== MasterNLMItemInTargetsList ========================================
//
//  This function checks to see if the master NLM item is present in the
//  global alert targets list.
//
//  Input:  nothing
//
//  Output: a value of TRUE if the master NLM item is found, FALSE otherwise
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

BOOL    MasterNLMItemInTargetsList(void)
{
    auto    BOOL            masterFound = FALSE;
    auto    CNLMTarget*     recPtr;
    auto    CString         tempStr;

    gAlertOptions.ResetPosition();
    while (recPtr = (CNLMTarget*)gAlertOptions.GetNextTarget(TARGET_NLM))
        {
        tempStr = recPtr->GetAddress();
        if  (MASTER_NLM_ITEM_STR == tempStr)
            {
            masterFound = TRUE;
            }
        }

    return (masterFound);

}  // end of "MasterNLMItemInTargetsList"










// -=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-
// -=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-
// -=*=-=*=-=*=-=*=-=*=-=*=- EXPORTED FUNCTIONS =*=-=*=-=*=-=*=-=*=-=*=-=*=-
// -=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-
// -=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-=*=-




// === CreateAlertOptsDlg ===================================================
//
//  This function is exported so that a non-MFC application (ie, NAV) can
//  use this DLLs dialogs in the Norton Control Panel in Quake (see the file
//  "alertui.txt" for further information).  It basically sets everything
//  into motion.
//
//  Input:  nothing
//  Output: ???
//
// ==========================================================================
//  Function created: 11/96, SEDWARD
// ==========================================================================

DllExport	LPVOID    CreateAlertOptsDlg(LPNAVOPTIONS  lpNavOptions)
{
//    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	SetResourceHandleToThisModule();
	 
    auto    BOOL        bResult = TRUE;

    // save a pointer to the NAV options structure
    glpNavOptions = lpNavOptions;

    // create the alert options dialog
    gAlertOptsDlgPtr = new CAlertOptsDlg;
    ASSERT(gAlertOptsDlgPtr);
    
    return gAlertOptsDlgPtr;

}  // end of "CreateAlertOptsDlg"




// === DestroyAlertOptsDlg ==================================================
//
//  This function is exported so that a non-MFC application (ie, NAV) can
//  cleanup after using this DLLs dialogs.
//
//  Input:  nothing
//  Output: ???
//
// ==========================================================================
//  Function created: 11/96, SEDWARD
// ==========================================================================

DllExport   BOOL    DestroyAlertOptsDlg(void)
{
//    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    auto    BOOL        bResult = TRUE;

    // destroy the alert options dialog
    delete  gAlertOptsDlgPtr;
    return (bResult);

}  // end of "DestroyAlertOptsDlg"



// === AlertOptsOK ==========================================================
//
//  Input:  nothing
//  Output: ???
//
// ==========================================================================
//  Function created: 11/96, SEDWARD
// ==========================================================================

DllExport   BOOL    AlertOptsOK(void)
{
//    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    auto    BOOL            result = TRUE;

    // save all alert option changes
    gAlertOptions.SaveOptions();

    return (result);

}  // end of "AlertOptsOK"



// === GetAlertOptDlgHwnd ===================================================
//
//  This exported function returns the HWND of the alert options dialog.
//
//  Input:  nothing
//  Output: the HWND of the alert options dialog
//
// ==========================================================================
//  Function created: 11/96, SEDWARD
// ==========================================================================

DllExport   HWND    GetAlertOptDlgHwnd(void)
{
//    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    auto    HWND        dlgHwnd;
    if (NULL == gAlertOptsDlgPtr)
        {
        dlgHwnd = NULL;
        }
    else
        {
        dlgHwnd = gAlertOptsDlgPtr->m_hWnd;
        }

    return (dlgHwnd);

}  // end of "GetAlertOptDlgHwnd"



// === InitControlsEx =======================================================
//
//  This exported function is used to initialize the dialog's controls
//  (mainly to support password protection).
//
//  Input:  nothing
//  Output: nothing
//
// ==========================================================================
//  Function created: 1/97, SEDWARD
// ==========================================================================

DllExport   void    InitControlsEx(void)
{
//    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    gAlertOptsDlgPtr->InitControls();

}  // end of "InitControlsEx"

void __declspec( dllexport ) InitAlertExtDll(void)
{

	// Insert this DLL into the resource chain
	// NOTE: If this Extension DLL is being implicitly linked to by
	//	an MFC Regular DLL (such as an ActiveX Control)
	//	instead of an MFC application, then you will want to
	//	remove this line from DllMain and put it in a separate
	//	function exported from this Extension DLL.	The Regular DLL
	//	that uses this Extension DLL should then explicitly call that
	//	function to initialize this Extension DLL.	Otherwise,
	//	the CDynLinkLibrary object will not be attached to the
	//	Regular DLL's resource chain, and serious problems will
	//	result.
 	TRACE0("ALERT.DLL InitExtensionDLL()!\n");

	new CDynLinkLibrary(EXTENSIONDLL);

}
// === SaveAlertOptions =======================================================
//
//  This exported function is used to save off the alert options object
//
//  Input:  nothing
//  Output: nothing
//
// ==========================================================================
//  Function created: 5/97, DBUCHES
// ==========================================================================
void __declspec( dllexport ) SaveAlertOptions(void)
{
    // save all alert option changes
    gAlertOptions.SaveOptions();
}
