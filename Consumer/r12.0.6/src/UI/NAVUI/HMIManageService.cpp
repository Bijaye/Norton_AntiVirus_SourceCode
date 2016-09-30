// HMIManageService.cpp: implementation of the HMIManageService class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HMIManageService.h"
#include "AVRES.h"
#include "ScriptSafeVerifyTrustCached.h"		// For scripting security
#include "NavLicense_i.c"
#include "NavLicense_h.h"
#include "StartBrowser.h"

/////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHMIManageService::CHMIManageService()
{
	SetTextID ( IDS_NAV_MANAGE_MYSERVICE_MENU_ITEM );
}

CHMIManageService::~CHMIManageService()
{

}

HRESULT CHMIManageService::DoWork (HWND hMainWnd)
{
	// Launch "Manage My Serivce" link stored in AVRES.
	TCHAR szURL[MAX_PATH] = {0};
	if(FetchURL(IDS_MANAGE_MYSERVICE_URL, szURL))
	{
		NAVToolbox::CStartBrowser browser;
		if(browser.ShowURL(szURL))
			return S_OK;
	}

	return E_FAIL;	
}

bool CHMIManageServiceFactory::ShouldShow()
{
	return ShouldShowHelpMenuOption(HELPMENU_ShowManageService);
}