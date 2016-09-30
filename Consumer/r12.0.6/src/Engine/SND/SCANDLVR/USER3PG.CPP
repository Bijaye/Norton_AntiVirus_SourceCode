//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
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


/////////////////////////////////////////////////////////////////////////////
// CUserInfo3PropertyPage property page

IMPLEMENT_DYNCREATE(CUserInfo3PropertyPage, CWizardPropertyPage)

CUserInfo3PropertyPage::CUserInfo3PropertyPage()
: CWizardPropertyPage(CUserInfo3PropertyPage::IDD, IDS_CAPTION_USER_INFORMATION)
{
	//{{AFX_DATA_INIT(CUserInfo3PropertyPage)
	m_szAdditionalSymptoms = _T("");
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
	DDX_Text(pDX, IDC_EDIT_SYMPTOMS, m_szAdditionalSymptoms);
	DDX_CBString(pDX, IDC_COMBO_COUNTRY, m_szCountryOfInfection);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserInfo3PropertyPage, CWizardPropertyPage)
	//{{AFX_MSG_MAP(CUserInfo3PropertyPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUserInfo3PropertyPage message handlers

//  Function created: 3/98, SEDWARD
BOOL CUserInfo3PropertyPage::UpdateConfigData(void)
{
	DWORD dwResult = 0;

	// update the member variables with current field contents
	this->UpdateData(TRUE);

	// Format the symptoms string
	CString cszSymptoms;
	FormatSymptomsString(cszSymptoms);

	// Always attempt to strip file content
	g_ConfigInfo.SetStripFileContent(TRUE);
	g_ConfigInfo.SetSymptoms(cszSymptoms);
	g_ConfigInfo.SetAddlSymptoms(m_szAdditionalSymptoms);
    g_ConfigInfo.SetCountryOfInfection(m_szCountryOfInfection);

	// write the current configuration data to disk
	g_ConfigInfo.WriteConfigFile(&dwResult);

	// update the data code associated with the currently selected country
	// (this is the value actually queried by a client of the S&D configuration
	// object, it's not written to or read from the DAT file)
	UpdateInfectionCountryCode();

	return (TRUE);

}  // end of "CUserInfo3PropertyPage::UpdateConfigData"


//  Function created: 12/10, APIERCE
BOOL CUserInfo3PropertyPage::FormatSymptomsString(CString &cszSymptoms)
{
	cszSymptoms.Empty();
	CButton *pCheckBox = NULL;

	// Process Symptom 01
	pCheckBox = (CButton*)GetDlgItem(IDC_CHK_SYMPTOM01);
	if(pCheckBox && (BST_CHECKED == pCheckBox->GetCheck()))
	{
		cszSymptoms += _T("(SYMPTOM01) Popups (only when browser running). ");
	}

	// Process Symptom 02
	pCheckBox = (CButton*)GetDlgItem(IDC_CHK_SYMPTOM02);
	if(pCheckBox && (BST_CHECKED == pCheckBox->GetCheck()))
	{
		cszSymptoms += _T("(SYMPTOM02) Popups (even when browser is not running). ");
	}

	// Process Symptom 03
	pCheckBox = (CButton*)GetDlgItem(IDC_CHK_SYMPTOM03);
	if(pCheckBox && (BST_CHECKED == pCheckBox->GetCheck()))
	{
		cszSymptoms += _T("(SYMPTOM03) Slow system performance. ");
	}

	// Process Symptom 04
	pCheckBox = (CButton*)GetDlgItem(IDC_CHK_SYMPTOM04);
	if(pCheckBox && (BST_CHECKED == pCheckBox->GetCheck()))
	{
		cszSymptoms += _T("(SYMPTOM04) Unwanted browser home page modification. ");
	}

	// Process Symptom 05
	pCheckBox = (CButton*)GetDlgItem(IDC_CHK_SYMPTOM05);
	if(pCheckBox && (BST_CHECKED == pCheckBox->GetCheck()))
	{
		cszSymptoms += _T("(SYMPTOM05) Unwanted buttons or toolbars added to browser. ");
	}

	// Process Symptom 06
	pCheckBox = (CButton*)GetDlgItem(IDC_CHK_SYMPTOM06);
	if(pCheckBox && (BST_CHECKED == pCheckBox->GetCheck()))
	{
		cszSymptoms += _T("(SYMPTOM06) Unwanted applications added to desktop. ");
	}

	// Process Symptom 07
	pCheckBox = (CButton*)GetDlgItem(IDC_CHK_SYMPTOM07);
	if(pCheckBox && (BST_CHECKED == pCheckBox->GetCheck()))
	{
		cszSymptoms += _T("(SYMPTOM07) Browser goes to unwanted web pages. ");
	}

	// Process Symptom 08
	pCheckBox = (CButton*)GetDlgItem(IDC_CHK_SYMPTOM08);
	if(pCheckBox && (BST_CHECKED == pCheckBox->GetCheck()))
	{
		cszSymptoms += _T("(SYMPTOM08) Existing applications no longer working. ");
	}

	return TRUE;
}


//  Function created: 3/98, SEDWARD
LRESULT CUserInfo3PropertyPage::OnWizardBack(void)
{
	UpdateConfigData();
	return CWizardPropertyPage::OnWizardBack();
}


//  Function created: 3/98, SEDWARD
LRESULT CUserInfo3PropertyPage::OnWizardNext(void)
{
	UpdateConfigData();
	return CWizardPropertyPage::OnWizardNext();
}


//  Function created: 3/98, SEDWARD
BOOL CUserInfo3PropertyPage::OnInitDialog(void)
{
	CWizardPropertyPage::OnInitDialog();

	// limit the length in the edit controls...
	CEdit* pEditCtrl;

	// symptoms
	pEditCtrl = (CEdit*)GetDlgItem(IDC_EDIT_SYMPTOMS);
	if(NULL != pEditCtrl)
	{
		pEditCtrl->SetLimitText(MAX_ADDL_SYMPTOMS_LEN);
	}

	// country of infection
	pEditCtrl = (CEdit*)GetDlgItem(IDC_COMBO_COUNTRY);
	if(NULL != pEditCtrl)
	{
		pEditCtrl->SetLimitText(MAX_COUNTRY_LEN);
	}

	// initialize the country combo box
	CComboBox*      pCountryCombo;
	pCountryCombo = (CComboBox*)GetDlgItem(IDC_COMBO_COUNTRY);
	if(NULL != pCountryCombo)
	{
        InitCountryListbox(*pCountryCombo);
	}

	// initialize this dialog's member variables
	m_szAdditionalSymptoms.Empty();

	g_ConfigInfo.GetCountryOfInfection(m_szCountryOfInfection.GetBuffer(MAX_COUNTRY_LEN)
		, MAX_COUNTRY_LEN);
	m_szCountryOfInfection.ReleaseBuffer();

	// If the country is empty attempt to set it automatically
	if(m_szCountryOfInfection.IsEmpty())
	{
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SCOUNTRY, m_szCountryOfInfection.GetBuffer(MAX_COUNTRY_LEN), MAX_COUNTRY_LEN);
		m_szCountryOfInfection.ReleaseBuffer();
	}

	// Set focus to the first item in the page
	GetDlgItem(IDC_CHK_SYMPTOM01)->SetFocus();

	// update field contents with the member variables
	this->UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CUserInfo3PropertyPage::OnInitDialog"


// Updates the global ScanCfg object with the country data value associated 
// with the currently selected country.
// Output: nothing (the user country member of the global ScanCfg object
//         is updated with either the derived data or
//         INVALID_COUNTRY_DATA_CODE)
// Function created: 5/98, SEDWARD
void CUserInfo3PropertyPage::UpdateInfectionCountryCode()
{
    DWORD dwCountryValue = INVALID_COUNTRY_DATA_CODE;

	// get the data value from the currently selected country
    CComboBox* pCountryCombo = (CComboBox*)GetDlgItem(IDC_COMBO_COUNTRY);
	if(NULL != pCountryCombo)
	{
		dwCountryValue = pCountryCombo->GetItemData(pCountryCombo->GetCurSel());
		if(CB_ERR == dwCountryValue)
		{
			dwCountryValue = INVALID_COUNTRY_DATA_CODE;
		}
	}

    // update the global ScanCfg object
	g_ConfigInfo.SetInfectionCountryCode(dwCountryValue);

}  // end of "CUserInfo3PropertyPage::UpdateInfectionCountryCode"
