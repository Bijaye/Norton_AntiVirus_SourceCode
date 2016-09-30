// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// Norton_AntiVirus\Corporate_Edition\Win32\src\AVCore\scandlvr\src\User4PG.CPP : implementation file
//

#include "stdafx.h"
#include "Global.h"
#include "ScanDeliverDLL.h"
#include "scandlvr.h"
#include "ScanWiz.h"
#include "User4PG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CUserInfo4PropertyPage dialog

IMPLEMENT_DYNAMIC(CUserInfo4PropertyPage, CWizardPropertyPage)
CUserInfo4PropertyPage::CUserInfo4PropertyPage()
	: CWizardPropertyPage(CUserInfo4PropertyPage::IDD, IDS_CAPTION_USER_INFORMATION)
{
	m_bPopupsWithBrowser = m_bPopupsWithOutBrowser = m_bSlowSysPerf = m_bBrowserHmPgMod = m_bUnwantedBtTbAdded2Browser = m_bUnwantedPgVisits = m_bAppsDoNotWork = m_bUnwantedAppsAdded2Dsktop = FALSE; 	
}

CUserInfo4PropertyPage::~CUserInfo4PropertyPage()
{
}

void CUserInfo4PropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_POPWBROWSER, m_bPopupsWithBrowser);
	DDX_Check(pDX, IDC_POP_WO_BROWSER, m_bPopupsWithOutBrowser);
	DDX_Check(pDX, IDC_SLOW_SYS_PERF, m_bSlowSysPerf);
	DDX_Check(pDX, IDC_BROWSER_HM_PG_MOD, m_bBrowserHmPgMod);
	DDX_Check(pDX, IDC_UNWAN_BUT_TBARS_BROW, m_bUnwantedBtTbAdded2Browser);
	DDX_Check(pDX, IDC_BROWSER_UNWAN_WEBPG, m_bUnwantedPgVisits);
	DDX_Check(pDX, IDC_APPS_NOT_WORK, m_bAppsDoNotWork);
	DDX_Check(pDX, IDC_UNWANTED_APPS_ADDED_DSKTOP, m_bUnwantedAppsAdded2Dsktop);
}


BEGIN_MESSAGE_MAP(CUserInfo4PropertyPage, CDialog)
END_MESSAGE_MAP()


// CUserInfo4PropertyPage message handlers

// ==== OnInitDialog ======================================================
BOOL CUserInfo4PropertyPage::OnInitDialog(void)
{
    CWizardPropertyPage::OnInitDialog();

    // TODO: Add extra initialization here

    // initialize this dialog's member variables
	m_bPopupsWithBrowser = g_ConfigInfo.GetPopupsWtBrow();
	m_bPopupsWithOutBrowser = g_ConfigInfo.GetPopupsWithOutBrowser();
	m_bSlowSysPerf = g_ConfigInfo.GetSlowSysPerf();
	m_bBrowserHmPgMod = g_ConfigInfo.GetBrowserHmPgMod();
	m_bUnwantedBtTbAdded2Browser = g_ConfigInfo.GetUnwantedBtTbAdded2Browser();
	m_bUnwantedPgVisits = g_ConfigInfo.GetSlowSysPerf();
	m_bAppsDoNotWork = g_ConfigInfo.GetAppsDoNotWork();
	m_bUnwantedAppsAdded2Dsktop = g_ConfigInfo.GetUnwatedAppsAdded2Dsktop();
    // update field contents with the member variables
    this->UpdateData(FALSE);


    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CUserInfo4PropertyPage::OnInitDialog"

// ==== UpdateConfigData ==================================================
//
//
BOOL CUserInfo4PropertyPage::UpdateConfigData(void)
{
    auto    DWORD       dwResult = 0;

    // update the member variables with current field contents
    this->UpdateData(TRUE);

    // update the global configuration data object
	g_ConfigInfo.SetPopupsWtBrow(m_bPopupsWithBrowser);
	g_ConfigInfo.SetPopupsWithOutBrowser(m_bPopupsWithOutBrowser );
	g_ConfigInfo.SetSlowSysPerf(m_bSlowSysPerf );
	g_ConfigInfo.SetBrowserHmPgMod(m_bBrowserHmPgMod );
	g_ConfigInfo.SetUnwantedBtTbAdded2Browser(m_bUnwantedBtTbAdded2Browser );
	g_ConfigInfo.SetUnwantedPgVisits(m_bUnwantedPgVisits );
	g_ConfigInfo.SetAppsDoNotWork(m_bAppsDoNotWork );
	g_ConfigInfo.SetUnwatedAppsAdded2Dsktop(m_bUnwantedAppsAdded2Dsktop);
    // write the current configuration data to disk
    g_ConfigInfo.WriteConfigFile(&dwResult);

    return (TRUE);

}  // end of "CUserInfo4PropertyPage::UpdateConfigData"

// ==== OnWizardNext ======================================================
//
//
LRESULT CUserInfo4PropertyPage::OnWizardNext(void)
{
    // TODO: Add your specialized code here and/or call the base class

    UpdateConfigData();    
    
    return CWizardPropertyPage::OnWizardNext();

}  // end of "CUserInfo4PropertyPage::OnWizardNext"



// ==== OnWizardBack ======================================================
//
//
LRESULT CUserInfo4PropertyPage::OnWizardBack(void)
{
    // TODO: Add your specialized code here and/or call the base class

    UpdateConfigData();
    return CWizardPropertyPage::OnWizardBack();

}  // end of "CUserInfo4PropertyPage::OnWizardBack"
