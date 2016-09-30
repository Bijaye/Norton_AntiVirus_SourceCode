// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/User3Pg.cpv   1.9   15 Jun 1998 18:02:22   sedward  $
/////////////////////////////////////////////////////////////////////////////
//
// User3Pg.cpp: implementation of the CUserInfo3PropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/User3Pg.cpv  $
// 
//    Rev 1.9   15 Jun 1998 18:02:22   sedward
// The static text control associated with the custom SMTP server edit control
// is now enabled or disabled based on the state of the 'use custom SMTP server'
// checkbox.
//
//    Rev 1.8   01 Jun 1998 17:06:16   SEDWARD
// Added support for the country drop-down listbox.
//
//    Rev 1.7   28 May 1998 14:00:44   SEDWARD
// Added support for independent handling of the custom SMTP server checkbox.
//
//    Rev 1.6   26 May 1998 18:27:34   SEDWARD
// Put 'm_' in front of the bitmap member variables.
//
//    Rev 1.5   25 May 1998 13:51:18   SEDWARD
// Set bitmap IDs and caption in constructor, call WizMessageBox instead of
// AfxMessageBox.
//
//    Rev 1.4   19 May 1998 19:45:34   SEDWARD
// Added code so a message appears if the user selects the custom SMTP server
// checkbox but leaves the corresponding field blank.
//
//    Rev 1.3   19 May 1998 19:07:30   SEDWARD
// Removed 'UpdateData' call in 'OnCheckCustomServer'; this was causing the
// other controls to be cleared.
//
//    Rev 1.2   27 Apr 1998 18:12:16   SEDWARD
// Removed needless calls to CString::GetBuffer for ScanCfg "Set" calls.
//
//    Rev 1.1   24 Apr 1998 18:22:40   SEDWARD
// Added support for "strip content" radio buttons and custom SMTP server.
//
//    Rev 1.0   20 Apr 1998 16:26:02   SEDWARD
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
#include "ScanDeliverDLL.h"
#include "scandlvr.h"
#include "ScanWiz.h"
#include "User3Pg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL g_bCQAnomaly;
/////////////////////////////////////////////////////////////////////////////
// CUserInfo3PropertyPage property page

IMPLEMENT_DYNCREATE(CUserInfo3PropertyPage, CWizardPropertyPage)

CUserInfo3PropertyPage::CUserInfo3PropertyPage()
    : CWizardPropertyPage(CUserInfo3PropertyPage::IDD, IDS_CAPTION_USER_INFORMATION)
{
    //{{AFX_DATA_INIT(CUserInfo3PropertyPage)
    m_szSmtpServer = _T("");
    m_checkboxCustomSmtpServer = FALSE;
    m_szSymptoms = _T("");
    m_szCountryOfInfection = _T("");
	//}}AFX_DATA_INIT

    m_bitmap16 = IDB_SDBMP_INFO_16;
    m_bitmap256 = IDB_SDBMP_INFO_256;
}

CUserInfo3PropertyPage::~CUserInfo3PropertyPage()
{
}

void CUserInfo3PropertyPage::DoDataExchange(CDataExchange* pDX)
{
    CWizardPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CUserInfo3PropertyPage)
    DDX_Text(pDX, IDC_EDIT_SMTP_SERVER, m_szSmtpServer);
    DDX_Check(pDX, IDC_CHECK_CUSTOM_SERVER, m_checkboxCustomSmtpServer);
    DDX_Text(pDX, IDC_EDIT_SYMPTOMS, m_szSymptoms);
    DDX_CBString(pDX, IDC_COMBO_COUNTRY, m_szCountryOfInfection);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserInfo3PropertyPage, CWizardPropertyPage)
    //{{AFX_MSG_MAP(CUserInfo3PropertyPage)
	ON_BN_CLICKED(IDC_CHECK_CUSTOM_SERVER, OnCheckCustomServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserInfo3PropertyPage message handlers



// ==== UpdateConfigData ==================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL CUserInfo3PropertyPage::UpdateConfigData(void)
{
    auto    DWORD       dwResult = 0;

    // update the member variables with current field contents
    this->UpdateData(TRUE);

    // update the global configuration data object
    GetSmtpServerFromUI();
    g_ConfigInfo.SetStripFileContent(GetStripFileContentRadioButtonValue());
    g_ConfigInfo.SetSymptoms(m_szSymptoms);
    g_ConfigInfo.SetCountryOfInfection(m_szCountryOfInfection);
    g_ConfigInfo.SetUseCustomSmtpServer(m_checkboxCustomSmtpServer);

    // write the current configuration data to disk
    g_ConfigInfo.WriteConfigFile(&dwResult);

    // update the data code associated with the currently selected country
    // (this is the value actually queried by a client of the S&D configuration
    // object, it's not written to or read from the DAT file)
    UpdateInfectionCountryCode();

    return (TRUE);

}  // end of "CUserInfo3PropertyPage::UpdateConfigData"



// ==== OnWizardBack ======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

LRESULT CUserInfo3PropertyPage::OnWizardBack(void)
{
    // TODO: Add your specialized code here and/or call the base class

    UpdateConfigData();
    return CWizardPropertyPage::OnWizardBack();

}  // end of "CUserInfo3PropertyPage::OnWizardBack"



// ==== OnWizardNext ======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

LRESULT CUserInfo3PropertyPage::OnWizardNext(void)
{
    // TODO: Add your specialized code here and/or call the base class

    UpdateConfigData();

    // if any of the required fields is empty, alert the user and return back to
    // this page
    if (FALSE == RequiredFieldsFilled())
        {
        auto    CScanDeliverWizard*         pWizSheet;
        auto    int                         nIndex;

        // display a message to the user
        WizMessageBox(IDS_EMPTY_CUSTOM_SMTP_SERVER, IDS_CAPTION_STANDARD_MESGBOX
                                                                , MB_ICONERROR);

        // get a pointer to the parent window (the property sheet)
        pWizSheet = (CScanDeliverWizard*)this->GetParent();
        if (NULL != pWizSheet)
            {
            nIndex = pWizSheet->GetIndexFromResID(IDD_USER_INFO3);
            if (nIndex > 0)
                {
                --nIndex;
                pWizSheet->SetActivePage(nIndex);
                }
            }
        }

    return CWizardPropertyPage::OnWizardNext();

}  // end of "CUserInfo3PropertyPage::OnWizardNext"



// ==== OnInitDialog ======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL CUserInfo3PropertyPage::OnInitDialog(void)
{
    CWizardPropertyPage::OnInitDialog();

    // TODO: Add extra initialization here

    // limit the length in the edit controls...
    auto    CEdit*      pEditCtrl;

    // symptoms
    pEditCtrl = (CEdit*)GetDlgItem(IDC_EDIT_SYMPTOMS);
    if (pEditCtrl)
        {
        pEditCtrl->SetLimitText(MAX_SYMPTOMS_LEN);
        }

    // SMTP server
    pEditCtrl = (CEdit*)GetDlgItem(IDC_EDIT_SMTP_SERVER);
    if (pEditCtrl)
        {
        pEditCtrl->SetLimitText(MAX_SMTP_SERVER_LEN);
        }

    // country of infection
    pEditCtrl = (CEdit*)GetDlgItem(IDC_COMBO_COUNTRY);
    if (pEditCtrl)
        {
        pEditCtrl->SetLimitText(MAX_COUNTRY_LEN);
        }

    // initialize the country combo box
    auto    CComboBox*      pCountryCombo;
    pCountryCombo = (CComboBox*)GetDlgItem(IDC_COMBO_COUNTRY);
    if (NULL != pCountryCombo)
        {
        InitCountryListbox(*pCountryCombo);
        }

    // initialize this dialog's member variables
    g_ConfigInfo.GetSymptoms(m_szSymptoms.GetBuffer(MAX_SYMPTOMS_LEN), MAX_SYMPTOMS_LEN);
    m_szSymptoms.ReleaseBuffer();
    InitializeSmtpServerControls();
    g_ConfigInfo.GetCountryOfInfection(m_szCountryOfInfection.GetBuffer(MAX_COUNTRY_LEN)
                                                                    , MAX_COUNTRY_LEN);
    m_szCountryOfInfection.ReleaseBuffer();

    // strip file content
    InitializeStripFileContentRadioButtons();

    // update field contents with the member variables
    this->UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CUserInfo3PropertyPage::OnInitDialog"



// ==== OnCheckCustomServer ===============================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

void CUserInfo3PropertyPage::OnCheckCustomServer(void)
{
	// TODO: Add your control notification handler code here

    // get a pointer to the edit control

    auto    CEdit*          ptrSmtpServerEditCtrl;
    auto    CStatic*        ptrSmtpServerStaticCtrl;

    // get pointers to our controls
    ptrSmtpServerEditCtrl = (CEdit*)GetDlgItem(IDC_EDIT_SMTP_SERVER);
    ptrSmtpServerStaticCtrl = (CStatic*)GetDlgItem(IDC_STATIC_SMTP_SERVER);
    if ((NULL == ptrSmtpServerEditCtrl)  ||  (NULL == ptrSmtpServerStaticCtrl))
        {
        return;
        }

    // toggle the checkbox
    m_checkboxCustomSmtpServer = !m_checkboxCustomSmtpServer;
    ptrSmtpServerEditCtrl->EnableWindow(m_checkboxCustomSmtpServer);
    ptrSmtpServerStaticCtrl->EnableWindow(m_checkboxCustomSmtpServer);

}  // end of "CUserInfo3PropertyPage::OnCheckCustomServer"



// ==== InitializeStripFileContentRadioButtons ============================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

void CUserInfo3PropertyPage::InitializeStripFileContentRadioButtons(void)
{
    auto    CButton*        ptrNo;
    auto    CButton*        ptrYes;

    // get pointers to the radio buttons
    ptrNo = (CButton*)GetDlgItem(IDC_RADIO_STRIP_FILE_CONTENT_NO);
    ptrYes = (CButton*)GetDlgItem(IDC_RADIO_STRIP_FILE_CONTENT_YES);

	if(g_bCQAnomaly)	//For Central Quarantine Anomaly Items
	{
		//Disable the cotrols for StripFile Content
		CStatic* pStripContent = (CStatic*)GetDlgItem(IDC_STATIC_STRIP_FILE_CONTENT);
		if(pStripContent)
			pStripContent->EnableWindow(FALSE);
		if(ptrNo)
			ptrNo->EnableWindow(FALSE);
		if(ptrYes)
		ptrYes->EnableWindow(FALSE);
	}
	else
	{
		if (TRUE == g_ConfigInfo.GetStripFileContent())
			{
			if (ptrNo)
				{
				ptrNo->SetCheck(FALSE);
				}

			if (ptrYes)
				{
				ptrYes->SetCheck(TRUE);
				}
			}
		else
			{
			if (ptrNo)
				{
				ptrNo->SetCheck(TRUE);
				}

			if (ptrYes)
				{
				ptrYes->SetCheck(FALSE);
				}
			}
	}

}  // end of "CUserInfo3PropertyPage::InitializeStripFileContentRadioButtons"



// ==== GetStripFileContentRadioButtonValue ===============================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL CUserInfo3PropertyPage::GetStripFileContentRadioButtonValue(void)
{
    auto    BOOL            bResult = TRUE;
    auto    CButton*        ptrYes;

    // the buttons are mutually exclusive, so we only need to get a pointer to
    // one of the buttons (we choose "yes")
    ptrYes = (CButton*)GetDlgItem(IDC_RADIO_STRIP_FILE_CONTENT_YES);
    if (ptrYes)
        {
        bResult = ptrYes->GetCheck();
        }

    return (bResult);

}  // end of "CUserInfo3PropertyPage::GetStripFileContentRadioButtonValue"



// ==== GetSmtpServerFromUI ===============================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

void    CUserInfo3PropertyPage::GetSmtpServerFromUI(void)
{
    auto    CString     szServerName;
    auto    TCHAR*      szResultPtr;

    // if the "custom" checkbox is set, use the string currently in the edit
    // control; else, use the default
    if (m_checkboxCustomSmtpServer)
        {
        szResultPtr = m_szSmtpServer.GetBuffer(MAX_SMTP_SERVER_LEN);
        m_szSmtpServer.ReleaseBuffer();
        }
    else if (TRUE == szServerName.LoadString(IDS_DEFAULT_SMTP_SERVER))
        {
        szResultPtr = szServerName.GetBuffer(MAX_SMTP_SERVER_LEN);
        szServerName.ReleaseBuffer();
        }

    g_ConfigInfo.SetSmtpServer(szResultPtr);

}  // end of "CUserInfo3PropertyPage::GetSmtpServerFromUI"



// ==== InitializeSmtpServerControls ======================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

void CUserInfo3PropertyPage::InitializeSmtpServerControls(void)
{
    auto    CEdit*      ptrSmtpServerCtrl;
    auto    CStatic*    ptrSmtpServerStaticCtrl;
    auto    CString     szServerName;
    auto    TCHAR       szBuf[MAX_SMTP_SERVER_LEN];

    // get a pointer to the edit control
    ptrSmtpServerCtrl = (CEdit*)GetDlgItem(IDC_EDIT_SMTP_SERVER);

    // initialize the member variable with whatever is currently in the
    // configuration DAT file
    if (TRUE == g_ConfigInfo.GetSmtpServerDisplayString(szBuf, MAX_SMTP_SERVER_LEN))
        {
        m_szSmtpServer = szBuf;
        }

    // get the saved value for the "custom SMTP server" checkbox and enable
    // or disable the custom SMTP edit control
    m_checkboxCustomSmtpServer = g_ConfigInfo.GetUseCustomSmtpServer();
    if (NULL != ptrSmtpServerCtrl)
        {
        ptrSmtpServerCtrl->EnableWindow(m_checkboxCustomSmtpServer);
        }

    // update the static text associated with the custom server edit field
    ptrSmtpServerStaticCtrl = (CStatic*)GetDlgItem(IDC_STATIC_SMTP_SERVER);
    if (NULL != ptrSmtpServerStaticCtrl)
        {
        ptrSmtpServerStaticCtrl->EnableWindow(m_checkboxCustomSmtpServer);		
        }

    UpdateData(FALSE);  // FALSE means initialize the controls

}  // end of "CUserInfo3PropertyPage::InitializeSmtpServerControls"



// ==== RequiredFieldsFilled ==============================================
//
//  This function checks to see if any of the required fields are empty.
//
//  Input:  nothing
//  Output: TRUE if all required fields are non-empty, FALSE if not
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL    CUserInfo3PropertyPage::RequiredFieldsFilled(void)
{
    // if the custom SMTP server checkbox is checked, make sure that the
    // corresponding server string is not empty
    if  ((TRUE == m_checkboxCustomSmtpServer)
    &&  (FALSE != m_szSmtpServer.IsEmpty()))
        {
        return (FALSE);
        }

    return (TRUE);

}  // end of "CUserInfo3PropertyPage::RequiredFieldsFilled"


// ==== UpdateInfectionCountryCode ========================================
//
//  This function updates the global ScanCfg object with the country data
//  value associated with the currently selected country.
//
//  Input:  nothing
//
//  Output: nothing (the user country member of the global ScanCfg object
//          is updated with either the derived data or
//          INVALID_COUNTRY_DATA_CODE)
//
// ========================================================================
//  Function created: 5/98, SEDWARD
// ========================================================================

void    CUserInfo3PropertyPage::UpdateInfectionCountryCode(void)
{
    auto    CComboBox*      pCountryCombo;
    auto    DWORD           dwCountryValue = INVALID_COUNTRY_DATA_CODE;

    // get the data value from the currently selected country
    pCountryCombo = (CComboBox*)GetDlgItem(IDC_COMBO_COUNTRY);
    if (NULL != pCountryCombo)
        {
        dwCountryValue = pCountryCombo->GetItemData(pCountryCombo->GetCurSel());
        if (CB_ERR == dwCountryValue)
            {
            dwCountryValue = INVALID_COUNTRY_DATA_CODE;
            }
        }

    // update the global ScanCfg object
    g_ConfigInfo.SetInfectionCountryCode(dwCountryValue);

}  // end of "CUserInfo3PropertyPage::UpdateInfectionCountryCode"

