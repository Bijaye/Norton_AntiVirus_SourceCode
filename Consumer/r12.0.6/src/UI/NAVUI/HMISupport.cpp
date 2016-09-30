// HMISupport.cpp: implementation of the CHMISupport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HMISupport.h"
#include "Simon.h"
#include "StartBrowser.h"
#include "cltLicenseHelper.h"
#include "XLOK_UiProperties.h"
#include "NAVLicenseNames.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHMISupport::CHMISupport()
{
	SetTextID ( IDS_NAV_SUPPORT );
}

CHMISupport::~CHMISupport()
{
	
}
HRESULT CHMISupport::DoWork (HWND hMainWnd)
{
	// Load AVRES to get Support URL.
	TCHAR szURL[MAX_PATH] = {0};
	if(FetchSupportURL(IDS_TECHSUPP_WEBSITE, szURL))
	{
		NAVToolbox::CStartBrowser browser;
		if(browser.ShowURL(szURL))
			return S_OK;
	}

	return E_FAIL;
}

bool CHMISupportFactory::ShouldShow()
{
	return ShouldShowHelpMenuOption(HELPMENU_ShowSupport);
}

