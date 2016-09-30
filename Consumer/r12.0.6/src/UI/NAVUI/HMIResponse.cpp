// HMIResponse.cpp: implementation of the CHMIResponse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HMIResponse.h"
#include "StartBrowser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
bool CHMIResponseFactory::ShouldShow()
{
	return ShouldShowHelpMenuOption(HELPMENU_ShowResponse);
}

CHMIResponse::CHMIResponse()
{
    SetTextID ( IDS_RESPONSE_CENTER );
}

CHMIResponse::~CHMIResponse()
{

}

HRESULT CHMIResponse::DoWork (HWND hMainWnd)
{
	// Launch "product registration " link stored in AVRES.dll.
	TCHAR szURL[MAX_PATH] = {0};
	if(FetchURL(IDS_RESPONSE_URL, szURL))
	{
		NAVToolbox::CStartBrowser browser;
		browser.ShowURL(szURL);
		return S_OK;
	}

	return E_FAIL;	
}