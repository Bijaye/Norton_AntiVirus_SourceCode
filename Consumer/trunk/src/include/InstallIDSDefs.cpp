////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "isIDSStrings.h"
#include "SymIDSI.h"
#include "IDSLoader.h"
#include "IDSLoaderLink.h"
#include "InstallToolBox.h"
#include "NAVInfo.h"
#include "NAVLUStrings.h"

bool InstallIDSDefs()
{
	using namespace SymIDS;

	CCTRCTXI0 (_T("InstallIDSDefs() - Start"));
	CIDSLoader oIDSLoader;
	SYMRESULT dwResult;
	SIGNATURE_PROFILE cSigProfile = SIGNATURE_PROFILE_DEFAULT;

	InstallToolBox::ITB_VERSION verProduct;
	InstallToolBox::GetNAVVersion(&verProduct);
	UINT64 ui64Version = MAKE_PRODUCT_VERSION(verProduct.nMajorHi,verProduct.nMajorLo,verProduct.nMinorHi,verProduct.nMinorLo);

	CNAVInfo NAVInfo;
	CString szDefSourcePath = NAVInfo.GetNAVDir();
	PathAppend(szDefSourcePath.GetBuffer(MAX_PATH), _T("\\IDSDefs"));
	szDefSourcePath.ReleaseBuffer();

	//
	//Scope CSymPtr
	{
		CSymPtr<IIDSSubscription> pIDSSubscription;
		dwResult = oIDSLoader.GetObject(IID_IDSSubscription, (ISymBase **)&pIDSSubscription);													
		if(SYM_SUCCEEDED(dwResult))
		{
			CCTRCTXI0 (_T("InstallDefs::Loaded IIDSSubscription object"));
			dwResult = pIDSSubscription->Initialize ();
			if(SYM_SUCCEEDED(dwResult))
			{
				CCTRCTXI0 (_T("InstallDefs::Initialize() Successful"));
				//CCTRACEI("Subscribe(%s , %s, %s, %s, %s,%s)",szIDSAppName,cSigProfile,szIDSDir,szIDSMoniker,szIDSMicroMoniker,szIDSGroupMoniker);

				dwResult = pIDSSubscription->Subscribe (ATL::CT2CAEX<>(ISIDSStrings::IDS_PRODUCTID_NAV),
					cSigProfile,
					ui64Version,
					ATL::CT2CAEX<>(ISIDSStrings::IDS_PRODUCTDIR_NAV),
					ATL::CT2CAEX<>(NAVLUStrings::MONIKER_IDS_HUB),
					ATL::CT2CAEX<>(NAVLUStrings::MONIKER_IDS_MICRODEF),
					ATL::CT2CAEX<>(NAVLUStrings::MONIKER_IDS_GROUP));

				if(SYM_SUCCEEDED( dwResult ))
				{
					CCTRCTXI0 (_T("Subscribe() to IDS Successful"));
				}
				else
				{
					CCTRCTXE1 (_T("Subscribe() to IDS Un Successful. ERROR: 0x%x"),dwResult);
					return false;
				}
			}
			else
			{
				CCTRCTXE1 (_T("InstallDefs::pIDSSubscription->Initialize () Failed ERROR: 0x%x"),dwResult );
				return false;
			}

		}
		else
		{
			CCTRCTXE1 (_T("InstallDefs::GetObject(IID_IDSSubscription..) Failed ERROR: 0x%x"),dwResult);
			return false;
		}
	}


	//scope IIDSDefsManager
	{
		CSymPtr <IIDSDefsManager> poIDSDefsManager;
		dwResult = oIDSLoader.GetObject (IID_IDSDefsManager, (ISymBase **) &poIDSDefsManager);

		if (SYM_FAILED (dwResult))
		{
			CCTRACEE(_T("InstallDefs::Could not get object IID_IDSDefsManager. ERROR: 0x%x"),dwResult);
			return false;
		}

		dwResult = poIDSDefsManager->Initialize ();

		if (SYM_FAILED (dwResult))
		{
			CCTRACEE(_T("InstallDefs::poIDSDefsManager->Initialize () Failed. ERROR: 0x%x"),dwResult);
			return false;
		}

		for(int i=0; i < 10; i++)
		{
			dwResult = poIDSDefsManager->InstallDefs (ATL::CT2CAEX<>(ISIDSStrings::IDS_PRODUCTID_NAV), ATL::CT2CAEX<>(szDefSourcePath));

			//On Vista IDS Defs are imported using ccService plugin. Retry if this error was because of the plugin
			//not ready.
			if((E_FAIL == dwResult) || (RPC_E_DISCONNECTED == dwResult))
			{
				CCTRACEE(_T("Attempt #%d - InstallDefs() to IDS Un Successful. ERROR: 0x%x. Sleeping 500 ms"),i,dwResult);
				Sleep(500);
			}
			else
			{
				break;
			}
		}
	}

	if (SYM_FAILED (dwResult))
	{	
		CCTRCTXE3 (_T("InstallDefs::InstallDefs (%s, %s) Failed. ERROR: 0x%x "),ISIDSStrings::IDS_PRODUCTID_NAV,szDefSourcePath,dwResult);
		return false;
	}
	else
		CCTRCTXI2 (_T("InstallDefs::InstallDefs (%s, %s) Succeeded"),ISIDSStrings::IDS_PRODUCTID_NAV,szDefSourcePath);

	CCTRCTXI0 (_T("InstallIDSDefs() - Finish"));
	return true;
}
