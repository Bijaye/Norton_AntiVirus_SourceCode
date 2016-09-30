////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "LiveUpdateAction.h"
#include "FileAction.h"
#include "Utilities.h"
#include "ProductRegCOMNames.h"
#include "resource.h"
#include "NAVLuStrings.h"

using namespace InstallToolBox;
using namespace NAVLUStrings;

//This is the common LU Callback provided by Component Framework team
//{01745A4F-9372-4C50-943C-A43E9CB78505}
const CLSID CLSID_GenericLUCallback = { 0x01745A4F, 0x9372, 0x4C50,{ 0x94,0x3C,0xA4,0x3E,0x9C,0xB7,0x85,0x05}};

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
		
        CString strDefsDesc;

        if(strDefsDesc.LoadString(IDS_IWPINST_LU_DESC) == 0)
            CCTRACEE(_T("CIWPLUReg::Init - Could not load desc from string resource."));

		// add the two commandlines
		m_liveUpdate.AddProduct(MONIKER_IDS_HUB, PRODUCT_IDS_HUB, VERSION_IDS_HUB_OLD, LANGSTR_ALLLANGUAGES, strDefsDesc, TRUE, TRUE);
		m_liveUpdate.AddProduct(MONIKER_IDS_MICRODEF, PRODUCT_IDS_MICRODEF, VERSION_IDS_MICRODEFS, LANGSTR_ALLLANGUAGES, strDefsDesc, TRUE, TRUE);

		//add properties
		m_liveUpdate.AddProperty(MONIKER_IDS_HUB,LUPR_UI_PRIV,_T("2"));
		m_liveUpdate.AddProperty(MONIKER_IDS_MICRODEF,LUPR_UI_PRIV,_T("2"));
		
		// add the product callback
		m_liveUpdate.AddCallback(MONIKER_IDS_HUB, CLSID_GenericLUCallback, (LU_CALLBACK_TYPE)(PreSession | PostSession));

		// group the two commandlines
		m_liveUpdate.AddGroup(MONIKER_IDS_GROUP, LUPR_GT_COUPLED);
		m_liveUpdate.AddGroupMember(MONIKER_IDS_GROUP, PRODUCT_IDS_HUB, VERSION_IDS_HUB_OLD, LANGSTR_ALLLANGUAGES);
		m_liveUpdate.AddGroupMember(MONIKER_IDS_GROUP, PRODUCT_IDS_MICRODEF, VERSION_IDS_MICRODEFS, LANGSTR_ALLLANGUAGES);

		return TRUE;
	}
};

extern "C" __declspec(dllexport) UINT __stdcall IWPLiveUpdate(MSIHANDLE hInstall)
{
	if(SUCCEEDED(CoInitialize(NULL)))
	{
		CIWPLUReg actionLU(hInstall, _T("SSALiveUpdate"));

		CCustomAction* actionArray[] = {&actionLU};
		int nActionCount = sizeof(actionArray) / sizeof(*actionArray);

		CCustomAction::BaseAction(hInstall, actionArray, nActionCount, _T("SSALiveUpdate"));
	}

	return ERROR_SUCCESS;
}