#include "StdAfx.h"

#include "LiveUpdateAction.h"
#include "FileAction.h"
#include "Utilities.h"
#include "ProductRegCOMNames.h"

using namespace InstallToolBox;

class AVSTELUReg : public CLiveUpdateAction
{
public:
	AVSTELUReg(MSIHANDLE hInstall, LPCTSTR szActionName, LPCTSTR szCondition = NULL, LPCTSTR szUninstallCondition = NULL) :
	  CLiveUpdateAction(hInstall, szActionName, szCondition, szUninstallCondition) {};
	virtual ~AVSTELUReg() {};

protected:

	BOOL AVSTELUReg::Setup()
	{
		// this function gets called during Immediate execution, so we can get the property we setted
		// earlier, and use SetProperty to put in the property so that we will have access to
		// it during the deferred execution by calling GetProperty
		TCHAR szAVSTEType[MAX_PATH] = {0};
		DWORD dwSize = MAX_PATH;
		if(ERROR_SUCCESS == MsiGetProperty(m_hInstall, _T("AVSTETYPE"), szAVSTEType, &dwSize))
			SetProperty(_T("AVSTETYPE"), szAVSTEType);

		return TRUE;
	}

	BOOL AVSTELUReg::Init(BOOL &bKillInstall)
	{
		// this function gets called during deferred action only
		bKillInstall = FALSE;

		TCHAR szAVSTEType[MAX_PATH] = {0};
		DWORD dwSize = MAX_PATH;

		// add the commandline by looking at the property we setted in Setup()
		if(GetProperty(_T("AVSTETYPE"), szAVSTEType, dwSize))
			m_liveUpdate.AddProduct("{A15DF0C0-B5C9-4e25-B454-F3F61B4C88FA}", szAVSTEType, "NAV 2006", "SymAllLanguages", "Norton AntiVirus Suite Update");

		return TRUE;
	}
};

extern "C" __declspec(dllexport) UINT __stdcall AVSTELiveUpdate(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug AVSTELiveUpdate()", "Debug Break", NULL);
#endif

	if(SUCCEEDED(CoInitialize(NULL)))
	{
		AVSTELUReg actionLU(hInstall, "AVSTELiveUpdate");

		CCustomAction* actionArray[] = {&actionLU};
		int nActionCount = sizeof(actionArray) / sizeof(*actionArray);

		CCustomAction::BaseAction(hInstall, actionArray, nActionCount, "AVSTELiveUpdate");
	}

	return ERROR_SUCCESS;
}

extern "C" __declspec(dllexport) UINT _stdcall AVSTEType(MSIHANDLE hInstall)
{
	try
	{
		//read in the string and set it in the MSI
		TCHAR szAVSTEType[MAX_PATH] = {0};
		DWORD dwSize = MAX_PATH;
		CInstoptsDat Instopts;
        Instopts.ReadString(_T("INSTOPTS:AVSTETYPE"), szAVSTEType, dwSize);
		MsiSetProperty(hInstall, _T("AVSTETYPE") ,szAVSTEType);
	}
	catch(...)
	{
		//catch all exceptions here
	}

	return ERROR_SUCCESS;
}