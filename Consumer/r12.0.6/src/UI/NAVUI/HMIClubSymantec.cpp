// HMIClubSymantec.cpp: implementation of the CHMIClubSymantec class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HMIClubSymantec.h"
#include "StartBrowser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
bool CHMIClubSymantecFactory::ShouldShow()
{
	return ShouldShowHelpMenuOption(HELPMENU_ShowClubSymantec);
}

CHMIClubSymantec::CHMIClubSymantec()
{
    SetTextID ( IDS_CLUB_SYMANTEC );
}

CHMIClubSymantec::~CHMIClubSymantec()
{

}

HRESULT CHMIClubSymantec::DoWork (HWND hMainWnd)
{
	// Launch "club symantec" link stored in AVRES.dll.
	TCHAR szURL[MAX_PATH] = {0};
	if(FetchURL(IDS_CLUBSYMANTEC_URL, szURL))
	{
		NAVToolbox::CStartBrowser browser;
		browser.ShowURL(szURL);
		return S_OK;
	}

	return E_FAIL;	
}