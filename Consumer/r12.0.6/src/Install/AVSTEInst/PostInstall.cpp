//
// Post Install custom actoins for AVSTEInst
//
#include "StdAfx.h"
#include "InstallToolBox.h"
#include "BinaryFileExtracter.h"

#include "SSCoInitialize.h"

using namespace InstallToolBox;


extern "C" __declspec(dllexport) UINT __stdcall NAV_CTO_Action(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "attach to me", "NAV_CTO_Action", MB_OK);
#endif // _DEBUG

	StahlSoft::CSmartCoInitialize CoInit;
	HRESULT hr = CoInit.CoInitialize();

	if(SUCCEEDED(hr))
	{
		InstallToolBox::CInstoptsDat instopts(hInstall);
		DWORD dwCTO = 0;

		// OEM not found so its definitely not CTO
		if(!instopts.ReadDWORD(_T("OEM"), dwCTO))
		{
			CCTRACEI("Exiting NAV_CTO_Action, since we are not even OEM");
			return ERROR_SUCCESS;
		}

		// if not CTO then exit
		if(2 != dwCTO)
		{
			CCTRACEI("Existing NAV_CTO_Action, since we are not CTO");
			return ERROR_SUCCESS;
		}

		InstallToolBox::CBinaryFileExtracter actionCTO(hInstall, _T("CTO_Action"), _T("INSTALLDIR"), _T("AVSTE.DLL"), _T("AVCTO"));

		InstallToolBox::CCustomAction* actionArray[] = {&actionCTO};
		int nActionCount = sizeof(actionArray) / sizeof(*actionArray);

		return InstallToolBox::CCustomAction::BaseAction(hInstall, actionArray, nActionCount, _T("NAV_CTO_Action"));
	}

    CCTRACEE("NAV_CTO_Action: CoInitialize failed");
	InstallToolBox::CCustomAction::LogMessage(hInstall, InstallToolBox::CCustomAction::logError, _T("NAV_CTO_Action: Failed to initialize COM"));
	return ERROR_INSTALL_FAILURE;
}