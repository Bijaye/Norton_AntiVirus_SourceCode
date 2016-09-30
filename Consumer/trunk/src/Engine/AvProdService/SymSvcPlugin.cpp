////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SymSvcPlugin.h"

#include "SvcMgr.h"

using namespace AvProdSvc;

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CSymSvcPlugin::CSymSvcPlugin()
{
    // Intentionally empty.
}

CSymSvcPlugin::~CSymSvcPlugin(void)
{
    // Intentionally empty.
}

//****************************************************************************
//****************************************************************************
HRESULT CSymSvcPlugin::Load( ccService::IService* pCommand ) throw()
{
	CCTRACEI( CCTRCTX _T("Enter..."));

    HRESULT hrResult = S_OK;

    StahlSoft::HRX hrx;
    try
    {
        // Insure there is a AvSvcManager
        hrx << CSvcMgrSingleton::Initialize();

        // Initialize the AvAppManager
        CSvcMgrPtr spSvcMgr = CSvcMgrSingleton::GetSvcMgr();
        hrx << spSvcMgr->Initialize();
        
    }    
    catch(_com_error& e)
    {
        hrResult = e.Error();
        CCTRCTXE1(_T("Exception handled: hr=0x%08X"), hrResult);
    }


    return hrResult;
}

//****************************************************************************
//****************************************************************************
HRESULT CSymSvcPlugin::PrepareDestroy( DestroyCause eCause ) throw()
{
	CCTRACEI( CCTRCTX _T("Enter..."));

	// Destroy the AvAppManager
	CSvcMgrPtr spSvcMgr = CSvcMgrSingleton::GetSvcMgr();
	if(spSvcMgr)
		spSvcMgr->PrepareDestroy();

	CCTRCTXI0(L"Exit");
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CSymSvcPlugin::Destroy( ) throw()
{
	CCTRACEI( CCTRCTX _T("Enter..."));
	CSvcMgrPtr spSvcMgr = CSvcMgrSingleton::GetSvcMgr();
	if(spSvcMgr != NULL)
		spSvcMgr->Destroy();

	CSvcMgrSingleton::Destroy();
	CCTRCTXI0(L"Exit");
    return S_OK;
}
