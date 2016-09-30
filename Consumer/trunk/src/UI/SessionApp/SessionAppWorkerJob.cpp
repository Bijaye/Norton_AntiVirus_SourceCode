////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "SessionAppWorkerJob.h"

using namespace SessionApp;

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CWorkerJob::CWorkerJob()
{
	m_hRunning = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CWorkerJob::~CWorkerJob()
{
}

//****************************************************************************
//****************************************************************************
HRESULT CWorkerJob::IsRunning()
{
	HRESULT hrReturn = S_OK;

	DWORD dwRet = WaitForSingleObject(m_hRunning, 0);
	if(WAIT_TIMEOUT == dwRet)
	{
		hrReturn = S_OK;
	}
	else if(WAIT_OBJECT_0 == dwRet)
	{
		hrReturn = S_FALSE;
	}
	else
	{
		hrReturn = E_FAIL;
	}

	return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CWorkerJob::RequestExit()
{
	BOOL bFailed = SetEvent(m_hRunning);

	return bFailed ? E_FAIL : S_OK;
}


//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CProcessNotifyMessage::CProcessNotifyMessage(void) :
m_hWndParent(NULL)
{
}

//****************************************************************************
//****************************************************************************
CProcessNotifyMessage::~CProcessNotifyMessage(void)
{
}

//********************************************* *******************************
//****************************************************************************
HRESULT CProcessNotifyMessage::Run()
{
    HRESULT hrReturn = S_OK;
    CCTRACEI(_T("CProcessNotifyMessage::Run() BEGIN.\r\n"));

    //
    //  Protect ourselves from the evil that
    //   we are capable of.
    //
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        SYMRESULT sr;
        cc::INotify2Ptr spNotify;
        sr = cc::ccAlertMgd_INotify2::CreateObject(GETMODULEMGR(), &spNotify);
        hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);

        cc::INotifyCallbackPtr spNotifyCallback;
        CNotifySink_ViewRecentHistory::CreateObject(spNotifyCallback);

        DWORD dwDuration;
        ISymBasePtr spTemp;
        
        // Set callback to handle hyperlink clicks(see CNotifySink_ViewRecentHistory for IDs handled)
        spNotify->SetHyperlinkCallback(spNotifyCallback);

        // Get and Set Properties.
        if(m_spCommand->GetValue(SessionApp::NotifyMessage::eProperties, spTemp))
        {
            cc::IKeyValueCollectionQIPtr spProperties = spTemp;
            size_t iCount = spProperties->GetCount();
            for(size_t iIndex = 0; iIndex < iCount; iIndex++)
            {
                size_t iKey;
                if(spProperties->GetKeyAtIndex(iIndex, iKey))
                {
                    bool bVal = NULL;
                    if(spProperties->GetValue(iKey, bVal))
                    {
                        spNotify->SetProperty((cc::INotify::PROPERTIES)iKey, bVal);
                    }
                }
            }

            spTemp.Release();

        }

        // Get and Set Timeout
		if(m_spCommand->GetValue(SessionApp::NotifyMessage::eDuration, dwDuration))
            spNotify->SetTimeout(dwDuration);

        // Get/Set message height/width
        LONG lWidth = 0, lHeight = 0;
        if(m_spCommand->GetValue(SessionApp::NotifyMessage::eWidth, lWidth) && m_spCommand->GetValue(SessionApp::NotifyMessage::eHeight, lHeight))
            spNotify->SetSize(lWidth, lHeight);

        // Get and Set Title
		if(m_spCommand->GetValue(SessionApp::NotifyMessage::eTitle, spTemp))
        {
            cc::IStringQIPtr spString = spTemp; 
            spNotify->SetTitle(spString->GetStringW());
            spTemp.Release();
        }

        // Get and Set Text
		if(m_spCommand->GetValue(SessionApp::NotifyMessage::eText, spTemp))
        {
            cc::IStringQIPtr spString = spTemp; 
            spNotify->SetText(spString->GetStringW());
            spTemp.Release();
        }

        // Get and Set ToolTip
		if(m_spCommand->GetValue(SessionApp::NotifyMessage::eToolTip, spTemp))
        {
            cc::IStringQIPtr spString = spTemp; 
            spNotify->SetTooltip(spString->GetStringW());
            spTemp.Release();
        }
		
        // Get and Set More Info
        if(m_spCommand->GetValue(SessionApp::NotifyMessage::eMoreInfo, spTemp))
        {
            cc::IStringQIPtr spString = spTemp; 
            spTemp.Release();

            CString cszString = spString->GetStringW();
            spNotify->SetButton(cc::INotify::BUTTONTYPE_MOREINFO, spString->GetStringW(), spNotifyCallback);
        }
        

        spNotify->Display(m_hWndParent);
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    return hrReturn;
};

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CProcessCustomCommand::CProcessCustomCommand(void) :
m_dwStartTime(NULL)
{
}

//****************************************************************************
//****************************************************************************
CProcessCustomCommand::~CProcessCustomCommand(void)
{
}

//********************************************* *******************************
//****************************************************************************
HRESULT CProcessCustomCommand::Run()
{
    HRESULT hrReturn = S_OK;
    CCTRACEI(_T("CProcessCustomCommand::Run() BEGIN.\r\n"));

    //
    //  Protect ourselves from the evil that
    //   we are capable of.
    //
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        
        const DWORD dwTimeout = 1000 * 60 * 6; // 5 minutes

        // Make sure our timeout hasn't expired
        if(!( (GetTickCount() - m_dwStartTime) > dwTimeout))
        {
            ISymBasePtr spReturnCommand;
            m_spSessionAppServerSink->OnCommand(m_spAppServer, m_guidActualCommand, m_spCommand, &spReturnCommand);

            // Check the timeout period again
            if(!( (GetTickCount() - m_dwStartTime) > dwTimeout))
            {
                ISymBasePtr spOutValue;
                m_spCommServer->SendCommand(m_guidCommand, m_cwszCallbackChannel, spReturnCommand, &spOutValue);
            }
        }

        if((GetTickCount() - m_dwStartTime) > dwTimeout)
        {
            CCTRACEW( _T("Timeout expired."));
        }
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    return hrReturn;
};