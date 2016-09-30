#include "StdAfx.h"

#include "LiveUpdateAction.h"
#include "FileAction.h"
#include "Utilities.h"
#include "IWPLUCallback_h.h"
#include "ProductRegCOMNames.h"

using namespace InstallToolBox;

class CIWPLUReg : public CLiveUpdateAction
{
public:
	CIWPLUReg(MSIHANDLE hInstall, LPCTSTR szActionName, LPCTSTR szCondition = NULL, LPCTSTR szUninstallCondition = NULL) :
		CLiveUpdateAction(hInstall, szActionName, szCondition, szUninstallCondition) {};
	virtual ~CIWPLUReg() {};

protected:
	BOOL CIWPLUReg::Setup()
	{
		return TRUE;
	}

	BOOL CIWPLUReg::Init(BOOL &bKillInstall)
	{
		bKillInstall = FALSE;

		// add the two commandlines
		m_liveUpdate.AddProduct("{E608DE78-7C7C-41af-9466-EBBD707FE3D2}", "AV IDS Defs 2006 MicroDefs25", "MicroDefsB.Old", "SymAllLanguages", "Norton AntiVirus Internet Worm Protection Signature Updates", TRUE, TRUE);
		m_liveUpdate.AddProduct("{7FD40184-A905-4f7d-8078-BE634AB384DA}", "AV IDS Defs 2006 MicroDefs25", "MicroDefsB.CurDefs", "SymAllLanguages", "Norton AntiVirus Internet Worm Protection Signature Updates", TRUE, TRUE);

		// add the product callback
		m_liveUpdate.AddCallback("{E608DE78-7C7C-41af-9466-EBBD707FE3D2}", CLSID_luIWPCallback, (LU_CALLBACK_TYPE)(PreSession | PostSession));

		// group the two commandlines
		m_liveUpdate.AddGroup("{303D4D8C-76A8-4f99-8191-7B382B182F7E}", "Coupled");
		m_liveUpdate.AddGroupMember("{303D4D8C-76A8-4f99-8191-7B382B182F7E}", "AV IDS Defs 2006 MicroDefs25", "MicroDefsB.Old", "SymAllLanguages");
		m_liveUpdate.AddGroupMember("{303D4D8C-76A8-4f99-8191-7B382B182F7E}", "AV IDS Defs 2006 MicroDefs25", "MicroDefsB.CurDefs", "SymAllLanguages");

		return TRUE;
	}
};

extern "C" __declspec(dllexport) UINT __stdcall IWPLiveUpdate(MSIHANDLE hInstall)
{
	if(SUCCEEDED(CoInitialize(NULL)))
	{
		CIWPLUReg actionLU(hInstall, "SSALiveUpdate");

		CCustomAction* actionArray[] = {&actionLU};
		int nActionCount = sizeof(actionArray) / sizeof(*actionArray);

		CCustomAction::BaseAction(hInstall, actionArray, nActionCount, "SSALiveUpdate");
	}

	return ERROR_SUCCESS;
}