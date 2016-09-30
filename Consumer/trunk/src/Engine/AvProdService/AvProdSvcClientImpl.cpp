////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "AvProdSvcClientImpl.h"

#include <ccSymStringImpl.h>
#include <ccSymKeyValueCollectionImpl.h>

#include <ccServiceLoader.h>
#include <ccInstanceFactory.h>

#include <AvProdSvcCommIds.h>

using namespace AvProdSvc;

CAvProdSvcClientImpl::CAvProdSvcClientImpl(void)
{
}

CAvProdSvcClientImpl::~CAvProdSvcClientImpl(void)
{
    Destroy();
}

HRESULT CAvProdSvcClientImpl::Initialize()
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        SYMRESULT sr;

        //
        // Open up a session channel
        CCTRACEI( CCTRCTX _T("Loading COMM library."));
        sr = ccService::ccServiceMgd_IComLib::CreateObject(GETMODULEMGR(), m_spCommLib);
        hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);

        CCTRACEI( CCTRCTX _T("Init'ing COMM channel: %s."), AvProdSvc::CommChannelGeneral);
        hrx << m_spCommLib->CreateClient(AvProdSvc::CommChannelGeneral, &m_spCommClient);
        
        CString cszClientCookie;
        cszClientCookie.Format(_T("0x%08X:0x%08X:%u"), ::GetCurrentProcessId(), ::GetCurrentThreadId(), ::GetTickCount());

        m_spClientCookie.Release();
        m_spClientCookie.Attach(ccSym::CStringImpl::CreateStringImpl());
        CCTHROW_BAD_ALLOC(m_spClientCookie);

        if(!m_spClientCookie->SetString(cszClientCookie))
            hrx << E_FAIL;
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        return E_FAIL;

    return S_OK;
}

void CAvProdSvcClientImpl::Destroy()
{
    if(m_spCommClient)
        m_spCommClient->Disconnect();

    m_spCommClient.Release();
    m_spCommLib.Release();
    return;
}

HRESULT CAvProdSvcClientImpl::DisableRebootDialog(cc::IString** ppstrClientCookie)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        cc::IKeyValueCollectionPtr spCommand;
        spCommand.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
        CCTHROW_BAD_ALLOC(spCommand);

        spCommand->SetValue((size_t)AvProdSvc::DisableRebootDialog::eClientCookie, m_spClientCookie);

        ISymBaseQIPtr spOutValue = spCommand;
        ISymBasePtr spInValue;
		if(m_spCommClient)
	        hrx << m_spCommClient->SendCommand(AvProdSvc::CMDID_DisableRebootDialog, spOutValue, &spInValue);

        if(ppstrClientCookie)
        {
            cc::IStringPtr spClientCookie;
            spClientCookie.Attach(ccSym::CStringImpl::CreateStringImpl());
            CCTHROW_BAD_ALLOC(spClientCookie);

            if(!spClientCookie->SetString(m_spClientCookie->GetStringW()))
                hrx << E_FAIL;

            SYMRESULT sr;
            sr = spClientCookie->QueryInterface(cc::IID_String, (void**)ppstrClientCookie);
            hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);
        }

    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        return E_FAIL;

    return S_OK;
}

HRESULT CAvProdSvcClientImpl::EnableRebootDialog(const cc::IString* pClientCookie, bool bProcessPendingRebootRequests, bool bForceRebootDialog)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        cc::IKeyValueCollectionPtr spCommand;
        spCommand.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
        CCTHROW_BAD_ALLOC(spCommand);

        if(pClientCookie)
            spCommand->SetValue((size_t)AvProdSvc::EnableRebootDialog::eClientCookie, pClientCookie);
        else
            spCommand->SetValue((size_t)AvProdSvc::EnableRebootDialog::eClientCookie, m_spClientCookie);
        
        spCommand->SetValue((size_t)AvProdSvc::EnableRebootDialog::eProcessPendingRebootRequests, bProcessPendingRebootRequests);
        spCommand->SetValue((size_t)AvProdSvc::EnableRebootDialog::eForceRebootDialog, bForceRebootDialog);

        ISymBaseQIPtr spOutValue = spCommand;
        ISymBasePtr spInValue;
		if(m_spCommClient)
	        hrx << m_spCommClient->SendCommand(AvProdSvc::CMDID_EnableRebootDialog, spOutValue, &spInValue);

    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        return E_FAIL;

    return S_OK;
}

HRESULT CAvProdSvcClientImpl::LogManualScanResults(REFGUID guidScanId) throw()
{
	StahlSoft::HRX hrx;
	ccLib::CExceptionInfo exceptionInfo;
	try
	{
		cc::IKeyValueCollectionPtr spCommand;
		spCommand.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
		CCTHROW_BAD_ALLOC(spCommand);

		spCommand->SetValue((size_t)AvProdSvc::LogManualScanResults::eManualScanInstanceId, guidScanId);

		ISymBaseQIPtr spOutValue = spCommand;
		ISymBasePtr spInValue;
		if(m_spCommClient)
			hrx << m_spCommClient->SendCommand(AvProdSvc::CMDID_LogManualScanResults, spOutValue, &spInValue);

	}
	CCCATCHMEM(exceptionInfo)
	CCCATCHCOM(exceptionInfo);

	if(exceptionInfo.IsException())
		return E_FAIL;

	return S_OK;
}