// OEHUIDlg.cpp: implementation of the COEHeurUIDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OEHeurUIDlg.h"
#include "NAVInfo.h"

#include "OSInfo.h"
#include "ResourceHelper.h"
#include "Resource.h"
#include "..\NAVComUIRes\ResResource.h"
#include <atlstr.h>
#include "ccSymCommonClientInfo.h"
#include "ccResourceLoader.h"
#include <map>

// Resource Loader
extern cc::CResourceLoader g_ResLoader;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COEHeurUIDlg::COEHeurUIDlg() :
	m_bQuarEnabled(true)
{ 
}

COEHeurUIDlg::~COEHeurUIDlg()
{
}

void COEHeurUIDlg::SetProcessName(const char* szProcessName)
{
	m_szProcessName = szProcessName;
}

void COEHeurUIDlg::SetEmailInfo(const char* szSubject,
								const char* szSender, 
								const char* szRecipient)
{
	m_strSender = szSender;
	m_strRecipient = szRecipient;
	m_strSubject = szSubject;
}

void COEHeurUIDlg::SetActionQuar(bool bEnabled)
{
	m_bQuarEnabled = bEnabled;
}

int COEHeurUIDlg::DoModal()
{
    // Set Alert Text.
	UINT uID;

	if (m_bQuarEnabled == true)
	{
		uID = IDS_OEH_ALERT_WORM_DETECTED;
	}
	else
	{
		uID = IDS_OEH_ALERT_QUAR_FAILED;
	}
    tstring strAlertTitle;
    CResourceHelper::LoadString ( uID, strAlertTitle, g_ResLoader.GetResourceInstance() );
    m_pUI->SetAlertTitle ( strAlertTitle.c_str());
    m_pUI->SetTitleBitmap ( _Module.m_hInstResource, IDB_OEH_ALERT );
    
	// Set the description text.

	if (m_bQuarEnabled == true)
	{
		uID = IDS_OEH_DESCRIPTION;
	}
	else
	{
		uID = IDS_OEH_QUARANTINE_FAILED;
	}
    tstring strBriefDesc;
    CResourceHelper::LoadString ( uID, strBriefDesc, g_ResLoader.GetResourceInstance() );
    m_pUI->SetBriefDesc (strBriefDesc.c_str());

    tstring strTemp;

    // Tables
    //
    // Process name
    CResourceHelper::LoadString ( IDS_PROCESS_NAME, strTemp, g_ResLoader.GetResourceInstance() );
    m_pUI->AddTableRowPath ( strTemp.c_str(), m_szProcessName.c_str() );

    CResourceHelper::LoadString ( IDS_SENDER, strTemp, g_ResLoader.GetResourceInstance() );
    m_pUI->AddTableRow ( strTemp.c_str(), m_strSender.c_str() );

    CResourceHelper::LoadString ( IDS_RECIPIENT, strTemp, g_ResLoader.GetResourceInstance() );
    m_pUI->AddTableRow ( strTemp.c_str(), m_strRecipient.c_str() );

    CResourceHelper::LoadString ( IDS_SUBJECT, strTemp, g_ResLoader.GetResourceInstance() );
    m_pUI->AddTableRow ( strTemp.c_str(), m_strSubject.c_str() );
    //
    // Tables end

    // Actions (what do you want to do?)
    //
    std::map <int, OEHACTION> mapActions;

    // (Quarantine, Allow, Block, and Authorize.)
	if (m_bQuarEnabled)
    {
        // allow quarantine
        //
        mapActions [m_pUI->AddAction ( _Module.m_hInstResource, IDS_QUARANTINE_THIS )] = OEHACTION_QUARANTINE;
        mapActions [m_pUI->AddAction ( _Module.m_hInstResource, IDS_BLOCK_THIS )] = OEHACTION_BLOCK;
    }
    else
        mapActions [m_pUI->AddAction ( _Module.m_hInstResource, IDS_BLOCK_THIS_RECOMMENDED )] = OEHACTION_BLOCK;

    mapActions [m_pUI->AddAction ( _Module.m_hInstResource, IDS_ALLOW_THIS )] = OEHACTION_ALLOW;
    mapActions [m_pUI->AddAction ( _Module.m_hInstResource, IDS_AUTHORIZE_THIS )] = OEHACTION_AUTHORIZE;

    m_pUI->SetRecAction (0);

	m_pUI->SetThreatLevel ( cc::IAlert::THREAT_HIGH ); 

    // Returns from DoModal, matches the action index in the dialog
    // with our internal list. 
    //
    int iResult = CBaseAlertUI::DoModal ();
    m_Action = mapActions[iResult];
    return iResult;
}

