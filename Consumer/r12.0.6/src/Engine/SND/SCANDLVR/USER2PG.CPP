//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// User2Pg.cpp: implementation of the CUserInfo2PropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/User2Pg.cpv  $
// 
//    Rev 1.5   01 Jun 1998 17:06:04   SEDWARD
// Added support for the country drop-down listbox.
//
//    Rev 1.4   26 May 1998 18:27:56   SEDWARD
// Put 'm_' in front of the bitmap member variables.
//
//    Rev 1.3   25 May 1998 13:50:08   SEDWARD
// Set bitmap IDs and caption in constructor.
//
//    Rev 1.2   27 Apr 1998 18:12:14   SEDWARD
// Removed needless calls to CString::GetBuffer for ScanCfg "Set" calls.
//
//    Rev 1.1   27 Apr 1998 02:08:14   SEDWARD
// Some cleanup.
//
//    Rev 1.0   20 Apr 1998 16:26:00   SEDWARD
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
#include "scandlvr.h"
#include "ScanWiz.h"
#include "User2Pg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CUserInfo2PropertyPage property page

IMPLEMENT_DYNCREATE(CUserInfo2PropertyPage, CWizardPropertyPage)

CUserInfo2PropertyPage::CUserInfo2PropertyPage()
: CWizardPropertyPage(CUserInfo2PropertyPage::IDD, IDS_CAPTION_USER_INFORMATION)
{
	//{{AFX_DATA_INIT(CUserInfo2PropertyPage)
	m_szCountry = _T("");
	m_szAddress1 = _T("");
	m_szAddress2 = _T("");
	m_szAtlasNumber = _T("");
	m_szCity = _T("");
	m_szState = _T("");
	m_szZipCode = _T("");
	//}}AFX_DATA_INIT

	m_bitmap16 = IDB_SDBMP_INFO_16;
	m_bitmap256 = IDB_SDBMP_INFO_256;
}

CUserInfo2PropertyPage::~CUserInfo2PropertyPage()
{
}


void CUserInfo2PropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserInfo2PropertyPage)
	DDX_CBString(pDX, IDC_COMBO_COUNTRY, m_szCountry);
	DDX_Text(pDX, IDC_EDIT_ADDRESS1, m_szAddress1);
	DDX_Text(pDX, IDC_EDIT_ADDRESS2, m_szAddress2);
	DDX_Text(pDX, IDC_EDIT_ATLAS_NUMBER, m_szAtlasNumber);
	DDX_Text(pDX, IDC_EDIT_CITY, m_szCity);
	DDX_Text(pDX, IDC_EDIT_STATE_PROVINCE, m_szState);
	DDX_Text(pDX, IDC_EDIT_ZIP_CODE, m_szZipCode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserInfo2PropertyPage, CWizardPropertyPage)
	//{{AFX_MSG_MAP(CUserInfo2PropertyPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUserInfo2PropertyPage message handlers

//  Function created: 3/98, SEDWARD
BOOL CUserInfo2PropertyPage::OnInitDialog(void)
{
	CWizardPropertyPage::OnInitDialog();

	// limit the length in the edit controls.
	CEdit* pEditCtrl;

	// address1
	pEditCtrl = (CEdit*)GetDlgItem(IDC_EDIT_ADDRESS1);
	if(NULL != pEditCtrl)
	{
		pEditCtrl->SetLimitText(MAX_ADDRESS_LEN);
	}

	// address2
	pEditCtrl = (CEdit*)GetDlgItem(IDC_EDIT_ADDRESS2);
	if(NULL != pEditCtrl)
	{
		pEditCtrl->SetLimitText(MAX_ADDRESS_LEN);
	}

	// city
	pEditCtrl = (CEdit*)GetDlgItem(IDC_EDIT_CITY);
	if(NULL != pEditCtrl)
	{
		pEditCtrl->SetLimitText(MAX_CITY_LEN);
	}

	// state
	pEditCtrl = (CEdit*)GetDlgItem(IDC_EDIT_STATE_PROVINCE);
	if(NULL != pEditCtrl)
	{
		pEditCtrl->SetLimitText(MAX_STATE_LEN);
	}

	// zip code
	pEditCtrl = (CEdit*)GetDlgItem(IDC_EDIT_ZIP_CODE);
	if(NULL != pEditCtrl)
	{
		pEditCtrl->SetLimitText(MAX_ZIPCODE_LEN);
	}

	// country
	pEditCtrl = (CEdit*)GetDlgItem(IDC_COMBO_COUNTRY);
	if(NULL != pEditCtrl)
	{
		pEditCtrl->SetLimitText(MAX_COUNTRY_LEN);
	}

	// atlas number
	pEditCtrl = (CEdit*)GetDlgItem(IDC_EDIT_ATLAS_NUMBER);
	if(NULL != pEditCtrl)
	{
		pEditCtrl->SetLimitText(MAX_ATLAS_NUMBER_LEN);
	}

	// initialize the country combo box
	CComboBox* pCountryCombo;
	pCountryCombo = (CComboBox*)GetDlgItem(IDC_COMBO_COUNTRY);
	if(NULL != pCountryCombo)
	{
        InitCountryListbox(*pCountryCombo);
	}

	// initialize this dialog's member variables
	g_ConfigInfo.GetCountry(m_szCountry.GetBuffer(MAX_COUNTRY_LEN), MAX_COUNTRY_LEN);
	m_szCountry.ReleaseBuffer();

	// If the country is empty attempt to set it automatically
	if(m_szCountry.IsEmpty())
	{
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SCOUNTRY, m_szCountry.GetBuffer(MAX_COUNTRY_LEN), MAX_COUNTRY_LEN);
		m_szCountry.ReleaseBuffer();
	}

	g_ConfigInfo.GetAddress1(m_szAddress1.GetBuffer(MAX_ADDRESS_LEN), MAX_ADDRESS_LEN);
	m_szAddress1.ReleaseBuffer();
	g_ConfigInfo.GetAddress2(m_szAddress2.GetBuffer(MAX_ADDRESS_LEN), MAX_ADDRESS_LEN);
	m_szAddress2.ReleaseBuffer();
	g_ConfigInfo.GetAtlasNumber(m_szAtlasNumber.GetBuffer(MAX_ATLAS_NUMBER_LEN)
		, MAX_ATLAS_NUMBER_LEN);
	m_szAtlasNumber.ReleaseBuffer();
	g_ConfigInfo.GetCity(m_szCity.GetBuffer(MAX_CITY_LEN), MAX_CITY_LEN);
	m_szCity.ReleaseBuffer();
	g_ConfigInfo.GetState(m_szState.GetBuffer(MAX_STATE_LEN), MAX_STATE_LEN);
	m_szState.ReleaseBuffer();
	g_ConfigInfo.GetZipCode(m_szZipCode.GetBuffer(MAX_ZIPCODE_LEN), MAX_ZIPCODE_LEN);
	m_szZipCode.ReleaseBuffer();

	// update field contents with the member variables
	this->UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CUserInfo2PropertyPage::OnInitDialog"


//  Function created: 3/98, SEDWARD
BOOL CUserInfo2PropertyPage::UpdateConfigData(void)
{
	DWORD dwResult = 0;

	// update the member variables with current field contents
	this->UpdateData(TRUE);

	// update the global configuration data object
    g_ConfigInfo.SetCountry(m_szCountry);
	g_ConfigInfo.SetAddress1(m_szAddress1);
	g_ConfigInfo.SetAddress2(m_szAddress2);
	g_ConfigInfo.SetAtlasNumber(m_szAtlasNumber);
	g_ConfigInfo.SetCity(m_szCity);
	g_ConfigInfo.SetState(m_szState);
	g_ConfigInfo.SetZipCode(m_szZipCode);

	// write the current configuration data to disk
	g_ConfigInfo.WriteConfigFile(&dwResult);

    // update the data code associated with the currently selected country
    // (this is the value actually queried by a client of the S&D configuration
    // object, it's not written to or read from the DAT file)
    UpdateUserCountryCode();

	return (TRUE);
}  // end of "CUserInfo2PropertyPage::UpdateConfigData"


//  Function created: 3/98, SEDWARD
LRESULT CUserInfo2PropertyPage::OnWizardNext(void)
{
	UpdateConfigData();
	return CWizardPropertyPage::OnWizardNext();
}


//  Function created: 3/98, SEDWARD
LRESULT CUserInfo2PropertyPage::OnWizardBack(void)
{
	UpdateConfigData();
	return CWizardPropertyPage::OnWizardBack();
}


// Updates the global ScanCfg object with the country data value associated 
// with the currently selected country.
// Output: nothing (the user country member of the global ScanCfg object
//         is updated with either the derived data or
//         INVALID_COUNTRY_DATA_CODE)
// Function created: 5/98, SEDWARD
void    CUserInfo2PropertyPage::UpdateUserCountryCode(void)
{
	DWORD      dwCountryValue = INVALID_COUNTRY_DATA_CODE;

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
	g_ConfigInfo.SetUserCountryCode(dwCountryValue);

}  // end of "CUserInfo2PropertyPage::UpdateUserCountryCode"
