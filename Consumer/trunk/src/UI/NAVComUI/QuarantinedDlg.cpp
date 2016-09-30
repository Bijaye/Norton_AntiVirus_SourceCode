////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// QuarantinedDlg.cpp: implementation of the CQuarantinedDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QuarantinedDlg.h"
#include "Resource.h"
#include "..\NAVComUIRes\ResResource.h"
#include "ResourceHelper.h"
#include "ccResourceLoader.h"

// Resource Loader
extern cc::CResourceLoader g_ResLoader;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuarantinedDlg::CQuarantinedDlg()
{ 
}

CQuarantinedDlg::~CQuarantinedDlg()
{
}

int CQuarantinedDlg::DoModal()
{
    // Set Alert Text.
    m_pUI->SetAlertTitle ( _Module.m_hInstResource, IDS_QUARANTINED_SUCCESSFUL );
    m_pUI->SetTitleBitmap ( _Module.m_hInstResource, IDB_OEH_QUAR_SUCCESS );

	// Set the description, additional description, quarantine description 
	// and Ok button description text.

	CString csBriefDescFormat;
	CString csBriefDesc;
	CString csBuffer;

	csBriefDescFormat.LoadString(g_ResLoader.GetResourceInstance(), IDS_QUARANTINED_DESC);
	csBriefDesc.Format(csBriefDescFormat, m_strProductName);
	csBriefDesc += _T("\r\n");
	csBuffer.LoadString(g_ResLoader.GetResourceInstance(), IDS_QUARANTINED_DESC_ADDITIONAL);
	csBriefDesc += csBuffer;
	m_pUI->SetBriefDesc (csBriefDesc);

    // Actions (what do you want to do?)
    //
    std::map <int, OEHACTION> mapActions;

    mapActions [m_pUI->AddAction ( _Module.m_hInstResource, IDS_QUAR_DESCRIPTION )] = OEHACTION_LAUNCH_QUARANTINE;
    mapActions [m_pUI->AddAction ( _Module.m_hInstResource, IDS_OK_DESCRIPTION )] = OEHACTION_NONE;
    m_pUI->SetRecAction (0);
    m_pUI->SetThreatLevel (cc::IAlert::THREAT_NONE);

    int iResult = CBaseAlertUI::DoModal ();
    m_Action = mapActions[iResult];
    return iResult;
}
