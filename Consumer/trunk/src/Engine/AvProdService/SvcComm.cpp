////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include <AvProdSvcCommIds.h>
#include "SvcMgr.h"
#include "SvcComm.h"

using namespace AvProdSvc;

CSvcComm::CSvcComm(void)
{
}

CSvcComm::~CSvcComm(void)
{
}

HRESULT CSvcComm::Command( const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
                 ISymBase* pBase, ISymBase** ppOutBase )
{
    CSvcMgrPtr spSvcMgr = CSvcMgrSingleton::GetSvcMgr();
    if(!spSvcMgr)
        return S_OK;

    if(SymIsEqualIID(gCommandGuid, AvProdSvc::CMDID_DisableRebootDialog))
    {
        spSvcMgr->OnDisableRebootDialog(gCommandGuid, szCallbackChannel, pBase, ppOutBase);
    }
    else if(SymIsEqualIID(gCommandGuid, AvProdSvc::CMDID_EnableRebootDialog))
    {
        spSvcMgr->OnEnableRebootDialog(gCommandGuid, szCallbackChannel, pBase, ppOutBase);
    }
	else if(SymIsEqualIID(gCommandGuid, AvProdSvc::CMDID_LogManualScanResults))
	{
		spSvcMgr->OnLogManualScanResults(gCommandGuid, szCallbackChannel, pBase, ppOutBase);
	}
    else
    {
        CCTRCTXW0(_T("Unknown CommandId received."));
    }

    return S_OK;
}
