// IEContextMenuHelper.cpp : Implementation of CIEContextMenuHelper

// IEToolBandHelper.cpp : Implementation of CIEToolBandHelper
#include "StdAfx.h"
#include "IEContextMenuHelper.h"
#include "InstOptsNames.h"
#include "NavUIHelp.h"
#include "NAVUIHTM_Resource.h"
#include "TraceHR.h"
// PEP Headers
#define _V2AUTHORIZATION_SIMON
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "ComponentPepHelper.h"
#include "PepMiddleMan.h"
#include "SuiteOwnerHelper.h"
#include "NAVOptHelperEx.h"
//#include "PathValidation.h"

// Returns formatted error string & HRESULT mapping with this error string
HRESULT	CHelper::ErrorFromResource(int iId, const IID& guid, CString& csError)
{
	TRACEHR (hr);
	GetResourceString(iId,csError);
	hr = _Error(csError,guid);
	CCTRACEI(_T("ErrorFromResource(%d): returning hr(0x%08X): %s"),iId,hr,csError);
	return hr;
}
void	CHelper::GetResourceString(UINT uiIDs,CString &csResource)
{
	CString csFormat;
	csFormat.LoadString(_AtlBaseModule.GetResourceInstance(), uiIDs);
	if( -1 != csFormat.Find(_T("%s")))
	{
		csResource.Format(csFormat, m_csProductName);
	}
	else
	{
		csResource = csFormat;
	}
}

bool CHelper::IsCfgwizFinished()
{
	CCTRACEI("CHelper::IsCfgwizFinished() Enter");

	bool bFinished = false;	
	CString csCfgWizDat;

	if(SuiteOwnerHelper::GetOwnerCfgWizDat(csCfgWizDat))
	{
		CNAVOptFileEx CfgWizOpts;
		if(CfgWizOpts.Init(csCfgWizDat, FALSE))
		{
			DWORD dwValue = 0;
			CfgWizOpts.GetValue(InstallToolBox::CFGWIZ_Finished, dwValue, 0);

			if(dwValue == 1)
			{
				bFinished = true;
			}
		}
	}
	CCTRACEI("CHelper::IsCfgwizFinished() Exit(%s)", bFinished ? "TRUE" : "FALSE");

	return bFinished;
}

void CHelper::LaunchCfgwiz()
{	
	CCTRACEI("CHelper::LaunchCfgwiz() Enter()");

	CString csApp, csParam;
	if(SuiteOwnerHelper::GetOwnerCfgWizCmdLine(csApp, csParam))
	{
		CString csCmdLine;
		csCmdLine.Format(_T("\"%s\" %s"), csApp, csParam);

		// Launch ConfigWiz
		STARTUPINFO si = {0};
		si.cb = sizeof( STARTUPINFO );
		PROCESS_INFORMATION pi = {0};

		CreateProcess(NULL, csCmdLine.GetBuffer(), NULL, NULL, FALSE, DETACHED_PROCESS, NULL, NULL, &si, &pi);

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);

		CCTRACEI("CHelper::LaunchCfgwiz() CreateProcess(%s)",csCmdLine);
	}

	CCTRACEI("CHelper::LaunchCfgwiz() Exit");
}


// Returns S_OK if Config Wizard is completed & reboot is not needed
HRESULT	CHelper::IsConfigured()
{
	TRACEHR (hrx);
	hrx = S_OK;
	if(!IsCfgwizFinished())	{
		CString csError;

		// Return if configwiz hasn't finished yet
		LaunchCfgwiz();
		hrx = S_FALSE;
	}
	return hrx;
}
